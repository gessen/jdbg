catch_dep = dependency('catch2')

jdbg_tests_src = [
  'jdbg_tests.cpp',
  'main.cpp',
]

jdbg_tests = executable('jdbg-tests',
  sources: jdbg_tests_src,
  dependencies: [jdbg_dep, catch_dep],
)

test('jdbg-tests', jdbg_tests)
