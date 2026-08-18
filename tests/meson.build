# Tests for TILE-001
test_env = environment()
# Ensure GSettings finds compiled schemas
test_env.set('GSETTINGS_SCHEMA_DIR', meson.project_build_root() / 'data')

# The clue-color test links the PRODUCTION interface.c (which defines
# tilepaint_clue_color) plus rules.c/score.c. interface.c includes "config.h",
# so configure a tests-local copy from the same root configuration data.
tests_config_h = configure_file(
  output: 'config.h',
  configuration: config_h,
)

test_clue = executable('test-clue-color',
  ['test-clue-color.c', tests_config_h, '../src/interface.c', '../src/rules.c', '../src/score.c'],
  dependencies: [glib_dependency, gio_dependency, gtk_dependency, adw_dependency, gmodule_dependency, cairo_dependency],
  include_directories: [include_directories('..'), include_directories('../src')],
  c_args: ['-DAPPLICATION_ID="@0@"'.format(application_id), '-DHAVE_CONFIG_H', '-DGETTEXT_PACKAGE="@0@"'.format(meson.project_name())],
)

test('clue-color', test_clue, env: test_env)

test_win = executable('test-win-path',
  ['test-win-path.c', '../src/rules.c'],
  dependencies: [glib_dependency, gio_dependency, gtk_dependency, adw_dependency, gmodule_dependency, cairo_dependency],
  include_directories: [include_directories('..'), include_directories('../src')],
  c_args: ['-DAPPLICATION_ID="@0@"'.format(application_id), '-DHAVE_CONFIG_H'],
)

test('win-path', test_win, env: test_env)

test_prefs = executable('test-preferences-gsettings',
  'test-preferences-gsettings.c',
  dependencies: [gio_dependency],
  c_args: ['-DAPPLICATION_ID="@0@"'.format(application_id)],
)

test('preferences-gsettings', test_prefs, env: test_env)
