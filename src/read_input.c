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


/* File includes all functions for providing parameters from GUI to MCERD code */



#include "general.h"
#include "prototypes.h"
#include "read_input.h"
#include "protot.h"

/* Parameters from the GUI to the original MCERD code.
 * Making exactly same things than the original MCERD code.
 * Units for parameters used in GUI is changed to SI units */
void read_input(Global *global,Ion *ion,Target *target,Detector *detector)
{
   Point p1,p2,p3;
   double unit;
   double rec_dist_unit,M;
   int i,j;
   

   global->simtype = sim_type;
   
   get_ion(beam_ion,&(ion[PRIMARY].Z),&(ion[PRIMARY].A),&(ion[PRIMARY].I));
   ion[PRIMARY].type = PRIMARY;
   fprintf(stdout,"Beam ion: Z=%.0f, M=%.3f\n",ion[PRIMARY].Z,ion[PRIMARY].A/C_U);

   get_unit_value(U_ENERGY,&unit,C_MEV);
   global->E0 = beam_energy*unit;
   global->ionemax = 1.01*global->E0;
   
   /* Target */
   target_input(global,target,TYPE_TARGET);
   
   /* Detector */
   detector_input(global,detector,target);

   get_ion(recoil_atom,&(ion[SECONDARY].Z),&(ion[SECONDARY].A),&(ion[SECONDARY].I));
   ion[SECONDARY].type = SECONDARY;
   fprintf(stdout,"\nRecoil atom: Z=%.0f, M=%.3f\n",ion[SECONDARY].Z,ion[SECONDARY].A/C_U);
   fprintf(stdout,"Recoil atom isotopes:\n");
   for(i=0;i<ion[SECONDARY].I.n;i++)
      fprintf(stdout,"%10.3f %10.3f%%\n",ion[SECONDARY].I.A[i]/C_U,
              ion[SECONDARY].I.c[i]*100.0);
      if(ion[SECONDARY].I.n > 0)
         fprintf(stdout,"Most abundant isotope: %10.3f\n",ion[SECONDARY].I.Am/C_U);
      
   rec_dist_unit = C_NM;
   target->recmaxd = rdist_x[MAX_RDIST-1]*rec_dist_unit;
   target->effrecd = 0.0;
   for(i=0;i<MAX_RDIST;i++){
      target->recdist[i].x = rdist_x[i]*rec_dist_unit;
      target->recdist[i].y = rdist_y[i]*rec_dist_unit;
      if(i > 0 && (target->recdist[i-1].y > 0.0 || target->recdist[i].y > 0.0)){
         target->effrecd += (target->recdist[i].x - target->recdist[i-1].x);
      }
   }
   target->nrecdist = MAX_RDIST;

   get_unit_value(U_ANGLE,&unit,C_DEG);
   global->beamangle = PI/2.0 - target_angle*unit;

   global->bspot.x = 0.5*beam_size_x;
   global->bspot.y = 0.5*beam_size_y;
   get_unit_value(U_SIZE,&unit,C_KEV);
   global->bspot.x *= unit;
   global->bspot.y *= unit;            

   get_unit_value(U_ANGLE,&unit,C_DEG);
   global->minangle = min_angle*unit;

   get_unit_value(U_ENERGY,&unit,C_KEV);
   global->emin = min_energy*unit;

   global->nsimu = nions;

   global->npresimu = nions_presim;

   global->nrecave = aver_nions;

   global->seed = nseed;
   /* initializing random generator */
   rnd(0.0,1.0,RND_SEED,fabs(global->seed));

   global->recwidth = angle_width;

   if(use_presimu == TRUE){
      for(i=0;i<nlayers;i++){
            target->recpar[i].x = presimu_x[i]*C_DEG/C_NM;
            target->recpar[i].y = presimu_y[i]*C_DEG;
      }
      global->predata = TRUE;
   }
   
   /* case: MINSCAT */
   /*
   get_unit_value(U_ANGLE,&unit,C_DEG);
   global->costhetamin = cos(number*unit);
   */
   

   p1.x = p1.y = p1.z = 0.0;
   p2 = p1;
   p3 = p1;
   
   p2.y = 1.0;
   p3.x = sin(PI/2.0 - global->beamangle);
   p3.z = cos(PI/2.0 - global->beamangle);

   target->plane = get_plane_params(&p1,&p2,&p3);
      
   if(global->recwidth == REC_WIDE && global->npresimu > 0){
      global->npresimu = 0;
      error_dialog("Presimulation not needed with wide recoil angle scheme");
   }

   if(global->recwidth == REC_WIDE)
      global->predata = FALSE;

   if(global->predata){
      global->npresimu = 0;      
   }else if(global->npresimu == 0 && global->recwidth == REC_NARROW){
      error_dialog("No precalculated recoiling data in the narrow recoiling scheme");
      longjmp(err_buf,1);
   }

   if(global->npresimu > 0){
      global->cpresimu = 0;   
      global->simstage = PRESIMULATION;
     
      global->presimu = (Presimu *)
            malloc(global->npresimu * global->nrecave * 2 * sizeof(Presimu));
            
      if(global->presimu == NULL){
         error_dialog("Could not allocate enough memory for the presimulation");
         longjmp(err_buf,1);
      }
   }else{
      global->simstage = REALSIMULATION;      
   }

   target->ntarget = target->nlayers - detector->nfoils;

   if(detector->type == DET_TOF){
      detector->tdet[0] += target->ntarget;
      detector->tdet[1] += target->ntarget;
   }

   for(i=target->nlayers-1;i>target->ntarget;i--){
      target->layer[i].dlow = 0.0;
      target->layer[i].dhigh -= target->layer[i-1].dhigh;
   }
  
   global->nsimu += global->npresimu;

   M = 4.0*ion[PRIMARY].A*ion[SECONDARY].A/
          ipow2(ion[PRIMARY].A + ion[SECONDARY].A);

   if(global->simtype == SIM_ERD){
      global->costhetamax = sqrt(global->emin/(global->E0*M));
      global->costhetamin = 1.0;
   } else if(global->simtype == SIM_RBS){   
      if(ion[PRIMARY].A <= ion[SECONDARY].A){
         global->costhetamax = -1.0;
      } else {
         global->costhetamax = sqrt(1.0 - ipow2(ion[SECONDARY].A/ion[PRIMARY].A));
      }   
         /* costhetamax is calculated for average recoil mass! */
   }

/* We calculate ZBL- or SRIM-stopping powers for ion-target -combinations for which
   experimental stopping powers were not given and Yang-straggling for all */
   
/*
   fp = fopen("stoppings","w");
*/
   for(i=0;i<target->nlayers;i++){
      if(target->layer[i].sto[PRIMARY].stodiv < 0.0)
         calc_zblsto(global,ion+PRIMARY,target,i);
      calc_yangstragg(ion+PRIMARY,target,i);
      if(target->layer[i].sto[SECONDARY].stodiv < 0.0)
         calc_zblsto(global,ion+SECONDARY,target,i);
      calc_yangstragg(ion+SECONDARY,target,i);

/*
      fprintf(fp,"\n\"%.0f\"\n",ion[PRIMARY].Z);
      for(j=0;j<target->layer[i].sto[PRIMARY].n_sto;j++)
         fprintf(fp,"%14.5e %14.5e %14.5e\n",target->layer[i].sto[PRIMARY].vel[j],
             target->layer[i].sto[PRIMARY].sto[j]/C_KEV_NM,
             sqrt(target->layer[i].sto[PRIMARY].stragg[j]*100*C_NM)/C_KEV);
      fprintf(fp,"\n\"%.0f\"\n",ion[SECONDARY].Z);
      for(j=0;j<target->layer[i].sto[SECONDARY].n_sto;j++)
         fprintf(fp,"%14.5e %14.5e %14.5e\n",target->layer[i].sto[SECONDARY].vel[j],
             target->layer[i].sto[SECONDARY].sto[j]/C_KEV_NM,
             sqrt(target->layer[i].sto[SECONDARY].stragg[j]*100.0*C_NM)/C_KEV);
*/
   }

   if(global->simtype == SIM_RBS){
      ion[TARGET_ATOM] = ion[SECONDARY];
      ion[SECONDARY] = ion[PRIMARY];
      for(i=0;i<target->nlayers;i++){
         target->layer[i].sto[SECONDARY].n_sto = target->layer[i].sto[PRIMARY].n_sto;
         target->layer[i].sto[SECONDARY].stodiv = target->layer[i].sto[PRIMARY].stodiv;
         for(j=0;j<target->layer[i].sto[SECONDARY].n_sto;j++){
            target->layer[i].sto[SECONDARY].vel[j] = target->layer[i].sto[PRIMARY].vel[j];
            target->layer[i].sto[SECONDARY].sto[j] = target->layer[i].sto[PRIMARY].sto[j];
            target->layer[i].sto[SECONDARY].stragg[j] = target->layer[i].sto[PRIMARY].stragg[j];
         }
      }
   }

}

/* Target parameters from GUI to the original MCERD code. Same function used for target layers
 * and detector foils */
void target_input(Global *global,Target *target, int type)
{
   Fvalue *fval;
   double unit,Z,M,con[MAXELEMENTS],sumcon = 0.0;
   double d=0.0,vel_unit,sto_unit,density,minN=1.0e100,sumM,sumN;
   int i,j,k,n=0,natoms,nlayer,max,layers;
   
   natoms = target->natoms;
   nlayer = target->nlayers;
   
   /* Different parameters used (in the GUI) in the case of foil or target */
   if(type == TYPE_TARGET){
      max = MAX_ELEMENTS;
      layers = nlayers;
   }
   else if(type == TYPE_FOIL){
      max = MAX_FELEMENTS;
      layers = nfoils;
   }
   
   get_unit_value(U_DENSITY,&unit,C_G_CM3);
   
   /* Handling all elements and some calculations with them */
   switch(type){
      case TYPE_TARGET:
         for(i = 0; i < layers; i++){
            k = 0; /* k is the index for each layers atoms */
            sumM = 0.0;
            sumcon = 0.0;
            density = layer_density[i]*unit;
            
            printf("\nTarget layer %d:\n", i+1);
            
            for(j = 0; j < max; j++){
               if( elem_layer[j][i] != NULL && strcmp(elem_layer[j][i], "") ){
                  M = mass_layer[j][i];
                  M *= C_U;
                  get_atom(elem_layer[j][i],&Z);
                  target->ele[natoms].Z = Z;
                  target->ele[natoms].A = M;
                  
                  con[k] = con_layer[j][i];
                  target->layer[i+nlayer].atom[k] = natoms; /* Now natoms is the index for all atoms */
                  sumM += target->ele[natoms].A*con[k];
                  sumcon += con[k];
         
                  natoms++;
                  k++;
                  
                  printf("Atom %d: %5.4f %f\n", k, Z, M/C_U);
               }
            }
            sumM /= sumcon;
            target->layer[i+nlayer].natoms = k;
            sumN = density/sumM;
            target->layer[i+nlayer].Ntot  = sumN;
            for(j=0;j<k;j++){
               con[j] /= sumcon;
               target->layer[i+nlayer].N[j] = con[j]*sumN;
            }
         }
         break;
      case TYPE_FOIL:
         for(i = 0; i < layers; i++){
            k = 0; /* k is index for each layers atom */
            sumM = 0.0;
            sumcon = 0.0;
            density = foil_density[i]*unit;
            
            printf("\nTarget foil %d:\n", i+1);
            
            for(j = 0; j < max; j++){
               if( elem_foil[j][i] != NULL && strcmp(elem_foil[j][i], "") ){
                  M = mass_foil[j][i];
                  M *= C_U;
                  get_atom(elem_foil[j][i],&Z);
                  target->ele[natoms].Z = Z;
                  target->ele[natoms].A = M;
                  
                  con[k] = con_foil[j][i];
                  target->layer[i+nlayer].atom[k] = natoms; /* Now natoms is the index for all atoms */
                  sumM += target->ele[natoms].A*con[k];
                  sumcon += con[k];
         
                  natoms++;
                  k++;
                  
                  printf("Atom %d: %5.4f %f\n", k, Z, M/C_U);
               }
            }
            sumM /= sumcon;
            target->layer[i+nlayer].natoms = k;
            sumN = density/sumM;
            target->layer[i+nlayer].Ntot  = sumN;
            for(j=0;j<k;j++){
               con[j] /= sumcon;
               target->layer[i+nlayer].N[j] = con[j]*sumN;
            }
         }
         break;
   }
   
   target->natoms = natoms;
   
   for(i = 0; i < layers; i++){
      target->layer[i+nlayer].type = TARGET_FILM;
      target->layer[i+nlayer].gas = FALSE;
   }
   
   get_unit_value(U_THICKNESS,&unit,C_NM);
      
   switch(type){
      case TYPE_TARGET:
         for(i = 0; i < layers; i++){
            if(layer_thickness[i] < 0){
               target->layer[i+nlayer].type = TARGET_BULK;
               target->layer[i+nlayer].dlow = d;            
               target->layer[i+nlayer].dhigh = 1.0;  /* 1.0 meters */
            }
            if(target->layer[i+nlayer].type == TARGET_FILM){
               target->layer[i+nlayer].dlow = d;
               target->layer[i+nlayer].dhigh = d + layer_thickness[i]*unit;
               d = target->layer[i+nlayer].dhigh;
            }
         }
         break;
      case TYPE_FOIL:
         for(i = 0; i < layers; i++){
            if(foil_thickness[i] < 0){
               target->layer[i+nlayer].type = TARGET_BULK;
               target->layer[i+nlayer].dlow = d;            
               target->layer[i+nlayer].dhigh = 1.0;  /* 1.0 meters */
            }
            if(target->layer[i+nlayer].type == TARGET_FILM){
               target->layer[i+nlayer].dlow = d;
               target->layer[i+nlayer].dhigh = d + foil_thickness[i]*unit;
               d = target->layer[i+nlayer].dhigh;
            }
         }
         break;
   }
      
      
   /* First stoppings for ion (when ion_num is 1 or 2) */
   switch(type){
      case TYPE_TARGET:
         for(i = 0; i < layers; i++){
            if(!strcmp(stop_ion_layer[i],"ZBL"))
               target->layer[i+nlayer].sto[0].stodiv = STOP_ZBL; /* flag to calculate later */
            else if(!strcmp(stop_ion_layer[i],"SRIM"))
               target->layer[i+nlayer].sto[0].stodiv = STOP_SRIM; /* flag to calculate later */
            else{
               fval = read_file(stop_ion_layer[i],2,&n);
               if(fval == NULL){
                  error_dialog("Reading the stopping power file failed");
                  longjmp(err_buf,1);
               }
               target->layer[i+nlayer].sto[0].n_sto = n;
               vel_unit = C_V0;
               sto_unit = C_KEV_NM;
               for(j=0;j<n;j++){
                  target->layer[i+nlayer].sto[0].vel[j] = fval->a[j]*vel_unit;
                  target->layer[i+nlayer].sto[0].sto[j] = fval->b[j]*sto_unit;
               }
               target->layer[i+nlayer].sto[0].stodiv = 1.0/(target->layer[i+nlayer].sto[0].vel[1] - target->layer[i+nlayer].sto[0].vel[0]);
               free(fval);
            }
         }
         break;
      case TYPE_FOIL:
         for(i = 0; i < layers; i++){
            if(!strcmp(stop_ion_foil[i],"ZBL"))
               target->layer[i+nlayer].sto[0].stodiv = STOP_ZBL; /* flag to calculate later */
            else if(!strcmp(stop_ion_foil[i],"SRIM"))
               target->layer[i+nlayer].sto[0].stodiv = STOP_SRIM; /* flag to calculate later */
            else{
               fval = read_file(stop_ion_foil[i],2,&n);
               if(fval == NULL){
                  error_dialog("Reading the stopping power file failed");
                  longjmp(err_buf,1);
               }
               target->layer[i+nlayer].sto[0].n_sto = n;
               vel_unit = C_V0;
               sto_unit = C_KEV_NM;
               for(j=0;j<n;j++){
                  target->layer[i+nlayer].sto[0].vel[j] = fval->a[j]*vel_unit;
                  target->layer[i+nlayer].sto[0].sto[j] = fval->b[j]*sto_unit;
               }
               target->layer[i+nlayer].sto[0].stodiv = 1.0/(target->layer[i+nlayer].sto[0].vel[1] - target->layer[i+nlayer].sto[0].vel[0]);
               free(fval);
            }
         }
         break;
   }
   
   /* Stoppings for recoils (ion_num is 2) */
   if(ion_num == 2){
      switch(type){
         case TYPE_TARGET:
            for(i = 0; i < layers; i++){
               if(!strcmp(stop_rec_layer[i],"ZBL"))
                  target->layer[i+nlayer].sto[1].stodiv = STOP_ZBL; /* flag to calculate later */
               else if(!strcmp(stop_rec_layer[i],"SRIM"))
                  target->layer[i+nlayer].sto[1].stodiv = STOP_SRIM; /* flag to calculate later */
               else{
                  fval = read_file(stop_rec_layer[i],2,&n);
                  if(fval == NULL){
                     error_dialog("Reading the stopping power file failed");
                     longjmp(err_buf,1);
                  }
                  target->layer[i+nlayer].sto[1].n_sto = n;
                  vel_unit = C_V0;
                  sto_unit = C_KEV_NM;
                  for(j=0;j<n;j++){
                     target->layer[i+nlayer].sto[1].vel[j] = fval->a[j]*vel_unit;
                     target->layer[i+nlayer].sto[1].sto[j] = fval->b[j]*sto_unit;
                  }
                  target->layer[i+nlayer].sto[1].stodiv = 1.0/(target->layer[i+nlayer].sto[1].vel[1] - target->layer[i+nlayer].sto[1].vel[0]);
                  free(fval);
               }
            }
            break;
         case TYPE_FOIL:
            for(i = 0; i < layers; i++){
               if(!strcmp(stop_rec_foil[i],"ZBL"))
                  target->layer[i+nlayer].sto[1].stodiv = STOP_ZBL; /* flag to calculate later */
               else if(!strcmp(stop_rec_foil[i],"SRIM"))
                  target->layer[i+nlayer].sto[1].stodiv = STOP_SRIM; /* flag to calculate later */
               else{
                  fval = read_file(stop_rec_foil[i],2,&n);
                  if(fval == NULL){
                     error_dialog("Reading the stopping power file failed");
                     longjmp(err_buf,1);
                  }
                  target->layer[i+nlayer].sto[1].n_sto = n;
                  vel_unit = C_V0;
                  sto_unit = C_KEV_NM;
                  for(j=0;j<n;j++){
                     target->layer[i+nlayer].sto[1].vel[j] = fval->a[j]*vel_unit;
                     target->layer[i+nlayer].sto[1].sto[j] = fval->b[j]*sto_unit;
                  }
                  target->layer[i+nlayer].sto[1].stodiv = 1.0/(target->layer[i+nlayer].sto[1].vel[1] - target->layer[i+nlayer].sto[1].vel[0]);
                  free(fval);
               }
            }
            break;
      }
   }

   target->nlayers += layers;
   
   for(i=0;i<target->nlayers;i++){
      if(target->layer[i].Ntot < minN){
         target->minN = target->layer[i].Ntot;
         minN = target->minN;
      }
   }

   if(type == TYPE_TARGET)
   {
      printf("\nNumber of target layers: %d\n", target->nlayers);
      printf("Number of atoms: %d\n", target->natoms);
   }
   
   if(type == TYPE_FOIL)
   {
      printf("\nTotal number of layers: %d\n", target->nlayers);
      printf("Total number of atoms: %d\n", target->natoms);
   }
   
}

/* Detector parameters from GUI to the original MCERD code */
void detector_input(Global *global,Detector *detector,Target *target)
{
     
   int i;
   double unit;

   global->virtualdet = FALSE;
   
   detector->type = det_type;
   
   get_unit_value(U_ANGLE,&unit,C_DEG);
   detector->angle = det_angle*unit;
   
   detector->vsize[0] = vdet_size1;
   detector->vsize[1] = vdet_size2;
   
   /* indexes for timing foils. tdet[] uses indexes starting from 0, but for det_num indexes starts from 1 */
   detector->tdet[0] = det_num1 - 1;
   detector->tdet[1] = det_num2 - 1;

   get_unit_value(U_SIZE,&unit,C_MM);
      
   for(i=0;i<nfoils;i++){
                         
      detector->foil[i].type = foil_type[i];
      
      if(detector->foil[i].type == FOIL_CIRC){
         detector->foil[i].size[0] = 0.5*foil_diameter[0][i]*unit; /* We use radius! */
      }else if(detector->foil[i].type == FOIL_RECT){
         detector->foil[i].size[0] = 0.5*foil_diameter[1][i]*unit;
         detector->foil[i].size[1] = 0.5*foil_diameter[2][i]*unit;
      }
      
      detector->foil[i].dist = foil_distance[i]*unit;
      
   }
      
   detector->nfoils = nfoils;
   
   if(detector->vsize[0] > 1.0 && detector->vsize[1] > 1.0){
      global->virtualdet = TRUE;
   }
   
   /* Detector foils */
   target_input(global,target,TYPE_FOIL);
   
}
   
   
   
