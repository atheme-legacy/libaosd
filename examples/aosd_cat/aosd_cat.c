/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 *
 * some additions by Trent Apted <trent@apted.net>
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <sys/time.h>

#include <aosd.h>

#include <pango/pangocairo.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

static float RED = 0.0f;
static float GREEN = 1.0f;
static float BLUE = 0.0f;
static float ALPHA = 1.0f;

static int XPOS = -50;
static int YPOS = -50;

static int SHADOW_OFFSET = 2;
static float SHADOW_RED = 0.0f;
static float SHADOW_GREEN = 0.0f;
static float SHADOW_BLUE = 0.0f;
static float SHADOW_ALPHA = 0.5f;

static int FADE_MS = 300;
static int TIME_MS = 1500;
static unsigned long MAXLINE = 55;

static FILE* INFILE = NULL;
static const char* TEXT = 0;
static unsigned LINES = 1;
static unsigned AGE_MS = 0;

static const char* FONTDESC = "Trebuchet 30";
static const char* EXAMPLE_TEXT = "some sample text using <b>libaosd</b>";

static const int RECT_PADDING = 3; /** Padding for rectangle */
#ifndef VERSION
static const char * const VERSION = "0.0.2";
#endif
static const char * const REVISION = "1";
static const char * const BUILD_TIME = __DATE__ " at " __TIME__;
static const char * const USAGE =
    "Usage: %s [OPTION] [TEXT]...\n"
    "Version: %s\n"
    "Revision: %s built on %s\n"
    "Display TEXT, or standard input, on top of display.\n"
    "\n"

    "  -F, --fontdesc=FONT Use font description (default: \"%s\")\n"
    "  -r, --red=LEVEL     Red level, default is %.2f\n"   /* these could possibly take an optional argument */
    "  -g, --green=LEVEL   Green level, default is %.2f\n" /* ie passing --green (eg) implies {0, 1, 0} */
    "  -b, --blue=LEVEL    Blue level, default is %.2f\n"
    "  -a, --alpha=LEVEL   Alpha level, default is %.2f\n"

    "  -x, --xpos=OFFSET   X-position on screen, default is %d\n"
    "  -y, --ypos=OFFSET   Y-position on screen, default is %d\n"

    "  -d, --delay=TIME    Show for specified time (milliseconds), default is %d\n"
    "  -e, --fade=TIME     Time to spend fading, default is %d\n"
    "  -h, --help          Show this help\n"

    "  -s, --shadow=SHADOW Offset of shadow, default is %d (0 is no shadow)\n"
    "  -S, --shadowalpha=level\n"
    "                      Alpha level of shadow, default is %.2f\n"

    "  -f, --file[=FILE]   Read from file rather than command line (use - or omit FILE for stdin)\n"
    "  -t, --text[=TEXT]   Display TEXT (or omit TEXT for example text)\n"
    "  -m, --maxlen=LEN    Maximum length of any line displayed when -t not specified, default is %lu\n"
    "  -l, --lines=n       Scroll using n lines. Default is %u. \n"
    "  -G, --age=TIME      Time in milliseconds before old scroll lines are discarded,\n"
    "                      use 0 to ignore, default is %u\n"
    "                      (Note that at most min{AGE/DELAY, LINES} will ever be shown at once)\n"
    "\n"
    "  -O, --outline=SHADOW\n"
    "                      Offset of outline, default is 0 which is no outline *\n"
    "  -u, --outlinecolour=colour\n"
    "                      Colour of outline, default is black *\n"
    "  -w, --wait          Delay display even when new lines are ready *\n"
    "\n"
    "  -p, --pos=(top|middle|bottom)\n"
    "                      Display at top/middle/bottom of screen. Top is default *\n"
    "  -A, --align=(left|right|center)\n"
    "                      Display at left/right/center of screen.Left is default *\n"
    "  -o, --offset=OFFSET Vertical Offset *\n"
    "  -i, --indent=OFFSET Horizontal Offset *\n"
    "\n"
    "  -B, --barmode=(percentage|slider)\n"
    "                      Lets you display a percentage or slider bar instead of just text.\n"
    "                      Options may be 'percentage' or 'slider'.\n"
    "                      Disregards any text or files when used.\n"
    "                      When this option is used, the following options are also valid. *\n"
    "  -P, --percentage=percentage\n"
    "                      The length of the percentage bar / slider position (0 to 100). *\n"
    "  -T, --bartext=text  The text to get displayed above the percentage bar. *\n"
    "\n"
    "\n"
    "* indicates not yet implemented\n"
    "TEXT defaults to \"%s\"\n";

static void print_usage(const char* argv1)
{
  printf(USAGE, argv1,
      VERSION, REVISION, BUILD_TIME,
      FONTDESC,
      RED, GREEN, BLUE, ALPHA,
      XPOS, YPOS,
      TIME_MS, FADE_MS,
      SHADOW_OFFSET, SHADOW_ALPHA,
      MAXLINE, LINES, AGE_MS,
      EXAMPLE_TEXT);
}

static int flag;
#define todo &flag

static const char*
getarg(int argc, char* argv[], int* optind, char* optarg)
{
  if (optarg)
    return optarg;

  if (*optind >= argc)
    return NULL;

  if (argv[*optind][0] != '-' ||
      argv[*optind][1] == '\0')
    return argv[(*optind)++];

  return NULL;
}

static double
getflt(const char* optarg, int* ok, char arg)
{
  char* endptr;
  double v = strtod(optarg, &endptr);

  if (*endptr)
  {
    *ok = 0;
    fprintf(stderr, "Argument to -%c must be numeric (arg = %s)\n", arg, optarg);
  }

  return v;
}

static long int
getnum(const char* optarg, int* ok, char arg)
{
  char* endptr;
  long int v = strtol(optarg, &endptr, 10);

  if (*endptr)
  {
    *ok = 0;
    fprintf(stderr, "Argument to -%c must be an integer (arg = %s)\n", arg, optarg);
  }

  return v;
}

static char*
concatargs(char** argv, int i, const int n, const char* start, const char sep)
{
  static char NULSTR[] = {'\0'};
  size_t sz = start ? strlen(start) + 1 : 0;
  char* ret;
  char* p;
  int j;

  if (i >= n)
    return NULSTR;

  for (j = i; j < n; ++j)
    sz += strlen(argv[j]) + 1; /* ' ' */

  ret = (char*)malloc(sz * sizeof(char));
  p = ret;

  if (start)
  {
    strcpy(p, start);
    p += strlen(start);
    *p++ = sep;
  }

  for (j = i; j < n; ++j)
  {
    strcpy(p, argv[j]);
    p += strlen(argv[j]);
    *p++ = sep;
  }

  *--p = '\0'; /* overwrite last space */
  return ret;
}

static int
parse_opts(int argc, char* argv[])
{
  int c;
  int ok = 1;

  while (1)
  {
    /* int this_option_optind = optind ? optind : 1; */
    int option_index = 0;
    static struct option long_options[] =
    {
      /* replace todo with 0 when implmeneted */
      /* note this is
       * struct option {
       *    const char *name; //name of the long option
       *    int has_arg;      //in {0: no argument, 1: required arg, 2: optional arg}
       *    int *flag;        //IF NULL geopt_long returns val ELSE returns 0 and *flag = val
       *    int val;          //value to return or value to assign to *flag
       * };
       */
      {"age", 1, 0, 'G'},
      {"pos", 1, todo, 'p'},
      {"align", 1, todo, 'A'},
      {"fontdesc", 1, 0, 'F'},
      {"file", 2, 0, 'f'},
      {"maxlen", 1, 0, 'm'},
      {"text", 2, 0, 't'},
      {"red", 1, 0, 'r'},
      {"green", 1, 0, 'g'},
      {"blue", 1, 0, 'b'},
      {"alpha", 1, 0, 'a'},
      {"xpos", 1, 0, 'x'},
      {"ypos", 1, 0, 'y'},
      {"delay", 1, 0, 'd'},
      {"fade", 1, 0, 'e'},
      {"offset", 1, todo, 'o'},
      {"indent", 1, todo, 'i'},
      {"help", 0, 0, 'h'},
      {"shadow", 1, 0, 's'},
      {"shadowalpha", 1, 0, 'S'},
      {"outline", 1, todo, 'O'},
      {"outlinecolor", 1, todo, 'u'},
      {"wait", 0, todo, 'w'},
      {"lines", 1, 0, 'l'},
      {"barmode", 1, todo, 'B'},
      {"percentage", 1, todo, 'P'},
      {"bartext", 1, todo, 'T'},
      {0, 0, 0, 0}
    };

    c = getopt_long(argc, argv,
        "G:p:A:F:m:f::t::r:g:b:a:x:y:d:e:o:i:hs:S:O:u:wl:B:P:T:",
        long_options, &option_index);

    if (c == -1)
      break;

    switch (c)
    {
      case 0:
        printf ("NOT IMPLEMENTED: option %s", long_options[option_index].name);
        if (optarg)
          printf (" with arg %s", optarg);
        printf ("\n");
        break;

      case 'F':
        FONTDESC = optarg;
        break;

      case 'r':
        RED = getflt(optarg, &ok, c);
        break;
      case 'g':
        GREEN = getflt(optarg, &ok, c);
        break;
      case 'b':
        BLUE = getflt(optarg, &ok, c);
        break;
      case 'a':
        ALPHA = getflt(optarg, &ok, c);
        break;

      case 'm':
        MAXLINE = getnum(optarg, &ok, c);
        break;
      case 'l':
        LINES = getnum(optarg, &ok, c);
        break;
      case 'G':
        AGE_MS = getnum(optarg, &ok, c);
        break;

      case 'f':
        {
          const char* arg = getarg(argc, argv, &optind, optarg);

          if (arg && arg[0] != '-')
          {
            INFILE = fopen(arg, "r");
            if (!INFILE)
            {
              fprintf(stderr, "Couldn't open %s\n", arg);
              ok = 0;
            }
          }
          else
          {
            INFILE = stdin;
            fprintf(stderr, "Reading from stdin (f-arg = %s)\n", arg);
          }
        }
        break;
      case 't':
        TEXT = getarg(argc, argv, &optind, optarg);
        if (!TEXT)
          TEXT = EXAMPLE_TEXT;
        break;

      case 'x':
        XPOS = getnum(optarg, &ok, c);
        break;
      case 'y':
        YPOS = getnum(optarg, &ok, c);
        break;

      case 'd':
        TIME_MS = getnum(optarg, &ok, c);
        break;
      case 'e':
        FADE_MS = getnum(optarg, &ok, c);
        break;

      case 'h':
        print_usage(argv[0]);
        ok = 0;
        break;

      case 's':
        SHADOW_OFFSET = getnum(optarg, &ok, c);
        break;
      case 'S':
        SHADOW_ALPHA = getflt(optarg, &ok, c);
        break;

      case ':':
        printf ("Option %s requires an argument\n", argv[optind]);
        ok = 0;
        break;

      case '?':
        printf ("Unknown option: %s\n", argv[optind]);
        ok = 0;
        break;

      default:
        printf ("?? getopt returned character code 0%o ??\n", c);
        ok = 0;
    }
  }

  if (optind < argc)
  {
    TEXT = concatargs(argv, optind, argc, TEXT, ' ');
  }
  else if (!TEXT && !INFILE && ok)
  {
    INFILE = stdin;
    fprintf(stderr, "Reading from stdin..\n");
  }

  return ok;
}

static void
render(cairo_t* cr, void* data)
{
  PangoLayout* layout = data;

  if (SHADOW_OFFSET)
  {
    /* drop shadow! */
    cairo_set_source_rgba(cr, SHADOW_RED, SHADOW_GREEN, SHADOW_BLUE, SHADOW_ALPHA);
    cairo_move_to(cr, SHADOW_OFFSET, SHADOW_OFFSET);
    pango_cairo_show_layout(cr, layout);
  }

  /* and the actual text. */
  cairo_set_source_rgba(cr, RED, GREEN, BLUE, ALPHA);
  cairo_move_to(cr, 0, 0);
  pango_cairo_show_layout(cr, layout);
}

static int
do_render(PangoLayout* layout, int unref_layout)
{
  PangoRectangle ink_rect;
  static Aosd* aosd = NULL;

  if (!layout)
    return 1;

  pango_layout_get_pixel_extents(layout, &ink_rect, NULL);
  {
    /* \note: issues if SHADOW_OFFSET is negative */
    const int width = ink_rect.x + ink_rect.width + abs(SHADOW_OFFSET) + RECT_PADDING;
    const int height = ink_rect.y + ink_rect.height + abs(SHADOW_OFFSET) + RECT_PADDING;

    if (!aosd)
    {
      aosd = aosd_new();
      aosd_set_transparency(aosd, TRANSPARENCY_COMPOSITE);
    }

    aosd_set_position_with_offset(aosd, COORDINATE_MAXIMUM, COORDINATE_MAXIMUM,
        width, height, XPOS, YPOS);
  }

  aosd_set_renderer(aosd, render, layout);
  aosd_flash(aosd, FADE_MS, TIME_MS, FADE_MS);

  /* Why do we have to do it again??
   * A: Otherwise, the area below the popup remains the background at the time
   * the text was rendered until we draw more text!
   * Drawing a 1x1 pixel for 0ms is a hack. And you still have that one-pixel dot!
   * Unforunately, it needs to be on-screen otherwise we get an X-Window error.
   */
  aosd_set_geometry(aosd, 0, 0, 1, 1);
  aosd_flash(aosd, 0, 0, 0);
  aosd_hide(aosd); /* doesn't seem to do anything to fix */
  /* aosd_destroy(aosd); */ /* ditto -- does nothing to remove the pixel */

  if (unref_layout)
    g_object_unref(layout);

  return 0;
}

static PangoLayout*
make_layout(PangoContext* context, const char* markup)
{
  PangoLayout* layout = pango_layout_new(context);

  pango_layout_set_markup(layout, markup, -1);

  return layout;
}

static PangoContext*
make_context(void)
{
  return pango_cairo_font_map_create_context(
      PANGO_CAIRO_FONT_MAP(pango_cairo_font_map_get_default()));
}

static int
do_string(PangoContext* context, const char* text)
{
  char* markup = NULL;

  if (asprintf(&markup,
        "<span font_desc='%s'>"
        "%s"
        "</span>",
        FONTDESC, text) < 0)
  {
    return 1;
  }

  do_render(make_layout(context, markup), 1);
  free(markup);

  return 0;
}

static void
do_age(char** scroll, struct timeval* stamp, const size_t n, const char* insert)
{
  struct timeval now;
  int i;

  gettimeofday(&now, NULL);
  free(scroll[0]);

  for (i = 1; i < LINES; ++i)
  {
    if (AGE_MS && scroll[i][0] &&
        ((now.tv_sec - stamp[i].tv_sec) * 1000 +
         (now.tv_usec - stamp[i].tv_usec)/1000) > AGE_MS)
    {
      free(scroll[i]);
      scroll[i-1] = strdup("");
    }
    else
    {
      scroll[i-1] = scroll[i];
      stamp[i-1] = stamp[i];
    }
  }

  scroll[LINES-1] = strdup(insert);
  stamp[LINES-1] = now;
}

static int
do_input(void)
{
  PangoContext* context;
  char* buf = (char*)malloc(sizeof(char) * MAXLINE + 2);
  char** scroll = LINES > 1 ? (char**)malloc(sizeof(char*) * LINES) : NULL;
  struct timeval* stamp = LINES > 1 ? (struct timeval*)malloc(sizeof(struct timeval) * LINES) : NULL;
  int i;

  MAXLINE += 1;
  if (scroll && stamp)
  {
    struct timeval now;
    if (gettimeofday(&now, NULL))
    {
      /* error! */
    }

    for (i = 0; i < LINES; ++i)
    {
      scroll[i] = strdup("");
      stamp[i] = now;
    }
  }
  else if (LINES > 1)
  {
    fprintf(stderr, "Couldn't allocate scrollback buffer\n");
    return 1;
  }

  if (!buf)
  {
    fprintf(stderr, "Couldn't allocate a buffer of size %lu\n", MAXLINE + 1);
    return 1;
  }

  if (!(context = make_context()))
  {
    fprintf(stderr, "Couldn't create pango context\n");
    return 1;
  }

  while (fgets(buf, MAXLINE, INFILE))
  {
    char* end = buf + strlen(buf) - 1;

    if (*end == '\n')
    { /* got a full line; replace \n with \0 */
      *end = '\0';
    }
    else
    { /* no full line, append continuation char */
      int nxt = fgetc(INFILE);
      if (nxt != '\n' && nxt != EOF)
      {
        strcat(end + 1, "\\");
        ungetc(nxt, INFILE);
      }
    }

    if (scroll)
    {
      char* cat;
      do_age(scroll, stamp, LINES, buf);
      cat = concatargs(scroll, 0, LINES, 0, '\n');
      do_string(context, cat);
      free(cat);
    }
    else
      do_string(context, buf);
  }

  g_object_unref(context);
  /* moving the above line into the while loop and creating each time
   * doesn't seem to fix the redraw problem */

  free(buf);
  return 0;
}

int
main(int argc, char* argv[])
{
  if (!parse_opts(argc, argv))
    return 1;

  g_type_init();

  if (TEXT && !INFILE)
    do_string(make_context(), TEXT);
  else
    do_input();

  return 0;
}

/* vim: set ts=2 sw=2 et : */
