#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xatom.h>

#ifdef HAVE_XCOMPOSITE
#include <X11/extensions/Xcomposite.h>
#endif

#include "aosd-types.h"
#include "aosd-internal.h"

void
make_window(Aosd* aosd)
{
  Display* dsp = aosd->display;
  Window root_win = aosd->root_win;
  int scr = aosd->screen_num;

  if (aosd->background.set)
  {
    XFreePixmap(dsp, aosd->background.pixmap);
    aosd->background.set = False;
  }

  if (aosd->colormap != None)
  {
    XFreeColormap(dsp, aosd->colormap);
    aosd->colormap = None;
  }

  if (aosd->win != None)
  {
    XDestroyWindow(dsp, aosd->win);
    aosd->win = None;
  }

  if (root_win == None)
    return;

  Visual* visual = NULL;
  XSetWindowAttributes att;

  att.backing_store = WhenMapped;
  att.background_pixel = 0x0;
  att.background_pixmap = None;
  att.border_pixel = 0;
  att.event_mask = ExposureMask | StructureNotifyMask | ButtonPressMask;
  att.save_under = True;
  att.override_redirect = True;

  unsigned long value_mask = CWBackingStore | CWBackPixel | CWBackPixmap |
    CWBorderPixel | CWEventMask | CWSaveUnder | CWOverrideRedirect;

#define WIN(depth, vis, mask) \
  XCreateWindow(dsp, root_win, -1, -1, 1, 1, 0, depth, InputOutput, vis, \
      value_mask | mask, &att)

  if (aosd->mode == TRANSPARENCY_COMPOSITE)
  {
#ifdef HAVE_XCOMPOSITE
    if (composite_check_ext_and_mgr(dsp, scr) &&
        (visual = composite_find_argb_visual(dsp, scr)) != NULL)
    {
      aosd->colormap = att.colormap =
        XCreateColormap(dsp, root_win, visual, AllocNone);
      aosd->win = WIN(32, visual, CWColormap);
    }
    else
#endif
    {
      aosd->mode = TRANSPARENCY_FAKE;
      make_window(aosd);
      return;
    }
  }
  else
    aosd->win = WIN(CopyFromParent, CopyFromParent, 0);
#undef WIN

  if (visual == NULL)
    visual = DefaultVisual(dsp, scr);

  aosd->xrformat = XRenderFindVisualFormat(dsp, visual);

  set_window_properties(dsp, aosd->win);
  if (aosd->width && aosd->height)
    aosd_set_geometry(aosd, aosd->x, aosd->y, aosd->width, aosd->height);
  if (aosd->shown)
    aosd_show(aosd);
}

Pixmap
take_snapshot(Aosd* aosd)
{
  Display* dsp = aosd->display;
  Window root_win = aosd->root_win, win = aosd->win;
  int width = aosd->width, height = aosd->height;
  int scr = aosd->screen_num;
  Pixmap pixmap;
  GC gc;

  /* create a pixmap to hold the screenshot */
  pixmap = XCreatePixmap(dsp, win, width, height, DefaultDepth(dsp, scr));

  /* then copy the screen into the pixmap */
  gc = XCreateGC(dsp, pixmap, 0, NULL);
  XSetSubwindowMode(dsp, gc, IncludeInferiors);
  XCopyArea(dsp, root_win, pixmap, gc, aosd->x, aosd->y, width, height, 0, 0);
  XFreeGC(dsp, gc);

  return pixmap;
}

void
set_window_properties(Display* dsp, Window win)
{
  /* we're almost a _NET_WM_WINDOW_TYPE_SPLASH, but we don't want
   * to be centered on the screen.  instead, manually request the
   * behavior we want. */

  /* turn off window decorations.
   * we could pull this in from a motif header, but it's easier to
   * use this snippet i found on a mailing list. */
  Atom mwm_hints = XInternAtom(dsp, "_MOTIF_WM_HINTS", False);
  struct
  {
    long flags, functions, decorations, input_mode;
  }
  mwm_hints_setting = { (1L<<1), 0L, 0L, 0L };

  XChangeProperty(dsp, win, mwm_hints, mwm_hints, 32,
      PropModeReplace, (unsigned char *)&mwm_hints_setting, 4);

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

Bool
init_lock_pair(LockPair* pair)
{
  if (pair == NULL)
    return False;

  if (pthread_mutex_init(&pair->mutex, NULL) != 0)
    return False;

  if (pthread_cond_init(&pair->cond, NULL) != 0)
  {
    pthread_mutex_destroy(&pair->mutex);
    return False;
  }

  return True;
}

void
kill_lock_pair(LockPair* pair)
{
  if (pair == NULL)
    return;

  pthread_mutex_destroy(&pair->mutex);
  pthread_cond_destroy(&pair->cond);
}

#ifdef HAVE_XCOMPOSITE
Bool
composite_check_ext_and_mgr(Display* dsp, int scr)
{
  int event_base, error_base;
  Atom comp_manager_atom;
  char comp_manager_hint[32];
  
  if (!XCompositeQueryExtension(dsp, &event_base, &error_base))
    return False;

  snprintf(comp_manager_hint, 32, "_NET_WM_CM_S%d", scr);
  comp_manager_atom = XInternAtom(dsp, comp_manager_hint, False);

  return (XGetSelectionOwner(dsp, comp_manager_atom) != None);
}

Visual*
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

/* vim: set ts=2 sw=2 et : */
