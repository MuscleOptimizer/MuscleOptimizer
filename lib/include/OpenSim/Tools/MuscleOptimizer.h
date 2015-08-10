/* --------------------------------------------------------------------------*
*                  Muscle Optimizer: MuscleOptimizer.h                       *
* -------------------------------------------------------------------------- *
*                                                                            *
* Copyright 2015  Elena Ceseracciu, Luca Modenese                            *
*                                                                            *
* Licensed under the Apache License, Version 2.0 (the "License");            *
* you may not use this file except in compliance with the License.           *
* You may obtain a copy of the License at                                    *
*                                                                            *
*     http://www.apache.org/licenses/LICENSE-2.0                             *
*                                                                            *
* Unless required by applicable law or agreed to in writing, software        *
* distributed under the License is distributed on an "AS IS" BASIS,          *
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
* See the License for the specific language governing permissions and        *
* limitations under the License.                                             *
* ---------------------------------------------------------------------------*/
// Author: Elena Ceseracciu elena.ceseracciu@gmail.com

#ifndef MUSCLE_OPTIMIZER_FOR_OPENSIM_H_
#define MUSCLE_OPTIMIZER_FOR_OPENSIM_H_



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
    public:
        OpenSim_DECLARE_PROPERTY(apply, bool,
            "Whether or not to use the model optimizer during optimization");
        OpenSim_DECLARE_LIST_PROPERTY(coordinates, std::string,
            "Specifies the coordinates (degrees of freedom) to consider (default: ALL)");
        OpenSim_DECLARE_LIST_PROPERTY(muscles, std::string,
            "Specifies the muscles to consider (default: ALL)");
        OpenSim_DECLARE_PROPERTY(n_evaluation_points, double,
            "Number of evaluation points for each degree of freedom");
        OpenSim_DECLARE_PROPERTY(min_degrees_increment, double,
            "Minimum degree interval when sampling a degree of freedom(<= 0->no limit check).Default value : 2.5 (degrees)");
        OpenSim_DECLARE_PROPERTY(output_model_file, std::string,
            "Name of OpenSim model file (.osim) to write when done optimizing.");

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
        virtual ~MuscleOptimizer();

        /* Register types to be used when reading a MuscleOptimizer object from xml file. */
        static void registerTypes();

        //--------------------------------------------------------------------------
        // INTERFACE
        //--------------------------------------------------------------------------
        virtual bool processModel(Model* inputModel, Model* referenceModel, const std::string& aPathToSubject = ""); // TODO check if I really need pathToSubject
        //
        void setPrintResultFiles(bool aToWrite) { _printResultFiles = aToWrite; }
        typedef std::vector< std::pair< std::string, std::vector<double> > > CoordinateCombinations;
        CoordinateCombinations sampleROMsForMuscle(Model& model, SimTK::State& si, const std::string& muscleName, unsigned int nEval);

        // TEMPORARY WORKAROUNDS FOR GETTING/SETTING LIST PROPERTIES
        Array<std::string> getCoordinates();
        void setCoordinates(const Array<std::string>& newCoords);

        Array<std::string> getMuscles();
        void setMuscles(const Array<std::string>& newMuscles);

    private:
        struct TemplateMuscleInfo
        {
            double normalizedFiberLength;
            double normalizedTendonLength;
            double pennationAngle;
        };
        void constructProperties();
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

#endif // MUSCLE_OPTIMIZER_FOR_OPENSIM_H_

