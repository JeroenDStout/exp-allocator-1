init_directory(version)

# Define the Version project
init_project(version "meta")

# Sources static
setup_project_source(version "version"
  git_version.cpp
  git_version.h GENERATED
)
setup_project_source(version "script"
  ../touch
  generate_git_version.py
)

# Target
configure_project_static_lib(version)
configure_cxx_target(version)

# Custom
add_custom_command(
  COMMENT			"Generate version file"
  OUTPUT    		${abs_gen}/git_version.h
  COMMAND   		${Python3_EXECUTABLE}
					${abs_src}/generate_git_version.py
					${abs_gen}/git_version.h
                    "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"
  DEPENDS			${abs_src}/../touch
                    ${abs_src}/generate_git_version.py
  WORKING_DIRECTORY ${project_root_dir}
  VERBATIM
)