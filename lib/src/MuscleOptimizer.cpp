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
#include <OpenSim/Common/Units.h>
#include <OpenSim/Simulation/Model/BodySet.h>
#include <OpenSim/Simulation/Model/MarkerSet.h>
#include <OpenSim/Simulation/Model/Marker.h>
#include <OpenSim/Simulation/Model/ForceSet.h>
#include <OpenSim/Simulation/SimbodyEngine/CoordinateCouplerConstraint.h>
#include <OpenSim/Actuators/MuscleFixedWidthPennationModel.h>
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
MuscleOptimizer::MuscleOptimizer()
{
    constructProperties();
    _printResultFiles = true;
}

//_____________________________________________________________________________
/**
 * Destructor.
 */
MuscleOptimizer::~MuscleOptimizer()
{
}


//=============================================================================
// CONSTRUCTION
//=============================================================================
//_____________________________________________________________________________
/**
 * Construct properties.
 */
void MuscleOptimizer::constructProperties()
{
    constructProperty_apply(true);
    constructProperty_coordinates();
    constructProperty_muscles();
    constructProperty_n_evaluation_points(10);
    constructProperty_min_degrees_increment(2.5);
    constructProperty_output_model_file("");
}

//_____________________________________________________________________________
/**
 * Register the types used by this class.
 */
void MuscleOptimizer::registerTypes()
{
}

//=============================================================================
// UTILITY
//=============================================================================
//_____________________________________________________________________________
bool MuscleOptimizer::processModel(Model* inputModel, Model* referenceModel, const std::string& aPathToSubject)
{

    if (!get_apply()) return false;

    try
    {
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
                std::cout << "Optimizing muscle: " << currentMuscleName << "; ";
                // Reset models' poses
                SimTK::State& referenceInitialState = referenceModel->initSystem();
                SimTK::State& inputInitialState = inputModel->initSystem();

                MuscleOptimizer::CoordinateCombinations coordCombinations = sampleROMsForMuscle(*referenceModel, referenceInitialState, currentMuscleName, get_n_evaluation_points());
                if (coordCombinations.size()>0)
                {
                    std::cout << "using coordinates [ ";
                    for (auto& cit : coordCombinations)
                        std::cout << cit.first << " ";
                    std::cout << "], total no. of combinations " << coordCombinations.at(0).second.size() << endl;
                }
                else
                {
                    std::cout << "no coordinates for " << currentMuscleName << ", skipping optimization" << std::endl;
                    continue;
                }
                if (coordCombinations.size() > 0 && coordCombinations.at(0).second.size() < get_n_evaluation_points() / 2) //just a check that the sampling did not fail
                    std::cout << "WARNING! no. of coordinate combinations is less than half the number of eval points" << endl;
                std::vector<TemplateMuscleInfo> templateQuantities = sampleTemplateQuantities(*referenceModel, referenceInitialState, currentMuscleName, coordCombinations);


                SimTK::Vector targetMTUlength = sampleMTULength(*inputModel, inputInitialState, currentMuscleName, coordCombinations);

                SimTK::Matrix A(templateQuantities.size(), 2);
                for (size_t i = 0; i < templateQuantities.size(); ++i)
                {
                    A(i, 0) = templateQuantities.at(i).normalizedFiberLength*cos(templateQuantities.at(i).pennationAngle);
                    A(i, 1) = templateQuantities.at(i).normalizedTendonLength;
                }

                SimTK::Vector x;
                SimTK::FactorQTZ qtz(A);
                qtz.solve(targetMTUlength, x);


                if (x[0] <= 0 || x[1] <= 0)
                {
                    std::cout << "Negative value estimate for muscle parameter of muscle " << currentMuscleName << std::endl;

                    if (max(A.col(1)) - min(A.col(1)) < 0.0001)
                        std::cout << "Tendon length not changing throughout range of motion" << std::endl;

                    SimTK::Vector templateMTUlength = sampleMTULength(*referenceModel, referenceInitialState, currentMuscleName, coordCombinations);

                    SimTK::Vector Lfib_targ(targetMTUlength.size());
                    for (int i = 0; i < Lfib_targ.size(); ++i)
                    {
                        double Lten_fraction = A(i, 1)*referenceModel->getMuscles().get(currentMuscleName).getTendonSlackLength() / templateMTUlength(i);
                        Lfib_targ(i) = (1 - Lten_fraction)*targetMTUlength(i);
                    }
                    // first round - estimate Lopt, assuming that the same proportion between fiber and tendon in kept as in reference muscle
                    SimTK::Matrix A_1(A.col(0));
                    SimTK::FactorQTZ qtz1(A_1);
                    SimTK::Vector x1(1);
                    qtz1.solve(Lfib_targ, x1);
                    //second round - estimate Lts, calculating tendon length from previous results on fiber length
                    SimTK::Matrix A_2(A.col(1));
                    SimTK::Vector b_2 = targetMTUlength - (A_1*x1(0)).col(0);
                    SimTK::FactorQTZ qtz2(A_2);
                    SimTK::Vector x2(1);
                    qtz2.solve(b_2, x2);
                    x(0) = x1(0);
                    x(1) = x2(0);
                }

                inputModel->getMuscles()[im].setOptimalFiberLength(x(0));
                inputModel->getMuscles()[im].setTendonSlackLength(x(1));

            }

        }

        if (_printResultFiles) {

            if (!getProperty_output_model_file().getValueIsDefault() && get_output_model_file() != "")
            {
                std::string outputModelFileName = isAbsolute(get_output_model_file().c_str()) ? get_output_model_file() : aPathToSubject + get_output_model_file();
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

    //TODO: check that all columns have the same length
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
            if (model.getMuscles().get(muscleName).getForce(si) == 0.0 || model.getMuscles().get(muscleName).getNormalizedFiberLength(si)<0.5)
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
                            result.push_back(std::pair<string, vector<double> >(indepCoordName, vector<double>()));
                            actualDofsPerJoint++;
                        }
                    }
                }
                else
                {
                    result.push_back(std::pair<string, vector<double> >(coordinatesForCurrentJoint.get(iDof).getName(), vector<double>()));
                    actualDofsPerJoint++;
                }
            }
        }

        for (unsigned int iActualDof = 0; iActualDof < actualDofsPerJoint; ++iActualDof)
            nEvalPerDof.push_back(nEval);
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
        OpenSim::Units degrees(OpenSim::Units::Degrees);
        double min_radians_increment = degrees.convertTo(OpenSim::Units::Radians, get_min_degrees_increment());
        if (get_min_degrees_increment() > 0 && degIncrement < min_radians_increment)
            degIncrement = min_radians_increment;

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
    if (getProperty_muscles().getValueIsDefault() || getProperty_muscles().findIndex(muscleName) >= 0)
        return true;
    if (getProperty_muscles().size() == 0)
        return true;
    if (getProperty_muscles().size() > 0 && (get_muscles(0) == "ALL" || get_muscles(0) == ""))
        return true;
    return false;
}
bool MuscleOptimizer::isEnabledCoordinate(std::string coordinateName)
{
    if (getProperty_coordinates().getValueIsDefault() || getProperty_coordinates().findIndex(coordinateName) >= 0)
        return true;
    if (getProperty_coordinates().size() == 0)
        return true;
    if (getProperty_coordinates().size() > 0 && (get_coordinates(0) == "ALL" || get_coordinates(0) == ""))
        return true;
    return false;

}

// TEMPORARY WORKAROUNDS FOR GETTING/SETTING LIST PROPERTIES
Array<std::string> MuscleOptimizer::getCoordinates()
{
    const Property<std::string>& coordProp = getProperty_coordinates();
    Array<std::string> coordList;
    coordList.ensureCapacity(coordProp.size());
    for (int i = 0; i < coordProp.size(); ++i)
        coordList.append(coordProp[i]);
    return coordList;
}

void MuscleOptimizer::setCoordinates(const Array<std::string>& newCoords)
{
    Property<std::string>& coordProp= updProperty_coordinates();

    coordProp.clear();
    for (int i = 0; i < newCoords.size(); ++i)
        coordProp.appendValue(newCoords[i]);

}

Array<std::string> MuscleOptimizer::getMuscles()
{
    const Property<std::string>& muscProp = getProperty_muscles();
    Array<std::string> muscList;
    muscList.ensureCapacity(muscProp.size());
    for (int i = 0; i < muscProp.size(); ++i)
        muscList.append(muscProp[i]);
    return muscList;
}

void MuscleOptimizer::setMuscles(const Array<std::string>& newMuscles)
{
    Property<std::string>& muscProp = updProperty_muscles();

    muscProp.clear();
    for (int i = 0; i < newMuscles.size(); ++i)
        muscProp.appendValue(newMuscles[i]);

}
