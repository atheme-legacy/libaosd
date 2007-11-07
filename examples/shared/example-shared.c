/* ghosd -- OSD with fake transparency, cairo, and pango.
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 */

#include <stdlib.h>
#include <glib.h>
#include <aosd.h>

#include "example-shared.h"

ExampleOptions opts = {
  TRUE,
  50, -50
};

gboolean
example_options_parse(int *argc, char ***argv, ...) {
  GOptionEntry options[] = {
    { "opaque", 0, G_OPTION_FLAG_REVERSE, G_OPTION_ARG_NONE,
      (gpointer)&opts.transparent, "turn off transparent background" },
    { "x", 'x', 0, G_OPTION_ARG_INT, (gpointer)&opts.x,
      "x coordinate for window" },
    { "y", 'y', 0, G_OPTION_ARG_INT, (gpointer)&opts.y,
      "y coordinate for window" },
    { NULL }
  };
  GOptionContext *ctx;
  GOptionGroup *group;
  gboolean ret;
  va_list ap;

  va_start(ap, argv);

  ctx = g_option_context_new("");
  g_option_context_add_main_entries(ctx, options, "bar");

  while ((group = va_arg(ap, GOptionGroup*)) != NULL) {
    g_option_context_add_group(ctx, group);
  }
  va_end(ap);

  g_option_context_set_ignore_unknown_options(ctx, FALSE);
  ret = g_option_context_parse(ctx, argc, argv, NULL);
  g_option_context_free(ctx);

  if (opts.x == 0) opts.x = AOSD_COORD_CENTER;
  if (opts.y == 0) opts.y = AOSD_COORD_CENTER;

  return ret;
}

void
example_round_rect(cairo_t *cr, int x, int y, int w, int h, int r) {
  cairo_move_to(cr, x+r, y);
  cairo_line_to(cr, x+w-r, y); /* top edge */
  cairo_curve_to(cr, x+w, y, x+w, y, x+w, y+r);
  cairo_line_to(cr, x+w, y+h-r); /* right edge */
  cairo_curve_to(cr, x+w, y+h, x+w, y+h, x+w-r, y+h);
  cairo_line_to(cr, x+r, y+h); /* bottom edge */
  cairo_curve_to(cr, x, y+h, x, y+h, x, y+h-r);
  cairo_line_to(cr, x, y+r); /* left edge */
  cairo_curve_to(cr, x, y, x, y, x+r, y);
  cairo_close_path(cr);
}

/* vim: set ts=2 sw=2 et cino=(0 : */
