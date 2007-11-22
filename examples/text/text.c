/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 */

#include <stdio.h>

#include <cairo/cairo.h>
#include <pango/pangocairo.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>

#include <libaosd/aosd.h>
#include <libaosd/aosd-text.h>

int
main(int argc, char* argv[])
{
  Aosd* aosd;

  aosd = aosd_new_text(
      "<span font_desc='Times New Roman 30' color='green'>"
      "some sample text using <b>libaosd</b>\n"
      "some unicode: Ελληνικά · 한국어 · עברית ·"
      " ქართული · كورد"
      "</span>",
      50, -50);

  aosd_set_transparency(aosd, TRANSPARENCY_COMPOSITE);

  aosd_flash(aosd, 200, 5000);

  aosd_destroy(aosd);

  return 0;
}

/* vim: set ts=2 sw=2 et : */
