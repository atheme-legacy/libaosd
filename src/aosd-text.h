/* aosd -- OSD with transparency, cairo, and pango.
 *
 * Copyright (C) 2006 Evan Martin <martine@danga.com>
 */

#ifndef __AOSD_TEXT_H__
#define __AOSD_TEXT_H__

#include <pango/pango-layout.h>

Aosd* aosd_text_new(const char* markup, int x, int y);

#endif /* __AOSD_TEXT_H__ */

/* vim: set ts=2 sw=2 et cino=(0 : */
