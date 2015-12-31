
add_subdirectory(muscleParameterComparer)

option (EXECUTE_MATLAB_TESTS "Launch matlab scripts to generate optimized models for comparison" OFF)
find_program(matlab_exe matlab)

if(EXECUTE_MATLAB_TESTS)
    if(matlab_exe)
        set(matlab_toolbox_path "${CMAKE_SOURCE_DIR}/matlab/MuscleParOptTool")
        if(WIN32)
            add_test(NAME matlab_processing_example1 COMMAND ${matlab_exe} -wait -automation -r "addpath('${matlab_toolbox_path}');cd('${CMAKE_CURRENT_SOURCE_DIR}/Example1');run_example1; quit" )
            add_test(NAME matlab_processing_example2 COMMAND ${matlab_exe} -wait -automation -r "addpath('${matlab_toolbox_path}');cd('${CMAKE_CURRENT_SOURCE_DIR}/Example2');run_example2;quit" )
        else()
            add_test(NAME matlab_processing_example1 COMMAND ${matlab_exe} -wait -nodisplay -r "addpath('${matlab_toolbox_path}');cd('${CMAKE_CURRENT_SOURCE_DIR}/Example1');run_example1; quit" ) #TODO: test it!!
            add_test(NAME matlab_processing_example2 COMMAND ${matlab_exe} -wait -nodisplay -r "addpath('${matlab_toolbox_path}');cd('${CMAKE_CURRENT_SOURCE_DIR}/Example2');run_example2; quit" ) #TODO: test it!!
        endif()
    else()
        message(WARNING "Matlab tests were enable but matlab was not found!\nPlease set the \"matlab_exe\" variable pointing to the matlab executable")
    endif()
endif()

add_test(NAME testCase0_process
    COMMAND muscleOptimize -S ${CMAKE_CURRENT_SOURCE_DIR}/Example1/setup_example0.xml)

add_test(NAME testCase0_compare
         COMMAND muscleParameterComparer ${CMAKE_CURRENT_SOURCE_DIR}/Example1/Optim_Models/cpp_test0_Optimized_Hamner_scaled_L.osim
         ${CMAKE_CURRENT_SOURCE_DIR}/Example1/MSK_Models/Reference_Hamner_L.osim 0.001)

add_test(NAME testExample1_process
    COMMAND muscleOptimize -S ${CMAKE_CURRENT_SOURCE_DIR}/Example1/setup_example1.xml)

add_test(NAME testExample1_compare
         COMMAND muscleParameterComparer ${CMAKE_CURRENT_SOURCE_DIR}/Example1/Optim_Models/Target_Hamner_scaled_L_opt_N10.osim
         ${CMAKE_CURRENT_SOURCE_DIR}/Example1/Optim_Models/cpp_Optimized_Hamner_scaled_L.osim 0.001)

add_test(NAME testExample2_process
         COMMAND muscleOptimize -S ${CMAKE_CURRENT_SOURCE_DIR}/Example2/setup_example2.xml)

add_test(NAME testExample2_compare
         COMMAND muscleParameterComparer ${CMAKE_CURRENT_SOURCE_DIR}/Example2/Optim_Models/Target_LHDL_Schutte_R_opt_N10.osim
         ${CMAKE_CURRENT_SOURCE_DIR}/Example2/Optim_Models/cpp_Optimized_LHDL_Schutte_R.osim 0.001)
