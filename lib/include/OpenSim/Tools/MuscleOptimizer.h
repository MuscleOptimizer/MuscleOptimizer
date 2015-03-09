// Authors: Elena Ceseracciu, Luca Modenese, Claudio Pizzolato

#ifndef __MuscleOptimizer_h__
#define __MuscleOptimizer_h__



// INCLUDE
#include <iostream>
#include <string>
#include <math.h>
#include "osimToolsDLL.h"
#include <OpenSim/Common/PropertyDbl.h>
#include <OpenSim/Common/PropertyDblArray.h>
#include <OpenSim/Common/PropertyObj.h>
#include <OpenSim/Common/PropertyBool.h>
#include <OpenSim/Common/PropertyStr.h>
#include <OpenSim/Common/PropertyStrArray.h>
#include <OpenSim/Common/PropertyObjArray.h>
#include <OpenSim/Common/Storage.h>

namespace OpenSim {

    class Model;

    //=============================================================================
    //=============================================================================
    /**
     *  //TODO: add documentation ^^;;
     */
    class OSIMTOOLS_API MuscleOptimizer : public Object {
        OpenSim_DECLARE_CONCRETE_OBJECT(MuscleOptimizer, Object);

        //=============================================================================
        // DATA
        //=============================================================================
    private:

    protected:
        /** Pointer to the model being investigated. */
        //Model *_inputModel;

        /** Pointer to the reference model. */
        //Model *_referenceModel;

        // whether or not to apply optimizing
        PropertyBool _applyProp;
        bool &_apply;

        //// joints to consider
        PropertyStrArray _coordinatesProp;
        Array<std::string> &_coordinates;

        //// muscles to consider
        PropertyStrArray _musclesProp;
        Array<std::string> &_muscles;

        //// constraints
      //  PropertyObjArray<Constraints> _constraintsProp;

        //// number of points to sample each ROM
        PropertyDbl _nEvalPointsProp;
        double &_nEvalPoints;

        // name of XML model file to write when done optimizing
        PropertyStr _outputModelFileNameProp;
        std::string &_outputModelFileName;

        // Whether or not to write write to the designated output files (GUI will set this to false)
        bool _printResultFiles;

        //=============================================================================
        // METHODS
        //=============================================================================
        //--------------------------------------------------------------------------
        // CONSTRUCTION
        //--------------------------------------------------------------------------
    public:
        MuscleOptimizer();
        MuscleOptimizer(const MuscleOptimizer &aMuscleOptimizer);
        virtual ~MuscleOptimizer();

#ifndef SWIG
        MuscleOptimizer& operator=(const MuscleOptimizer &aMuscleOptimizer);
#endif
        void copyData(const MuscleOptimizer &aMuscleOptimizer);

        /* Register types to be used when reading a MuscleOptimizer object from xml file. */
        static void registerTypes();

        //--------------------------------------------------------------------------
        // GET AND SET
        //--------------------------------------------------------------------------

        bool getApply() const { return _apply; }
        void setApply(bool aApply) {
            _apply = aApply;
            _applyProp.setValueIsDefault(false);
        }

        const Array<std::string> &getCoordinates() const { return _coordinates; }
        void setJoints(Array<std::string> joints) {
            _coordinates = joints;
            _coordinatesProp.setValueIsDefault(false);
        }

        const Array<std::string> &getMuscles() const { return _muscles; }
        void setMuscles(Array<std::string> muscles) {
            _muscles = muscles;
            _musclesProp.setValueIsDefault(false);
        }

        double getNEvalPoints() const { return _nEvalPoints; }
        void setNEvalPoints(double nEvalPoints) {
            _nEvalPoints = nEvalPoints;
            _nEvalPointsProp.setValueIsDefault(false);
        }

        const std::string& getOutputModelFileName() const { return _outputModelFileName; }
        void setOutputModelFileName(const std::string& aOutputModelFileName) {
            _outputModelFileName = aOutputModelFileName;
            _outputModelFileNameProp.setValueIsDefault(false);
        }


        //--------------------------------------------------------------------------
        // INTERFACE
        //--------------------------------------------------------------------------
        virtual bool processModel(Model* inputModel, Model* referenceModel, const std::string& aPathToSubject = ""); // TODO check if I really need pathToSubject
        //
        void setPrintResultFiles(bool aToWrite) { _printResultFiles = aToWrite; }
        typedef std::vector< std::pair< std::string, std::vector<double> > > CoordinateCombinations;
        CoordinateCombinations sampleROMsForMuscle(Model& model, SimTK::State& si, const std::string& muscleName, unsigned int nEval);
    private:
        struct TemplateMuscleInfo
        {
            double normalizedFiberLength;
            double normalizedTendonLength;
            double pennationAngle;
        };
        void setNull();
        void setupProperties();
        bool isEnabledMuscle(std::string muscleName);
        bool isEnabledCoordinate(std::string coordinateName);
        Array<std::string> getJointSpannedByMuscle(Model& model, const std::string& muscleName);
        std::string getIndependentCoordinate(Model& model, const std::string& coordinateName);
        std::vector<double> generateAngleSamples(double anglesStart, double anglesEnd, unsigned int noEval, unsigned int multeplicity, unsigned int totalSamples);

        SimTK::Vector sampleMTULength(Model& model, SimTK::State& si, const std::string& muscleName, const MuscleOptimizer::CoordinateCombinations& coordinateCombinations);
        std::vector<TemplateMuscleInfo> sampleTemplateQuantities(Model& model, SimTK::State& si, const std::string& muscleName, MuscleOptimizer::CoordinateCombinations& coordinateCombinations);

        //=============================================================================
    };	// END of class MuscleOptimizer
    //=============================================================================
    //=============================================================================

} // end of namespace OpenSim

#endif // __MuscleOptimizer_h__


