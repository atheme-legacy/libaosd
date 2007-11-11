/* Copyright (C) 2006 Evan Martin <martine@danga.com> */

#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include <libaosd/aosd.h>

#define MARGIN 10
#define RADIUS 20
#define WIDTH 185
#define HEIGHT 220

struct
{
  gboolean transparent;
  int x, y;
  int alpha;
  char* filename;
} opts =
{
  TRUE,
  0, 0,
  50,
  NULL
};

static void
round_rect(cairo_t* cr, int x, int y, int w, int h, int r)
{
  cairo_move_to(cr, x+r, y);
  cairo_line_to(cr, x+w-r, y); /* top edge */
  cairo_curve_to(cr, x+w, y, x+w, y, x+w, y+r);
  cairo_line_to(cr, x+w, y+h-r); /* right edge */
  cairo_curve_to(cr, x+w, y+h, x+w, y+h, x+w-r, y+h);
  cairo_line_to(cr, x+r, y+h); /* bottom edge */
  cairo_curve_to(cr, x, y+h, x, y+h, x, y+h-r);
  cairo_line_to(cr, x, y+r); /* left edge */
  cairo_curve_to(cr, x, y, x, y, x+r, y);
}

static void
render(Aosd* aosd, cairo_t* cr, void* data)
{
  cairo_surface_t* image = data;
  const int width  = cairo_image_surface_get_width(image);
  const int height = cairo_image_surface_get_height(image);

  cairo_set_source_rgba(cr, 0, 0, 0, 0);
  cairo_new_path(cr);
  round_rect(cr, 0, 0, width+(2*MARGIN), height+(2*MARGIN), RADIUS);
  cairo_close_path(cr);
  cairo_fill(cr);

  cairo_save(cr);
  cairo_set_source_surface(cr, image, MARGIN, MARGIN);
  cairo_paint_with_alpha(cr, opts.alpha / (float)100);
  cairo_restore(cr);
}

static gboolean
parse_options(int* argc, char** argv[])
{
  GOptionContext* ctx;
  GOptionGroup* group;
  gboolean ret;

  GOptionEntry display[] =
  {
    { "opaque", 'o', G_OPTION_FLAG_REVERSE, G_OPTION_ARG_NONE,
      (gpointer)&opts.transparent, "turn off transparent background" },
    { "alpha", 'a', 0, G_OPTION_ARG_INT,
      (gpointer)&opts.alpha, "alpha level of a displayed image (0:100)" },
    { NULL }
  };

  GOptionEntry coords[] =
  {
    { "x", 'x', G_OPTION_FLAG_NOALIAS, G_OPTION_ARG_INT,
      (gpointer)&opts.x, "x coordinate for window" },
    { "y", 'y', G_OPTION_FLAG_NOALIAS, G_OPTION_ARG_INT,
      (gpointer)&opts.y, "y coordinate for window" },
    { NULL }
  };

  GOptionEntry image[] =
  {
    { "image", 'i', G_OPTION_FLAG_FILENAME, G_OPTION_ARG_FILENAME,
      (gpointer)&opts.filename, "image to display", "PATH" },
    { NULL }
  };

  ctx = g_option_context_new("");
  g_option_context_set_ignore_unknown_options(ctx, FALSE);

#define ADD_GROUP(name, desc, help, entries, main) \
  group = g_option_group_new((name), (desc), (help), NULL, NULL); \
  g_option_group_add_entries(group, (entries)); \
  if ((main)) \
    g_option_context_set_main_group(ctx, group); \
  else \
    g_option_context_add_group(ctx, group);

  ADD_GROUP("display", "Display Options:", "Show display help options", display, FALSE);
  ADD_GROUP("coords", "Coordinates Options:", "Show coordinates help options", coords, FALSE);
  ADD_GROUP("image", "Image Options:", "Show image help options", image, TRUE);
#undef ADD_GROUP


  ret = g_option_context_parse(ctx, argc, argv, NULL);
  g_option_context_free(ctx);

  if (ret == FALSE ||
      opts.filename == NULL)
    return FALSE;

  if (opts.x == 0)
    opts.x = AOSD_COORD_CENTER;
  if (opts.y == 0)
    opts.y = AOSD_COORD_CENTER;

  if (opts.alpha < 0)
    opts.alpha = 0;
  if (opts.alpha > 100)
    opts.alpha = 100;

  return TRUE;
}

int
main(int argc, char* argv[])
{
  if (parse_options(&argc, &argv) == FALSE)
  {
    fprintf(stderr, "Error parsing options, try --help.\n");
    return 1;
  }

  Aosd* aosd;

  cairo_surface_t* image = cairo_image_surface_create_from_png(opts.filename);
  const int width  = cairo_image_surface_get_width(image);
  const int height = cairo_image_surface_get_height(image);

  aosd = aosd_new();
  aosd_set_transparency(aosd,
      opts.transparent ? TRANSPARENCY_COMPOSITE : TRANSPARENCY_NONE);

  aosd_set_geometry(aosd,
                    opts.x, opts.y,
                    width + (2 * MARGIN), height + (2 * MARGIN));

  aosd_set_renderer(aosd, render, image, NULL);

  aosd_flash(aosd, 300, 3000);

  cairo_surface_destroy(image);
  aosd_destroy(aosd);

  return 0;
}

/* vim: set ts=2 sw=2 et cino=(0 : */
