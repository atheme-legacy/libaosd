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
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <sys/poll.h>

#include <X11/Xlib.h>

#include <pango/pango-layout.h>

#include "aosd-internal.h"

static void
aosd_main_iteration(Aosd* aosd)
{
  if (aosd == NULL)
    return;

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
        /* XXX if the window manager disagrees with our positioning here,
         * we loop. */
        if (aosd->x != ev.xconfigure.x ||
            aosd->y != ev.xconfigure.y)
        {
          // width = ev.xconfigure.width; 
          // height = ev.xconfigure.height;
          XMoveResizeWindow(dsp,
              aosd->win, aosd->x, aosd->y, aosd->width, aosd->height);
        }
      }
      break;

    case ButtonPress:
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
        aosd->mouse_processor.mouse_event_cb(aosd, &mev, aosd->mouse_processor.data);
      }
      break;
  }
}

void
aosd_main_iterations(Aosd* aosd)
{
  if (aosd == NULL)
    return;

  while (XPending(aosd->display))
    aosd_main_iteration(aosd);
}

void
aosd_main_until(Aosd* aosd, struct timeval* until)
{
  if (aosd == NULL)
    return;

  aosd_main_iterations(aosd);

  if (until == NULL)
    return;

  struct timeval tv_now;

  for (;;)
  {
    gettimeofday(&tv_now, NULL);
    int dt = (until->tv_sec  - tv_now.tv_sec ) * 1000 +
             (until->tv_usec - tv_now.tv_usec) / 1000;
    if (dt <= 0)
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
      aosd_main_iterations(aosd);
  }
}

typedef struct
{
  cairo_surface_t* surface;
  float alpha;
  RenderCallback user_render;
} AosdFlashData;

static void
flash_render(Aosd* aosd, cairo_t* cr, void* data)
{
  AosdFlashData* flash = data;

  /* the first time we render, let the client render into their own surface */
  if (flash->surface == NULL)
  {
    cairo_t* rendered_cr;
    flash->surface = cairo_surface_create_similar(cairo_get_target(cr),
        CAIRO_CONTENT_COLOR_ALPHA, aosd->width, aosd->height);
    rendered_cr = cairo_create(flash->surface);
    flash->user_render.render_cb(aosd, rendered_cr, flash->user_render.data);
    cairo_destroy(rendered_cr);
  }

  /* now that we have a rendered surface, all we normally do is copy that to
   * the screen */
  cairo_set_source_surface(cr, flash->surface, 0, 0);
  cairo_paint_with_alpha(cr, flash->alpha);
}

/* we don't need to free the flashdata object, because we stack-allocate that.
 * but we do need to let the old user data free itself... */
static void
flash_destroy(void* data)
{
  AosdFlashData* flash = data;
  if (flash->user_render.data_destroyer)
    flash->user_render.data_destroyer(flash->user_render.data);
}

void
aosd_flash(Aosd* aosd, int fade_ms, int total_display_ms)
{
  if (aosd == NULL)
    return;

  AosdFlashData flash = {0};
  memcpy(&flash.user_render, &aosd->renderer, sizeof(RenderCallback));
  aosd_set_renderer(aosd, flash_render, &flash, flash_destroy);

  aosd_show(aosd);

  const int STEP_MS = 50;
  const float dalpha = 1.0 / (fade_ms / (float)STEP_MS);
  struct timeval tv_nextupdate;

  /* fade in */
  for (flash.alpha = 0; flash.alpha < 1.0; flash.alpha += dalpha)
  {
    if (flash.alpha > 1.0)
      flash.alpha = 1.0;
    aosd_render(aosd);

    gettimeofday(&tv_nextupdate, NULL);
    tv_nextupdate.tv_usec += STEP_MS * 1000;
    aosd_main_until(aosd, &tv_nextupdate);
  }

  /* full display */
  flash.alpha = 1.0;
  aosd_render(aosd);

  gettimeofday(&tv_nextupdate, NULL);
  tv_nextupdate.tv_usec += (total_display_ms - (2 * fade_ms)) * 1000;
  aosd_main_until(aosd, &tv_nextupdate);

  /* fade out */
  for (flash.alpha = 1.0; flash.alpha > 0.0; flash.alpha -= dalpha)
  {
    aosd_render(aosd);

    gettimeofday(&tv_nextupdate, NULL);
    tv_nextupdate.tv_usec += STEP_MS * 1000;
    aosd_main_until(aosd, &tv_nextupdate);
  }

  flash.alpha = 0;
  aosd_render(aosd);

  /* display for another half-second,
   * because otherwise the fade out attracts your eye
   * and then you'll see a flash while it repaints where the aosd was.
   */
  gettimeofday(&tv_nextupdate, NULL);
  tv_nextupdate.tv_usec += 500 * 1000;
  aosd_main_until(aosd, &tv_nextupdate);

  aosd_hide(aosd);

  /* restore initial renderer */
  aosd_set_renderer(aosd,
      flash.user_render.render_cb,
      flash.user_render.data,
      flash.user_render.data_destroyer);
}

/* vim: set ts=2 sw=2 et : */
