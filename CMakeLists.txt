cmake_minimum_required(VERSION 2.8.6)

project(MuscleOptimizer)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(OpenSim 3.2 REQUIRED)

set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Build shared libraries" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(WIN32) # copied from OpenSim's
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif()

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

#ARGHHH:
link_directories(${OpenSim_LIB_DIR})

add_subdirectory(lib)
add_subdirectory(application)
add_subdirectory(wrappers)

include(CTest)
enable_testing()
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
