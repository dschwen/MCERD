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


/* Writing all parameters to files. Format for writing has to be similar with reading methods */


#include "general.h"
#include "protot.h"
#include "read_input.h"

/* Write all files */
void write_files(char *fname)
{
  error = 0;
  write_target(fname);
  if(error)
    return;
  write_detector(fname);
  if(error)
    return;
  write_foils(fname);
  if(error)
    return;
  write_recdist(fname);
  if(error)
    return;
  write_presimu(fname);
  if(error)
    return;
  write_parameters(fname);
  if(error)
    return;
  write_espe(fname);
  if(error)
    return;
  saved = 1;
}

/* Target layers */
void write_target(char *fname)
{
  FILE *file;
  char *fcomb;
  fcomb = (char *)calloc( strlen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL )) + strlen(FTARGET) + 1, sizeof(char) );
  strcat(fcomb, g_locale_from_utf8( fname, -1, NULL, NULL, NULL ));
  strcat(fcomb, FTARGET);
  
  file = fopen(g_locale_from_utf8( fcomb, -1, NULL, NULL, NULL ), "w");
  if (file == NULL)
  {
    error_dialog("Save failed");
    free(fcomb);
    return;
  }
  
  int i;
  int j;
  
  for (i = 0; i < nlayers; i++)
  {
    for (j = 0; j < MAX_ELEMENTS; j++)
    {
      if (elem_layer[j][i] != NULL && strcmp(elem_layer[j][i], ""))
      {
        fprintf(file, "%f\n", mass_layer[j][i]);
        fprintf(file, "%s\n", elem_layer[j][i]);
      }
    }
  }
  int elem_number = 0;
  for (i = 0; i < nlayers; i++)
  {
    fprintf(file, "\n");
    fprintf(file, "%f %s\n", layer_thickness[i], U_THICKNESS);
    fprintf(file, "%s\n", stop_ion_layer[i]);
    fprintf(file, "%s\n", stop_rec_layer[i]);
    fprintf(file, "%f %s\n", layer_density[i], U_DENSITY);
    
    for (j = 0; j < MAX_ELEMENTS; j++)
    {
      if (elem_layer[j][i] != NULL && strcmp(elem_layer[j][i], ""))
      {
        fprintf(file, "%d\n", elem_number);
        fprintf(file, "%f\n", con_layer[j][i]);
        elem_number++;
      }
    }
  }
  
  fclose(file);
  free(fcomb);
  
}

/* Detector parameters */
void write_detector(char *fname)
{
  FILE *file;
  char *fcomb;
  fcomb = (char *)calloc( strlen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL )) + strlen(FDETECTOR) + 1, sizeof(char) );
  strcat(fcomb, g_locale_from_utf8( fname, -1, NULL, NULL, NULL ));
  strcat(fcomb, FDETECTOR);
  
  file = fopen(g_locale_from_utf8( fcomb, -1, NULL, NULL, NULL ), "w");
  if (file == NULL)
  {
    error_dialog("Save failed");
    free(fcomb);
    return;
  }
  
  char *type_det;
  if (det_type == DET_GAS) {type_det = "GAS";}
  else {type_det = "TOF";}

  fprintf(file, "Detector type: %s\n", type_det);
  fprintf(file, "Detector angle: %f\n", det_angle);
  fprintf(file, "Virtual detector size: %f %f\n", vdet_size1, vdet_size2);
  fprintf(file, "Timing detector numbers: %d %d\n", det_num1, det_num2);
  if(nfoils > 0)
    fprintf(file, "==========\n");
  
  char *type_foil;
  double size1, size2;
  int i;
  for (i = 0; i < nfoils; i++)
  {
    if (foil_type[i] == FOIL_CIRC)
    {
      type_foil = "circular";
      size1 = foil_diameter[0][i];
      fprintf(file, "Foil type: %s\n", type_foil);
      fprintf(file, "Foil diameter: %f\n", size1);
    }
    else if (foil_type[i] == FOIL_RECT)
    {
      type_foil = "rectangular";
      size1 = foil_diameter[1][i];
      size2 = foil_diameter[2][i];
      fprintf(file, "Foil type: %s\n", type_foil);
      fprintf(file, "Foil size: %f %f\n", size1, size2);
    }
    
    fprintf(file, "Foil distance: %f\n", foil_distance[i]);
    if(i < (nfoils - 1))
      fprintf(file, "----------\n");
    
  }
   
  fclose(file);
  free(fcomb);
}

/* Detector foils */
void write_foils(char *fname)
{
  FILE *file;
  char *fcomb;
  fcomb = (char *)calloc( strlen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL )) + strlen(FFOIL) + 1, sizeof(char));
  strcat(fcomb, g_locale_from_utf8( fname, -1, NULL, NULL, NULL ));
  strcat(fcomb, FFOIL);
  
  file = fopen(g_locale_from_utf8( fcomb, -1, NULL, NULL, NULL ), "w");
  if (file == NULL)
  {
    error_dialog("Save failed");
    free(fcomb);
    return;
  }
  
  int i;
  int j;
  
  for (i = 0; i < nfoils; i++)
  {
    for (j = 0; j < MAX_FELEMENTS; j++)
    {
      if (elem_foil[j][i] != NULL && strcmp(elem_foil[j][i], ""))
      {
        fprintf(file, "%f\n", mass_foil[j][i]);
        fprintf(file, "%s\n", elem_foil[j][i]);
      }
    }
  }
  int elem_number = 0;
  for (i = 0; i < nfoils; i++)
  {
    fprintf(file, "\n");
    fprintf(file, "%f %s\n", foil_thickness[i], U_THICKNESS);
    fprintf(file, "%s\n", stop_ion_foil[i]);
    fprintf(file, "%s\n", stop_rec_foil[i]);
    fprintf(file, "%f %s\n", foil_density[i], U_DENSITY);
    
    for (j = 0; j < MAX_FELEMENTS; j++)
    {
      if (elem_foil[j][i] != NULL && strcmp(elem_foil[j][i], ""))
      {
        fprintf(file, "%d\n", elem_number);
        fprintf(file, "%f\n", con_foil[j][i]);
        elem_number++;
      }
    }
  }
  
  fclose(file);
  free(fcomb);
  
}

/* Recoil distribution parameters */
void write_recdist(char *fname)
{

  FILE *file;
  char *fcomb;
  fcomb = (char *)calloc( strlen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL )) + strlen(FRECOIL) + 1, sizeof(char) );
  strcat(fcomb, g_locale_from_utf8( fname, -1, NULL, NULL, NULL ));
  strcat(fcomb, FRECOIL);
  
  file = fopen(g_locale_from_utf8( fcomb, -1, NULL, NULL, NULL ), "w");
  if (file == NULL)
  {
    error_dialog("Save failed");
    free(fcomb);
    return;
  }
  
  int i;
  for (i = 0; i < MAX_RDIST; i++)
  {
    fprintf(file, "%f %f\n", rdist_x[i], rdist_y[i]);
  }
  
  fclose(file);
  free(fcomb);
  
}

/* Pre simulation parameters */
void write_presimu(char *fname)
{

  FILE *file;
  char *fcomb;
  fcomb = (char *)calloc( strlen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL )) + strlen(FPRE) + 1, sizeof(char) );
  strcat(fcomb, g_locale_from_utf8( fname, -1, NULL, NULL, NULL ));
  strcat(fcomb, FPRE);
  
  file = fopen(g_locale_from_utf8( fcomb, -1, NULL, NULL, NULL ), "w");
  if (file == NULL)
  {
    error_dialog("Save failed");
    free(fcomb);
    return;
  }
  
  fprintf(file, "%d\n", use_presimu);
  int i;
  for (i = 0; i < MAX_PRESIMU; i++)
  {
    fprintf(file, "%f %f\n", presimu_x[i], presimu_y[i]);
  }
  
  fclose(file);
  free(fcomb);
  
}

/* Basic simulation parameters */
void write_parameters(char *fname)
{
  FILE *file;
  
  file = fopen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL ), "w");
  
  if (file == NULL)
  {
    error_dialog("Save failed");
    return;
  }
  
  char *type;
  if (sim_type == SIM_RBS) {type = "RBS";}
  else {type = "ERD";}
  
  char *width;
  if (angle_width == REC_WIDE) {width = "wide";}
  else {width = "narrow";}
  
  
  
  fprintf(file, "******************* Type of the simulation *******************************\n");
  fprintf(file, "\n");
  fprintf(file, "%s %s\n",inlines[0], type);
  fprintf(file, "\n");
  fprintf(file, "*************** General physical parameters for the simulation ***********\n");
  fprintf(file, "\n");
  fprintf(file, "%s %s\n",inlines[1], beam_ion);
  fprintf(file, "%s %f %s\n",inlines[2], beam_energy, U_ENERGY);
  fprintf(file, "\n");
  fprintf(file, "********** Physical parameters conserning specific simulation type *******\n");
  fprintf(file, "\n");
  fprintf(file, "----------------------- ERD -simulation ----------------------------------\n");
  fprintf(file, "\n");
  fprintf(file, "%s %s\n",inlines[3], recoil_atom);
  fprintf(file, "%s %f %s\n",inlines[4], target_angle, U_ANGLE);
  fprintf(file, "%s %f %f %s\n",inlines[5], beam_size_x, beam_size_y, U_SIZE);
  fprintf(file, "\n");
  fprintf(file, "******************* Parameters with physical significance ****************\n");
  fprintf(file, "\n");
  fprintf(file, "%s %f %s\n",inlines[6], min_angle, U_ANGLE);
  fprintf(file, "%s %f %s\n",inlines[7], min_energy, U_ENERGY);
  fprintf(file, "%s %d\n",inlines[8], aver_nions);
  fprintf(file, "%s %s\n",inlines[9], width);
  fprintf(file, "\n");
  fprintf(file, "******************* Nonphysical parameters for simulation ****************\n");
  fprintf(file, "\n");
  fprintf(file, "%s %d\n",inlines[10], nions);
  fprintf(file, "%s %d\n",inlines[11], nions_presim);
  fprintf(file, "%s %d\n",inlines[12], nseed);
  
  fclose(file);
  
}

/* Energy spectrum parameters */
void write_espe(char *fname)
{

  FILE *file;
  char *fcomb;
  fcomb = (char *)calloc( strlen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL )) + strlen(FESPE) + 1, sizeof(char) );
  strcat(fcomb, g_locale_from_utf8( fname, -1, NULL, NULL, NULL ));
  strcat(fcomb, FESPE);
  
  file = fopen(g_locale_from_utf8( fcomb, -1, NULL, NULL, NULL ), "w");
  if (file == NULL)
  {
    error_dialog("Save failed");
    free(fcomb);
    return;
  }
  
  fprintf(file, "%i %f %i\n", espe.virtual, espe.vsize, espe.vnum);
  fprintf(file, "%i\n", espe.wide);
  fprintf(file, "%i %f %f\n", espe.timeres, espe.tres, espe.eres);
  fprintf(file, "%f\n", espe.toflen);
  fprintf(file, "%f\n", espe.chwidth);
  fprintf(file, "%f\n", espe.scale);
  fprintf(file, "%i %f %f\n", espe.dsize, espe.dx, espe.dy);
  fprintf(file, "%i %f %f\n", espe.dwindow, espe.dmin, espe.dmax);
  fprintf(file, "%i %f\n", espe.mass, espe.mass_value);
  fprintf(file, "%i\n", espe.pnvd);
  
  fclose(file);
  free(fcomb);
  
}
