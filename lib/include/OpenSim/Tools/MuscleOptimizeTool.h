// Authors: Elena Ceseracciu, Luca Modenese, Claudio Pizzolato

#ifndef MUSCLE_OPTIMIZE_TOOL_FOR_OPENSIM_H_
#define MUSCLE_OPTIMIZE_TOOL_FOR_OPENSIM_H_

// INCLUDE
#include <iostream>

#include <math.h>
#include "osimToolsDLL.h"
#include <OpenSim/Common/PropertyObj.h>
#include <OpenSim/Common/PropertyStr.h>
#include <OpenSim/Common/PropertyDbl.h>
#include <OpenSim/Common/Storage.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Tools/MuscleOptimizer.h>

namespace OpenSim {


    class OSIMTOOLS_API MuscleOptimizeTool : public Object {
        OpenSim_DECLARE_CONCRETE_OBJECT(MuscleOptimizeTool, Object);
    public:
        OpenSim_DECLARE_PROPERTY(notes, std::string,
            "Notes for the subject.");
        OpenSim_DECLARE_PROPERTY(model, std::string,
            "Specifies the name of the input model (.osim)");
        OpenSim_DECLARE_PROPERTY(reference_model, std::string,
            "Specifies the name of the reference model (.osim)");
        OpenSim_DECLARE_UNNAMED_PROPERTY(MuscleOptimizer,
            "Specifies parameters for optimizing the muscle parameters for the model.");

        /** All files in workflow are specified relative to
         * where the subject file is. Need to keep track of that in case absolute
         * path is needed later
         */
        std::string _pathToSubject;

        //=============================================================================
        // METHODS
        //=============================================================================
        //--------------------------------------------------------------------------
        // CONSTRUCTION
        //--------------------------------------------------------------------------
    public:
        MuscleOptimizeTool();
        MuscleOptimizeTool(const std::string &aFileName) SWIG_DECLARE_EXCEPTION;
        virtual ~MuscleOptimizeTool();

        Model* loadInputModel();
        Model* loadReferenceModel();

        MuscleOptimizer& getMuscleOptimizer()
        {
            return upd_MuscleOptimizer();
        }

        bool isDefaultMuscleOptimizer() { return getProperty_MuscleOptimizer().getValueIsDefault(); }

        /* Register types to be used when reading a MuscleOptimizeTool object from xml file. */
        static void registerTypes();

        /**
         * Accessor methods to set and get path to Subject. This is needed
         * since all file names referred to in the subject file are relative
         * to subject file
         */
        const std::string& getPathToSubject() const
        {
            return _pathToSubject;
        }
        void setPathToSubject(const std::string& aPath)
        {
            _pathToSubject = aPath;
        }
        void setPrintResultFiles(bool aToWrite) {
            upd_MuscleOptimizer().setPrintResultFiles(aToWrite);
        }

    protected:

    private:
        void constructProperties();
        //=============================================================================
    };	// END of class MuscleOptimizeTool
    //=============================================================================
    //=============================================================================

} // end of namespace OpenSim

#endif // MUSCLE_OPTIMIZE_TOOL_FOR_OPENSIM_H_
