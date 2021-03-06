/* --------------------------------------------------------------------------*
*        Muscle Optimizer GUI: QuantityNameFilterCoordinatePanel.java        *
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

import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;
import java.util.regex.PatternSyntaxException;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import org.openide.DialogDisplayer;
import org.openide.NotifyDescriptor;
import org.opensim.modeling.ArrayObjPtr;
import org.opensim.modeling.ArrayStr;
import org.opensim.modeling.ForceSet;
import org.opensim.modeling.Model;
import org.opensim.modeling.ObjectGroup;
import org.opensim.view.pub.OpenSimDB;
import org.opensim.view.pub.ViewDB;

public class QuantityNameFilterCoordinatePanel extends javax.swing.JPanel implements Observer, TableModelListener,
                                                       DocumentListener
{
   private MuscleOptimizeToolModel optimizeToolModel;
   private QuantityNameFilterTableModel tableModel;
   private String pattern="";
   private Model currentModel=null;
   ArrayList<String> metaCharacters=new ArrayList<String>();

   public QuantityNameFilterCoordinatePanel(MuscleOptimizeToolModel optimizeToolModel) {
      initComponents();
      metaCharacters.add("*");
      metaCharacters.add("+");
      metaCharacters.add("?");
      FilterTextField.getDocument().addDocumentListener(this);
      FilterTextField.setText(getPattern());
      this.optimizeToolModel=optimizeToolModel;

      ArrayStr coordNames= new ArrayStr("");
      optimizeToolModel.getOriginalModel().getCoordinateSet().getNames(coordNames);
      String[] allAvailableCoords= new String[coordNames.size()];
      for(int i=0;i<coordNames.size(); i++){
          allAvailableCoords[i]= coordNames.get(i);
      }
      String selectedString="";
      ArrayStr currentSelectedCoords=optimizeToolModel.getCoordinates();
      if (currentSelectedCoords.size() > 0 && (currentSelectedCoords.get(0) == "ALL" || currentSelectedCoords.get(0) == ""))
          currentSelectedCoords=coordNames;
      boolean first=true;
      for(int i=0;i<currentSelectedCoords.size(); i++){
        if (first){
            selectedString += currentSelectedCoords.get(i);
            first=false;
        }
        else
            selectedString += ", "+currentSelectedCoords.get(i);
      }

       String[] preSelectedList = selectedString.split(",|\\+");
       Vector<String> selectedVec = new Vector<String>();
       for(int i=0; i<preSelectedList.length; i++){
           String trimmed=preSelectedList[i].trim();
           selectedVec.add(trimmed);
       }
       jTable1.putClientProperty("terminateEditOnFocusLost", Boolean.TRUE);
       tableModel = new QuantityNameFilterTableModel(allAvailableCoords, new String[]{"Coordinate Name", "Selected"});
       jTable1.setModel(tableModel);
       jTable1.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
       tableModel.addTableModelListener(this);
       tableModel.markSelectedNames(selectedVec);
       optimizeToolModel.addObserver(this);
       updateSelected();
   }

   public void update(Observable observable, Object obj) {
      MuscleOptimizeToolModel.Operation op = (MuscleOptimizeToolModel.Operation)obj;
      if(/*op==MuscleOptimizeToolModel.Operation.CoordinatesChanged ||*/ op==MuscleOptimizeToolModel.Operation.AllDataChanged)
         coordinatesChanged();
      updateSelected();
   }

   public void coordinatesChanged()
   {
      ArrayStr coordNames= new ArrayStr("");
      optimizeToolModel.getOriginalModel().getCoordinateSet().getNames(coordNames);
      String selectedString="";
      ArrayStr currentSelectedCoords=optimizeToolModel.getCoordinates();
      if (currentSelectedCoords.size() ==  0 || (currentSelectedCoords.size() > 0 && (currentSelectedCoords.get(0) == "ALL" || currentSelectedCoords.get(0) == "")))
          currentSelectedCoords=coordNames;
      boolean first=true;
      for(int i=0;i<currentSelectedCoords.size(); i++){
        if (first){
            selectedString += currentSelectedCoords.get(i);
            first=false;
        }
        else
            selectedString += ", "+currentSelectedCoords.get(i);
      }
      String[] preSelectedList = selectedString.split(",|\\+");
       Vector<String> selectedVec = new Vector<String>();
       for(int i=0; i<preSelectedList.length; i++){
           String trimmed=preSelectedList[i].trim();
           selectedVec.add(trimmed);
       }
       tableModel.markSelectedNames(selectedVec);
   }

   /** This method is called from within the constructor to
    * initialize the form.
    * WARNING: Do NOT modify this code. The content of this method is
    * always regenerated by the Form Editor.
    */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        buttonGroup1 = new javax.swing.ButtonGroup();
        jScrollPane1 = new javax.swing.JScrollPane();
        jTable1 = new javax.swing.JTable();

        jPanel1 = new javax.swing.JPanel();
        jNumSelectedLabel = new javax.swing.JLabel();
        jPanel2 = new javax.swing.JPanel();
        FilterTextField = new javax.swing.JFormattedTextField();
        jLabel1 = new javax.swing.JLabel();
        jShowAllButton = new javax.swing.JButton();
        jSelectAllCheckBox = new javax.swing.JCheckBox();
        jDeselectAllCheckBox = new javax.swing.JCheckBox();

        setPreferredSize(new java.awt.Dimension(300, 300));

        jTable1.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null},
                {null, null, null, null}
            },
            new String [] {
                "Title 1", "Title 2", "Title 3", "Title 4"
            }
        ));
        jTable1.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_LAST_COLUMN);
        jScrollPane1.setViewportView(jTable1);

        jNumSelectedLabel.setText("0 items selected");

        org.jdesktop.layout.GroupLayout jPanel1Layout = new org.jdesktop.layout.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(org.jdesktop.layout.GroupLayout.TRAILING, jPanel1Layout.createSequentialGroup()
                .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .add(jNumSelectedLabel)
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(jPanel1Layout.createSequentialGroup()
                .add(jNumSelectedLabel, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 25, Short.MAX_VALUE)
                .addContainerGap())
        );

        FilterTextField.setToolTipText("Regular expression. Use '.*' for wildcard, add patterns with '|' ");

        jLabel1.setText("Filter by pattern");

        jShowAllButton.setText("Show All");
        jShowAllButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jShowAllButtonActionPerformed(evt);
            }
        });

        org.jdesktop.layout.GroupLayout jPanel2Layout = new org.jdesktop.layout.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .add(jLabel1)
                .add(18, 18, 18)
                .add(FilterTextField, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 109, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(18, 18, 18)
                .add(jShowAllButton)
                .addContainerGap(12, Short.MAX_VALUE))
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .add(jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.BASELINE)
                    .add(jLabel1)
                    .add(FilterTextField, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                    .add(jShowAllButton))
                .addContainerGap(org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        FilterTextField.getAccessibleContext().setAccessibleName("");

        jSelectAllCheckBox.setText("select all shown");
        jSelectAllCheckBox.setToolTipText("select all shown quantities");
        jSelectAllCheckBox.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        jSelectAllCheckBox.setMargin(new java.awt.Insets(0, 0, 0, 0));
        jSelectAllCheckBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jSelectAllCheckBoxActionPerformed(evt);
            }
        });

        jDeselectAllCheckBox.setText("deselect all shown");
        jDeselectAllCheckBox.setToolTipText("deselect all shown quantities");
        jDeselectAllCheckBox.setBorder(javax.swing.BorderFactory.createEmptyBorder(0, 0, 0, 0));
        jDeselectAllCheckBox.setMargin(new java.awt.Insets(0, 0, 0, 0));
        jDeselectAllCheckBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jDeselectAllCheckBoxActionPerformed(evt);
            }
        });

        org.jdesktop.layout.GroupLayout layout = new org.jdesktop.layout.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(org.jdesktop.layout.GroupLayout.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.TRAILING)
                    .add(org.jdesktop.layout.GroupLayout.LEADING, jScrollPane1, 0, 394, Short.MAX_VALUE)
                    .add(org.jdesktop.layout.GroupLayout.LEADING, layout.createSequentialGroup()
                        .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                            .add(jSelectAllCheckBox)
                            .add(jDeselectAllCheckBox))
                        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                        .add(jPanel1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                    .add(org.jdesktop.layout.GroupLayout.LEADING, jPanel2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
            .add(org.jdesktop.layout.GroupLayout.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .add(jPanel2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(18, 18, 18)
                .add(jScrollPane1, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, 200, Short.MAX_VALUE)
                .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                    .add(layout.createSequentialGroup()
                        .add(jSelectAllCheckBox)
                        .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
                        .add(jDeselectAllCheckBox))
                    .add(jPanel1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents


    private void jDeselectAllCheckBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jDeselectAllCheckBoxActionPerformed
// TODO add your handling code here:
         tableModel.selectShown(false);
         updateSelected();
        ((JCheckBox)evt.getSource()).setSelected(false);

    }//GEN-LAST:event_jDeselectAllCheckBoxActionPerformed

    private void jSelectAllCheckBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jSelectAllCheckBoxActionPerformed
        tableModel.selectShown(true);
        updateSelected();
        ((JCheckBox)evt.getSource()).setSelected(false);
// TODO add your handling code here:
    }//GEN-LAST:event_jSelectAllCheckBoxActionPerformed

    private void jShowAllButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jShowAllButtonActionPerformed
        pattern = ".*";
        tableModel.restrictNamesBy(getPattern());
        // TODO add your handling code here:
    }//GEN-LAST:event_jShowAllButtonActionPerformed

   public void applyPattern(String regex)
{
      pattern = regex;
      FilterTextField.setText(getPattern());

      regex=convertToJavaRegex(FilterTextField);
      FilterTextField.setText(regex);
      pattern = regex;
      try{
         tableModel.applyFilter(regex);
         tableModel.fireTableDataChanged();
      } catch(PatternSyntaxException ex){
         // Should complain here'
         //jLabel4.setText(regex+".*"+"will complain");
      }

      updateSelected();
   }
    private void updateSelected() {
        int n = tableModel.getNumSelected();
        jNumSelectedLabel.setText(String.valueOf(n)+" items selected");
        int nShownSelected = tableModel.getNumShownAndSelected();
        if (nShownSelected==0){
            // enable select all, disable deselect all
            jSelectAllCheckBox.setEnabled(true);
            jDeselectAllCheckBox.setEnabled(false);
        }
        else if (nShownSelected==tableModel.getRowCount()){
            jSelectAllCheckBox.setEnabled(false);
            jDeselectAllCheckBox.setEnabled(true);
        }
        else {
            jSelectAllCheckBox.setEnabled(true);
            jDeselectAllCheckBox.setEnabled(true);
        }
    }

    public void tableChanged(TableModelEvent e) {
        int type=e.getType();
        updateSelected();

       String selectedCoords=getSelectedAsString();
       String[] selectedCoordsList = selectedCoords.split(",|\\+");
       ArrayStr selectedCoordVec = new ArrayStr();
       for(int i=0; i<selectedCoordsList.length; i++){
           String trimmed=selectedCoordsList[i].trim();
           selectedCoordVec.append(trimmed);
       }
       optimizeToolModel.setCoordinates(selectedCoordVec);
    }


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JFormattedTextField FilterTextField;
    private javax.swing.ButtonGroup buttonGroup1;
    private javax.swing.JCheckBox jDeselectAllCheckBox;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jNumSelectedLabel;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JCheckBox jSelectAllCheckBox;
    private javax.swing.JButton jShowAllButton;
    private javax.swing.JTable jTable1;
    // End of variables declaration//GEN-END:variables

     /**
     * Convert string to canonical regular expression syntax
     * in particular convert * -> .*, ?->.?
     *
     */
    private String convertToJavaRegex(JTextField field)
    {
      String regex = FilterTextField.getText();
      // Replace * with .* ?with .? as it's more common
      // There appears to be a bug where the replacement cant be done inplace
      int idx = regex.indexOf("*", 0);
      //System.out.println("Regex=["+regex+"]");
      String test="";
      while(getPattern().matches(".*[^\\.]\\*")){
         test=regex.replaceAll("\\*", "\\.\\*");
         regex=test;
      }
      idx = regex.indexOf("?", 0);
      if (idx !=-1 && regex.indexOf(".?", 0)==-1){
         test=regex.replaceAll("\\?", "\\.\\?");
         regex=test;
      }
      return regex;
    }

    String getSelectedAsString() {
        return tableModel.getSelectedAsString();
    }

    public String getPattern() {
        return pattern;
        /**/

    }
    // DocumentListener implementation for changes in the pattern text field
    // to avoid working with individual key strokes that may correspond
    // to Ctrl characters, delete, paste text, ..
    public void insertUpdate(DocumentEvent e) {
        handlePatternChange();
    }

    public void removeUpdate(DocumentEvent e) {
        handlePatternChange();
    }

    public void changedUpdate(DocumentEvent e) {
        handlePatternChange();
    }
    /**
     * Handle changes to the text in the FilterTextField
     * a leading and a trailing .* are pre/appended for matching any substring
     */
    private void handlePatternChange()
    {
       String rawPattern = FilterTextField.getText();
       // Remove leading and trailing special characters that may interfere
       if (rawPattern.length()>0){
          // remove leading and trailing special characters
          int index=0;
          String newPattern=rawPattern;
          while(index < rawPattern.length()){
            String letter=rawPattern.substring(index, index+1);
            if (metaCharacters.contains(letter))
               newPattern=rawPattern.substring(index);
            else
               break;
            index++;
          }
          rawPattern=newPattern;
          // start from the end and repeat
          index = rawPattern.length()-1;
           while(index >= 0){
            String letter=rawPattern.substring(index, index+1);
            if (metaCharacters.contains(letter))
               newPattern=rawPattern.substring(0, index);
            else
               break;
            index--;
          }
          rawPattern=newPattern;
      }

       tableModel.restrictNamesBy(".*"+rawPattern+".*");
    }

    public void addSelectionChangeListener(TableModelListener l)
    {
        tableModel.addTableModelListener(l);
    }
    public void removeSelectionChangeListener(TableModelListener l)
    {
        tableModel.removeTableModelListener(l);
    }
}
