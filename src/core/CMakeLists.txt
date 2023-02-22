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
  memory_logging.h
)
setup_project_source(core "allocators"
  allocator_libc.h
  allocator_stack.h
  allocator_linear_pushpop.h
  allocator_reuse.h
  allocator_dummy.h
  allocator_ptr.h
)

# Target
configure_project_executable(core)
configure_cxx_target(core)

add_dependencies(core version)