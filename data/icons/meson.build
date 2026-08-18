if get_option('profile') == 'development'
  main_icon = 'tilepaint-devel.svg'
  symbolic_icon = 'tilepaint-symbolic.svg' # Could add stripes here too if wanted
else
  main_icon = 'tilepaint.svg'
  symbolic_icon = 'tilepaint-symbolic.svg'
endif

scalable_dir = join_paths('hicolor', 'scalable', 'apps')
install_data(
  join_paths('scalable', main_icon),
  rename: ('@0@.svg'.format(application_id)),
  install_dir: join_paths(get_option('datadir'), 'icons', scalable_dir)
)

install_data(
    join_paths('hicolor', '512x512', 'apps', 'tilepaint.png'),
    rename: (application_id + '.png'),
    install_dir: join_paths(get_option('datadir'), 'icons', 'hicolor', '512x512', 'apps')
)

symbolic_dir = join_paths('hicolor', 'symbolic', 'apps')
install_data(
  join_paths('symbolic', symbolic_icon),
  rename: ('@0@-symbolic.svg'.format(application_id)),
  install_dir: join_paths(get_option('datadir'), 'icons', symbolic_dir)
)
