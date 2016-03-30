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


/* Menu arrays */


#include "protot.h"

/* Main menu, an array of GtkItemFactoryEntry structures that defines each menu item.
 * Format: {Menu item, shortcut key, call function, callback action, item type/icon} */
static GtkItemFactoryEntry menu_items[] = {
  { "/_File",                             NULL,         NULL,              0, "<Branch>" },
  { "/File/_Open",                        "<CTRL>O",    open_file,         1, "<StockItem>", GTK_STOCK_OPEN },
  { "/File/Read simulated data",          NULL,         open_plot,         1, "<Item>" },
  { "/File/_Save",                        "<CTRL>S",    save_file,         TYPE_SAVE, "<StockItem>", GTK_STOCK_SAVE },
  { "/File/Save As",                      NULL,         save_file,         TYPE_SAVEAS, "<Item>" },
  { "/File/separ",                        NULL,         NULL,              0, "<Separator>" },
  { "/File/_Quit",                        "<CTRL>Q",    quit_menu,         1, "<StockItem>", GTK_STOCK_QUIT },
  { "/_Detector",                         NULL,         NULL,              0, "<Branch>" },
  { "/Detector/Detector parameters",      NULL,         detector_param,    1, "<Item>" },
  { "/Detector/Detector foils",           NULL,         foils,             1, "<Item>" },
  { "/_Target",                           NULL,         NULL,              0, "<Branch>" },
  { "/Target/Target parameters",          NULL,         layers,            1, "<Item>" },
  { "/_Recoil distribution",              NULL,         NULL,              0, "<Branch>" },
  { "/Recoil distribution/Recoil material distribution",NULL,        recdist_param,     1, "<Item>" },
  { "/_Pre simulation",                   NULL,         NULL,              0, "<Branch>" },
  { "/Pre simulation/Pre simulation parameters",NULL,   presimu_param,     1, "<Item>" },
  { "/_Energy spectrum",                  NULL,         NULL,              0, "<Branch>" },
  { "/Energy spectrum/Energy spectrum parameters",NULL, espe_param,        1, "<Item>" },
  { "/_Simulation",                       NULL,         NULL,              0, "<Branch>" },
  { "/Simulation/Simulation parameters",  NULL,         simulation_param,  1, "<Item>" },
  { "/Simulation/separ",                  NULL,         NULL,              0, "<Separator>" },
  { "/Simulation/_Run",                   NULL,         run_simu,          0, "<Item>" },
  { "/_Help",                             NULL,         NULL,              0, "<LastBranch>" },
  { "/Help/About",                        NULL,         about_dialog,      1, "<Item>" },
};

/* For creating the menu */
static gint n_entries = sizeof(menu_items) / sizeof(menu_items[0]);

/* Returns a menubar widget made from the above menuitems */
GtkWidget *get_menubar_menu(GtkWidget *window)
{
  GtkItemFactory *item_factory;
  GtkAccelGroup *accel_group;

  /* Make an accelerator group (shortcut keys) */
  accel_group = gtk_accel_group_new();

  /* Make an ItemFactory (that makes a menubar) */
  item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<MainMenu>", accel_group);

  gtk_item_factory_create_items(item_factory, n_entries, menu_items, window);

  /* Attach the new accelerator group to the window. */
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  /* Return the created menu */
  return gtk_item_factory_get_widget(item_factory, "<MainMenu>");
}


/* Plot menu items.
 * Format: {Menu item, shortcut key, call function, callback action, item type/icon} */
static GtkItemFactoryEntry plot_menu_items[] = {
  { "/_File",                                NULL,         NULL,              0, "<Branch>" },
  { "/File/_Read experimental data",         "<CTRL>O",    open_expplot,      1, "<StockItem>", GTK_STOCK_OPEN },
  { "/File/Remove experimental data",        NULL,         remove_plot,       0, "<Item>" },
  { "/File/_Save",                           "<CTRL>S",    save_file,         TYPE_SAVE, "<StockItem>", GTK_STOCK_SAVE },
  { "/File/separ",                           NULL,         NULL,              0, "<Separator>" },
  { "/File/_Close",                          "<CTRL>Q",    close_plot,        1, "<StockItem>", GTK_STOCK_QUIT },
  { "/_Options",                             NULL,         NULL,              0, "<Branch>" },
  { "/Options/Energy spectrum parameters",   NULL,         espe_param,        1, "<Item>" },
  { "/Options/Number of distribution points",NULL,         ndistp_param,      1, "<Item>" },
  { "/Options/separ",                        NULL,         NULL,              0, "<Separator>" },
  { "/Options/Export energy spectrum",       NULL,         export_graph,      TYPE_PLOT, "<Item>" },
  { "/Options/Export energy spectrum and distribution graph",NULL,export_graph,TYPE_CANVAS, "<Item>" },
  { "/_Hand tool",                           NULL,         NULL,              0, "<Branch>" },
  { "/Hand tool/Select tool/Normal",         NULL,         hand_tool,         HAND_NORMAL, "<RadioItem>" },
  { "/Hand tool/Select tool/Selection",      NULL,         hand_tool,         HAND_SELECT, "/Hand tool/Select tool/Normal" },
  { "/Hand tool/Select tool/Zoom",           NULL,         hand_tool,         HAND_ZOOM, "/Hand tool/Select tool/Normal" },
  { "/Hand tool/separ",                      NULL,         NULL,              0, "<Separator>" },
  { "/Hand tool/Unzoom",                     NULL,         unzoom,            0, "<StockItem>", GTK_STOCK_ZOOM_OUT },
  { "/_Refresh",                             NULL,         NULL,              0, "<Branch>" },
  { "/Refresh/Refresh spectrum",             "<CTRL>R",    refresh_espe,      0, "<Item>" },
};

/* For creating the menu */
static gint nplot_entries = sizeof(plot_menu_items) / sizeof(plot_menu_items[0]);

/* Returns a menubar widget made from the above menuitems */
GtkWidget *get_plot_menu(GtkWidget *window)
{
  GtkItemFactory *item_factory;
  GtkAccelGroup *accel_group;

  /* Make an accelerator group (shortcut keys) */
  accel_group = gtk_accel_group_new();

  /* Make an ItemFactory (that makes a menubar) */
  item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<PlotMenu>", accel_group);

  gtk_item_factory_create_items(item_factory, nplot_entries, plot_menu_items, window);

  /* Attach the new accelerator group to the window. */
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  /* Return the created menu */
  return gtk_item_factory_get_widget(item_factory, "<PlotMenu>");
}
