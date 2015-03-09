# - Try to find OpenSim and SimTK
# Once done this will define
#  OPENSIM_FOUND - System has OpenSim
#  OPENSIM_INCLUDE_DIRS - The OpenSim include directories
#  SIMTK_INCLUDE_DIRS - The SimTK include directories
#  OpenSim_LIBRARIES - The libraries needed to use Opensim

find_package(OpenSim NO_MODULE QUIET) # todo: check if required, verbose etc.
if(OpenSim_FOUND)
  return()
endif()

set(OPENSIM_POSSIBLE_DIRECTORIES
    ~/Programs/opensim
    $ENV{OPENSIM_HOME}
    "C:/OpenSim\ 3.0/"
    "C:/OpenSim3.1/"
    )

find_path(OpenSim_INCLUDE_DIRS OpenSim/OpenSim.h
          HINTS ${OPENSIM_HOME}
          PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
          PATH_SUFFIXES sdk/include )
MESSAGE( STATUS "OpenSim Include path: ${OPENSIM_INCLUDE_DIRS}")

find_path(SIMTK_INCLUDE_DIRS Simbody.h
          HINTS ${OPENSIM_HOME}
          PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
          PATH_SUFFIXES sdk/include/SimTK/include sdk/include/SimTK/simbody)
MESSAGE( STATUS "SimTK Include path: ${SIMTK_INCLUDE_DIRS}")

if(SIMTK_INCLUDE_DIRS)
    list(APPEND OpenSim_INCLUDE_DIRS ${SIMTK_INCLUDE_DIRS})
    list(REMOVE_DUPLICATES OpenSim_INCLUDE_DIRS)
endif()

#why?
#find_path(OPENSIM_DLL_DIR
#        NAMES
#            osimCommon.dll
#           id
#          PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
#          PATH_SUFFIXES bin)
#MESSAGE( STATUS "OpenSim dll path: ${OPENSIM_DLL_DIR}")

#SET(OPENSIM_LIBRARY_DIRS ${OPENSIM_LIBS_DIR} ${OPENSIM_DLL_DIR})
#MESSAGE( STATUS "OpenSim Library path: ${OPENSIM_LIBRARY_DIRS}")

if(UNIX)
    set(NameSpace )
else()
    set(NameSpace OpenSim_) #what if someone compiled opensim and used a different namespace?
endif()

find_library(osimSimulation
             NAMES osimSimulation
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(osimCommon
             NAMES osimCommon
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(osimActuators
             NAMES osimActuators
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(osimAnalyses
             NAMES osimAnalyses
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(osimTools
             NAMES osimTools
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(SimTKcommon
             NAMES ${NameSpace}SimTKcommon SimTKcommon
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(SimTKmath
             NAMES ${NameSpace}SimTKmath SimTKmath
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(SimTKsimbody
             NAMES ${NameSpace}SimTKsimbody SimTKsimbody
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

#does this exist?
#find_library(SimTKlapack
#             NAMES SimTKlapack
#             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
#             PATH_SUFFIXES sdk/lib lib)

## DEBUG VERSIONS:

set(debug_postfix "_d")

find_library(osimSimulation_debug
             NAMES osimSimulation${debug_postfix}
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(osimCommon_debug
             NAMES osimCommon${debug_postfix}
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(osimActuators_debug
             NAMES osimActuators${debug_postfix}
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(osimAnalyses_debug
             NAMES osimAnalyses${debug_postfix}
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(osimTools_debug
             NAMES osimTools${debug_postfix}
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(SimTKcommon_debug
             NAMES ${NameSpace}SimTKcommon${debug_postfix} SimTKcommon${debug_postfix}
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(SimTKmath_debug
             NAMES ${NameSpace}SimTKmath${debug_postfix} SimTKmath${debug_postfix}
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)

find_library(SimTKsimbody_debug
             NAMES ${NameSpace}SimTKsimbody${debug_postfix} SimTKsimbody${debug_postfix}
             HINTS ${OPENSIM_HOME}
             PATHS ${OPENSIM_POSSIBLE_DIRECTORIES}
             PATH_SUFFIXES sdk/lib lib)


#we assume that Release version should always be present, Debug version might be missing
set(OpenSim_LIBRARIES)
if(osimSimulation_debug)
    list(APPEND OpenSim_LIBRARIES optimized ${osimSimulation} debug ${osimSimulation_debug})
else()
    list(APPEND OpenSim_LIBRARIES ${osimSimulation})
endif()
if(osimCommon_debug)
    list(APPEND OpenSim_LIBRARIES optimized ${osimCommon} debug ${osimCommon_debug})
else()
    list(APPEND OpenSim_LIBRARIES ${osimCommon})
endif()
if(osimActuators_debug)
    list(APPEND OpenSim_LIBRARIES optimized ${osimActuators} debug ${osimActuators_debug})
else()
    list(APPEND OpenSim_LIBRARIES ${osimActuators})
endif()
if(osimAnalyses_debug)
    list(APPEND OpenSim_LIBRARIES optimized ${osimAnalyses} debug ${osimAnalyses_debug})
else()
    list(APPEND OpenSim_LIBRARIES ${osimAnalyses})
endif()
if(osimTools_debug)
    list(APPEND OpenSim_LIBRARIES optimized ${osimTools} debug ${osimTools_debug})
else()
    list(APPEND OpenSim_LIBRARIES ${osimTools})
endif()
if(SimTKcommon_debug)
    list(APPEND OpenSim_LIBRARIES optimized ${SimTKcommon} debug ${SimTKcommon_debug})
else()
    list(APPEND OpenSim_LIBRARIES ${SimTKcommon})
endif()
if(SimTKmath_debug)
    list(APPEND OpenSim_LIBRARIES optimized ${SimTKmath} debug ${SimTKmath_debug})
else()
    list(APPEND OpenSim_LIBRARIES ${SimTKmath})
endif()
if(SimTKsimbody_debug)
    list(APPEND OpenSim_LIBRARIES optimized ${SimTKsimbody} debug ${SimTKsimbody_debug})
else()
    list(APPEND OpenSim_LIBRARIES ${SimTKsimbody})
endif()


#IF(WIN32)
#   SET(PLATFORM_LIBS  pthreadVC2)
#ELSE (WIN32)
#  SET(NameSpace "")
#  IF(APPLE)
#   SET(PLATFORM_LIBS  SimTKAtlas)
#  ELSE(APPLE)
#   SET(PLATFORM_LIBS SimTKAtlas_Lin_generic)
#  ENDIF(APPLE)
#ENDIF (WIN32)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set OPENSIM_FOUND to TRUE
# if all listed variables are TRUE
# DEFAULT_MSG is predefined... change only if you need a custom msg
find_package_handle_standard_args(OpenSim DEFAULT_MSG
                                  OpenSim_LIBRARIES OpenSim_INCLUDE_DIRS SIMTK_INCLUDE_DIRS)
