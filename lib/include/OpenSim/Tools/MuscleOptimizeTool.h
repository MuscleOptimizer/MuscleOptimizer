// Authors: Elena Ceseracciu, Luca Modenese, Claudio Pizzolato

#ifndef __MuscleOptimizeTool_h__
#define __MuscleOptimizeTool_h__

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

        //=============================================================================
        // DATA
        //=============================================================================
    protected:

        PropertyStr _notesProp;
        std::string &_notes;

        /** Name of the xml file used to deserialize or construct a model. */
        PropertyStr _inputModelFileNameProp;
        std::string &_inputModelFileName;

        /** Name of the xml file used to deserialize or construct the reference model. */
        PropertyStr _referenceModelFileNameProp;
        std::string &_referenceModelFileName;

        PropertyObj _muscleOptimizerProp;
        MuscleOptimizer &_muscleOptimizer;

        /** All files in workflow are specified relative to
         * where the subject file is. Need to keep track of that in case absolute
         * path is needed later
         */
        std::string	 _pathToSubject;

        //=============================================================================
        // METHODS
        //=============================================================================
        //--------------------------------------------------------------------------
        // CONSTRUCTION
        //--------------------------------------------------------------------------
    public:
        MuscleOptimizeTool();
        MuscleOptimizeTool(const std::string &aFileName) SWIG_DECLARE_EXCEPTION;
        MuscleOptimizeTool(const MuscleOptimizeTool &aSubject);
        virtual ~MuscleOptimizeTool();

#ifndef SWIG
        MuscleOptimizeTool& operator=(const MuscleOptimizeTool &aSubject);
#endif
        void copyData(const MuscleOptimizeTool &aSubject);

        Model* loadInputModel();
        Model* loadReferenceModel();

        MuscleOptimizer& getMuscleOptimizer()
        {
            return _muscleOptimizer;
        }

        bool isDefaultMuscleOptimizer() { return _muscleOptimizerProp.getValueIsDefault(); }

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
        void setReferenceModelFilename(const std::string refModelFilename);
        std::string getReferenceModelFilename() const;
        void setPrintResultFiles(bool aToWrite) {
            _muscleOptimizer.setPrintResultFiles(aToWrite);
        }

    protected:

    private:
        void setNull();
        void setupProperties();
        //=============================================================================
    };	// END of class MuscleOptimizeTool
    //=============================================================================
    //=============================================================================

} // end of namespace OpenSim

#endif // __MuscleOptimizeTool_h__
