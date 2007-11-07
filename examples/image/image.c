/* ghosd -- OSD with fake transparency, cairo, and pango.
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 */

#include <stdio.h>
#include <aosd.h>

#include "example-shared.h"

#define MARGIN 10
#define RADIUS 20
#define WIDTH 185
#define HEIGHT 220

static void
render(Aosd *ghosd, cairo_t *cr, void* data) {
  cairo_surface_t* image = data;
  const int width  = cairo_image_surface_get_width(image);
  const int height = cairo_image_surface_get_height(image);

  cairo_set_source_rgba(cr, 0, 0, 0, 0.5);
  cairo_new_path(cr);
  example_round_rect(cr, 0, 0, width+(2*MARGIN), height+(2*MARGIN), RADIUS);
  cairo_fill(cr);

  cairo_save(cr);
  cairo_set_source_surface(cr, image, MARGIN, MARGIN);
  cairo_paint_with_alpha(cr, 0.5);
  cairo_restore(cr);
}

int main(int argc, char* argv[]) {
  char *filename = NULL;
  GOptionEntry options[] = {
    { "image", 'i', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_STRING,
      (gpointer)&filename, "image to display", "PATH" },
    { NULL }
  };
  GOptionGroup *group = g_option_group_new("image", "Image Options", "image help",
                                           NULL, NULL);
  g_option_group_add_entries(group, options);
  example_options_parse(&argc, &argv, group, NULL);

  if (filename == NULL) {
    printf("must specify image with -i flag.\n"
           "try e.g. /usr/share/pixmaps/gnome-logo-large.png\n");
    return 1;
  }

  Aosd *ghosd;
  cairo_surface_t* image = cairo_image_surface_create_from_png(filename);
  const int width  = cairo_image_surface_get_width(image);
  const int height = cairo_image_surface_get_height(image);

  ghosd = aosd_new();
  aosd_set_transparent(ghosd, opts.transparent);
  aosd_set_position(ghosd, opts.x, opts.y,
                     width+(2*MARGIN), height+(2*MARGIN));
  aosd_set_renderer(ghosd, render, image, NULL);

  aosd_flash(ghosd, 300, 1500);

  cairo_surface_destroy(image);
  aosd_destroy(ghosd);

  return 0;
}

/* vim: set ts=2 sw=2 et cino=(0 : */
