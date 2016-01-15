.. _installMO:

Muscle Optimizer installation instructions
##########################################

The easiest way to use the Muscle Parameter Optimization software on Windows is to get an archive with
pre-compiled files from https://simtk.org/home/opt_muscle_par. You need to have OpenSim 3.3 installed in
your system. *Make sure* that you download an archive that is compatible with your OpenSim installation
(i.e., 32 or 64 bit). If you want to use the standalone application on Linux, you need to compile from
sources yourself; you can get the released source code from https://simtk.org/home/opt_muscle_par or the
latest version from the GitHub page(TODO).

If you are only interested in the MatLab package, please refer directly to the :ref:`MOmatlab` section.

If you wish to use the OpenSim GUI plugin or the standalone application, follow these steps:

  - Unzip the provided archive
  - Copy the ``muscleOptimizeTool.dll`` file to a directory listed in the ``PATH`` environment variable of your machine (you might copy it to the *plugins* subfolder in your OpenSim installation folder, and add it to the ``PATH`` as OpenSim developers suggest)

To use the OpenSim GUI plugin:

  - Start OpenSim, then open the Plugins Manager (``Tools -> Plugins``)
  - Locate and install the *.nmb* file that you extracted(``Downloaded -> Add plugins...`` ; locate the *nbm* file; ``Install``)
  - You should now find the ``Muscle Optimizer Tool ...`` item in the `Tools` menu

Please refer to :ref:`MOguiplugin` for instructions on how to use the plugin.

If you want to use the standalone application, you can open a command prompt and move to the directory
where you unzipped the files, or you can copy the ``muscleOptimize.exe`` file to a directory that is listed in
the ``PATH`` environment variable for your machine.
Please refer to :ref:`MOstandalone` for instructions on how to use the standalone application.