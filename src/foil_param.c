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

/* Dialog for controlling detector foils. */
void foils(gpointer parent, guint callback_action, GtkWidget *widget)
{
  GtkWidget *notebook, *dialog;
  
  /* Tables for indexing text entry or spin button widgets. */
  GtkWidget *type[2][MAX_FOILS];
  GtkWidget *size[3][MAX_FOILS];
  GtkWidget *distance[MAX_FOILS];
  GtkWidget *stop_ion[MAX_FOILS];
  GtkWidget *stop_rec[MAX_FOILS];
  GtkWidget *element[MAX_FELEMENTS][MAX_FOILS];
  GtkWidget *concentration[MAX_FELEMENTS][MAX_FOILS];
  GtkWidget *mass[MAX_FELEMENTS][MAX_FOILS];
  GtkWidget *thickness[MAX_FOILS];
  GtkWidget *density[MAX_FOILS];
  
  gint result = TYPE_ADD;
  int i;
  int foil_count; /* For saving the changes in the number of foils */
  foil_count = nfoils;
  
  notebook = gtk_notebook_new();
  
  /* Add all foils */
  for (i = 0; i < nfoils; i++)
  {
    add_foil(notebook, i, type, size, distance, stop_ion, stop_rec, element,
             concentration, mass, thickness, density);
  }
  
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
  dialog = gtk_dialog_new_with_buttons("Foils", parent, GTK_DIALOG_MODAL, GTK_STOCK_OK,
           GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, "Add foil", TYPE_ADD,
           "Remove foil", TYPE_REMOVE, NULL);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), notebook);
  
  /* Closing the dialog only when something else than "add" or "remove" is pressed. */
  while ( (result == TYPE_REMOVE) || (result == TYPE_ADD) )
  {
    gtk_widget_show_all(dialog);
    result = gtk_dialog_run(GTK_DIALOG(dialog));
    
    /* Actions when clicking "Add foil" or "Remove foil" */
    if (result == TYPE_REMOVE)
    {
      if (foil_count > 0)
      {
        remove_foil(notebook, foil_count);
        foil_count--;
      }
    }
    
    else if (result == TYPE_ADD)
    {
      if (foil_count < MAX_FOILS)
      {
        add_foil(notebook, foil_count, type, size, distance, stop_ion, stop_rec,
                 element, concentration, mass, thickness, density);
        foil_count++;
      }
    }
    
    /* Actions when clicking OK or CANCEL */
    else if (result == GTK_RESPONSE_OK)
    {
      /* Save parameters (in the variables). */
      save_foil_values(foil_count, type, size, distance, stop_ion, stop_rec, element,
                       concentration, mass, thickness, density);
    }
  }
  gtk_widget_destroy(dialog);
}

/* Add one foil */
void add_foil(GtkWidget *notebook, int foil_num, GtkWidget *type[][MAX_FOILS],
              GtkWidget *size[][MAX_FOILS], GtkWidget *distance[], GtkWidget *stop_ion[],
              GtkWidget *stop_rec[], GtkWidget *element[][MAX_FOILS],
              GtkWidget *concentration[][MAX_FOILS], GtkWidget *mass[][MAX_FOILS],
              GtkWidget *thickness[], GtkWidget *density[])
{
  GtkWidget *table, *hbox_type, *hbox_size, *size_x, *size_y;
  
  table = gtk_table_new(4, 8 + MAX_FELEMENTS, FALSE);

  type[0][foil_num] = gtk_radio_button_new_with_label(NULL, "circular");
  type[1][foil_num] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(type[0][foil_num]), "rectangular");
  hbox_type = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_type), type[0][foil_num]);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_type), type[1][foil_num]);
  size[0][foil_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_DIAM, MAX_DIAM, 1.00, 10.00, 0.00)), 1.00, 2);
  size[1][foil_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_DIAM, MAX_DIAM, 1.00, 10.00, 0.00)), 1.00, 2);
  size[2][foil_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_DIAM, MAX_DIAM, 1.00, 10.00, 0.00)), 1.00, 2);
  size_x = gtk_label_new("x:");
  size_y = gtk_label_new("y:");
  hbox_size = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_size), size_x);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_size), size[1][foil_num]);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_size), size_y);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_size), size[2][foil_num]);
  
  if (foil_type[foil_num] == FOIL_RECT)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(type[1][foil_num]) ,TRUE);
    gtk_widget_set_sensitive(size[0][foil_num], FALSE);
  }
  else
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(type[0][foil_num]) ,TRUE);
    gtk_widget_set_sensitive(hbox_size, FALSE);
  }
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(size[0][foil_num]), foil_diameter[0][foil_num]);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(size[1][foil_num]), foil_diameter[1][foil_num]);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(size[2][foil_num]), foil_diameter[2][foil_num]);
  
  /* Controls that only one (circular or rectangular) can be selected. */
  g_signal_connect(G_OBJECT(type[0][foil_num]), "toggled", G_CALLBACK(hide_check), (gpointer) hbox_size);
  g_signal_connect(G_OBJECT(type[1][foil_num]), "toggled", G_CALLBACK(hide_check), (gpointer) size[0][foil_num]);
  
  distance[foil_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_DIST, MAX_DIST, 10.00, 100.00, 0.00)), 10.00, 2);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(distance[foil_num]), foil_distance[foil_num]);
  
  stop_ion[foil_num] = gtk_entry_new();
  stop_rec[foil_num] = gtk_entry_new();
  if (stop_ion_foil[foil_num] != NULL)
  {
    gtk_entry_set_text(GTK_ENTRY(stop_ion[foil_num]),
                       g_locale_to_utf8(stop_ion_foil[foil_num], -1, NULL, NULL, NULL));
  }
  if (stop_rec_foil[foil_num] != NULL)
  {
    gtk_entry_set_text(GTK_ENTRY(stop_rec[foil_num]),
                       g_locale_to_utf8(stop_rec_foil[foil_num], -1, NULL, NULL, NULL));
  }
  
  thickness[foil_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_D, MAX_D, 10.00, 100.00, 0.00)), 10.00, 2);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(thickness[foil_num]), foil_thickness[foil_num]);
  density[foil_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.000, MIN_DENS, MAX_DENS, 1.000, 10.000, 0.000)), 1.000, 3);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(density[foil_num]), foil_density[foil_num]);

  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Foil type:"), 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), hbox_type, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Foil diameter (circular):"), 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), size[0][foil_num], 1, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(U_SIZE), 2, 3, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Foil width and height (rectangular):"), 0, 1, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), hbox_size, 1, 2, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(U_SIZE), 2, 3, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Foil distance:"), 0, 1, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table), distance[foil_num], 1, 2, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(U_SIZE), 2, 3, 3, 4);
  
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Foil thickness:"), 0, 1, 5, 6);
  gtk_table_attach_defaults(GTK_TABLE(table), thickness[foil_num], 1, 2, 5, 6);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(U_THICKNESS), 2, 3, 5, 6);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Stopping power for the ion:"), 0, 1, 6, 7);
  gtk_table_attach_defaults(GTK_TABLE(table), stop_ion[foil_num], 1, 2, 6, 7);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Stopping power for the recoil:"), 0, 1, 7, 8);
  gtk_table_attach_defaults(GTK_TABLE(table), stop_rec[foil_num], 1, 2, 7, 8);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Foil density:"), 0, 1, 8, 9);
  gtk_table_attach_defaults(GTK_TABLE(table), density[foil_num], 1, 2, 8, 9);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new(U_DENSITY), 2, 3, 8, 9);
  
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Element:"), 0, 1, 9, 10);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Mass [u]:"), 1, 2, 9, 10);
  gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Concentration:"), 2, 3, 9, 10);
  
  /* Add element table */
  add_felements(table, foil_num, element, concentration, mass);
  
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, gtk_label_new("foil"));

}

/* Remove one foil. Only the last foil can be removed. */
void remove_foil(GtkWidget *notebook, int foil_num)
{
  int page_num = foil_num - 1;
  gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), page_num);
}

/* Add all elements to the specific foil. */
void add_felements(GtkWidget *table, int foil_num, GtkWidget *element[][MAX_FOILS],
                   GtkWidget *concentration[][MAX_FOILS], GtkWidget *mass[][MAX_FOILS])
{
  int j;
  for (j = 0; j < MAX_FELEMENTS; j++)
  {
    element[j][foil_num] = gtk_entry_new();
    mass[j][foil_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.000, MIN_M, MAX_M, 1.000, 10.000, 0.000)), 1.000, 3);
    concentration[j][foil_num] = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(0.000, MIN_CON, MAX_CON, 0.100, 0.100, 0.000)), 0.100, 3);
    
    if (elem_foil[j][foil_num] != NULL)
    {
      gtk_entry_set_text(GTK_ENTRY(element[j][foil_num]), g_locale_to_utf8(elem_foil[j][foil_num], -1, NULL, NULL, NULL));
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(mass[j][foil_num]), mass_foil[j][foil_num]);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(concentration[j][foil_num]), con_foil[j][foil_num]);
    }
    
    gtk_table_attach_defaults(GTK_TABLE(table), element[j][foil_num], 0, 1, j+10, j+11);
    gtk_table_attach_defaults(GTK_TABLE(table), mass[j][foil_num], 1, 2, j+10, j+11);
    gtk_table_attach_defaults(GTK_TABLE(table), concentration[j][foil_num], 2, 3, j+10, j+11);
    
    gtk_table_set_row_spacings(GTK_TABLE(table), 5);
    gtk_table_set_col_spacings(GTK_TABLE(table), 5);
    gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  }
}

/* Save all parameters (in the variables). */
void save_foil_values(int foil_count, GtkWidget *type[][MAX_FOILS], GtkWidget *size[][MAX_FOILS],
                      GtkWidget *distance[], GtkWidget *stop_ion[], GtkWidget *stop_rec[],
                      GtkWidget *element[][MAX_FOILS], GtkWidget *concentration[][MAX_FOILS],
                      GtkWidget *mass[][MAX_FOILS], GtkWidget *thickness[], GtkWidget *density[])
{
  nfoils = foil_count;
  int i;
  int j;

  for (i = 0; i < nfoils; i++)
  {
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(type[0][i])) )
      foil_type[i] = FOIL_CIRC; 
    else if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(type[1][i])) )
      foil_type[i] = FOIL_RECT;

    foil_diameter[0][i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(size[0][i]));
    foil_diameter[1][i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(size[1][i]));
    foil_diameter[2][i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(size[2][i]));

    foil_distance[i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(distance[i]));
    
    for (j = 0; j < MAX_FELEMENTS; j++)
    {
      if (gtk_entry_get_text(GTK_ENTRY(element[j][i])) != NULL)
      {
        elem_foil[j][i] = g_locale_from_utf8(gtk_entry_get_text(GTK_ENTRY(element[j][i])), -1, NULL, NULL, NULL);
      }
      mass_foil[j][i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(mass[j][i]));
      con_foil[j][i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(concentration[j][i]));
    }
    
    foil_thickness[i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(thickness[i]));
    stop_ion_foil[i] = g_locale_from_utf8(gtk_entry_get_text(GTK_ENTRY(stop_ion[i])), -1, NULL, NULL, NULL);
    stop_rec_foil[i] = g_locale_from_utf8(gtk_entry_get_text(GTK_ENTRY(stop_rec[i])), -1, NULL, NULL, NULL);
    foil_density[i] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(density[i]));
  }
  
  format_foils(nfoils); /* Cleans removed foils in the "memory" */
  saved = 0;
}
