
add_subdirectory(muscleParameterComparer)

option (EXECUTE_MATLAB_TESTS "Launch matlab scripts to generate optimized models for comparison" OFF)
find_program(matlab_exe matlab)

if(EXECUTE_MATLAB_TESTS)
    if(matlab_exe)
        if(WIN32)
            add_test(NAME matlab_processing_hamner COMMAND ${matlab_exe} -wait -automation -r "cd('${CMAKE_CURRENT_SOURCE_DIR}/matlab_code');hamner; quit" )
            add_test(NAME matlab_processing_wcb COMMAND ${matlab_exe} -wait -automation -r "cd('${CMAKE_CURRENT_SOURCE_DIR}/matlab_code');wcb;quit" )
        else()
            add_test(NAME matlab_processing_hamner COMMAND ${matlab_exe} -wait -nodisplay -r "cd('${CMAKE_CURRENT_SOURCE_DIR}/matlab_code');hamner; quit" ) #TODO: test it!!
            add_test(NAME matlab_processing_wcb COMMAND ${matlab_exe} -wait -nodisplay -r "cd('${CMAKE_CURRENT_SOURCE_DIR}/matlab_code');wcb; quit" ) #TODO: test it!!
        endif()
    else()
        message(WARNING "Matlab tests were enable but matlab was not found!\nPlease set the \"matlab_exe\" variable pointing to the matlab executable")
    endif()
endif()

add_test(NAME testCase0_process
    COMMAND muscleOptimize -S ${CMAKE_CURRENT_SOURCE_DIR}/Hamner/basicTest.xml)

add_test(NAME testCase0_compare
         COMMAND muscleParameterComparer ${CMAKE_CURRENT_SOURCE_DIR}/Hamner/cpp_test0_Optimized_Hamner_Scaled_adj_rangeROM_L.osim
         ${CMAKE_CURRENT_SOURCE_DIR}/Hamner/5_Starting_MSK_Models/Template_Hamner_Generic_adj_rangeROM_L.osim 0.01)

add_test(NAME testHamner_process
    COMMAND muscleOptimize -S ${CMAKE_CURRENT_SOURCE_DIR}/Hamner/hamner.xml)

add_test(NAME testHamner_compare
         COMMAND muscleParameterComparer ${CMAKE_CURRENT_SOURCE_DIR}/Hamner/output_matlab/Optimized_Target_Hamner_Scaled_adj_rangeROM_L.osim
         ${CMAKE_CURRENT_SOURCE_DIR}/Hamner/output_cpp/cpp_Optimized_Hamner_Scaled_adj_rangeROM_L.osim 0.01)

add_test(NAME testWCB_process
         COMMAND muscleOptimize -S ${CMAKE_CURRENT_SOURCE_DIR}/WCB/wcb.xml)

add_test(NAME testWCB_compare
         COMMAND muscleParameterComparer ${CMAKE_CURRENT_SOURCE_DIR}/WCB/output_matlab/Optimized_Target_GenericFemale_WCB_Schuttev3.2_wPennAngles.osim
         ${CMAKE_CURRENT_SOURCE_DIR}/WCB/output_cpp/cpp_Optimized_Target_GenericFemale_WCB_Schuttev3.2.osim 0.11)
