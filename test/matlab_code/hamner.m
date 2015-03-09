clear;clc
close all
% importing OpenSim libraries
import org.opensim.modeling.*

%========= SETTINGS =============
%osimModel_scaled_file = 'gait2392_R_ArnoldROM_locked_scaled.osim';
%osimModel_file = 'gait2392_R_only_ArnoldROM.osim';
osimModel_scaled_file = '../Hamner/5_Starting_MSK_Models/Target_Hamner_Scaled_adj_rangeROM_L.osim';
osimModel_template_file = '../Hamner/5_Starting_MSK_Models/Template_Hamner_Generic_adj_rangeROM_L.osim';
n_eval = 10;

output_file= '../Hamner/output_matlab/Optimized_Target_Hamner_Scaled_adj_rangeROM_L.osim';
%================================
muscleOptimizerCore(osimModel_scaled_file, osimModel_template_file, n_eval, output_file);
