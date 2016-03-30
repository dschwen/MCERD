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

void finalize(Global *global)
{
   int i,j;
   
   fprintf(global->master.fpdat,"Statistics of ion finishes: \n");
 
   for(i=0;i<MAXIONS;i++)
      for(j=0;j<NIONSTATUS;j++)
         fprintf(global->master.fpdat,"%2i %2i: %5i\n",i,j,global->finstat[i][j]);
         
   fclose(global->master.fperd);
   fclose(global->master.fpout);   
   fclose(global->master.fpdat);
   fclose(global->master.fprange); /* added */
   
   global->master.fperd = NULL;
   global->master.fpout = NULL;
   global->master.fpdat = NULL;
   global->master.fprange = NULL;
   
#ifdef DEBUG
   fclose(global->master.fpdebug);
   global->master.fpdebug = NULL;
#endif

}
   
/* Close all open files when error occurs during the simulation */
void close_files(Global *global)
{
     
   if(global->master.fperd != NULL)
      fclose(global->master.fperd);
   if(global->master.fpout != NULL)
      fclose(global->master.fpout);
   if(global->master.fpdat != NULL) 
      fclose(global->master.fpdat);
   if(global->master.fprange != NULL)
      fclose(global->master.fprange);
      
#ifdef DEBUG
   if(global->master.fpdebug != NULL)
      fclose(global->master.fpdebug);
#endif  
     
}
