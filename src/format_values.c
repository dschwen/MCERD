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

int ion_num; /* Number of different ions in simulation (1 or 2) */
int saved;   /* 0, if not saved and 1 if saved */

/* Give all parameters some (initial) value */
void init_values()
{

  file_path = "";
  saved = 1;
  ion_num = 2;
  n_distpoints = 5;

  sim_type = SIM_ERD;
  beam_ion = "";
  beam_energy = 0;
  recoil_atom = "";
  target_angle = 0;
  beam_size_x = 0;
  beam_size_y = 0;
  min_angle = 0;
  min_energy = 0;
  aver_nions = 0;
  angle_width = REC_NARROW;
  nions = 0;
  nions_presim = 0;
  nseed = 0;
  
  det_type = DET_TOF;
  det_angle = 0;
  vdet_size1 = 0;
  vdet_size2 = 0;
  det_num1 = 0;
  det_num2 = 0;
  
  nlayers = 0;
  int i, j;
  for (i = 0; i < MAX_LAYERS; i++)
  {
    layer_thickness[i] = 0;
    layer_density[i] = 0;
    stop_ion_layer[i] = "ZBL";
    stop_rec_layer[i] = "ZBL";
    for (j = 0; j < MAX_ELEMENTS; j++)
    {
      mass_layer[j][i] = 0;
      elem_layer[j][i] = "";
      con_layer[j][i] = 0;
    }
  }
  
  nfoils = 0;
  for (i = 0; i < MAX_FOILS; i++)
  {
    foil_type[i] = FOIL_CIRC;
    foil_diameter[0][i] = 0;
    foil_diameter[1][i] = 0;
    foil_diameter[2][i] = 0;
    foil_distance[i] = 0;
    stop_ion_foil[i] = "ZBL";
    stop_rec_foil[i] = "ZBL";
    foil_thickness[i] = 0;
    foil_density[i] = 0;
    for (j = 0; j < MAX_FELEMENTS; j++)
    {
      elem_foil[j][i] = "";
      con_foil[j][i] = 0;
      mass_foil[j][i] = 0;
    }
  }
  
  for (i = 0; i < MAX_RDIST; i++)
  {
    rdist_x[i] = 0;
    rdist_y[i] = 0;
  }
  
  use_presimu = 0;
  for (i = 0; i < MAX_PRESIMU; i++)
  {
    presimu_x[i] = 0;
    presimu_y[i] = 0;
  }
  
  espe.virtual = FALSE;
  espe.vsize = 0;
  espe.vnum = 0;
  
  espe.dsize = FALSE;
  espe.dx = 0;
  espe.dy = 0;
  
  espe.pnvd = FALSE;
  
  espe.wide = FALSE;
  
  espe.dwindow = FALSE;
  espe.dmin = 0;
  espe.dmax = 0;
  
  espe.ddist = FALSE;
  
  espe.mass = FALSE;
  espe.mass_value = 0;
  
  espe.timeres = TRUE;
  espe.tres = 0;
  espe.eres = 0;
  
  espe.toflen = 0;
  espe.chwidth = 0;
  espe.scale = 0;
  
}

/* Clean "memory" when foil(s) are removed or new file is opened */
void format_foils(int nfoils)
{
  int i, j;
  for (i = nfoils; i < MAX_FOILS; i++)
  {
    foil_type[i] = FOIL_CIRC;
    foil_diameter[0][i] = 0;
    foil_diameter[1][i] = 0;
    foil_diameter[2][i] = 0;
    foil_distance[i] = 0;
    
    for (j = 0; j < MAX_FELEMENTS; j++)
    {
      elem_foil[j][i] = "";
      mass_foil[j][i] = 0;
      con_foil[j][i] = 0;
    }
    
    foil_thickness[i] = 0;
    stop_ion_foil[i] = "ZBL";
    stop_rec_foil[i] = "ZBL";
    foil_density[i] = 0;
  }
  
}

/* Clean "memory" when target layer(s) are removed or new file is opened */
void format_layers(int nlayers)
{
  int i, j;
  for (i = nlayers; i < MAX_LAYERS; i++)
  {
 
    for (j = 0; j < MAX_ELEMENTS; j++)
    {
      elem_layer[j][i] = "";
      mass_layer[j][i] = 0;
      con_layer[j][i] = 0;
    }
    
    layer_thickness[i] = 0;
    stop_ion_layer[i] = "ZBL";
    stop_rec_layer[i] = "ZBL";
    layer_density[i] = 0;
  }
  
}
    
