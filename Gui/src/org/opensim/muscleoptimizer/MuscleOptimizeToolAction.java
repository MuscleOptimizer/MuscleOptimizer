// TODO: add license header
// Author: Elena Ceseracciu (elena.ceseracciu@gmail.com)
package org.opensim.muscleoptimizer;

import java.io.IOException;
import org.openide.util.HelpCtx;
import org.openide.util.NbBundle;
import org.openide.util.actions.CallableSystemAction;
import org.opensim.modeling.Model;
import org.opensim.tracking.BaseToolPanel;
import org.opensim.muscleoptimizer.MuscleOptimizeToolPanel;
import org.opensim.utils.ErrorDialog;
import org.opensim.view.pub.OpenSimDB;



public final class MuscleOptimizeToolAction extends CallableSystemAction {

   public void performAction() {
      Model model = OpenSimDB.getInstance().getCurrentModel();
      if(model==null) return;

      try {
         final MuscleOptimizeToolPanel panel = new MuscleOptimizeToolPanel(model);
         BaseToolPanel.openToolDialog(panel, "Muscle Optimizer Tool");
      } catch (IOException ex) {
         ErrorDialog.displayIOExceptionDialog("Muscle Optimizer Tool Error","Error while initializing muscle optimization tool",ex);
      }
   }

   public String getName() {
      return NbBundle.getMessage(MuscleOptimizeToolAction.class, "CTL_MuscleOptimizeToolAction");
   }

   protected void initialize() {
      super.initialize();
      // see org.openide.util.actions.SystemAction.iconResource() javadoc for more details
      putValue("noIconInMenu", Boolean.TRUE);
   }

   public HelpCtx getHelpCtx() {
      return HelpCtx.DEFAULT_HELP;
   }

   protected boolean asynchronous() {
      return false;
   }

   public boolean isEnabled() {
      return (OpenSimDB.getInstance().getCurrentModel()!=null &&
              OpenSimDB.getInstance().getCurrentModel().getNumBodies()>1);
   }
}
