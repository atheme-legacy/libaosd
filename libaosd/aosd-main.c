/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 *
 * With further development by Giacomo Lozito <james@develia.org>
 * - added real transparency with X Composite Extension
 * - added mouse event handling on OSD window
 * - added/changed some other stuff
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <sys/poll.h>
#include <sys/time.h>

#include <X11/Xlib.h>

#include "aosd-internal.h"

static void
aosd_loop_iteration(Aosd* aosd)
{
  Display* dsp = aosd->display;
  XEvent ev, pev;
  XNextEvent(dsp, &ev);

  /* smash multiple configure/exposes into one. */
  if (ev.type == ConfigureNotify)
  {
    while (XPending(dsp))
    {
      XPeekEvent(dsp, &pev);
      if (pev.type != ConfigureNotify &&
          pev.type != Expose)
        break;
      XNextEvent(dsp, &ev);
    }
  }

  switch (ev.type)
  {
    case Expose:
      break;

    case ConfigureNotify:
      if (aosd->width > 0)
      {
        /* FIXME We might loop here if window manager
         * disagrees with our positioning */
        if (aosd->x != ev.xconfigure.x ||
            aosd->y != ev.xconfigure.y)
          XMoveResizeWindow(dsp, aosd->win,
              aosd->x, aosd->y, aosd->width, aosd->height);
      }
      break;

    case ButtonPress:
      if (aosd->mouse_hide)
        aosd_hide(aosd);

      /* create AosdMouseEvent and pass it to callback function */
      if (aosd->mouse_processor.mouse_event_cb != NULL)
      {
        AosdMouseEvent mev;
        mev.x = ev.xbutton.x;
        mev.y = ev.xbutton.y;
        mev.x_root = ev.xbutton.x_root;
        mev.y_root = ev.xbutton.y_root;
        mev.button = ev.xbutton.button;
        mev.send_event = ev.xbutton.send_event;
        mev.time = ev.xbutton.time;
        aosd->mouse_processor.mouse_event_cb(&mev, aosd->mouse_processor.data);
      }
      break;
  }
}

void
aosd_loop_once(Aosd* aosd)
{
  if (aosd == NULL)
    return;

  while (XPending(aosd->display))
    aosd_loop_iteration(aosd);
}

void
aosd_loop_for(Aosd* aosd, unsigned loop_ms)
{
  if (aosd == NULL)
    return;

  aosd_loop_once(aosd);

  if (loop_ms == 0 || !aosd->shown)
    return;

  struct timeval tv_now;
  struct timeval tv_until;
  gettimeofday(&tv_until, NULL);
  tv_until.tv_usec += loop_ms * 1000;

  for (;;)
  {
    gettimeofday(&tv_now, NULL);
    int dt = (tv_until.tv_sec  - tv_now.tv_sec ) * 1000 +
             (tv_until.tv_usec - tv_now.tv_usec) / 1000;
    if (dt <= 0 || !aosd->shown)
      break;

    struct pollfd pollfd = { ConnectionNumber(aosd->display), POLLIN, 0 };
    int ret = poll(&pollfd, 1, dt);

    if (ret == 0)
      break;

    if (ret < 0)
    {
      if (errno != EINTR)
      {
        perror("poll");
        exit(1);
      }
    }
    else
      aosd_loop_once(aosd);
  }
}

typedef struct
{
  int width, height;
  cairo_surface_t* surface;
  float alpha;
  RenderCallback user_render;
} AosdFlashData;

static void
flash_render(cairo_t* cr, void* data)
{
  AosdFlashData* flash = data;

  /* the first time we render, let the client render into their own surface */
  if (flash->surface == NULL)
  {
    cairo_t* rendered_cr;
    flash->surface = cairo_surface_create_similar(cairo_get_target(cr),
        CAIRO_CONTENT_COLOR_ALPHA, flash->width, flash->height);
    rendered_cr = cairo_create(flash->surface);
    flash->user_render.render_cb(rendered_cr, flash->user_render.data);
    cairo_destroy(rendered_cr);
  }

  /* now that we have a rendered surface, all we normally do is copy that to
   * the screen */
  cairo_set_source_surface(cr, flash->surface, 0, 0);
  cairo_paint_with_alpha(cr, flash->alpha);
}

void
aosd_flash(Aosd* aosd,
    unsigned fade_in_ms, unsigned full_ms, unsigned fade_out_ms)
{
  if (aosd == NULL ||
      (fade_in_ms == 0 &&
       full_ms == 0 &&
       fade_out_ms == 0))
    return;

  AosdFlashData flash = {0};
  memcpy(&flash.user_render, &aosd->renderer, sizeof(RenderCallback));
  aosd_set_renderer(aosd, flash_render, &flash);
  flash.alpha = 0;
  flash.width = aosd->width;
  flash.height = aosd->height;

  float step;

  if (!aosd->shown)
  {
    aosd_show(aosd);
    aosd_loop_once(aosd);
  }

  if (fade_in_ms != 0 && aosd->shown)
  {
    step = 1.0 / (float)fade_in_ms;
    for (; flash.alpha < 1.0 && aosd->shown; flash.alpha += step)
    {
      aosd_render(aosd);
      aosd_loop_for(aosd, step);
    }
  }

  flash.alpha = 1.0;

  if (full_ms != 0 && aosd->shown)
  {
    aosd_render(aosd);
    aosd_loop_for(aosd, full_ms);
  }

  if (fade_out_ms != 0 && aosd->shown)
  {
    step = 1.0 / (float)fade_out_ms;
    for (; flash.alpha > 0.0 && aosd->shown; flash.alpha -= step)
    {
      aosd_render(aosd);
      aosd_loop_for(aosd, step);
    }
  }

  if (aosd->shown)
  {
    aosd_hide(aosd);
    aosd_loop_once(aosd);
  }

  /* restore initial renderer */
  aosd_set_renderer(aosd,
      flash.user_render.render_cb,
      flash.user_render.data);
}

/* vim: set ts=2 sw=2 et : */
