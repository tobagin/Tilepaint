subdir('icons')

app_conf = configuration_data()
app_conf.set('APP_ID', application_id)

configured_desktop = configure_file(
  input: 'tilepaint.desktop.in',
  output: 'tilepaint.desktop.in.in',
  configuration: app_conf
)

desktop_file = i18n.merge_file(
  type: 'desktop',
  input: configured_desktop,
  output: '@0@.desktop'.format(application_id),
  po_dir: join_paths(meson.source_root(), 'po'),
  install: true,
  install_dir: join_paths(get_option('datadir'), 'applications'),
)

desktop_file_validate = find_program('desktop-file-validate', required: false)
if desktop_file_validate.found()
  test(
    'validate-desktop',
    desktop_file_validate,
    args: [
      desktop_file.full_path(),
    ],
  )
endif

configured_metainfo = configure_file(
  input: 'tilepaint.metainfo.xml.in',
  output: 'tilepaint.metainfo.xml.in',
  configuration: app_conf
)

metainfo_file = i18n.merge_file(
  input: configured_metainfo,
  output: '@0@.metainfo.xml'.format(application_id),
  po_dir: join_paths(meson.source_root(), 'po'),
  install: true,
  install_dir: join_paths(get_option('datadir'), 'metainfo'),
)

appstreamcli = find_program('appstreamcli', required: false)
if appstreamcli.found()
  test(
    'validate-metainfo', appstreamcli,
    args: [
      'validate', '--no-net', '--explain', metainfo_file.full_path(),
    ],
  )
endif

schema_conf = configuration_data()
schema_conf.set('appid', application_id)
schema_conf.set('apppath', '/' + application_id.replace('.', '/') + '/')

configured_schema = configure_file(
  input: 'io.github.tobagin.Tilepaint.gschema.xml.in',
  output: '@0@.gschema.xml'.format(application_id),
  configuration: schema_conf,
  install: true,
  install_dir: join_paths(get_option('datadir'), 'glib-2.0', 'schemas'),
)

glib_compile_schemas = find_program('glib-compile-schemas', required: false)
if glib_compile_schemas.found()
  custom_target('compile-schemas',
    input: configured_schema,
    output: 'gschemas.compiled',
    command: [glib_compile_schemas, meson.current_build_dir()],
    build_by_default: true,
  )
endif


# Blueprint UI files
blueprint_compiler = find_program('blueprint-compiler', required: true)

blueprint_files = [
  'tilepaint.blp',
  'help-overlay.blp',
  'preferences.blp',
]

compiled_ui_files = []

foreach blp : blueprint_files
  compiled_ui_files += custom_target(
    blp.underscorify() + '_ui',
    input: blp,
    output: '@BASENAME@.ui',
    command: [
      blueprint_compiler, 'batch-compile', '@OUTDIR@', '@CURRENT_SOURCE_DIR@', '@INPUT@'
    ],
  )
endforeach

# gtk4-builder-tool only understands Gtk types; Adw UIs (AdwApplicationWindow,
# AdwPreferencesDialog, AdwShortcutsDialog) are correctly validated at build
# time by blueprint-compiler batch-compile. The previous gtk4-builder-tool
# "validate" tests always failed for Adw UIs and blocked `meson test`.
# Blueprint compilation is the correct validator here, so we do not add a
# separate validate step for these UI files.
_ = compiled_ui_files  # keep variable referenced so meson does not warn unused


