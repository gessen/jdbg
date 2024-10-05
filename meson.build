project('jdbg', 'cpp',
  version: '0.4.0',
  license: 'MIT',
  default_options: [
    'cpp_std=c++17',
    'warning_level=3',
  ],
)

extra_args = []

if get_option('warning_level').to_int() >= 2
  extra_args += [
    '-Wformat',
    '-Wformat-pedantic',
    '-Woverloaded-virtual',
    '-Wold-style-cast',
  ]
endif

extra_args += [
  '-fasynchronous-unwind-tables',
  '-fstack-protector-strong',
]

compiler = meson.get_compiler('cpp')
foreach arg: extra_args
  if compiler.has_argument(arg)
    add_project_arguments(arg, language: 'cpp')
  endif
endforeach

project_name = meson.project_name()

jdbg_dep = declare_dependency(
  include_directories: 'include',
)

if get_option('build_testing')
  subdir('tests')
endif

if get_option('build_examples')
  subdir('examples')
endif

install_subdir('include',
  install_dir: get_option('includedir'),
  strip_directory: true,
)

pkg = import('pkgconfig')
pkg.generate(
  name: project_name,
  description: 'A helpful debugging macro',
  filebase: project_name,
)
