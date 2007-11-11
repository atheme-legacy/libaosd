/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 *
 * With further development by Giacomo Lozito <james@develia.org>
 * - added real transparency with X Composite Extension
 * - added mouse event handling on OSD window
 * - added/changed some other stuff
 */

#include "config.h"

#include "aosd.h"

typedef struct
{
  AosdRenderer render_cb;
  void* data;
  void (*data_destroyer)(void*);
} RenderCallback;

typedef struct
{
  AosdMouseEventCb mouse_event_cb;
  void* data;
} MouseEventCallback;

typedef struct
{
  Pixmap pixmap;
  int set;
} AosdBackground;

struct _Aosd
{
  Display* display;
  int screen_num;
  unsigned int depth;
  Window root_win;
  Window win;
  Visual* visual;
  Colormap colormap;
  int x, y, width, height;

  AosdBackground background;
  RenderCallback renderer;
  AosdTransparency mode;
  MouseEventCallback mouse_processor;
};

void make_window(Aosd*);
void set_window_properties(Display*, Window);
Pixmap take_snapshot(Aosd*);

#ifdef HAVE_XCOMPOSITE
Bool composite_check_ext_and_mgr(Display*, int);
Visual* composite_find_argb_visual(Display*, int);
#endif

/* vim: set ts=2 sw=2 et : */
