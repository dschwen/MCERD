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

/* Create a dialog where espe parameters can be changed */
void espe_param(gpointer parent, guint callback_action, GtkWidget *widget)
{

  GtkWidget *dialog, *table;
  
  /* Virtual/real */
  GtkWidget *v_radio, *r_radio, *vr_hbox, *vsize, *vsize_label, *nvdepth_label, *nvdepth, *v_hbox;
  GtkAdjustment *vsize_adj, *nvdepth_adj;
  
  /* Dsize */
  GtkWidget *d_check, *dx, *dx_label, *dy, *dy_label, *d_hbox;
  GtkAdjustment *dx_adj, *dy_adj;
  
  /* Pn */
  GtkWidget *pn_check;
  
  /* Wide/narrow */
  GtkWidget *w_radio, *n_radio, *wn_hbox;
  
  /* Depth */
  GtkWidget *dw_check, *dmin, *dmin_label, *dmax, *dmax_label, *dw_hbox;
  GtkAdjustment *dmin_adj, *dmax_adj;
  
  /* Mass */
  GtkWidget *m_check, *mass, *mass_label, *m_hbox;
  GtkAdjustment *mass_adj;
  
  /* Timeres/eres */
  GtkWidget *t_radio, *e_radio, *te_hbox, *tres, *tres_label, *eres, *eres_label;
  GtkWidget *tres_hbox, *eres_hbox;
  GtkAdjustment *tres_adj, *eres_adj;
  
  /* Ch */
  GtkWidget *chwidth, *chwidth_label, *ch_hbox;
  GtkAdjustment *chwidth_adj;
  
  /* Toflen */
  GtkWidget *toflen, *toflen_label, *tl_hbox;
  GtkAdjustment *toflen_adj;
  
  /* Scale */
  GtkWidget *scale, *scale_label, *sc_hbox;
  GtkAdjustment *scale_adj;
  
  gint result;
  
  dialog = gtk_dialog_new_with_buttons("Parameters for the energy spectrum", parent, GTK_DIALOG_MODAL,
           GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
  
  
  /* Virtual/real */
  v_radio = gtk_radio_button_new_with_label(NULL, "Virtual");
  r_radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(v_radio), "Real      ");
  vr_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(vr_hbox), r_radio);
  gtk_box_pack_start_defaults(GTK_BOX(vr_hbox), v_radio);
  vsize_label = gtk_label_new("size of the virtual detector:");
  vsize_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.0, MIN_VDETSIZE, MAX_VDETSIZE, 1.0, 5.0, 0.0));
  vsize = gtk_spin_button_new(vsize_adj, 1.0, 1);
  nvdepth_label  = gtk_label_new("depth channels in virtual scaling:");
  nvdepth_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0, MINNDEPTH, MAXNDEPTH, 1, 5, 0));
  nvdepth = gtk_spin_button_new(nvdepth_adj, 1, 0);
  v_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(v_hbox), vsize_label);
  gtk_box_pack_start_defaults(GTK_BOX(v_hbox), vsize);
  gtk_box_pack_start_defaults(GTK_BOX(v_hbox), nvdepth_label);
  gtk_box_pack_start_defaults(GTK_BOX(v_hbox), nvdepth);
  gtk_widget_set_sensitive(v_hbox, FALSE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(r_radio), TRUE);
  if (espe.virtual == TRUE)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(v_radio), TRUE);
    gtk_widget_set_sensitive(v_hbox, TRUE);
  }
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(vsize), espe.vsize);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(nvdepth), (double)espe.vnum);
  g_signal_connect(G_OBJECT(v_radio), "toggled", G_CALLBACK(show_radio), (gpointer) v_hbox);
  g_signal_connect(G_OBJECT(r_radio), "toggled", G_CALLBACK(hide_radio), (gpointer) v_hbox);
  
  /* Dsize */
  d_check = gtk_check_button_new_with_label("Dsize");
  dx_label = gtk_label_new(" dx [mm]:");
  dx_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.0, MIN_DSIZE, MAX_DSIZE, 1.0, 5.0, 0.0));
  dx = gtk_spin_button_new(dx_adj, 1.0, 0);
  dy_label = gtk_label_new("dy [mm]:");
  dy_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0, MIN_DSIZE, MAX_DSIZE, 1, 5, 0));
  dy = gtk_spin_button_new(dy_adj, 1, 0);
  d_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(d_hbox), dx_label);
  gtk_box_pack_start_defaults(GTK_BOX(d_hbox), dx);
  gtk_box_pack_start_defaults(GTK_BOX(d_hbox), dy_label);
  gtk_box_pack_start_defaults(GTK_BOX(d_hbox), dy);
  gtk_widget_set_sensitive(d_hbox, FALSE);
  if (espe.dsize == TRUE)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d_check), TRUE);
    gtk_widget_set_sensitive(d_hbox, TRUE);
  }
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(dx), espe.dx);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(dy), espe.dy);
  g_signal_connect(G_OBJECT(d_check), "toggled", G_CALLBACK(show_check), (gpointer) d_hbox);
  
  /* Pn */
  pn_check = gtk_check_button_new_with_label("Pn");
  if (espe.pnvd == TRUE)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pn_check), TRUE);
    
  /* Wide/narrow */
  w_radio = gtk_radio_button_new_with_label(NULL, "Wide");
  n_radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(w_radio), "Narrow");
  wn_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(wn_hbox), n_radio);
  gtk_box_pack_start_defaults(GTK_BOX(wn_hbox), w_radio);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(n_radio), TRUE);
  if (espe.wide == TRUE)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_radio), TRUE);
    
  /* Depth */
  dw_check = gtk_check_button_new_with_label("Depth");
  dmin_label = gtk_label_new("  min [nm]:  ");
  dmin_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.0, MIN_DEPTH, MAX_DEPTH, 1.0, 5.0, 0.0));
  dmin = gtk_spin_button_new(dmin_adj, 1.0, 1);
  dmax_label = gtk_label_new("max [nm]:  ");
  dmax_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.0, MIN_DEPTH, MAX_DEPTH, 1.0, 5.0, 0.0));
  dmax = gtk_spin_button_new(dmax_adj, 1.0, 1);
  dw_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(dw_hbox), dmin_label);
  gtk_box_pack_start_defaults(GTK_BOX(dw_hbox), dmin);
  gtk_box_pack_start_defaults(GTK_BOX(dw_hbox), dmax_label);
  gtk_box_pack_start_defaults(GTK_BOX(dw_hbox), dmax);
  gtk_widget_set_sensitive(dw_hbox, FALSE);
  if (espe.dwindow == TRUE)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dw_check), TRUE);
    gtk_widget_set_sensitive(dw_hbox, TRUE);
  }
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(dmin), espe.dmin);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(dmax), espe.dmax);
  g_signal_connect(G_OBJECT(dw_check), "toggled", G_CALLBACK(show_check), (gpointer) dw_hbox);
  
  /* Mass */
  m_check = gtk_check_button_new_with_label("Mass of the recoil");
  mass_label = gtk_label_new("    mass [u]:   ");
  mass_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_M, MAX_M, 1.00, 5.00, 0.00));
  mass = gtk_spin_button_new(mass_adj, 1.00, 3);
  m_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(m_hbox), mass_label);
  gtk_box_pack_start_defaults(GTK_BOX(m_hbox), mass);
  gtk_widget_set_sensitive(m_hbox, FALSE);
  if (espe.mass == TRUE)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check), TRUE);
    gtk_widget_set_sensitive(m_hbox, TRUE);
  }
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(mass), espe.mass_value);
  g_signal_connect(G_OBJECT(m_check), "toggled", G_CALLBACK(show_check), (gpointer) m_hbox);
  
  /* Timeres/eres */
  t_radio = gtk_radio_button_new_with_label(NULL, "Time resolution     ");
  e_radio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(t_radio), "Energy resolution");
  te_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(te_hbox), t_radio);
  gtk_box_pack_start_defaults(GTK_BOX(te_hbox), e_radio);
  tres_label = gtk_label_new("resolution [ns]:");
  tres_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0, MIN_TRES, MAX_TRES, 0.1, 1.0, 0.0));
  tres = gtk_spin_button_new(tres_adj, 0.1, 4);
  eres_label  = gtk_label_new("resolution [MeV]:");
  eres_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.0, MIN_ERES, MAX_ERES, 0.01, 0.1, 0.0));
  eres = gtk_spin_button_new(eres_adj, 0.01, 4);
  tres_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(tres_hbox), tres_label);
  gtk_box_pack_start_defaults(GTK_BOX(tres_hbox), tres);
  eres_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(eres_hbox), eres_label);
  gtk_box_pack_start_defaults(GTK_BOX(eres_hbox), eres);
  gtk_widget_set_sensitive(eres_hbox, FALSE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(t_radio), TRUE);
  if (espe.timeres == FALSE)
  {
    gtk_widget_set_sensitive(eres_hbox, TRUE);
    gtk_widget_set_sensitive(tres_hbox, FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(e_radio), TRUE);
  }
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(tres), espe.tres);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(eres), espe.eres);
  g_signal_connect(G_OBJECT(t_radio), "toggled", G_CALLBACK(hide_check), (gpointer) eres_hbox);
  g_signal_connect(G_OBJECT(e_radio), "toggled", G_CALLBACK(hide_check), (gpointer) tres_hbox);
  
  /* Ch */
  chwidth_label = gtk_label_new("Channel width [MeV]:");
  chwidth_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_CHWIDTH, MAX_CHWIDTH, 0.010, 0.10, 0.00));
  chwidth = gtk_spin_button_new(chwidth_adj, 0.010, 3);
  ch_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(ch_hbox), chwidth_label);
  gtk_box_pack_start_defaults(GTK_BOX(ch_hbox), chwidth);
  gtk_widget_set_sensitive(chwidth, TRUE);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(chwidth), espe.chwidth);
  
  /* Toflen */
  toflen_label = gtk_label_new("TOF distance [m]:      ");
  toflen_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_TOFLEN, MAX_TOFLEN, 0.01, 0.1, 0.00));
  toflen = gtk_spin_button_new(toflen_adj, 0.01, 4);
  tl_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(tl_hbox), toflen_label);
  gtk_box_pack_start_defaults(GTK_BOX(tl_hbox), toflen);
  gtk_widget_set_sensitive(toflen, TRUE);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(toflen), espe.toflen);
  
  /* Scale */
  scale_label = gtk_label_new("Scale:                      ");
  scale_adj = GTK_ADJUSTMENT(gtk_adjustment_new(0.00, MIN_SCALEY, MAX_SCALEY, 0.000001, 1.00, 0.00));
  scale = gtk_spin_button_new(scale_adj, 0.000001, 6);
  sc_hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start_defaults(GTK_BOX(sc_hbox), scale_label);
  gtk_box_pack_start_defaults(GTK_BOX(sc_hbox), scale);
  gtk_widget_set_sensitive(scale, TRUE);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(scale), espe.scale);
  
  
  /* Table */
  table = gtk_table_new(9, 3, FALSE);
  gtk_table_attach_defaults(GTK_TABLE(table), vr_hbox, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), v_hbox, 1, 3, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), wn_hbox, 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), te_hbox, 0, 1, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), tres_hbox, 1, 2, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), eres_hbox, 2, 3, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), tl_hbox, 0, 1, 3, 4);
  gtk_table_attach_defaults(GTK_TABLE(table), ch_hbox, 0, 1, 4, 5);
  gtk_table_attach_defaults(GTK_TABLE(table), sc_hbox, 0, 1, 5, 6);
  gtk_table_attach_defaults(GTK_TABLE(table), d_check, 0, 1, 6, 7);
  gtk_table_attach_defaults(GTK_TABLE(table), d_hbox, 1, 3, 6, 7);
  gtk_table_attach_defaults(GTK_TABLE(table), dw_check, 0, 1, 7, 8);
  gtk_table_attach_defaults(GTK_TABLE(table), dw_hbox, 1, 3, 7, 8);
  gtk_table_attach_defaults(GTK_TABLE(table), m_check, 0, 1, 8, 9);
  gtk_table_attach_defaults(GTK_TABLE(table), m_hbox, 1, 2, 8, 9);
  /* gtk_table_attach_defaults(GTK_TABLE(table), pn_check, 0, 1, 9, 10); */
  
  gtk_table_set_row_spacings(GTK_TABLE(table), 20);
  gtk_table_set_col_spacings(GTK_TABLE(table), 10);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), table);
  gtk_widget_show_all(dialog);

  result = gtk_dialog_run(GTK_DIALOG(dialog));
  
  /* Actions when clicking OK or CANCEL */
  if (result == GTK_RESPONSE_OK)
  {
    /* Save parameters (in the variables) */
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(v_radio)) )
      espe.virtual = TRUE;
    else
      espe.virtual = FALSE;
    espe.vsize = gtk_spin_button_get_value(GTK_SPIN_BUTTON(vsize));
    espe.vnum = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(nvdepth));
    
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d_check)) )
      espe.dsize = TRUE;
    else
      espe.dsize = FALSE;
    espe.dx = gtk_spin_button_get_value(GTK_SPIN_BUTTON(dx));
    espe.dy = gtk_spin_button_get_value(GTK_SPIN_BUTTON(dy));
    
    /*
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pn_check)) )
      espe.pnvd = TRUE;
    else
      espe.pnvd = FALSE;
    */
    espe.pnvd = FALSE;
      
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w_radio)) )
      espe.wide = TRUE;
    else
      espe.wide = FALSE;
      
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dw_check)) )
      espe.dwindow = TRUE;
    else
      espe.dwindow = FALSE;
    espe.dmin = gtk_spin_button_get_value(GTK_SPIN_BUTTON(dmin));
    espe.dmax = gtk_spin_button_get_value(GTK_SPIN_BUTTON(dmax));
    
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_check)) )
      espe.mass = TRUE;
    else
      espe.mass = FALSE;
    espe.mass_value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(mass));
    
    if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(t_radio)) )
      espe.timeres = TRUE;
    else
      espe.timeres = FALSE;
    espe.tres = gtk_spin_button_get_value(GTK_SPIN_BUTTON(tres));
    espe.eres = gtk_spin_button_get_value(GTK_SPIN_BUTTON(eres));
    
    espe.chwidth = gtk_spin_button_get_value(GTK_SPIN_BUTTON(chwidth));
    
    espe.toflen = gtk_spin_button_get_value(GTK_SPIN_BUTTON(toflen));
    
    espe.scale = gtk_spin_button_get_value(GTK_SPIN_BUTTON(scale));
      
    saved = 0;
    
  }
  
  gtk_widget_destroy(dialog);
  
}
