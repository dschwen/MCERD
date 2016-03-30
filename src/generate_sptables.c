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


/*
   This file includes functions for generating SRIM stopping power data.
   Some of the functions in this file is taken (and modified) from Corteo, F. Schiettekatte.
   Corteo is distributed under the terms of GNU GPL.
*/


#include "general.h"
#include "read_input.h"

#include "generate_sptables.h" /* All important constants! */

#define TRUE 1
#define FALSE 0


/* All unstable elements listed from Z=1 to Z=83. */
/* You have to update these tables if you increase the number of elements when generating stopping tables.
 * Elements have to be in order ( with increasing Z ). */
/*                   Tc   Pm   end  */
int unstable_z[] = { 43,  61, -1 }; /* unstable elements Z */
int unstable_a[] = { 98, 145, -1 }; /* mass numbers of unstable elements which is used */


/* If mass number is not given (a <= 0) finds mass of the most abundant isotope for given z.
 * If mass number is given (a > 0) finds mass for the specific isotope of the element.
 * Reads masses from the file and returns mass (u). */
double get_mass(int a, int z)
{
   FILE *fp;
   char S[3];
   double U, C;
   int N, Z, A, found;
   double maxcon; /* for checking max concentration */
   double mass; /* mass of the isotope */
   

   fp = fopen(FILE_MASSES,"r");
   
   if(fp == NULL)
   {
      fprintf(stderr,"Could not open file %s\n",FILE_MASSES);
      exit(10);
   }
   
   Z = 0;
   maxcon = -1;
   found = FALSE;
   
   /* Finding mass for given mass number and z */
   if(a > 0)
   {
      while( (!found) && fscanf(fp,"%i %i %i %s %lf %lf",&N,&Z,&A,S,&U,&C)==6 && (Z <= z) )
         if( (Z == z) && (A == a) )
         {
            found = TRUE;
            mass = U/1.0e6; /* mass in the units of u */
         }
   }
   /* Finding mass of the most abundant isotope for given z */
   else
   {
      while( fscanf(fp,"%i %i %i %s %lf %lf",&N,&Z,&A,S,&U,&C)==6 && (Z <= z) )
         if( (Z == z) && (C > 0.0) )
         {
            found = TRUE;
            if(C > maxcon)
            {
               maxcon = C;
               mass = U/1.0e6; /* mass in the units of u */
            }
         }
   }

   fclose(fp);

   if(!found)
   {
      fprintf(stderr,"atom data not found from database\n");
      exit(12);
   }
   else
      return mass;

}

/* Calculating velocity (m/s) from index, using constant spacing */
double calc_velocity(int index)
{
  double minv, maxv, vstep;
  
  minv = MINVEL;
  maxv = MAXVEL;
  vstep = (maxv - minv)/NSTOP;
  
  return minv + vstep*index;
  
}

/* Skip to end of line, including end of line character */
void ignoreline(FILE *fp)
{
  fscanf(fp, "%*[^\n]");
  fscanf(fp, "%*1[\n]");
}

/* Generates stopping tables for all ion-atom pairs using SRModule.
 * Data file is made for each ion, Z from 1 to MAXELEMZ.
 * Each data file (Z.esd) contains electronic stopping data for the specific ion in all elements, Z from 1 to MAXELEMZ.
 * NSTOP determines how many stopping points are calculated for each ion-atom pair.
 * MINVEL and MAXVEL determines min and max velocity used for calculating stoppings.
 * Stoppings are calculated for the most abundant isotope */
void generate_sptables()
{
  char filename[NFILE];
  int i, k, z;
  int maxelem;
  double buf[NSTOP+1];
  double m_ion, m_atom; /* masses for ion and target atom */
  double energy, vel;
  char svalue[LINE];
  FILE *srin, *srout, *fout;
  int u_ion, u_atom; /* counter for unstable elements (for ion and target atom) */
  
  maxelem = MAXELEMZ;
  u_ion = u_atom = 0;

  char endline[] = "\n";

  for(z=1; z<=maxelem; z++)
  {
    sprintf(filename, "%s/%d%s", SPATH, z, SFEND);
    fout = fopen(filename, "wb");
    if(fout == NULL)
    {
      fprintf(stderr,"Could not open file %s\n", filename);
      exit(10);
    }
    fprintf(stdout, "Generating stopping table %s (%d/%d) ", filename, z, maxelem);
    fflush(stdout);

    /* ion mass (u) */
    if( z == unstable_z[u_ion] )
    {
       m_ion = get_mass(unstable_a[u_ion], z); /* unstable element */
       u_ion++;
    }
    else
       m_ion = get_mass(STABLE, z); /* stable elements */
    
    u_atom = 0;
    
    for(i=1; i<=maxelem; i++)
    {
      if(i%9==0)
      {
        fprintf(stdout, ".");
        fflush(stdout);
      }
      
      /* target element mass, affects nuclear stopping only */
      if( i == unstable_z[u_atom] )
      {
         m_atom = get_mass(unstable_a[u_atom], i); /* unstable element */
         u_atom++;
      }
      else
         m_atom = get_mass(STABLE, i); /* stable elements */
      
      /* write SRModule input file. format has to be correct! check SRModule help file */
      srin = fopen("sr.in", "w");
      if(srin == NULL)
      {
        fprintf(stderr,"Could not open file: sr.in\n");
        exit(10);
      }
      fprintf(srin, endline);
      fprintf(srin, endline);
      fprintf(srin, "sr.out"); /* output file name */
      fprintf(srin, endline);
      fprintf(srin, endline);
      fprintf(srin, "%d %f", z, m_ion); /* ion atomic number and mass */
      fprintf(srin, endline);
      fprintf(srin, endline);
      fprintf(srin, " 0 1 0"); /* target data: solid/gas, density, compound correction */
      fprintf(srin, endline);
      fprintf(srin, endline);
      fprintf(srin, "1"); /* number of target atoms */
      fprintf(srin, endline);
      fprintf(srin, endline);
      fprintf(srin, "%d \"Target atom\" 1 %f", i, m_atom); /* target atom: element, name, stoichiometry, mass */
      fprintf(srin, endline);
      fprintf(srin, endline);
      fprintf(srin, " 7 "); /* output stopping units, 7 = eV/(1E15 atoms/cm2) */
      fprintf(srin, endline);
      fprintf(srin, endline);
      fprintf(srin, " 0 0 "); /* energy range, 0 0 means that energies are listed */
      fprintf(srin, endline);

      /* energy values. NOTE: 1000 points is maximum and first value can't be zero */
      for(k=0; k<NSTOP; k++)
      {
        vel = calc_velocity(k+1); /* now velocity is not MINVEL which can be zero */
        energy = 0.5*m_ion*C_U*(vel*vel); /* energy of the ion with calculated velocity (J) */
        energy = energy/C_KEV; /* SRModule uses keV */
        fprintf(srin, "%g", energy);
        fprintf(srin, endline);
      }
      fprintf(srin, " 0 ");
      fclose(srin);
    
      /* running SRModule which generates stopping values (sr.out file) */
      system("SRModule.exe");

      /* reading sr.out, ignoring comment lines */
      srout = fopen("sr.out", "r");
      if(srout == NULL)
      {
        fprintf(stderr,"Could not open file: sr.out\n");
        exit(10);
      }
      ignoreline(srout);
      ignoreline(srout);
      ignoreline(srout);
      ignoreline(srout);
      for(k=0; k<NSTOP; k++)
      {
        fscanf(srout,"%*s %s", svalue); /* we need only electronic stopping value */
        ignoreline(srout);
        buf[k] = atof(svalue);
      }
      fclose(srout);

      buf[NSTOP]=(double)i; /* Z of the element as a control data */
      fwrite((void*)buf, sizeof(double), (NSTOP+1), fout); /* writes stopping values + control data */
    }
    fclose(fout);
    fprintf(stdout, " done.\n");
    fflush(stdout);
  }
  
}

#undef TRUE
#undef FALSE

