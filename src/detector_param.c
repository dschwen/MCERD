/*
    Copyright 2010 Kai Arstila, Jouni Heiskanen

    This file is part of MCERD.

    MCERD is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MCERD is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MCERD. If not, see <http://www.gnu.org/licenses/>.
*/



#include "general.h"
#include "protot.h"

/* Create a dialog where detector parameters can be changed. */
void detector_param(gpointer parent, guint callback_action, GtkWidget *widget)
{

  GtkWidget *dialog, *table;
  GtkWidget *type_label, *type_tof, *type_gas, *hbox_type;
  GtkWidget *angle_label, *angle, *angle_unit;
  GtkAdjustment *angle_adj;
  GtkWidget *size_label, *size1, *size2;
  GtkAdjustment *size1_adj, *size2_adj;
  GtkWidget *num_label, *num1, *num2;
  GtkAdjustment *num1_adj, *num2_adj;
  gint result;

  dialog = gtk_dialog_new_with_buttons("Detector parameters", parent,
           GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL,
           GTK_RESPONSE_CANCEL, NULL);

  type_label = gtk_label_new("Detector type:");
  type_tof = gtk_radio_button_new_with_label(NULL, "TOF");
  type_gas = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(type_tof), "Gas");
  hbox_type = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_type), type_tof);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_type), type_gas);
  if (det_type == DET_GAS)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(type_gas) ,TRUE);
  }
  else
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(type_tof) ,TRUE);
  }
  
  angle_label = gtk_label_new("Detector angle (relative to the beam direction):");
  angle_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_DETANGLE, MAX_DETANGLE, 1.00, 10.00, 0.00));
  angle = gtk_spin_button_new(angle_adj, 1.00, 2);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(angle), det_angle);
  angle_unit = gtk_label_new(U_ANGLE);
  
  size_label = gtk_label_new("Virtual detector size (relative to the real detector):");
  size1_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_VDETSIZE, MAX_VDETSIZE, 1.00, 10.00, 0.00));
  size2_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_VDETSIZE, MAX_VDETSIZE, 1.00, 10.00, 0.00));
  size1 = gtk_spin_button_new(size1_adj, 1.00, 2);
  size2 = gtk_spin_button_new(size2_adj, 1.00, 2);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(size1), vdet_size1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(size2), vdet_size2);
  
  num_label = gtk_label_new("Timing detector numbers (detector foil indexes):");
  num1_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0, MIN_TDETNUM, MAX_TDETNUM, 1, 2, 0));
  num2_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0, MIN_TDETNUM, MAX_TDETNUM, 1, 2, 0));
  num1 = gtk_spin_button_new(num1_adj, 1, 0);
  num2 = gtk_spin_button_new(num2_adj, 1, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(num1), (double)det_num1);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(num2), (double)det_num2);

  /* Table for all parameters */
  table = gtk_table_new(4, 4, FALSE);
  gtk_table_attach_defaults(GTK_TABLE(table), type_label, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), angle_label, 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), size_label, 0, 1, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), num_label, 0, 1, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table), hbox_type, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), angle, 1, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), size1, 1, 2, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), num1, 1, 2, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table), angle_unit, 2, 3, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), size2, 2, 3, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), num2, 2, 3, 3, 4);

  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 10);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), table);
  gtk_widget_show_all(dialog);

  result = gtk_dialog_run(GTK_DIALOG(dialog));

  /* Actions when clicking OK or CANCEL. */
  if (result == GTK_RESPONSE_OK)
  {
    /* Save parameters (in the variables) */
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(type_tof)) )
      det_type = DET_TOF;
    else if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(type_gas)) )
      det_type = DET_GAS;
    det_angle = gtk_spin_button_get_value(GTK_SPIN_BUTTON(angle));
    vdet_size1 = gtk_spin_button_get_value(GTK_SPIN_BUTTON(size1));
    vdet_size2 = gtk_spin_button_get_value(GTK_SPIN_BUTTON(size2));
    det_num1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(num1));
    det_num2 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(num2));
    
    saved = 0;
    
  }
  gtk_widget_destroy(dialog);
}
