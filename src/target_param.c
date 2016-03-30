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

/* Dialog for controlling target layers */
void layers(gpointer parent, guint callback_action, GtkWidget *widget)
{
     
  GtkWidget *notebook, *dialog;
  
  /* Tables for indexing text entry or spin button widgets */
  GtkWidget *thickness[MAX_LAYERS];
  GtkWidget *stop_ion[MAX_LAYERS];
  GtkWidget *stop_rec[MAX_LAYERS];
  GtkWidget *density[MAX_LAYERS];
  GtkWidget *element[MAX_ELEMENTS][MAX_LAYERS];
  GtkWidget *concentration[MAX_ELEMENTS][MAX_LAYERS];
  GtkWidget *mass[MAX_ELEMENTS][MAX_LAYERS];
  
  gint result = TYPE_ADD;
  int i;
  int layer_count; /* For saving the changes in the number of layers */
  layer_count = nlayers;
  
  notebook = gtk_notebook_new();
  
  /* Add all layers */
  for (i = 0; i < nlayers; i++)
  {
    add_layer(notebook, i, thickness, stop_ion, stop_rec, density, element, concentration, mass);
  }
  
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
  dialog = gtk_dialog_new_with_buttons("Target layers", parent, GTK_DIALOG_MODAL,
           GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
           "Add layer", TYPE_ADD, "Remove layer", TYPE_REMOVE, NULL);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), notebook);
  
  /* Closing the dialog only when something else than "add" or "remove" is pressed. */
  while ( (result == TYPE_REMOVE) || (result == TYPE_ADD) )
  {
    gtk_widget_show_all(dialog);
    result = gtk_dialog_run(GTK_DIALOG(dialog));
    
    /* Actions when clicking "Add layer" or "Remove layer" */
    if (result == TYPE_REMOVE)
    {
      if (layer_count > 0)
      {
        remove_layer(notebook, layer_count);
        layer_count--;
      }
    }
    
    else if (result == TYPE_ADD)
    {
      if (layer_count < MAX_LAYERS)
      {
        add_layer(notebook, layer_count, thickness, stop_ion, stop_rec, density, element, concentration, mass);
        layer_count++;
      }
    }
    /* Actions when clicking OK or CANCEL */
    else if (result == GTK_RESPONSE_OK)
    {
      /* Save parameters (in the variables) */
      save_target_values(layer_count, thickness, stop_ion, stop_rec, density, element, concentration, mass);
    }
  }
  gtk_widget_destroy(dialog);
}

/* Add one layer */
void add_layer(GtkWidget *notebook, int layer_num, GtkWidget *thickness[], GtkWidget *stop_ion[],
               GtkWidget *stop_rec[], GtkWidget *density[], GtkWidget *element[][MAX_LAYERS],
               GtkWidget *concentration[][MAX_LAYERS], GtkWidget *mass[][MAX_LAYERS])
{

  GtkWidget *table;
  
  table = gtk_table_new(3, MAX_ELEMENTS + 3, FALSE);
  
  thickness[layer_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_D, MAX_D, 10.00, 100.00, 0.00)), 10.00, 2);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(thickness[layer_num]), layer_thickness[layer_num]);
  stop_ion[layer_num] = gtk_entry_new();
  stop_rec[layer_num] = gtk_entry_new();
  if (stop_ion_layer[layer_num] != NULL)
  {
    gtk_entry_set_text(GTK_ENTRY(stop_ion[layer_num]), g_locale_to_utf8(stop_ion_layer[layer_num], -1, NULL, NULL, NULL));
  }
  if (stop_rec_layer[layer_num] != NULL)
  {
    gtk_entry_set_text(GTK_ENTRY(stop_rec[layer_num]), g_locale_to_utf8(stop_rec_layer[layer_num], -1, NULL, NULL, NULL));
  }
  density[layer_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.000, MIN_DENS, MAX_DENS, 1.000, 10.000, 0.000)), 10.000, 3);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(density[layer_num]), layer_density[layer_num]);
  
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Layer thickness:"), 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), thickness[layer_num], 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(U_THICKNESS), 2, 3, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Stopping power for the ion:"), 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), stop_ion[layer_num], 1, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Stopping power for the recoil:"), 0, 1, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), stop_rec[layer_num], 1, 2, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Layer density:"), 0, 1, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table), density[layer_num], 1, 2, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(U_DENSITY), 2, 3, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Element:"), 0, 1, 4, 5);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Mass [u]:"), 1, 2, 4, 5);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Concentration:"), 2, 3, 4, 5);
  
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, gtk_label_new("layer"));
  
  /* Add element table */
  add_elements(table, layer_num, element, concentration, mass);
}

/* Remove one layer. Only the last layer can be removed. */
void remove_layer(GtkWidget *notebook, int layer_num)
{
  int page_num = layer_num - 1;
  gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), page_num);
}

/* Add all elements to specific layer */
void add_elements(GtkWidget *table, int layer_num, GtkWidget *element[][MAX_LAYERS],
                  GtkWidget *concentration[][MAX_LAYERS], GtkWidget *mass[][MAX_LAYERS])
{
  int j;
  for (j = 0; j < MAX_ELEMENTS; j++)
  {
    element[j][layer_num] = gtk_entry_new();
    mass[j][layer_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.000, MIN_M, MAX_M, 1.000, 10.000, 0.000)), 1.000, 3);
    concentration[j][layer_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.000, MIN_CON, MAX_CON, 0.100, 0.100, 0.000)), 0.100, 3);
    if (elem_layer[j][layer_num] != NULL)
    {
      gtk_entry_set_text(GTK_ENTRY(element[j][layer_num]), g_locale_to_utf8(elem_layer[j][layer_num], -1, NULL, NULL, NULL));
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(mass[j][layer_num]), mass_layer[j][layer_num]);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(concentration[j][layer_num]), con_layer[j][layer_num]);
    }

    gtk_table_attach_defaults(GTK_TABLE(table), element[j][layer_num], 0, 1, j+5, j+6);
    gtk_table_attach_defaults(GTK_TABLE(table), mass[j][layer_num], 1, 2, j+5, j+6);
    gtk_table_attach_defaults(GTK_TABLE(table), concentration[j][layer_num], 2, 3, j+5, j+6);
    
    gtk_table_set_row_spacings(GTK_TABLE(table), 5);
    gtk_table_set_col_spacings(GTK_TABLE(table), 5);
    gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  }
}

/* Save parameters in the variables */
void save_target_values(int layer_count, GtkWidget *thickness[], GtkWidget *stop_ion[],
                        GtkWidget *stop_rec[], GtkWidget *density[], GtkWidget *element[][MAX_LAYERS],
                        GtkWidget *concentration[][MAX_LAYERS], GtkWidget *mass[][MAX_LAYERS])
{
  nlayers = layer_count;
  int i;
  int j;
  for (i = 0; i < nlayers; i++)
  {
    layer_thickness[i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(thickness[i]));
    stop_ion_layer[i] = g_locale_from_utf8(gtk_entry_get_text(GTK_ENTRY(stop_ion[i])), -1, NULL, NULL, NULL);
    stop_rec_layer[i] = g_locale_from_utf8(gtk_entry_get_text(GTK_ENTRY(stop_rec[i])), -1, NULL, NULL, NULL);
    layer_density[i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(density[i]));
    
    for (j = 0; j < MAX_ELEMENTS; j++)
    {
      if (gtk_entry_get_text(GTK_ENTRY(element[j][i])) != NULL)
      {
        elem_layer[j][i] = g_locale_from_utf8(gtk_entry_get_text(GTK_ENTRY(element[j][i])), -1, NULL, NULL, NULL);
      }
      mass_layer[j][i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(mass[j][i]));
      con_layer[j][i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(concentration[j][i]));
    }
  }
  
  format_layers(nlayers); /* Clean "memory" */
  saved = 0;

}
