#include <unistd.h>
#include <errno.h>

#include "aosd-types.h"
#include "aosd-thread.h"

void
aosd_lock(Aosd* aosd)
{
  char c = 0;
  write(aosd->pipe[1], &c, sizeof(c));
  pthread_mutex_lock(&aosd->lock_main.mutex);
}

void
aosd_unlock(Aosd* aosd)
{
  char c;
  read(aosd->pipe[0], &c, sizeof(c));
  pthread_cond_signal(&aosd->lock_main.cond);
  pthread_mutex_unlock(&aosd->lock_main.mutex);
}

void*
aosd_main_event_loop(void* thread_data)
{
  Aosd* aosd = thread_data;
  Display* dsp = aosd->display;
  Window win;

  int xfd = ConnectionNumber(dsp);
  int max = (xfd > aosd->pipe[0] ? xfd : aosd->pipe[0]) + 1;

  pthread_mutex_lock(&aosd->lock_main.mutex);

  while (aosd->update != UP_FINISH)
  {
    int ret;
    fd_set readfds;
    //struct timeval tv;
    struct timeval* tvp = NULL;

    FD_ZERO(&readfds);
    FD_SET(xfd, &readfds);
    FD_SET(aosd->pipe[0], &readfds);

    win = aosd->win;

    if (aosd->update & UP_HIDE)
    {
      if (aosd->shown)
      {
        XUnmapWindow(dsp, win);
        aosd->shown = False;
      }
      else
        aosd->update &= ~UP_HIDE;
    }

    if (aosd->update & UP_POS ||
        aosd->update & UP_SIZE)
    {
      XWindowChanges ch =
      {
        aosd->x,
        aosd->y,
        aosd->width,
        aosd->height,
        0, None, 0
      };
      unsigned mask = 0;

      if (aosd->update & UP_POS)
        mask |= CWX | CWY;
      if (aosd->update & UP_SIZE)
        mask |= CWWidth | CWHeight;

      XConfigureWindow(dsp, win, mask, &ch);
    }

    /* XXX Rendering must be done here */

    if (aosd->update & UP_SHOW)
    {
      if (aosd->shown)
        aosd->update &= ~UP_SHOW;
      else
      {
        XMapRaised(dsp, win);
        aosd->shown = True;
      }
    }

    if (aosd->update & ~UP_TIME)
    {
      XFlush(dsp);
      aosd->update |= UP_TIME;
    }

    if (aosd->update & UP_TIME)
    {
    }

    /* all updating is finished */
    pthread_mutex_lock(&aosd->lock_update.mutex);
    pthread_cond_broadcast(&aosd->lock_update.cond);
    pthread_mutex_unlock(&aosd->lock_update.mutex);

    ret = select(max, &readfds, NULL, NULL, tvp);

    if (ret == -1)
      if (errno == EINTR)
        continue;
      else
        break;
    else
      if (ret == 0)
        // Consider signalling a timeout here
        continue;

    if (FD_ISSET(aosd->pipe[0], &readfds))
    {
      pthread_cond_wait(&aosd->lock_main.cond, &aosd->lock_main.mutex);
    }
    else if (FD_ISSET(xfd, &readfds))
    {
      XEvent ev;
      XNextEvent(dsp, &ev);

      /* throw out unneded events */
      while (ev.type != ButtonPress &&
          XPending(dsp))
        XNextEvent(dsp, &ev);

      switch (ev.type)
      {
        case ButtonPress:
          if (aosd->mouse_hide)
            aosd->update |= UP_HIDE;

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
            aosd->mouse_processor.mouse_event_cb(&mev,
                aosd->mouse_processor.data);
          }
          break;
      }
    }
    else
      break;
  }

  pthread_mutex_unlock(&aosd->lock_main.mutex);

  return NULL;
}

/* vim: set ts=2 sw=2 et : */
