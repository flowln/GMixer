project('GMixer', 'cpp', version: '0.0.2', default_options : ['cpp_std=c++17', 'cpp_args=-Wall'])

gst_libs = [
  dependency('glib-2.0', version: '>=2.40'),
  dependency('gobject-2.0'),
  dependency('gstreamer-1.0')
]

cpp_libs = [
  dependency('sigc++-3.0'),
  dependency('threads')
]

gtk_libs = [
  dependency('gtkmm-4.0')
]

sources = files([])

subdir('src')

executable('gmixer', sources, dependencies: [gst_libs, gtk_libs, cpp_libs], install: true, include_directories: 'src/')
