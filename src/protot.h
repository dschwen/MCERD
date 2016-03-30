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



#include "param.h"

/* Prototypes of functions for GUI */

/* menu.c */
GtkWidget *get_menubar_menu(GtkWidget *window);
GtkWidget *get_plot_menu(GtkWidget *window);

/* dialogs.c */
gboolean ask_quit(GtkWindow *parent);
gboolean ask_save(GtkWindow *parent);
void about_dialog(gpointer parent, guint callback_action, GtkWidget *widget);
void error_dialog(char *message);
void error_dialog_open(char *fname);
void output_dialog(char *fname);
void export_dialog(char *fname);

/* simulation_param.c */
void simulation_param(gpointer parent, guint callback_action, GtkWidget *widget);

/* detector_param.c */
void detector_param(gpointer parent, guint callback_action, GtkWidget *widget);

/* target_param.c */
void layers(gpointer parent, guint callback_action, GtkWidget *widget);
void add_layer(GtkWidget *notebook, int layer_num, GtkWidget *thickness[], GtkWidget *stop_ion[], GtkWidget *stop_rec[], GtkWidget *density[], GtkWidget *element[][MAX_LAYERS], GtkWidget *concentration[][MAX_LAYERS], GtkWidget *mass[][MAX_LAYERS]);
void remove_layer(GtkWidget *notebook, int layer_num);
void add_elements(GtkWidget *table, int layer_num, GtkWidget *element[][MAX_LAYERS], GtkWidget *concentration[][MAX_LAYERS], GtkWidget *mass[][MAX_LAYERS]);
void save_target_values(int layer_count, GtkWidget *thickness[], GtkWidget *stop_ion[], GtkWidget *stop_rec[], GtkWidget *density[], GtkWidget *element[][MAX_LAYERS], GtkWidget *concentration[][MAX_LAYERS], GtkWidget *mass[][MAX_LAYERS]);

/* foil_param.c */
void foils(gpointer parent, guint callback_action, GtkWidget *widget);
void add_foil(GtkWidget *notebook, int foil_num, GtkWidget *type[][MAX_FOILS], GtkWidget *size[][MAX_FOILS], GtkWidget *distance[], GtkWidget *stop_ion[], GtkWidget *stop_rec[], GtkWidget *element[][MAX_FOILS], GtkWidget *concentration[][MAX_FOILS], GtkWidget *mass[][MAX_FOILS], GtkWidget *thickness[], GtkWidget *density[]);
void remove_foil(GtkWidget *notebook, int foil_num);
void add_felements(GtkWidget *table, int foil_num, GtkWidget *element[][MAX_FOILS], GtkWidget *concentration[][MAX_FOILS], GtkWidget *mass[][MAX_FOILS]);
void save_foil_values(int foil_count, GtkWidget *type[][MAX_FOILS], GtkWidget *size[][MAX_FOILS], GtkWidget *distance[], GtkWidget *stop_ion[], GtkWidget *stop_rec[], GtkWidget *element[][MAX_FOILS], GtkWidget *concentration[][MAX_FOILS], GtkWidget *mass[][MAX_FOILS], GtkWidget *thickness[], GtkWidget *density[]);

/* write.c */
void write_files(char *fname);
void write_target(char *fname);
void write_detector(char *fname);
void write_foils(char *fname);
void write_recdist(char *fname);
void write_presimu(char *fname);
void write_parameters(char *fname);
void write_espe(char *fname);

/* widgets.c */
GtkWidget *create_window(const char *title, int width, int height);
GtkWidget *create_vbox_main(GtkWidget *window);

/* quit.c */
void close_plot(gpointer window, guint callback_action, GtkWidget *widget);
void quit_menu(gpointer window, guint callback_action, GtkWidget *widget);
gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
void destroy(GtkWidget *widget, gpointer data);

/* read_param.c */
void read_param(char *file_name);
void read_espe_params_file(char *fname);
void read_rdist_file(char *fname);
void read_presimu_file(char *fname);

/* file_handling.c */
void open_file(gpointer parent, guint callback_action, GtkWidget *widget);
void open_plot(gpointer parent, guint callback_action, GtkWidget *widget);
void open_expplot(gpointer parent, guint callback_action, GtkWidget *widget);
void save_file(gpointer parent, guint callback_action, GtkWidget *widget);
void file_changed(GtkFileChooser *chooser);
void plot_file_changed(GtkFileChooser *chooser);

/* run_simu.c */
void run_simu();

/* format_values.c */
void init_values();
void format_foils(int nfoils);
void format_layers(int nlayers);

/* check_param.c */
void check_param();
void check_espe_param();

/* recdist_param.c */
void recdist_param(gpointer parent, guint callback_action, GtkWidget *widget);

/* presimu_param.c */
void presimu_param(gpointer parent, guint callback_action, GtkWidget *widget);

/* espe_param.c */
void espe_param(gpointer parent, guint callback_action, GtkWidget *widget);

/* get_espe.c */
void get_espe(double *dist_x, double *dist_y);

/* plot_espe.c */
void plot_espe(int type, char *fname);
void quit(GtkWidget *widget, gpointer data);
void refresh_espe();
void read_data(char *fname, int plot_type, double x_scale);
void add_distplot();
void add_distpoints();
void add_maindistpoints();
void read_distpoints();
void scale_distplot();
void ndistp_param(gpointer parent, guint callback_action, GtkWidget *widget);
void add_simplot();
void add_expplot();
void update_plot();
void remove_plot();
void export_graph(gpointer parent, guint callback_action, GtkWidget *widget);
void hand_tool(gpointer callback_data, guint callback_action, GtkWidget *widget);
void select_region(GtkWidget *canvas, gdouble xmin, gdouble ymin, gdouble xmax, gdouble ymax, gpointer data);
void zoom_plot(double *px, double *py, double *px2, double *py2, double xmin, double xmax, double *miny, double *maxy, int type);
void unzoom();
void lin_scale();
void log_scale();
void change_xrange(double xmin, double xmax);

/* check_buttons.c */
void show_check(GtkToggleButton *toggle, GtkWidget *widget);
void hide_check(GtkToggleButton *toggle, GtkWidget *widget);
void show_radio(GtkToggleButton *toggle, GtkWidget *widget);
void hide_radio(GtkToggleButton *toggle, GtkWidget *widget);

/* scale_param.c */
void xscale_param(gpointer parent, guint callback_action, GtkWidget *widget);
void xrange_param(gpointer parent, guint callback_action, GtkWidget *widget);
