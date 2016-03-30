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



#include <stdio.h>
#include <stdlib.h>
#ifdef ALPHA
#include <cpml.h>
#endif

#define NINDEX 100

typedef struct {
   int t[NINDEX];
   double *x;
   double *y;
   double *a;
   double *b;
   double *c;
   int *d;
   int n;
} RDist;

void make_rand_dist(RDist *,double *,double *,int);
double rand_dist(RDist *);
