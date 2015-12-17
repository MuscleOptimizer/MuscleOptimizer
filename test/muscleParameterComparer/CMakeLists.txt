# Author: Elena Ceseracciu, October 2014

project(muscleParameterComparer)

include_directories(${OPENSIMSIMBODY_INCLUDE_DIRS})
add_executable(muscleParameterComparer muscleParameterComparer.cpp)
target_link_libraries(muscleParameterComparer ${OPENSIMSIMBODY_LIBRARIES})
