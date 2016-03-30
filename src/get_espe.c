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


/* File includes functions for generating energy spectrum from *.erd file */


#include "general.h"
#include "prototypes.h"
#include "protot.h"

#define NDATA 3000000  /* Number of data points (from *.erd file) */
#define MAXSPE 1000000 /* Number of data points for energy spectrum. Max number of channels = max_energy[MeV]/min_chwidth[MeV/ch] */
#define NAMELEN 100

#define NDEPTH 20

typedef struct {
   char virtual;
   double time;
   double weight;
   double depth;
   double energy;
   double mass;
   double dx;
   double dy;
} Mps_erd;

typedef struct {
   int virtual;
   int wide;
   int ddist;
   int dwindow;
   int fmass;
   int etime;
   int nvdepth;
   int pnvd;
   double timeres;
   double eres;
   double vsize;
   double chwidth;
   double mass;
   double dmin;
   double dmax;
   double toflen;
   double scale;
   double dx;
   double dy;
   char dfile[NAMELEN];
} Params;

typedef struct {
   int n;
   double x[NDDIST];
   double y[NDDIST];   
} Ddist;

void virtual_data(Mps_erd *,double *,double,double,double,double,int,int);
void readparams(Params *);
void read_ddist(double *, double *, Ddist *);
double get_ddist_value(double,Ddist *);

/* Reads simulated data from *.erd file and generates energy spectrum file.
 * Uses distribution data when generating spectrum.
 * Only small modifications to the original get_espe. */
void get_espe(double *dist_x, double *dist_y)
{
   FILE *fin, *fout; /* File pointers for input and output */
   char *fcomb;
   Params params;
   /* Mps_erd data[NDATA]; */ /* This needs a lot of memory! */
   /* double spe[MAXSPE]; */
   Ddist ddist;
   char line[NLINE];
   double vcorr[MAXNDEPTH],mass,wsqr=0.0,w_ave,w_sd;
   double mine = 1e20,maxe = -1e20,wsum=0.0,maxd,vr,dweight,wtotal=0.0,scale;
   int i,n,j,maxj=0,minj=100000,nd,end,nr=0,nv=0,valid,nerr,ndout,linelen;
   int ntotal=0,dsize=FALSE;
   
   /* Making a pointer to struct Mps_erd and allocating memory */
   Mps_erd *data;
   data = malloc( (NDATA+1) * (sizeof*data) );
   if(data == NULL)
   {
      error_dialog("Could not allocate memory for erd data");
      return;
   }
     
   /* Allocating memory for spectrum data */
   double *spe;
   spe = malloc( sizeof(double) * (MAXSPE+1) );
   if(spe == NULL)
   {
      error_dialog("Could not allocate memory for energy spectrum data");
      free(data);
      return;
   } 
   
   
   /* Here is the file what is readed (*.erd) */
   fcomb = (char *)calloc(strlen(file_path) + strlen(FERD) + 1, sizeof(char));
   strcat(fcomb, file_path);
   strcat(fcomb, FERD);
   
   fin = fopen(fcomb, "r");
   if(fin == NULL){
      error_dialog_open(fcomb);
      free(data);
      free(spe);
      free(fcomb);
      return;
   }
   
   for(i=0;i<MAXSPE;i++)
      spe[i] = 0.0;

   readparams(&params);

   if(params.ddist)
      read_ddist(dist_x, dist_y, &ddist);
      
   i = nerr = ndout = 0;
   end = FALSE;

   if(params.dx > 0.0 && params.dy > 0.0)
      dsize = TRUE;

   while(!end && (fgets(line,NLINE,fin) != NULL)){
      valid = FALSE;
      if(i == 0) {
         linelen = strlen(line);
         valid = TRUE;
      } else {
         if(strlen(line) == linelen)
            valid = TRUE;
         else {
            valid = FALSE;
            nerr++;
         }         
      }
    
      if(valid){
         j = sscanf(line,"%c %lf %lf %lf %lf %lf %lf %lf",&(data[i].virtual),
                    &(data[i].energy),&(data[i].mass),&(data[i].depth),
                    &(data[i].weight),&(data[i].dx),&(data[i].dy),
                    &(data[i].time));
         if(j != 8){
            valid = FALSE;
            nerr++;
         } else {
            ntotal++;
            wtotal += data[i].weight;
            wsqr += data[i].weight*data[i].weight;
         }
         if(dsize){
            if(ipow2(data[i].dx/params.dx) + ipow2(data[i].dy/params.dy) > 1.0){
               valid = FALSE;
               ndout++;
            }
         }
         if(params.dwindow){
            if(data[i].depth < params.dmin || data[i].depth > params.dmax){
               valid = FALSE;
               ndout++;
            }            
         }
         
         if(params.ddist){
            dweight = get_ddist_value(data[i].depth,&ddist);
            if(dweight > 0)
               data[i].weight *= dweight;
            else {
               valid = FALSE;
               ndout++;
            }
         }
         if(valid && (params.virtual || params.wide || (!params.virtual && (data[i].virtual == 'R'))))
            i++;
         if(i >= NDATA){
            /* fprintf(stderr,"Maximum number of data (%i) read\n",NDATA); */
            error_dialog("Maximum number of data read");
            end = TRUE;
         }
      }
   }
   
   fclose(fin);
   free(fcomb);
   
   n = i;
   
   if(params.ddist){
      fprintf(stderr,"%i data points read\n",n);
      fprintf(stderr,"%i erraneous lines\n",nerr);
      fprintf(stderr,"%i data points out of the depth or dsize window\n",ndout);
   }

   w_ave = wtotal/ntotal;
   w_sd = sqrt(wsqr/ntotal - ipow2(w_ave));

   maxd = 0.0;
   if(params.fmass)
      mass = params.mass;

   for(i=0;i<n;i++){
      if(data[i].virtual == 'V')
      	nv++;
      else
      	nr++;
      if(data[i].depth > maxd)
         maxd = data[i].depth;

      if(!params.fmass)
         mass = data[i].mass;
         
      if(params.etime){
         data[i].time += params.timeres*gaussian2();
         data[i].energy = (0.5*mass*C_U*ipow2(params.toflen/(data[i].time*C_NS)))/C_MEV;
      } else {
         data[i].energy += params.eres*gaussian2();
      }

      if(data[i].energy < mine)
         mine = data[i].energy;
      if(data[i].energy > maxe)
         maxe = data[i].energy;
   }
   
   if(params.ddist)
      fprintf(stderr,"%i real %i virtual\n",nr,nv);
      
   maxd *= 1.01;

   if(params.virtual)
      virtual_data(data,vcorr,params.vsize,maxd,w_ave,w_sd,n,params.nvdepth);

   for(i=0;i<n;i++){
      if(params.virtual && data[i].virtual == 'V') {
         nd = (int) (data[i].depth*params.nvdepth/maxd);
         wsum += data[i].weight*vcorr[nd];
         data[i].weight *= vcorr[nd];
      } else {
         wsum += data[i].weight;
      }
   }

   mine = (floor(mine/params.chwidth) - 1)*params.chwidth;
   maxe = (floor(maxe/params.chwidth) + 1)*params.chwidth;

   for(i=0;i<n;i++){
      j = (int) (0.5 + (data[i].energy - mine)/params.chwidth);
      if (j>MAXSPE)
      {
         error_dialog("Too many channels in energy spectrum. Check energy spectrum parameters.");
         free(data);
         free(spe);
         return;
      }
      spe[j] += data[i].weight;
      if(j < minj)
         minj = j;
      if(j > maxj)
         maxj = j;
   }
/*
   if(params.scale < 0.0)
      scale = ((double) ntotal)/wtotal;
   else
      scale = params.scale/((double) ntotal);
*/
   if(params.scale < 0.0)
      scale = 1.0;
   else
      scale = params.scale;
      
   /* Here is the file for output (energy spectrum) */
   fcomb = (char *)calloc(strlen(file_path) + strlen(FSPE) + 1, sizeof(char));
   strcat(fcomb, file_path);
   strcat(fcomb, FSPE);
      
   fout = fopen(fcomb, "w");
   if(fout == NULL){
      error_dialog_open(fcomb);
      free(data);
      free(spe);
      free(fcomb);
      return;
   }

   for(i=minj;i<=maxj;i++){
      fprintf(fout,"%10.3f %10.3f\n",params.chwidth*i + mine,scale*spe[i]);
   }
   
   fclose(fout);
   free(data);
   free(spe);
   free(fcomb);
   
   /* Here is the file for output (distribution points) */
   if(params.ddist == TRUE)
   {
      fcomb = (char *)calloc(strlen(file_path) + strlen(FDIST) + 1, sizeof(char));
      strcat(fcomb, file_path);
      strcat(fcomb, FDIST); 
      
      fout = fopen(fcomb, "w");
      if(fout == NULL){
	     error_dialog_open(fcomb);
         free(fcomb);
         return;
      }

      for(i = 0; i < n_distpoints; i++){
         fprintf(fout, "%f %f\n", dist_x[i], dist_y[i]);
      }

      fclose(fout);
      free(fcomb);
   }

}

void virtual_data(Mps_erd *data,double *vcorr,double vsize,double maxd,
                  double w_ave,double w_sd,int n,int ndepth)
{
   int i,j1,j2,nd;
   double vsum[MAXNDEPTH],rsum[MAXNDEPTH];

   for(i=0;i<ndepth;i++){
      vsum[i] = 0.0;
      rsum[i] = 0.0;
   }

   w_sd = 3.0*w_sd;
   
   for(i=0;i<n;i++){
      nd = (int) (data[i].depth*ndepth/maxd);
      if(fabs(data[i].weight - w_ave) < w_sd){
         if(data[i].virtual == 'V')
            vsum[nd] += data[i].weight;
         else
            rsum[nd] += data[i].weight;
      }
   }
   
   for(i=0;i<ndepth;i++){
      if(rsum[i] > 0.0 && vsum[i] > 0.0){
         vcorr[i] = vsize*rsum[i]/vsum[i];
      } else {
         vcorr[i] = -1.0;
      }
      /* fprintf(stderr,"%6i %10.3f %10.3f %10.3f %10.3f\n",i,rsum[i],vsum[i],i*maxd/ndepth,vcorr[i]); */
   }

   if(vcorr[0] < 0.0)
      vcorr[0] = vsize;

   if(vcorr[ndepth-1] < 0.0)
      vcorr[ndepth-1] = 1.0;

   i = 0;
   /* fprintf(stderr,"%6i %10.3f\n",i,vcorr[i]); */
   for(i=1;i<ndepth-1;i++){
      if(vcorr[i] < 0.0){
         j1 = i-1;
         while(vcorr[j1] < 0.0)
            j1--;
         j2 = i+1;
         while(vcorr[j2] < 0.0)
            j2++;
         vcorr[i] = vcorr[j1] + (vcorr[j2] - vcorr[j1])*(i - j1)/(j2 - j1);
      }
      /* fprintf(stderr,"%6i %10.3f\n",i,vcorr[i]); */
   }
}

/* Espe params from GUI to get_espe */
void readparams(Params *params)
{

   params->virtual = FALSE; /* Real */
   params->wide = FALSE; /* Narrow */
   params->ddist = FALSE;
   params->dwindow = FALSE;
   params->etime = TRUE;
   params->fmass = FALSE;
   params->pnvd = FALSE;   
   
   params->scale = -1.0;
   params->dx = -1.0;
   params->dy = -1.0;
      
      
      /* Reading (optional) parameters */
      if(espe.virtual == TRUE){
         params->virtual = TRUE;
         params->vsize = espe.vsize;
         params->nvdepth = espe.vnum;     
      }
      if(espe.dsize == TRUE){
            params->dx = espe.dx;
            params->dy = (int)espe.dy;       
      }
      if(espe.pnvd == TRUE){
         params->pnvd = TRUE;
      }
      if(espe.wide == TRUE){
         params->wide = TRUE;
      }
      if(espe.dwindow == TRUE){
         params->dwindow = TRUE;
         params->dmin = espe.dmin;
         params->dmax = espe.dmax;     
      }
      /* distribution points */
      if(espe.ddist == TRUE){
         /* params->dfile[NAMELEN-1] = '\0'; */ /* We don´t need the file anymore */
         params->ddist = TRUE;       
      }      
      if(espe.mass == TRUE){
         params->fmass = TRUE;
         params->mass = espe.mass_value;      
      }      
      if(espe.timeres == TRUE){
         params->etime = TRUE;
         params->timeres = espe.tres;      
      }      
      if(espe.timeres == FALSE){
         params->etime = FALSE;            
         params->eres = espe.eres;     
      }
      params->chwidth = espe.chwidth;
      params->toflen = espe.toflen;       
      params->scale = espe.scale;        

/*

typedef struct {
   int virtual;
   int wide;
   int ddist;
   int dwindow;
   int fmass;
   int etime;
   int nvdepth;
   int pnvd;
   double timeres;
   double eres;
   double vsize;
   double chwidth;
   double mass;
   double dmin;
   double dmax;
   double toflen;
   char dfile[NAMELEN];
} Params;

 - virtual vs. real
 - number of depth channels in virtual scaling
 - output of the virtual scaling data
 - size of the virtual detector
 - wide or narrow recoiling scheme 
 - virtual size 
 - channel width
 - time resolution or energy resolution
 - mass of the recoil
 - depth window
 - depth distribution

 
*/

}

/* dist_x and dist_y are the distribution points (readed from the graph) */
void read_ddist(double *dist_x, double *dist_y, Ddist *ddist)
{
   int i;

   /* 
   while(i < NDDIST && fscanf(fp,"%lf %lf",&(ddist->x[i]),&(ddist->y[i])) == 2)
      i++;
   */
   
   for(i = 0; i < n_distpoints; i++)
   {
     ddist->x[i] = dist_x[i];
     ddist->y[i] = dist_y[i];
   }

   /* These are tested elsewhere */
   /* 
   if(i == 0){
      fprintf(stderr,"Could not read any depth data from file %s\n",fname);
      exit(2);
   }
   if(i >= NDDIST){
      fprintf(stderr,"Too many data points (> %i) in file %s\n",NDDIST,fname);
      exit(3);
   }
   */
  
   ddist->n = n_distpoints;

}
double get_ddist_value(double depth,Ddist *d)
{
   double value;
   int n,i=0;
   
   n = d->n;
   
   if(depth <= d->x[0] || depth >= d->x[n-1])
      return(0.0);

   while(depth > d->x[i])
      i++;

   value = d->y[i-1] + (depth - d->x[i-1])*(d->y[i] - d->y[i-1])/
                       (d->x[i] - d->x[i-1]);

   return(value);

}

/* Name of the original gaussian() changed,
 * because this is already defined in random.c.
 * get_espe uses this "new" gaussian2() */
double gaussian2()
{
   double value;
   static long int seed=1;
      
   value = gasdev(&seed);
   
   return(value);
}
