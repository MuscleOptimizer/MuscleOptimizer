# Author: Elena Ceseracciu
find_package(SWIG)
find_package(Java)
find_package(JNI)

if(SWIG_FOUND AND JAVA_FOUND AND JNI_FOUND)

    include(${SWIG_USE_FILE})
    set(CMAKE_SWIG_FLAGS "-package;org.opensim.muscleoptimizer;-Wall")
    set(CMAKE_SWIG_OUTDIR "${CMAKE_SOURCE_DIR}/Gui/muscleoptimizerGUI/src/org/opensim/muscleoptimizer")

    include_directories(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
    include_directories(${muscleOptimizeTool_INCLUDE_DIRS})
    
    # Try to locate OpenSim's source, needed to find their swig and wrapping related files
    find_path(Opensim_SOURCE_DIR    NAMES OpenSim/Wrapping/Java/swig/javaWrapOpenSim.i 
                                    HINTS ENV OPENSIM_HOME ENV OPENSIM_SOURCE)
    if(Opensim_SOURCE_DIR)
        include_directories("${Opensim_SOURCE_DIR}/OpenSim/Wrapping/Java/swig" "${Opensim_SOURCE_DIR}" "${Opensim_SOURCE_DIR}/OpenSim/Wrapping")
        set_source_files_properties(muscleOptimizer.i PROPERTIES CPLUSPLUS ON)
        swig_add_module(muscleOptimizerJ java muscleOptimizer.i)

        swig_link_libraries(muscleOptimizerJ muscleOptimizeTool)
        set_target_properties(muscleOptimizerJ PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # don't know exactly why I have to do this for swig targets
                                                                                                         # (apparently DLLs are considered library, not runtime)

    else()
        message(WARNING "Could not locate Opensim's source directory, please download the source tree and set the OPENSIM_SOURCE environment variable pointing to it")
    endif()
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