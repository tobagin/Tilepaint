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
 * Tilepaint is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tilepaint.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "generator.h"
#include "main.h"
#include "rules.h"

/* Helper to grow a tile */
static void grow_tile(gint **tile_ids, gboolean **solution, guint size, guint x,
                      guint y, gint current_tile_id) {
  /* Randomly try to add neighbors of the same color to this tile */
  /* Uses a simple queue for BFS growth with probability */

  typedef struct {
    guint x;
    guint y;
  } Point;
  Point *queue = g_new(Point, size * size);
  guint q_start = 0;
  guint q_end = 0;

  queue[q_end++] = (Point){x, y};

  while (q_start < q_end) {
    Point p = queue[q_start++];

    /* Check neighbors */
    Point neighbors[4] = {
        {p.x + 1, p.y}, {p.x - 1, p.y}, {p.x, p.y + 1}, {p.x, p.y - 1}};

    for (int i = 0; i < 4; i++) {
      guint nx = neighbors[i].x;
      guint ny = neighbors[i].y;

      if (nx >= size || ny >= size)
        continue;

      if (tile_ids[nx][ny] == -1 && solution[nx][ny] == solution[x][y]) {
        /* 70% chance to merge, preventing huge monolithic tiles */
        if ((rand() % 100) < 70) {
          tile_ids[nx][ny] = current_tile_id;
          queue[q_end++] = (Point){nx, ny};
        }
      }
    }
  }
  g_free(queue);
}

/* Solver context for uniqueness check */
typedef struct {
  guint size;
  guint num_tiles;
  guchar *row_clues;
  guchar *col_clues;
  int **tile_row_map; /* tile_id -> row -> count */
  int **tile_col_map; /* tile_id -> col -> count */
  int *current_row_sums;
  int *current_col_sums;
  int *remaining_row_tiles_sum; /* potential left in unassigned tiles for this
                                   row */
  int *remaining_col_tiles_sum;
  int solutions_found;
} SolverCtx;

static void solve_recursive(SolverCtx *ctx, guint tile_idx) {
  if (ctx->solutions_found > 1)
    return;

  if (tile_idx == ctx->num_tiles) {
    /* All tiles assigned, verify all clues matched (pruning should have handled
     * this, but check anyway) */
    for (guint i = 0; i < ctx->size; i++) {
      if (ctx->current_row_sums[i] != ctx->row_clues[i] ||
          ctx->current_col_sums[i] != ctx->col_clues[i])
        return;
    }
    ctx->solutions_found++;
    return;
  }

  /* Try Unpainted (0) */
  gboolean possible = TRUE;
  for (guint i = 0; i < ctx->size; i++) {
    /* If we DON'T paint this tile, can we still reach the required row/col
     * clues? */
    ctx->remaining_row_tiles_sum[i] -= ctx->tile_row_map[tile_idx][i];
    ctx->remaining_col_tiles_sum[i] -= ctx->tile_col_map[tile_idx][i];

    if (ctx->current_row_sums[i] + ctx->remaining_row_tiles_sum[i] <
            ctx->row_clues[i] ||
        ctx->current_col_sums[i] + ctx->remaining_col_tiles_sum[i] <
            ctx->col_clues[i]) {
      possible = FALSE;
    }
  }

  if (possible) {
    solve_recursive(ctx, tile_idx + 1);
  }

  /* Backtrack state for unpainted check */
  for (guint i = 0; i < ctx->size; i++) {
    ctx->remaining_row_tiles_sum[i] += ctx->tile_row_map[tile_idx][i];
    ctx->remaining_col_tiles_sum[i] += ctx->tile_col_map[tile_idx][i];
  }

  if (ctx->solutions_found > 1)
    return;

  /* Try Painted (1) */
  possible = TRUE;
  for (guint i = 0; i < ctx->size; i++) {
    /* If we DO paint this tile, do we exceed any clues? */
    int new_row_sum = ctx->current_row_sums[i] + ctx->tile_row_map[tile_idx][i];
    int new_col_sum = ctx->current_col_sums[i] + ctx->tile_col_map[tile_idx][i];

    if (new_row_sum > ctx->row_clues[i] || new_col_sum > ctx->col_clues[i]) {
      possible = FALSE;
    }
  }

  if (possible) {
    /* Assign Painted */
    for (guint i = 0; i < ctx->size; i++) {
      ctx->current_row_sums[i] += ctx->tile_row_map[tile_idx][i];
      ctx->current_col_sums[i] += ctx->tile_col_map[tile_idx][i];
      ctx->remaining_row_tiles_sum[i] -= ctx->tile_row_map[tile_idx][i];
      ctx->remaining_col_tiles_sum[i] -= ctx->tile_col_map[tile_idx][i];
    }

    solve_recursive(ctx, tile_idx + 1);

    /* Backtrack */
    for (guint i = 0; i < ctx->size; i++) {
      ctx->current_row_sums[i] -= ctx->tile_row_map[tile_idx][i];
      ctx->current_col_sums[i] -= ctx->tile_col_map[tile_idx][i];
      ctx->remaining_row_tiles_sum[i] += ctx->tile_row_map[tile_idx][i];
      ctx->remaining_col_tiles_sum[i] += ctx->tile_col_map[tile_idx][i];
    }
  }
}

static int count_solutions(TilepaintApplication *tilepaint, int num_tiles) {
  SolverCtx ctx;
  ctx.size = tilepaint->board_size;
  ctx.num_tiles = num_tiles;
  ctx.row_clues = tilepaint->row_clues;
  ctx.col_clues = tilepaint->col_clues;
  ctx.solutions_found = 0;

  ctx.tile_row_map = g_new0(int *, num_tiles);
  ctx.tile_col_map = g_new0(int *, num_tiles);
  for (int i = 0; i < num_tiles; i++) {
    ctx.tile_row_map[i] = g_new0(int, ctx.size);
    ctx.tile_col_map[i] = g_new0(int, ctx.size);
  }

  ctx.current_row_sums = g_new0(int, ctx.size);
  ctx.current_col_sums = g_new0(int, ctx.size);
  ctx.remaining_row_tiles_sum = g_new0(int, ctx.size);
  ctx.remaining_col_tiles_sum = g_new0(int, ctx.size);

  /* Populate maps */
  for (guint x = 0; x < ctx.size; x++) {
    for (guint y = 0; y < ctx.size; y++) {
      int tid = tilepaint->board[x][y].tile_id;
      ctx.tile_row_map[tid][y]++;
      ctx.tile_col_map[tid][x]++;
      ctx.remaining_row_tiles_sum[y]++;
      ctx.remaining_col_tiles_sum[x]++;
    }
  }

  solve_recursive(&ctx, 0);

  /* Cleanup */
  for (int i = 0; i < num_tiles; i++) {
    g_free(ctx.tile_row_map[i]);
    g_free(ctx.tile_col_map[i]);
  }
  g_free(ctx.tile_row_map);
  g_free(ctx.tile_col_map);
  g_free(ctx.current_row_sums);
  g_free(ctx.current_col_sums);
  g_free(ctx.remaining_row_tiles_sum);
  g_free(ctx.remaining_col_tiles_sum);

  return ctx.solutions_found;
}

void tilepaint_generate_board(TilepaintApplication *tilepaint,
                              guint new_board_size, guint seed) {
  guint x, y;

  g_return_if_fail(tilepaint != NULL);
  g_return_if_fail(new_board_size > 0);

  /* Seed the random number generator */
  if (seed == 0)
    seed = g_get_real_time();

  if (tilepaint->debug)
    g_debug("Seed value: %u", seed);

  srand(seed);

  int attempts = 0;
  while (TRUE) {
    attempts++;

    /* Deallocate any previous board */
    tilepaint_free_board(tilepaint);

    tilepaint->board_size = new_board_size;

    /* Allocate the board */
    tilepaint->board = g_new(TilepaintCell *, tilepaint->board_size);
    for (x = 0; x < tilepaint->board_size; x++)
      tilepaint->board[x] =
          g_slice_alloc0(sizeof(TilepaintCell) * tilepaint->board_size);

    /* 1. Generate Solution */
    gboolean *solution_data =
        g_new(gboolean, tilepaint->board_size * tilepaint->board_size);
    gboolean **solution = g_new(gboolean *, tilepaint->board_size);
    for (x = 0; x < tilepaint->board_size; x++)
      solution[x] = solution_data + (x * tilepaint->board_size);

    for (x = 0; x < tilepaint->board_size; x++) {
      for (y = 0; y < tilepaint->board_size; y++) {
        solution[x][y] = rand() % 2; /* 50% chance */
      }
    }

    /* 2. Partition into Tiles */
    gint *tile_ids_data =
        g_new(gint, tilepaint->board_size * tilepaint->board_size);
    gint **tile_ids = g_new(gint *, tilepaint->board_size);
    for (x = 0; x < tilepaint->board_size; x++)
      tile_ids[x] = tile_ids_data + (x * tilepaint->board_size);

    for (x = 0; x < tilepaint->board_size * tilepaint->board_size; x++)
      tile_ids_data[x] = -1;

    int current_tile_id = 0;

    for (x = 0; x < tilepaint->board_size; x++) {
      for (y = 0; y < tilepaint->board_size; y++) {
        if (tile_ids[x][y] == -1) {
          tile_ids[x][y] = current_tile_id;
          grow_tile(tile_ids, solution, tilepaint->board_size, x, y,
                    current_tile_id);
          current_tile_id++;
        }
      }
    }

    /* 3. Store to app (temporary for rules check) */
    for (x = 0; x < tilepaint->board_size; x++) {
      for (y = 0; y < tilepaint->board_size; y++) {
        tilepaint->board[x][y].tile_id = tile_ids[x][y];
        if (solution[x][y])
          tilepaint->board[x][y].status = CELL_SHOULD_BE_PAINTED;
        else
          tilepaint->board[x][y].status = 0;
      }
    }

    /* 4. Calculate Clues */
    for (y = 0; y < tilepaint->board_size; y++) {
      int count = 0;
      for (x = 0; x < tilepaint->board_size; x++) {
        if (solution[x][y])
          count++;
      }
      tilepaint->row_clues[y] = count;
    }

    for (x = 0; x < tilepaint->board_size; x++) {
      int count = 0;
      for (y = 0; y < tilepaint->board_size; y++) {
        if (solution[x][y])
          count++;
      }
      tilepaint->col_clues[x] = count;
    }

    /* 5. Check Uniqueness */
    int sol_count = count_solutions(tilepaint, current_tile_id);

    /* Cleanup temporary structures */
    g_free(solution);
    g_free(solution_data);
    g_free(tile_ids);
    g_free(tile_ids_data);

    if (sol_count == 1) {
      if (tilepaint->debug)
        g_debug("Found unique board in %d attempts", attempts);
      break;
    }
  }

  /* Clear cell statuses for the player (except secret goal) */
  for (x = 0; x < tilepaint->board_size; x++) {
    for (y = 0; y < tilepaint->board_size; y++) {
      gboolean should_be =
          (tilepaint->board[x][y].status & CELL_SHOULD_BE_PAINTED) != 0;
      tilepaint->board[x][y].status = should_be ? CELL_SHOULD_BE_PAINTED : 0;
    }
  }

  /* Update things */
  tilepaint_enable_events(tilepaint);
}
