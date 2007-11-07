/* ghosd -- OSD with fake transparency, cairo, and pango.
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 */

#include <stdio.h>

#include <cairo/cairo.h>
#include <pango/pangocairo.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>

#include <aosd.h>
#include <aosd-text.h>

int main(int argc, char* argv[]) {
  Aosd* aosd;

  aosd = aosd_text_new("<span font_desc='Trebuchet 30'>"
                        "some sample text using <b>aosd</b>"
                        "اريد"
                        "</span>", -50, -50);

  aosd_flash(aosd, 1000, 2500);

  aosd_destroy(aosd);

  return 0;
}

/* vim: set ts=2 sw=2 et cino=(0 : */

