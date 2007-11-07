/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 *
 * With further development by Giacomo Lozito <james@develia.org>
 * - added real transparency with X Composite Extension
 * - added mouse event handling on OSD window
 * - added/changed some other stuff
 */

#ifndef __AOSD_H__
#define __AOSD_H__

#include <cairo/cairo.h>

#include <limits.h>  /* INT_MAX */
#include <sys/time.h>  /* timeval */

/* global object type */
typedef struct _Aosd Aosd;

#define AOSD_COORD_CENTER INT_MAX

/* minimal struct to handle mouse events */
typedef struct
{
  // relative coordinates
  int x, y;
  // global coordinates
  int x_root, y_root;

  // whether we should send the event further
  int send_event;

  // button being pressed
  unsigned int button;
  unsigned long time;
}
AosdMouseEvent;

/* various callbacks */
typedef void (*AosdRenderer)(Aosd* aosd, cairo_t* cr, void* user_data);
typedef void (*AosdMouseEventCb)(Aosd* aosd, AosdMouseEvent* event,
                                 void* user_data);

#ifdef HAVE_XCOMPOSITE
/* composite checkers */
int aosd_check_composite_ext(void);
int aosd_check_composite_mgr(void);
#endif

/* object allocators */
Aosd* aosd_new(void);
#ifdef HAVE_XCOMPOSITE
Aosd* aosd_new_argb(void);
#endif

/* object deallocator */
void aosd_destroy(Aosd* aosd);

int aosd_get_socket(Aosd* aosd);

/* object configurators */
void aosd_set_transparent(Aosd* aosd, int transparent);
void aosd_set_position(Aosd* aosd, int x, int y, int width, int height);
void aosd_set_renderer(Aosd* aosd, AosdRenderer renderer, void* user_data,
                       void (*user_data_d)(void*));
void aosd_set_mouse_event_cb(Aosd* aosd, AosdMouseEventCb cb, void* user_data);

/* manual object manipulators */
void aosd_render(Aosd* aosd);
void aosd_show(Aosd* aosd);
void aosd_hide(Aosd* aosd);

/* automatic object manipulators */
void aosd_main_iterations(Aosd* aosd);
void aosd_main_until(Aosd* aosd, struct timeval* until);
void aosd_flash(Aosd* aosd, int fade_ms, int total_display_ms);

#endif /* __AOSD_H__ */

/* vim: set ts=2 sw=2 et cino=(0 : */
