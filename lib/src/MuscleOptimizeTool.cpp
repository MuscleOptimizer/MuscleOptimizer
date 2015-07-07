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
MuscleOptimizeTool::MuscleOptimizeTool()
{
    constructProperties();
}

//_____________________________________________________________________________
/**
 * Constructor from an XML file
 */
MuscleOptimizeTool::MuscleOptimizeTool(const string &aFileName) :
Object(aFileName, true)
{
    constructProperties();

    updateFromXMLDocument();

    _pathToSubject = IO::getParentDirectory(aFileName);

    if (!isAbsolute(get_reference_model().c_str()))
        set_reference_model(_pathToSubject + get_reference_model());

    if (!isAbsolute(get_model().c_str()))
        set_model(_pathToSubject + get_model());
}

//_____________________________________________________________________________
/**
 * Destructor.
 */
MuscleOptimizeTool::~MuscleOptimizeTool()
{
}

//=============================================================================
// CONSTRUCTION METHODS
//=============================================================================
//_____________________________________________________________________________
/**
 * Construct properties.
 */
void MuscleOptimizeTool::constructProperties()
{
    constructProperty_notes("");
    constructProperty_model("");
    constructProperty_reference_model("");
    constructProperty_MuscleOptimizer(MuscleOptimizer());
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
    Model* model = NULL;
    try{
        model = new Model(get_model());
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
    Model* model = NULL;
    try{
        model = new Model(get_reference_model());
        SimTK::State &state = model->initSystem();
    }
    catch (const Exception& x)
    {
        x.print(cout);
        return NULL;
    }

    return model;
}

