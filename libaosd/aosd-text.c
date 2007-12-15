/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 */

#include "config.h"

#include <pango/pangocairo.h>

#include "aosd-internal.h"
#include "aosd-text.h"

static gboolean
filter(PangoAttribute* attr, gpointer data)
{
  switch (attr->klass->type)
  {
    /* Attributes we don't want to be replicated */
    case PANGO_ATTR_FOREGROUND:
    case PANGO_ATTR_BACKGROUND:
    case PANGO_ATTR_UNDERLINE:
    case PANGO_ATTR_UNDERLINE_COLOR:
    case PANGO_ATTR_STRIKETHROUGH:
    case PANGO_ATTR_STRIKETHROUGH_COLOR:
      return FALSE;

    default:
      return TRUE;
  }
}

static void
render_text(cairo_t* cr, void* data)
{
  PangoLayout* layout = data;

  /* get a copy of the layout and filter its attributes */
  PangoLayout* back = pango_layout_copy(layout);
  PangoAttrList* attrs = pango_layout_get_attributes(back);
  PangoAttrList* new_attrs = NULL;
  if (attrs != NULL)
  {
    new_attrs = pango_attr_list_filter(attrs, filter, NULL);
    pango_layout_set_attributes(back, new_attrs);
  }

  /* drop half-opaque shadow */
  cairo_set_source_rgba(cr, 0, 0, 0, 0.5);
  cairo_move_to(cr, 3, 3);
  pango_cairo_show_layout(cr, back);

  /* clean up */
  if (attrs != NULL)
    pango_attr_list_unref(new_attrs);
  g_object_unref(back);

  /* and the actual text */
  cairo_set_source_rgba(cr, 1, 1, 1, 1.0);
  cairo_move_to(cr, 0, 0);
  pango_cairo_show_layout(cr, layout);
}

Aosd*
aosd_new_text(const char* markup, int x, int y)
{
  Aosd* aosd;
  PangoContext* context;
  PangoLayout* layout;
  PangoRectangle ink_rect;

  g_type_init();

  context = pango_cairo_font_map_create_context(
      PANGO_CAIRO_FONT_MAP(pango_cairo_font_map_get_default()));
  layout = pango_layout_new(context);

  pango_layout_set_markup(layout, markup, -1);

  pango_layout_get_pixel_extents(layout, &ink_rect, NULL);

  const int width = ink_rect.x + ink_rect.width + 5;
  const int height = ink_rect.y + ink_rect.height + 5;

  aosd = aosd_new();
  aosd->layout = layout;
  aosd_set_geometry(aosd, x, y, width, height);
  aosd_set_renderer(aosd, render_text, layout, g_object_unref);
  
  return aosd;
}

void
aosd_destroy_text(Aosd* aosd)
{
  if (aosd == NULL || aosd->layout == NULL)
    return;

  g_object_unref(pango_layout_get_context(aosd->layout));
  g_object_unref(aosd->layout);
}

/* vim: set ts=2 sw=2 et : */
