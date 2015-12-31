%-------------------------------------------------------------------------%
% Copyright (c) 2015 Modenese L., Ceseracciu, E., Reggiani M., Lloyd, D.G.%
%                                                                         %
% Licensed under the Apache License, Version 2.0 (the "License");         %
% you may not use this file except in compliance with the License.        %
% You may obtain a copy of the License at                                 %
% http://www.apache.org/licenses/LICENSE-2.0.                             %
%                                                                         % 
% Unless required by applicable law or agreed to in writing, software     %
% distributed under the License is distributed on an "AS IS" BASIS,       %
% WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or         %
% implied. See the License for the specific language governing            %
% permissions and limitations under the License.                          %
%                                                                         %
%    Author: Luca Modenese, August 2014                                   %
%                            revised for paper May 2015                   %
%    email:    l.modenese@sheffield.ac.uk                                 % 
% ----------------------------------------------------------------------- %
%

% clear;clc;close all
% importing OpenSim libraries
import org.opensim.modeling.*

%========= USERS SETTINGS =======
% model files with paths
osimModel_ref_filepath   = 'MSK_Models/Reference_Hamner_L.osim';
osimModel_targ_filepath  = 'MSK_Models/Target_Hamner_scaled_L.osim';
OptimizedModel_folder   = 'Optim_Models'; % folder for storing optimized model
% evaluations
N_eval = 10;
%================================

% initializing folders and log file
log_folder              = OptimizedModel_folder;
% checking if results folder exists. If not, create it.
if ~isdir(OptimizedModel_folder)
    warning(['Folder ', OptimizedModel_folder, ' does not exist. It will be created.'])
    mkdir(OptimizedModel_folder);
end

% optimizing target model based on reference model fro N_eval points per
% degree of freedom
[osimModel_opt, SimsInfo{N_eval}] = optimMuscleParams(osimModel_ref_filepath, osimModel_targ_filepath, N_eval, log_folder);

% printing the optimized model
osimModel_opt.print(fullfile(OptimizedModel_folder, char(osimModel_opt.getName())));

    
