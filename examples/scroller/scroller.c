#include <string.h>

#include <aosd-text.h>

static struct
{
  Aosd* aosd;
  unsigned width, height;
  TextRenderData rend;
} data;

static void
scroll(void)
{
  int x, y, i, pos = 8, step = 3;

  aosd_set_position(data.aosd, pos, data.width, data.height);
  aosd_get_geometry(data.aosd, &x, &y, NULL, NULL);
  aosd_set_position_offset(data.aosd, data.width, data.height);
  aosd_show(data.aosd);

  x -= 1;
  y += data.height - 1;
  for (i = 1; i <= data.height; i += step)
  {
    aosd_loop_for(data.aosd, 20);
    aosd_set_geometry(data.aosd, x, y -= step, data.width, i);
  }

  aosd_set_position(data.aosd, pos, data.width, data.height);
  aosd_set_position_offset(data.aosd, -1, -1);
  aosd_get_geometry(data.aosd, &x, &y, NULL, NULL);
  aosd_loop_for(data.aosd, 2000);

  for (i = data.height; i >= 1; i -= step)
  {
    aosd_set_geometry(data.aosd, x, y += step, data.width, i);
    aosd_loop_for(data.aosd, 20);
  }

  aosd_hide(data.aosd);
}

static void
setup(void)
{
  data.aosd = aosd_new();
  aosd_set_renderer(data.aosd, aosd_text_renderer, &data.rend);
/*  aosd_set_transparency(data.aosd, TRANSPARENCY_COMPOSITE);
  if (aosd_get_transparency(data.aosd) != TRANSPARENCY_COMPOSITE)*/
    aosd_set_transparency(data.aosd, TRANSPARENCY_NONE);

  data.rend.geom.x_offset = 10;
  data.rend.geom.y_offset = 10;

  data.rend.back.color = "white";
  data.rend.back.opacity = 80;

  data.rend.shadow.color = "black";
  data.rend.shadow.opacity = 127;
  data.rend.shadow.x_offset = 2;
  data.rend.shadow.y_offset = 2;

  data.rend.fore.color = "green";
  data.rend.fore.opacity = 255;

  data.rend.lay = pango_layout_new_aosd();
  pango_layout_set_font_aosd(data.rend.lay, "Times New Roman Italic 24");
  pango_layout_set_wrap(data.rend.lay, PANGO_WRAP_WORD_CHAR);
  pango_layout_set_alignment(data.rend.lay, PANGO_ALIGN_RIGHT);
  pango_layout_set_width(data.rend.lay,
      aosd_text_get_screen_wrap_width(data.aosd, &data.rend));
}

static void
set_string(const char* text)
{
  pango_layout_set_text(data.rend.lay, text, -1);
  aosd_text_get_size(&data.rend, &data.width, &data.height);
}

static void
cleanup(void)
{
  aosd_destroy(data.aosd);
  pango_layout_unref_aosd(data.rend.lay);
}

int
main(int argc, char* argv[])
{
  int i;

  g_type_init();

  memset(&data, 0, sizeof(data));

  setup();

  for (i = 1; i < argc; i++)
  {
    set_string(argv[i]);
    scroll();
  }

  cleanup();

  return 0;
}

/* vim: set ts=2 sw=2 et : */
