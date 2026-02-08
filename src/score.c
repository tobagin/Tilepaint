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

#include "config.h"
#include "main.h"
#include <glib/gi18n.h>

#include "score.h"

void tilepaint_score_free(TilepaintScore *score) {
  g_free(score->name);
  g_free(score);
}

static gint compare_scores(gconstpointer a, gconstpointer b) {
  const TilepaintScore *score_a = (const TilepaintScore *)a;
  const TilepaintScore *score_b = (const TilepaintScore *)b;

  if (score_a->time < score_b->time)
    return -1;
  else if (score_a->time > score_b->time)
    return 1;

  return 0;
}

GList *tilepaint_score_get_top_scores(Tilepaint *tilepaint, guint board_size) {
  GVariant *variant;
  GVariantIter iter;
  guint size, time;
  gchar *name = NULL;
  GList *scores = NULL;

  variant = g_settings_get_value(tilepaint->settings, "high-scores");
  g_variant_iter_init(&iter, variant);

  while (g_variant_iter_loop(&iter, "(usu)", &size, &name, &time)) {
    if (size == board_size) {
      TilepaintScore *score = g_new0(TilepaintScore, 1);
      score->board_size = size;
      score->name = g_strdup(name);
      score->time = time;
      scores = g_list_prepend(scores, score);
    }
  }

  g_variant_unref(variant);

  scores = g_list_sort(scores, compare_scores);

  /* Keep only top 10 */
  /* Since we're just reading here, we return all of them, but the saving logic
     ensures top 10. Wait, if we change board size strategy we might have more.
     Let's clamp here too just in case. */

  /* Actually for display we might want to ensure it's limited,
     but typically we limit on write. */

  return scores;
}

gboolean tilepaint_score_is_high_score(Tilepaint *tilepaint, guint board_size, guint time) {
  GList *scores = tilepaint_score_get_top_scores(tilepaint, board_size);
  guint count = g_list_length(scores);
  gboolean is_high = FALSE;

  if (count < 10) {
    is_high = TRUE;
  } else {
    TilepaintScore *last = (TilepaintScore *)g_list_last(scores)->data;
    if (time < last->time)
      is_high = TRUE;
  }

  g_list_free_full(scores, (GDestroyNotify)tilepaint_score_free);
  return is_high;
}

void tilepaint_score_add(Tilepaint *tilepaint, guint board_size, const gchar *name,
                    guint time) {
  GVariant *variant;
  GVariantIter iter;
  guint size, t;
  gchar *n = NULL;
  GVariantBuilder builder;
  GList *scores = NULL;
  GList *l;

  /* Load existing scores for ALL sizes, but parsing them into a list structure
     might be complex if we want to preserve other sizes.
     Actually, GVariantBuilder makes this easier. We can iterate the existing
     ones, parse the ones matching our board_size into a list, add the new one,
     sort, trim, and then rebuild the GVariant with the new list + the untouched
     ones from other sizes. */

  /* Approach:
     1. Read all scores.
     2. Append scores NOT matching current board_size directly to builder.
     3. Collect scores matching current board_size into a GList.
     4. Add new score to GList.
     5. Sort GList.
     6. Trim GList to 10.
     7. Append GList items to builder.
     8. Save.
  */

  variant = g_settings_get_value(tilepaint->settings, "high-scores");
  g_variant_builder_init(&builder, G_VARIANT_TYPE("a(usu)"));
  g_variant_iter_init(&iter, variant);

  /* Step 1 & 2 & 3 */
  while (g_variant_iter_loop(&iter, "(usu)", &size, &n, &t)) {
    if (size != board_size) {
      g_variant_builder_add(&builder, "(usu)", size, n, t);
    } else {
      TilepaintScore *s = g_new0(TilepaintScore, 1);
      s->board_size = size;
      s->name = g_strdup(n);
      s->time = t;
      scores = g_list_prepend(scores, s);
    }
  }

  g_variant_unref(variant);

  /* Step 4 */
  TilepaintScore *new_score = g_new0(TilepaintScore, 1);
  new_score->board_size = board_size;
  new_score->name = g_strdup(name);
  new_score->time = time;
  scores = g_list_prepend(scores, new_score);

  /* Step 5 */
  scores = g_list_sort(scores, compare_scores);

  /* Step 6 */
  while (g_list_length(scores) > 10) {
    GList *last = g_list_last(scores);
    tilepaint_score_free((TilepaintScore *)last->data);
    scores = g_list_delete_link(scores, last);
  }

  /* Step 7 */
  for (l = scores; l != NULL; l = l->next) {
    TilepaintScore *s = (TilepaintScore *)l->data;
    g_variant_builder_add(&builder, "(usu)", s->board_size, s->name, s->time);
  }

  g_list_free_full(scores, (GDestroyNotify)tilepaint_score_free);

  /* Step 8 */
  g_settings_set_value(tilepaint->settings, "high-scores",
                       g_variant_builder_end(&builder));
}
