/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2008 Eugene Paskevich <eugene@raptor.kiev.ua>
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <aosd-text.h>

#include "aosd_cat.h"

static gboolean
parse_options(int* argc, char** argv[])
{
  GOptionContext* ctx = NULL;
  GOptionGroup* group = NULL;
  GSList* opt_list = NULL;
  gboolean ret = TRUE;
  gchar* str = NULL;
  guint n = 0;

  START_CATCH;

#define ADD_ELEM(data) \
  CATCH(ret, "Memory allocation error", data != NULL); \
  opt_list = g_slist_append(opt_list, data)
#define ADD_NUMB(fmt, var) \
  str = g_strdup_printf("%"fmt, config.var); \
  ADD_ELEM(str)
#define ADD_STRN(var) \
  str = g_strdup(((config.var) == NULL ? "None" : (config.var))); \
  ADD_ELEM(str)

  ADD_NUMB("u", back_alpha);
  ADD_NUMB("u", shadow_alpha);
  ADD_NUMB("u", fore_alpha);
  ADD_NUMB("u", position);
  ADD_NUMB(G_GINT16_FORMAT, x_offset);
  ADD_NUMB(G_GINT16_FORMAT, y_offset);
  ADD_NUMB("i", shadow_offset);
  ADD_NUMB("u", padding);
  ADD_STRN(back_color);
  ADD_STRN(shadow_color);
  ADD_STRN(fore_color);
  ADD_NUMB(G_GUINT16_FORMAT, fade_in);
  ADD_NUMB(G_GUINT16_FORMAT, fade_full);
  ADD_NUMB(G_GUINT16_FORMAT, fade_out);
#undef ADD_ELEM
#undef ADD_NUMB
#undef ADD_STRN

#define DEF (gchar*)g_slist_nth_data(opt_list, n++)
#define OPT_INT(longname, shortname, configname, desc) \
  { longname, shortname, 0, G_OPTION_ARG_INT, &config.configname, "Sets the " desc, DEF }
#define OPT_STR(longname, shortname, configname, desc) \
  { longname, shortname, 0, G_OPTION_ARG_STRING, &config.configname, "Sets the " desc, DEF }

  GOptionEntry opacity[] =
  {
    OPT_INT("back-opacity", 'b', back_alpha, "background opacity. **"),
    OPT_INT("shadow-opacity", 's', shadow_alpha, "shadow opacity. **"),
    OPT_INT("fore-opacity", 'f', fore_alpha, "foreground opacity. **"),
    { NULL }
  };

  GOptionEntry geometry[] =
  {
    OPT_INT("position", 'p', position, "OSD window position. *"),
    OPT_INT("x-offset", 'x', x_offset, "x-axis window offset."),
    OPT_INT("y-offset", 'y', y_offset, "y-axis window offset."),
    OPT_INT("shadow-offset", 'e', shadow_offset, "shadow offset."),
    OPT_INT("padding", 'd', padding, "margin from the edge to the contents. **"),
    { NULL }
  };

  GOptionEntry coloring[] =
  {
    OPT_STR("back-color", 't', back_color, "background color."),
    OPT_STR("shadow-color", 'w', shadow_color, "shadow color."),
    OPT_STR("fore-color", 'r', fore_color, "foreground color."),
    { NULL }
  };

  GOptionEntry timing[] =
  {
    OPT_INT("fade-in", 'i', fade_in, "fade in time."),
    OPT_INT("fade-full", 'u', fade_full, "time to show with full opacity."),
    OPT_INT("fade-out", 'o', fade_out, "fade out time."),
    { NULL }
  };

#undef DEF
#undef OPT_INT

  CATCH(ret, "Unable to allocate option context",
      (ctx = g_option_context_new("")) != NULL);

  g_option_context_set_ignore_unknown_options(ctx, FALSE);
  g_option_context_set_summary(ctx,
      "Default values are specified after the equal sign.");
  g_option_context_set_description(ctx,
      "* Valid range is 0-8, where 0 is top-left corner and 8 is bottom-right corner.\n"
      "** Valid range is 0-255.\n"
      "\n"
      "Thank you for using libaosd!\n"
      "Please send bug reports to: http://bugzilla.atheme.org/");

#define ADD_GROUP(name, desc) \
  group = g_option_group_new(#name, desc " Options:", \
      "Show " #name " help options", NULL, NULL); \
  CATCH(ret, "Unable to create an option group", group != NULL); \
  g_option_group_add_entries(group, name); \
  g_option_context_add_group(ctx, group)

  ADD_GROUP(geometry, "Geometry");
  ADD_GROUP(opacity, "Opacity");
  ADD_GROUP(coloring, "Coloring");
  ADD_GROUP(timing, "Timing");
#undef ADD_GROUP

  CATCH(ret, "Error parsing options, try --help.",
      g_option_context_parse(ctx, argc, argv, NULL));

  CATCH(ret, "Invalid position argument specified.", config.position <= 8);
  END_CATCH;

  if (ctx != NULL)
    g_option_context_free(ctx);
  if (opt_list != NULL)
    g_slist_free(opt_list);

  return ret;
}

int
main(int argc, char* argv[])
{
  if (argc <= 1)
    FATAL("No command-line options specified...");
  if (parse_options(&argc, &argv) == FALSE)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

/* vim: set ts=2 sw=2 et : */
