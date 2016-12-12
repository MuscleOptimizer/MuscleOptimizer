.. _MOmatlab:

MatLab toolbox
##############

The MatLab Muscle Parameter Optimization Toolbox can be downloaded as part of the `Muscle Optimizer repository <http://github.com/MuscleOptimizer/MuscleOptimizer>`_ on GitHub, under the ``matlab`` directory. A version of this Matlab toolbox,
designed to reproduce the results and figures of the paper associated with the Muscle Optimizer toolbox :cite:`modenese2016estimation`,
is available from our `project page <https://simtk.org/projects/opt_muscle_par>`_  on SimTK.org.

To use the toolbox, you should add the folder ``MuscleParOptTool`` (located in the ``matlab`` directory of the repository) to the MatLab path (`see Matlab documentation <http://it.mathworks.com/help/matlab/matlab_env/add-remove-or-reorder-folders-on-the-search-path.html>`_).

The toolbox provides the ``optimMuscleParams`` function, that can be called as in the following example:

.. literalinclude:: ../../test/Example1/run_example1.m
   :language: matlab
   :start-after: % clear;clc;close all
   :encoding: latin-1
