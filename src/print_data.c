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
#include "symbols.h"

void print_data(Global *global,Ion *ion,Target *target,
                Scattering scat[][MAXELEMENTS],Detector *detector)
                
{
   FILE *fp;
   double M;
   
   M = 4.0*ion[PRIMARY].A*ion[SECONDARY].A/
       ipow2(ion[PRIMARY].A + ion[SECONDARY].A);
               
/*
   char fname[LINE]="";
*/

   fp = global->master.fpdat;

   fprintf(fp,"Number of simulated ion histories: \t\t%i\n",global->nsimu);
   fprintf(fp,"Minimum energy for simulation: \t\t\t%.3f keV\n",
              global->emin/C_KEV);
   fprintf(fp,"Minimum scattering angle: \t\t\t%.2f%s\n",global->minangle/C_DEG,SYM_DEG);
   fprintf(fp,"Seed number of random number generator: \t%i\n",global->seed);

   fprintf(fp,"Initial energy of the ion:\t\t\t%.4f MeV\n",global->E0/C_MEV);

/*
   fclose(fp);
*/
}
