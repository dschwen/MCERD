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

/* Mersenne twister RNG. Returns a random number on [0,1]-real-interval.
 * Doesnt use seed number. */
double ran_mt(long *seed)
{
   return genrand_real1();
}

double rnd(double low,double high,int period,long seed)
{
   double value=0.0,length;
   
   length = high - low;
   
   if(length < 0.0){
      /* fatal_error("Random length negative or zero\n"); */
      error_dialog("Random length negative or zero");
      longjmp(err_buf,1);
   }
   
   switch(period){
      case RND_CLOSED:
         do {
            value = length*RANDOM_GENERATOR(&seed) + low;
         } while(value < low || value > high);
         break;
      case RND_OPEN:
         do {
            value = length*RANDOM_GENERATOR(&seed) + low;
         } while(value <= low || value >= high);
         break;      
      case RND_LEFT: /* left closed, right open */
         do {
            value = length*RANDOM_GENERATOR(&seed) + low;
         } while(value < low || value >= high);
         break;
      case RND_RIGHT: /* right closed, left open */
         do {
            value = length*RANDOM_GENERATOR(&seed) + low;
         } while(value <= low || value > high);
         break;
      case RND_SEED:
         init_genrand((unsigned long)seed);
         break;
      default:
      	value = low - high;
      	break;
   }


   if(value >= low && value <= high)
      return(value);
   else {
      /* fprintf(stderr,"%f %f %f %i %li\n",low,high,value,period,seed);
      fatal_error("Illegal value from random number generator\n"); */
      error_dialog("Illegal value from random number generator");
      longjmp(err_buf,1);
      return(-10.0);      /* Actually we should never get here */
   }

}
double gaussian(long seed)
{
   double value;
   
   value = gasdev(&seed);
   
   return(value);
}
double gasdev(long *idum)
{
   static int iset=0;     
   static double gset;     
   double fac,rsq,v1,v2;     

   if(iset == 0){
      do {
         v1 = 2.0*RANDOM_GENERATOR(idum) - 1.0;
         v2 = 2.0*RANDOM_GENERATOR(idum) - 1.0;
         rsq = v1*v1 + v2*v2;
      } while (rsq >= 1.0 || rsq == 0.0);
      fac = sqrt(-2.0*log(rsq)/rsq);
      gset=v1*fac;
      iset=1;
      return v2*fac;
   } else {
      iset=0;
      return gset;
   }
}

