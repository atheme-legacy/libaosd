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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cairo/cairo-xlib-xrender.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "aosd-internal.h"

Aosd*
aosd_new(void)
{
  Aosd* aosd = NULL;
  Display* dsp = XOpenDisplay(NULL);

  if (dsp == NULL)
    fprintf(stderr, "libaosd: Couldn't open the display.\n");
  else
  {
    int screen_num = DefaultScreen(dsp);
    Window root_win = DefaultRootWindow(dsp);

    aosd = calloc(1, sizeof(Aosd));
    aosd->display = dsp;
    aosd->screen_num = screen_num;
    aosd->root_win = root_win;
    aosd->mode = TRANSPARENCY_NONE;

    make_window(aosd);
    aosd_set_name(aosd, NULL);
  }

  return aosd;
}

void
aosd_destroy(Aosd* aosd)
{
  if (aosd == NULL)
    return;

  aosd->root_win = None;
  make_window(aosd);

  XCloseDisplay(aosd->display);
  free(aosd);
}

void
aosd_get_name(Aosd* aosd, XClassHint* result)
{
  if (aosd == NULL || result == NULL)
    return;

  XGetClassHint(aosd->display, aosd->win, result);
}

void
aosd_get_names(Aosd* aosd, char** res_name, char** res_class)
{
  if (aosd == NULL)
    return;

  XClassHint ret;
  aosd_get_name(aosd, &ret);

  if (res_name != NULL)
    *res_name = strdup(ret.res_name);
  XFree(ret.res_name);

  if (res_class != NULL)
    *res_class = strdup(ret.res_class);
  XFree(ret.res_class);
}

AosdTransparency
aosd_get_transparency(Aosd* aosd)
{
  if (aosd == NULL)
    return TRANSPARENCY_NONE;

  return aosd->mode;
}

void
aosd_get_geometry(Aosd* aosd, int* x, int* y, int* width, int* height)
{
  if (x != NULL)
    *x = (aosd == NULL) ? 0 : aosd->x;
  if (y != NULL)
    *y = (aosd == NULL) ? 0 : aosd->y;
  if (width != NULL)
    *width  = (aosd == NULL) ? 0 : aosd->width;
  if (height != NULL)
    *height = (aosd == NULL) ? 0 : aosd->height;
}

void
aosd_get_screen_size(Aosd* aosd, int* width, int* height)
{
  if (aosd == NULL)
    return;

  Display* dsp = aosd->display;
  int scr = aosd->screen_num;

  if (width != NULL)
    *width = DisplayWidth(dsp, scr);
  if (height != NULL)
    *height = DisplayHeight(dsp, scr);
}

Bool
aosd_get_is_shown(Aosd* aosd)
{
  if (aosd == NULL)
    return False;

  return aosd->shown;
}

void
aosd_set_name(Aosd* aosd, XClassHint* name)
{
  Bool flag = False;

  if (aosd == NULL)
    return;

  if (name == NULL)
  {
    name = XAllocClassHint();
    name->res_name = "libaosd";
    name->res_class = "Atheme";
    flag = True;
  }

  XSetClassHint(aosd->display, aosd->win, name);

  if (flag)
    XFree(name);
}

void
aosd_set_names(Aosd* aosd, const char* res_name, const char* res_class)
{
  if (aosd == NULL)
    return;

  XClassHint name = {(char*)res_name, (char*)res_class};
  aosd_set_name(aosd, &name);
}

void
aosd_set_transparency(Aosd* aosd, AosdTransparency mode)
{
  if (aosd == NULL || aosd->mode == mode)
    return;

  // we have to preserve window name
  XClassHint name;
  aosd_get_name(aosd, &name);

  aosd->mode = mode;
  make_window(aosd);

  aosd_set_name(aosd, &name);
  XFree(name.res_name);
  XFree(name.res_class);
}

void
aosd_set_geometry(Aosd* aosd, int x, int y, int width, int height)
{
  if (aosd == NULL)
    return;

  aosd->x      = x;
  aosd->y      = y;
  aosd->width  = width;
  aosd->height = height;

  XMoveResizeWindow(aosd->display, aosd->win, x, y, width, height);
}

void
aosd_set_position(Aosd* aosd, unsigned pos, int width, int height)
{
  if (aosd == NULL)
    return;

  if (pos > 8)
    pos = 4;

  aosd_set_position_with_offset(aosd, pos % 3, pos / 3, width, height, 0, 0);
}

void
aosd_set_position_offset(Aosd* aosd, int x_offset, int y_offset)
{
  if (aosd == NULL)
    return;

  aosd_set_geometry(aosd, aosd->x + x_offset, aosd->y + y_offset,
      aosd->width, aosd->height);
}

void
aosd_set_position_with_offset(Aosd* aosd,
    AosdCoordinate abscissa, AosdCoordinate ordinate, int width, int height,
    int x_offset, int y_offset)
{
  if (aosd == NULL)
    return;

  int dsp_width, dsp_height;

  aosd_get_screen_size(aosd, &dsp_width, &dsp_height);

  int x = dsp_width - width;
  int y = dsp_height - height;

  if (abscissa == COORDINATE_MINIMUM)
    x = 0;
  else
    if (abscissa == COORDINATE_CENTER)
      x /= 2;

  x += x_offset;

  if (ordinate == COORDINATE_MINIMUM)
    y = 0;
  else
    if (ordinate == COORDINATE_CENTER)
      y /= 2;

  y += y_offset;

  aosd_set_geometry(aosd, x, y, width, height);
}

void
aosd_set_renderer(Aosd* aosd, AosdRenderer renderer, void* user_data)
{
  if (aosd == NULL)
    return;

  aosd->renderer.render_cb = renderer;
  aosd->renderer.data = user_data;
}

void
aosd_set_mouse_event_cb(Aosd* aosd, AosdMouseEventCb cb, void* user_data)
{
  if (aosd == NULL)
    return;

  aosd->mouse_processor.mouse_event_cb = cb;
  aosd->mouse_processor.data = user_data;
}

void
aosd_set_hide_upon_mouse_event(Aosd* aosd, Bool enable)
{
  if (aosd == NULL)
    return;

  aosd->mouse_hide = enable;
}

void
aosd_render(Aosd* aosd)
{
  if (aosd == NULL)
    return;

  Display* dsp = aosd->display;
  int scr = aosd->screen_num;
  int width = aosd->width, height = aosd->height;
  Window win = aosd->win;
  Pixmap pixmap;
  GC gc;

  if (aosd->mode == TRANSPARENCY_COMPOSITE)
  {
    pixmap = XCreatePixmap(dsp, win, width, height, 32);
    gc = XCreateGC(dsp, pixmap, 0, NULL);
    XFillRectangle(dsp, pixmap, gc, 0, 0, width, height);
  }
  else
  {
    pixmap = XCreatePixmap(dsp, win, width, height, DefaultDepth(dsp, scr));
    gc = XCreateGC(dsp, pixmap, 0, NULL);
    if (aosd->mode == TRANSPARENCY_FAKE)
      /* make our own copy of the background pixmap as the initial surface */
      XCopyArea(dsp, aosd->background.pixmap, pixmap, gc,
          0, 0, width, height, 0, 0);
    else
      XFillRectangle(dsp, pixmap, gc, 0, 0, width, height);
  }
  XFreeGC(dsp, gc);

  /* render with cairo */
  if (aosd->renderer.render_cb)
  {
    /* create cairo surface using the pixmap */
    XRenderPictFormat* xrformat;
    cairo_surface_t* surf;
    if (aosd->mode == TRANSPARENCY_COMPOSITE)
      xrformat = XRenderFindVisualFormat(dsp, aosd->visual);
    else
      xrformat = XRenderFindVisualFormat(dsp, DefaultVisual(dsp, scr));

    surf = cairo_xlib_surface_create_with_xrender_format(
        dsp, pixmap, ScreenOfDisplay(dsp, scr), xrformat, width, height);

    /* draw some stuff */
    cairo_t* cr = cairo_create(surf);
    aosd->renderer.render_cb(cr, aosd->renderer.data);
    cairo_destroy(cr);
    cairo_surface_destroy(surf);
  }

  /* point window at its new backing pixmap */
  XSetWindowBackgroundPixmap(dsp, win, pixmap);

  /* I think it's ok to free it here because XCreatePixmap(3X11) says: "the X
   * server frees the pixmap storage when there are no references to it".
   */
  XFreePixmap(dsp, pixmap);

  /* and tell the window to redraw with this pixmap */
  XClearWindow(dsp, win);
}

void
aosd_show(Aosd* aosd)
{
  if (aosd == NULL || aosd->shown)
    return;

  if (aosd->mode == TRANSPARENCY_FAKE)
  {
    if (aosd->background.set)
    {
      XFreePixmap(aosd->display, aosd->background.pixmap);
      aosd->background.set = False;
    }
    aosd->background.pixmap = take_snapshot(aosd);
    aosd->background.set = True;
  }

  aosd_render(aosd);
  XMapRaised(aosd->display, aosd->win);
  aosd->shown = True;
}

void
aosd_hide(Aosd* aosd)
{
  if (aosd == NULL || !aosd->shown)
    return;

  XUnmapWindow(aosd->display, aosd->win);
  aosd->shown = False;
}

/* vim: set ts=2 sw=2 et : */
