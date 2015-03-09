// Authors: Elena Ceseracciu, Luca Modenese, Claudio Pizzolato

// INCLUDES
#include <string>
//#include <OpenSim/version.h>
#include <OpenSim/OpenSim.h>
#include <OpenSim/Common/Storage.h>
#include <OpenSim/Common/IO.h>
#include <OpenSim/Common/ScaleSet.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Tools/MuscleOptimizeTool.h>
#include <OpenSim/Common/MarkerData.h>
#include <OpenSim/Simulation/Model/MarkerSet.h>
#include <OpenSim/Simulation/Model/ForceSet.h>
#include <OpenSim/Common/LoadOpenSimLibrary.h>
#include <OpenSim/Simulation/Model/Analysis.h>



using namespace std;
using namespace OpenSim;

static void PrintUsage(const char *aProgName, ostream &aOStream);

int main(int argc, char **argv)
{
    //TODO: put these options on the command line
    //LoadOpenSimLibrary("osimSimbodyEngine");

    // SET OUTPUT FORMATTING
    IO::SetDigitsPad(4);

    // REGISTER TYPES
    Object::registerType(VisibleObject());
    Object::registerType(MuscleOptimizeTool());
    MuscleOptimizeTool::registerTypes();

    // PARSE COMMAND LINE
    string inName;
    string option = "";
    if (argc < 2) {
        PrintUsage(argv[0], cout);
        exit(-1);
    }
    else {
        // Load libraries first
        LoadOpenSimLibraries(argc, argv);
        int i;
        for (i = 1; i <= (argc - 1); i++) {
            option = argv[i];

            // PRINT THE USAGE OPTIONS
            if ((option == "-help") || (option == "-h") || (option == "-Help") || (option == "-H") ||
                (option == "-usage") || (option == "-u") || (option == "-Usage") || (option == "-U")) {
                PrintUsage(argv[0], cout);
                return(0);

                // Identify the setup file
            }
            else if ((option == "-S") || (option == "-Setup")) {
                if (argv[i + 1] == 0){
                    PrintUsage(argv[0], cout);
                    return(0);
                }
                inName = argv[i + 1];
                break;

                // Print a default setup file
            }
            else if ((option == "-PrintSetup") || (option == "-PS")) {
                MuscleOptimizeTool *subject = new MuscleOptimizeTool();
                subject->setName("default");
                // Add in useful objects that may need to be instantiated
                Object::setSerializeAllDefaults(true);
                subject->print("default_Setup_MuscleOptimize.xml");
                Object::setSerializeAllDefaults(false);
                cout << "Created file default_Setup_MuscleOptimize.xml with default setup" << endl;
                return(0);

                // PRINT PROPERTY INFO
            }
            else if ((option == "-PropertyInfo") || (option == "-PI")) {
                if ((i + 1) >= argc) {
                    Object::PrintPropertyInfo(cout, "");

                }
                else {
                    char *compoundName = argv[i + 1];
                    if (compoundName[0] == '-') {
                        Object::PrintPropertyInfo(cout, "");
                    }
                    else {
                        Object::PrintPropertyInfo(cout, compoundName);
                    }
                }
                return(0);

                // Unrecognized
            }
            else {
                cout << "Unrecognized option " << option << " on command line... Ignored" << endl;
                PrintUsage(argv[0], cout);
                return(0);
            }
        }
    }


    try {
        // Construct models and read parameters file
        MuscleOptimizeTool* subject = new MuscleOptimizeTool(inName);

        Model* inputmodel = subject->loadInputModel();
        if (!inputmodel) throw Exception("muscleoptimizer: ERROR- No model specified.", __FILE__, __LINE__);

        Model* referencemodel = subject->loadReferenceModel();
        if (!referencemodel) throw Exception("muscleoptimizer: ERROR- No reference model specified.", __FILE__, __LINE__);

        // Realize the topology and initialize state

        SimTK::State& s = inputmodel->initSystem();

        if (!subject->isDefaultMuscleOptimizer() && subject->getMuscleOptimizer().getApply())
        {
            MuscleOptimizer& optimizer = subject->getMuscleOptimizer();
            if (!optimizer.processModel(inputmodel, referencemodel, subject->getPathToSubject())) return -1;
        }
        else
        {
            cout << "Scaling parameters disabled (apply is false) or not set. Model is not optimized." << endl;
        }

        delete inputmodel;
        delete referencemodel;
        delete subject;
    }
    catch (const Exception& x) {
        x.print(cout);
        return -1;
    }

    return 0;
}

//_____________________________________________________________________________
/**
* Print the usage for this application
*/
void PrintUsage(const char *aProgName, ostream &aOStream)
{
    string progName = IO::GetFileNameFromURI(aProgName);
    aOStream << "\n\n" << progName << ":\n";// << GetVersionAndDate() << "\n\n";
    aOStream << "Option            Argument          Action / Notes\n";
    aOStream << "------            --------          --------------\n";
    aOStream << "-Help, -H                           Print the command-line options for " << progName << ".\n";
    aOStream << "-PrintSetup, -PS                    Generates a template Setup file to customize optimizing\n";
    aOStream << "-Setup, -S        SetupFileName     Specify an xml setup file for optimizing a generic model.\n";
    aOStream << "-PropertyInfo, -PI                  Print help information for properties in setup files.\n";

}
