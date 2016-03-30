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

double get_isotope(Isotopes *);

int erd_scattering(Global *global,Ion *ion,Ion *recoil,Target *target,
                   Detector *detector)
{
   Ion *tatom;
   Vector sc_lab,sc_target,sc_ion;
   double sq_tmp,K;
   int i;

   if(global->simtype == SIM_RBS)
      tatom = ion + TARGET_ATOM;
   
   recoil->w = ion->w;

   if(ion->wtmp > 0.0)
   	recoil->w *= ion->wtmp;

   if(global->recwidth == REC_NARROW) {

      /* Recoiling direction in the laboratory coordinate system */

      sc_lab = get_recoiling_dir(global,ion,recoil,target,detector);

      /* Recoiling direction in the target coordinate system */

      rotate(global->beamangle,0.0,sc_lab.theta,sc_lab.fii,
             &(sc_target.theta),&(sc_target.fii));

      /* Recoiling direction in the ion coordinate system */

      rotate(ion->theta,ion->fii - PI,sc_target.theta,sc_target.fii,
             &(sc_ion.theta),&(sc_ion.fii));

   } else {
         sc_ion = get_recoiling_dir(global,ion,recoil,target,detector);

         rotate(ion->theta,ion->fii,sc_ion.theta,sc_ion.fii,
                &(sc_target.theta),&(sc_target.fii));

         rotate(global->beamangle,PI,sc_target.theta,sc_target.fii,
                &(sc_lab.theta),&(sc_lab.fii));
   }

   if(global->simtype == SIM_ERD){   
      if(sc_ion.theta < PI/2.0){
         if(recoil->I.n > 0)
            recoil->A = get_isotope(&(recoil->I));
         recoil->E = ion->E*ipow2(cos(sc_ion.theta))*4.0*(ion->A*recoil->A)/
                     ipow2(ion->A + recoil->A);
         recoil->theta = sc_target.theta;
         recoil->fii = sc_target.fii;
         recoil->opt.cos_theta = cos(recoil->theta);
         recoil->opt.sin_theta = sin(recoil->theta);
         recoil->p = ion->p;
         recoil->tlayer = ion->tlayer;
         recoil->nsct = 0;
         recoil->w /= (ipow2(ion->E/C_MEV)*ipow(cos(sc_ion.theta),3));
         recoil->time = 0.0;
         recoil->type = SECONDARY;
         recoil->status = NOT_FINISHED;
         recoil->lab.p = ion->lab.p;
         recoil->lab.theta = ion->lab.theta;
         recoil->lab.fii = ion->lab.fii;
         recoil->virtual = FALSE;
         for(i=0;i<(target->nlayers - target->ntarget);i++)
 	    recoil->Ed[i] = 0.0;
#ifdef REC_ANG
         recoil->rec_ion_theta = sc_ion.theta/C_DEG;
         recoil->rec_lab_theta = sc_lab.theta/C_DEG;
         if(sc_ion.fii > PI)
            sc_ion.fii -= (2.0*PI);
         if(sc_lab.fii > PI)
            sc_lab.fii -= (2.0*PI);
         recoil->rec_ion_fii = sc_ion.fii/C_DEG;
         recoil->rec_lab_fii = sc_lab.fii/C_DEG;      
#endif    
         /*
          *   Save the physical state of the primary and the secondary ion
          *   at the scattering point.
          */
	
         save_ion_history(global,ion,recoil,detector,sc_lab,sc_ion);

#ifdef DEBUG
         print_ion_position(global,ion,"R",ANYSIMULATION);
#endif
         return(TRUE);
      } else {
         recoil->status = ion->status = FIN_NO_RECOIL;
         return(FALSE);
      }
   } else if(global->simtype == SIM_RBS){
      if(tatom->I.n > 0)
         tatom->A = get_isotope(&(tatom->I));
      sq_tmp = ipow2(tatom->A) - ipow2(ion->A*sin(sc_ion.theta));
      
      if(sq_tmp > 0.0){
         sq_tmp = sqrt(sq_tmp);
         K = ipow2((sq_tmp + ion->A*cos(sc_ion.theta))/(ion->A + tatom->A));
         recoil->E = ion->E*K;
         recoil->w *= 0.001*C_EV*ipow2(sq_tmp + tatom->A*cos(sc_ion.theta))/
                      (tatom->A*ipow(sin(sc_ion.theta),4)*sq_tmp*ipow2(ion->E));
      } else { /* rare case because of the average masses */
         sq_tmp = 0.0;
         K = ipow2((sq_tmp + ion->A*cos(sc_ion.theta))/(ion->A + tatom->A));
         recoil->E = ion->E*K;
         recoil->w = 0.0;                           
      }
      recoil->theta = sc_target.theta;
      recoil->fii = sc_target.fii;
      recoil->opt.cos_theta = cos(recoil->theta);
      recoil->opt.sin_theta = sin(recoil->theta);
      recoil->p = ion->p;
      recoil->tlayer = ion->tlayer;
      recoil->nsct = 0;
      recoil->time = 0.0;
      recoil->type = SECONDARY;
      recoil->status = NOT_FINISHED;
      recoil->lab.p = ion->lab.p;
      recoil->lab.theta = ion->lab.theta;
      recoil->lab.fii = ion->lab.fii;
      recoil->virtual = FALSE;
#ifdef REC_ANG
         recoil->rec_ion_theta = sc_ion.theta/C_DEG;
         recoil->rec_lab_theta = sc_lab.theta/C_DEG;
         if(sc_ion.fii > PI)
            sc_ion.fii -= (2.0*PI);
         if(sc_lab.fii > PI)
            sc_lab.fii -= (2.0*PI);
         recoil->rec_ion_fii = sc_ion.fii/C_DEG;
         recoil->rec_lab_fii = sc_lab.fii/C_DEG;      
#endif    
	
      save_ion_history(global,ion,recoil,detector,sc_lab,sc_ion);

#ifdef DEBUG
      print_ion_position(global,ion,"R",ANYSIMULATION);
#endif
      return(TRUE);
   } else {
      /* fatal_error("Unknown simulation type\n"); */
      error_dialog("Unknown simulation type");
      longjmp(err_buf,1);
   }
   
}
double get_isotope(Isotopes *I)
{
   double r,c=0.0,mass;
   int i=0;
   
   r = rnd(0.0,1.0,RND_OPEN,RND_CONT);

   while(i < I->n && r > c){
      c += I->c[i];
      i++;
   }

   if(i == I->n && r > c){
      /* fatal_error("Error calculating random isotope mass\n"); */
      error_dialog("Error calculating random isotope mass");
      longjmp(err_buf,1);
   }

   mass = I->A[i-1];

   return(mass);
}

void save_ion_history(Global *global,Ion *ion,Ion *recoil,Detector *detector,
		      Vector sc_lab,Vector sc_ion)
{
   recoil->hist.tar_recoil.p = recoil->p;
   recoil->hist.tar_recoil.theta = recoil->theta;
   recoil->hist.tar_recoil.fii = recoil->fii;

   recoil->hist.lab_recoil = sc_lab;
   recoil->hist.lab_recoil.p = ion->lab.p;

   recoil->hist.ion_recoil = sc_ion;
   
   recoil->hist.tar_primary.p = ion->p;
   recoil->hist.tar_primary.theta = ion->theta;
   recoil->hist.tar_primary.fii = ion->fii;

   recoil->hist.lab_primary = ion->lab;

   recoil->hist.ion_E = ion->E;
   recoil->hist.recoil_E = recoil->E;

   recoil->hist.layer = recoil->tlayer;
   recoil->hist.nsct = ion->nsct;
   recoil->hist.time = ion->time;
   
}
Vector get_recoiling_dir(Global *global,Ion *ion,Ion *recoil,Target *target,
                         Detector *det)
{
/*
 *  We calculate here the recoiling angle in the laboratory coordinates.
 */

   Vector d;
   double t,costhetamax,thetamax,theta,fii;
   int n;

   if(global->simstage == PRESIMULATION){
      theta = 0.0;
      fii = 0.0;
      rotate(det->angle,0.0,theta,fii,&(d.theta),&(d.fii));
   } else {
      if(global->recwidth == REC_NARROW){
         n = ion->tlayer;
         thetamax = target->recpar[n].x * ion->p.z + target->recpar[n].y;
/*
         thetamax = max(0.0,thetamax);
         thetamax += det->thetamax;
*/
         costhetamax = cos(thetamax);
      /*
       * This may be dangerous if thetamax is very small
       * because costhetamax is then very close to 1.0.   
       */
         t = rnd(costhetamax,1.0,RND_CLOSED,RND_CONT);
         theta = acos(t);
         recoil->w *= ipow2(thetamax/det->thetamax);
         fii = rnd(0,2.0*PI,RND_OPEN,RND_CONT);
         rotate(det->angle,0.0,theta,fii,&(d.theta),&(d.fii));
      } else {
         t = rnd(global->costhetamax,global->costhetamin,RND_CLOSED,RND_CONT);
         d.theta = acos(t);
         d.fii = rnd(0,2.0*PI,RND_OPEN,RND_CONT);
      }
   }
   
   return(d);
}
