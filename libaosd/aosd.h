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

#include <X11/Xutil.h>

#include <cairo/cairo.h>

#include <sys/time.h>  /* timeval */

/* global object type */
typedef struct _Aosd Aosd;

typedef enum {
  AOSD_COORD_MINIMUM = 0,
  AOSD_COORD_CENTER,
  AOSD_COORD_MAXIMUM
} AosdCoordinate;

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

typedef enum
{
  TRANSPARENCY_NONE = 0,
  TRANSPARENCY_FAKE,
  TRANSPARENCY_COMPOSITE
} AosdTransparency;

/* object (de)allocators */
Aosd* aosd_new(void);
void aosd_destroy(Aosd* aosd);

/* object inspectors */
void aosd_get_name(Aosd* aosd, XClassHint* result);
AosdTransparency aosd_get_transparency(Aosd* aosd);
void aosd_get_geometry(Aosd* aosd, int* x, int* y, int* width, int* height);

/* object configurators */
void aosd_set_name(Aosd* aosd, XClassHint* name);
void aosd_set_transparency(Aosd* aosd, AosdTransparency mode);
void aosd_set_geometry(Aosd* aosd, int x, int y, int width, int height);
void aosd_set_position(Aosd* aosd,
    AosdCoordinate abscissa, AosdCoordinate ordinate, int width, int height,
    int x_offset, int y_offset);
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

/* vim: set ts=2 sw=2 et : */
