/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Tilepaint
 * Copyright (C) Thiago Fernandes 2026 <thiago@example.com>
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

#ifndef TILEPAINT_SCORE_H
#define TILEPAINT_SCORE_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct _TilepaintApplication Tilepaint;

typedef struct {
  guint board_size;
  gchar *name;
  guint time;
} TilepaintScore;

void tilepaint_score_free(TilepaintScore *score);
GList *tilepaint_score_get_top_scores(Tilepaint *tilepaint, guint board_size);
gboolean tilepaint_score_is_high_score(Tilepaint *tilepaint, guint board_size, guint time);
void tilepaint_score_add(Tilepaint *tilepaint, guint board_size, const gchar *name,
                    guint time);

G_END_DECLS

#endif /* TILEPAINT_SCORE_H */
