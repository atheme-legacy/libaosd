/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2007 Eugene Paskevich <eugene@raptor.kiev.ua>
 */

#include <string.h>

#include "aosd-text.h"

PangoLayout*
pango_layout_new_aosd()
{
  return pango_layout_new(pango_cairo_font_map_create_context(
        PANGO_CAIRO_FONT_MAP(pango_cairo_font_map_get_default())));
}

void
pango_layout_unref_aosd(PangoLayout* lay)
{
  if (lay == NULL)
    return;

  g_object_unref(pango_layout_get_context(lay));
  g_object_unref(lay);
}

void
pango_layout_get_size_aosd(PangoLayout* lay,
    unsigned* width, unsigned* height, int* lbearing)
{
  if (lay == NULL)
    return;

  PangoRectangle ink;
  pango_layout_get_pixel_extents(lay, &ink, NULL);

  if (width != NULL)
    *width = ink.width;
  if (height != NULL)
    *height = PANGO_DESCENT(ink);
  if (lbearing != NULL)
    *lbearing = -ink.x;
}

void
pango_layout_set_attr_aosd(PangoLayout* lay, PangoAttribute* attr)
{
  if (lay == NULL || attr == NULL)
    return;

  if (attr->start_index == 0 && attr->end_index == 0)
    attr->end_index = strlen(pango_layout_get_text(lay));

  PangoAttrList* attrs = pango_layout_get_attributes(lay);
  if (attrs == NULL)
    attrs = pango_attr_list_new();
  pango_attr_list_change(attrs, attr);
  pango_layout_set_attributes(lay, attrs);
}

void
pango_layout_set_font_aosd(PangoLayout* lay, char* font_desc)
{
  if (lay == NULL || font_desc == NULL)
    return;

  pango_layout_set_font_description(lay,
      pango_font_description_from_string(font_desc));
}

static gboolean
filter_for_bg(PangoAttribute* attr, gpointer data)
{
  switch (attr->klass->type)
  {
    case PANGO_ATTR_FOREGROUND:
    case PANGO_ATTR_BACKGROUND:
    case PANGO_ATTR_UNDERLINE_COLOR:
    case PANGO_ATTR_STRIKETHROUGH_COLOR:
      return FALSE;

    default:
      return TRUE;
  }
}

static gboolean
filter_for_fg(PangoAttribute* attr, gpointer data)
{
  switch (attr->klass->type)
  {
    case PANGO_ATTR_FOREGROUND:
    case PANGO_ATTR_BACKGROUND:
      return FALSE;

    default:
      return TRUE;
  }
}

void
aosd_text_renderer(cairo_t* cr, TextRenderData* data)
{
  PangoLayout* lay = NULL;
  PangoAttrList* attrs = pango_layout_get_attributes(data->lay);
  PangoAttrList* new_attrs = NULL;
  PangoColor col = {0, 0, 0};

  // Draw background
  if (data->back.color != NULL && data->back.opacity != 0)
  {
    pango_color_parse(&col, data->back.color);
    cairo_set_source_rgb(cr,
        col.red   / (double)65535,
        col.green / (double)65535,
        col.blue  / (double)65535);
    cairo_paint_with_alpha(cr, data->back.opacity / (double)255);

    col = (PangoColor){0, 0, 0};
  }

  // Drop the shadow
  if (data->shadow.opacity != 0 &&
      (data->shadow.x_offset != 0 || data->shadow.y_offset != 0))
  {
    if (attrs != NULL)
    {
      lay = pango_layout_copy(data->lay);
      new_attrs = pango_attr_list_filter(attrs, filter_for_bg, NULL);
      pango_layout_set_attributes(lay, new_attrs);
    }
    else
      lay = data->lay;

    if (data->shadow.color != NULL)
      pango_color_parse(&col, data->shadow.color);
    cairo_set_source_rgba(cr,
        col.red     / (double)65535,
        col.green   / (double)65535,
        col.blue    / (double)65535,
        data->shadow.opacity / (double)255);

    cairo_move_to(cr, data->shadow.x_offset + data->lbearing, data->shadow.y_offset);
    pango_cairo_show_layout(cr, lay);

    if (new_attrs != NULL)
      pango_attr_list_unref(new_attrs);
    if (attrs != NULL)
      g_object_unref(lay);

    lay = NULL;
    new_attrs = NULL;
    col = (PangoColor){0, 0, 0};
  }

  // And finally the foreground
  if (data->fore.opacity != 0)
  {
    if (attrs != NULL)
    {
      lay = pango_layout_copy(data->lay);
      new_attrs = pango_attr_list_filter(attrs, filter_for_fg, NULL);
      pango_layout_set_attributes(lay, new_attrs);
    }
    else
      lay = data->lay;

    if (data->fore.color != NULL)
      pango_color_parse(&col, data->fore.color);
    cairo_set_source_rgba(cr,
        col.red   / (double)65535,
        col.green / (double)65535,
        col.blue  / (double)65535,
        data->fore.opacity / (double)255);

    cairo_move_to(cr, data->lbearing, 0);
    pango_cairo_show_layout(cr, lay);

    if (new_attrs != NULL)
      pango_attr_list_unref(new_attrs);
    if (attrs != NULL)
      g_object_unref(lay);

    lay = NULL;
    new_attrs = NULL;
    col = (PangoColor){0, 0, 0};
  }
}

/* vim: set ts=2 sw=2 et : */
