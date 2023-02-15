#
# Initial set-up
#

cmake_minimum_required(VERSION 3.15)

project(Core)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Languages
include(cmake/cxx.cmake)
include(cmake/python.cmake)

# Support
include(cmake/get_cpm.cmake)
include(cmake/artefacts.cmake)
include(cmake/sources.cmake)

#
# Base config
#

 # intentionally left blank

#
# Paths
#

 # intentionally left blank

#
# Subprojects
#

add_subdirectory(src/core)
add_subdirectory(src/version)

message(STATUS "")