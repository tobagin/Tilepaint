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
 * DEPRECATED: We now allow any visual solution that obeys row/col counts.
 * The internal tile structure is ignored for victory.
 */

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
    }
  }

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
    }
  }

  return success;
}

gboolean tilepaint_check_win(TilepaintApplication *tilepaint) {
  /* Check all rules (Rule 1 is now deprecated) */
  if (tilepaint_check_rule2(tilepaint) && tilepaint_check_rule3(tilepaint)) {

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
