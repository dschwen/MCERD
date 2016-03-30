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
#include "zbl96.h"
#include "yang.h"
#include "protot.h"

#include "generate_sptables.h" /* See this for SRIM stoppings related constants and parameters! */

#define NSTO 250 /* Number of ZBL96 stopping points */

/* Functions related to calculating stopping powers */


/* Stopping powers from SRIM SRModule.
 * Reads electronic stoppings from generated data files for ion z1 in element z2.
 * Returns stopping and velocity tables with same index (stopping[i] at the velocity of velocity[i]).
 * Data files are from generate_sptables(), see function comments for more information. */
double **read_sptable(int z1, int z2)
{

  char filename[NFILE];
  /* double buf[NSTOP+1]; */
  FILE *fp;
  double **sp;
  int k;
  
  /* Allocating memory for buffer */
  double *buf;
  buf = malloc(sizeof(double)*(NSTOP+1));
  
  if(buf == NULL)
  {
      error_dialog("Could not allocate memory for stopping values");
      longjmp(err_buf,1);
  }
  
  /* Allocating memory for stopping tables */
  sp = (double **) malloc(sizeof(double *)*2);
  sp[0] = (double *) malloc(sizeof(double)*NSTOP);
  sp[1] = (double *) malloc(sizeof(double)*NSTOP);
  
  if(sp[0] == NULL || sp[1] == NULL)
  {
      error_dialog("Could not allocate memory for stopping values");
      free(buf);
      longjmp(err_buf,1);
  }

  sprintf(filename, "%s/%d%s", SPATH, z1, SFEND);
    
  fp = fopen(filename, "rb");
  if(fp == NULL)
  {
    error_dialog_open(filename);
    free(buf);
    free(sp[0]);
    free(sp[1]);
    free(sp);
    longjmp(err_buf,1);
  }
  
  fseek(fp, sizeof(double)*(NSTOP+1)*(z2-1), 1);
  fread((void*)buf, sizeof(double), NSTOP+1, fp); /* Read stopping data plus control data */
  fclose(fp);
  
  /* Each table must end with the element number as a control data */
  if(buf[NSTOP] != z2)
  {
    error_dialog("Error: reading SRIM stopping power");
    fprintf(stderr, "Error reading stopping power for ion Z=%d in material Z=%d\n", z1, z2);
    free(buf);
    free(sp[0]);
    free(sp[1]);
    free(sp);
    longjmp(err_buf,1);
  }
  
  /* Stoppings */
  for(k=0; k<NSTOP; k++)
    sp[1][k] = buf[k]; /* buf[NSTOP] is control data so it's not needed anymore */
  
  /* Velocities */
  /* Indexes in this loop has to be same when generating tables. See generate_sptables(). */
  for(k=0; k<NSTOP; k++)
    sp[0][k] = calc_velocity(k+1); /* now velocity is not MINVEL which can be zero */
  
  free(buf);
  return sp;
  
}

/* Calculates stoppings (ZBL96 or SRIM).
 * Option for SRIM-stoppings has been added later */
void calc_zblsto(Global *global,Ion *ion,Target *target,int nlayer)
{
   Target_layer *layer;
   double **sto,m1,m2,den,minv,maxv,vstep;
   unsigned int flag;
   int i,j,p,n,z1,z2,nion,nsto;
   int use_srim;
   
   nion = ion->type;
   layer = &(target->layer[nlayer]);
   
   use_srim = 0; /* Using ZBL for default */
   
   if(layer->sto[nion].stodiv == STOP_SRIM)
      use_srim = 1;
   
   /* Velocity range and step for SRIM-stoppings */
   if(use_srim){
                  
      /* These values have to be same as in calc_velocity()! */
      nsto = NSTOP;
      minv = MINVEL;
      maxv = MAXVEL;
      vstep = (maxv - minv)/nsto;
   }

   /* Velocity range and step for ZBL-stoppings */
   else{
        
      nsto = NSTO;
      flag = (ZBL_EV_1E15ATOMS_CM2 | ZBL_M_S | ZBL_N_NO);
      den = 0.0;  /* density doesn't matter in the atomic units */ 
      minv = 0.0;
      maxv = 1.1*sqrt(2.0*global->ionemax/(ion->A));
      vstep = (maxv - minv)/nsto;
   }
   
   z1 = (int) (ion->Z + 0.5);
   m1 = ion->A/C_U;     /* m1 is in the units of u */

   for(i=0;i<MAXSTO;i++)
      layer->sto[nion].sto[i] = 0.0;

   layer->sto[nion].stodiv = 1.0/vstep;
   layer->sto[nion].n_sto = nsto;
     
   for(i=0;i<layer->natoms;i++){
                                
      p = layer->atom[i];
      z2 = target->ele[p].Z;
      m2 = target->ele[p].A*C_U; /* m2 is also in the units of u */
      
      if(use_srim){
         sto = read_sptable(z1,z2); /* see read_sptable() and generate_sptables()! */
         n = nsto;
      }
      else
         sto = zbl96(z1,z2,m1,m2,den,minv,maxv,vstep,flag,&n);
         
      for(j=0;j<n;j++){
         layer->sto[nion].vel[j] = sto[0][j];
         layer->sto[nion].sto[j] += sto[1][j]*layer->N[i]*C_EVCM2_1E15ATOMS;
      }
      
      free(sto[0]);
      free(sto[1]);
      free(sto);
   }

}

void calc_yangstragg(Ion *ion,Target *target,int nlayer)
{
/*
 *  Here we assume that we already have stopping values.
 *  Straggling is given in the units of J^2/m.
 */  

   Target_layer *layer;
   double **gamma,e,den=0.0,minv,maxv,vstep,m1,m2,vel;
   unsigned int flag;
   int i,j,n,p,z1,z2,nion;
   
   flag = (ZBL_EFFCHARGE | ZBL_M_S | ZBL_N_NO);

   nion = ion->type;
   layer = &(target->layer[nlayer]);
   
   z1 = (int) (ion->Z + 0.5);
   m1 = ion->A;

   minv = layer->sto[nion].vel[0];
   maxv = layer->sto[nion].vel[layer->sto[nion].n_sto - 1];
   vstep = 1.0/layer->sto[nion].stodiv;

   for(i=0;i<MAXSTO;i++)
      layer->sto[nion].stragg[i] = 0.0;

   for(i=0;i<layer->natoms;i++){
      p = layer->atom[i];
      z2 = target->ele[p].Z;
      m2 = target->ele[p].A;
      gamma = zbl96(z1,z2,m1/C_U,m2/C_U,den,minv,maxv,vstep,flag,&n);
      for(j=0;j<n;j++){
         vel = gamma[0][j];
         e = 0.5*m1*ipow(vel,2);
         layer->sto[nion].stragg[j] += layer->N[i]*Bohr(z1,z2)*
               (Chu(z2,m1,e)*ipow(gamma[1][j],2) + 
                Yang_correl(z1,m1,z2,e,layer->gas));
      }
      free(gamma[0]);
      free(gamma[1]);
      free(gamma);
   }   
}

double Yang_correl(int Z1,double M1,int Z2,double E,int gas)
{
/*
 *    Yang's expression for contribution of correlation effects in straggling
 *    Q. Yang, D.J. O'Connor, and Z. Wang NIM B61(1991)149
 *
 *    All gases are treated as atomic gases here!
 */   
  
   double yang,gamma,eps,e_m;

   eps = (E*C_U/(C_MEV*M1))/(pow((double) Z1,1.5)*pow(Z2,0.5));

   e_m = E*C_U/(C_MEV*M1);

   if(gas){
      if(Z1 == 1){
         gamma = Y_G_B3*(1.0 - exp(-Y_G_B4*e_m));
         yang = Y_G_B1*gamma/(ipow2(e_m - Y_G_B2) + ipow2(gamma));
      } else {
         gamma = Y_G_C3*(1.0 - exp(-Y_G_C4*eps));
         yang = (pow(Z1,4.0/3.0)/pow(Z2,1.0/3.0))*Y_G_C1*gamma/
                (ipow2(eps - Y_G_C2) + ipow2(gamma));
      }
   } else {
      if(Z1 == 1){
         gamma = Y_S_B3*(1.0 - exp(-Y_S_B4*e_m));
         yang = Y_S_B1*gamma/(ipow2(e_m - Y_S_B2) + ipow2(gamma));
      } else {
         gamma = Y_S_C3*(1.0 - exp(-Y_S_C4*eps));
         yang = (pow(Z1,4.0/3.0)/pow(Z2,1.0/3.0))*Y_S_C1*gamma/
                (ipow2(eps - Y_S_C2) + ipow2(gamma));
      }
   }

   return(yang);

}

double Bohr(int Z1,int Z2)
{
/*   
 *  Variance of the Bohr straggling in the units of Jm^2
 */
  
   double bohr;
   
   bohr = 4.0*PI*Z1*Z1*Z2*pow(P_E,4.0)/pow(4.0*PI*P_EPS0,2.0);
   
   return(bohr);
}
double Chu(int Z2,double M1,double E)
{
/*
 *  Yang's fittings to the Chu's calculations of Z2-dependence of straggling 
 *    Q. Yang, D.J. O'Connor, and Z. Wang NIM B61(1991)149
 */

   double chu;

/*   
 *    In the case of H in target we assume Bohr straggling 
 *    (not in Yang's paper!)
 */

   if(Z2 == 1)
      return(1.0);
      
   if(E <= 0.0)
      return(1.0);
   
   E *= C_U/(M1*C_MEV);

   if(Z2 < 2 || Z2 > 97){
      /* fprintf(stderr,"Chu's fitting coefficients are not given for Z2=%i\n",Z2);
      exit(11); */
      error_dialog("Chu's fitting coefficients are not given for Z2 < 2 or Z2 > 97");
      longjmp(err_buf,1);
   }
   Z2 -= 2;    /* Arrays start from 0, not from 2 */
      
   chu = 1.0/(1 + Y_A1[Z2]*pow(E,Y_A2[Z2]) + Y_A3[Z2]*pow(E,Y_A4[Z2]));
   
   return(chu);
}
