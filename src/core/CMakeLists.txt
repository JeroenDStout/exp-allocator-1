init_directory(core)

# Define the Version project
init_project(core "core")

# Sources static
setup_project_source(core "core"
  main.cpp
  main.h
  memory.cpp
  memory.h
  tests.h
)
setup_project_source(core "allocators"
  allocator_passthrough.h
)

# Target
configure_project_executable(core)
configure_cxx_target(core)

add_dependencies(core version)