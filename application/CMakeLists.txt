project(muscleOptimize)

include_directories(${OPENSIMSIMBODY_INCLUDE_DIRS} ${muscleOptimizeTool_INCLUDE_DIRS})

add_executable(muscleOptimize optimizeMuscles.cpp)

target_link_libraries(muscleOptimize ${OPENSIMSIMBODY_LIBRARIES} muscleOptimizeTool)
set_target_properties(muscleOptimize PROPERTIES PROJECT_LABEL "Applications - Muscle Optimizer")
