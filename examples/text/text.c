/* aosd -- OSD with fake transparency, cairo, and pango.
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

int main(int argc, char* argv[]) {
  Aosd* aosd;

  aosd = aosd_text_new("<span font_desc='Trebuchet 30'>"
                        "some sample text using <b>libaosd</b>"
                        "اريد"
                        "</span>", -50, -50);

  aosd_set_transparency(aosd, TRANSPARENCY_COMPOSITE);
  aosd_flash(aosd, 1000, 2500);

  aosd_destroy(aosd);

  return 0;
}

/* vim: set ts=2 sw=2 et cino=(0 : */

