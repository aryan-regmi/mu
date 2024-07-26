project('mu', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

# Includes
# =============================================
public_headers = include_directories('include')

# Library
# =============================================
sources = files([])
subdir('src')
mu_lib = library(
  'mu',
  sources,
  include_directories: [public_headers]
)

# Tests
# =============================================
subdir('tests')
