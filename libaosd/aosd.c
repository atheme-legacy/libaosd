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

#include <cairo/cairo-xlib-xrender.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "aosd-internal.h"

#include "aosd.h"

Aosd*
aosd_new(void)
{
  Display* dsp = aosd_get_display();

  if (dsp == NULL)
    return NULL;

  return aosd_internal_new(dsp, False);
}

#ifdef HAVE_XCOMPOSITE
Aosd*
aosd_new_argb(void)
{
  Display* dsp = aosd_get_display();

  if (dsp == NULL)
    return NULL;

  return aosd_internal_new(dsp, True);
}
#endif

void
aosd_destroy(Aosd* aosd)
{
  if (aosd->background.set)
  {
    XFreePixmap(aosd->display, aosd->background.pixmap);
    aosd->background.set = 0;
  }

  if (aosd->composite)
    XFreeColormap(aosd->display, aosd->colormap);

  XDestroyWindow(aosd->display, aosd->win);
  XCloseDisplay(aosd->display);
  free(aosd);
}

int
aosd_get_socket(Aosd* aosd)
{
  return ConnectionNumber(aosd->display);
}

void
aosd_set_transparent(Aosd* aosd, int transparent)
{
  aosd->transparent = (transparent != 0);
}

void
aosd_set_position(Aosd* aosd, int x, int y, int width, int height)
{
  Display* dsp = aosd->display;
  int scr = DefaultScreen(dsp);
  const int dsp_width  = DisplayWidth(dsp, scr);
  const int dsp_height = DisplayHeight(dsp, scr);

  if (x == AOSD_COORD_CENTER)
    x = (dsp_width - width) / 2;
  else if (x < 0)
    x = (dsp_width - width) + x;

  if (y == AOSD_COORD_CENTER)
    y = (dsp_height - height) / 2;
  else if (y < 0)
    y = (dsp_height - height) + y;

  aosd->x      = x;
  aosd->y      = y;
  aosd->width  = width;
  aosd->height = height;

  XMoveResizeWindow(dsp, aosd->win, x, y, width, height);
}

void
aosd_set_renderer(Aosd* aosd, AosdRenderer renderer,
                  void* user_data, void (*user_data_d)(void*))
{
  aosd->renderer.render_cb = renderer;
  aosd->renderer.data = user_data;
  aosd->renderer.data_destroyer = user_data_d;
}

void
aosd_set_mouse_event_cb(Aosd* aosd, AosdMouseEventCb cb, void* user_data)
{
  aosd->mouse_processor.mouse_event_cb = cb;
  aosd->mouse_processor.data = user_data;
}

void
aosd_render(Aosd* aosd)
{
  Display* dsp = aosd->display;
  int width = aosd->width, height = aosd->height;
  Window win = aosd->win;
  Pixmap pixmap;
  GC gc;

  if (aosd->composite)
  {
    pixmap = XCreatePixmap(dsp, win, width, height, 32);
    gc = XCreateGC(dsp, pixmap, 0, NULL);
    XFillRectangle(dsp, pixmap, gc, 0, 0, width, height);
  }
  else
  {
    pixmap = XCreatePixmap(dsp, win, width, height,
                           DefaultDepth(dsp, DefaultScreen(dsp)));
    gc = XCreateGC(dsp, pixmap, 0, NULL);
    if (aosd->transparent)
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
    if (aosd->composite)
    {
      xrformat = XRenderFindVisualFormat(dsp, aosd->visual);
      surf = cairo_xlib_surface_create_with_xrender_format(
             dsp, pixmap, ScreenOfDisplay(dsp, aosd->screen_num),
             xrformat, width, height);
    }
    else
    {
      xrformat = XRenderFindVisualFormat(dsp,
             DefaultVisual(dsp, DefaultScreen(dsp)));
      surf = cairo_xlib_surface_create_with_xrender_format(
             dsp, pixmap, ScreenOfDisplay(dsp, DefaultScreen(dsp)),
             xrformat, width, height);
    }

    /* draw some stuff */
    cairo_t* cr = cairo_create(surf);
    aosd->renderer.render_cb(aosd, cr, aosd->renderer.data);
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
  if (!aosd->composite &&
      aosd->transparent)
  {
    if (aosd->background.set)
    {
      XFreePixmap(aosd->display, aosd->background.pixmap);
      aosd->background.set = 0;
    }
    aosd->background.pixmap = take_snapshot(aosd);
    aosd->background.set = 1;
  }

  aosd_render(aosd);
  XMapRaised(aosd->display, aosd->win);
}

void
aosd_hide(Aosd* aosd)
{
  XUnmapWindow(aosd->display, aosd->win);
}

/* vim: set ts=2 sw=2 et cino=(0 : */
