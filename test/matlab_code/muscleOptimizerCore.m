%__________________________________________________________________________
% Author: Luca Modenese, August 2014
% email: l.modenese@griffith.edu.au
%
% DO NOT REDISTRIBUTE WITHOUT PERMISSION
%__________________________________________________________________________
function muscleOptimizerCore(osimModel_scaled_file, osimModel_file, n_eval, output_file)

% importing OpenSim libraries
import org.opensim.modeling.*
% import models
osimModel = Model(osimModel_file);
osimModel_scaled = Model(osimModel_scaled_file);

% get muscles
muscles = osimModel.getMuscles;
muscles_scaled = osimModel_scaled.getMuscles;

% init model
si = osimModel.initSystem;
si_scaled = osimModel_scaled.initSystem;

% initialize with recognizable values
LmOptLts_opt = ones(muscles.getSize,2)*(-1000);

for n_mus = 0:muscles.getSize-1
    tic

    % current muscle name (here so that it is possible to choose a single
    % muscle when developing.
    curr_mus_name = char(muscles.get(n_mus).getName);
    display(['processing mus ',num2str(n_mus+1),': ',char(curr_mus_name)]);

    % import muscles
    curr_mus = muscles.get(n_mus);
    curr_mus_scaled = muscles_scaled.get(curr_mus_name);

    % extracting the muscle behaviour from the template
    LmOptLts = [curr_mus.getOptimalFiberLength, curr_mus.getTendonSlackLength];
    PenAngleOpt = curr_mus.getPennationAngleAtOptimalFiberLength();
    Mus_template = SampleMuscleQuantities(osimModel,curr_mus,'all',n_eval);

    % calculating minimum fiber length before having pennation 90 deg
    limitPenAngle = acos(0.1);
    % this is the minimum length the fiber can be for geometrical reasons.
    LfibNorm_min = sin(PenAngleOpt)/sin(limitPenAngle);
    % LfibNorm as calculated above can be shorter than the minimum length
    % at which the fiber can generate force (taken to be 0.5 Zajac 1989)
    if (LfibNorm_min<0.5)==1
        LfibNorm_min = 0.5;
    end

    % checking the muscle configuration that do not respect the condition.
    LfibNorm = Mus_template(:,2);
    okList = (LfibNorm>LfibNorm_min);

    % keeping only acceptable values
    LfibNorm    = LfibNorm(okList);
    LtenNorm    = Mus_template(okList,3)/LmOptLts(2);
    Ffib        = Mus_template(okList,4);
    MTL         = Mus_template(okList,1);
    penAngle    = Mus_template(okList,5);
    LfibNormOnTen = LfibNorm.*cos(penAngle);

    % in the target only MTL is needed for all muscles
    MTL_scaled = SampleMuscleQuantities(osimModel_scaled,curr_mus_scaled,'MTL',n_eval);
    MTL_scaled = MTL_scaled(okList)';

    % PROBLEM TO BE SOLVED: [LmNorm*cos(penAngle) LtNorm]*[Lmopt Lts]' = MTL;
    % written as
    A = [LfibNormOnTen LtenNorm];
    b = MTL_scaled;

    % ===== LINSOL =======
    x = A\b;
    %x =lsqnonneg(A, b);
    LmOptLts_opt(n_mus+1,:) = x;

    % ====== TEST1 =======
    % Does fminunc give the same results as A\b?
%     x0 = LmOptLts';
%     costFun = @(x) norm(A*x-b).^2.0;
%     [xopt, fval, exitflag] = fminunc(costFun, x0);
%     TEST1_LmOptLts_opt(n_mus+1,1:2) = xopt;
%     TEST1_diff(:,n_mus+1) = LmOptLts_opt(n_mus+1,:)-TEST1_LmOptLts_opt(n_mus+1,1:2);
%     if max(max(TEST1_diff))<10^-7;
%         display('TEST1 PASSED')
%     else
%         warndlg(['TEST1 NOT PASSED FOR MUS: ', curr_mus_name])
%     end

%     % ====== TEST2 =======
%     % What about fmincon?
%         % building inequalities
%     LfinNormNoF = 0.2;
%     Adis = [LfibNormOnTen,                  ones(size(LfibNormOnTen));...
%            LfinNormNoF*ones(size(LtenNorm)), LtenNorm];
%     bdis = [b;             b];
%
%     disEq = Adis*LmOptLts_opt(n_mus+1,:)'-bdis<0;
%     failedDisEq(n_mus+1,1:2) = [sum(~disEq), length(disEq)];

%     % checks
%     costFun = @(x) norm(A*x-b).^2.0;
%     lb = [1; 1].*0.00001;
%     ub = [1; 1].*max(b);
%     x0 = LmOptLts';
%     [xopt,fval,exitflag] = fmincon(costFun, x0, Adis, bdis, [], [], lb, ub);
%     TEST2_LmOptLts_opt(n_mus+1,1:2) = xopt;
%     TEST2_diff(:,n_mus+1) = LmOptLts_opt(n_mus+1,:)-TEST2_LmOptLts_opt(n_mus+1,1:2);
% %     if max(max(TEST2_diff))<10^-7;
% %         display('TEST2 PASSED')
% %     else
% %         display(['TEST2 NOT PASSED FOR MUS: ', curr_mus_name])
% %     end

    % calculating the error
    fval = norm(A*x-b).^2.0;

    % update muscles from scaled model
    curr_mus_scaled.setOptimalFiberLength(LmOptLts_opt(n_mus+1,1));
    curr_mus_scaled.setTendonSlackLength(LmOptLts_opt(n_mus+1,2));

    % PRINT OUTPUTS
    diary('logfile.txt')% save to log file
    display('  ');
    display(['Calculated optimized muscle parameters for ', char(curr_mus),' in ',num2str(toc),' seconds.'])
    % if length_alert_flag; display([num2str(n_skipped),'/',num2str(optInfos.n_points),' fiber lengths are shorter than theorical minimum']);end;
    display( '                         Lm Opt        Lts'   );
    display(['Template model       : ',num2str(LmOptLts)]);
    display(['Optimized param      : ',num2str(LmOptLts_opt(n_mus+1,:))]);
    % display(['Nr of eval points    : ',num2str(optInfos.n_points)])
    % display(['Optim type           : ',optim_type]);
    % display(['Algorithm            : ',alg]);
    display(['fval                 : ',num2str(fval)]);
%     display(['Exitflag: ', num2str(exitflag)]);
    display(['var from template [%]: ',num2str(100*(abs(LmOptLts-LmOptLts_opt(n_mus+1,:)))./LmOptLts),'%'])
    display('  ');
    diary off

end

%write updated model
osimModel_scaled.print(output_file)

end
