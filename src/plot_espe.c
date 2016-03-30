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


/* This file includes all functions needed for energy spectrum/distribution window */


#include "gtkextra.h"
#include "general.h"
#include "prototypes.h"
#include "protot.h"


/* Energy spectrum graph contains 2 datasets (simulation and experimental) */
/* Distribution graph contains 1 dataset (concentration distribution) */

/* Plot paths */
char *sim_plot;                                  /* path for simulation data file */
char *plot_path;                                 /* path for experimental or opened data file */

/* Plot widgets */
GtkWidget *active_plot1, *active_plot2;           /* 1 for energy spectrum graph, 2 for distribution graph */
GtkPlotData *dataset[3];                          /* 1 for simulation data, 2 for experimental data, 3 for distribution data */
GtkWidget *canvas;                                /* canvas for all graphs */
GtkPlotCanvasChild *child1, *child2;              /* 1 for energy spectrum graph, 2 for distribution graph */

/* Scaling and zooming */
double default_scale;                            /* default scale used for data (energy spectrum) */
double exp_scale;                                /* scale used for readed (experimental) data */
int zoom_active;                                 /* 1 zoomed, 0 not zoomed */

/* Data tables */
/* TODO: Memory allocation for these for reducing memory usage */
double px[MAX_POINTS], py[MAX_POINTS];    /* simulation */
double px2[MAX_POINTS], py2[MAX_POINTS];  /* experimental */
double px3[MAX_DISTP], py3[MAX_DISTP];    /* distribution */

/* Misc */
int bool_exp_plot;                               /* is experimental plot active */
double d_max;                                    /* max thickness of target for distribution graph */
double d_min;                                    /* min thickness of target for distribution graph */
int n_distpoints;                                /* number of distribution points */

/* Contains variables for simulation and experimental data (scaling and zooming) */
typedef struct {
        
  double min_x, min_y, max_x, max_y;               /* for scaling energy spectrum graph */
  int ndata;                                       /* number of data points */
  int ndata_zoom;                                  /* number of data points in zoomed area */
  double temp_px[MAX_POINTS], temp_py[MAX_POINTS]; /* tables for zoomed data */
  
} Scale;

Scale simu; /* simulation data */
Scale expr; /* experimental data */



/* Exit the plot window. Free some memory here! */
void quit(GtkWidget *widget, gpointer data)
{
  close_plot(widget, 1, NULL);
  free(sim_plot);
  free(file_path);
  sim_plot = "";
  file_path = "";
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), 0.0);
  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), NULL);
}

/* Run get_espe with some parameters -> energy spectrum file.
 * Read data from energy spectrum file to tables and refresh spectrum.
 * Uses distribution points from graph. */
void refresh_espe()
{

  double *xp, *yp;
  int num;
  int i, j;
  double tmp, tmp2; /* For sorting arrays */

  error = 0;

  /* Read distribution points from the graph */
  xp = gtk_plot_data_get_x(dataset[2], &num);
  yp = gtk_plot_data_get_y(dataset[2], &num);
  
  /* Checking that distribution points are in the correct area (0 < d_min < x < d_max and 0 < y < 1) */
  for (i = 0; i < num; i++)
  {
    if (xp[i] < d_min)
      xp[i] = d_min;
    if (d_max < xp[i])
      xp[i] = d_max;
    if (yp[i] < 0)
      yp[i] = 0;
    if (1 < yp[i])
      yp[i] = MAX_DISTR;
  }
  
  /* Sorting arrays so that always xp[i] <= xp[i+1].
   * Exchange sorting algorithm is used here */
  for (i = 0; i < num - 1; i++)
  {
    for (j = i + 1; j < num; j++)
    {
      if (xp[i] > xp[j])
      {
        tmp = xp[i];
        tmp2 = yp[i];
        xp[i] = xp[j];
        yp[i] = yp[j];
        xp[j] = tmp;
        yp[j] = tmp2;
      }
    }
  }

  /* Use distribution points when running get_espe */
  espe.ddist = TRUE;
  
  check_espe_param();
  if (error)
  {
    espe.ddist = FALSE;
    return;
  }
  get_espe(xp, yp);

  read_data(sim_plot, PLOT_SIM, DEF_SCALE/default_scale);
  
  if (zoom_active)
    unzoom();
  else
    update_plot();

  espe.ddist = FALSE;
     
}

/* Read data from the file and save scaled data points (counts v channel) to arrays */
void read_data(char *fname, int plot_type, double x_scale)
{
     
  FILE *fp;
  char line[NLINE];
  int i, j, n, err;
  double x, y;
  
  if (plot_type == PLOT_EXP && bool_exp_plot == 0)
    add_expplot();
  
  fp = fopen(fname, "r");
   
  if (fp == NULL)
  {
    error_dialog_open(fname);
    return;
  }
  
  i = 0;
  err = 0;
  
  /* Give initial values for scaling */
  switch(plot_type){
    case PLOT_SIM:
      simu.min_x = BIG_VALUE;
      simu.min_y = 0; /* using miny <= 0 */
      simu.max_x = simu.max_y = -BIG_VALUE;
      break;
    case PLOT_EXP:
      expr.min_x = BIG_VALUE;
      expr.min_y = 0; /* using miny <= 0 */
      expr.max_x = expr.max_y = -BIG_VALUE;
      break;
  }
  
  while (i < MAX_POINTS && (fgets(line, NLINE, fp) != NULL))
  {
    j = sscanf(line, "%lf %lf", &x, &y);
    if (j == 2)
    {
          
      switch(plot_type){
        case PLOT_SIM:
          px[i] = x_scale*x;
          py[i] = y;
      
          /* Checking min and max values */
          if (px[i] < simu.min_x)
            simu.min_x = px[i];
          if (simu.max_x < px[i])
            simu.max_x = px[i];
          if (py[i] < simu.min_y)
            simu.min_y = py[i];
          if (simu.max_y < py[i])
            simu.max_y = py[i];
        
          break;
          
        case PLOT_EXP:
          px2[i] = x_scale*x;
          py2[i] = y;
      
          /* Checking min and max values */
          if (px2[i] < expr.min_x)
            expr.min_x = px2[i];
          if (expr.max_x < px2[i])
            expr.max_x = px2[i];
          if (py2[i] < expr.min_y)
            expr.min_y = py2[i];
          if (expr.max_y < py2[i])
            expr.max_y = py2[i];
        
          break;
      }
      i++;
    }
    else if (j != 2)
      err = 1;
  }
  
  n = i; /* Number of data points */
  
  fclose(fp);
  
  if (err == 1)
  {
    error_dialog("Problems when reading data points");
    remove_plot();
    return;
  }
  if (n == MAX_POINTS)
    error_dialog("Maximum number of data points reached"); /* but still continuing */
  
  /* Adding data */
  switch(plot_type){
    case PLOT_SIM:
      simu.ndata = n;
      gtk_plot_data_set_points(dataset[0], px, py, 0, 0, simu.ndata);
      break;
    case PLOT_EXP:
      expr.ndata = n;
      gtk_plot_data_set_points(dataset[1], px2, py2, 0, 0, expr.ndata);
      if (zoom_active)
        unzoom();
      else
        update_plot();
      break;
  }
     
}

/* Add distribution plot */
void add_distplot()
{
  
  GdkColor color;

  /* Axis options */
  gtk_plot_axis_set_visible(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_TOP), TRUE);
  gtk_plot_axis_set_visible(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_BOTTOM), TRUE);
  gtk_plot_axis_set_visible(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_LEFT), TRUE);
  gtk_plot_axis_set_visible(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_RIGHT), TRUE);
  gtk_plot_axis_hide_title(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_TOP));
  gtk_plot_axis_hide_title(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_RIGHT));
  gtk_plot_axis_set_title(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_LEFT), "Concentration distribution");
  gtk_plot_axis_set_title(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_BOTTOM), "Depth [nm]");
  gtk_plot_axis_move_title(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_BOTTOM), 0, 0.4, 0.39);
  gtk_plot_grids_set_visible(GTK_PLOT(active_plot2), TRUE, FALSE, TRUE, FALSE);
  
  dataset[2] = GTK_PLOT_DATA(gtk_plot_data_new());
  gtk_plot_add_data(GTK_PLOT(active_plot2), dataset[2]);
  gtk_widget_show(GTK_WIDGET(dataset[2]));
      
  /* Data options */
  gdk_color_parse("black", &color);
  gdk_color_alloc(gdk_colormap_get_system(), &color); 
  gtk_plot_data_set_symbol(dataset[2], GTK_PLOT_SYMBOL_DIAMOND,
                           GTK_PLOT_SYMBOL_FILLED, 5, 2, &color, &color);
  gtk_plot_data_set_line_attributes(dataset[2], GTK_PLOT_LINE_NONE, 0, 0, 1, &color);
  gtk_plot_hide_legends(GTK_PLOT(active_plot2));
  /* gtk_plot_data_set_legend(dataset[2], "Distribution points"); */
  gtk_plot_legends_move(GTK_PLOT(active_plot2), .8, -0.2);
  
  gtk_plot_major_vgrid_set_attributes(GTK_PLOT(active_plot2), GTK_PLOT_LINE_DASHED, 1, &color);
  gtk_plot_major_hgrid_set_attributes(GTK_PLOT(active_plot2), GTK_PLOT_LINE_DASHED, 1, &color);
  
}

/* Add distribution points to the graph when changing the number of points (using constant spacing) */
void add_distpoints()
{

  double gap;  /* gap between the points */
  int i;
  
  gap = (d_max - d_min)/(n_distpoints - 1);
  
  /* Give values for points */
  for(i = 0; i < n_distpoints; i++)
  {
    px3[i] = d_min + i*gap;
    py3[i] = 1.0;
  }

  gtk_plot_data_set_points(dataset[2], px3, py3, 0, 0, n_distpoints);
  
  scale_distplot();

}

/* Add distribution points to the graph (values that are used in main simulation) */
void add_maindistpoints()
{
  int i;
  
  n_distpoints = MAX_RDIST;
  
  /* Give values for points */
  for(i = 0; i < n_distpoints; i++)
  {
    px3[i] = rdist_x[i];
    py3[i] = rdist_y[i];
  }

  gtk_plot_data_set_points(dataset[2], px3, py3, 0, 0, n_distpoints);
  
  scale_distplot();

}

/* Read distribution points from file. */
void read_distpoints()
{

  FILE *fp;
  char line[NLINE];
  char *fcomb;
  int i, j, err;
  double x, y;


  fcomb = (char *)calloc(strlen(file_path) + strlen(FDIST) + 1, sizeof(char));
  strcat(fcomb, file_path);
  strcat(fcomb, FDIST);
  
  fp = fopen(fcomb, "r");
   
  if (fp == NULL)
  {
    error_dialog("No distribution data (*.dist). Using defaults.");
    free(fcomb);
    add_maindistpoints(); /* Using defaults */
    return;
  }
  
  i = 0;
  err = 0;

  while (i < MAX_DISTP && (fgets(line, NLINE, fp) != NULL))
  {
    j = sscanf(line, "%lf %lf", &x, &y);
    if (j == 2)
    {
      px3[i] = x;
      py3[i] = y;
    }    
    else if (j != 2)
      err = 1;

    i++;
  }
  
  if (err == 1)
  {
    error_dialog("Problems when reading data points");
    add_maindistpoints(); /* Using defaults */
    fclose(fp);
    free(fcomb);
    return;
  }

  n_distpoints = i;
  
  if (n_distpoints == MAX_DISTP)
    error_dialog("Maximum number of data points reached");
  
  fclose(fp);
  free(fcomb);

  gtk_plot_data_set_points(dataset[2], px3, py3, 0, 0, n_distpoints);
  
  scale_distplot();

}

/* Scaling options for distribution graph. */
void scale_distplot()
{
     
  gint p;
  double length;
  
  length = d_max - d_min;

  /* Scaling */
  gtk_plot_axis_ticks_autoscale(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_BOTTOM), d_min, 1.1*length, &p);
  gtk_plot_axis_ticks_autoscale(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_TOP), d_min, 1.1*length, &p);
  gtk_plot_axis_set_ticks_limits(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_LEFT), 0, 1.0);
  gtk_plot_axis_set_ticks_limits(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_RIGHT), 0, 1.0);
  gtk_plot_axis_set_ticks_limits(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_BOTTOM), d_min, 1.1*length);
  gtk_plot_axis_set_ticks_limits(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_TOP), d_min, 1.1*length);
  
  gtk_plot_axis_set_ticks(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_LEFT), 0.1, 1);
  gtk_plot_axis_set_ticks(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_RIGHT), 0.1, 1);
  gtk_plot_set_xrange(GTK_PLOT(active_plot2), d_min - 0.05*length, d_max + 0.05*length);
  gtk_plot_set_yrange(GTK_PLOT(active_plot2), -0.1, 1.1);
  
  gtk_plot_axis_ticks_recalc(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_BOTTOM));
  gtk_plot_axis_ticks_recalc(gtk_plot_get_axis(GTK_PLOT(active_plot2), GTK_PLOT_AXIS_TOP));

  gtk_plot_canvas_paint(GTK_PLOT_CANVAS(canvas));
  gtk_widget_queue_draw(GTK_WIDGET(canvas));
     
}

/* Create a dialog where number of distribution points can be changed */
void ndistp_param(gpointer parent, guint callback_action, GtkWidget *widget)
{
     
  GtkWidget *dialog, *table;
  GtkWidget *label;
  GtkWidget *ndistp;
  GtkAdjustment *ndistp_adj;
  int num;
  gint result;
  
  dialog = gtk_dialog_new_with_buttons("Distribution points", parent,
           GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL,
           GTK_RESPONSE_CANCEL, NULL);
           
  label = gtk_label_new("Number of distribution points:");
  
  ndistp_adj = GTK_ADJUSTMENT(gtk_adjustment_new(5, MIN_DISTP, MAX_DISTP, 1.00, 2.00, 0.00));
  ndistp = gtk_spin_button_new(ndistp_adj, 1.00, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ndistp),(double) n_distpoints);
  
  /* Table */
  table = gtk_table_new(2, 1, FALSE);
  gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), ndistp, 1, 2, 0, 1);
  
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 10);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), table);
  gtk_widget_show_all(dialog);

  result = gtk_dialog_run(GTK_DIALOG(dialog));
  
  /* Actions when clicking OK or CANCEL. */
  if (result == GTK_RESPONSE_OK)
  {
    num = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ndistp));
    n_distpoints = num;
    add_distpoints();
  }

  gtk_widget_destroy(dialog);
     
}

/* Add simulation data plot */
void add_simplot()
{
     
  GdkColor color;
     
  dataset[0] = GTK_PLOT_DATA(gtk_plot_data_new());
  gtk_plot_add_data(GTK_PLOT(active_plot1), dataset[0]);
  gtk_widget_show(GTK_WIDGET(dataset[0]));
  
  /* Data options */
  gdk_color_parse("black", &color);
  gdk_color_alloc(gdk_colormap_get_system(), &color); 
  /* gtk_plot_data_set_symbol(dataset[0], GTK_PLOT_SYMBOL_CROSS,
                           GTK_PLOT_SYMBOL_FILLED, 4, 3, &color, &color); */
  gtk_plot_data_set_line_attributes(dataset[0], GTK_PLOT_LINE_SOLID, 0, 0, 2, &color);
  gtk_plot_data_set_connector(dataset[0], GTK_PLOT_CONNECT_STRAIGHT);
  gtk_plot_data_set_legend(dataset[0], "Simulation");
  
}
  
/* Add experimental data plot */
void add_expplot()
{
     
  GdkColor color;
  
  dataset[1] = GTK_PLOT_DATA(gtk_plot_data_new());
  gtk_plot_add_data(GTK_PLOT(active_plot1), dataset[1]);
  gtk_widget_show(GTK_WIDGET(dataset[1]));
  
  /* Data options */
  gdk_color_parse("red", &color);
  gdk_color_alloc(gdk_colormap_get_system(), &color); 
  /* gtk_plot_data_set_symbol(dataset[1], GTK_PLOT_SYMBOL_CIRCLE,
                           GTK_PLOT_SYMBOL_FILLED, 4, 1, &color, &color); */
  gtk_plot_data_set_line_attributes(dataset[1], GTK_PLOT_LINE_DASHED, 0, 0, 2, &color);
  gtk_plot_data_set_connector(dataset[0], GTK_PLOT_CONNECT_STRAIGHT);
  gtk_plot_data_set_legend(dataset[1], "Experimental");

  bool_exp_plot = 1;

}

/* Update plot. Scale and calculate ticks again */
void update_plot()
{
     
  double xmin, xmax;
  double ymin, ymax;
  gint p;
  
  xmin = simu.min_x;
  xmax = simu.max_x;
  ymin = simu.min_y;
  ymax = simu.max_y;
  
  /* If experimental plot is active, checking min and max from experimental data for scaling */
  if (bool_exp_plot)
  {
    if (expr.min_x < xmin)
      xmin = expr.min_x;
    if (xmax < expr.max_x)
      xmax = expr.max_x;
    if (expr.min_y < ymin)
      ymin = expr.min_y;
    if (ymax < expr.max_y)
      ymax = expr.max_y;
  }
  
  gtk_plot_autoscale(GTK_PLOT(active_plot1));
  gtk_plot_axis_ticks_autoscale(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT), ymin, 1.1*(ymax-ymin), &p);
  gtk_plot_axis_ticks_autoscale(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM), xmin, xmax + 0.1*(xmax-xmin), &p); /* also if x<0 */

/* Timo's addition 28.1.2011, removes extra zeros from y-axis */
  gtk_plot_axis_set_labels_numbers(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT), GTK_PLOT_LABEL_FLOAT, 0);

  gtk_plot_axis_set_ticks_limits(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT), ymin, 1.1*(ymax-ymin));
  gtk_plot_axis_set_ticks_limits(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM), xmin, xmax + 0.1*(xmax-xmin));
  
  gtk_plot_set_yrange(GTK_PLOT(active_plot1), ymin, 1.1*(ymax-ymin));
  gtk_plot_set_xrange(GTK_PLOT(active_plot1), xmin, xmax + 0.1*(xmax-xmin));
  
  gtk_plot_axis_ticks_recalc(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT));
  gtk_plot_axis_ticks_recalc(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM));
  
  gtk_plot_canvas_paint(GTK_PLOT_CANVAS(canvas));
  gtk_widget_queue_draw(GTK_WIDGET(canvas));
  
}

/* Remove experimental plot */
void remove_plot()
{
     
  if(bool_exp_plot == 1)
  {
    gtk_plot_remove_data(GTK_PLOT(active_plot1), GTK_PLOT_DATA(dataset[1]));
    bool_exp_plot = 0;
    if (zoom_active)
      unzoom();
    else
      update_plot();
  }
  
  plot_path = "";
  bool_exp_plot = 0;
  
}

/* Export graph in the format (.ps). Whole canvas (energy spectrum + distribution graph) or only energy spectrum can be exported */
void export_graph(gpointer parent, guint callback_action, GtkWidget *widget)
{

  char *fname;

  if (callback_action == TYPE_PLOT)
  {
    fname = (char *)calloc(strlen(file_path) + strlen("_1.ps") + 1, sizeof(char));
    strcat(fname, file_path);
    strcat(fname, "_1.ps");
    gtk_plot_export_ps(GTK_PLOT(active_plot1), fname, GTK_PLOT_PORTRAIT, FALSE, GTK_PLOT_LETTER);
    export_dialog(fname);
  }

  if (callback_action == TYPE_CANVAS)
  {
    fname = (char *)calloc(strlen(file_path) + strlen("_2.ps") + 1, sizeof(char));
    strcat(fname, file_path);
    strcat(fname, "_2.ps");
    gtk_plot_canvas_export_ps(GTK_PLOT_CANVAS(canvas), fname, GTK_PLOT_PORTRAIT, FALSE, GTK_PLOT_LETTER);
    export_dialog(fname);
  }

  free(fname);

}

/* Change canvas flags for different options (normal, select and zoom).
 * When some option is selected, flags for other options are unset */
void hand_tool(gpointer callback_data, guint callback_action, GtkWidget *widget)
{
     
  /* Normal: Plots and titles can be moved */
  if(callback_action == HAND_NORMAL)
  {
    GTK_PLOT_CANVAS_UNSET_FLAGS(GTK_PLOT_CANVAS(canvas), GTK_PLOT_CANVAS_CAN_SELECT);
    GTK_PLOT_CANVAS_PLOT(child2)->flags &= ~GTK_PLOT_CANVAS_PLOT_SELECT_POINT;
    GTK_PLOT_CANVAS_PLOT(child2)->flags &= ~GTK_PLOT_CANVAS_PLOT_DND_POINT;
  
    GTK_PLOT_CANVAS_SET_FLAGS(GTK_PLOT_CANVAS(canvas), GTK_PLOT_CANVAS_CAN_SELECT_ITEM);
    GTK_PLOT_CANVAS_SET_FLAGS(GTK_PLOT_CANVAS(canvas), GTK_PLOT_CANVAS_CAN_DND);
  }
  /* Select: Data points from distribution graph can be selected and moved */
  if(callback_action == HAND_SELECT)
  {
    GTK_PLOT_CANVAS_UNSET_FLAGS(GTK_PLOT_CANVAS(canvas), GTK_PLOT_CANVAS_CAN_SELECT);
  
    GTK_PLOT_CANVAS_SET_FLAGS(GTK_PLOT_CANVAS(canvas), GTK_PLOT_CANVAS_CAN_SELECT_ITEM);
    GTK_PLOT_CANVAS_SET_FLAGS(GTK_PLOT_CANVAS(canvas), GTK_PLOT_CANVAS_CAN_DND);
    GTK_PLOT_CANVAS_PLOT(child2)->flags |= GTK_PLOT_CANVAS_PLOT_SELECT_POINT;
    GTK_PLOT_CANVAS_PLOT(child2)->flags |= GTK_PLOT_CANVAS_PLOT_DND_POINT;
  }
  /* Area can be selected (for zooming) */
  if(callback_action == HAND_ZOOM)
  {
    GTK_PLOT_CANVAS_UNSET_FLAGS(GTK_PLOT_CANVAS(canvas), GTK_PLOT_CANVAS_CAN_SELECT_ITEM);
    GTK_PLOT_CANVAS_UNSET_FLAGS(GTK_PLOT_CANVAS(canvas), GTK_PLOT_CANVAS_CAN_DND);
    GTK_PLOT_CANVAS_PLOT(child2)->flags &= ~GTK_PLOT_CANVAS_PLOT_SELECT_POINT;
    GTK_PLOT_CANVAS_PLOT(child2)->flags &= ~GTK_PLOT_CANVAS_PLOT_DND_POINT;
  
    GTK_PLOT_CANVAS_SET_FLAGS(GTK_PLOT_CANVAS(canvas), GTK_PLOT_CANVAS_CAN_SELECT);
  }
  
}

/* When area is selected, pixels are changed to data values and these are used for zooming.
 * Only x-values are used for zooming, autoscaling is used for y-axis so that all data points between xmin and xmax are used */
void select_region(GtkWidget *canvas, gdouble xmin, gdouble ymin, gdouble xmax, gdouble ymax, gpointer data)
{
     
  gint pix_x1, pix_x2, pix_y1, pix_y2; /* pixels from canvas */
  double temp_x;
  double miny, maxy; /* for scaling y-axis */
  double miny_sim, maxy_sim; /* for determining min and max from simulation data points */
  double miny_exp, maxy_exp; /* for determining min and max from experimental data points */
  int temp1, temp2; /* just used for saving and restoring number of data points */
  gint p;
  
  miny = 0; /* use miny = 0, maxy = 0 => scaling between 0 and maxy */
  maxy = 0; /* and use miny = BIG_VALUE, maxy = -BIG_VALUE => scaling between miny and maxy */
  
  miny_sim = maxy_sim = miny_exp = maxy_exp = 0;
     
  temp_x = 0;
  temp1 = 0;
  temp2 = 0;
  
  /* getting values from selected area */   
  gtk_plot_canvas_get_pixel(GTK_PLOT_CANVAS(canvas), xmin, ymin, &pix_x1, &pix_y1);
  gtk_plot_canvas_get_pixel(GTK_PLOT_CANVAS(canvas), xmax, ymax, &pix_x2, &pix_y2);
  gtk_plot_get_point(GTK_PLOT(active_plot1), pix_x1, pix_y1, &xmin, &ymin);
  gtk_plot_get_point(GTK_PLOT(active_plot1), pix_x2, pix_y2, &xmax, &ymax);
      
  /* Checking which one is minimum */
  if (xmax < xmin)
  {
    temp_x = xmin;
    xmin = xmax;
    xmax = temp_x;
  }
  
  /* when zooming "original" data */
  if (!zoom_active)
  {
    simu.ndata_zoom = simu.ndata;
    temp1 = simu.ndata_zoom;
    zoom_plot(simu.temp_px, simu.temp_py, px, py, xmin, xmax, &miny_sim, &maxy_sim, PLOT_SIM);
        
    if (bool_exp_plot)
    {
      expr.ndata_zoom = expr.ndata;
      temp2 = expr.ndata_zoom;
      zoom_plot(expr.temp_px, expr.temp_py, px2, py2, xmin, xmax, &miny_exp, &maxy_exp, PLOT_EXP);
    }
          
  }
  /* when zooming already zoomed data */
  else if (zoom_active)
  {
    temp1 = simu.ndata_zoom;
    zoom_plot(simu.temp_px, simu.temp_py, simu.temp_px, simu.temp_py, xmin, xmax, &miny_sim, &maxy_sim, PLOT_SIM);
        
    if (bool_exp_plot)
    {
      temp2 = expr.ndata_zoom;
      zoom_plot(expr.temp_px, expr.temp_py, expr.temp_px, expr.temp_py, xmin, xmax, &miny_exp, &maxy_exp, PLOT_EXP);
    }
  }
  
  if ( (!bool_exp_plot && simu.ndata_zoom < 1) || (bool_exp_plot && simu.ndata_zoom < 1 && expr.ndata_zoom < 1) )
  {
    error_dialog("Not any data points in the selected area");
    gtk_widget_queue_draw(GTK_WIDGET(canvas));
    simu.ndata_zoom = temp1;
    expr.ndata_zoom = temp2;
    return;
  }
            
  gtk_plot_data_set_points(dataset[0], simu.temp_px, simu.temp_py, 0, 0, simu.ndata_zoom);
  if (bool_exp_plot)
  {
    gtk_plot_data_set_points(dataset[1], expr.temp_px, expr.temp_py, 0, 0, expr.ndata_zoom);
  }
      
  zoom_active = 1;
  
  /* Checking min and max for scaling y-axis */
  if (miny_sim < miny)
    miny = miny_sim;
  if (miny_exp < miny)
    miny = miny_exp;
  if (maxy < maxy_sim)
    maxy = maxy_sim;
  if (maxy < maxy_exp)
    maxy = maxy_exp;
     
  gtk_plot_axis_ticks_autoscale(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM), xmin, xmax, &p);
  gtk_plot_axis_ticks_autoscale(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT), miny, maxy, &p);
  /* Timo's addition 28.1.2011, removes extra zeros from y-axis */
  gtk_plot_axis_set_labels_numbers(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT), GTK_PLOT_LABEL_FLOAT, 0);
  

  gtk_plot_axis_set_ticks_limits(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM), xmin, xmax);
  gtk_plot_axis_set_ticks_limits(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT), miny, maxy);

  
  gtk_plot_set_xrange(GTK_PLOT(active_plot1), xmin, xmax);
  gtk_plot_set_yrange(GTK_PLOT(active_plot1), miny, maxy);
  
  gtk_plot_axis_ticks_recalc(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT));
  gtk_plot_axis_ticks_recalc(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM));
  
  gtk_plot_canvas_paint(GTK_PLOT_CANVAS(canvas));
  gtk_widget_queue_draw(GTK_WIDGET(canvas));
  
}

/* Reads data points from arrays (px2, py2) and saves all data points between xmin and xmax to arrays (px, py).
 * Same function used for different type of data (simulation and experimental).
 * Gives values for miny and maxy determined from zoomed "new" data */
void zoom_plot(double *px, double *py, double *px2, double *py2, double xmin, double xmax, double *miny, double *maxy, int type)
{
     
  int i, j;
  int n;
  double min, max;
   
  i = 0;
  j = 0;
  n = 0;
  
  min = BIG_VALUE;
  max = 0; /* y can't be negative */
      
  if (type == PLOT_SIM)
    n = simu.ndata_zoom;
  if (type == PLOT_EXP)
    n = expr.ndata_zoom;
  
  /* Reading data and saving data between xmin and xmax */    
  while (i < MAX_POINTS && i < n)
  {
    if (xmin <= px2[i] && px2[i] <= xmax)
    {
      px[j] = px2[i];
      py[j] = py2[i];
       
      /* Updating min and max values */   
      if (py2[i] < min)
        min = py2[i];
      if (max < py2[i])
        max = py2[i];
            
      j++;
    }
        
    i++;
  }
      
  if (type == PLOT_SIM)
    simu.ndata_zoom = j;
  if (type == PLOT_EXP)
    expr.ndata_zoom = j;
      
  *miny = min;
  *maxy = max;
     
}

/* Returning to "original" nonzoomed data */
void unzoom()
{
     
  if (zoom_active)
  {
    gtk_plot_data_set_points(dataset[0], px, py, 0, 0, simu.ndata);
    simu.ndata_zoom = 0;
    if (bool_exp_plot)
    {
      gtk_plot_data_set_points(dataset[1], px2, py2, 0, 0, expr.ndata);
      expr.ndata_zoom = 0;
    }
      
    update_plot();
  }
     
  zoom_active = 0;
     
}

/* Change scale to linear */
void lin_scale()
{
     
  gtk_plot_set_yscale(GTK_PLOT(active_plot1), GTK_PLOT_SCALE_LINEAR);
  update_plot();

}

/* Change scale to logarithmic */
void log_scale()
{
     
  gtk_plot_set_yscale(GTK_PLOT(active_plot1), GTK_PLOT_SCALE_LOG10);
  update_plot();

}

/* Change range to be between xmin and xmax */
void change_xrange(double xmin, double xmax)
{
  gint p;
  double temp;
  
  if(xmin == xmax)
    return;
  if(xmax < xmin)
  {
    temp = xmin;
    xmin = xmax;
    xmax = temp;
  }
     
  gtk_plot_axis_ticks_autoscale(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM), xmin, xmax, &p);
  gtk_plot_set_xrange(GTK_PLOT(active_plot1), xmin, xmax);
  
  gtk_plot_canvas_paint(GTK_PLOT_CANVAS(canvas));
  gtk_widget_queue_draw(GTK_WIDGET(canvas));
  
}

/* Create plot window, and add simulation and distribution plots */
void plot_espe(int type, char *fname)
{
  
  /* When plotting simulated data */
  if (type == TYPE_SIMU)
  {
    sim_plot = (char *)calloc(strlen(file_path) + strlen(FSPE) + 1, sizeof(char));
    strcat(sim_plot, file_path);
    strcat(sim_plot, FSPE);
  }
  
  /* When opening an existing plot (file) */
  if (type == TYPE_OPEN)
  {
    sim_plot = (char *)calloc(strlen(fname) + 1, sizeof(char));
    strcpy(sim_plot, fname);
    file_path = (char *)calloc(strlen(fname) - strlen(FSPE) + 1, sizeof(char));
    strncpy(file_path, fname, strlen(fname) - strlen(FSPE));
  }
      
  plot_path = "";
  bool_exp_plot = 0;
  default_scale = DEF_SCALE;
  zoom_active = 0;
  
  rnd(0.0,1.0,RND_SEED,SEED_ESPE); /* initializing RNG for get_espe() */
  
  /* Min and max values for scaling the distribution graph */
  d_min = rdist_x[0];
  d_max = rdist_x[MAX_RDIST - 1];
      
  /* Here starts GtkPlot part */
  GtkWidget *window1;
  GtkWidget *vbox1;
  GtkWidget *menubar;
  GtkWidget *scrollw1;
  gint page_width, page_height;
  gfloat scale = 1.0;                       /* Scale for canvas */
  gdouble magnif = 1.35;                     /* Magnification for canvas */
  gint p;
  
  /* Window and canvas options */
  page_width = GTK_PLOT_LETTER_W * scale;
  page_height = GTK_PLOT_LETTER_H * scale;

  window1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window1), "MCERD");
  gtk_window_set_default_size(GTK_WINDOW(window1), 800, 750);
  gtk_container_border_width(GTK_CONTAINER(window1) ,0);
  gtk_window_set_modal(GTK_WINDOW(window1), TRUE);

  g_signal_connect(G_OBJECT(window1), "destroy",
                     G_CALLBACK(quit), NULL);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window1), vbox1);
  gtk_widget_show(vbox1);
  
  menubar = get_plot_menu(window1);
  gtk_box_pack_start(GTK_BOX(vbox1), menubar, FALSE, TRUE, 0);
  gtk_widget_show(menubar);

  scrollw1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_border_width(GTK_CONTAINER(scrollw1), 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollw1),
				GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(vbox1), scrollw1, TRUE, TRUE, 0);
  gtk_widget_show(scrollw1);

  canvas = gtk_plot_canvas_new(page_width, page_height, magnif);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrollw1), canvas);

  /* Background color */
  /*
  gdk_color_parse("light blue", &color);
  gdk_color_alloc(gtk_widget_get_colormap(canvas), &color);
  gtk_plot_canvas_set_background(GTK_PLOT_CANVAS(canvas), &color);
  */

  gtk_widget_show(canvas);

  /* Create simulation plot */
  active_plot1 = gtk_plot_new_with_size(NULL, 0.5, .25);
  
  /* Axis options */
  gtk_plot_legends_move(GTK_PLOT(active_plot1), 1.1, 0.0);
  gtk_plot_set_legends_border(GTK_PLOT(active_plot1), 0, 0);
  gtk_plot_axis_hide_title(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_TOP));
  gtk_plot_axis_hide_title(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_RIGHT));
  gtk_plot_axis_show_ticks(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM), GTK_PLOT_TICKS_OUT, GTK_PLOT_TICKS_OUT);
  gtk_plot_axis_show_ticks(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT), GTK_PLOT_TICKS_OUT, GTK_PLOT_TICKS_OUT);
  gtk_plot_axis_set_visible(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_TOP), FALSE);
  gtk_plot_axis_set_visible(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_RIGHT), FALSE);
  gtk_plot_axis_set_title(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT), "Counts");
  gtk_plot_axis_set_title(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM), "Energy [MeV]");
  gtk_plot_axis_move_title(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM), 0, 0.4, 0.39);

  
  add_simplot();
  read_data(sim_plot, PLOT_SIM, DEF_SCALE/default_scale);

  /* Scaling */
  gtk_plot_autoscale(GTK_PLOT(active_plot1));
  gtk_plot_axis_ticks_autoscale(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT), simu.min_y, 1.1*(simu.max_y-simu.min_y), &p);
  gtk_plot_axis_ticks_autoscale(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM), simu.min_x, simu.max_x + 0.1*(simu.max_x-simu.min_x), &p);
  /* Timo's addition 28.1.2011, removes extra zeros from y-axis */
  gtk_plot_axis_set_labels_numbers(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT), GTK_PLOT_LABEL_FLOAT, 0);

  gtk_plot_axis_set_ticks_limits(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_LEFT), simu.min_y, 1.1*(simu.max_y-simu.min_y));
  gtk_plot_axis_set_ticks_limits(gtk_plot_get_axis(GTK_PLOT(active_plot1), GTK_PLOT_AXIS_BOTTOM), simu.min_x, simu.max_x + 0.1*(simu.max_x-simu.min_x));
  
  /* Add simulation plot to the canvas */
  child1 = gtk_plot_canvas_plot_new(GTK_PLOT(active_plot1));
  gtk_plot_canvas_put_child(GTK_PLOT_CANVAS(canvas), child1, .15, .02, .65, .27);
  gtk_widget_show(active_plot1);
  
  /* Create distribution plot */
  active_plot2 = gtk_plot_new_with_size(NULL, 0.5, .25);
  
  /* Add distribution points and graph (after simulation). Using default distribution points. */
  if (type == TYPE_SIMU)
  {
    add_distplot();
    add_maindistpoints();
  }
  
  /* Add distribution points and graph (when opening an existing file). Reading distribution points from file. */
  if (type == TYPE_OPEN)
  {
    add_distplot();
    read_distpoints();
  }
  
  /* Add distribution plot to the canvas */
  child2 = gtk_plot_canvas_plot_new(GTK_PLOT(active_plot2));
  gtk_plot_canvas_put_child(GTK_PLOT_CANVAS(canvas), child2, .15, .35, .65, .60);
  gtk_widget_show(active_plot2);
  hand_tool(NULL, HAND_NORMAL, NULL);
  gtk_plot_clip_data(GTK_PLOT(active_plot2), TRUE);
  
  g_signal_connect(G_OBJECT(canvas), "select_region", G_CALLBACK(select_region), NULL);

  gtk_widget_show_all(window1);
  
  refresh_espe();

}
