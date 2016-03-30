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


/* This file includes functions for checking GUI parameters */


#include "general.h"
#include "protot.h"

/* Check all gui parameters that there is not any empty fields or
 * any incorrect values. Actually limits for most of the parameters are defined in param.h */
void check_param()
{

  int i, j, elem;
  double d_total;
  
  d_total = 0;
  
  /* Misc parameters */
  if (file_path == NULL || strcmp(file_path, "") == 0)
  {
    error_dialog("Save first for the output data");
    return;
  }
  if (strlen(file_path) > NFILE - 10) /* seed number and file format is added to file_path afterwards */
  {
    error_dialog("Filename is too long for output data");
    return;
  }
  if (ion_num != 1 && ion_num != 2)
  {
    error_dialog("Invalid number of different ions in the simulation");
    return;
  }
     
  /* Simulation parameters */
  if (sim_type != SIM_ERD && sim_type != SIM_RBS)
  {
    error_dialog("Simulation type not selected");
    return;
  }
  if (beam_ion == NULL || strcmp(beam_ion, "") == 0)
  {
    error_dialog("Empty field: beam ion");
    return;
  }
  if (beam_energy == 0)
  {
    error_dialog("Empty field: beam energy");
    return;
  }
  if (recoil_atom == NULL || strcmp(recoil_atom, "") == 0)
  {
    error_dialog("Empty field: recoil atom");
    return;
  }
  if (beam_size_x == 0 || beam_size_y == 0)
  {
    error_dialog("Empty field: beam size");
    return;
  }
  if (min_angle == 0)
  {
    error_dialog("Empty field: minimum angle of scattering");
    return;
  }
  if (min_energy == 0)
  {
    error_dialog("Empty field: minimum energy of ions");
    return;
  }
  if (angle_width != REC_NARROW && angle_width != REC_WIDE)
  {
    error_dialog("Recoiling angle width type not selected");
    return;
  }
  if (nions == 0)
  {
    error_dialog("Empty field: number of simulated ions");
    return;
  }
  
  /* Detector parameters */
  if (det_type != DET_TOF && det_type != DET_GAS)
  {
    error_dialog("Detector type not selected");
    return;
  }
  if (det_type == DET_TOF)
  {
    if (det_num1 >= det_num2 || det_num2 > nfoils)
    {
      error_dialog("Incorrect timing detector foil numbers");
      return;
    }
  }
  
  /* Target parameters */
  if (nlayers == 0)
  {
    error_dialog("Target has no layers");
    return;
  }
  for (i = 0; i < nlayers; i++)
  {
      
    /* calculating total target thickness. this is needed later */
    if(layer_thickness[i] < 0)
      d_total += 1E9; /* bulk sample, adding 1m */
    else
      d_total += layer_thickness[i];
      
    if (layer_thickness[i] == 0)
    {
      error_dialog("Empty field: target thickness");
      return;
    }
    if (layer_density[i] == 0)
    {
      error_dialog("Empty field: target density");
      return;
    }
    if (stop_ion_layer[i] == NULL || strcmp(stop_ion_layer[i], "") == 0)
    {
      error_dialog("Empty field: target stopping power");
      return;
    }
    if (ion_num == 2)
    {
      if (stop_rec_layer[i] == NULL || strcmp(stop_rec_layer[i], "") == 0)
      {
        error_dialog("Empty field: target stopping power");
        return;
      }
    }
    elem = 0;
    /* Only lines with some given element are taken into account */
    for (j = 0; j < MAX_ELEMENTS; j++)
    {
      if (elem_layer[j][i] != NULL && strcmp(elem_layer[j][i], ""))
      {
        if (mass_layer[j][i] == 0 || con_layer[j][i] == 0)
        {
          error_dialog("Target layer element paramater(s) missing");
          return;
        }
        elem++;
      }
    }
    if (elem == 0)
    {
      error_dialog("Target layer has no elements");
      return;
    }
  }
  
  /* Foil parameters */
  if (nfoils == 0)
  {
    error_dialog("Detector has no foils");
    return;
  }
  for (i = 0; i < nfoils; i++)
  {
      
    if ( i != nfoils-1 )
    {
      if (foil_distance[i] >= foil_distance[i+1])
      {
        error_dialog("Incorrect detector foil distances");
        return;
      }
    }
    if (foil_type[i] == FOIL_CIRC)
    {
      if (foil_diameter[0][i] == 0)
      {
        error_dialog("Empty field: detector foil diameter");
        return;
      }
    }
    else if (foil_type[i] == FOIL_RECT)
    {
      if (foil_diameter[1][i] == 0 || foil_diameter[2][i] == 0)
      {
        error_dialog("Empty field: detector foil diameter");
        return;
      }
    }
    if (foil_distance[i] == 0)
    {
      error_dialog("Empty field: detector foil distance");
      return;
    }
    if (stop_ion_foil[i] == NULL || strcmp(stop_ion_foil[i], "") == 0)
    {
      error_dialog("Empty field: detector foil stopping power");
      return;
    }
    if (ion_num == 2)
    {
      if (stop_rec_foil[i] == NULL || strcmp(stop_rec_foil[i], "") == 0)
      {
        error_dialog("Empty field: detector foil stopping power");
        return;
      }
    }
    if (foil_thickness[i] == 0)
    {
      error_dialog("Empty field: detector foil thickness");
      return;
    }
    if (foil_density[i] == 0)
    {
      error_dialog("Empty field: detector foil density");
      return;
    }
    elem = 0;
    /* Only lines with some given element are taken into account */
    for (j = 0; j < MAX_FELEMENTS; j++)
    {
      if (elem_foil[j][i] != NULL && strcmp(elem_foil[j][i], ""))
      {
        if ((mass_foil[j][i] == 0) || (con_foil[j][i] == 0))
        {
          error_dialog("Detector foil element paramater(s) missing");
          return;
        }
        elem++;
      }
    }
    if (elem == 0)
    {
      error_dialog("Detector foil has no elements");
      return;
    }
  }
  
  /* Recoil distribution parameters */
  if (MAX_RDIST < 2)
  {
    error_dialog("Too few points in the recoiling material distribution");
    return;
  }
  /* This shouldn´t be possible */
  if (rdist_x[0] < 0)
  {
    error_dialog("Recoil distribution can not start from a negative depth");
    return;
  }
  for (i = 0; i < MAX_RDIST; i++)
  {
    if (i != MAX_RDIST-1)
    {
      if (rdist_x[i] >= rdist_x[i+1])
      {
        error_dialog("Invalid recoil material distribution");
        return;
      }
    }
    if (rdist_y[i] > 0 && rdist_x[i] > d_total)
    {
    error_dialog("Depth for recoil distribution > target thickness");
    return;
    }
  }
  
  /* Pre simulation parameters */
  /* Checking these only when "use pre simulation" is selected */
  if (use_presimu == TRUE)
  {
    for (i = 0; i < nlayers; i++)
    {
      if ( presimu_x[i] == 0 || presimu_y[i] == 0)
      {
        error_dialog("All target layers doesn't have pre simulation values");
        return;
      }
    }
  }

  
}

/* Energy spectrum parameters */
void check_espe_param()
{
     
  if (n_distpoints == 0)
  {
    error_dialog("No any distribution data points");
    return;
  }
  if (n_distpoints >= NDDIST)
  {
    error_dialog("Too many distribution data points");
    return;
  }
  
  if (espe.virtual == TRUE)
  {
    if (!(espe.vsize >= 0.0 && espe.vsize <= 10000.0))
    {
      error_dialog("Invalid energy spectrum parameter, virtual detector size");
      return;
    }
    if (!(espe.vnum > 0.0 && espe.vnum <= MAXNDEPTH))
    {
      error_dialog("Invalid energy spectrum parameter, number of depth channels in virtual scaling");
      return;
    }
  }
  
  if (espe.dsize == TRUE)
  {
    if (!(espe.dx > 0.0 && espe.dx <= 10000.0))
    {
      error_dialog("Invalid energy spectrum parameter, dsize");
      return;
    }
    if (!(espe.dy > 0.0 && espe.dy <= 10000.0))
    {
      error_dialog("Invalid energy spectrum parameter, dsize");
      return;
    }
  }
  
  if (espe.dwindow == TRUE)
  {
    if (!(espe.dmin >= 0.0 && espe.dmax >= 0.0))
    {
      error_dialog("Invalid energy spectrum parameter, depth");
      return;
    }
  }
  
  if (espe.mass == TRUE)
  {
    if (!(espe.mass_value > 0.0 && espe.mass_value < 300.0))
    {
      error_dialog("Invalid energy spectrum parameter, recoil mass");
      return;
    }
  }
  
  if (espe.timeres == TRUE)
  {
    if (!(espe.tres >= 0.0 && espe.tres <= 10000.0))
    {
      error_dialog("Invalid energy spectrum parameter, time resolution");
      return;
    }
    if (!(espe.toflen > 0.0 && espe.toflen <= 50.0))
    {
      error_dialog("Invalid energy spectrum parameter, TOF length");
      return;
    }
  }
  
  if (espe.timeres == FALSE)
  {
    if (!(espe.eres > 0.0 && espe.eres <= 100000.0))
    {
      error_dialog("Invalid energy spectrum parameter, energy resolution");
      return;
    }
  }
  
  if (!(espe.chwidth > 0.0 && espe.chwidth <= 10.0))
  {
    error_dialog("Invalid energy spectrum parameter, channel width");
    return;
  }
  
  if (!(espe.scale > 0.0))
  {
    error_dialog("Invalid energy spectrum parameter, scale");
    return;
  }
  
}
