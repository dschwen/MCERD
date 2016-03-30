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

/* Some misc dialogs */

/* Create a new message dialog for confirming to quit the main program.
 * Returns FALSE when user wants to quit or TRUE otherwise. */
gboolean ask_quit(GtkWindow *parent)
{
  GtkWidget *dialog;
  /* For testing what the user clicks */
  gint result;
  
  dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
           GTK_BUTTONS_YES_NO, "Are you sure to quit?");
  gtk_window_set_title(GTK_WINDOW(dialog), "Quit?");
  
  result = gtk_dialog_run(GTK_DIALOG(dialog));
  
  /* Actions when clicking YES or NO */
  if (result == GTK_RESPONSE_YES)
  {
    gtk_widget_destroy(dialog);
    return FALSE;
  }

  gtk_widget_destroy(dialog);
  return TRUE;
  
}

/* Create a dialog for fonfirming to quit the main program if there is an unsaved file.
 * Returns FALSE if user wants to save or leave without saving, TRUE if user wants to cancel. */
gboolean ask_save(GtkWindow *parent)
{
  GtkWidget *dialog;
  /* For testing what the user clicks */
  gint result;
  
  dialog = gtk_dialog_new_with_buttons("Save changes before quitting?", parent,
           GTK_DIALOG_MODAL, GTK_STOCK_YES, GTK_RESPONSE_YES, GTK_STOCK_NO,
           GTK_RESPONSE_NO, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
  
  result = gtk_dialog_run(GTK_DIALOG(dialog));
  
  /* Actions when clicking YES, NO or CANCEL */
  if (result == GTK_RESPONSE_YES)
  {
    save_file(parent, TYPE_SAVE, NULL);
    gtk_widget_destroy(dialog);
    return FALSE;
  }
  
  if (result == GTK_RESPONSE_NO)
  {
    gtk_widget_destroy(dialog);
    return FALSE;
  }

  gtk_widget_destroy(dialog);
  return TRUE;
  
}


/* Create an about dialog which gives some information of the program */
void about_dialog(gpointer parent, guint callback_action, GtkWidget *widget)
{
  GtkWidget *dialog;
  
  const gchar *authors[] = {
    "MCERD: Kai Arstila", 
    "GUI: Jouni Heiskanen",
    NULL
  };
  
  dialog = gtk_about_dialog_new();
  gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
  
  /* Set all the needed information here. */
  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "MCERD");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "17122010");
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "(C) 2010 Kai Arstila, Jouni Heiskanen");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "See manual.pdf");
  gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog), "This program is distributed under the terms of the GNU General Public License.\nSee file: COPYING and <http://www.gnu.org/licenses/> for more information.");
  /* gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "http://www.com"); */
  /* gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog), "www.com"); */
  gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);
  /* gtk_about_dialog_set_documenters(GTK_ABOUT_DIALOG(dialog), documenters); */
  /* gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(dialog), "Translator #1\nTranslator #2"); */

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

/* Create an error dialog with error message */
void error_dialog(char *message)
{
  GtkWidget *dialog;
  
  dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING,
           GTK_BUTTONS_OK, message);
  gtk_window_set_title(GTK_WINDOW(dialog), "Error");
  
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  
  error = 1;
  
}

/* Create an error dialog when opening the file fails */
void error_dialog_open(char *fname)
{
  GtkWidget *dialog;
  char *fcomb;
  
  fcomb = (char *)calloc( strlen(ERROR_OPEN) + strlen(fname) + 1, sizeof(char) );
  strcat(fcomb,ERROR_OPEN);
  strcat(fcomb,fname);
  
  dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING,
           GTK_BUTTONS_OK, fcomb);
  gtk_window_set_title(GTK_WINDOW(dialog), "Error");
  
  free(fcomb);
  
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  
  error = 1;
  
}

/* Dialog when simulation is finished and what output files were generated */
void output_dialog(char *fname)
{
  GtkWidget *dialog;
  char *fcomb;
  char *message;
  
  message = "Simulation ready. Output files: ";
  
  fcomb = (char *)calloc( strlen(message) + strlen(fname) + strlen(".*") + 1, sizeof(char) );
  strcat(fcomb,message);
  strcat(fcomb,fname);
  strcat(fcomb,".*");
  
  dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
           GTK_BUTTONS_OK, fcomb);
  gtk_window_set_title(GTK_WINDOW(dialog), "Simulation ready");
  
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  
  free(fcomb);
  
}


/* Dialog for output files when exporting graph */
void export_dialog(char *fname)
{
  GtkWidget *dialog;
  char *fcomb;
  char *message;
  
  message = "Graph exported. Filename: ";
  
  fcomb = (char *)calloc( strlen(message) + strlen(fname) + 1, sizeof(char) );
  strcat(fcomb,message);
  strcat(fcomb,fname);
  
  dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
           GTK_BUTTONS_OK, fcomb);
  gtk_window_set_title(GTK_WINDOW(dialog), "Graph exported");
  
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  
  free(fcomb);
  
}
