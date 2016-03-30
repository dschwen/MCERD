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

/* Create a dialog where x axis range can be changed. */
void xrange_param(gpointer parent, guint callback_action, GtkWidget *widget)
{

  GtkWidget *dialog, *table;
  GtkWidget *label1, *label2;
  GtkWidget *min, *max;
  GtkAdjustment *min_adj, *max_adj;
  double min_value, max_value;
  gint result;
  
  dialog = gtk_dialog_new_with_buttons("Range for x axis", parent,
           GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL,
           GTK_RESPONSE_CANCEL, NULL);
           
  label1 = gtk_label_new("Min:");
  label2 = gtk_label_new("Max:");
  
  min_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, 0.00, MAX_CH, 0.10, 1.00, 0.00));
  min = gtk_spin_button_new(min_adj, 0.10, 2);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(min), 0);
  
  max_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, 0.00, MAX_CH, 0.10, 1.00, 0.00));
  max = gtk_spin_button_new(max_adj, 0.10, 2);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(max), beam_energy);
  
  /* Table */
  table = gtk_table_new(2, 2, FALSE);
  gtk_table_attach_defaults(GTK_TABLE(table), label1, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), label2, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), min, 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), max, 1, 2, 1, 2);
  
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 10);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), table);
  gtk_widget_show_all(dialog);

  result = gtk_dialog_run(GTK_DIALOG(dialog));
  
  /* Actions when clicking OK or CANCEL. */
  if (result == GTK_RESPONSE_OK)
  {
    min_value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(min));
    max_value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(max));
    change_xrange(min_value, max_value);
  }
  gtk_widget_destroy(dialog);
}


/* Create a dialog where x axis scale can be changed.
 * Scale value (keV/channel) */
void xscale_param(gpointer parent, guint callback_action, GtkWidget *widget)
{
     
  GtkWidget *dialog, *table;
  GtkWidget *label, *label2;
  GtkWidget *scale;
  GtkAdjustment *scale_adj;
  double scale_value;
  gint result;
  
  dialog = gtk_dialog_new_with_buttons("Scale for x axis", parent,
           GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL,
           GTK_RESPONSE_CANCEL, NULL);
           
  label = gtk_label_new("Energy calibration:");
  label2 = gtk_label_new("[keV/channel]");
  
  scale_adj = GTK_ADJUSTMENT(gtk_adjustment_new(1.00, MIN_SCALE, MAX_SCALE, 1.00, 10.00, 0.00));
  scale = gtk_spin_button_new(scale_adj, 1.00, 3);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(scale), default_scale);
  
  /* Table */
  table = gtk_table_new(3, 1, FALSE);
  gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), scale, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), label2, 2, 3, 0, 1);
  
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 10);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), table);
  gtk_widget_show_all(dialog);

  result = gtk_dialog_run(GTK_DIALOG(dialog));
  
  /* Actions when clicking OK or CANCEL. */
  if (result == GTK_RESPONSE_OK)
  {

    scale_value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(scale));

    /* Change scale for both (simulation/experimental) data if both are used,
     * and use that for default scale */
    if (callback_action == PLOT_SIM)
    {
      default_scale = scale_value;
      read_data(sim_plot, PLOT_SIM, DEF_SCALE/default_scale);
      if (plot_path != NULL && strcmp(plot_path, ""))
        read_data(plot_path, PLOT_EXP, exp_scale/default_scale);
      update_plot();
    }
    /* Change scale for the experimental data in the same scale with the used default,
     * and keep scale for the experimental data in the memory */
    if (callback_action == PLOT_EXP)
    {
      exp_scale = scale_value;
      read_data(plot_path, PLOT_EXP, exp_scale/default_scale);
    }
    
  }
  gtk_widget_destroy(dialog);
     
}

