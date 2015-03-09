// Author: Elena Ceseracciu
// Date: Octorber 2014

#include <string>
#include <iostream>
#include <OpenSim/OpenSim.h>

using namespace std;
//using namespace OpenSim;


int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "Not enough input arguments!" << endl;
        return -1;
    }

    string firstModelName = argv[1];
    string secondModelName = argv[2];
    double tolerance = 0.0001;
    if (argc>3)
        tolerance = atof(argv[3]);
    else
        cout << "Using default tolerance of " << tolerance << endl;

    cout << "Loading subject " << firstModelName << endl;
    OpenSim::Model* firstModel = NULL;
    try{
        firstModel = new OpenSim::Model(firstModelName);
        firstModel->initSystem();
    }
    catch (const OpenSim::Exception& x)
    {
        x.print(cout);
        return -1;
    }
    cout << "Loading subject " << secondModelName << endl;
    OpenSim::Model* secondModel = NULL;
    try{
        secondModel = new OpenSim::Model(secondModelName);
        secondModel->initSystem();
    }
    catch (const OpenSim::Exception& x)
    {
        x.print(cout);
        return -1;
    }

    OpenSim::Array<std::string> musclesFirst;
    firstModel->getMuscles().getNames(musclesFirst);

    bool all_passed = true;

    for (int iMusc = 0; iMusc < musclesFirst.getSize(); ++iMusc)
    {
        const OpenSim::Muscle& currentMuscleFirst = firstModel->getMuscles().get(musclesFirst[iMusc]);
        const OpenSim::Muscle& currentMuscleSecond = secondModel->getMuscles().get(musclesFirst[iMusc]);

        double optFibLenDiff = std::abs(currentMuscleFirst.getOptimalFiberLength() - currentMuscleSecond.getOptimalFiberLength());
        if (optFibLenDiff>tolerance)
        {
            all_passed = false;
            std::cout << "Optimal fiber length difference for muscle " << musclesFirst[iMusc] << " is " <<
                currentMuscleFirst.getOptimalFiberLength() << " - " << currentMuscleSecond.getOptimalFiberLength()<< " = " << optFibLenDiff << " >" << tolerance << std::endl;
        }
        double tendSlackLenDiff = std::abs(currentMuscleFirst.getTendonSlackLength() - currentMuscleSecond.getTendonSlackLength());
        if (tendSlackLenDiff>tolerance)
        {
            all_passed = false;
            std::cout << "Tendon slack length difference for muscle " << musclesFirst[iMusc] << " is " <<
                currentMuscleFirst.getTendonSlackLength() << " - " << currentMuscleSecond.getTendonSlackLength() << " = " << tendSlackLenDiff << " >" << tolerance << std::endl;
        }
    }

    return all_passed? 0:-1;
}
