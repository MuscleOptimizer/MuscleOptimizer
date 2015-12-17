###############################
## Author: Elena Ceseracciu  ##
###############################

project(muscleOptimizeTool)

set(muscleOptimizeTool_HEADERS  include/OpenSim/Tools/MuscleOptimizeTool.h
                                include/OpenSim/Tools/MuscleOptimizer.h)
set(muscleOptimizeTool_HELPERS  include/OpenSim/Tools/osimToolsDLL.h)
set(muscleOptimizeTool_SOURCES  src/MuscleOptimizeTool.cpp
                                src/MuscleOptimizer.cpp )

include_directories(${OPENSIMSIMBODY_INCLUDE_DIRS} include)

add_library(muscleOptimizeTool ${muscleOptimizeTool_SOURCES} ${muscleOptimizeTool_HEADERS} ${muscleOptimizeTool_HELPERS})

target_link_libraries(muscleOptimizeTool ${OPENSIMSIMBODY_LIBRARIES})
set_target_properties(muscleOptimizeTool PROPERTIES DEFINE_SYMBOL OSIMTOOLS_EXPORTS PROJECT_LABEL "Libraries - MuscleOptimizerTool")

set(muscleOptimizeTool_INCLUDE_DIRS  "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/include/OpenSim/Tools" ${OPENSIMSIMBODY_INCLUDE_DIRS} CACHE INTERNAL "")
