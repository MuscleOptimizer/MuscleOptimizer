project(muscleOptimize)

include_directories(${OpenSim_INCLUDE_DIRS} ${muscleOptimizeTool_INCLUDE_DIRS})

add_executable(muscleOptimize optimizeMuscles.cpp)

target_link_libraries(muscleOptimize ${OpenSim_LIBRARIES} muscleOptimizeTool)
set_target_properties(muscleOptimize PROPERTIES PROJECT_LABEL "Applications - Muscle Optimizer")
