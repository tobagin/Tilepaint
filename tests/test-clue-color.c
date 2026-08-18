/* test-clue-color.c — clue color helper logic (exercises the PRODUCTION
 * tilepaint_clue_color() from src/interface.c, which is linked directly
 * into this test along with rules.c and score.c). Main.c-only symbols are
 * stubbed below; everything else is real production code. */
#include <glib.h>
#include <gtk/gtk.h>
#include <math.h>
#include "../src/main.h"
#include "../src/interface.h"

/* GdkRGBA returns trigger -Waggregate-return (enabled project-wide with
 * -Werror); the production helper is pragma-guarded in interface.c, and the
 * call sites here need the same relaxation. */
#pragma GCC diagnostic ignored "-Waggregate-return"

/* ---- Stubs for symbols defined only in src/main.c (never called by these
 * tests; they exist so the production interface.c translation unit links) ---- */
void tilepaint_new_game(Tilepaint *t, guint s) { (void)t; (void)s; }
void tilepaint_pause_timer(Tilepaint *t) { (void)t; }
void tilepaint_reset_timer(Tilepaint *t) { (void)t; }
void tilepaint_start_timer(Tilepaint *t) { (void)t; }
void tilepaint_set_board_size(Tilepaint *t, guint s) { (void)t; (void)s; }
void tilepaint_quit(Tilepaint *t) { (void)t; }
void tilepaint_disable_events(Tilepaint *t) { (void)t; }

static const TilepaintTheme theme_dark_test = {
    {0.141, 0.122, 0.192, 1.0},
    {0.102, 0.102, 0.102, 1.0},
    {0.239, 0.220, 0.275, 1.0},
    {0.102, 0.102, 0.102, 1.0},
    {0.965, 0.961, 0.957, 1.0},
    {0.427, 0.427, 0.427, 1.0},
    {1.0, 0.482, 0.388, 1.0},
    {0.200, 0.820, 0.478, 1.0},
};
static const TilepaintTheme theme_light_test = {
    {0.929, 0.929, 0.929, 1.0},
    {0.957, 0.957, 0.957, 1.0},
    {0.180, 0.204, 0.212, 1.0},
    {0.730, 0.737, 0.722, 1.0},
    {0.180, 0.204, 0.212, 1.0},
    {0.655, 0.671, 0.655, 1.0},
    {0.937, 0.161, 0.161, 1.0},
    {0.106, 0.604, 0.239, 1.0},
};

static gboolean rgba_equal(GdkRGBA a, GdkRGBA b) {
  const double eps = 0.001;
  return fabs(a.red - b.red) < eps && fabs(a.green - b.green) < eps &&
         fabs(a.blue - b.blue) < eps && fabs(a.alpha - b.alpha) < eps;
}

/* These tests exercise the production tilepaint_clue_color() declared in interface.h */

static void test_feedback_disabled_always_default(void) {
  TilepaintApplication app = {0};
  app.theme = &theme_dark_test;
  for (int board_size = 5; board_size <= 10; board_size++) {
    for (int clue = 0; clue <= board_size; clue++) {
      for (int count = 0; count <= board_size + 1; count++) {
        GdkRGBA c = tilepaint_clue_color(&app, count, clue, FALSE);
        g_assert_true(rgba_equal(c, app.theme->unpainted_text));
      }
    }
  }
  app.theme = &theme_light_test;
  for (int board_size = 5; board_size <= 10; board_size++) {
    for (int clue = 0; clue <= board_size; clue++) {
      for (int count = 0; count <= board_size + 1; count++) {
        GdkRGBA c = tilepaint_clue_color(&app, count, clue, FALSE);
        g_assert_true(rgba_equal(c, app.theme->unpainted_text));
      }
    }
  }
}

static void test_feedback_enabled_colors(void) {
  TilepaintApplication app = {0};
  const TilepaintTheme *themes[] = {&theme_dark_test, &theme_light_test};
  for (int ti = 0; ti < 2; ti++) {
    app.theme = themes[ti];
    for (int board_size = 5; board_size <= 10; board_size++) {
      for (int clue = 1; clue <= board_size; clue++) {
        GdkRGBA c_under = tilepaint_clue_color(&app, clue - 1, clue, TRUE);
        g_assert_true(rgba_equal(c_under, app.theme->unpainted_text));
        GdkRGBA c_exact = tilepaint_clue_color(&app, clue, clue, TRUE);
        g_assert_true(rgba_equal(c_exact, app.theme->success_text));
        if (clue < board_size) {
          GdkRGBA c_over = tilepaint_clue_color(&app, clue + 1, clue, TRUE);
          g_assert_true(rgba_equal(c_over, app.theme->error_text));
        }
      }
      GdkRGBA c_big = tilepaint_clue_color(&app, 10, 2, TRUE);
      g_assert_true(rgba_equal(c_big, app.theme->error_text));
    }
  }
}

static void test_board_integration(void) {
  TilepaintApplication app = {0};
  app.theme = &theme_dark_test;
  app.board_size = 5;
  TilepaintCell *rows5[10];
  TilepaintCell data5[10][10] = {0};
  for (int x = 0; x < 5; x++) rows5[x] = data5[x];
  app.board = rows5;
  app.col_clues[0] = 2;
  app.row_clues[0] = 2;
  data5[0][0].status = CELL_PAINTED;
  data5[1][0].status = CELL_PAINTED;
  data5[0][1].status = CELL_PAINTED;
  int row0_count = 0;
  for (int x = 0; x < 5; x++) if (data5[x][0].status & CELL_PAINTED) row0_count++;
  GdkRGBA c = tilepaint_clue_color(&app, row0_count, app.row_clues[0], TRUE);
  g_assert_true(rgba_equal(c, app.theme->success_text));
  int col0_count = 0;
  for (int y = 0; y < 5; y++) if (data5[0][y].status & CELL_PAINTED) col0_count++;
  c = tilepaint_clue_color(&app, col0_count, app.col_clues[0], TRUE);
  g_assert_true(rgba_equal(c, app.theme->success_text));
  data5[2][0].status = CELL_PAINTED;
  row0_count = 0;
  for (int x = 0; x < 5; x++) if (data5[x][0].status & CELL_PAINTED) row0_count++;
  c = tilepaint_clue_color(&app, row0_count, app.row_clues[0], TRUE);
  g_assert_true(rgba_equal(c, app.theme->error_text));

  app.board_size = 10;
  TilepaintCell *rows10[10];
  TilepaintCell data10[10][10] = {0};
  for (int x = 0; x < 10; x++) rows10[x] = data10[x];
  app.board = rows10;
  app.col_clues[5] = 5;
  app.row_clues[5] = 10;
  for (int x = 0; x < 5; x++) data10[x][5].status = CELL_PAINTED;
  int row5_count = 0;
  for (int x = 0; x < 10; x++) if (data10[x][5].status & CELL_PAINTED) row5_count++;
  c = tilepaint_clue_color(&app, row5_count, app.row_clues[5], TRUE);
  g_assert_true(rgba_equal(c, app.theme->unpainted_text));
  int col5_count = 0;
  for (int y = 0; y < 10; y++) if (data10[5][y].status & CELL_PAINTED) col5_count++;
  c = tilepaint_clue_color(&app, col5_count, app.col_clues[5], TRUE);
  g_assert_true(rgba_equal(c, app.theme->unpainted_text));
}

int main(int argc, char *argv[]) {
  g_test_init(&argc, &argv, NULL);
  g_test_add_func("/clue-color/disabled_always_default", test_feedback_disabled_always_default);
  g_test_add_func("/clue-color/enabled_colors", test_feedback_enabled_colors);
  g_test_add_func("/clue-color/board_integration", test_board_integration);
  return g_test_run();
}
