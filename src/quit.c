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

/* Close the plot window */
void close_plot(gpointer window, guint callback_action, GtkWidget *widget)
{
  gtk_widget_destroy(GTK_WIDGET(window));
}

/* Quits the program with confirmation when quit button is clicked in the menu.
 * Check if user has saved and ask confirmation for quitting. */
void quit_menu(gpointer window, guint callback_action, GtkWidget *widget)
{
  if (!saved)
  {
    if (ask_save(GTK_WINDOW(window)) == FALSE)
    {
      gtk_main_quit();
    }
  }
  
  else if (ask_quit(GTK_WINDOW(window)) == FALSE)
  {
    gtk_main_quit();
  }
}

/* If you return FALSE in the "delete_event" signal handler,
 * GTK will emit the "destroy" signal. Returning TRUE means
 * you don't want the window to be destroyed.
 * Check if user has saved and ask confirmation for quitting. */
gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  if (!saved)
  {
    return ask_save(GTK_WINDOW(widget));
  }
  
  return ask_quit(GTK_WINDOW(widget));
}

/* Quits the program */
void destroy(GtkWidget *widget, gpointer data)
{
  gtk_main_quit();
}
