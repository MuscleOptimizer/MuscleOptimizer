clear;clc
close all
%========= SETTINGS =============
n_eval = 10;
osimModel_scaled_file = '../WCB/1b_preprocessedTargetModel/Target_GenericFemale_WCB_Schuttev3.2_wPennAngles.osim';
osimModel_template_file = '../WCB/1_Starting_MSK_Models/Template_Case3_Arnold_R.osim';

output_file='../WCB/output_matlab/Optimized_Target_GenericFemale_WCB_Schuttev3.2_wPennAngles.osim';
%================================
muscleOptimizerCore(osimModel_scaled_file, osimModel_template_file, n_eval, output_file)
