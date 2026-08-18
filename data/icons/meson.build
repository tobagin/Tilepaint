scalable_dir = join_paths('hicolor', 'scalable', 'apps')
if profile == 'Devel'
  install_data(
    join_paths('scalable', 'tilepaint-devel.svg'),
    rename: ('@0@.svg'.format(application_id)),
    install_dir: join_paths(get_option('datadir'), 'icons', scalable_dir)
  )
else
  install_data(
    join_paths('scalable', 'tilepaint.svg'),
    rename: ('@0@.svg'.format(application_id)),
    install_dir: join_paths(get_option('datadir'), 'icons', scalable_dir)
  )
endif

install_data(
    join_paths('hicolor', '512x512', 'apps', 'tilepaint.png'),
    rename: (application_id + '.png'),
    install_dir: join_paths(get_option('datadir'), 'icons', 'hicolor', '512x512', 'apps')
)

symbolic_dir = join_paths('hicolor', 'symbolic', 'apps')
install_data(
  join_paths('symbolic', 'tilepaint-symbolic.svg'),
  rename: ('@0@-symbolic.svg'.format(application_id)),
  install_dir: join_paths(get_option('datadir'), 'icons', symbolic_dir)
)
