/* -- macosx_sck.c -- */

#include "macosx_sck.h"

#if (defined(__MACH__) && defined(__APPLE__))

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <Availability.h>

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 140000

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreMedia/CoreMedia.h>
#include <CoreVideo/CoreVideo.h>
#include <Foundation/Foundation.h>
#include <dispatch/dispatch.h>
#include <ScreenCaptureKit/ScreenCaptureKit.h>

extern CGDirectDisplayID displayID;
extern int show_cursor;
extern int wireframe;

static pthread_mutex_t sck_lock = PTHREAD_MUTEX_INITIALIZER;
static int sck_saved_show_cursor = -1;
static int sck_saved_wireframe = -1;
static SCStreamConfiguration *sck_config = nil;
static SCDisplay *sck_display = nil;
static NSArray *sck_windows = nil;
static char *sck_fb = NULL;
static char *sck_tmp_fb = NULL;
static size_t sck_fb_len = 0;
static int sck_w = 0;
static int sck_h = 0;
static int sck_bpl = 0;
static int sck_bpp = 32;
static int sck_valid = 0;
static int sck_active = 0;
static int sck_snap_in_flight = 0;
static int sck_content_in_flight = 0;
static NSTimeInterval sck_content_interval = 1.0;
static NSTimeInterval sck_snap_interval = 0.05;
static NSTimeInterval sck_content_time = 0;
static NSTimeInterval sck_last_snap_time = 0;
static NSTimeInterval sck_log_time = 0;
static unsigned long sck_snaps = 0;
static unsigned long sck_frames = 0;
static unsigned long sck_errors = 0;
static unsigned long sck_copies = 0;

static const char *sck_error_text(NSError *error)
{
	const char *msg;
	if (error == nil) {
		return "no error";
	}
	msg = error.localizedDescription.UTF8String;
	return msg == NULL ? "unknown error" : msg;
}

static NSTimeInterval sck_interval(const char *name, NSTimeInterval def,
    double min, double max)
{
	const char *val = getenv(name);
	double v;

	if (val == NULL || *val == '\0') {
		return def;
	}
	v = atof(val);
	if (v <= 0.0) {
		return def;
	}
	if (v < min) {
		v = min;
	}
	if (v > max) {
		v = max;
	}
	return (NSTimeInterval) v;
}

static void sck_log_stats(int force)
{
	if (! force && sck_log_time != 0 &&
	    [NSDate timeIntervalSinceReferenceDate] - sck_log_time < 2.0) {
		return;
	}
	sck_log_time = [NSDate timeIntervalSinceReferenceDate];
	fprintf(stderr, "macosx_sck: snaps=%lu frames=%lu errors=%lu copies=%lu\n",
	    sck_snaps, sck_frames, sck_errors, sck_copies);
}

static int sck_frame_has_content(const char *base, size_t w, size_t h, size_t bpr)
{
	size_t y, x;
	const unsigned char *p;
	if (base == NULL) {
		return 0;
	}
	for (y = 0; y < h; y += 16) {
		for (x = 0; x < w; x += 32) {
			p = (const unsigned char *) (base + y * bpr + x * 4);
			if (p[0] || p[1] || p[2] || p[3]) {
				return 1;
			}
		}
	}
	return 0;
}

static int sck_wait(dispatch_semaphore_t sem, NSTimeInterval seconds)
{
	NSDate *deadline = [NSDate dateWithTimeIntervalSinceNow:seconds];
	while (dispatch_semaphore_wait(sem,
	    dispatch_time(DISPATCH_TIME_NOW, 100 * NSEC_PER_MSEC)) != 0) {
		if ([deadline timeIntervalSinceNow] <= 0) {
			return 0;
		}
		@autoreleasepool {
			[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode
			    beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
		}
	}
	return 1;
}

static void sck_release_semaphore(dispatch_semaphore_t sem, int completed)
{
	if (completed) {
		dispatch_release(sem);
	}
}

static int sck_refresh_content(void)
{
	if (@available(macOS 14.0, *)) {
		dispatch_semaphore_t sem;
		__block SCShareableContent *content = nil;
		__block NSError *error = nil;
		__block SCDisplay *target = nil;
		int completed;

		pthread_mutex_lock(&sck_lock);
		if (sck_content_in_flight) {
			pthread_mutex_unlock(&sck_lock);
			return 0;
		}
		sck_content_in_flight = 1;
		pthread_mutex_unlock(&sck_lock);

		sem = dispatch_semaphore_create(0);
		[SCShareableContent getShareableContentExcludingDesktopWindows:NO
		    onScreenWindowsOnly:YES
		    completionHandler:^(SCShareableContent *c, NSError *err) {
			content = c == nil ? nil : [c retain];
			error = [err retain];
			if (c != nil) {
				for (SCDisplay *d in c.displays) {
					if (d.displayID == displayID) {
						target = [d retain];
						break;
					}
				}
			}
			dispatch_semaphore_signal(sem);
		}];
		completed = sck_wait(sem, 1.0);
		sck_release_semaphore(sem, completed);

		if (! completed || error != nil || content == nil || target == nil) {
			fprintf(stderr, "macosx_sck: content refresh failed: %s\n",
			    sck_error_text(error));
			[content release];
			[error release];
			[target release];
			pthread_mutex_lock(&sck_lock);
			sck_content_in_flight = 0;
			pthread_mutex_unlock(&sck_lock);
			return 0;
		}

		NSArray *windows = [content.windows retain];
		pthread_mutex_lock(&sck_lock);
		[sck_windows release];
		sck_windows = [windows retain];
		[sck_display release];
		sck_display = [target retain];
		sck_content_time = [NSDate timeIntervalSinceReferenceDate];
		pthread_mutex_unlock(&sck_lock);

		fprintf(stderr, "macosx_sck: refreshed %lu windows\n",
		    (unsigned long) windows.count);
		[windows release];
		[content release];
		[error release];
		[target release];
		pthread_mutex_lock(&sck_lock);
		sck_content_in_flight = 0;
		pthread_mutex_unlock(&sck_lock);
		return 1;
	}
	return 0;
}

static SCContentFilter *sck_build_filter(void)
{
	if (@available(macOS 14.0, *)) {
		SCDisplay *target = nil;
		NSArray *windows = nil;
		SCContentFilter *filter = nil;

		pthread_mutex_lock(&sck_lock);
		target = [sck_display retain];
		windows = [sck_windows retain];
		pthread_mutex_unlock(&sck_lock);
		if (target == nil) {
			return nil;
		}

		if (windows != nil && windows.count > 0) {
			@try {
				filter = [[SCContentFilter alloc] initWithDisplay:target
				    includingWindows:windows];
			} @catch (NSException *win_exception) {
				fprintf(stderr, "macosx_sck: including windows failed: %s\n",
				    [[win_exception description] UTF8String]);
				[filter release];
				filter = nil;
			}
		}
		if (filter == nil) {
			@try {
				filter = [[SCContentFilter alloc] initWithDisplay:target
				    excludingWindows:@[]];
			} @catch (NSException *display_exception) {
				fprintf(stderr, "macosx_sck: display filter failed: %s\n",
				    [[display_exception description] UTF8String]);
			}
		}
		if (filter != nil) {
			if (@available(macOS 14.2, *)) {
				filter.includeMenuBar = YES;
			}
		}
		[windows release];
		[target release];
		return filter;
	}
	return nil;
}

static int sck_store_image(CGImageRef image)
{
	size_t iw, ih;
	CGColorSpaceRef colorspace;
	CGContextRef context;
	int blank;

	if (image == NULL || sck_tmp_fb == NULL || sck_fb == NULL) {
		return 0;
	}
	iw = CGImageGetWidth(image);
	ih = CGImageGetHeight(image);
	if (iw != (size_t) sck_w || ih != (size_t) sck_h) {
		fprintf(stderr, "macosx_sck: image size %lux%lu, expected %dx%d\n",
		    (unsigned long) iw, (unsigned long) ih, sck_w, sck_h);
		return 0;
	}

	colorspace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
	if (colorspace == NULL) {
		return 0;
	}
	context = CGBitmapContextCreate(sck_tmp_fb, iw, ih, 8, (size_t) sck_bpl,
	    colorspace, kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipFirst);
	CGColorSpaceRelease(colorspace);
	if (context == NULL) {
		return 0;
	}
	CGContextDrawImage(context, CGRectMake(0, 0, iw, ih), image);
	CGContextRelease(context);

	blank = ! sck_frame_has_content(sck_tmp_fb, iw, ih, (size_t) sck_bpl);
	pthread_mutex_lock(&sck_lock);
	if (blank && sck_valid) {
		sck_errors++;
		sck_log_stats(sck_errors <= 5);
		sck_content_time = 0;
		pthread_mutex_unlock(&sck_lock);
		return 0;
	}
	memcpy(sck_fb, sck_tmp_fb, sck_fb_len);
	sck_valid = 1;
	sck_frames++;
	pthread_mutex_unlock(&sck_lock);
	return 1;
}

void macosx_sck_snap(void)
{
	if (@available(macOS 14.0, *)) {
		NSTimeInterval now;
		SCStreamConfiguration *config = nil;
		SCContentFilter *filter = nil;
		dispatch_semaphore_t sem;
		__block CGImageRef image = nil;
		__block NSError *error = nil;
		int completed;

		now = [NSDate timeIntervalSinceReferenceDate];
		if (now - sck_last_snap_time < sck_snap_interval) {
			return;
		}
		sck_last_snap_time = now;
		if (now - sck_content_time > sck_content_interval) {
			sck_refresh_content();
		}

		pthread_mutex_lock(&sck_lock);
		if (! sck_active || sck_snap_in_flight || sck_config == nil) {
			pthread_mutex_unlock(&sck_lock);
			return;
		}
		sck_snap_in_flight = 1;
		config = [sck_config retain];
		pthread_mutex_unlock(&sck_lock);

		filter = sck_build_filter();
		if (filter == nil) {
			[config release];
			pthread_mutex_lock(&sck_lock);
			sck_snap_in_flight = 0;
			pthread_mutex_unlock(&sck_lock);
			return;
		}

		pthread_mutex_lock(&sck_lock);
		sck_snaps++;
		pthread_mutex_unlock(&sck_lock);

		sem = dispatch_semaphore_create(0);
		[SCScreenshotManager captureImageWithFilter:filter
		    configuration:config
		    completionHandler:^(CGImageRef captured, NSError *err) {
			image = captured == NULL ? NULL : CGImageRetain(captured);
			error = [err retain];
			dispatch_semaphore_signal(sem);
		}];
		completed = sck_wait(sem, 1.0);
		sck_release_semaphore(sem, completed);

		if (! completed || error != nil || image == NULL) {
			fprintf(stderr, "macosx_sck: screenshot failed: %s\n",
			    sck_error_text(error));
			pthread_mutex_lock(&sck_lock);
			sck_errors++;
			sck_log_stats(sck_errors <= 5);
			pthread_mutex_unlock(&sck_lock);
		} else {
			sck_store_image(image);
		}

		if (image != NULL) {
			CGImageRelease(image);
		}
		[error release];
		[filter release];
		[config release];
		pthread_mutex_lock(&sck_lock);
		sck_snap_in_flight = 0;
		pthread_mutex_unlock(&sck_lock);
	}
}

int macosx_sck_init(void)
{
	if (@available(macOS 14.0, *)) {
		CGRect bounds;
		int w, h;
		size_t sz;
		SCStreamConfiguration *config = nil;

		sck_content_interval = sck_interval("X11VNC_SCK_CONTENT_INTERVAL", 1.0, 0.1, 10.0);
		sck_snap_interval = sck_interval("X11VNC_SCK_SNAP_INTERVAL", 0.05, 0.01, 1.0);
		fprintf(stderr, "macosx_sck_init: starting SCScreenshotManager path\n");
		fprintf(stderr, "macosx_sck: snap_interval=%.3f content_interval=%.3f shows_cursor=1 wireframe=0\n",
		    sck_snap_interval, sck_content_interval);
		if (getenv("X11VNC_NO_SCK")) {
			fprintf(stderr, "macosx_sck_init: disabled by X11VNC_NO_SCK.\n");
			return 0;
		}

		bounds = CGDisplayBounds(displayID);
		w = (int) bounds.size.width;
		h = (int) bounds.size.height;
		if (w <= 0 || h <= 0) {
			return 0;
		}

		sz = (size_t) w * 4 * (size_t) h;
		if (sck_fb == NULL || sck_fb_len < sz) {
			char *tmp = (char *) realloc(sck_fb, sz + 4);
			char *scratch = (char *) realloc(sck_tmp_fb, sz + 4);
			if (tmp == NULL || scratch == NULL) {
				if (tmp != NULL) {
					sck_fb = tmp;
				}
				if (scratch != NULL) {
					sck_tmp_fb = scratch;
				}
				return 0;
			}
			sck_fb = tmp;
			sck_tmp_fb = scratch;
			sck_fb_len = sz;
		}
		memset(sck_fb, 0, sz + 4);
		memset(sck_tmp_fb, 0, sz + 4);

		sck_w = w;
		sck_h = h;
		sck_bpp = 32;
		sck_bpl = w * 4;
		sck_snaps = 0;
		sck_frames = 0;
		sck_errors = 0;
		sck_copies = 0;
		sck_valid = 0;
		sck_content_time = 0;
		sck_last_snap_time = 0;
		sck_log_time = 0;

		if (! sck_refresh_content()) {
			fprintf(stderr, "macosx_sck_init: no shareable content.\n");
			return 0;
		}

		config = [[SCStreamConfiguration alloc] init];
		config.width = (size_t) w;
		config.height = (size_t) h;
		config.pixelFormat = kCVPixelFormatType_32BGRA;
		config.showsCursor = YES;
		if (@available(macOS 14.2, *)) {
			config.includeChildWindows = YES;
		}
		[sck_config release];
		sck_config = [config retain];
		[config release];

		if (sck_saved_show_cursor < 0) {
			sck_saved_show_cursor = show_cursor;
		}
		if (sck_saved_wireframe < 0) {
			sck_saved_wireframe = wireframe;
		}
		show_cursor = 0;
		wireframe = 0;

		pthread_mutex_lock(&sck_lock);
		sck_active = 1;
		pthread_mutex_unlock(&sck_lock);

		macosx_sck_snap();
		fprintf(stderr, "macosx_sck_init: using SCScreenshotManager snapshots %dx%d bpp=%d Bpl=%d\n",
		    sck_w, sck_h, sck_bpp, sck_bpl);
		return 1;
	}

	fprintf(stderr, "macosx_sck_init: SCScreenshotManager needs macOS 14.0 or newer.\n");
	return 0;
}

void macosx_sck_fini(void)
{
	pthread_mutex_lock(&sck_lock);
	sck_active = 0;
	sck_valid = 0;
	sck_snap_in_flight = 0;
	sck_content_in_flight = 0;
	pthread_mutex_unlock(&sck_lock);

	[sck_config release];
	sck_config = nil;
	[sck_display release];
	sck_display = nil;
	[sck_windows release];
	sck_windows = nil;

	free(sck_fb);
	sck_fb = NULL;
	free(sck_tmp_fb);
	sck_tmp_fb = NULL;
	sck_fb_len = 0;
	sck_w = 0;
	sck_h = 0;
	sck_bpl = 0;
	if (sck_saved_show_cursor >= 0) {
		show_cursor = sck_saved_show_cursor;
		sck_saved_show_cursor = -1;
	}
	if (sck_saved_wireframe >= 0) {
		wireframe = sck_saved_wireframe;
		sck_saved_wireframe = -1;
	}
}

int macosx_sck_active(void)
{
	int active;
	pthread_mutex_lock(&sck_lock);
	active = sck_active && sck_valid;
	pthread_mutex_unlock(&sck_lock);
	return active;
}

int macosx_sck_get_width(void)
{
	return sck_w;
}

int macosx_sck_get_height(void)
{
	return sck_h;
}

int macosx_sck_get_bpp(void)
{
	return sck_bpp;
}

int macosx_sck_get_Bpl(void)
{
	return sck_bpl;
}

int macosx_sck_copy(char *dest, int x, int y, unsigned int w, unsigned int h)
{
	int pixelsize;
	const char *src;
	unsigned int line;

	if (sck_fb == NULL || dest == NULL) {
		return 0;
	}
	if (x < 0 || y < 0 ||
	    (int) (x + w) > sck_w || (int) (y + h) > sck_h) {
		return 0;
	}

	pixelsize = sck_bpp / 8;
	pthread_mutex_lock(&sck_lock);
	if (! sck_active || ! sck_valid) {
		pthread_mutex_unlock(&sck_lock);
		return 0;
	}
	src = sck_fb + (size_t) sck_bpl * (size_t) y +
	    (size_t) pixelsize * (size_t) x;
	for (line = 0; line < h; line++) {
		memcpy(dest + (size_t) line * (size_t) w * (size_t) pixelsize,
		    src + (size_t) line * (size_t) sck_bpl,
		    (size_t) w * (size_t) pixelsize);
	}
	sck_copies++;
	sck_log_stats(0);
	pthread_mutex_unlock(&sck_lock);
	return 1;
}

#else

int macosx_sck_init(void) { return 0; }
void macosx_sck_fini(void) {}
void macosx_sck_snap(void) {}
int macosx_sck_active(void) { return 0; }
int macosx_sck_get_width(void) { return 0; }
int macosx_sck_get_height(void) { return 0; }
int macosx_sck_get_bpp(void) { return 0; }
int macosx_sck_get_Bpl(void) { return 0; }
int macosx_sck_copy(char *dest, int x, int y, unsigned int w, unsigned int h)
{
	if (dest || x || y || w || h) {}
	return 0;
}

#endif

#endif
