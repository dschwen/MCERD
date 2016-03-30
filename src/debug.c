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
#include "prototypes.h"

void print_ion_position(Global *global,Ion *ion,const char label[],int flag)
{
  /*
   *  Print ion position in the laboratory coordinates
   */

   Point plab;

   if(global->simstage == REALSIMULATION){
      plab = coord_transform(ion->lab.p,ion->lab.theta,ion->lab.fii,ion->p,
                              FORW);
      fprintf(global->master.fpdebug,"%s %2i %2i %9.2f %11.7f %11.7f %11.7f ",
             label,ion->type,ion->tlayer,ion->E/C_KEV,plab.x/C_MM,plab.y/C_MM,
             plab.z/C_MM);
      fprintf(global->master.fpdebug," %7.2f %7.2f %7.2f\n",ion->p.x/C_NM,
              ion->p.y/C_NM,ion->p.z/C_NM);
   }
}
