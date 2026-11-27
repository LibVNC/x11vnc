#ifndef _X11VNC_MACOSX_SCK_H
#define _X11VNC_MACOSX_SCK_H

extern int  macosx_sck_init(void);
extern void macosx_sck_fini(void);
extern void macosx_sck_snap(void);
extern int  macosx_sck_active(void);
extern int  macosx_sck_get_width(void);
extern int  macosx_sck_get_height(void);
extern int  macosx_sck_get_bpp(void);
extern int  macosx_sck_get_Bpl(void);
extern int  macosx_sck_copy(char *dest, int x, int y, unsigned int w, unsigned int h);

#endif
