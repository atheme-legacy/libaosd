/* ghosd -- OSD with fake transparency, cairo, and pango.
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 */

#include <glib.h>
#include <cairo/cairo.h>

typedef struct {
  gboolean transparent;
  int x, y;
} ExampleOptions;
extern ExampleOptions opts;

gboolean example_options_parse(int *argc, char ***argv, ...);
void     example_round_rect(cairo_t *cr, int x, int y, int w, int h, int r);

