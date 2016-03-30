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


/* Reading all parameters from files. Using same methods than original MCERD */


#include "general.h"
#include "prototypes.h"
#include "read_input.h"
#include "protot.h"

/* File parameters */
char *file_path;

/* Misc parameters */
int error;

/* Simulation parameters */
int sim_type;
const char *beam_ion;
double beam_energy;
const char *recoil_atom;
double target_angle;
double beam_size_x;
double beam_size_y;
double min_angle;
double min_energy;
int aver_nions;
int angle_width;
int nions;
int nions_presim;
unsigned int nseed;

/* Detector parameters */
int det_type;
double det_angle;
double vdet_size1;
double vdet_size2;
int det_num1;
int det_num2;

/* Target parameters */
int nlayers;
double layer_thickness[MAX_LAYERS];
double layer_density[MAX_LAYERS];
const char *stop_ion_layer[MAX_LAYERS];
const char *stop_rec_layer[MAX_LAYERS];
double mass_layer[MAX_ELEMENTS][MAX_LAYERS];
const char *elem_layer[MAX_ELEMENTS][MAX_LAYERS];
double con_layer[MAX_ELEMENTS][MAX_LAYERS];

/* Foil parameters */
int nfoils;
int foil_type[MAX_FOILS];
double foil_diameter[3][MAX_FOILS];
double foil_distance[MAX_FOILS];
const char *stop_ion_foil[MAX_FOILS];
const char *stop_rec_foil[MAX_FOILS];
const char *elem_foil[MAX_FELEMENTS][MAX_FOILS];
double con_foil[MAX_FELEMENTS][MAX_FOILS];
double mass_foil[MAX_FELEMENTS][MAX_FOILS];
double foil_thickness[MAX_FOILS];
double foil_density[MAX_FOILS];

/* Recoil material distribution parameters */
double rdist_x[MAX_RDIST];
double rdist_y[MAX_RDIST];

/* Pre simulation parameters */
double presimu_x[MAX_PRESIMU];
double presimu_y[MAX_PRESIMU];
int use_presimu;

/* get_espe parameters */
Espe espe;

/* Read all parameters from the files */
void read_param(char *file_name)
{
   FILE *fp;
   char buf[MAXLEN],*c,*word;
   TextLine *line;
   double number;
   int i,n,rinput[NINPUT];
   
   for(i=0;i<NINPUT;i++)
      rinput[i] = FALSE; 
   
   error = 0;

   line = NULL;

   fp = fopen(file_name,"r");
      if(fp == NULL){
         error_dialog_open(file_name);
         return;
      }

   while(fgets(buf,MAXLEN,fp)!=NULL){
      i = 0;
      while(i<NINPUT && (c=strstr(buf,inlines[i]))==NULL)
         i++;
      
      if(i<NINPUT){
         c += strlen(inlines[i]);
         c = trim_space(c);
      }

      switch(i){
         case I_TYPE:
            if(c != NULL && strcmp(c, "")){
               word = get_word(c,&n);  /* we should free word-memory also! */
               if(strcmp(word,"ERD") == 0)
                  sim_type = SIM_ERD;
               else if(strcmp(word,"RBS") == 0)
                  sim_type = SIM_RBS;
 	           else{
 	              sim_type = SIM_ERD;
                  error = 1;
               }
               free(word);
            }
            else{
               sim_type = SIM_ERD;
               error = 1;
            }
            break;
         case  I_ION:
            if(c != NULL){
	      if(strcmp(c, "")){
                 c = get_string(c);
                 beam_ion = c;
	      }
	      else
	         beam_ion = "";
            }
            else{
               beam_ion = "";
               error = 1;
            }
            break;
         case  I_ENERGY:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               beam_energy = number;
            }
            else{
               beam_energy = 0;
               error = 1;
            }
            break;
         case  I_RECOIL:
            if(c != NULL){
               if(strcmp(c, "")){
                  c = get_string(c);
                  recoil_atom = c;
	       }
	       else
	          recoil_atom = "";
            }
            else{
               recoil_atom = "";
               error = 1;
            }
            break;
         case I_TANGLE:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               target_angle = number;
            }
            else{
               target_angle = 0;
               error = 1;
            }
            break;
         case I_SPOTSIZE:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               beam_size_x = number;
            }
            else{
               beam_size_x = 0;
               error = 1;
            }
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               beam_size_y = number;
            }
            else{
               beam_size_y = 0;
               error = 1;
            }
            break;
         case I_MINANGLE:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               min_angle = number;
            }
            else{
               min_angle = 0;
               error = 1;
            }
            break;
         case I_MINENERGY:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               min_energy = number;
            }
            else{
               min_energy = 0;
               error = 1;
            }
            break;
         case I_NIONS:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               nions = number;
            }
            else{
               nions = 0;
               error = 1;
            }
            break;
         case I_NPREIONS:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               nions_presim = number;
            }
            else{
               nions_presim = 0;
               error = 1;
            }
            break;
         case I_RECAVE:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               aver_nions = number;
            }
            else{
               aver_nions = 0;
               error = 1;
            }
            break;
         case I_SEED:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               nseed = number;
            }
            else{
               nseed = 0;
               error = 1;
            }
            break;
         case I_RECWIDTH:
            if(c != NULL && strcmp(c, "")){
               word = get_word(c,&n);
               if(strcmp(word,"wide") == 0 || strcmp(word,"WIDE") == 0)
                  angle_width = REC_WIDE;
               else if(strcmp(word,"narrow") == 0 || strcmp(word,"NARROW") == 0)
                  angle_width = REC_NARROW;
               else{
                  angle_width = REC_NARROW;
                  error = 1;
               }
               free(word);
            }
            else{
               angle_width = REC_NARROW;
               error = 1;
            }
            break;
         default:
            break;
      }
   }
   
   read_target_file(file_name, TYPE_TARGET);
   read_target_file(file_name, TYPE_FOIL);
   read_detector_file(file_name);
   read_rdist_file(file_name);
   read_presimu_file(file_name);
   read_espe_params_file(file_name);

  /* If error occurs somewhere */
  if(error)
    error_dialog("Problems with reading the data");
    
  fclose(fp);
  
  /* Clean "memory" */
  format_layers(nlayers);
  format_foils(nfoils);
    
  saved = 1;
}

/* Read recoil distribution */
void read_rdist_file(char *fname)
{
   FILE *fp;
   char buf[LINE];
   char *fcomb;
   int c,i,n;
   
   fcomb = (char *)calloc( strlen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL )) + strlen(FRECOIL) + 1, sizeof(char) );
   strcat(fcomb, g_locale_from_utf8( fname, -1, NULL, NULL, NULL ));
   strcat(fcomb, FRECOIL);
   
   fp = fopen(fcomb,"r");
   
   if(fp == NULL){
      error_dialog_open(fcomb);
      free(fcomb);
      return;
   }
   
   i=0;
   while(fgets(buf,LINE,fp) != NULL && i < MAX_RDIST){
      c = sscanf(buf,"%lf %lf",&(rdist_x[i]),&(rdist_y[i]));
      if(c != 2){
         rdist_x[i] = 0;
         rdist_y[i] = 0;
         error = 1;
      }
      i++;
   }
   fclose(fp);
   free(fcomb);
   
   /* Clean "memory" */
   for(n = i; n < MAX_RDIST; n++){
      rdist_x[n] = 0;
      rdist_y[n] = 0;
   }
}

/* Read pre simulation parameters */
void read_presimu_file(char *fname)
{
   FILE *fp;
   char buf[LINE];
   char *fcomb;
   int c,i,n;
   
   fcomb = (char *)calloc( strlen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL )) + strlen(FPRE) + 1, sizeof(char) );
   strcat(fcomb, g_locale_from_utf8( fname, -1, NULL, NULL, NULL ));
   strcat(fcomb, FPRE);
   
   fp = fopen(fcomb,"r");
   
   if(fp == NULL){
      error_dialog_open(fcomb);
      free(fcomb);
      return;
   }
   
   fgets(buf,LINE,fp);
   c = sscanf(buf,"%d",&(use_presimu));
   i=0;
   while(fgets(buf,LINE,fp) != NULL && i < MAX_PRESIMU){
      c = sscanf(buf,"%lf %lf",&(presimu_x[i]),&(presimu_y[i]));
      if(c != 2){
         presimu_x[i] = 0;
         presimu_y[i] = 0;
         error = 1;
      }
      i++;
   }
   fclose(fp);
   free(fcomb);
   
   /* clean "memory" */
   for(n = i; n < MAX_PRESIMU; n++){
      presimu_x[n] = 0;
      presimu_y[n] = 0;
   }
}

/* Read detector parameters */
void read_detector_file(char *fname)
{
   FILE *fp;
   char dtype[10],ftype[20],buf[LINE];
   char *fcomb;
   int c,n=0;
   
   fcomb = (char *)calloc( strlen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL )) + strlen(FDETECTOR) + 1, sizeof(char) );
   strcat(fcomb, g_locale_from_utf8( fname, -1, NULL, NULL, NULL ));
   strcat(fcomb, FDETECTOR);

   fp = fopen(fcomb,"r");

   if(fp == NULL){
      error_dialog_open(fcomb);
      free(fcomb);
      return;
   }
   
   c = fscanf(fp,"Detector type: %s\n",dtype);
   if(c != 1){
      det_type = DET_TOF;
      error = 1;
   }
   else{
      if(strcmp(dtype,"TOF") == 0)
         det_type = DET_TOF;
      else if(strcmp(dtype,"GAS") == 0)
         det_type = DET_GAS;
      else{
         det_type = DET_TOF;
         error = 1;
      }
   }

   c = fscanf(fp,"Detector angle: %lf\n",&(det_angle));
   if(c != 1){
      det_angle = 0;
      error = 1;
   }

   c = fscanf(fp,"Virtual detector size: %lf %lf\n",&(vdet_size1),&(vdet_size2));
   if(c != 2){
      vdet_size1 = 0;
      vdet_size2 = 0;
      error = 1;
   }

   c = fscanf(fp,"Timing detector numbers: %i %i\n",&(det_num1),&(det_num2));
   if(c != 2){
      det_num1 = 0;
      det_num2 = 0;
      error = 1;
   }

   while(fgets(buf,LINE,fp) != NULL){
      c = fscanf(fp,"Foil type: %s\n",ftype);
      if(c != 1){
         foil_type[n] = FOIL_CIRC;
         error = 1;
      }
      else{
         if(strcmp(ftype,"circular") == 0)
            foil_type[n] = FOIL_CIRC;
         else if(strcmp(ftype,"rectangular") == 0)
            foil_type[n] = FOIL_RECT;
         else{
            foil_type[n] = FOIL_CIRC;
            error = 1;
         }
      }
      
      if(foil_type[n] == FOIL_CIRC) {
         c = fscanf(fp,"Foil diameter: %lf\n",&(foil_diameter[0][n]));
         if(c != 1){
            foil_diameter[0][n] = 0;
            error = 1;
         }
      }
      else if(foil_type[n] == FOIL_RECT) {
         c = fscanf(fp,"Foil size: %lf %lf\n",&(foil_diameter[1][n]),&(foil_diameter[2][n]));
         if(c != 2){
            foil_diameter[1][n] = 0;
            foil_diameter[2][n] = 0;
            error = 1;
         }
      }
      c = fscanf(fp,"Foil distance: %lf\n",&(foil_distance[n]));
      if(c != 1){
         foil_distance[n] = 0;
         error = 1;
      }
      
      n++;
   }

   nfoils = n;
  
   fclose(fp);
   free(fcomb);

}

/* Read target parameters */
void read_target_file(char *fname, int type)
{
   FILE *fp;
   char buf[LINE],*c;
   char *fcomb;
   double number;
   int i,j,n=0,nlayer,natoms;
   
   char *elements[MAX_LAYERS * MAX_ELEMENTS];
   double masses[MAX_LAYERS * MAX_ELEMENTS];
   natoms = 0;
   nlayer = 0;
   
   switch(type){
      case TYPE_TARGET:
         fcomb = (char *)calloc( strlen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL )) + strlen(FTARGET) + 1, sizeof(char) );
         strcat(fcomb, g_locale_from_utf8( fname, -1, NULL, NULL, NULL ));
         strcat(fcomb, FTARGET);
         break;
      case TYPE_FOIL:
         fcomb = (char *)calloc( strlen(g_locale_from_utf8( fname, -1, NULL, NULL, NULL )) + strlen(FFOIL) + 1, sizeof(char) );
         strcat(fcomb, g_locale_from_utf8( fname, -1, NULL, NULL, NULL ));
         strcat(fcomb, FFOIL);
         break;
   }

   fp = fopen(fcomb,"r");
   
   if(fp == NULL){
      error_dialog_open(fcomb);
      free(fcomb);
      return;
   }

   while(fgets(buf,LINE,fp) != NULL && line_not_empty(buf)){
      c = buf;
      if(c != NULL && strcmp(c, "")){
         c = get_number(c,&number);
         masses[natoms] = number;
      }
      else{
         masses[natoms] = 0;
         error = 1;
      }
      fgets(buf,LINE,fp);
      c = buf;
      if(c != NULL && strcmp(c, ""))
         elements[natoms] = get_word(c,&n);
      else{
         elements[natoms] = "";
         error = 1;
      }
         
      natoms++;
   }
   
   while(fgets(buf,LINE,fp) != NULL){
      c = buf;
      switch(type){
         case TYPE_TARGET:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               layer_thickness[nlayer] = number;
            }
            else{
               layer_thickness[nlayer] = 0;
               error = 1;
            }
            break;
         case TYPE_FOIL:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               foil_thickness[nlayer] = number;
            }
            else{
               foil_thickness[nlayer] = 0;
               error = 1;
            }
            break;
      }
      
      /* First handling ion (when ion_num is 1 or 2) */
      fgets(buf,LINE,fp);
      c = buf;
      switch(type){
        case TYPE_TARGET:
          if(c != NULL && strcmp(c, ""))
            stop_ion_layer[nlayer] = get_word(c,&n);
          else{
            stop_ion_layer[nlayer] = "";
            error = 1;
          }
          break;
        case TYPE_FOIL:
          if(c != NULL && strcmp(c, ""))
            stop_ion_foil[nlayer] = get_word(c,&n);
          else{
            stop_ion_foil[nlayer] = "";
            error = 1;
          }
          break;
      }
      
      /* Then recoil (when ion_num = 2) */
      if(ion_num == 2){
         fgets(buf,LINE,fp);
         c = buf;
         switch(type){
            case TYPE_TARGET:
               if(c != NULL && strcmp(c, ""))
                  stop_rec_layer[nlayer] = get_word(c,&n);
               else{
                  stop_rec_layer[nlayer] = "";
                  error = 1;
               }
               break;
            case TYPE_FOIL:
               if(c != NULL && strcmp(c, ""))
                  stop_rec_foil[nlayer] = get_word(c,&n);
               else{
                  stop_rec_foil[nlayer] = "";
                  error = 1;
               }
               break;
         }
      }
                                
      fgets(buf,LINE,fp); 
      c = buf;
      switch(type){
         case TYPE_TARGET:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               layer_density[nlayer] = number;
            }
            else{
               layer_density[nlayer] = 0;
               error = 1;
            }
            break;
         case TYPE_FOIL:
            if(c != NULL && strcmp(c, "")){
               c = get_number(c,&number);
               foil_density[nlayer] = number;
            }
            else{
               foil_density[nlayer] = 0;
               error = 1;
            }
            break;
      }

      i = 0;
      while(fgets(buf,LINE,fp) != NULL && line_not_empty(buf)){
         c = buf;
         switch(type){
            case TYPE_TARGET:
               if(c != NULL && strcmp(c, "")){
                  c = get_number(c,&number);
                  mass_layer[i][nlayer] = masses[(int)number];
                  elem_layer[i][nlayer] = elements[(int)number];
               }
               else{
                  mass_layer[i][nlayer] = 0;
                  elem_layer[i][nlayer] = "";
                  error = 1;
               }
               fgets(buf,LINE,fp);
               c = buf;
               if(c != NULL && strcmp(c, "")){
                  c = get_number(c,&number);
                  con_layer[i][nlayer] = number;
               }
               else{
                  con_layer[i][nlayer] = 0;
                  error = 1;
               }
               break;
            case TYPE_FOIL:
               if(c != NULL && strcmp(c, "")){
                  c = get_number(c,&number);
                  mass_foil[i][nlayer] = masses[(int)number];
                  elem_foil[i][nlayer] = elements[(int)number];
               }
               else{
                  mass_foil[i][nlayer] = 0;
                  elem_foil[i][nlayer] = "";
                  error = 1;
               }
               fgets(buf,LINE,fp);
               c = buf;
               if(c != NULL && strcmp(c, "")){
                  c = get_number(c,&number);
                  con_foil[i][nlayer] = number;
               }
               else{
                  con_foil[i][nlayer] = 0;
                  error = 1;
               }
               break;
         }

         i++;
      }
      
      /* clean "memory" */
      switch(type){
         case TYPE_TARGET:
            for(j=i;j<MAX_ELEMENTS;j++){
               mass_layer[j][nlayer] = 0;
               elem_layer[j][nlayer] = "";
               con_layer[j][nlayer] = 0;
            }
            break;
         case TYPE_FOIL:
            for(j=i;j<MAX_FELEMENTS;j++){
               mass_foil[j][nlayer] = 0;
               elem_foil[j][nlayer] = "";
               con_foil[j][nlayer] = 0;
            }
            break;
      }

      nlayer++; 
   }
   if(type == TYPE_TARGET)
      nlayers = nlayer;
   
   fclose(fp);
   free(fcomb);
   
}

/* Read energy spectrum parameters */
void read_espe_params_file(char *file_name)
{
   FILE *fp;
   char *fcomb;
   char buf[LINE];
   int c;
   
   fcomb = (char *)calloc( strlen(g_locale_from_utf8( file_name, -1, NULL, NULL, NULL )) + strlen(FESPE) + 1, sizeof(char) );
   strcat(fcomb, g_locale_from_utf8( file_name, -1, NULL, NULL, NULL ));
   strcat(fcomb, FESPE);
   
   fp = fopen(fcomb,"r");
   
   if(fp == NULL){
      error_dialog_open(fcomb);
      free(fcomb);
      return;
   }
   
   fgets(buf,LINE,fp);
   c = sscanf(buf,"%i %lf %i",&(espe.virtual),&(espe.vsize),&(espe.vnum));
   if(c!=3){
      espe.virtual = FALSE;
      espe.vsize = 0;
      espe.vnum = 0;
      error = 1;
   }
   fgets(buf,LINE,fp);
   c = sscanf(buf,"%i",&(espe.wide));
   if(c!=1){
      espe.wide = FALSE;
      error = 1;
   }
   fgets(buf,LINE,fp);
   c = sscanf(buf,"%i %lf %lf",&(espe.timeres),&(espe.tres),&(espe.eres));
   if(c!=3){
      espe.timeres = TRUE;
      espe.tres = 0;
      espe.eres = 0;
      error = 1;
   }
   fgets(buf,LINE,fp);
   c = sscanf(buf,"%lf",&(espe.toflen));
   if(c!=1){
      espe.toflen = 0;
      error = 1;
   }
   fgets(buf,LINE,fp);
   c = sscanf(buf,"%lf",&(espe.chwidth));
   if(c!=1){
      espe.chwidth = 0;
      error = 1;
   }
   fgets(buf,LINE,fp);
   c = sscanf(buf,"%lf",&(espe.scale));
   if(c!=1){
      espe.scale = 0;
      error = 1;
   }
   fgets(buf,LINE,fp);
   c = sscanf(buf,"%i %lf %lf",&(espe.dsize),&(espe.dx),&(espe.dy));
   if(c!=3){
      espe.dsize = FALSE;
      espe.dx = 0;
      espe.dy = 0;
      error = 1;
   }
   fgets(buf,LINE,fp);
   c = sscanf(buf,"%i %lf %lf",&(espe.dwindow),&(espe.dmin),&(espe.dmax));
   if(c!=3){
      espe.dwindow = FALSE;
      espe.dmin = 0;
      espe.dmax = 0;
      error = 1;
   }
   fgets(buf,LINE,fp);
   c = sscanf(buf,"%i %lf",&(espe.mass),&(espe.mass_value));
   if(c!=2){
      espe.mass = FALSE;
      espe.mass_value = 0;
      error = 1;
   }
   fgets(buf,LINE,fp);
   c = sscanf(buf,"%i",&(espe.pnvd));
   if(c!=1){
      espe.pnvd = FALSE;
      error = 1;
   }
   
   fclose(fp);
   free(fcomb);
   
}
