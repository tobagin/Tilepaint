/* test-preferences-gsettings.c — GSettings clue-color-feedback persistence */
#include <gio/gio.h>
#include <glib.h>

static void test_default_false(void) {
  GSettings *settings = g_settings_new(APPLICATION_ID);
  gboolean val = g_settings_get_boolean(settings, "clue-color-feedback");
  g_assert_false(val);
  /* default value via get_default_value */
  GVariant *def = g_settings_get_default_value(settings, "clue-color-feedback");
  g_assert_nonnull(def);
  gboolean d = g_variant_get_boolean(def);
  g_assert_false(d);
  g_variant_unref(def);
  g_object_unref(settings);
}

static void test_toggle_persist(void) {
  GSettings *settings = g_settings_new(APPLICATION_ID);
  g_settings_set_boolean(settings, "clue-color-feedback", TRUE);
  g_assert_true(g_settings_get_boolean(settings, "clue-color-feedback"));
  g_settings_set_boolean(settings, "clue-color-feedback", FALSE);
  g_assert_false(g_settings_get_boolean(settings, "clue-color-feedback"));
  /* set true again for next test to see persistence across new instance */
  g_settings_set_boolean(settings, "clue-color-feedback", TRUE);
  g_object_unref(settings);

  /* Simulate restart: new GSettings instance with same backend */
  GSettings *s2 = g_settings_new(APPLICATION_ID);
  g_assert_true(g_settings_get_boolean(s2, "clue-color-feedback"));
  /* reset to default */
  g_settings_reset(s2, "clue-color-feedback");
  g_assert_false(g_settings_get_boolean(s2, "clue-color-feedback"));
  g_object_unref(s2);
}

static gboolean changed_fired = FALSE;
static void on_changed(GSettings *s, const gchar *key, gpointer ud) {
  gint *count = (gint *)ud;
  (*count)++;
  changed_fired = TRUE;
}

static void test_changed_signal(void) {
  GSettings *settings = g_settings_new(APPLICATION_ID);
  g_settings_reset(settings, "clue-color-feedback");
  gint count = 0;
  gulong id = g_signal_connect(settings, "changed::clue-color-feedback",
                               G_CALLBACK(on_changed), &count);
  g_settings_set_boolean(settings, "clue-color-feedback", TRUE);
  /* signal is synchronous */
  g_assert_cmpint(count, ==, 1);
  g_settings_set_boolean(settings, "clue-color-feedback", FALSE);
  g_assert_cmpint(count, ==, 2);
  g_signal_handler_disconnect(settings, id);
  g_settings_reset(settings, "clue-color-feedback");
  g_object_unref(settings);
}

int main(int argc, char *argv[]) {
  g_test_init(&argc, &argv, NULL);

  /* Hermetic isolation: use a temporary keyfile backend so the tests never
   * read from or write to the user's real dconf settings, and a fresh
   * XDG_CONFIG_HOME guarantees a clean "fresh install" state. */
  gchar *tmp_conf = g_dir_make_tmp("tilepaint-gsettings-test-XXXXXX", NULL);
  g_assert_nonnull(tmp_conf);
  gchar *xdg_conf = g_build_filename(tmp_conf, "config", NULL);
  gchar *keyfile_dir =
      g_build_filename(xdg_conf, "glib-2.0", "settings", NULL);
  g_mkdir_with_parents(keyfile_dir, 0700);
  g_setenv("XDG_CONFIG_HOME", xdg_conf, TRUE);
  g_setenv("GSETTINGS_BACKEND", "keyfile", TRUE);
  g_free(keyfile_dir);
  g_free(xdg_conf);

  g_test_add_func("/preferences/default_false", test_default_false);
  g_test_add_func("/preferences/toggle_persist", test_toggle_persist);
  g_test_add_func("/preferences/changed_signal", test_changed_signal);
  int ret = g_test_run();

  /* Best-effort cleanup of the temporary config tree */
  gchar *rm_argv[] = {(gchar *)"rm", (gchar *)"-rf", tmp_conf, NULL};
  g_spawn_sync(NULL, rm_argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL,
               NULL, NULL, NULL);
  g_free(tmp_conf);
  return ret;
}
