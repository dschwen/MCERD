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
#include "protot.h"


jmp_buf err_buf; /* Error buffer. When simulation fails, returning to this "point" */
GtkWidget *progress; /* Progress bar */

/* Run simulation. Makes same things than the original MCERD main */
void run_simu()
{

   /* Checking that parameters are given correctly */
   error = 0;
   check_param();
   check_espe_param();
   if(error){
      error_dialog("Cannot run simulation. Missing or invalid parameters.");
      return;
   }
   
   /* TODO: Memory allocation for all structures and tables for reducing memory usage.
    * And releasing all allocated memory after simulation */
   Global global;
   Ion ion[MAXIONS],*cur_ion; /* cur_ion is a pointer to the current ion */
   Target target;
   Scattering scat[MAXIONS][MAXELEMENTS];
   SNext snext;
   Detector detector;
   Potential pot;
   int i,j,nscat,primary_finished;
   char label[6];
   char file_label[NFILE];
   double fraction;
   int refresh_freq; /* How often progress bar (and GUI) is updated during simulation. */
                     /* (How many ions are calculated between updates) */
   
   /* Initializing file pointers */
   global.master.fperd = NULL;
   global.master.fpdat = NULL;
   global.master.fpout = NULL;
   global.master.fprange = NULL;
   
   global.master.fpdebug = NULL;
   
   /* Returning here when fatal error occurs */
   /* Close all open files and release allocated memory! */
   if(setjmp(err_buf)){
      error_dialog("Simulation failed.");
      close_files(&global);
      gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), 0.0);
      gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), NULL);
      return;
   }
   
   init_params(&global,ion,&target,&detector);
   
   read_input(&global,ion,&target,&detector);

   init_io(&global,ion,&target);
   
   make_screening_table(&pot);
         
   for(i=0;i<global.nions;i++){
      for(j=0;j<target.natoms;j++){
/*       scat[i][j].pot = &pot;  */
         scattering_table(&global,&(ion[i]),&target,&(scat[i][j]),&pot,j);
         calc_cross_sections(&global,&(scat[i][j]),&pot);
      }
   }

   init_detector(&global,&detector);

   print_data(&global,ion,&target,scat,&detector);

   if(global.predata)
      init_recoiling_angle(&global,ion,&target,&detector);
      
   /* Initializing progress bar */
   gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), "simulating..");
   refresh_freq = 100; /* How often GUI is updated during simulation.
                        * Too large value slows down the computer, but too small value "freezes" GUI. */
	
   for(global.cion=0;global.cion < global.nsimu;global.cion++){
                                 
      /* Progress bar */
      if(global.nsimu >= 100)
      {
         if(global.cion%((int) global.nsimu/refresh_freq) == 0)
         {
            fraction = (double) global.cion/global.nsimu;
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), fraction);
            while(gtk_events_pending())
               gtk_main_iteration();
         }
      }

      output_data(&global);

      cur_ion = ion + PRIMARY;
      create_ion(&global,cur_ion,&detector);

      primary_finished = FALSE;

      while(!primary_finished){
	
         next_scattering(&global,cur_ion,&target,scat,&snext);
         nscat = move_ion(&global,cur_ion,&target,&snext);

         if(nscat == ERD_SCATTERING){
            if(erd_scattering(&global,ion+PRIMARY,ion+SECONDARY,&target,&detector))
            	cur_ion = ion + SECONDARY;
         }

         if(cur_ion->status == FIN_RECOIL || cur_ion->status == FIN_OUT_DET){
            if(global.simstage == PRESIMULATION){
               finish_presimulation(&global,&detector,cur_ion);
               cur_ion = ion + PRIMARY;
            } else {
               move_to_erd_detector(&global,cur_ion,&target,&detector);
            }
         }

         if(global.simstage == PRESIMULATION && global.cion == (global.npresimu-1)){
            analyze_presimulation(&global,&target,&detector);
            init_recoiling_angle(&global,ion,&target,&detector);            
	 }

         if(nscat == MC_SCATTERING)
            mc_scattering(&global,cur_ion,&target,scat,&snext);

         if(ion_finished(&global,cur_ion,&target))
            primary_finished = TRUE;
/*
         if(cur_ion->status == FIN_DET){
            output_erd(&global,cur_ion,&target,&detector);
         }
*/
         if(cur_ion->status != NOT_FINISHED && cur_ion->tlayer >= target.ntarget){
            output_erd(&global,cur_ion,ion,&target,&detector);
         }


         if(cur_ion->type == SECONDARY && cur_ion->status != NOT_FINISHED){
            global.finstat[SECONDARY][cur_ion->status]++;
/*
            printf("scat: %8i %8i %8i\n",global.cion,ion[PRIMARY].nsct,
                   ion[SECONDARY].nsct);
*/
            cur_ion = ion + PRIMARY;
            primary_finished = FALSE;
         }
            
      }
      global.finstat[PRIMARY][cur_ion->status]++;
      finish_ion(&global,cur_ion,&target,&detector);
   }

   finalize(&global);
   
   /* Modifying file_path correct for get_espe */
   sprintf(label,".%u",nseed);
   strcpy(file_label, file_path);

   file_path = (char *)calloc(strlen(file_label) + strlen(label) + 1, sizeof(char));
   strcat(file_path, file_label);
   strcat(file_path, label);
   
   espe.ddist = FALSE; /* distribution points are not used yet */
   get_espe(NULL, NULL);
   
   /* Plotting simulation result */
   plot_espe(TYPE_SIMU, NULL);

   gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), 1.0);
   gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), "finished");

   /* Note that simulation is finished */
   output_dialog(file_path);
   
   /* Free memory for the next simulation!!?? */
     
}
