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

/* Create a dialog where pre simulation parameters can be changed. */
void presimu_param(gpointer parent, guint callback_action, GtkWidget *widget)
{

  GtkWidget *dialog, *table;
  GtkWidget *pre_check;
  GtkWidget *label1, *label2;
  GtkWidget *presimu_a[MAX_PRESIMU], *presimu_b[MAX_PRESIMU];
  GtkAdjustment *adj_a[MAX_PRESIMU], *adj_b[MAX_PRESIMU];
  int i;
  gint result;
  char label[10];
  
  dialog = gtk_dialog_new_with_buttons("Pre simulation parameters", parent,
           GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL,
           GTK_RESPONSE_CANCEL, NULL);
           
  pre_check = gtk_check_button_new_with_label("Use pre calculated pre simulation parameters");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pre_check), FALSE);
  if (use_presimu == TRUE)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pre_check), TRUE);
           
  label1 = gtk_label_new("a [deg/nm]:");
  label2 = gtk_label_new("b [deg]:");
  
  for (i = 0; i < MAX_PRESIMU; i++)
  {

    adj_a[i] = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_PREA, MAX_PREA, 0.01, 0.10, 0.00));
    presimu_a[i] = gtk_spin_button_new(adj_a[i], 0.01, 5);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(presimu_a[i]), presimu_x[i]);
    adj_b[i] = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_PREB, MAX_PREB, 0.1, 1.0, 0.00));
    presimu_b[i] = gtk_spin_button_new(adj_b[i], 0.1, 5);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(presimu_b[i]), presimu_y[i]);
    
  }
  
  /* Table */
  table = gtk_table_new(3, MAX_PRESIMU + 1, FALSE);
  gtk_table_attach_defaults(GTK_TABLE(table), label1, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), label2, 2, 3, 0, 1);
  for (i = 0; i < MAX_PRESIMU; i++)
  {
    sprintf(label, "Layer %d:", i+1);
    gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(label), 0, 1, i+1, i+2);
    gtk_table_attach_defaults(GTK_TABLE(table), presimu_a[i], 1, 2, i+1, i+2);
    gtk_table_attach_defaults(GTK_TABLE(table), presimu_b[i], 2, 3, i+1, i+2);
  }
  
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 10);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), pre_check);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), table);
  gtk_widget_show_all(dialog);

  result = gtk_dialog_run(GTK_DIALOG(dialog));
  
  /* Actions when clicking OK or CANCEL. */
  if (result == GTK_RESPONSE_OK)
  {
             
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pre_check)) )
      use_presimu = TRUE;
    else
      use_presimu = FALSE;
             
    for (i = 0; i < MAX_PRESIMU; i++)
    {
      presimu_x[i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(presimu_a[i]));
      presimu_y[i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(presimu_b[i]));
    }
    saved = 0;
  }
  gtk_widget_destroy(dialog);
  
}
