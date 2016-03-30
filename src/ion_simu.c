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

int recdist_nonzero(double *,Ion *,Target *);

void create_ion(Global *global,Ion *ion,Detector *detector)
{
/*
 *     Create the ion ie. calculate and give all the parameters for the
 *     new ion (primary or secondary).
 *
 *     About coordinate systems: 
 *       - laboratory coordinates: z-axis is along the beam line
 *       - target: z-axis is along the target normal
 *       - ion: z-axis is along the direction of ion movement
 *       - detector: z-axis is along the detector direction
 */
 
   Point pt;  
   double theta,fii;

   switch(global->simtype){
      case SIM_ERD:
         /*
          *   Here we also give the random hit point to the target surface.
          */
         pt.x = rnd(-global->bspot.x,global->bspot.x,RND_CLOSED,RND_CONT);
         pt.y = rnd(-global->bspot.y,global->bspot.y,RND_CLOSED,RND_CONT);
         if(global->beamangle > 0)
            pt.z = pt.x/tan(PI/2.0 - global->beamangle);
         else
            pt.z = 0.0;
         break;
      case SIM_RBS:
         /*
          *   Here we also give the random hit point to the target surface.
          */
         pt.x = rnd(-global->bspot.x,global->bspot.x,RND_CLOSED,RND_CONT);
         pt.y = rnd(-global->bspot.y,global->bspot.y,RND_CLOSED,RND_CONT);
         if(global->beamangle > 0)
            pt.z = pt.x/tan(PI/2.0 - global->beamangle);
         else
            pt.z = 0.0;
         break;
      default:
         /*fatal_error("no such simulation type\n"); */
         error_dialog("Invalid simulation type");
         longjmp(err_buf,1);
      	 break;
   }

   ion->lab.p = pt;

   theta = global->beamangle;
   fii = 0.0;

   ion->tlayer = 0;
   ion->time = 0.0;
   ion->w = 1.0;      /* Original ion weight is 1 */
   ion->p.x = 0.0;
   ion->p.y = 0.0;
   ion->p.z = 0.0;
   ion->E = global->E0;
   ion->nsct = 0;
   ion->theta = theta;
   ion->fii = fii;
   ion->opt.cos_theta = cos(theta);
   ion->opt.sin_theta = sin(theta);
   ion->type = PRIMARY;
   ion->status = NOT_FINISHED;
   
   ion->lab.theta = global->beamangle;
   ion->lab.fii = PI;
   
}
void next_scattering(Global *global,Ion *ion,Target *target,
                     Scattering scat[][MAXELEMENTS],SNext *snext)
{
/*
 *     Calculate the distance to the next scattering point and determine 
 *     the scattering atom. We loop through the atoms in the current layer.
 */
 
   Target_layer *layer;
   double cross=0.0,b[MAXATOMS],rcross;
   int i,p;

   layer = &(target->layer[ion->tlayer]);

   for(i=0;i<layer->natoms;i++){
      p = layer->atom[i];
      b[i] = layer->N[i]*get_cross(ion,&(scat[ion->type][p]));
      cross += b[i];
   }

   rcross = rnd(0.0,cross,RND_OPEN,RND_CONT);

   i = 0;
   while(i < layer->natoms && rcross >= 0.0){
      rcross -= b[i];
      i++;
   }
   if(i < 1 || i > layer->natoms){
      /* fatal_error("Scattering atom calculated wrong\n"); */
      error_dialog("Scattering atom calculated wrong");
      longjmp(err_buf,1);
   }

   i--;

   snext->natom = layer->atom[i];

   ion->opt.y = sqrt(-rcross/(PI*layer->N[i]))/scat[ion->type][snext->natom].a;
/*
   printf("%5.2f %5.2f %5.2f %7.5f %10.4f %15.7f\n",
         ion->Z,ion->A/C_U,target->ele[snext->natom].Z,
         target->ele[snext->natom].A/C_U,ion->E/C_MEV,
         sqrt((b[i]/layer->N[i])/PI)/C_ANGSTROM);
*/
   snext->d = -log(rnd(0.0,1.0,RND_RIGHT,RND_CONT))/(cross);

}
int move_ion(Global *global,Ion *ion,Target *target,SNext *snext)
{
/*
 *     Move ion to the next point, which is the closest of following
 *       - next MC-scattering point
 *       - point where the relative electronic stopping power has
 *         changed more than MAXELOSS
 *       - next crossing of the target layer
 *       - next crossing of the recoil material distribution array
 *       - next ERD scattering
 *
 *     Default is that we proceed to the next MC scattering
 *
 *     To make things fast we do not evaluate all these values at the
 *     beginning but with a stepwise procedure
 *      (since we are in the innermost loop we have to worry about the speed)
 */

   Target_layer *layer;
   double stopping,d,nextz,vel,vel1,vel2,sto1,sto2,dE,drec,straggling,eloss;
   double recang;
   int cross_layer=FALSE,cross_recdist=FALSE,sto_dec=FALSE,sc=MC_SCATTERING,nz;
   int n;
   
   d = snext->d;
   nextz = d*ion->opt.cos_theta + ion->p.z;
   layer = &(target->layer[ion->tlayer]);

   if(nextz < layer->dlow || nextz > layer->dhigh){
      sc = NO_SCATTERING;
      if(nextz < layer->dlow){
         d = fabs((layer->dlow - ion->p.z)/ion->opt.cos_theta);
         cross_layer = -1;
      } else {
         d = fabs((ion->p.z - layer->dhigh)/ion->opt.cos_theta);
         cross_layer = 1;
      }
   }
         
   if(ion->type == PRIMARY){
      drec = d;
      nz = recdist_nonzero(&drec,ion,target);
      if(drec < d){
         sc = NO_SCATTERING;
         cross_recdist = TRUE;
         cross_layer = FALSE;
         d = drec;
      }
      if(nz){
         drec = -log(rnd(0.0,1.0,RND_RIGHT,RND_CONT));
/*
         drec *= (target->effrecd/(cos(global->beamangle)*global->nrecave));
*/
         if(global->recwidth == REC_WIDE || global->simstage == PRESIMULATION){
            drec *= (target->effrecd/(cos(global->beamangle)*global->nrecave));
	    ion->wtmp = -1.0;
         } else {
            n = ion->tlayer;
            recang = ipow2(target->recpar[n].x * ion->p.z + 
                           target->recpar[n].y);
            ion->wtmp = target->angave/recang;
/*            printf("wtmp: %10.3f %12.8f\n",ion->p.z/C_NM,ion->wtmp);  */
            drec *= (target->effrecd/(cos(global->beamangle)*global->nrecave))/
                    (recang/target->angave);
         }
         if(drec < d){
            d = drec;
            sc = ERD_SCATTERING;
            cross_layer = FALSE;
            cross_recdist = FALSE;
/*
            if(ion->theta > 90.0*C_DEG){
               fprintf(stderr,"recoiling by outgoing primary ion!\n");
               fprintf(stderr,"theta: %10.3f°\ndepth: %10.3f nm\n",
                       ion->theta/C_DEG,ion->p.z/C_NM);
            }
*/
         }
      }
   }
   
   vel1 = sqrt(2.0*ion->E/ion->A);
   sto1 = inter_sto(&(layer->sto[ion->type]),vel1,STOPPING); 

   dE = sto1*d;
   
   vel2 = sqrt(2.0*(max(ion->E - dE,0.0))/ion->A);
   sto2 = inter_sto(&(layer->sto[ion->type]),vel2,STOPPING);

   while(fabs(sto1 - sto2)/sto1 > MAXELOSS || dE >= ion->E){
      sc = NO_SCATTERING;
      cross_layer = FALSE;
      cross_recdist = FALSE;
      sto_dec = TRUE;
      d /= 2.0;

      dE = sto1*d;
      vel2 = sqrt(2.0*(max(ion->E - dE,0.0))/ion->A);
      sto2 = inter_sto(&(layer->sto[ion->type]),vel2,STOPPING);            
   }

   stopping = 0.5*(sto1 + sto2);
   vel = 0.5*(vel1 + vel2);

   if(cross_layer || cross_recdist)
      d += 0.001*C_ANGSTROM; /* To be sure that the layer surface is crossed */

   if(cross_recdist){
    /*  
     *  We must check whether we cross targe layer and recoil distribution
     *  layer at the same time!
     */
      nextz = d*ion->opt.cos_theta + ion->p.z;
      if(nextz < layer->dlow)
         cross_layer = -1;
      if(nextz > layer->dhigh)
         cross_layer = 1;
   }

   if(cross_layer){
      (ion->tlayer) += cross_layer;
   }
   
   straggling = sqrt(inter_sto(&(layer->sto[ion->type]),vel,STRAGGLING)*d);

   /* we always want a positive energy loss, ie. we don't increase ion energy */

   straggling *= gaussian(RND_CONT);

#ifdef NO_STRAGGLING
   straggling = 0.0;
#endif

   eloss = d*stopping;

   if(fabs(straggling) < eloss)
      eloss += straggling;

#ifdef NO_ELOSS
   eloss = 0.0;
#endif

   ion->E -= eloss;
   ion->E = max(ion->E,0.001*C_EV);
   
   ion->p.x += d*ion->opt.sin_theta*cos(ion->fii);
   ion->p.y += d*ion->opt.sin_theta*sin(ion->fii);
   ion->p.z += d*ion->opt.cos_theta;

   ion->time += d/vel;

   if(cross_layer && ion->tlayer > target->ntarget)
      ion->status = FIN_OUT_DET; 

   if(cross_layer && ion->type == SECONDARY && ion->tlayer == target->ntarget)
      ion->status = FIN_RECOIL;

   ion_finished(global,ion,target);
   

#ifdef DEBUG
   if(cross_layer)
      print_ion_position(global,ion,"L",ANYSIMULATION);
   else if(sto_dec)
      print_ion_position(global,ion,"S",ANYSIMULATION);
   else if(cross_recdist)
      print_ion_position(global,ion,"D",ANYSIMULATION);
   else if(sc == ERD_SCATTERING)
      print_ion_position(global,ion,"R",ANYSIMULATION);
   else
      print_ion_position(global,ion,"M",ANYSIMULATION);
#endif

   return(sc);      
}
double inter_sto(Target_sto *stop,double vel,int mode)
{
/*
 *    Interpolate the electronic stopping power or straggling value.
 *    The array is assumed to have constant spacing, so we can calculate
 *    the index.
 */
 
   int i=0;
   double sto;
   double d,vlow,slow,shigh;
   
   d = stop->stodiv;

   if(vel >= stop->vel[stop->n_sto-1]){
      fprintf(stderr,"Warning: ion velocity exceeds the maximum velocity of stopping power table\n");
      /* error_dialog("Warning: ion velocity exceeds the maximum velocity of stopping power table"); */
      if(mode == STOPPING)
         return(stop->sto[stop->n_sto-1]);
      else /* straggling */
         return(stop->stragg[stop->n_sto-1]);
   }
   if(vel <= stop->vel[0]){
      if(mode == STOPPING)
         return(stop->sto[0]);
      else /* straggling */
         return(stop->stragg[0]);
   }

   i = (int) (vel*d);

   vlow = stop->vel[i];
   if(mode == STOPPING) {
      slow = stop->sto[i];
      shigh = stop->sto[i+1];
   } else {
      slow = stop->stragg[i];
      shigh = stop->stragg[i+1];
   }
   
   sto = slow + (shigh - slow)*(vel - vlow)*d;
   
   return(sto);
      
}
int ion_finished(Global *global,Ion *ion,Target *target)
{
/*
 *     Check all the conditions and decide whether to finish the ion.
 */
   
   if(ion->E < global->emin)
      ion->status = FIN_STOP;

   if(ion->type == PRIMARY && ion->tlayer >= target->ntarget)
      ion->status = FIN_TRANS;
      
   if(ion->tlayer < 0){
      if(ion->type == PRIMARY)
         ion->status = FIN_BS;
      else
         ion->status = FIN_RECOIL;
   }
   
   if(ion->type == PRIMARY && ion->p.z > target->recmaxd)
      ion->status = FIN_MAXDEPTH;

   if(ion->type == SECONDARY && ion->tlayer >= target->nlayers)
      ion->status = FIN_DET;

/*
 *  Here we should also check the case that the recoil comes out of the
 *  detector layer from sides.
 */

   return(ion->status);
}

void mc_scattering(Global *global,Ion *ion,Target *target,
                   Scattering scat[][MAXELEMENTS],SNext *snext)
{
/*
 *   Normal elastic scattering from a potential during the slowing down process 
 */
 
   Scattering *s;
   double theta,fii,cos_theta_cm,sin_theta_cm,sin_theta,cos_theta,n,targetA,Ef;
   double angle;
   
   targetA = target->ele[snext->natom].A;

   s = &(scat[ion->type][snext->natom]);

   ion->opt.e = ion->E*s->E2eps;

   if(ion->E < global->emin)
      return;

   n = ion->A/targetA;

   angle = get_angle(s,ion);

   cos_theta_cm = cos(angle);
   sin_theta_cm = sin(angle);

   theta = atan2(sin_theta_cm,cos_theta_cm + n);
   cos_theta = cos(theta);
   sin_theta = sin(theta);

   Ef = ipow2((sqrt(ipow2(targetA) - ipow2(ion->A*sin_theta)) +
              ion->A*cos_theta)/(ion->A + targetA));

#ifdef SCAT_ANAL
   if(ion->nsct < NMAXSCT)
      ion->sct_angle[ion->nsct] = theta;
   else{
      fprintf(stderr,"Number of scatterings exceeds the reserved space (%i)\n",NMAXSCT);
      /* error_dialog("Number of scatterings exceeds the reserved space"); */
   }
#endif

#ifdef NO_ETRANS
   Ef = 1.0;
#endif

#ifdef NO_ANGLES
   cos_theta = 1.0;
   sin_theta = 0.0;
#endif

#ifdef NO_PRI_ANGLES
   if(ion->type == PRIMARY){
      cos_theta = 1.0;
      sin_theta = 0.0;
   }
#endif

#ifdef NO_SEC_ANGLES
   if(ion->type == SECONDARY){
      cos_theta = 1.0;
      sin_theta = 0.0;
   }
#endif

   ion->E *= Ef;
   ion->opt.e = ion->E*s->E2eps;

   fii = rnd(0.0,2.0*PI,RND_RIGHT,RND_CONT);

   ion_rotate(ion,cos_theta,fii);

   ion->nsct++;
      
}
double get_angle(Scattering *scat,Ion *ion)
{
/*
 *   Interpolate the cosinus of the scattering angle according to the
 *   ion energy and impact parameter.
 */

   double y,e,angle,tmp0,tmp1,tmp2;
   double elow,ehigh,ylow,yhigh,angle11,angle12,angle21,angle22;
   double angle2;

   int i=1,j=1;

   y = ion->opt.y;
   e = ion->opt.e;

   i = (int) ((log(e) - scat->logemin)*scat->logediv) + 1;
   j = (int) ((log(y) - scat->logymin)*scat->logydiv) + 1;
   
   if(i > (EPSNUM - 2)){
      fprintf(stderr,"Warning: energy exceeds the maximum of the scattering table energy\n");
      /* error_dialog("Warning: energy exceeds the maximum of the scattering table energy"); */
      return(1.0);
   }
   if(i < 1){
      fprintf(stderr,"Warning: energy is below the minimum of the scattering table energy\n");
      /* error_dialog("Warning: energy is below the minimum of the scattering table energy"); */
      return(1.0);
   }
   if(j < 1){
      fprintf(stderr,"Warning: impact parameter is below the minimum of the scattering table value\n");
      /* error_dialog("Warning: impact parameter is below the minimum of the scattering table value"); */
      return(-1.0);
   }
   if(j > (YNUM - 2)){
/*
      fprintf(stderr,"Warning: impact parameter exceeds the maximum of the scattering table value: %i\n",j);
      printf("y: %10.3f\n",y*scat->a/C_ANGSTROM);
*/
      return(1.0);
   }

   ylow  = scat->angle[0][j];
   yhigh = scat->angle[0][j+1];
   elow  = scat->angle[i][0];
   ehigh = scat->angle[i+1][0];

   angle11 = scat->angle[i][j];
   angle12 = scat->angle[i][j+1];
   angle21 = scat->angle[i+1][j];
   angle22 = scat->angle[i+1][j+1];

   tmp0 = 1.0/(yhigh - ylow);

   tmp1 = angle11 + (y - ylow)*(angle12 - angle11)*tmp0;
   tmp2 = angle21 + (y - ylow)*(angle22 - angle21)*tmp0;

   angle = tmp1 + (e - elow)*(tmp2 - tmp1)/(ehigh - elow);
/*
   printf("%10.6f %10.6f\n%10.6f %10.6f\n",angle21,angle22,angle11,angle12);
   printf("%10.6f %10.6f %10.6f %10.6f\n",ehigh,ylow,ehigh,yhigh);
   printf("%10.6f %10.6f %10.6f %10.6f\n",elow,ylow,elow,yhigh);
   printf("%10.6f %10.6f %10.6f\n\n",e,y,angle);
   
   angle2 = scattering_angle(scat->pot,ion);
   printf("ANG: %10.6f %10.6f\n",angle/C_DEG,angle2/C_DEG);
*/
   return(angle);

}
void ion_rotate(Ion *p,double cos_theta,double fii)
{
   double x,y,z,rx,ry,rz;
   double cosa1,cosa2,cosa3,sina1,sina2,sina3;  

   double sin_theta,sin_fii,cos_fii;

   /* Change this to use the general rotate subroutine!! */
 
   sin_theta = sqrt(fabs(1.0 - cos_theta*cos_theta));
   cos_fii = cos(fii);
   sin_fii = sin(fii);

   x = sin_theta*cos_fii;
   y = sin_theta*sin_fii;
   z = cos_theta;

   cosa1 = p->opt.cos_theta;
   sina1 = sqrt(fabs(1.0 - cosa1*cosa1));

   sina2 = sin(p->fii + PI/2.0);
   cosa2 = cos(p->fii + PI/2.0);

   cosa3 = cosa2;
   sina3 = -sina2;

   rx = x*( cosa3*cosa2 - cosa1*sina2*sina3) +
        y*(-sina3*cosa2 - cosa1*sina2*cosa3) +
        z*sina1*sina2;

   ry = x*( cosa3*sina2 + cosa1*cosa2*sina3) +
        y*(-sina3*sina2 + cosa1*cosa2*cosa3) -
        z*sina1*cosa2;

   rz = x*sina1*sina3 +
        y*sina1*cosa3 +
        z*cosa1;

   p->opt.cos_theta = rz;
   p->opt.sin_theta = sqrt(fabs(1.0 - ipow2(rz)));

   p->theta = acos(rz);
   p->fii = atan2(ry,rx);
   if(p->fii < 0.0)
      p->fii += 2.0*PI;

}
int recdist_nonzero(double *dist,Ion *ion,Target *target)
{
/*
 *    This subroutine checks whether the recoil material distribution
 *    is nonzero at point of ion position, and whether the ion crosses
 *    a recoil distribution layer when moving a distance of dist.
 *    In the case of crossing dist return the distance to the crossing
 *    point.
 */
    
   double z1,z2;
   int n,i1=0,i2=0;

   z1 = ion->p.z; 
   z2 = *dist*ion->opt.cos_theta + ion->p.z;

   n = target->nrecdist;

   while(i1 < n && z1 > target->recdist[i1].x)
      i1++;

   while(i2 < n && z2 > target->recdist[i2].x)
      i2++;

   if(i2 > i1)
      *dist = (target->recdist[i1].x - z1)/ion->opt.cos_theta;
   else if(i1 < i2)
      *dist = (z1 - target->recdist[i1-1].x)/ion->opt.cos_theta;      

   if(i1 == 0 || i1 == n)
      return(FALSE);

   if(target->recdist[i1-1].y > 0.0 || target->recdist[i1].y > 0.0)
      return(TRUE);
   else
      return(FALSE);

}
