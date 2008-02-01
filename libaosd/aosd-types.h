/* aosd -- OSD with transparency and cairo.
 *
 * Copyright (C) 2008 Eugene Paskevich <eugene@raptor.kiev.ua>
 */

#ifndef __AOSD_TYPES_H__
#define __AOSD_TYPES_H__

#include <pthread.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>

#include "aosd.h"

typedef struct
{
  AosdRenderer render_cb;
  void* data;
} RenderCallback;

typedef struct
{
  AosdMouseEventCb mouse_event_cb;
  void* data;
} MouseEventCallback;

typedef struct
{
  Pixmap pixmap;
  Bool set;
} AosdBackground;

typedef struct
{
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} LockPair;

struct _Aosd
{
  Display* display;
  int screen_num;
  unsigned int depth;
  Window root_win;
  Window win;
  int x, y, width, height;
  XRenderPictFormat* xrformat;

  AosdBackground background;
  RenderCallback renderer;
  AosdTransparency mode;
  MouseEventCallback mouse_processor;

  pthread_t main_thread;
  LockPair lock_main;
  LockPair lock_update;
  int pipe[2];

  Bool mouse_hide;
  Bool shown;
};

#endif

/* vim: set ts=2 sw=2 et : */
