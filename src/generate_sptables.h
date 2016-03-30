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


/* These constants are important for SRIM stopping values */
#define NSTOP 500 /* number of points in the given energy range for calculating stopping powers, maximum is 1000 in SRModule */
#define MINVEL 0 /* min velocity used for calculating stopping powers (m/s) */
#define MAXVEL 3.0E7 /* max velocity used for calculating stopping powers (m/s) */
#define MAXELEMZ 83 /* number of elements, when generating stopping tables. SRModule uses SCOEF03 which has data to z = 92. */

/* Misc */
#define STABLE -1 /* for unstable elements */
#define SPATH "stopping_data" /* path for stopping data files (folder) */
#define SFEND ".esd" /* file format for stopping data files */


double calc_velocity(int index);
