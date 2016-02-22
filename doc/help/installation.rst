.. _installMO:

Muscle Optimizer installation instructions
##########################################

The easiest way to use the Muscle Parameter Optimization software on Windows is to get an archive with
pre-compiled files from https://simtk.org/home/opt_muscle_par. You need to have OpenSim 3.3 installed in
your system. *Make sure* that you download an archive that is compatible with your OpenSim installation
(i.e., 32 or 64 bit). If you want to use the standalone application on Linux, you need to compile from
sources yourself; you can get the released source code from https://simtk.org/home/opt_muscle_par or the
latest version from the GitHub page(TODO).

.. note::
  If you are only interested in the MatLab package, please refer directly to the :ref:`MOmatlab` section.

Preliminary step
================

If you wish to use the OpenSim GUI plugin or the standalone application, follow these steps:

  - Unzip the provided archive
  - Copy the ``muscleOptimizeTool.dll`` file from the ``bin`` folder to a directory listed in the ``PATH`` environment variable of your machine (you might copy it to the *plugins* subfolder in your OpenSim installation folder, and add it to the ``PATH`` as OpenSim developers suggest)

Opensim GUI plugin (Windows only)
=================================

To use the OpenSim GUI plugin:

  - Start OpenSim
  - Open the Plugins Manager (``Tools -> Plugins``)

  .. _figMOinst1:

  .. figure:: images/MOinstall1.png
      :align: center
      :alt:  Tools-Plugins menu
      :figclass: align-center


  - Go to the ``Downloaded`` tab and click the ``Add plugins...`` button

  .. _figMOinst2:

  .. figure:: images/MOinstall2.png
      :align: center
      :alt:  Downloaded tab - Add plugins button
      :figclass: align-center


  - Locate the *nbm* file

  .. _figMOinst3:

  .. figure:: images/MOinstall3.png
      :align: center
      :alt:  File explorer for NBM file
      :figclass: align-center


  - Click the ``Install`` button

  .. _figMOinst4:

  .. figure:: images/MOinstall4.png
      :align: center
      :alt:  Bottom-left Install button
      :figclass: align-center

  - Click the ``Next`` button

  .. _figMOinst5:

  .. figure:: images/MOinstall5.png
      :align: center
      :alt:  Bottom-right Next button
      :figclass: align-center

  - Read the license agreement, mark the *"I accept the terms ..."* checkbox, then click ``Install`` button

  .. _figMOinst6:

  .. figure:: images/MOinstall6.png
      :align: center
      :alt:  Accept license agreement
      :figclass: align-center

  - Click ``Continue`` to install the plugin even if it is not digitally signed (we are too poor to get a proper certificate)

  .. _figMOinst7:

  .. figure:: images/MOinstall7.png
      :align: center
      :alt:  Click Continue
      :figclass: align-center

  - Wait for installation to complete; click the ``Finish`` button, the close the Plugins window

  .. _figMOinst8:

  .. figure:: images/MOinstall8.png
      :align: center
      :alt:  Bottom-right Finish button
      :figclass: align-center

  - You should now find the ``Map Muscles Operating Range...`` item in the `Tools` menu, below ``Scale Model...``

Please refer to :ref:`MOguiplugin` for instructions on how to use the plugin.

Standalone application
======================

If you want to use the standalone application, you can open a command prompt and move to the
``bin`` directory that you extracted from the package archive. Alternatively, you can copy the
``muscleOptimize.exe`` file from the ``bin`` folder to a directory that is listed in the
``PATH`` environment variable for your machine.

Please refer to :ref:`MOstandalone` for instructions on how to use the standalone application.
