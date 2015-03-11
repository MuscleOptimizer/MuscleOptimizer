// Authors: Elena Ceseracciu, Luca Modenese, Claudio Pizzolato

//=============================================================================
// INCLUDES
//=============================================================================
#include <OpenSim/Common/ScaleSet.h>
#include "OpenSim/Tools/MuscleOptimizer.h"
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/SimbodyEngine/SimbodyEngine.h>
#include <OpenSim/Common/MarkerData.h>
#include <OpenSim/Common/IO.h>
#include <OpenSim/Simulation/Model/BodySet.h>
#include <OpenSim/Simulation/Model/MarkerSet.h>
#include <OpenSim/Simulation/Model/Marker.h>
#include <OpenSim/Simulation/Model/ForceSet.h>
#include <OpenSim/Simulation/SimbodyEngine/CoordinateCouplerConstraint.h>
#include <OpenSim/Actuators/MuscleFixedWidthPennationModel.h>
//#include <simmath/Optimizer.h>
#include <simmath/LinearAlgebra.h>

#include <vector>
#include <set>
#include <string>
#include <cmath>

//=============================================================================
// STATICS
//=============================================================================
using namespace std;
using namespace OpenSim;
using SimTK::Vec3;

inline bool isAbsolute(const char *path) {
    if (path[0] == '/' || path[0] == '\\') {
        return true;
    }
    std::string str(path);
    if (str.length() > 1) {
        if (str[1] == ':') {
            return true;
        }
    }
    return false;
};
//=============================================================================
// CONSTRUCTOR(S) AND DESTRUCTOR
//=============================================================================
//_____________________________________________________________________________
/**
 * Default constructor.
 */
MuscleOptimizer::MuscleOptimizer() :
_apply(_applyProp.getValueBool()),
_coordinates(_coordinatesProp.getValueStrArray()),
_muscles(_musclesProp.getValueStrArray()),
_nEvalPoints(_nEvalPointsProp.getValueDbl()),
_outputModelFileName(_outputModelFileNameProp.getValueStr())
{
    setNull();
    setupProperties();
}

//_____________________________________________________________________________
/**
 * Destructor.
 */
MuscleOptimizer::~MuscleOptimizer()
{
}

//_____________________________________________________________________________
/**
 * Copy constructor.
 *
 * @param aMuscleOptimizer MuscleOptimizer to be copied.
 */
MuscleOptimizer::MuscleOptimizer(const MuscleOptimizer &aMuscleOptimizer) :
Object(aMuscleOptimizer),
_apply(_applyProp.getValueBool()),
_coordinates(_coordinatesProp.getValueStrArray()),
_muscles(_musclesProp.getValueStrArray()),
_nEvalPoints(_nEvalPointsProp.getValueDbl()),
_outputModelFileName(_outputModelFileNameProp.getValueStr())
{
    setNull();
    setupProperties();
    copyData(aMuscleOptimizer);
}

//=============================================================================
// CONSTRUCTION
//=============================================================================
//_____________________________________________________________________________
/**
 * Copy data members from one MuscleOptimizer to another.
 *
 * @param aMuscleOptimizer MuscleOptimizer to be copied.
 */
void MuscleOptimizer::copyData(const MuscleOptimizer &aMuscleOptimizer)
{
    _apply = aMuscleOptimizer._apply;
    _coordinates = aMuscleOptimizer._coordinates;
    _muscles = aMuscleOptimizer._muscles;
    _nEvalPoints = aMuscleOptimizer._nEvalPoints;
    _outputModelFileName = aMuscleOptimizer._outputModelFileName;
}

//_____________________________________________________________________________
/**
 * Set the data members of this MuscleOptimizer to their null values.
 */
void MuscleOptimizer::setNull()
{
    _apply = true;
    _nEvalPoints = 10;
    _printResultFiles = true;
}

//_____________________________________________________________________________
/**
 * Connect properties to local pointers.
 */
void MuscleOptimizer::setupProperties()
{
    _applyProp.setComment("Whether or not to use the model optimizer during optimization");
    _applyProp.setName("apply");
    _propertySet.append(&_applyProp);

    _coordinatesProp.setComment("Specifies the coordinates (degrees of freedom) to consider (default: ALL)");
    _coordinatesProp.setName("coordinates");
    _propertySet.append(&_coordinatesProp);

    _musclesProp.setComment("Specifies the muscles to consider (default: ALL)");
    _musclesProp.setName("muscles");
    _propertySet.append(&_musclesProp);

    _nEvalPointsProp.setComment("Number of evaluation points for each degree of freedom");
    _nEvalPointsProp.setName("n_evaluation_points");
    _propertySet.append(&_nEvalPointsProp);

    _outputModelFileNameProp.setComment("Name of OpenSim model file (.osim) to write when done optimizing.");
    _outputModelFileNameProp.setName("output_model_file");
    _propertySet.append(&_outputModelFileNameProp);
}

//_____________________________________________________________________________
/**
 * Register the types used by this class.
 */
void MuscleOptimizer::registerTypes()
{
}

//=============================================================================
// OPERATORS
//=============================================================================
//_____________________________________________________________________________
/**
 * Assignment operator.
 *
 * @return Reference to this object.
 */
MuscleOptimizer& MuscleOptimizer::operator=(const MuscleOptimizer &aMuscleOptimizer)
{
    // BASE CLASS
    Object::operator=(aMuscleOptimizer);

    copyData(aMuscleOptimizer);

    return(*this);
}

//=============================================================================
// UTILITY
//=============================================================================
//_____________________________________________________________________________
bool MuscleOptimizer::processModel(Model* inputModel, Model* referenceModel, const std::string& aPathToSubject)
{

    if (!getApply()) return false;

    try
    {
        std::cout << "input Model Name: " << inputModel->getName() << ", ref model name: " << referenceModel->getName() << std::endl;
        OpenSim::Array<std::string> musclesInput, musclesReference;
        referenceModel->getMuscles().getNames(musclesReference);
        inputModel->getMuscles().getNames(musclesInput);

        // Check that all input model's muscles can be found in the reference model
        for (int im = 0; im < musclesInput.getSize(); ++im)
        {
            if (musclesReference.findIndex(musclesInput[im]) < 0)
            {
                cout << "Muscle optimizer: ERROR- Muscle " << musclesInput[im] << " could not be found in reference model! Aborting." << std::endl;
                return false;
            }
        }


        for (int im = 0; im < musclesInput.getSize(); ++im) {
            std::string currentMuscleName = musclesInput[im];
            if (isEnabledMuscle(currentMuscleName))
            {
                std::cout << "Optimizing " << currentMuscleName << endl;
                // Reset models' poses
                SimTK::State& referenceInitialState = referenceModel->initSystem();
                SimTK::State& inputInitialState = inputModel->initSystem();

                MuscleOptimizer::CoordinateCombinations coordCombinations = sampleROMsForMuscle(*referenceModel, referenceInitialState, currentMuscleName, _nEvalPoints);
                std::cout << "no of coordinates " << coordCombinations.size() << endl;
                if (coordCombinations.size() > 0 && coordCombinations.at(0).second.size() < _nEvalPoints / 2) //just a check that the sampling did not fail
                    std::cout << "WARNING! no of coordinate combinations is less than half the number of eval points" << endl;
                std::vector<TemplateMuscleInfo> templateQuantities = sampleTemplateQuantities(*referenceModel, referenceInitialState, currentMuscleName, coordCombinations);
                if (coordCombinations.size()>0)
                    std::cout << "no of combinations " << coordCombinations.at(0).second.size() << endl;

                SimTK::Vector targetMTUlength = sampleMTULength(*inputModel, inputInitialState, currentMuscleName, coordCombinations);

                SimTK::Matrix A(templateQuantities.size(), 2);
                for (size_t i = 0; i < templateQuantities.size(); ++i)
                {
                    A(i, 0) = templateQuantities.at(i).normalizedFiberLength*cos(templateQuantities.at(i).pennationAngle);
                    A(i, 1) = templateQuantities.at(i).normalizedTendonLength;
                }

                //tmp:
                std::cout << A << std::endl;
                std::cout << targetMTUlength << std::endl;

                SimTK::Vector x;

                //SimTK::FactorLU lu(A);
                //if (!lu.isSingular())
                //    lu.solve(targetMTUlength, x);     // writes into delta also
                //else {
                SimTK::FactorQTZ qtz(A);
                qtz.solve(targetMTUlength, x);    // writes into delta also
                //}

                inputModel->getMuscles()[im].setOptimalFiberLength(x(0));
                inputModel->getMuscles()[im].setTendonSlackLength(x(1));

            }

        }

        if (_printResultFiles) {

            if (!_outputModelFileNameProp.getValueIsDefault() && _outputModelFileName != "")
            {
                std::string outputModelFileName = isAbsolute(_outputModelFileName.c_str()) ? _outputModelFileName : aPathToSubject + _outputModelFileName;
                if (inputModel->print(outputModelFileName))
                    cout << "Wrote model file " << outputModelFileName << " from model " << inputModel->getName() << endl;
            }
        }

        inputModel->initializeState(); //this is to bring coordinates back to default, for the GUI

    }
    catch (const Exception& x)
    {
        x.print(cout);
        return false;
    }

    return true;
}

Array<std::string> MuscleOptimizer::getJointSpannedByMuscle(Model& model, const string& muscleName)
{
    OpenSim::Array<std::string> jointNames;

    OpenSim::BodySet bodySet(model.getBodySet());
    OpenSim::Set<OpenSim::Muscle> muscles = model.getMuscles();
    OpenSim::Muscle& muscle = muscles.get(muscleName);

    OpenSim::GeometryPath musclePath(muscle.getGeometryPath());
    OpenSim::PathPointSet musclePathPointSet(musclePath.getPathPointSet());

    int proximalBodyIndex = 0;
    OpenSim::Body& distalBody = musclePathPointSet.get(musclePathPointSet.getSize() - 1).getBody();
    OpenSim::Body& proximalBody = musclePathPointSet.get(proximalBodyIndex).getBody();

    std::string currentBodyName(distalBody.getName());
    std::string proximalBodyName(proximalBody.getName());

    const OpenSim::JointSet& jointSet = model.getJointSet();
    while (currentBodyName != proximalBodyName) {
        for (int i = 0; i < jointSet.getSize(); i++)
        {
//            if (jointSet.get(i).getChildBody().getName() == currentBodyName) //was:
            if (jointSet.get(i).getBody().getName() == currentBodyName)
            {
                if (jointSet.get(i).getCoordinateSet().getSize() != 0)
                    jointNames.append(jointSet.get(i).getName());
                currentBodyName = jointSet.get(i).getParentBody().getName();
                continue;
            }
        }
    }
    return jointNames;

}

string MuscleOptimizer::getIndependentCoordinate(Model& model, const std::string& dofName)
{
    ConstraintSet constraintSet = model.getConstraintSet();
    for (int n = 0; n < constraintSet.getSize(); ++n)
    {
        CoordinateCouplerConstraint* constraint = dynamic_cast<CoordinateCouplerConstraint*>(&constraintSet.get(n));
        if (constraint != NULL) // we can only handle this type of constraint, so we just skip the others
        {
            string depCoordinateName = constraint->getDependentCoordinateName();
            if (depCoordinateName == dofName)
            {
                const Array<std::string>& indepCoordinateNamesSet = constraint->getIndependentCoordinateNames();
                if (indepCoordinateNamesSet.getSize() == 1)
                {
                    return indepCoordinateNamesSet.get(0);
                }
                else if (indepCoordinateNamesSet.getSize() > 1)
                {
                    std::cout << "CoordinateCouplerConstraint with dependent coordinate " << dofName << " has more than one indipendent coordinate and this is not managed by this software yet." << std::endl;
                    return "";
                }
            }

        }

    }
    return "";
}

SimTK::Vector MuscleOptimizer::sampleMTULength(Model& model, SimTK::State& si, const std::string& muscleName, const MuscleOptimizer::CoordinateCombinations& coordinateCombinations)
{
    if (coordinateCombinations.size() < 1)
        return SimTK::Vector();

    //TODO: check that all columns have the same lenght
    SimTK::Vector mtuLength(coordinateCombinations.at(0).second.size()); //output variable

    model.getMuscles().get(muscleName).setActivation(si, 1.0);
    //model.getMuscles().get(muscleName).setIgnoreTendonCompliance(si, true);

    for (size_t combinationInd = 0; combinationInd < coordinateCombinations.at(0).second.size(); ++combinationInd)
    {
        for (MuscleOptimizer::CoordinateCombinations::const_iterator coordIt = coordinateCombinations.begin(); coordIt < coordinateCombinations.end(); ++coordIt)
        {
            try // in case the coordinate name is not found in the input model
            {
                model.updCoordinateSet().get((*coordIt).first).setValue(si, (*coordIt).second.at(combinationInd));
            }
            catch (const Exception& x)
            {
                x.print(cout);
                return SimTK::Vector();
            }
        }


        model.getMultibodySystem().realize(si, SimTK::Stage::Position);
        try
        {
            //model.getMuscles().get(muscleName).equilibrate(si);
            mtuLength[combinationInd] = model.getMuscles().get(muscleName).getLength(si);

        }
        catch (const Exception& x)
        {
            cout << "WARNING could not retieve MTL for muscle " << muscleName << " for combination " << combinationInd << std::endl;
            mtuLength[combinationInd] = SimTK::NaN;
        }

    }

    return mtuLength;
}

std::vector<MuscleOptimizer::TemplateMuscleInfo> MuscleOptimizer::sampleTemplateQuantities(Model& model, SimTK::State& si, const std::string& muscleName, MuscleOptimizer::CoordinateCombinations& coordinateCombinations)
{
    if (coordinateCombinations.size() < 1)
        return std::vector<MuscleOptimizer::TemplateMuscleInfo>();

    //TODO: check that all columns have the same lenght
    std::vector<MuscleOptimizer::TemplateMuscleInfo> templateQuantities;

    model.getMuscles().get(muscleName).setActivation(si, 1.0);
    //model.getMuscles().get(muscleName).setIgnoreTendonCompliance(si, true);

    MuscleFixedWidthPennationModel fibKin = MuscleFixedWidthPennationModel(model.getMuscles().get(muscleName).getOptimalFiberLength(), model.getMuscles().get(muscleName).getPennationAngleAtOptimalFiberLength(), acos(0.1));
    double minFiberLength = fibKin.getMinimumFiberLength() / model.getMuscles().get(muscleName).getOptimalFiberLength();


    vector<size_t> pointsToRemove;
    for (size_t combinationInd = 0; combinationInd < coordinateCombinations.at(0).second.size(); ++combinationInd)
    {
        for (MuscleOptimizer::CoordinateCombinations::iterator coordIt = coordinateCombinations.begin(); coordIt < coordinateCombinations.end(); ++coordIt)
        {
            try // in case the coordinate name is not found in the input model
            {
                model.updCoordinateSet().get((*coordIt).first).setValue(si, (*coordIt).second.at(combinationInd));
            }
            catch (const Exception& x)
            {
                x.print(cout);
                return std::vector<MuscleOptimizer::TemplateMuscleInfo>();
            }
        }


        model.getMultibodySystem().realize(si, SimTK::Stage::Dynamics);
        MuscleOptimizer::TemplateMuscleInfo muscleInfo;
        try
        {
            model.getMuscles().get(muscleName).equilibrate(si);
            muscleInfo.normalizedFiberLength = model.getMuscles().get(muscleName).getNormalizedFiberLength(si);
            //if (muscleInfo.normalizedFiberLength < minFiberLength || model.getMuscles().get(muscleName).getActuation(si) == 0.0) //was:
            if (model.getMuscles().get(muscleName).getForce(si) == 0.0)
            {
                pointsToRemove.push_back(combinationInd);
            }
            else
            {
                muscleInfo.normalizedTendonLength = model.getMuscles().get(muscleName).getTendonLength(si) / model.getMuscles().get(muscleName).getTendonSlackLength();
                muscleInfo.pennationAngle = model.getMuscles().get(muscleName).getPennationAngle(si);
                templateQuantities.push_back(muscleInfo);
            }
        }
        catch (const Exception& x)
        {
            pointsToRemove.push_back(combinationInd);
        }
    }

    for (vector<size_t>::reverse_iterator pointsIt = pointsToRemove.rbegin(); pointsIt != pointsToRemove.rend(); ++pointsIt)
    for (MuscleOptimizer::CoordinateCombinations::iterator coordIt = coordinateCombinations.begin(); coordIt < coordinateCombinations.end(); ++coordIt)
        (*coordIt).second.erase((*coordIt).second.begin() + *pointsIt);

    return templateQuantities;

}


MuscleOptimizer::CoordinateCombinations MuscleOptimizer::sampleROMsForMuscle(Model& model, SimTK::State& si, const std::string& muscleName, unsigned int nEval)
{
    MuscleOptimizer::CoordinateCombinations result;
    std::vector<int> nEvalPerDof;

    //Get joints crossed by muscle
    Array<std::string> crossedJoints = getJointSpannedByMuscle(model, muscleName);

    //Get list of coordinates involved in motion of joints spanned by the muscle, and put it in the result variable
    for (int jointInd = 0; jointInd < crossedJoints.getSize(); ++jointInd)
    {
        unsigned int actualDofsPerJoint = 0;
        const OpenSim::CoordinateSet& coordinatesForCurrentJoint = model.getJointSet().get(crossedJoints[jointInd]).getCoordinateSet();
        for (int iDof = 0; iDof < coordinatesForCurrentJoint.getSize(); ++iDof)
        {
            //check that this coordinate belongs to the user defined list (or this list is the default one = ALL)
            if (!isEnabledCoordinate(coordinatesForCurrentJoint.get(iDof).getName()))
                continue;
            if (!coordinatesForCurrentJoint.get(iDof).getLocked(si)
                /*&& model.updCoordinateSet().get(dofsp->get(iDof).getName()).getRangeMin() != model.updCoordinateSet().get(dofsp->get(iDof).getName()).getRangeMax()*/)
            {
                if (coordinatesForCurrentJoint.get(iDof).isConstrained(si))
                {
                    string indepCoordName = getIndependentCoordinate(model, coordinatesForCurrentJoint.get(iDof).getName());
                    if (indepCoordName != "")
                    {
                        // Not very elegant to put it here, but this is just a helper function
                        struct key_compare : public std::unary_function<std::pair<string, vector<double> >, bool>
                        {
                            explicit key_compare(const  std::string &baseline) : baseline(baseline) {}
                            bool operator() (const std::pair<string, vector<double> > &arg)
                            {
                                return arg.first == baseline;
                            }
                            std::string baseline;
                        };
                        //
                        CoordinateCombinations::iterator foundCoord = std::find_if(result.begin(), result.end(), key_compare(indepCoordName));
                        if (foundCoord == result.end() && !model.getCoordinateSet().get(indepCoordName).getLocked(si))
                        {
                            std::cout << " pushing back " << indepCoordName << std::endl;
                            result.push_back(std::pair<string, vector<double> >(indepCoordName, vector<double>()));
                            actualDofsPerJoint++;
                        }
                    }
                }
                else
                {
                    std::cout << " pushing back " << coordinatesForCurrentJoint.get(iDof).getName() << std::endl;
                    result.push_back(std::pair<string, vector<double> >(coordinatesForCurrentJoint.get(iDof).getName(), vector<double>()));
                    actualDofsPerJoint++;
                }
            }
        }

        int nEvalPerJoint;
        if (actualDofsPerJoint > 2)
        {
            nEvalPerJoint = (double)nEval / 2;
            std::cout << "Changing nr of evaluation points for Lm norm from " << nEval;
            std::cout << " to " << nEvalPerJoint << " for joint " << crossedJoints[jointInd] << std::endl;
        }
        else
            nEvalPerJoint = nEval;

        for (unsigned int iActualDof = 0; iActualDof < actualDofsPerJoint; ++iActualDof)
            nEvalPerDof.push_back(nEvalPerJoint);
    }


    unsigned int totalSamples = 1;
    for (std::vector<int>::const_iterator itNeval = nEvalPerDof.cbegin(); itNeval != nEvalPerDof.cend(); ++itNeval)
        totalSamples *= *itNeval;

    unsigned int multeplicity = 1;
    std::vector<int>::reverse_iterator itNeval = nEvalPerDof.rbegin();
    for (MuscleOptimizer::CoordinateCombinations::reverse_iterator coordIt = result.rbegin(); coordIt != result.rend(); ++coordIt)
    {
        (*coordIt).second = generateAngleSamples(model.getCoordinateSet().get((*coordIt).first).getRangeMin(), model.getCoordinateSet().get((*coordIt).first).getRangeMax(), *itNeval, multeplicity, totalSamples);
        multeplicity *= *itNeval;
        ++itNeval;
    }

    return result;
}

std::vector<double> MuscleOptimizer::generateAngleSamples(double anglesStart, double anglesEnd, unsigned int noEval, unsigned int multeplicity, unsigned int totalSamples)
{
    std::vector<double> result;
    unsigned int sampleCounter = 0;
    while (sampleCounter < totalSamples)
    {
        double degIncrement = (anglesEnd - anglesStart) / (noEval - 1);
        for (unsigned int evalInd = 0; evalInd < noEval; ++evalInd)
        {
            for (unsigned multInd = 0; multInd < multeplicity; ++multInd)
                result.push_back(anglesStart + evalInd*degIncrement);
        }
        sampleCounter += multeplicity*noEval;
    }
    return result;
}

bool MuscleOptimizer::isEnabledMuscle(std::string muscleName)
{
    if (_musclesProp.getValueIsDefault() || _muscles.findIndex(muscleName) >= 0)
        return true;
    if (_muscles.size() == 0)
        return true;
    if (_muscles.size() > 0 && (_muscles.get(0) == "ALL" || _muscles.get(0) == ""))
        return true;
    return false;
}
bool MuscleOptimizer::isEnabledCoordinate(std::string coordinateName)
{
    if (_coordinatesProp.getValueIsDefault() || _coordinates.findIndex(coordinateName) >= 0)
        return true;
    if (_coordinates.size() == 0)
        return true;
    if (_coordinates.size() > 0 && (_coordinates.get(0) == "ALL" || _coordinates.get(0) == ""))
        return true;
    return false;

}
