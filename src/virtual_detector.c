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

#define MAXVRATIO 0.80

double get_eloss_corr(Ion *,Target *,double,double,double);
double get_eloss(double,Ion *,Target *,double);

double rbs_kin(double,double,double,int *);
double rbs_cross(double,double,double,int *);

void hit_virtual_detector(Global *global,Ion *ion,Target *target,
                          Detector *det,Point *cross,Point *pout)
{
/*
 *    Here we calculate a random projection point from virtual detector to
 *    the real detector, and correct the ion energy according to the
 *    changed kinematics.
 */
 
   Vector plocal,pvirtual,pnew,pdiff;
   Vector precoil,precnew,prectar,preclab,ptarold,ptarnew,pscat;
   double theta,fii,dx,dy,r,dE1,dE2,dw,dist,eloss,m1,m2;
   int err=FALSE,err1,err2;

   switch(det->vfoil.type){
      case FOIL_CIRC:
         r = det->foil[0].size[0]*sqrt(rnd(0.0,1.0,RND_CLOSED,RND_CONT));
         fii = rnd(0.0,2.0*PI,RND_CLOSED,RND_CONT);
         plocal.p.x = r*cos(fii);
         plocal.p.y = r*sin(fii);
         break;
      case FOIL_RECT:
         dx = det->foil[0].size[0];
         plocal.p.x =  rnd(-dx,dx,RND_CLOSED,RND_CONT);
         dy = det->foil[0].size[1];
         plocal.p.y =  rnd(-dy,dy,RND_CLOSED,RND_CONT);         
         break;
   }
   plocal.p.z = 0.0;
   theta = det->angle;
   fii = 0.0;

   precoil = ion->hist.ion_recoil;
   prectar = ion->hist.tar_recoil;
   preclab = ion->hist.lab_recoil;

   /*  Because of the beam spot effect pvirtual and pnew must be given
    *  in the coordinates relative to the recoil point.
    */
    
   pvirtual.p = *cross;

   pvirtual.p.x -= preclab.p.x;
   pvirtual.p.y -= preclab.p.y;
   pvirtual.p.z -= preclab.p.z;

   r = sqrt(ipow2(pvirtual.p.x) + ipow2(pvirtual.p.y) + ipow2(pvirtual.p.z));   
   pvirtual.theta = acos(pvirtual.p.z/r);
   pvirtual.fii = atan2(pvirtual.p.y,pvirtual.p.x);

   pnew.p = coord_transform(det->vfoil.center,theta,fii,plocal.p,FORW);

   pnew.p.x -= preclab.p.x;
   pnew.p.y -= preclab.p.y;
   pnew.p.z -= preclab.p.z;

   r = sqrt(ipow2(pnew.p.x) + ipow2(pnew.p.y) + ipow2(pnew.p.z));

   pnew.theta = acos(pnew.p.z/r);
   pnew.fii = atan2(pnew.p.y,pnew.p.x);

   rotate(pvirtual.theta,PI+pvirtual.fii,pnew.theta,pnew.fii,
          &(pdiff.theta),&(pdiff.fii));

   rotate(precoil.theta,precoil.fii,pdiff.theta,pdiff.fii,
          &(precnew.theta),&(precnew.fii));

   if(global->simtype == SIM_RBS){
      m1 = ion[PRIMARY].A;
      m2 = ion[TARGET_ATOM].A;
   }

   if(global->simtype == SIM_ERD)
      dE1 = (ipow2(cos(precnew.theta)/cos(precoil.theta)) - 1.0);
   else if(global->simtype == SIM_RBS){
      dE1 = rbs_kin(m1,m2,precnew.theta,&err1)/
            rbs_kin(m1,m2,precoil.theta,&err2) - 1.0;
      if(err1 || err2)
         err = TRUE;
   }

   pscat.p = coord_transform(preclab.p,global->beamangle,PI,prectar.p,FORW);
   
   ptarold.p.x = pout->x - pscat.p.x;
   ptarold.p.y = pout->y - pscat.p.y;
   ptarold.p.z = pout->z - pscat.p.z;

   r = sqrt(ipow2(ptarold.p.x) + ipow2(ptarold.p.y) + ipow2(ptarold.p.z));

   ptarold.theta = acos(ptarold.p.z/r);
   ptarold.fii = atan2(ptarold.p.y,ptarold.p.x);

   rotate(ptarold.theta,ptarold.fii,pdiff.theta,pdiff.fii,
          &(ptarnew.theta),&(ptarnew.fii));

   rotate(global->beamangle,0.0,ptarold.theta,ptarold.fii,
          &(ptarold.theta),&(ptarold.fii));

   rotate(global->beamangle,0.0,ptarnew.theta,ptarnew.fii,
          &(ptarnew.theta),&(ptarnew.fii));
/*
   printf("TAR: %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n",
          ptarold.theta/C_DEG,ptarold.fii/C_DEG,
          pvirtual.theta/C_DEG,pvirtual.fii/C_DEG,
          ptarnew.theta/C_DEG,ptarnew.fii/C_DEG,
          precnew.theta/C_DEG,precnew.fii/C_DEG);
*/
   eloss = ion->hist.recoil_E - ion->E;

   dE2 = get_eloss_corr(ion,target,dE1,ptarold.theta,ptarnew.theta);

/*
   printf("DIST %10.3f %10.3f %10.3f %10.3f\n",d1/C_NM,d2/C_NM,eloss/C_MEV,dE2);
*/

   dE1 *= ion->hist.recoil_E;
   dE2 *= -eloss;

   if(fabs(dE1 + dE2)/ion->E > MAXVRATIO){
      ion->status = FIN_MISS_DET;
      fprintf(stderr,"Energy would change too much in virtual detector ");
      fprintf(stderr,"%10.3f MeV\n",(dE1+dE2)/C_MEV);
      
      /* error_dialog("Energy would change too much in virtual detector"); */
   }

   if(global->simtype == SIM_ERD)
      dw = ipow(cos(precoil.theta)/cos(precnew.theta),3);
   else if(global->simtype == SIM_RBS){
      dw = rbs_cross(m1,m2,precnew.theta,&err1)/
           rbs_cross(m1,m2,precoil.theta,&err2);
      if(err1 || err2)
         err = TRUE;
   }
                  

/*
   printf("VIR1 %7.3f %8.3f %7.3f %8.3f %7.3f %8.3f %7.2f %7.2f %6.3f\n",
          pvirtual.theta/C_DEG,pvirtual.fii/C_DEG,
          pnew.theta/C_DEG,pnew.fii/C_DEG,
          ion->hist.recoil_E/C_MEV,
          ion->hit.x/C_MM,ion->hit.y/C_MM,
          dE1/C_MEV,dE2/C_MEV);
   printf("VIR2 %7.3f %8.3f %7.3f %8.3f %7.3f %8.3f %7.2f %7.2f %6.3f\n",
          precoil.theta/C_DEG,precoil.fii/C_DEG,
          precnew.theta/C_DEG,precnew.fii/C_DEG,
          ion->hist.recoil_E/C_MEV,
          ion->hit.x/C_MM,ion->hit.y/C_MM,
          dE1/C_MEV,dE2/C_MEV);
*/

   ion->w *= dw;
   ion->E += dE1 + dE2;
   
   /* We may end up with a negative energy */
   
   if(ion->E > global->ionemax || ion->E < global->emin){
      ion->status = FIN_MISS_DET;
      fprintf(stderr,"Energy would change too much in virtual detector\n");
      
      /* error_dialog("Energy would change too much in virtual detector"); */
   }

   if(err){
      ion->status = FIN_MISS_DET;
      fprintf(stderr,"Unphysical RBS-scattering in virtual detector\n");
      /* error_dialog("Unphysical RBS-scattering in virtual detector"); */
   }
      
   if(ion->status == NOT_FINISHED) {

      pnew.p.x += preclab.p.x;
      pnew.p.y += preclab.p.y;
      pnew.p.z += preclab.p.z;

      dist = get_distance(pout,&(pnew.p));
      ion->time += dist/sqrt(2.0*ion->E/ion->A);

      ion->lab.p = pnew.p;
      ion->lab.theta = det->angle;
      ion->lab.fii = 0.0;

      ion->p.y = 0.0;
      ion->p.y = 0.0;
      ion->p.y = 0.0;

      rotate(det->angle,PI,pnew.theta,pnew.fii,&(ion->theta),&(ion->fii));
   
      ion->opt.cos_theta = cos(ion->theta);
      ion->opt.sin_theta = sin(ion->theta);
   }

}

double get_eloss_corr(Ion *ion,Target *target,double dE1,double theta1,
                      double theta2)
{

   double eloss,Ed1,Ed2,E;
   
   E = ion->hist.recoil_E;

   eloss = ion->hist.recoil_E - ion->E;

   Ed1 = get_eloss(E,ion,target,cos(theta1));
   
   Ed2 = get_eloss(E + dE1*E,ion,target,cos(theta2));

   if(Ed1 > E || Ed2 > (E + dE1*E))
      return(-2.0);

   Ed2 *= eloss/Ed1;
/*
   printf("ELO %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f\n",
          (theta1-PI/2.0)/C_DEG,(theta2-PI/2.0)/C_DEG,
          E/C_MEV,(E + dE1*E)/C_MEV,
          eloss/C_MEV,Ed1/C_MEV,Ed2/C_MEV);
*/

   if(eloss > 0.0)
      Ed2 = Ed2/eloss - 1.0;
   else 
      Ed2 = 0.0;

   return(Ed2);

}

double get_eloss(double E,Ion *ion,Target *target,double cos_theta)
{
   Target_layer *layer;
   Vector rec;
   double d,v,sto,v2,sto2,nextz,Eorig;
   int nlayer,cont=TRUE,next_layer,surf=FALSE;

   nlayer = ion->hist.layer;
   layer = &(target->layer[nlayer]);

   rec.p.z = ion->hist.tar_recoil.p.z;

   Eorig = E;

   while(cont){
      d = 20.0*C_NM;
      nextz = rec.p.z + d*cos_theta;
      v = sqrt(2.0*E/ion->A);
      next_layer = FALSE;
      if(nextz < layer->dlow){
         nextz = layer->dlow + 0.1*C_ANGSTROM*cos_theta;
         d = (nextz - rec.p.z)/cos_theta;
         next_layer = TRUE;
      }

      sto = inter_sto(&(layer->sto[SECONDARY]),v,STOPPING);

      if((E - d*sto) < 0.0)
         cont = FALSE;
      else {
         v2 =  sqrt(2.0*(E - d*sto)/ion->A);
         sto2 = inter_sto(&(layer->sto[SECONDARY]),v2,STOPPING);
         E -= 0.5*(sto + sto2)*d;
      }

      if(E < 0.0)
         cont = FALSE;   
  
      if(next_layer){
         nlayer--;
         if(nlayer < 0){
            cont = FALSE;
            surf = TRUE;
         } else
            layer = &(target->layer[nlayer]);
      }

      rec.p.z = nextz;
/*
      printf("STO %10.3f %10.3f %4i\n",E/C_MEV,rec.p.z/C_NM,nlayer);
*/      
   }

   if(surf)
      return(Eorig - E);
   else
      return(1.1*Eorig);
}

double rbs_cross(double m1,double m2,double theta,int *err)
{
   double sq_tmp,value;
   
   *err = FALSE;
   
   if(sin(theta) >= m2/m1){
      *err = TRUE;
      return(1.0);      
   }
   
   if(fmod(theta,PI) == 0){
      *err = TRUE;
      return(1.0);
   }
   
   sq_tmp = sqrt(ipow2(m2) - ipow2(m1*sin(theta)));
   
   value = ipow2(sq_tmp + m2*cos(theta))/(m2*pow(sin(theta),4)*sq_tmp);
   
   return(value);
}

double rbs_kin(double m1,double m2,double theta,int *err)
{
   double sq_tmp,value;
   
   *err = FALSE;
   
   if(sin(theta) >= m2/m1){
      *err = TRUE;
      return(1.0);      
   }

   sq_tmp = sqrt(ipow2(m2) - ipow2(m1*sin(theta)));

   value = ipow2((sq_tmp + m1*cos(theta))/(m1 + m2));
   
   return(value);
}
