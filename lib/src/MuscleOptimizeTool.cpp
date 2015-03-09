// Authors: Elena Ceseracciu, Luca Modenese, Claudio Pizzolato

//=============================================================================
// INCLUDES
//=============================================================================
#include "OpenSim/Tools/MuscleOptimizeTool.h"
#include <OpenSim/Common/SimmIO.h>
#include <OpenSim/Common/IO.h>
#include <OpenSim/Simulation/Model/Model.h>
#include "SimTKsimbody.h"

//=============================================================================
// STATICS
//=============================================================================
using namespace std;
using namespace OpenSim;

inline bool isAbsolute(const char *path) {
    if (path[0] == '/' || path[0] == '\\') {
        return true;
    }
    std::string str(path);
    if (str.length()>1) {
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
MuscleOptimizeTool::MuscleOptimizeTool() :
_notes(_notesProp.getValueStr()),
_inputModelFileName(_inputModelFileNameProp.getValueStr()),
_referenceModelFileName(_referenceModelFileNameProp.getValueStr()),
_muscleOptimizerProp(PropertyObj("", MuscleOptimizer())),
_muscleOptimizer((MuscleOptimizer&)_muscleOptimizerProp.getValueObj())
{
    setNull();
    setupProperties();
}

//_____________________________________________________________________________
/**
 * Constructor from an XML file
 */
MuscleOptimizeTool::MuscleOptimizeTool(const string &aFileName) :
Object(aFileName, true),
_notes(_notesProp.getValueStr()),
_inputModelFileName(_inputModelFileNameProp.getValueStr()),
_referenceModelFileName(_referenceModelFileNameProp.getValueStr()),
_muscleOptimizerProp(PropertyObj("", MuscleOptimizer())),
_muscleOptimizer((MuscleOptimizer&)_muscleOptimizerProp.getValueObj())
{
    setNull();
    setupProperties();

    updateFromXMLDocument();

    _pathToSubject = IO::getParentDirectory(aFileName);

    if (!isAbsolute(_referenceModelFileName.c_str()))
        _referenceModelFileName=_pathToSubject + _referenceModelFileName;

    if (!isAbsolute(_inputModelFileName.c_str()))
        _inputModelFileName = _pathToSubject + _inputModelFileName;
}

//_____________________________________________________________________________
/**
 * Destructor.
 */
MuscleOptimizeTool::~MuscleOptimizeTool()
{
}

//_____________________________________________________________________________
/**
 * Copy constructor.
 *
 * @param aSubject MuscleOptimizeTool to be copied.
 */
MuscleOptimizeTool::MuscleOptimizeTool(const MuscleOptimizeTool &aSubject) :
Object(aSubject),
_notes(_notesProp.getValueStr()),
_inputModelFileName(_inputModelFileNameProp.getValueStr()),
_referenceModelFileName(_referenceModelFileNameProp.getValueStr()),
_muscleOptimizerProp(PropertyObj("", MuscleOptimizer())),
_muscleOptimizer((MuscleOptimizer&)_muscleOptimizerProp.getValueObj())
{
    setNull();
    setupProperties();
    copyData(aSubject);
}

//=============================================================================
// CONSTRUCTION METHODS
//=============================================================================
//_____________________________________________________________________________
/**
 * Copy data members from one MuscleOptimizeTool to another.
 *
 * @param aSubject MuscleOptimizeTool to be copied.
 */
void MuscleOptimizeTool::copyData(const MuscleOptimizeTool &aSubject)
{
    _notes = aSubject._notes;
    _muscleOptimizer = aSubject._muscleOptimizer;
}

//_____________________________________________________________________________
/**
 * Set the data members of this MuscleOptimizeTool to their null values.
 */
void MuscleOptimizeTool::setNull()
{
}

//_____________________________________________________________________________
/**
 * Connect properties to local pointers.
 */
void MuscleOptimizeTool::setupProperties()
{

    _notesProp.setComment("Notes for the subject.");
    _notesProp.setName("notes");
    _propertySet.append(&_notesProp);

    _inputModelFileNameProp.setComment("Specifies the name of the optimized model (.osim)");
    _inputModelFileNameProp.setName("model");
    _propertySet.append(&_inputModelFileNameProp);

    _referenceModelFileNameProp.setComment("Specifies the name of the reference model(.osim)");
    _referenceModelFileNameProp.setName("reference_model");
    _propertySet.append(&_referenceModelFileNameProp);

    _muscleOptimizerProp.setComment("Specifies parameters for optimizing the muscle parameters for the model.");
    _muscleOptimizerProp.setName("MuscleOptimizer");
    _propertySet.append(&_muscleOptimizerProp);

}

//_____________________________________________________________________________
/**
 * Register the types used by this class.
 */
void MuscleOptimizeTool::registerTypes()
{
    Object::registerType(MuscleOptimizer());
    MuscleOptimizer::registerTypes();
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
MuscleOptimizeTool& MuscleOptimizeTool::operator=(const MuscleOptimizeTool &aSubject)
{
    // BASE CLASS
    Object::operator=(aSubject);

    copyData(aSubject);

    return(*this);
}

//=============================================================================
// UTILITY
//=============================================================================
//_____________________________________________________________________________
/**
 * Load input model
 *
 * @return Pointer to the Model that is created.
 */
Model* MuscleOptimizeTool::loadInputModel()
{
    cout << "Loading input subject " << _inputModelFileName << endl;
    Model* model = NULL;
    try{
        model = new Model(_inputModelFileName);
        SimTK::State &state = model->initSystem();

        model->setName(getName());
    }
    catch (const Exception& x)
    {
        x.print(cout);
        return NULL;
    }

    return model;
}
/**
 * Load reference model
 *
 * @return Pointer to the Model that is created.
 */
Model* MuscleOptimizeTool::loadReferenceModel()
{
    cout << "Loading reference model " << _referenceModelFileName << endl;
    Model* model = NULL;
    try{
        model = new Model(_referenceModelFileName);
        SimTK::State &state = model->initSystem();
    }
    catch (const Exception& x)
    {
        x.print(cout);
        return NULL;
    }

    return model;
}

void MuscleOptimizeTool::setReferenceModelFilename(const std::string refModelFilename)
{
    _referenceModelFileName = refModelFilename;
}

std::string MuscleOptimizeTool::getReferenceModelFilename() const
{
    return _referenceModelFileName;
}
