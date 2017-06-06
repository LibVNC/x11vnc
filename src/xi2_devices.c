/*
  XInput2 device handling routines for x11vnc.  

  Copyright (C) 2009-2010 Christian Beier <dontmind@freeshell.org>
  All rights reserved.

  This file is part of x11vnc.

  x11vnc is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at
  your option) any later version.

  x11vnc is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with x11vnc; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
  or see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <X11/Xproto.h> 
#include <X11/keysym.h> 

#include "x11vnc.h" 
#include "cursor.h"
#include "cleanup.h"
#include "win_utils.h"
#include "xi2_devices.h" 

#ifdef HAVE_LIBXCURSOR
#include <X11/Xcursor/Xcursor.h> 
#ifdef HAVE_CAIRO
#include <cairo.h>
#endif
#endif


/* does the X version we're running on support XI2? */
int xinput2_present;
int xi2_device_creation_in_progress;


/*
  create MD with given name
  returns device id, -1 on error
*/
int createMD(Display* dpy, char* name)
{
#ifndef HAVE_XI2
  return -1;
#else
  int dev_id = -1;
  XErrorHandler old_handler;
  XIAddMasterInfo c;
  XIDeviceInfo	*devinfo;
  int		num_devices, i;
  char handle[256]; /* device name */
  snprintf(handle, 256, "%s pointer", name);

  c.type = XIAddMaster;
  c.name = name;
  c.send_core = 1;
  c.enable = 1;

  X_LOCK;

  trapped_xerror = 0;
  old_handler = XSetErrorHandler(trap_xerror);
	
  XIChangeHierarchy(dpy, (XIAnyHierarchyChangeInfo*)&c, 1);
  XSync(dpy, False);

  if(trapped_xerror) {
    XSetErrorHandler(old_handler);
    trapped_xerror = 0;
    X_UNLOCK;
    return -1;
  }

  XSetErrorHandler(old_handler);
  trapped_xerror = 0;

  /* find newly created dev by name
     FIXME: better wait for XIHierarchy event here? */
  devinfo = XIQueryDevice(dpy, XIAllMasterDevices, &num_devices);
  for(i = num_devices-1; i >= 0; --i) 
    if(strcmp(devinfo[i].name, handle) == 0)
      {
	dev_id = devinfo[i].deviceid;
	break;
      }
 
  XIFreeDeviceInfo(devinfo);

  X_UNLOCK;

  return dev_id;
#endif
}



/*
  remove device 
  return 1 on success, 0 on failure
*/
int removeMD(Display* dpy, int dev_id)
{
#ifndef HAVE_XI2
  return 0;
#else
  int found = 0, res = 0;
  XIDeviceInfo	*devinfo;
  int		num_devices, i;

  if(dev_id < 0)
    return 0;

  X_LOCK;

  /* see if this device exists */
  devinfo = XIQueryDevice(dpy, XIAllMasterDevices, &num_devices);
  for(i = 0; i < num_devices; ++i)
    if(devinfo[i].deviceid == dev_id)
      found = 1;
  XIFreeDeviceInfo(devinfo);

  if(found) {
    XIRemoveMasterInfo r;

    /* we need to unset client pointer */
    XISetClientPointer(dpy, None, dev_id);
    XSync(dpy, False);
  
    /* actually remove device pair */
    r.type = XIRemoveMaster;
    r.deviceid = dev_id;
    r.return_mode = XIFloating;

    res = XIChangeHierarchy(dpy, (XIAnyHierarchyChangeInfo*)&r, 1) == Success ? 1 : 0;
    XSync(dpy, False);
  }

  X_UNLOCK;

  return res;
#endif
}


void removeAllMDs(Display *dpy)
{
    /* remove all created XInput2 devices */
    rfbClientIteratorPtr iter = rfbGetClientIterator(screen);
    rfbClientPtr cl;
    while((cl = rfbClientIteratorNext(iter))) {
	ClientData *cd = (ClientData *) cl->clientData;
	if(removeMD(dpy, cd->ptr_id))
	    rfbLog("cleanup: removed XInput2 MD for client %s.\n", cl->host);
    }
    rfbReleaseClientIterator(iter);
}




int getPairedMD(Display* dpy, int dev_id)
{
#ifndef HAVE_XI2
  return -1;
#else
  int paired = -1;
  XIDeviceInfo* devinfo;
  int devicecount = 0;

  if(dev_id < 0)
    return paired;

  X_LOCK;

  devinfo = XIQueryDevice(dpy, dev_id, &devicecount);
  if(devicecount)
    paired = devinfo->attachment;
  XIFreeDeviceInfo(devinfo);

  X_UNLOCK;

  return paired;
#endif
}






/*
  set cursor of pointer dev.
  returns the cursor shape as an rfbCursorPtr
*/
rfbCursorPtr setClientCursor(Display *dpy, int dev_id, float r, float g, float b, char *label)
{
#ifndef HAVE_LIBXCURSOR
  return NULL;
#else
#ifndef HAVE_CAIRO
  return NULL;
#else

  /* label setup */
  const int idFontSize = 18;
  const int idXOffset = 11;
  const int idYOffset = 25;
  const size_t textsz = 64;
  char text[textsz];
  int total_width, total_height;
  cairo_surface_t* main_surface;
  cairo_surface_t* dummy_surface;
  cairo_surface_t* barecursor_surface;
  cairo_t* cr;
  cairo_text_extents_t est;
  Cursor cursor;
  XcursorImage *cursor_image = NULL;
  rfbCursorPtr rfbcursor = NULL;

  if(dev_id < 0)
    return NULL;

  if(label)
    snprintf(text, textsz, "%s", label);
  else
    snprintf(text, textsz, "%i", (int) dev_id);
 
  /* simple cursor w/o label */
  barecursor_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 24, 24);
  cr = cairo_create(barecursor_surface);
  cairo_move_to (cr, 1, 1);
  cairo_line_to (cr, 12, 8);
  cairo_line_to (cr, 5, 15);
  cairo_close_path (cr);
  cairo_set_source_rgba(cr, r, g, b, 0.9);
  cairo_fill_preserve (cr);
  cairo_set_source_rgba(cr, 0, 0, 0, 0.8);
  cairo_set_line_width (cr, 0.8);
  cairo_stroke (cr);

    
  /* get estimated text extents */
  dummy_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 500, 10);/* ah well, but should fit */
  cr = cairo_create(dummy_surface);
  cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (cr, idFontSize);
  cairo_text_extents(cr, text, &est);

  /* an from these calculate our final size */
  total_width = (int)(idXOffset + est.width + est.x_bearing);	
  total_height = (int)(idYOffset + est.height + est.y_bearing);	

  /* draw evrything */
  main_surface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, total_width, total_height );
  cr = cairo_create(main_surface);
  cairo_set_source_surface(cr, barecursor_surface, 0, 0);
  cairo_paint (cr);
  cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (cr, idFontSize);
  cairo_set_source_rgba (cr, r, g, b, 0.8);
  cairo_move_to(cr, idXOffset, idYOffset);
  cairo_show_text(cr,text);
    
  X_LOCK;
  /* copy cairo surface to cursor image */
  cursor_image = XcursorImageCreate(total_width, total_height);
  /* this is important! otherwise we get badmatch, badcursor xerrrors galore... */
  cursor_image->xhot = cursor_image->yhot = 0; 
  memcpy(cursor_image->pixels, cairo_image_surface_get_data (main_surface), sizeof(CARD32) * total_width * total_height);
  X_UNLOCK;

  /* convert to rfb cursor which we return later */
  rfbcursor = pixels2curs((unsigned long*)cursor_image->pixels,
			  cursor_image->width,
			  cursor_image->height,
			  cursor_image->xhot,
			  cursor_image->yhot,
			  bpp/8);
  
  X_LOCK;

  /* and display  */
  cursor = XcursorImageLoadCursor(dpy, cursor_image);
  XIDefineCursor(dpy, dev_id, RootWindow(dpy, DefaultScreen(dpy)), cursor);
  XFreeCursor(dpy, cursor);

  /* clean up */
  cairo_destroy(cr);
  cairo_surface_destroy(dummy_surface);
  cairo_surface_destroy(main_surface);
  cairo_surface_destroy(barecursor_surface);
  XcursorImageDestroy(cursor_image);

  X_UNLOCK;

  return rfbcursor;
#endif
#endif
}



/*
  Sets the paired keyboard's focus to the window underneath the given pointer.
  returns 1 on success, 0 on fail
*/
int setDeviceFocus(Display* dpy, int ptr_id)
{
#ifndef HAVE_XI2
  return 0;
#else

  XErrorHandler old_handler;
  
  Window root_return;
  Window child_return;
  double root_x_return;
  double root_y_return;
  double win_x_return;
  double win_y_return;
  XIButtonState buttons_return;
  XIModifierState modifiers_return;
  XIGroupState group_return;

  XIDeviceInfo* devinfo;
  int devicecount = 0;
  int paired = -1;

  if(ptr_id < 0)
    return 0;
 
  X_LOCK;

  trapped_xerror = 0;
  old_handler = XSetErrorHandler(trap_xerror);
 
  /* get window the pointer is in */
  XIQueryPointer(dpy, ptr_id, rootwin, &root_return, &child_return,
		 &root_x_return, &root_y_return, &win_x_return, &win_y_return,
		 &buttons_return, &modifiers_return, &group_return);

  /* get paired keyboard */
  devinfo = XIQueryDevice(dpy, ptr_id, &devicecount);
  if(devicecount)
    paired = devinfo->attachment;
  XIFreeDeviceInfo(devinfo);

  XISetFocus(dpy, paired, find_client(dpy, root_return, child_return), CurrentTime);
  XSync(dpy, False);

  if(trapped_xerror) {
    XSetErrorHandler(old_handler);
    trapped_xerror = 0;
    X_UNLOCK;
    return 0;
  }

  XSetErrorHandler(old_handler);
  trapped_xerror = 0;

  X_UNLOCK;

  return 1;
#endif
}



/*
  sets the XI client pointer for client window underneath this pointer.
  returns 1 on success, 0 on fail
*/
int setXIClientPointer(Display* dpy, int dev_id)
{
#ifndef HAVE_XI2
  return 0;
#else

  XErrorHandler old_handler;
  
  Window root_return;
  Window child_return;
  double root_x_return;
  double root_y_return;
  double win_x_return;
  double win_y_return;
  XIButtonState buttons_return;
  XIModifierState modifiers_return;
  XIGroupState group_return;

  if(dev_id < 0)
    return 0;
 
  X_LOCK;

  trapped_xerror = 0;
  old_handler = XSetErrorHandler(trap_xerror);
 
  /* get window the pointer is in */
  XIQueryPointer(dpy, dev_id, rootwin, &root_return, &child_return,
		 &root_x_return, &root_y_return, &win_x_return, &win_y_return,
		 &buttons_return, &modifiers_return, &group_return);

  XISetClientPointer(dpy, find_client(dpy, root_return, child_return), dev_id);
  XSync(dpy, False);

  if(trapped_xerror) {
    XSetErrorHandler(old_handler);
    fprintf(stderr, "got x error\n");
    trapped_xerror = 0;
    X_UNLOCK;
    return 0;
  }

  XSetErrorHandler(old_handler);
  trapped_xerror = 0;

  X_UNLOCK;

  return 1;
#endif
}
