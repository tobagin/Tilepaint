/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Tilepaint
 * Copyright (C) Philip Withnall 2007-2008 <philip@tecnocode.co.uk>
 * 
 * Tilepaint is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tilepaint is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tilepaint.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <cairo/cairo.h>
#include "main.h"

#ifndef TILEPAINT_INTERFACE_H
#define TILEPAINT_INTERFACE_H

G_BEGIN_DECLS

GtkWidget* tilepaint_create_interface (Tilepaint *tilepaint);

G_END_DECLS

#endif /* TILEPAINT_INTERFACE_H */
