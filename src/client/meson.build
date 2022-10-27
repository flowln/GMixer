sources += files([
  'Client.cpp'
])

subdir('cmd')
subdir('common')

if get_option('gtk')
  sources += files(['GtkClient.cpp'])
  subdir('gtk')
endif

