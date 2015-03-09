%module(directors="1") muscleOptimizerJ


%typemap(javaimports) SWIGTYPE, SWIGTYPE *, SWIGTYPE & "import org.opensim.modeling.*;"
%pragma(java) jniclassimports="import org.opensim.modeling.*;"
%pragma(java) moduleimports="import org.opensim.modeling.*;" 

%import "javaWrapOpenSim.i"

%include "typemaps.i"
%include "std_string.i"
%{
#include <OpenSim/Tools/MuscleOptimizer.h>
#include <OpenSim/Tools/MuscleOptimizeTool.h>
#include <OpenSim/Wrapping/Java/OpenSimJNI/OpenSimContext.h>

using namespace OpenSim;
using namespace SimTK;
%}

%include <OpenSim/Tools/MuscleOptimizer.h>
%include <OpenSim/Tools/MuscleOptimizeTool.h>