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

#ifdef HAVE_XCOMPOSITE
#include <X11/extensions/Xcomposite.h>
#endif

#include "aosd-internal.h"


/* helpers forward declarations */
static Display* aosd_get_display(void);
static Window make_window(Display*, Window, Visual*, Colormap, Bool);
static void set_hints(Display*, Window);
static Pixmap take_snapshot(Aosd*);
static Aosd* aosd_internal_new(Display*, Bool);

#ifdef HAVE_XCOMPOSITE
int
aosd_check_composite_ext(void)
{
  Display* dsp = aosd_get_display();
  int result, event_base, error_base;
  
  if (dsp == NULL)
    return 0;
  
  result = XCompositeQueryExtension(dsp, &event_base, &error_base);

  XCloseDisplay(dsp);
  return result;
}

int
aosd_check_composite_mgr(void)
{
  Display *dsp = aosd_get_display();
  Atom comp_manager_atom;
  char comp_manager_hint[32];
  Window win;
  
  if (dsp == NULL)
    return 0;
  
  snprintf(comp_manager_hint, 32, "_NET_WM_CM_S%d", DefaultScreen(dsp));
  comp_manager_atom = XInternAtom(dsp, comp_manager_hint, False);
  win = XGetSelectionOwner(dsp, comp_manager_atom);
  
  XCloseDisplay(dsp);
  return (win != None);
}
#endif

#ifdef HAVE_XCOMPOSITE
static Visual*
composite_find_argb_visual(Display* dsp, int scr)
{
  XVisualInfo* xvi;
  XVisualInfo	template;
  int nvi, i;
  XRenderPictFormat* format;
  Visual* visual = NULL;

  template.screen = scr;
  template.depth = 32;
  template.class = TrueColor;

  xvi = XGetVisualInfo(dsp,
                       VisualScreenMask | VisualDepthMask | VisualClassMask,
                       &template, &nvi);
  if (xvi == NULL)
    return NULL;

  for (i = 0; i < nvi; i++)
  {
    format = XRenderFindVisualFormat(dsp, xvi[i].visual);
    if (format->type == PictTypeDirect &&
        format->direct.alphaMask)
    {
      visual = xvi[i].visual;
      break;
    }
  }

  XFree (xvi);  
  return visual;
}
#endif

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




/* internal helpers */
static Display*
aosd_get_display(void)
{
  Display* dsp = XOpenDisplay(NULL);

  if (dsp == NULL)
    fprintf(stderr, "libaosd: Couldn't open the display.\n");

  return dsp;
}

Aosd*
aosd_internal_new(Display* dsp, Bool argb)
{
  Aosd* aosd;
  int screen_num = DefaultScreen(dsp);
  Window win, root_win = DefaultRootWindow(dsp);
  Visual* visual = NULL;
  Colormap colormap = None;

#ifdef HAVE_XCOMPOSITE
  if (argb)
  {
    visual = composite_find_argb_visual(dsp, screen_num);
    if (visual == NULL)
      return NULL;
    colormap = XCreateColormap(dsp, root_win, visual, AllocNone);
  }
#endif

  win = make_window(dsp, root_win, visual, colormap, argb);

  aosd = calloc(1, sizeof(Aosd));
  aosd->display = dsp;
  aosd->visual = visual;
  aosd->colormap = colormap;
  aosd->win = win;
  aosd->root_win = root_win;
  aosd->screen_num = screen_num;
  aosd->transparent = 1;
  aosd->composite = argb ? 1 : 0;
  aosd->mouse_processor.mouse_event_cb = NULL;
  aosd->background.set = 0;

  return aosd;
}

static Pixmap
take_snapshot(Aosd* aosd)
{
  Display* dsp = aosd->display;
  int width = aosd->width, height = aosd->height;
  Pixmap pixmap;
  GC gc;

  /* create a pixmap to hold the screenshot */
  pixmap = XCreatePixmap(dsp, aosd->win, width, height,
                         DefaultDepth(dsp, DefaultScreen(dsp)));

  /* then copy the screen into the pixmap */
  gc = XCreateGC(dsp, pixmap, 0, NULL);
  XSetSubwindowMode(dsp, gc, IncludeInferiors);
  XCopyArea(dsp, DefaultRootWindow(dsp), pixmap, gc,
            aosd->x, aosd->y, width, height, 0, 0);
  XSetSubwindowMode(dsp, gc, ClipByChildren);
  XFreeGC(dsp, gc);

  return pixmap;
}

static void
set_hints(Display* dsp, Window win)
{
  // XXX: Should these be set externally by the users?
  XClassHint* classhints = XAllocClassHint();
  classhints->res_class = "Atheme";
  classhints->res_name = "aosd";
  XSetClassHint(dsp, win, classhints);
  XFree(classhints);

  /* we're almost a _NET_WM_WINDOW_TYPE_SPLASH, but we don't want
   * to be centered on the screen.  instead, manually request the
   * behavior we want. */

  /* turn off window decorations.
   * we could pull this in from a motif header, but it's easier to
   * use this snippet i found on a mailing list. */
  Atom mwm_hints = XInternAtom(dsp, "_MOTIF_WM_HINTS", False);
  struct {
    long flags, functions, decorations, input_mode;
  } mwm_hints_setting = { 1<<1, 0, 0, 0 };

  XChangeProperty(dsp, win,
    mwm_hints, mwm_hints, 32, PropModeReplace,
    (unsigned char *)&mwm_hints_setting, 4);

  /* always on top, not in taskbar or pager. */
  Atom win_state = XInternAtom(dsp, "_NET_WM_STATE", False);
  Atom win_state_setting[] =
  {
    XInternAtom(dsp, "_NET_WM_STATE_ABOVE", False),
    XInternAtom(dsp, "_NET_WM_STATE_SKIP_TASKBAR", False),
    XInternAtom(dsp, "_NET_WM_STATE_SKIP_PAGER", False)
  };
  XChangeProperty(dsp, win, win_state, XA_ATOM, 32,
    PropModeReplace, (unsigned char*)&win_state_setting, 3);
}

static Window
make_window(Display* dsp, Window root_win, Visual* visual, Colormap colormap, Bool argb)
{
  Window win;
  XSetWindowAttributes att;

  att.backing_store = WhenMapped;
  att.background_pixel = 0x0;
  att.border_pixel = 0;
  att.background_pixmap = None;
  att.colormap = colormap;
  att.save_under = True;
  att.event_mask = ExposureMask | StructureNotifyMask | ButtonPressMask;
  att.override_redirect = True;

  if (argb)
  {
    win = XCreateWindow(dsp, root_win,
      -1, -1, 1, 1, 0, 32, InputOutput, visual, CWBackingStore | CWBackPixel |
      CWBackPixmap | CWBorderPixel | CWColormap | CWEventMask | CWSaveUnder |
      CWOverrideRedirect, &att);
  }
  else
  {
    win = XCreateWindow(dsp, root_win,
      -1, -1, 1, 1, 0, CopyFromParent, InputOutput, CopyFromParent,
      CWBackingStore | CWBackPixel | CWBackPixmap | CWBorderPixel |
      CWEventMask | CWSaveUnder | CWOverrideRedirect, &att);
  }

  set_hints(dsp, win);

  return win;
}

/* vim: set ts=2 sw=2 et cino=(0 : */
