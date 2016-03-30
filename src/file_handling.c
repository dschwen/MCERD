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


/* File includes dialogs for opening/saving files */


#include "general.h"
#include "protot.h"

int changed; /* For controlling when the filename changes */

/* A dialog for opening a file. */
void open_file(gpointer parent, guint callback_action, GtkWidget *widget)
{
  GtkWidget *chooser, *dialog;
  GtkFileFilter *filter;
  gint result;
  char *file_type;
  char *file_temp;
  changed = 0;
  file_temp = file_path; /* If user changes the file but then cancels */
  
  file_type = (char *)calloc(strlen(FEND) + 2, sizeof(char)); /* *.FEND */
  strcat(file_type, "*");
  strcat(file_type, FEND);
  
  chooser = gtk_file_chooser_button_new("Open a file", GTK_FILE_CHOOSER_ACTION_OPEN);
  g_signal_connect(G_OBJECT(chooser), "selection_changed", G_CALLBACK(file_changed), NULL);
  filter = gtk_file_filter_new();
  
  /* Add a filter so only specific file types can be opened (*.FEND) */
  gtk_file_filter_set_name(filter, "MCERD files");
  gtk_file_filter_add_pattern(filter, file_type);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
  free(file_type);
  
  dialog = gtk_dialog_new_with_buttons("Open file ...", parent, GTK_DIALOG_MODAL,
           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), chooser);
  gtk_widget_show_all(dialog);
  result = gtk_dialog_run(GTK_DIALOG(dialog));
  
  if (result == GTK_RESPONSE_ACCEPT)
  {
    if (changed)
      read_param(file_path);
  }
  else
  {
    file_path = file_temp;
  }
  
  gtk_widget_destroy(dialog);
     
}


/* Open a file for reading data to the plot (new window) */
void open_plot(gpointer parent, guint callback_action, GtkWidget *widget)
{
  GtkWidget *chooser, *dialog;
  GtkFileFilter *filter;
  gint result;
  changed = 0;
  plot_path = "";
  char *file_type;
  char *file_temp;
  
  file_type = (char *)calloc(strlen(FSPE) + 2, sizeof(char)); /* *.FSPE */
  strcat(file_type, "*");
  strcat(file_type, FSPE);
  
  chooser = gtk_file_chooser_button_new("Open a spectrum", GTK_FILE_CHOOSER_ACTION_OPEN);
  g_signal_connect(G_OBJECT(chooser), "selection_changed", G_CALLBACK(plot_file_changed), NULL);
  filter = gtk_file_filter_new();
  
  /* Add a filter so only specific file types can be opened (*.FSPE) */
  gtk_file_filter_set_name(filter, "*.spe files");
  gtk_file_filter_add_pattern(filter, file_type);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
  free(file_type);
  
  dialog = gtk_dialog_new_with_buttons("Open spectrum ...", parent, GTK_DIALOG_MODAL,
           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), chooser);
  gtk_widget_show_all(dialog);
  result = gtk_dialog_run(GTK_DIALOG(dialog));
  
  if (result == GTK_RESPONSE_ACCEPT)
  {       
    if (plot_path == NULL || strcmp(plot_path, "") == 0)
      error_dialog("Empty filename");
    else if (changed)
    {

      file_temp = (char *)calloc(strlen(plot_path) - strlen(FSPE) + 1, sizeof(char));
      strncpy(file_temp, plot_path, strlen(plot_path) - strlen(FSPE) - 4); /* -4 = ".nseed", for example ".101" */
      read_param(file_temp); /* Reading also the parameters when opening simulated plot */
      free(file_temp);
      plot_espe(TYPE_OPEN, plot_path);
    }
  }
  
  gtk_widget_destroy(dialog);
  
}

/* Open a file for reading data to the experimental plot (existing window) */
void open_expplot(gpointer parent, guint callback_action, GtkWidget *widget)
{
  GtkWidget *chooser, *dialog;
  gint result;
  changed = 0;
  plot_path = "";
  
  chooser = gtk_file_chooser_button_new("Read data", GTK_FILE_CHOOSER_ACTION_OPEN);
  g_signal_connect(G_OBJECT(chooser), "selection_changed", G_CALLBACK(plot_file_changed), NULL);
  
  dialog = gtk_dialog_new_with_buttons("Read data ...", parent, GTK_DIALOG_MODAL,
           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), chooser);
  gtk_widget_show_all(dialog);
  result = gtk_dialog_run(GTK_DIALOG(dialog));
  
  if (result == GTK_RESPONSE_ACCEPT)
  {       
    if (plot_path == NULL || strcmp(plot_path, "") == 0)
      error_dialog("Empty filename");
    else if (changed)
    {
      xscale_param(parent, PLOT_EXP, NULL); /* Asks also E/ch for the file */
    }
  }
  
  gtk_widget_destroy(dialog);
  
}

/* A dialog for saving a file. */
void save_file(gpointer parent, guint callback_action, GtkWidget *widget)
{
  GtkWidget *dialog, *chooser;
  GtkFileFilter *filter;
  gint result;
  char *file_type;
  char *file_temp;
  file_temp = file_path;
  
  file_type = (char *)calloc(strlen(FEND) + 2, sizeof(char));
  strcat(file_type, "*");
  strcat(file_type, FEND);
  
  dialog = gtk_dialog_new_with_buttons("Save file ...", parent, GTK_DIALOG_MODAL,
           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_APPLY, NULL);
  gtk_widget_set_size_request(dialog, 600, 600);

  chooser = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_SAVE);
  
  /* When saving an existing file */
  if (file_path != NULL && callback_action == TYPE_SAVE)
  {
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(chooser),
                                  g_locale_from_utf8(file_path, -1, NULL, NULL, NULL));
  }
  
  filter = gtk_file_filter_new();
  gtk_file_filter_set_name(filter, "MCERD files");
  gtk_file_filter_add_pattern(filter, file_type);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);
  free(file_type);

  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), chooser);
  gtk_widget_show_all(dialog);

  result = gtk_dialog_run(GTK_DIALOG(dialog));
  if (result == GTK_RESPONSE_APPLY)
  {
    file_path = g_locale_to_utf8( gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser)),
                                  -1, NULL, NULL, NULL );
    if (file_path != NULL && strcmp(file_path, ""))
    {
      write_files(file_path);
      if(error)
        file_path = file_temp;
    }
    else
    {
      error_dialog("Save failed. Empty filename.");
      file_path = file_temp;
    }
  }
  
  gtk_widget_destroy(dialog);
}

/* Actions when user changes the file (for opening files) */
void file_changed(GtkFileChooser *chooser)
{
  file_path = g_locale_to_utf8( gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser)),
                               -1, NULL, NULL, NULL );
  changed = 1;
}

/* Actions when user changes the file (for opening plots) */
void plot_file_changed(GtkFileChooser *chooser)
{
  plot_path = g_locale_to_utf8( gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser)),
                               -1, NULL, NULL, NULL );
  changed = 1;
}
