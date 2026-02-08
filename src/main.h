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

#include <glib.h>
#include <gtk/gtk.h>

#ifndef TILEPAINT_MAIN_H
#define TILEPAINT_MAIN_H

#include "score.h"

G_BEGIN_DECLS

#define DEFAULT_BOARD_SIZE 5
#define MAX_BOARD_SIZE 10

typedef struct {
  guchar x;
  guchar y;
} TilepaintVector;

typedef enum {
  UNDO_NEW_GAME,
  UNDO_PAINT,
  UNDO_TAG1,
  UNDO_TAG2,
  UNDO_TAGS, /* = UNDO_TAG1 and UNDO_TAG2 */
  UNDO_TILE_PAINT
} TilepaintUndoType;

typedef struct _TilepaintUndo TilepaintUndo;
struct _TilepaintUndo {
  TilepaintUndoType type;
  TilepaintVector cell;
  TilepaintUndo *undo;
  TilepaintUndo *redo;
};

typedef enum {
  CELL_PAINTED = 1 << 1,
  CELL_SHOULD_BE_PAINTED = 1 << 2,
  CELL_TAG1 = 1 << 3,
  CELL_TAG2 = 1 << 4,
  CELL_ERROR = 1 << 5
} TilepaintCellStatus;

typedef struct {
  GdkRGBA unpainted_bg;
  GdkRGBA painted_bg;
  GdkRGBA unpainted_border;
  GdkRGBA painted_border;
  GdkRGBA unpainted_text;
  GdkRGBA painted_text;
  GdkRGBA error_text;
} TilepaintTheme;

typedef struct {
  guchar status;
  guchar tile_id;
} TilepaintCell;

#define TILEPAINT_TYPE_APPLICATION (tilepaint_application_get_type())
G_DECLARE_FINAL_TYPE(TilepaintApplication, tilepaint_application, TILEPAINT,
                     APPLICATION, GtkApplication)

struct _TilepaintApplication {
  GtkApplication parent;

  /* FIXME: This should all be merged into priv. */
  GtkWidget *window;
  GtkWidget *preferences_dialog;
  GtkWidget *board_theme_row;
  GtkWidget *board_size_row;
  GtkWidget *drawing_area;
  GSimpleAction *undo_action;
  GSimpleAction *redo_action;
  GSimpleAction *hint_action;

  gdouble drawing_area_width;
  gdouble drawing_area_height;

  gdouble drawing_area_x_offset;
  gdouble drawing_area_y_offset;

  PangoFontDescription *normal_font_desc;
  PangoFontDescription *painted_font_desc;

  guchar board_size;
  TilepaintCell **board;
  guchar row_clues[MAX_BOARD_SIZE];
  guchar col_clues[MAX_BOARD_SIZE];

  gboolean debug;
  gboolean processing_events;
  gboolean made_a_move;
  TilepaintUndo *undo_stack;

  guint hint_status;
  TilepaintVector hint_position;
  guint hint_timeout_id;

  guint timer_value; /* seconds into the game */
  GtkLabel *timer_label;
  guint timeout_id;

  gboolean cursor_active;
  TilepaintVector cursor_position;

  gboolean is_paused;
  GtkWidget *pause_overlay;
  GtkWidget *pause_button;

  const TilepaintTheme *theme;
  GSettings *settings;
};

TilepaintApplication *
tilepaint_application_new(void) G_GNUC_WARN_UNUSED_RESULT G_GNUC_MALLOC;

/* FIXME: Backwards compatibility. This should be phased out eventually. */
typedef TilepaintApplication Tilepaint;

void tilepaint_new_game(Tilepaint *tilepaint, guint board_size);
void tilepaint_clear_undo_stack(Tilepaint *tilepaint);
void tilepaint_set_board_size(Tilepaint *tilepaint, guint board_size);
void tilepaint_print_board(Tilepaint *tilepaint);
void tilepaint_free_board(Tilepaint *tilepaint);
void tilepaint_enable_events(Tilepaint *tilepaint);
void tilepaint_disable_events(Tilepaint *tilepaint);
void tilepaint_start_timer(Tilepaint *tilepaint);
void tilepaint_pause_timer(Tilepaint *tilepaint);
void tilepaint_reset_timer(Tilepaint *tilepaint);
void tilepaint_set_error_position(Tilepaint *tilepaint,
                                  TilepaintVector position);
void tilepaint_quit(Tilepaint *tilepaint);

void tilepaint_show_new_high_score_dialog(Tilepaint *tilepaint);
void tilepaint_show_high_scores_dialog(Tilepaint *tilepaint);
void tilepaint_show_win_dialog(Tilepaint *tilepaint);

G_END_DECLS

#endif /* TILEPAINT_MAIN_H */
