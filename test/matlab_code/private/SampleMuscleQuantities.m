%__________________________________________________________________________
% Author: Luca Modenese, March 2013
%                        revised July 2014 (improved management of joints)
%                        modified May 2015 (management of max resolution)
% email: l.modenese@griffith.edu.au
% DO NOT REDISTRIBUTE WITHOUT PERMISSION
%__________________________________________________________________________
%
% Given as INPUT an OpenSim muscle OSModel, OSMuscle, initial State and a nr
% of evaluation points this function returns as
% OUTPUT a vector of the normalized fiber lengths
% obtained by sampling the ROM of the joint spanned by the muscle in
% N_EvalPoints evaluation points.
% For multidof joint the combinations of ROMs are considered.
% For multiarticular muscles the combination of ROM are considered.
% The script is totally general apart from a check on patello femoral
% joint. The generality is achieved by generating strings of code that are
% evaluated at the end.
%
% IMPORTANT 1
% The function has an internal check for decrease the
% N_EvalPoints if there are too many dof involved (ASSUMPTION is that a
% better sampling will be vanified by the huge amount of data generated).
%
% IMPORTANT 2
% Another check is done on the dofs: only INDEPENDENT coordinate are
% considered. This is fundamental for patellofemoral joint that both in
% LLLM and Arnold's model are constrained dof, dependent on the knee
% flexion angle. This function assumes to be used with Arnold's model.
% currentState is initialState;
%
% IMPORTANT 3
% At the purposes of the muscle optimizer it is important that here the
% model is initialize every time. So there are no risk of working with an
% old state (important for Schutte muscles for instance).

function musOutput = SampleMuscleQuantities(osimModel,OSMuscle,muscleQuant, N_EvalPoints)

%======= SETTINGS ======
% limit (1) or not (0) the discretization of the joint space sampling
limit_discr = 0;
% minimum angular discretization
min_increm_in_deg = 2.5;
%=======================

% initialize the model
currentState = osimModel.initSystem();

% getting the joint crossed by a muscle
muscleCrossedJointSet = GetJointsSpannedByMuscle(osimModel, OSMuscle);

% index for effective dofs
n_dof = 1;
DOF_Index = [];
for n_joint = 1:size(muscleCrossedJointSet,2)

    % current joint
    curr_joint = muscleCrossedJointSet{n_joint};

    % get CoordinateSet for the crossed joint
    curr_joint_CoordinateSet = osimModel.getJointSet().get(curr_joint).getCoordinateSet();

    % Initial estimation of the nr of Dof of the CoordinateSet for that
    % joint before checking for locked and constraint dofs.
    nDOF = osimModel.getJointSet().get(curr_joint).getCoordinateSet().getSize();

    % skip welded joint and removes welded joint from muscleCrossedJointSet
    if nDOF == 0;
        continue;
    end

    % calculating effective dof for that joint
    effect_DOF = nDOF;
    for n_coord = 0:nDOF-1

        % get coordinate
        curr_coord = curr_joint_CoordinateSet.get(n_coord);
        curr_coord_name = char(curr_coord.getName());

        % skip dof if locked
        if curr_coord.getLocked(currentState)
            continue;
        end

        % if coordinate is constrained then the independent coordinate and
        % associated joint will be listed in the sampling "map"
        if curr_coord.isConstrained(currentState) && ~curr_coord.getLocked(currentState)
            constraint_coord_name = curr_coord_name;
            % finding the independent coordinate
            [ind_coord_name, ind_coord_joint_name] = getIndipCoordAndJoint(osimModel, constraint_coord_name); %#ok<NASGU>
            % updating the coordinate name to be saved in the list
            curr_coord_name = ind_coord_name;
            effect_DOF = effect_DOF-1;
            % ignoring constrained dof if they point to an independent
            % coordinate that has already been stored
            if sum(ismember(DOF_Index, osimModel.getCoordinateSet.getIndex(curr_coord_name)))>0
                continue
            end
            % skip dof if independent coordinate locked (the coord
            % correspondent to the name needs to be extracted)
            if osimModel.getCoordinateSet.get(curr_coord_name).getLocked(currentState)
                continue;
            end
        end

        % NB: DOF_Index is used later in the string generated code.
        % CRUCIAL: the index of dof now is model based ("global") and
        % different from the joint based used until now.
        DOF_Index(n_dof) = osimModel.getCoordinateSet.getIndex(curr_coord_name); %#ok<AGROW>

        % necessary update/reload the curr_coord to avoid problems with
        % dependent coordinates
        curr_coord = osimModel.getCoordinateSet.get(DOF_Index(n_dof));

        % Getting the values defining the range
        jointRange(1) = curr_coord.getRangeMin();
        jointRange(2) = curr_coord.getRangeMax();

        % Storing range of motion conveniently
        CoordinateBoundaries(n_dof) = {jointRange}; %#ok<AGROW,NASGU>

        % increments in the variables when sampling the mtl space.
        % Increments are different for each dof and based on N_eval.
        %Defining the increments
        degIncrem(n_dof) = (jointRange(2)-jointRange(1))/(N_EvalPoints-1);

        % limit or not the discretization of the joint space sampling
        if limit_discr==1
            % a limit to the increase can be set though
            if degIncrem(n_dof)<(min_increm_in_deg/180*pi)
                degIncrem(n_dof)=(min_increm_in_deg/180*pi); %#ok<*AGROW>
            end
        end

        % updating index of list of dof
        n_dof = n_dof+1;
    end
end


% initializes the counter to save the results
iter = 1;  %#ok<NASGU>

% assigns an initial and a final value variable for each dof X
% calling them setAngleStartDofX and setLimitDof respectively.
for n_instr = 1:size(DOF_Index,2);
    % setting up the variables
    eval(['setAngleStartDof',num2str(n_instr),' = CoordinateBoundaries{',num2str(n_instr),'}(1);'])
    eval(['setLimitDof',num2str(n_instr),' = CoordinateBoundaries{',num2str(n_instr),'}(2);'])
end


% setting up for loops in order to explore all the possible combination of
% joint angles (looping on all the dofs of each joint for all the joint
% crossed by the muscle).
% The model pose is updated via: " coordToUpd.setValue(currentState,setAngleDof)"
% The right dof to update is chosen via: "coordToUpd = osimModel.getCoordinateSet.get(n_instr)"
stringToExcute1 = '';
for n_instr = 1:size(DOF_Index,2);
    stringToExcute1 = [stringToExcute1,[' for setAngleDof',num2str(n_instr),'=setAngleStartDof',num2str(n_instr),':degIncrem(',num2str(n_instr),'):setLimitDof',num2str(n_instr),';   coordToUpd = osimModel.getCoordinateSet.get(DOF_Index(',num2str(n_instr),'));    coordToUpd.setValue(currentState,setAngleDof',num2str(n_instr),');']]; %#ok<AGROW>
end

% calculating muscle length for the muscle
switch muscleQuant
    case 'MTL'
            stringToExcute2 = 'musOutput(iter) = OSMuscle.getGeometryPath.getLength(currentState);';
    case 'LfibNorm'
            stringToExcute2 =  ['OSMuscle.setActivation(currentState,1.0);',...
                                ' osimModel.equilibrateMuscles(currentState);',...
                                ' musOutput(iter) = OSMuscle.getNormalizedFiberLength(currentState);'];
    case 'Lten'
            stringToExcute2 =  ['OSMuscle.setActivation(currentState,1.0);',...
                                ' osimModel.equilibrateMuscles(currentState);',...
                                ' musOutput(iter) = OSMuscle.getTendonLength(currentState);'];
    case 'Ffib'
            stringToExcute2 =  ['OSMuscle.setActivation(currentState,1.0);',...
                                ' osimModel.equilibrateMuscles(currentState);',...
                                ' musOutput(iter) = OSMuscle.getActiveFiberForce(currentState);'];
    case 'all'
            stringToExcute2 = [ 'OSMuscle.setActivation(currentState,1.0);',...
                                ' osimModel.equilibrateMuscles(currentState);',...
                                'musOutput(iter,1) = OSMuscle.getGeometryPath.getLength(currentState);',...
                                ' musOutput(iter,2) = OSMuscle.getNormalizedFiberLength(currentState);',...
                                ' musOutput(iter,3) = OSMuscle.getTendonLength(currentState);',...
                                ' musOutput(iter,4) = OSMuscle.getActiveFiberForce(currentState);',...
                                ' musOutput(iter,5) = OSMuscle.getPennationAngle(currentState);'];
end

% updating iteration
stringToExcute3 =['iter = iter+1;',...
%     'angles.colheaders(iter-1) = {char(osimModel.getCoordinateSet.get(DOF_Index(',num2str(n_instr),')).getName)};'...
%     'angles.data(iter-1,1) = setAngleDof',num2str(n_instr),';'...
    ''];

% closing the loops started in stringToExcute1 with appropriate nr of end
stringToExcute4 = '';
for n_instr = 1:size(DOF_Index,2);
    stringToExcute4 =[stringToExcute4,'end;']; %#ok<AGROW>
end

% assembling the code string
Code = [stringToExcute1,stringToExcute2,stringToExcute3,stringToExcute4];

% evaluate the code
eval(Code)

end
%__________________________________________________________________________
% Author: Luca Modenese, June 2013
% email: l.modenese@griffith.edu.au
% DO NOT REDISTRIBUTE WITHOUT PERMISSION
%__________________________________________________________________________
%
% Given as INPUT a muscle OSMuscleName from an OpenSim model, this function
% returns the OUTPUT structure jointNameSet containing the OpenSim jointNames
% crossed by the OSMuscle.
%
% It works through the following steps:
%   1) extracts the GeometryPath
%   2) loops through the single points, determining the body the belong to
%   3) stores the bodies to which the muscle points are attached to
%   4) determines the nr of joints based on body indexes
%   5) stores the crossed OpenSim joints in the output structure named jointNameSet
%
% NB this function return the crossed joints independently on the
% constraints applied to the coordinates. Eg the patella is considered as a
% joint, although in LLLM and Arnold's model it does not have independent
% coordinates.
%__________________________________________________________________________


function [jointNameSet,varargout] = GetJointsSpannedByMuscle(osimModel, OSMuscleName)

% just in case the OSMuscleName is given as java string
OSMuscleName = char(OSMuscleName);

%useful initializations
BodySet = osimModel.getBodySet();
muscle  = osimModel.getMuscles.get(OSMuscleName);

% Extracting the PathPointSet via GeometryPath
musclePath = muscle.getGeometryPath();
musclePathPointSet = musclePath.getPathPointSet();

% for loops to get the attachment bodies
n_body = 1;
jointNameSet = [];
muscleAttachBodies = '';
muscleAttachIndex = [];
for n_point = 0:musclePathPointSet.getSize()-1

    % get the current muscle point
    currentAttachBody = char(musclePathPointSet.get(n_point).getBodyName());

    %Initialize
    if n_point ==0;
        previousAttachBody = currentAttachBody;
        muscleAttachBodies{n_body} = currentAttachBody;
        muscleAttachIndex(n_body) = BodySet.getIndex(currentAttachBody);
        n_body = n_body+1;
    end;

    % building a vectors of the bodies attached to the muscles
    if ~strncmp(currentAttachBody,previousAttachBody, size(char(currentAttachBody),2))
        muscleAttachBodies{n_body} = currentAttachBody;
        muscleAttachIndex(n_body) = BodySet.getIndex(currentAttachBody);
        previousAttachBody = currentAttachBody;
        n_body = n_body+1;
    end
end

% From distal body checking the joint names going up until the desired
% OSJointName is found or the proximal body is reached as parent body.
DistalBodyName = muscleAttachBodies{end};
bodyName = DistalBodyName;
ProximalBodyName= muscleAttachBodies{1};
body =  BodySet.get(DistalBodyName);
spannedJointNameOld = '';
n_spanJoint = 1;
n_spanJointNoDof = 1;
NoDofjointNameSet = [];
jointNameSet = [];
while ~strcmp(bodyName,ProximalBodyName)

    spannedJoint = body.getJoint();
    spannedJointName = char(spannedJoint.getName());

    if strcmp(spannedJointName, spannedJointNameOld)
         body =  spannedJoint.getParentBody();
         spannedJointNameOld = spannedJointName;
    else
            if spannedJoint.getCoordinateSet().getSize()~=0

        jointNameSet{n_spanJoint} =  spannedJointName;
        n_spanJoint = n_spanJoint+1;
            else
                NoDofjointNameSet{n_spanJointNoDof} =  spannedJointName;
                n_spanJointNoDof = n_spanJointNoDof+1;
            end
        spannedJointNameOld = spannedJointName;
        body =  spannedJoint.getParentBody();
    end
    bodyName = char(body.getName());
end

if isempty(jointNameSet)
    error(['No joint detected for muscle ',OSMuscleName]);
end
if  ~isempty(NoDofjointNameSet)
    for n_v = 1:length(NoDofjointNameSet)
        display(['Joint ',NoDofjointNameSet{n_v},' has no dof.'])
    end
end

varargout = NoDofjointNameSet;
end