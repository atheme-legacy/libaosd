/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2008 Eugene Paskevich <eugene@raptor.kiev.ua>
 */

#define DEBUG(fmt, ...) \
  fprintf(stderr, fmt "\n",## __VA_ARGS__)

#define RESET_CATCH     __ret = TRUE
#define PREPARE_CATCH   gboolean RESET_CATCH
#define START_CATCH     do {
#define END_CATCH       } while (0)
#define RETURN_CATCH    return __ret
#define GOOD_CATCH      (__ret == TRUE)

#define CATCH(expr, err_fmt, ...) \
  __ret = (expr); \
  if (__ret != TRUE) \
  { \
    if (strlen(err_fmt) != 0) \
      DEBUG(err_fmt,## __VA_ARGS__); \
    break; \
  }

#define KILL_FIRST \
  { \
    Line* element = g_queue_pop_head(data.list); \
    free(element->str); \
    free(element); \
  }

static struct Configuration
{
  /* Opacity */
  gint back_alpha;
  gint shadow_alpha;
  gint fore_alpha;

  /* Geometry */
  gint position; // 0-8
  gint x_offset;
  gint y_offset;
  gint shadow_offset; // counted diagonal
  gint padding;

  /* Appearance */
  gint transparency;
  gchar* font;
  gint width;

  /* Coloring */
  gchar* back_color;
  gchar* shadow_color;
  gchar* fore_color;

  /* Timing */
  gint fade_in;
  gint fade_full;
  gint fade_out;

  /* Scrollback */
  gint age;
  gint lines;

  /* Source */
  gchar* input;
} config =
{
  0, 192, 255,

  6, 50, -50, 2, 0,

  2, NULL, 0,

  NULL, "black", "green",

  300, 3000, 300,

  0, 1,

  "-"
};

static struct Globals
{
  Aosd* aosd;
  TextRenderData* rend;
  FILE* input;
  GQueue* list;
  unsigned width;
  unsigned height;
} data =
{
  NULL, NULL, NULL, NULL,
  0, 0
};

typedef struct 
{
  gchar* str;
  time_t stamp;
} Line;

/* vim: set ts=2 sw=2 et : */
