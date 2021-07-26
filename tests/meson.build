catch_dep = dependency('catch2')

jdbg_tests_src = [
  'jdbg_tests.cpp',
  'pretty_print_tests.cpp',
  'type_name_tests.cpp',
  'main.cpp',
]

configure_file(
  input: meson.project_source_root() + '/cmake/' + project_name + '_pch.hpp.in',
  output: project_name + '_pch.hpp',
  copy: true,
)

jdbg_tests = executable('jdbg-tests',
  sources: jdbg_tests_src,
  dependencies: [jdbg_dep, catch_dep],
  cpp_pch: meson.current_build_dir() + '/' + project_name + '_pch.hpp',
)

test('jdbg-tests', jdbg_tests)
