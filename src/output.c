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

void output_erd(Global *global,Ion *cur_ion,Ion *ion,Target *target,
                Detector *detector)
{
   int i;

   switch(detector->type){
      case DET_TOF:
         if(cur_ion->type == SECONDARY && cur_ion->tlayer == target->nlayers){
#ifdef SCAT_ANAL
            for(i=0;i<ion[PRIMARY].nsct;i++){
               fprintf(global->master.fperd,"P %.4f\n",ion[PRIMARY].sct_angle[i]/C_DEG);
            }
            for(i=0;i<ion[SECONDARY].nsct;i++){
               fprintf(global->master.fperd,"S %.4f\n",ion[SECONDARY].sct_angle[i]/C_DEG);
            }
#endif         
            if(cur_ion->virtual)
               fprintf(global->master.fperd,"V ");
            else
               fprintf(global->master.fperd,"R ");
            fprintf(global->master.fperd,"%10.4f ",cur_ion->E/C_MEV);
            fprintf(global->master.fperd,"%10.4f ",cur_ion->A/C_U);
            fprintf(global->master.fperd,"%10.4f ",cur_ion->hist.tar_recoil.p.z/C_NM);
            fprintf(global->master.fperd,"%15.7e ",cur_ion->w);
            fprintf(global->master.fperd,"%8.2f ",cur_ion->hit.x/C_MM);
            fprintf(global->master.fperd,"%8.2f ",cur_ion->hit.y/C_MM);
            fprintf(global->master.fperd,"%10.3f",(cur_ion->dt[1] - cur_ion->dt[0])/C_NS);
#ifdef REC_ANG
            fprintf(global->master.fperd," %10.3f",cur_ion->rec_ion_theta);
            fprintf(global->master.fperd," %10.3f",cur_ion->rec_ion_fii);
            fprintf(global->master.fperd," %10.3f",cur_ion->rec_lab_theta);
            fprintf(global->master.fperd," %10.3f",cur_ion->rec_lab_fii);
#endif
            fprintf(global->master.fperd,"\n");
         }
         break;
      case DET_GAS:
         if(cur_ion->type == SECONDARY && cur_ion->tlayer > (target->ntarget+1)){
            if(cur_ion->virtual)
               fprintf(global->master.fperd,"V ");
            else
               fprintf(global->master.fperd,"R ");
            fprintf(global->master.fperd,"%10.4f ",cur_ion->E/C_MEV);
            fprintf(global->master.fperd,"%10.4f ",cur_ion->A/C_U);
            fprintf(global->master.fperd,"%10.4f ",cur_ion->hist.tar_recoil.p.z/C_NM);
            fprintf(global->master.fperd,"%15.7e ",cur_ion->w);
            fprintf(global->master.fperd,"%8.2f ",cur_ion->hit.x/C_MM);
            fprintf(global->master.fperd,"%8.2f ",cur_ion->hit.y/C_MM);
            for(i=0;i<(target->nlayers - target->ntarget - 1);i++){
               fprintf(global->master.fperd,"%8.4f ",(cur_ion->Ed[i] - cur_ion->Ed[i+1])/C_MEV);
            }
            fprintf(global->master.fperd,"\n");
         }
         break;
   }

   fflush(global->master.fperd);

}
void output_data(Global *global)
{
   int nion,nmaxion;
   
   if(global->simstage == PRESIMULATION){
      nion = global->cion;
      nmaxion = global->npresimu;
   } else {
      nion = global->cion - global->npresimu;
      nmaxion = global->nsimu - global->npresimu;
   }

   if(nmaxion > 100){
      if(nion%(nmaxion/100) == 0)
         fprintf(stderr,"Calculated %i of %i ions (%.0f%%)\n",
                 nion,nmaxion,100.0*nion/nmaxion);
         fflush(stderr);
   }

}


