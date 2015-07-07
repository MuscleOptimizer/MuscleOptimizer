// TODO: add license header
// Author: Elena Ceseracciu (elena.ceseracciu@gmail.com)

package org.opensim.muscleoptimizer;

import java.awt.Component;
import java.awt.Dialog;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.text.ParseException;
import java.util.Observable;
import java.util.Observer;
import javax.swing.AbstractAction;
import javax.swing.BorderFactory;
import javax.swing.JCheckBox;
import org.openide.DialogDescriptor;
import org.openide.DialogDisplayer;
import org.openide.NotifyDescriptor;
import org.opensim.modeling.ArrayStr;
import org.opensim.modeling.Model;
import org.opensim.swingui.ComponentTitledBorder;
import org.opensim.utils.BrowserLauncher;
import org.opensim.utils.TheApp;
import org.opensim.view.ModelEvent;
import org.opensim.view.pub.OpenSimDB;

public class MuscleOptimizeToolPanel extends org.opensim.tracking.BaseToolPanel implements Observer {

   private MuscleOptimizeToolModel optimizeToolModel = null;
   private QuantityNameFilterCoordinatePanel coordPanel=null;
   private QuantityNameFilterMusclePanel musclePanel=null;
   private JCheckBox modelOptimizerPanelCheckBox = new JCheckBox(new EnableMuscleOptimizerAction());
   private NumberFormat numFormat = NumberFormat.getInstance();

   class EnableMuscleOptimizerAction extends AbstractAction {
      public EnableMuscleOptimizerAction() { super("Optimize model muscles"); }
      public void actionPerformed(ActionEvent evt) { optimizeToolModel.setMuscleOptimizerEnabled(((JCheckBox)evt.getSource()).isSelected()); }
   }


   /** Creates new form OptimizeToolPanel */
   public MuscleOptimizeToolPanel(Model model) throws IOException {
      if(model==null) throw new IOException("MuscleOptimizeToolPanel got null model");

      optimizeToolModel = new MuscleOptimizeToolModel(model);

      if (numFormat instanceof DecimalFormat) {
        ((DecimalFormat) numFormat).applyPattern("#,##0.#########");
      }


      helpButton.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent ae) {
                //TODO add help
                //String path = BrowserLauncher.isConnected() ? "http://MOwebsite" : TheApp.getUsersGuideDir() +  "OptimizingMuscleParameters.html";
                //BrowserLauncher.openURL(path);
            }
      });

      initComponents();
      bindPropertiesToComponents();

      coordPanel=new QuantityNameFilterCoordinatePanel(optimizeToolModel);
      musclePanel= new QuantityNameFilterMusclePanel(optimizeToolModel);
      jTabbedPane.addTab("Coordinates", coordPanel);
      jTabbedPane.addTab("Muscles", musclePanel) ;
      referenceModelPath.setExtensionsAndDescription(".osim", "Reference OpenSim model");
      outputOptimizedModelFilePath.setExtensionsAndDescription(".osim", "Output OpenSim model");
      outputOptimizedModelFilePath.setSaveMode(true);
      setSettingsFileDescription("Muscle optimizer tool settings file");

      updateFromModel();

      optimizeToolModel.addObserver(this);
   }

   private void bindPropertiesToComponents() {
      org.opensim.tracking.ToolCommon.bindProperty(optimizeToolModel.getOptimizeTool().getMuscleOptimizer(), "n_evaluation_points", nEvalPoints);
      org.opensim.tracking.ToolCommon.bindProperty(optimizeToolModel.getOptimizeTool().getMuscleOptimizer(), "min_degrees_increment", minIncrement);
      org.opensim.tracking.ToolCommon.bindProperty(optimizeToolModel.getOptimizeTool(), "reference_model", referenceModelPath);
      org.opensim.tracking.ToolCommon.bindProperty(optimizeToolModel.getOptimizeTool().getMuscleOptimizer(), "output_model_file", outputOptimizedModelFilePath);
    }

   public void update(Observable observable, Object obj) {
      if (observable instanceof OpenSimDB){
           if (obj instanceof ModelEvent) {
                if (OpenSimDB.getInstance().hasModel(optimizeToolModel.getOriginalModel()))
                    return;
                else {
                    optimizeToolModel.deleteObserver(this);
                    OpenSimDB.getInstance().deleteObserver(this);
                    NotifyDescriptor.Message dlg =
                          new NotifyDescriptor.Message("Model used by the tool is being closed. Closing tool.");
                    DialogDisplayer.getDefault().notify(dlg);
                    this.close();
                    return;
                }
           }
           return;
      }
      if(observable == optimizeToolModel && obj == MuscleOptimizeToolModel.Operation.ExecutionStateChanged) {
         // Just need to update the buttons
         updateDialogButtons();
      } else {
         updateFromModel();
      }
   }

   public void updateFromModel() {

      Model unoptimizedModel = optimizeToolModel.getUnoptimizedModel(); //ndElena: I might need this for the joints/muscles list
      //---------------------------------------------------------------------
      // Model optimizer
      //---------------------------------------------------------------------
      // Model optimizer enabled
      boolean enabled = optimizeToolModel.getMuscleOptimizerEnabled();
      modelOptimizerPanelCheckBox.setSelected(enabled);
      for(Component comp : modelOptimizerPanel.getComponents()) comp.setEnabled(enabled);

      nEvalPoints.setText(numFormat.format(optimizeToolModel.getNumberOfEvaluationPoints()));
      minIncrement.setText(numFormat.format(optimizeToolModel.getMinimumDegreesIncrement()));
      referenceModelPath.setFileName(optimizeToolModel.getOptimizeTool().get_reference_model());
      outputOptimizedModelFilePath.setFileName(optimizeToolModel.getOptimizeTool().getMuscleOptimizer().get_output_model_file());

      //---------------------------------------------------------------------
      // Dialog buttons
      //---------------------------------------------------------------------
      updateDialogButtons();
   }

   public void updateDialogButtons() {
      updateApplyButton(!optimizeToolModel.isExecuting() && optimizeToolModel.isModified() && optimizeToolModel.isValid());
   }

   //------------------------------------------------------------------------
   // Overrides from BaseToolPanel
   //------------------------------------------------------------------------
   public void loadSettings(String fileName) { optimizeToolModel.loadSettings(fileName); }
   public void saveSettings(String fileName) { optimizeToolModel.saveSettings(fileName); }

   public void pressedCancel() {
      optimizeToolModel.cancel();
   }

   public void pressedClose() {
   }

   public void pressedApply() {
      optimizeToolModel.execute();
      updateDialogButtons();
   }

   /** This method is called from within the constructor to
    * initialize the form.
    * WARNING: Do NOT modify this code. The content of this method is
    * always regenerated by the Form Editor.
    */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        editMuscleList1 = new javax.swing.JLabel();
        jTabbedPane = new javax.swing.JTabbedPane();
        jPanel1 = new javax.swing.JPanel();
        modelOptimizerPanel = new javax.swing.JPanel();
        nEvalPointsLabel = new javax.swing.JLabel();
        nEvalPoints = new javax.swing.JTextField();
        refModelLabel = new javax.swing.JLabel();
        referenceModelPath = new org.opensim.swingui.FileTextFieldAndChooser();
        minIncrementLabel = new javax.swing.JLabel();
        minIncrement = new javax.swing.JTextField();
        outputPanel = new javax.swing.JPanel();
        optimizedModelFileLabel = new javax.swing.JLabel();
        outputOptimizedModelFilePath = new org.opensim.swingui.FileTextFieldAndChooser();

        editMuscleList1.setHorizontalAlignment(javax.swing.SwingConstants.TRAILING);
        editMuscleList1.setText("Muscles");

        modelOptimizerPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(javax.swing.BorderFactory.createEtchedBorder(), "Optimize Muscles"));

        nEvalPointsLabel.setHorizontalAlignment(javax.swing.SwingConstants.TRAILING);
        nEvalPointsLabel.setText("Number of evaluation points");

        nEvalPoints.setHorizontalAlignment(javax.swing.JTextField.TRAILING);
        nEvalPoints.setMinimumSize(new java.awt.Dimension(1, 20));
        nEvalPoints.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusLost(java.awt.event.FocusEvent evt) {
                nEvalPointsFocusLost(evt);
            }
        });
        nEvalPoints.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                nEvalPointsActionPerformed(evt);
            }
        });

        refModelLabel.setText("Reference model");

        referenceModelPath.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                referenceModelPathStateChanged(evt);
            }
        });

        minIncrementLabel.setHorizontalAlignment(javax.swing.SwingConstants.TRAILING);
        minIncrementLabel.setText("Minimum increment (degrees)");

        minIncrement.setHorizontalAlignment(javax.swing.JTextField.TRAILING);
        minIncrement.setMinimumSize(new java.awt.Dimension(1, 20));
        minIncrement.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusLost(java.awt.event.FocusEvent evt) {
                minIncrementFocusLost(evt);
            }
        });
        minIncrement.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                minIncrementActionPerformed(evt);
            }
        });

        org.jdesktop.layout.GroupLayout modelOptimizerPanelLayout = new org.jdesktop.layout.GroupLayout(modelOptimizerPanel);
        modelOptimizerPanel.setLayout(modelOptimizerPanelLayout);
        modelOptimizerPanelLayout.setHorizontalGroup(
            modelOptimizerPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(modelOptimizerPanelLayout.createSequentialGroup()
                .addContainerGap()
                .add(refModelLabel)
                .add(28, 28, 28)
                .add(referenceModelPath, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 414, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(19, Short.MAX_VALUE))
            .add(modelOptimizerPanelLayout.createSequentialGroup()
                .add(nEvalPointsLabel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 144, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(18, 18, 18)
                .add(nEvalPoints, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 35, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .add(minIncrementLabel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 144, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(18, 18, 18)
                .add(minIncrement, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 35, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(50, 50, 50))
        );
        modelOptimizerPanelLayout.setVerticalGroup(
            modelOptimizerPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(modelOptimizerPanelLayout.createSequentialGroup()
                .add(modelOptimizerPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
                    .add(referenceModelPath, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                    .add(refModelLabel))
                .add(11, 11, 11)
                .add(modelOptimizerPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                    .add(modelOptimizerPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                        .add(minIncrement, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                        .add(minIncrementLabel))
                    .add(modelOptimizerPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                        .add(nEvalPoints, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                        .add(nEvalPointsLabel)))
                .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        outputPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(javax.swing.BorderFactory.createEtchedBorder(), "Output"));
        outputPanel.setAutoscrolls(true);

        optimizedModelFileLabel.setText("Optimized Model File");

        outputOptimizedModelFilePath.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                outputOptimizedModelFilePathStateChanged(evt);
            }
        });

        org.jdesktop.layout.GroupLayout outputPanelLayout = new org.jdesktop.layout.GroupLayout(outputPanel);
        outputPanel.setLayout(outputPanelLayout);
        outputPanelLayout.setHorizontalGroup(
            outputPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(outputPanelLayout.createSequentialGroup()
                .addContainerGap()
                .add(optimizedModelFileLabel)
                .add(18, 18, 18)
                .add(outputOptimizedModelFilePath, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 405, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        outputPanelLayout.setVerticalGroup(
            outputPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(outputPanelLayout.createSequentialGroup()
                .addContainerGap()
                .add(outputPanelLayout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
                    .add(optimizedModelFileLabel)
                    .add(outputOptimizedModelFilePath, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        org.jdesktop.layout.GroupLayout jPanel1Layout = new org.jdesktop.layout.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .add(jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                    .add(modelOptimizerPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                    .add(outputPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(103, Short.MAX_VALUE))
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(jPanel1Layout.createSequentialGroup()
                .add(24, 24, 24)
                .add(modelOptimizerPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(18, 18, Short.MAX_VALUE)
                .add(outputPanel, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );

        modelOptimizerPanel.getAccessibleContext().setAccessibleName("");

        jTabbedPane.addTab("Settings", jPanel1);

        org.jdesktop.layout.GroupLayout layout = new org.jdesktop.layout.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(layout.createSequentialGroup()
                .addContainerGap()
                .add(jTabbedPane, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 670, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(layout.createSequentialGroup()
                .addContainerGap()
                .add(jTabbedPane))
        );
    }// </editor-fold>//GEN-END:initComponents

    private void outputOptimizedModelFilePathStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_outputOptimizedModelFilePathStateChanged
        optimizeToolModel.getOptimizeTool().getMuscleOptimizer().set_output_model_file(outputOptimizedModelFilePath.getFileName());
    }//GEN-LAST:event_outputOptimizedModelFilePathStateChanged

    private void minIncrementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_minIncrementActionPerformed
        try {
            double minIncrementValue= numFormat.parse(minIncrement.getText()).doubleValue();
            optimizeToolModel.setMinimumDegreesIncrement(minIncrementValue);
        } catch (ParseException ex) { // To catch parsing problems (string -> double)
            Toolkit.getDefaultToolkit().beep();
            double minIncrementOld = optimizeToolModel.getMinimumDegreesIncrement();
            minIncrement.setText(numFormat.format(minIncrementOld));
        }
    }//GEN-LAST:event_minIncrementActionPerformed

    private void minIncrementFocusLost(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_minIncrementFocusLost
        if(!evt.isTemporary()) minIncrementActionPerformed(null);
    }//GEN-LAST:event_minIncrementFocusLost

    private void referenceModelPathStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_referenceModelPathStateChanged
        // TODO add your handling code here:
        optimizeToolModel.updateReferenceModel(referenceModelPath.getFileName());
    }//GEN-LAST:event_referenceModelPathStateChanged

    private void nEvalPointsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_nEvalPointsActionPerformed
        try {
            double nevalpoints= numFormat.parse(nEvalPoints.getText()).doubleValue();
            optimizeToolModel.setNumberOfEvaluationPoints(nevalpoints);
        } catch (ParseException ex) { // To catch parsing problems (string -> double)
            Toolkit.getDefaultToolkit().beep();
            double nevalpointsOld = optimizeToolModel.getNumberOfEvaluationPoints();
            nEvalPoints.setText(numFormat.format(nevalpointsOld));
        }
    }//GEN-LAST:event_nEvalPointsActionPerformed

    private void nEvalPointsFocusLost(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_nEvalPointsFocusLost
        if(!evt.isTemporary()) nEvalPointsActionPerformed(null);
    }//GEN-LAST:event_nEvalPointsFocusLost

   //------------------------------------------------------------------------
   // Local variables
   //------------------------------------------------------------------------

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel editMuscleList1;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JTabbedPane jTabbedPane;
    private javax.swing.JTextField minIncrement;
    private javax.swing.JLabel minIncrementLabel;
    private javax.swing.JPanel modelOptimizerPanel;
    private javax.swing.JTextField nEvalPoints;
    private javax.swing.JLabel nEvalPointsLabel;
    private javax.swing.JLabel optimizedModelFileLabel;
    private org.opensim.swingui.FileTextFieldAndChooser outputOptimizedModelFilePath;
    private javax.swing.JPanel outputPanel;
    private javax.swing.JLabel refModelLabel;
    private org.opensim.swingui.FileTextFieldAndChooser referenceModelPath;
    // End of variables declaration//GEN-END:variables
   // Relinquish C++ resources by setting references to them to null
   public void cleanup()
   {
      optimizeToolModel.cleanup();
   }

}
