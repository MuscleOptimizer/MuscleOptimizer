// TODO: add license header
// Author: Elena Ceseracciu (elena.ceseracciu@gmail.com)

package org.opensim.muscleoptimizer;

import java.io.IOException;
import java.util.Observable;
import java.util.Observer;
import org.netbeans.api.progress.ProgressHandle;
import org.netbeans.api.progress.ProgressHandleFactory;
import org.openide.DialogDisplayer;
import org.openide.NotifyDescriptor;
import org.openide.util.Cancellable;
import org.opensim.modeling.Model;
import org.opensim.modeling.OpenSimContext;
import org.opensim.swingui.SwingWorker;
import org.opensim.utils.ErrorDialog;
import org.opensim.utils.FileUtils;
import org.opensim.view.pub.OpenSimDB;
import org.opensim.view.pub.ViewDB;
//==================================================================
// OptionalFile (helper class)
//==================================================================
class OptionalFile {
   public String fileName = "";
   public boolean enabled = false;
   public boolean isValid() { return enabled && !FileUtils.effectivelyNull(fileName); }
   public void fromProperty(String fileName) {
      if(FileUtils.effectivelyNull(fileName)) { enabled = false; this.fileName = ""; }
      else { enabled = true; this.fileName = fileName; }
   }
   public String toProperty() {
      return (enabled && !FileUtils.effectivelyNull(fileName)) ? fileName : "Unassigned";
   }
}


//==================================================================
// OptimizeToolModel
//==================================================================
public class MuscleOptimizeToolModel extends Observable implements Observer {

    static {
    System.loadLibrary("muscleOptimizerJ");
  }
    /**
     * @return the originalModel
     */
    public Model getOriginalModel() {
        return originalModel;
    }
   //========================================================================
   // OptimizeToolWorker
   //========================================================================
   class OptimizeToolWorker extends SwingWorker {
      private ProgressHandle progressHandle = null;
      boolean result = false;
      private Model processedModel = null; //TODO why need this?
      OpenSimContext processedModelContext=null;

      OptimizeToolWorker() throws Exception {
         updateOptimizeTool();

         progressHandle = ProgressHandleFactory.createHandle("Executing muscle scaling...",
                              new Cancellable() {
                                 public boolean cancel() {
                                    result = false;
                                    finished();
                                    return true;
                                 }
                              });
         progressHandle.start();

         processedModel = new Model(unoptimizedModel);
         processedModel.setName(optimizeTool.getName());
         processedModel.setInputFileName("");
         processedModel.setOriginalModelPathFromModel(unoptimizedModel); // important to keep track of the original path so bone loading works

         setExecuting(true);
      }

      // TODO: we can't actually interrupt model optimizer
      public void interrupt() {
      }

      public Object construct() {
         result = true;

         if(getMuscleOptimizerEnabled()) {
            System.out.println("ModelOptimizer...");
            processedModelContext = OpenSimDB.getInstance().createContext(processedModel);

            if(referenceModel!=null)
             if (!optimizeTool.getMuscleOptimizer().processModel(processedModel, referenceModel)){
               result = false;
               return this;
            }
         }

         return this;
      }

      @Override
      public void finished() {
         progressHandle.finish();

         if(result) {
            OpenSimDB.getInstance().replaceModel(optimizedModel, processedModel, processedModelContext);
            optimizedModel = processedModel;
            if(ViewDB.getInstance().getModelGuiElements(optimizedModel)!=null)
               ViewDB.getInstance().getModelGuiElements(optimizedModel).setUnsavedChangesFlag(true);

            resetModified();
         }

         setExecuting(false);
         if (cleanupAfterExecuting)
            cleanup();
         processedModel = null;
         worker = null;
      }
   }
   private OptimizeToolWorker worker = null;
   //========================================================================
   // END OptimizeToolWorker
   //========================================================================


   enum Operation { AllDataChanged, SubjectDataChanged, MarkerSetChanged, MeasurementSetChanged, ModelOptimizerDataChanged, MarkerPlacerDataChanged, ExecutionStateChanged };

   private MuscleOptimizeTool optimizeTool = null;
   private Model originalModel = null;
   private Model unoptimizedModel = null;  //Working copy to optimize inplace
   private Model optimizedModel = null;
   private Model referenceModel = null;

   private boolean modifiedSinceLastExecute = true;

   private boolean executing = false;
   private boolean cleanupAfterExecuting = false;  // Keep track if cleaning up needs to be done on execution finish vs. dialog close

   public MuscleOptimizeToolModel(Model originalModel) throws IOException {
      // Store original model; create copy of the original model as our unoptimized model (i.e. the model we'll optimize)
      this.originalModel = originalModel;
      unoptimizedModel = new Model(originalModel);
      unoptimizedModel.setInputFileName("");
      unoptimizedModel.setOriginalModelPathFromModel(originalModel); // important to keep track of the original path so bone loading works


      // Create optimization tool
      optimizeTool = new org.opensim.muscleoptimizer.MuscleOptimizeTool();
      setName(originalModel.getName()+"-optimized"); // initialize name of output (optimized) model
   }

   // Simple accessors
   public Model getUnoptimizedModel() { return unoptimizedModel; }
   public Model getReferenceModel() { return referenceModel; }
   public MuscleOptimizeTool getOptimizeTool() { return optimizeTool; }
   public void updateReferenceModel(String refModelFilename){
        optimizeTool.set_reference_model(refModelFilename);
        referenceModel = optimizeTool.loadReferenceModel();
   }

   //------------------------------------------------------------------------
   // Utilities for running/canceling tool
   //------------------------------------------------------------------------

   private void updateOptimizeTool() {
       optimizeTool.set_model(originalModel.getName());
       optimizeTool.setPrintResultFiles((optimizeTool.getMuscleOptimizer().get_output_model_file()!=""));
   }

   public void execute() {
      if(isModified() && worker==null) {
         try {
            worker = new OptimizeToolWorker();
            worker.start();
         } catch (Exception ex) {
            DialogDisplayer.getDefault().notify(new NotifyDescriptor.Message(ex.getMessage(), NotifyDescriptor.ERROR_MESSAGE));
            worker = null;
         }
      }
   }

   public void interrupt(boolean promptToKeepPartialResult) {
      if(worker!=null) worker.interrupt();
   }

   public void cancel() {
      interrupt(false);
   }

   public void update(Observable observable, Object obj) {
   }

   //------------------------------------------------------------------------
   // Execution status
   //------------------------------------------------------------------------

   private void setExecuting(boolean executing) {
      if(this.executing != executing) {
         this.executing = executing;
         setChanged();
         notifyObservers(Operation.ExecutionStateChanged);
      }
   }
   public boolean isExecuting() {
      return executing;
   }

   //------------------------------------------------------------------------
   // Modified flag
   //------------------------------------------------------------------------

   private void setModified(Operation change) {
      modifiedSinceLastExecute = true;
      setChanged(); // need to call this before calling notifyObservers
      notifyObservers(change);
   }
   private void resetModified() {
      modifiedSinceLastExecute = false;
   }
   public boolean isModified() {
      return modifiedSinceLastExecute;
   }

   //------------------------------------------------------------------------
   // Validation
   //------------------------------------------------------------------------



   private boolean getMuscleOptimizerValid() {return true;}

   public boolean isValid() {
      return getMuscleOptimizerValid();
   }

   //------------------------------------------------------------------------
   // Model Properties
   //------------------------------------------------------------------------

   public void setName(String name) {
      if(!optimizeTool.getName().equals(name)) {
         optimizeTool.setName(name);
         setModified(Operation.SubjectDataChanged);
      }
   }
   public String getName() {
      return optimizeTool.getName();
   }



   //------------------------------------------------------------------------
   // Filename effectively null if it's null, or "", or "Unassigned"
   //------------------------------------------------------------------------
   private boolean fileNameEffectivelyNull(String fileName) {
      return fileName==null || fileName.equals("") || fileName.equalsIgnoreCase("Unassigned");
   }
   private String getFileName(String fileName) {
      return FileUtils.effectivelyNull(fileName) ? "" : fileName;
   }

   //------------------------------------------------------------------------
   // Muscle Optimizer
   //------------------------------------------------------------------------

   // Muscle optimizer enabled
   public void setMuscleOptimizerEnabled(boolean enabled) {
      if(getMuscleOptimizerEnabled() != enabled) {
         optimizeTool.getMuscleOptimizer().set_apply(enabled);
         setModified(Operation.ModelOptimizerDataChanged);
      }
   }
   public boolean getMuscleOptimizerEnabled() {
      return optimizeTool.getMuscleOptimizer().get_apply();
   }

   public void setNumberOfEvaluationPoints(double npoints)
   {
       optimizeTool.getMuscleOptimizer().set_n_evaluation_points(npoints);
   }

   public double getNumberOfEvaluationPoints()
   {
       return optimizeTool.getMuscleOptimizer().get_n_evaluation_points();
   }

   public void setMinimumDegreesIncrement(double minIncrement)
   {
       optimizeTool.getMuscleOptimizer().set_min_degrees_increment(minIncrement);
   }

   public double getMinimumDegreesIncrement()
   {
       return optimizeTool.getMuscleOptimizer().get_min_degrees_increment();
   }

   //------------------------------------------------------------------------
   // Load/Save Settings
   //------------------------------------------------------------------------

   public boolean loadSettings(String fileName) {
      // TODO: set current working directory before trying to read it?
      MuscleOptimizeTool newOptimizeTool = null;
      try {
         newOptimizeTool = new MuscleOptimizeTool(fileName);
      } catch (IOException ex) {
         ErrorDialog.displayIOExceptionDialog("Error loading file","Could not load "+fileName,ex);
         return false;
      }
      optimizeTool = newOptimizeTool;
      referenceModel=optimizeTool.loadReferenceModel();

      // keep internal data in sync
      modifiedSinceLastExecute = true;

      setModified(Operation.AllDataChanged);
      return true;
   }

   public boolean saveSettings(String fileName) {
      String fullFilename = FileUtils.addExtensionIfNeeded(fileName, ".xml");
      updateOptimizeTool();
      //opensimModelJNI.OpenSimObject_setSerializeAllDefaults(true);
      optimizeTool.print(fullFilename);
      return true;
   }

   //------------------------------------------------------------------------
   // Utility
   //------------------------------------------------------------------------


   // Release C++ resources on exit (either dialog closing or execution finish whichever is later
   void cleanup() {
      if (isExecuting()){
         cleanupAfterExecuting = true;
      }
      else{
//         ikCommonModel.deleteObservers();
         optimizeTool = null;
         System.gc();
      }
   }
}
