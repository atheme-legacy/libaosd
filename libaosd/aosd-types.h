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

typedef enum
{
  UP_NONE = 0,
  UP_HIDE = 1 << 1,
  UP_SIZE = 1 << 2,
  UP_POS  = 1 << 3,
  UP_REND = 1 << 4,
  UP_SHOW = 1 << 5,
  UP_TIME = 1 << 6,

  UP_FINISH = 1 << 15
} UpdateTask;

typedef enum
{
  FLASH_OFF = 0,
  FLASH_START,
  FLASH_FADEIN,
  FLASH_FADEFULL,
  FLASH_FADEOUT,
  FLASH_HIDE,
  FLASH_STOP
} FlashMode;

typedef struct
{
  FlashMode mode;
  unsigned int fade_in;
  unsigned int fade_full;
  unsigned int fade_out;
  cairo_surface_t* surf;
} FlashData;

typedef struct timeval Timer;

struct _Aosd
{
  Display* display;
  int screen_num;
  unsigned int depth;
  Window root_win;
  Window win;
  int x, y, width, height;
  Colormap colormap;
  XRenderPictFormat* xrformat;

  unsigned short update;

  AosdBackground background;
  RenderCallback renderer;
  AosdTransparency mode;
  MouseEventCallback mouse_processor;

  pthread_t main_thread;
  LockPair lock_main;
  LockPair lock_time;
  int pipe[2];

  Timer start;
  unsigned int delta_time;

  Bool mouse_hide;
  Bool shown;
};

#endif

/* vim: set ts=2 sw=2 et : */
