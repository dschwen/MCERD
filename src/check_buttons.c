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


#include "protot.h"

/* functions for check/radio buttons */

/* Show/hide entrybox */
void show_check(GtkToggleButton *toggle, GtkWidget *widget)
{
  if (gtk_toggle_button_get_active(toggle))
    gtk_widget_set_sensitive(widget, TRUE);
  else
    gtk_widget_set_sensitive(widget, FALSE);
}

/* Controls that only one entrybox can be selected. */
void hide_check(GtkToggleButton *toggle, GtkWidget *widget)
{
  if (gtk_toggle_button_get_active(toggle))
    gtk_widget_set_sensitive(widget, FALSE);
  else
    gtk_widget_set_sensitive(widget, TRUE);
}

/* Show entrybox when clicked */
void show_radio(GtkToggleButton *toggle, GtkWidget *widget)
{
  gtk_widget_set_sensitive(widget, TRUE);
}

/* Hide entrybox when clicked */
void hide_radio(GtkToggleButton *toggle, GtkWidget *widget)
{
  gtk_widget_set_sensitive(widget, FALSE);
}
