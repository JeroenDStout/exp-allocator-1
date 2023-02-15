set(project_root_dir ${PROJECT_SOURCE_DIR})


macro(init_directory name)
  message(STATUS "")
  message(STATUS "-- ${name}")
  file(RELATIVE_PATH dir_rel ${project_root_dir} ${CMAKE_CURRENT_SOURCE_DIR})
  set(abs_src ${CMAKE_CURRENT_SOURCE_DIR})
  set(abs_gen ${CMAKE_BINARY_DIR}/gen/${dir_rel})
  file(RELATIVE_PATH rel_src ${project_root_dir} ${abs_src})
  file(RELATIVE_PATH rel_gen ${project_root_dir} ${abs_gen})
  message(STATUS "Source    : ./${rel_src}")
  message(STATUS "Generated : ./${rel_gen}")
endmacro()


macro(init_project name path)
  message(STATUS "Add project ${name}")
  project(${name})
  set(project_folder ${path})
endmacro()


macro(set_project_source_list project_ref)
  # set variable name for project list
  set(project_source_list __sources_${project_ref})
endmacro()


macro(internal_add_project_source source is_generated)
  if (NOT ${is_generated} STREQUAL "GENERATED")
    set(new_file ${abs_src}/${source})
    list(APPEND new_files_src ${new_file})
  else()
    set(new_file ${abs_gen}/${source})
    set_source_files_properties(${new_file} PROPERTIES GENERATED ON)
    list(APPEND new_files_gen ${new_file})
  endif()
  
  if (NOT EXISTS ${new_file})
    get_filename_component(directory ${new_file} DIRECTORY)
    file(MAKE_DIRECTORY ${directory})
	
    if (NOT ${is_generated} STREQUAL "GENERATED")
      message(STATUS " - Creating ${source}")
      file(TOUCH ${new_file})
    endif()
  endif()
	  
  list(APPEND new_files ${new_file})
endmacro()


macro(internal_recursive_setup_project_source arg1 arg2)  
  if (NOT ${arg2} STREQUAL "GENERATED")
    internal_add_project_source(${arg1} NO)
	if (${ARGC} GREATER 2)
	  internal_recursive_setup_project_source(${project_source_group} ${arg2} ${ARGN})
	else()
	  internal_add_project_source(${arg2} NO)
	endif()
  else()
    internal_add_project_source(${arg1} GENERATED)
	if (${ARGC} GREATER 3)
	  internal_recursive_setup_project_source(${project_source_group} ${ARGN})
	elseif (${ARGC} GREATER 2)
	  internal_add_project_source(${ARGV2} NO)
	endif()
  endif()
endmacro()


macro(setup_project_source project_ref project_source_group)
  set_project_source_list(${project_ref})
  
  set(new_files "")
  set(new_files_src "")
  set(new_files_gen "")
  
  message(STATUS "Add sources for ${project_ref}/${project_source_group}")
  
  if (${ARGC} GREATER 3)
    internal_recursive_setup_project_source(${ARGN})
  else()
    internal_add_project_source(${ARGN} NO)
  endif()
  
  # set source group
  IF (new_files_src)
    source_group(${project_source_group} FILES ${new_files_src})
    list(APPEND ${project_source_list} ${new_files_src})
  ENDIF()
  IF (new_files_gen)
    source_group(${project_source_group}/generated FILES ${new_files_gen})
    list(APPEND ${project_source_list} ${new_files_gen})
  ENDIF()
endmacro()


macro(setup_project_source_specific project_ref project_source_group generated)
  set_project_source_list(${project_ref})
  
  set(new_files "")
  
  message(STATUS "Add sources for ${project_ref}/${project_source_group}")
  
  foreach (var IN ITEMS ${ARGN})
    internal_add_project_source(${var} ${generated})
  endforeach()
  
  # set source group
  source_group(${project_source_group} FILES ${new_files})
  list(APPEND ${project_source_list} ${new_files})
endmacro()


macro(print_all_project_sources)
  foreach(var IN LISTS ${project_source_list})
    file(RELATIVE_PATH rel ${project_root_dir} ${var})
	if (${rel} MATCHES "${rel_gen}*")
	  STRING(REGEX REPLACE "^${rel_gen}" "${dir_rel}" rel ${rel})
      list(APPEND display_list "${rel} *")
	else()
      list(APPEND display_list "${rel}")
	endif()
  endforeach()
  
  list(SORT display_list)
  
  foreach(var IN LISTS display_list)
    message(STATUS " - ${var}")
  endforeach()
endmacro()


macro(clean_project_source_for_build)
  list(REMOVE_DUPLICATES ${project_source_list})
endmacro()

function(configure_project_executable project_ref)
  set_project_source_list(${project_ref})
  clean_project_source_for_build()
  
  message(STATUS "Configuring executable ${project_ref}")
  print_all_project_sources()
  
  add_executable(${project_ref} ${${project_source_list}})
  target_include_directories(${project_ref} PRIVATE ${abs_src} ${abs_gen} ${project_root_dir}/src ${CMAKE_BINARY_DIR}/gen/src)
  target_include_directories(${project_ref} PRIVATE ${abs_src} ${abs_gen})
  
  add_custom_command(
	TARGET     ${project_ref}
    POST_BUILD
    COMMAND    echo * ${project_ref} output $<CONFIG>: $<TARGET_FILE:${project_ref}>
    VERBATIM
  )
  
  install(TARGETS ${project_ref} DESTINATION ${project_root_dir}/bin)
endfunction()


function(configure_project_static_lib project_ref)
  set_project_source_list(${project_ref})
  clean_project_source_for_build()
  
  message(STATUS "Configuring static library ${project_ref}")
  print_all_project_sources()
  
  add_library(${project_ref} STATIC ${${project_source_list}})
  target_include_directories(${project_ref} PRIVATE ${abs_src} ${abs_gen} ${project_root_dir}/src ${CMAKE_BINARY_DIR}/gen/src)
  install(TARGETS ${project_ref} DESTINATION ${project_root_dir}/bin)
  
  set_target_properties(${project_ref} PROPERTIES FOLDER ${project_folder})
  target_include_directories(${project_ref} PRIVATE ${abs_src} ${abs_gen})
  install(TARGETS ${project_ref} DESTINATION ${project_root_dir}/bin)
endfunction()