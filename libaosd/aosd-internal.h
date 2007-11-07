/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 *
 * With further development by Giacomo Lozito <james@develia.org>
 * - added real transparency with X Composite Extension
 * - added mouse event handling on OSD window
 * - added/changed some other stuff
 */

#include <X11/Xlib.h>

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
  Window win;
  Window root_win;
  Visual* visual;
  Colormap colormap;
  int screen_num;
  unsigned int depth;
  int transparent;
  int composite;
  int x, y, width, height;

  AosdBackground background;
  RenderCallback renderer;
  MouseEventCallback mouse_processor;
};

/* vim: set ts=2 sw=2 et cino=(0 : */
