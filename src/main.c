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


/* Main function for GUI */


#include "protot.h"

/* Makes a main window, reads default values and controls gtk_main loop */
int main(int argc, char *argv[])
{
  /* Widgets */
  GtkWidget *window_main;
  GtkWidget *main_vbox;
  GtkWidget *menubar;

  gtk_init(&argc, &argv);
  
  /* Create a main window with specific size */
  window_main = create_window("MCERD", 600, 400);
  
  /* Make a vbox for widgets and add it to the main window*/
  main_vbox = create_vbox_main(window_main);

  /* Make a menu */
  menubar = get_menubar_menu(window_main);
  
  /* Make a progress bar */
  progress = gtk_progress_bar_new();
  
  /* Pack it all together in the main vbox */
  gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), progress, FALSE, TRUE, 0);
  
  /* Show all widgets. */
  gtk_widget_show_all(window_main);
  
  /* Initialize all parameters */
  init_values();
  
  /* Read all (default) parameters from the file */
  read_param(FDEFAULT);
  
  /* Close window */
  g_signal_connect(G_OBJECT(window_main), "delete_event", G_CALLBACK(delete_event), NULL);
    
  /* When calling gtk_widget_destroy() on the window,
   * or if we return FALSE in the "delete_event" callback. */
  g_signal_connect(G_OBJECT(window_main), "destroy", G_CALLBACK(destroy), NULL);
  
  /* Control ends here and waits for an event to occur */
  gtk_main();

  return 0;
}
