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

void finish_ion(Global *global,Ion *ion,Target *target,Detector *detector)
{

   if(ion->status == FIN_STOP)
      fprintf(global->master.fprange,"R %10.3f\n",ion->p.z/C_NM);

   if(ion->status == FIN_TRANS)
      fprintf(global->master.fprange,"T %12.6f\n",ion->E/C_MEV);

}


