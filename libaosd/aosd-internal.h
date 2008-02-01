#include "config.h"

void make_window(Aosd*);
void set_window_properties(Display*, Window);
Pixmap take_snapshot(Aosd*);
Bool init_lock_pair(LockPair* pair);
void kill_lock_pair(LockPair* pair);

#ifdef HAVE_XCOMPOSITE
Bool composite_check_ext_and_mgr(Display*, int);
Visual* composite_find_argb_visual(Display*, int);
#endif

/* vim: set ts=2 sw=2 et : */
