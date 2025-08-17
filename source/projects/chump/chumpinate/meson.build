cheaders_proj = subproject('cheaders')
cheaders_dep = cheaders_proj.get_variable('cheaders_dep')

shared_library(
  'Chumpinate',
  'Chumpinate.cpp',
  name_prefix: '', # 'lib' is prepended otherwise
  name_suffix: 'chug',
  dependencies: [cheaders_dep, chump_lib_dep, nlohmann_json_dep]
)
