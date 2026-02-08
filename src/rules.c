/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Tilepaint
 * Copyright (C) Philip Withnall 2007-2009 <philip@tecnocode.co.uk>
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

#include <adwaita.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gprintf.h>

#include "main.h"
#include "rules.h"

/* Rule 1: Tile Consistency
 * All cells within the same tile must share the same painted status.
 */
gboolean tilepaint_check_rule1(TilepaintApplication *tilepaint) {
  /* This rule is largely enforced by the UI, but we check it here for
     completeness. We iterate over all cells, and for each tile_id, we ensure
     all cells with that ID have the same CELL_PAINTED status. */

  /* A simple way is to store the status of the first cell found for each
     tile_id, and check subsequent cells against it. */

  /* Since tile_ids are arbitrary, we can map them or just do a nested loop
     check? Actually, since board size is small (up to 10x10), we can just
     iterate. But wait, tile_ids are likely 0 to N. We can use an array. */

  /* Let's assume tile_ids are < board_size * board_size. */
  gint max_tiles = tilepaint->board_size * tilepaint->board_size;
  gint *tile_status =
      g_new0(gint, max_tiles); /* 0: unknown, 1: unpainted, 2: painted */
  gboolean success = TRUE;

  for (int x = 0; x < tilepaint->board_size; x++) {
    for (int y = 0; y < tilepaint->board_size; y++) {
      int tid = tilepaint->board[x][y].tile_id;
      gboolean is_painted = (tilepaint->board[x][y].status & CELL_PAINTED) != 0;
      int current_status = is_painted ? 2 : 1;

      if (tile_status[tid] == 0) {
        tile_status[tid] = current_status;
      } else if (tile_status[tid] != current_status) {
        /* Inconsistency found */
        success = FALSE;
        /* Mark error? ideally we mark the specific cell, but for now just fail
         * rule */
      }
    }
  }

  g_free(tile_status);

  if (tilepaint->debug && success)
    g_debug("Rule 1 (Tile Consistency) OK");
  else if (tilepaint->debug)
    g_debug("Rule 1 (Tile Consistency) Failed");

  return success;
}

/* Rule 2: Row Counts
 * The number of painted cells in each row must match the row clue.
 */
gboolean tilepaint_check_rule2(TilepaintApplication *tilepaint) {
  gboolean success = TRUE;

  for (int y = 0; y < tilepaint->board_size; y++) {
    int count = 0;
    for (int x = 0; x < tilepaint->board_size; x++) {
      if (tilepaint->board[x][y].status & CELL_PAINTED) {
        count++;
      }
    }

    if (count != tilepaint->row_clues[y]) {
      success = FALSE;
      /* Could mark row error here if we had a way to visualize it */
      if (tilepaint->debug)
        g_debug("Rule 2 failed at row %d: expected %d, got %d", y,
                tilepaint->row_clues[y], count);
    }
  }

  if (tilepaint->debug && success)
    g_debug("Rule 2 (Row Counts) OK");

  return success;
}

/* Rule 3: Column Counts
 * The number of painted cells in each column must match the column clue.
 */
gboolean tilepaint_check_rule3(TilepaintApplication *tilepaint) {
  gboolean success = TRUE;

  for (int x = 0; x < tilepaint->board_size; x++) {
    int count = 0;
    for (int y = 0; y < tilepaint->board_size; y++) {
      if (tilepaint->board[x][y].status & CELL_PAINTED) {
        count++;
      }
    }

    if (count != tilepaint->col_clues[x]) {
      success = FALSE;
      if (tilepaint->debug)
        g_debug("Rule 3 failed at col %d: expected %d, got %d", x,
                tilepaint->col_clues[x], count);
    }
  }

  if (tilepaint->debug && success)
    g_debug("Rule 3 (Column Counts) OK");

  return success;
}

gboolean tilepaint_check_win(TilepaintApplication *tilepaint) {
  /* Check all rules */
  if (tilepaint_check_rule1(tilepaint) && tilepaint_check_rule2(tilepaint) &&
      tilepaint_check_rule3(tilepaint)) {

    /* Win! */
    tilepaint_disable_events(tilepaint);

    if (tilepaint_score_is_high_score(tilepaint, tilepaint->board_size,
                                      tilepaint->timer_value)) {
      /* New High Score! */
      tilepaint_show_new_high_score_dialog(tilepaint);
    } else {
      /* Standard Win */
      tilepaint_show_win_dialog(tilepaint);
    }
    return TRUE;
  }

  return FALSE;
}
