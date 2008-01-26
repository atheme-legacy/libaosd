/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2008 Eugene Paskevich <eugene@raptor.kiev.ua>
 */

#define DEBUG(fmt, ...) \
  fprintf(stderr, fmt "\n",## __VA_ARGS__)
#define FATAL(fmt, ...) \
{ \
  DEBUG(fmt,## __VA_ARGS__); \
  exit(EXIT_FAILURE); \
}

#define START_CATCH     do {
#define END_CATCH       } while (0)

#define CATCH(var, explanation, ...) \
  (var) = (__VA_ARGS__); \
  if ((var) != TRUE) \
  { \
    DEBUG(explanation); \
    break; \
  }

static struct
{
  /* Opacity */
  guint8 back_alpha;
  guint8 shadow_alpha;
  guint8 fore_alpha;

  /* Geometry */
  guint8 position; // 0-8
  gint16 x_offset;
  gint16 y_offset;
  gint8 shadow_offset; // counted diagonal
  guint8 padding;

  /* Coloring */
  gchar* back_color;
  gchar* shadow_color;
  gchar* fore_color;

  /* Timing */
  guint16 fade_in;
  guint16 fade_full;
  guint16 fade_out;
} config =
{
  0, 192, 255,

  8, -50, -50, 2, 0,

  NULL, "black", "green",

  300, 3000, 300
};

static struct
{
  Aosd* aosd;
  TextRenderData* rend;
} data;

/* vim: set ts=2 sw=2 et : */
