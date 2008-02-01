#ifndef __AOSD_H__
#define __AOSD_H__

#include <X11/Xutil.h>

#include <cairo/cairo.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* global object type */
typedef struct _Aosd Aosd;

// relative coordinates for positioning
typedef enum {
  COORDINATE_MINIMUM = 0,
  COORDINATE_CENTER,
  COORDINATE_MAXIMUM
} AosdCoordinate;

/* minimal struct to handle mouse events */
typedef struct
{
  // relative coordinates
  int x, y;
  // global coordinates
  int x_root, y_root;

  int send_event;

  // button being pressed
  unsigned int button;
  unsigned long time;
}
AosdMouseEvent;

/* various callbacks */
typedef void (*AosdRenderer)(cairo_t* cr, void* user_data);
typedef void (*AosdMouseEventCb)(AosdMouseEvent* event, void* user_data);

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
void aosd_get_names(Aosd* aosd, char** res_name, char** res_class);
AosdTransparency aosd_get_transparency(Aosd* aosd);
void aosd_get_geometry(Aosd* aosd, int* x, int* y, int* width, int* height);
void aosd_get_screen_size(Aosd* aosd, int* width, int* height);
Bool aosd_get_is_shown(Aosd* aosd);

/* object configurators */
void aosd_set_name(Aosd* aosd, XClassHint* name);
void aosd_set_names(Aosd* aosd, char* res_name, char* res_class);
void aosd_set_transparency(Aosd* aosd, AosdTransparency mode);
void aosd_set_geometry(Aosd* aosd, int x, int y, int width, int height);
void aosd_set_position(Aosd* aosd, unsigned pos, int width, int height);
void aosd_set_position_offset(Aosd* aosd, int x_offset, int y_offset);
void aosd_set_position_with_offset(Aosd* aosd,
    AosdCoordinate abscissa, AosdCoordinate ordinate, int width, int height,
    int x_offset, int y_offset);
void aosd_set_renderer(Aosd* aosd, AosdRenderer renderer, void* user_data);
void aosd_set_mouse_event_cb(Aosd* aosd, AosdMouseEventCb cb, void* user_data);
void aosd_set_hide_upon_mouse_event(Aosd* aosd, Bool enable);

/* object manipulators */
void aosd_render(Aosd* aosd);
void aosd_show(Aosd* aosd);
void aosd_hide(Aosd* aosd);

/* X main loop processing */
void aosd_loop_once(Aosd* aosd);
void aosd_loop_for(Aosd* aosd, unsigned loop_ms);

/* automatic object manipulator */
void aosd_flash(Aosd* aosd, unsigned fade_in_ms,
    unsigned full_ms, unsigned fade_out_ms);

#ifdef __cplusplus
}
#endif

#endif /* __AOSD_H__ */

/* vim: set ts=2 sw=2 et : */
