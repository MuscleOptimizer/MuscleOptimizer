# Author: Elena Ceseracciu
find_package(SWIG)
find_package(Java)
find_package(JNI)

if(SWIG_FOUND AND JAVA_FOUND AND JNI_FOUND)

    include(${SWIG_USE_FILE})
    set(CMAKE_SWIG_FLAGS "-package;org.opensim.muscleoptimizer;-Wall")
    set(CMAKE_SWIG_OUTDIR "${CMAKE_SOURCE_DIR}/Gui/src/org/opensim/muscleoptimizer")

    include_directories(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
    include_directories(${muscleOptimizeTool_INCLUDE_DIRS})

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern
                        ${CMAKE_CURRENT_SOURCE_DIR}/extern/OpenSim/Wrapping
                        ${CMAKE_CURRENT_SOURCE_DIR}/extern/OpenSim/Wrapping/Java/swig)

    set_source_files_properties(muscleOptimizer.i PROPERTIES CPLUSPLUS ON)
    swig_add_module(muscleOptimizerJ java muscleOptimizer.i)

    swig_link_libraries(muscleOptimizerJ muscleOptimizeTool)
    set_target_properties(muscleOptimizerJ PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # don't know exactly why I have to do this for swig targets
                                                                                                     # (apparently DLLs are considered library, not runtime)
    add_custom_command(TARGET muscleOptimizerJ POST_BUILD
                        COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:muscleOptimizerJ>"
                                "${CMAKE_SOURCE_DIR}/Gui/release/modules/lib/$<TARGET_FILE_NAME:muscleOptimizerJ>"
                        COMMENT "Copying to output directory")

else()
    message(WARNING "Not compiling JAVA wrappers:")
    if(NOT SWIG_FOUND)
        message(WARNING "Swig was not found")
    endif()
    if(NOT JAVA_FOUND)
        message(WARNING "Java was not found")
    endif()
    if(NOT JNI_FOUND)
        message(WARNING "JNI was not found")
    endif()
endif()