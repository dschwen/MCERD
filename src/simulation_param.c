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

/* Create a dialog where simulation parameters can be changed */
void simulation_param(gpointer parent, guint callback_action, GtkWidget *widget)
{

  GtkWidget *dialog, *table;
  
  GtkWidget *type_label, *type_erd, *type_rbs, *hbox_type;
  GtkWidget *ion_label, *ion;
  GtkWidget *energy_label, *energy, *energy_unit;
  GtkAdjustment *energy_adj;
  
  GtkWidget *recoil_label, *recoil;
  GtkWidget *angle_label, *angle, *angle_unit;
  GtkAdjustment *angle_adj;
  GtkWidget *beamsize_label, *x_label, *y_label, *x, *y, *beamsize_unit, *hbox_beamsize;
  GtkAdjustment *x_adj, *y_adj;
  
  GtkWidget *minangle_label, *minangle, *minangle_unit;
  GtkAdjustment *minangle_adj;
  GtkWidget *minenergy_label, *minenergy, *minenergy_unit;
  GtkAdjustment *minenergy_adj;
  GtkWidget *averions_label, *averions;
  GtkAdjustment *averions_adj;
  GtkWidget *width_label, *width_narrow, *width_wide, *hbox_width;
  
  GtkWidget *ions_label, *ions;
  GtkAdjustment *ions_adj;
  GtkWidget *ionspre_label, *ionspre;
  GtkAdjustment *ionspre_adj;
  GtkWidget *seed_label, *seed;
  GtkAdjustment *seed_adj;

  gint result;
  
  dialog = gtk_dialog_new_with_buttons("Simulation parameters", parent, GTK_DIALOG_MODAL,
           GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
  
  type_label = gtk_label_new("Simulation type:");
  type_erd = gtk_radio_button_new_with_label(NULL, "ERD");
  type_rbs = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(type_erd), "RBS");
  hbox_type = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_type), type_erd);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_type), type_rbs);
  if (sim_type == SIM_RBS)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(type_rbs) ,TRUE);
  }
  else
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(type_erd) ,TRUE);
  }
  
  ion_label = gtk_label_new("Beam ion:");
  ion = gtk_entry_new();
  if (beam_ion != NULL)
    gtk_entry_set_text(GTK_ENTRY(ion), g_locale_to_utf8(beam_ion, -1, NULL, NULL, NULL));
  
  energy_label = gtk_label_new("Beam energy:");
  energy_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_E, MAX_E, 1.00, 5.00, 0.00));
  energy = gtk_spin_button_new(energy_adj, 1.00, 3);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(energy), beam_energy);
  energy_unit = gtk_label_new(U_ENERGY);
  
  recoil_label = gtk_label_new("Recoiling atom:");
  recoil = gtk_entry_new();
  if (recoil_atom !=NULL)
    gtk_entry_set_text(GTK_ENTRY(recoil), g_locale_to_utf8(recoil_atom, -1, NULL, NULL, NULL));
  
  angle_label = gtk_label_new("Target angle:");
  angle_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_TARGANGLE, MAX_TARGANGLE, 1.00, 5.00, 0.00));
  angle = gtk_spin_button_new(angle_adj, 1.00, 2);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(angle), target_angle);
  angle_unit = gtk_label_new(U_ANGLE);
  
  beamsize_label = gtk_label_new("Beam spot size:");
  x_label = gtk_label_new("x:");
  y_label = gtk_label_new("y:");
  x_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_BEAMSIZE, MAX_BEAMSIZE, 1.00, 10.00, 0.00));
  x = gtk_spin_button_new(x_adj, 1.00, 2);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(x), beam_size_x);
  y_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_BEAMSIZE, MAX_BEAMSIZE, 1.00, 10.00, 0.00));
  y = gtk_spin_button_new(y_adj, 1.00, 2);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(y), beam_size_y);
  hbox_beamsize = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_beamsize), x_label);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_beamsize), x);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_beamsize), y_label);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_beamsize), y);
  beamsize_unit = gtk_label_new(U_SIZE);
  
  minangle_label = gtk_label_new("Minimum angle of scattering:");
  minangle_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.000, MIN_SANGLE, MAX_SANGLE, 0.050, 0.500, 0.000));
  minangle = gtk_spin_button_new(minangle_adj, 0.050, 3);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(minangle), min_angle);
  minangle_unit = gtk_label_new(U_ANGLE);
  
  minenergy_label = gtk_label_new("Minimum energy of ions:");
  minenergy_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.000, MIN_E, MAX_E/10, 0.100, 0.500, 0.000));
  minenergy = gtk_spin_button_new(minenergy_adj, 0.100, 3);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(minenergy), min_energy);
  minenergy_unit = gtk_label_new(U_ENERGY);
  
  averions_label = gtk_label_new("Average number of recoils per primary ion:");
  averions_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0, MIN_RECAVE, MAX_RECAVE, 1, 10, 0));
  averions = gtk_spin_button_new(averions_adj, 1, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(averions), (double) aver_nions);
  
  width_label = gtk_label_new("Recoil angle width:");
  width_narrow = gtk_radio_button_new_with_label(NULL, "Narrow");
  width_wide = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(width_narrow), "Wide");
  hbox_width = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_width), width_narrow);
  gtk_box_pack_start_defaults(GTK_BOX(hbox_width), width_wide);
  if (angle_width == REC_WIDE)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(width_wide) ,TRUE);
  }
  else
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(width_narrow) ,TRUE);
  }
  
  ions_label = gtk_label_new("Number of ions:");
  ions_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0, MIN_IONS, MAX_IONS, 10000, 100000, 0));
  ions = gtk_spin_button_new(ions_adj, 10000, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ions), (double) nions);
  
  ionspre_label = gtk_label_new("Number of ions in the presimulation:");
  ionspre_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0, MIN_IONS, MAX_IONS/10, 1000, 10000, 0));
  ionspre = gtk_spin_button_new(ionspre_adj, 1000, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ionspre), (double) nions_presim);
  
  seed_label = gtk_label_new("Seed number of the random number generator:");
  seed_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0, MIN_SEED, MAX_SEED, 10, 100, 0));
  seed = gtk_spin_button_new(seed_adj, 10, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(seed), (double) nseed);
  
  /* All parameters in the table */
  table = gtk_table_new(13, 3, FALSE);
  gtk_table_attach_defaults(GTK_TABLE(table), type_label, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), ion_label, 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), energy_label, 0, 1, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), recoil_label, 0, 1, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table), angle_label, 0, 1, 4, 5);
  gtk_table_attach_defaults(GTK_TABLE(table), beamsize_label, 0, 1, 5, 6);
  gtk_table_attach_defaults(GTK_TABLE(table), minangle_label, 0, 1, 6, 7);
  gtk_table_attach_defaults(GTK_TABLE(table), minenergy_label, 0, 1, 7, 8);
  gtk_table_attach_defaults(GTK_TABLE(table), averions_label, 0, 1, 8, 9);
  gtk_table_attach_defaults(GTK_TABLE(table), width_label, 0, 1, 9, 10);
  gtk_table_attach_defaults(GTK_TABLE(table), ions_label, 0, 1, 10, 11);
  gtk_table_attach_defaults(GTK_TABLE(table), ionspre_label, 0, 1, 11, 12);
  gtk_table_attach_defaults(GTK_TABLE(table), seed_label, 0, 1, 12, 13);
  gtk_table_attach_defaults(GTK_TABLE(table), hbox_type, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), ion, 1, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), energy, 1, 2, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), recoil, 1, 2, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table), angle, 1, 2, 4, 5);
  gtk_table_attach_defaults(GTK_TABLE(table), hbox_beamsize, 1, 2, 5, 6);
  gtk_table_attach_defaults(GTK_TABLE(table), minangle, 1, 2, 6, 7);
  gtk_table_attach_defaults(GTK_TABLE(table), minenergy, 1, 2, 7, 8);
  gtk_table_attach_defaults(GTK_TABLE(table), averions, 1, 2, 8, 9);
  gtk_table_attach_defaults(GTK_TABLE(table), hbox_width, 1, 2, 9, 10);
  gtk_table_attach_defaults(GTK_TABLE(table), ions, 1, 2, 10, 11);
  gtk_table_attach_defaults(GTK_TABLE(table), ionspre, 1, 2, 11, 12);
  gtk_table_attach_defaults(GTK_TABLE(table), seed, 1, 2, 12, 13);
  gtk_table_attach_defaults(GTK_TABLE(table), energy_unit, 2, 3, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), angle_unit, 2, 3, 4, 5);
  gtk_table_attach_defaults(GTK_TABLE(table), beamsize_unit, 2, 3, 5, 6);
  gtk_table_attach_defaults(GTK_TABLE(table), minangle_unit, 2, 3, 6, 7);
  gtk_table_attach_defaults(GTK_TABLE(table), minenergy_unit, 2, 3, 7, 8);
  
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 10);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), table);
  gtk_widget_show_all(dialog);

  result = gtk_dialog_run(GTK_DIALOG(dialog));

  /* Actions when clicking OK or CANCEL */
  if (result == GTK_RESPONSE_OK)
  {
    /* Save parameters (in the variables) */
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(type_erd)) ) { sim_type = SIM_ERD; }
    else if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(type_rbs)) ) { sim_type = SIM_RBS; }
    beam_ion = g_locale_from_utf8(gtk_entry_get_text(GTK_ENTRY(ion)), -1, NULL, NULL, NULL);
    beam_energy = gtk_spin_button_get_value(GTK_SPIN_BUTTON(energy));
    recoil_atom = g_locale_from_utf8(gtk_entry_get_text(GTK_ENTRY(recoil)), -1, NULL, NULL, NULL);
    target_angle = gtk_spin_button_get_value(GTK_SPIN_BUTTON(angle));
    beam_size_x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(x));
    beam_size_y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(y));
    min_angle = gtk_spin_button_get_value(GTK_SPIN_BUTTON(minangle));
    min_energy = gtk_spin_button_get_value(GTK_SPIN_BUTTON(minenergy));
    aver_nions = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(averions));
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(width_wide)) ) { angle_width = REC_WIDE; }
    else if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(width_narrow)) ) { angle_width = REC_NARROW; }
    nions = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ions));
    nions_presim = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ionspre));
    nseed = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(seed));
    
    saved = 0;
    
  }
  
  gtk_widget_destroy(dialog);
     
}
