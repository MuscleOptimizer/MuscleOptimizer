/* --------------------------------------------------------------------------*
*             Muscle Optimizer GUI: MuscleOptimizeToolAction.java            *
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
// Author: Elena Ceseracciu (elena.ceseracciu@gmail.com)
// Adapted from the OpenSim GUI code base

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
