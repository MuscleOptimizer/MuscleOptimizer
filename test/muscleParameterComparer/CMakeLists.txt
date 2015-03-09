# Author: Elena Ceseracciu, October 2014

project(muscleParameterComparer)

include_directories(${OpenSim_INCLUDE_DIRS})
add_executable(muscleParameterComparer muscleParameterComparer.cpp)
target_link_libraries(muscleParameterComparer ${OpenSim_LIBRARIES})
