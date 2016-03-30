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


/* This file includes all extern variables for handling GUI parameters and limits for parameters */


#include <gtk/gtk.h>
#include <setjmp.h>

/* Misc constants */

/* TRUE and FALSE already defined in GTK+ */
/*
#define TRUE 1
#define FALSE 0
*/
#define ERROR_OPEN "Could not open file "   /* Message text when opening file fails */

#define TYPE_TARGET 0    /* For reading layers */
#define TYPE_FOIL 1

#define TYPE_SAVE 1      /* For saving */
#define TYPE_SAVEAS 2

#define TYPE_REMOVE 1    /* For target/foil layers */
#define TYPE_ADD 2

#define PLOT_SIM 1       /* Which type of plot is used (simulated or experimental) */
#define PLOT_EXP 2

#define TYPE_SIMU 1      /* Opening an existing plot or simulating */
#define TYPE_OPEN 2

#define TYPE_PLOT 1      /* Exporting a plot or canvas */
#define TYPE_CANVAS 2

#define HAND_NORMAL 1    /* Types for hand tool */
#define HAND_SELECT 2
#define HAND_ZOOM 3

#define BIG_VALUE 1e10 /* Used for scaling energy spectrum */


/* Limits for the parameters (in units which is used in GUI) */

/* misc */
#define MAX_LAYERS 10       /* Max target layers */
#define MAX_ELEMENTS 10     /* Max elements per layer */
#define MAX_FOILS 10        /* Max detector foils */
#define MAX_FELEMENTS 5     /* Max elements per foil */
#define MAX_RDIST 5         /* Max recoil distribtution points (before simulating) */
#define MAX_PRESIMU MAX_LAYERS      /* Max pre simulation points, actually same as max target layers */
#define MAX_CH 10000        /* Max channel number (x-axis in the plot) when changing range */
#define MIN_SCALE 0.1       /* Min scale (keV/channel) */
#define MAX_SCALE 10000     /* Max scale (keV/channel) */
#define MIN_DISTP 2         /* Min number of distribution points (in distribution graph) */
#define MAX_DISTP 20        /* Max number of distribution points (in distribution graph) */
#define DEF_SCALE 1000      /* How many keV/channel is used in default (in the files from get_espe) */

/* detector */
#define MIN_DETANGLE 0      /* Detector angle */
#define MAX_DETANGLE 360
#define MIN_VDETSIZE 0      /* Virtual detector size */
#define MAX_VDETSIZE 100
#define MIN_TDETNUM 1       /* Timing detector numbers */
#define MAX_TDETNUM MAX_FOILS

/* layers */
#define MIN_DIAM 0          /* Diameter */
#define MAX_DIAM 1000
#define MIN_DIST 0          /* Distance */
#define MAX_DIST 10000
#define MIN_D -1            /* Thickness, negative values mean "bulk" thickness */
#define MAX_D 10000
#define MIN_DENS 0          /* Density */
#define MAX_DENS 100
#define MIN_M 0             /* Mass */
#define MAX_M 300
#define MIN_CON 0           /* Concentration */
#define MAX_CON 1

/* simulation */
#define MIN_TARGANGLE 0     /* Targer angle */
#define MAX_TARGANGLE 90
#define MIN_E 0             /* Energy */
#define MAX_E 500
#define MIN_BEAMSIZE 0      /* Beam spot size */
#define MAX_BEAMSIZE 100
#define MIN_SANGLE 0        /* Minimum scattering angle */
#define MAX_SANGLE 1
#define MIN_RECAVE 1        /* Average recoils per primary ion */
#define MAX_RECAVE 100
#define MIN_IONS 0          /* Number of ions */
#define MAX_IONS 10000000
#define MIN_SEED 100        /* Seed number, length has to be 3 numbers */
#define MAX_SEED 999

/* recoil distribution */
#define MAX_DISTR 1         /* Max value for distribution */

/* pre simulation (recoiling solid angle fitted parameters) */
#define MIN_PREA -10
#define MAX_PREA 10
#define MIN_PREB -100
#define MAX_PREB 100

/* Espe constants, some of them is taken from original get_espe.c */
#define NLINE 200
#define NDDIST 500          /* Max number of distribution points (get_espe) */
#define MAX_POINTS 1000     /* Max number of data points in energy spectrum graph*/
#define SEED_ESPE 101       /* seed number for RNG in get_espe() */

#define MINNDEPTH 0         /* Number of depth channels in virtual scaling */
#define MAXNDEPTH 100
#define MIN_DSIZE 0         /* dx, dy */
#define MAX_DSIZE 1000
#define MIN_DEPTH 0         /* dwindow depth */
#define MAX_DEPTH 10000
#define MIN_TRES 0          /* time resolution */
#define MAX_TRES 10
#define MIN_ERES 0          /* energy resolution */
#define MAX_ERES 1
#define MIN_CHWIDTH 0       /* channel width */
#define MAX_CHWIDTH 1
#define MIN_TOFLEN 0        /* TOF length */
#define MAX_TOFLEN 50
#define MIN_SCALEY 0        /* scaling */
#define MAX_SCALEY 50

/* Units (used in GUI) */
#define U_ENERGY "MeV"
#define U_ANGLE "deg"
#define U_SIZE "mm"
#define U_THICKNESS "nm"
#define U_DENSITY "g/cm3"
#define U_MASS "u"

/* For filenames */
#define FDEFAULT "default.dat" /* Includes default parameters */
#define FEND ".mcerd"          /* File format for the program */
#define FTARGET ".targ"        /* Target file */
#define FDETECTOR ".det"       /* Detector file */
#define FFOIL ".foil"          /* Detector foils file */
#define FRECOIL ".rec"         /* Recoil distribution file */
#define FPRE ".pre"            /* Pre simulation file */
#define FERD ".erd"            /* Output data file */
#define FESPE ".espe"          /* Energy spectrum parameters file */
#define FSPE ".spe"            /* Energy spectrum file*/
#define FDIST ".dist"          /* File for distribution points*/

/* File parameters */
extern char *file_path;        /* Path of the file for open/save */
extern char *sim_plot;         /* Path for the simulation plot */
extern char *plot_path;        /* Path for the opened/experimental plot */

/* Misc parameters */
extern int saved;              /* For checking has user saved or not */
extern int error;              /* 1, when error occurs. For testing errors */
extern int ion_num;            /* Number of different ions in the simulation (1 or 2) */
extern jmp_buf err_buf;        /* Memory buffer when simulation fails */
extern GtkWidget *progress;    /* Progress bar */
extern double default_scale;   /* For the default scale (keV/ch) */
extern double exp_scale;       /* Scale for the experimental data */
extern int n_distpoints;       /* Number of distribution points */

/* Simulation parameters */
extern int sim_type;            /* Type of the simulation (ERD or RBS) */
extern const char *beam_ion;    /* Beam ion */
extern double beam_energy;      /* Energy of the primary ion beam */
extern const char *recoil_atom; /* Recoiling atom */
extern double target_angle;     /* Angle between target surface normal and beam */
extern double beam_size_x;      /* Size of the beam spot */
extern double beam_size_y;
extern double min_angle;        /* Minimum angle of the scattering */
extern double min_energy;       /* Minimum energy of the simulation */
extern int aver_nions;          /* Average number of recoils per primary ion */
extern int angle_width;         /* Recoiling angle width type (wide or narrow) */
extern int nions;               /* Total number of the simulated ions */
extern int nions_presim;        /* Number of simulated ions the the presimulation */
extern unsigned int nseed;      /* Seed number of the random number generator */

/* Detector parameters */
extern int det_type;      /* Detector type (TOF or gas) */
extern double det_angle;  /* Detector angle relative to the beam direction */
extern double vdet_size1; /* Size of the virtual detector relative to the real */
extern double vdet_size2;
extern int det_num1;      /* Layer numbers for the timing detectors */
extern int det_num2;

/* Target parameters */
extern int nlayers;                          /* Number of layers in the target */
/* Tables for the properties of each layer */
extern double layer_thickness[];             /* Thickness */
extern double layer_density[];               /* Density */
extern const char *stop_ion_layer[];         /* Stopping power for the primary ion */
extern const char *stop_rec_layer[];         /* Stopping power for the recoil atom */
extern double mass_layer[][MAX_LAYERS];      /* Masses of elements */
extern const char *elem_layer[][MAX_LAYERS]; /* Elements */
extern double con_layer[][MAX_LAYERS];       /* Element concentration */

/* Foil parameters */
extern int nfoils;                         /* Number of detector foils */
/* Tables for the properties of each foil */
extern int foil_type[];                    /* Detector foil type (circular or rectangular) */
extern double foil_diameter[][MAX_FOILS];  /* Foil diameter, 1 value for circular and 2 values for rectangular */
extern double foil_distance[];             /* Foil distance */
extern double foil_thickness[];            /* Foil thickness */
extern const char *stop_ion_foil[];        /* Stopping power for the secondary ion */
extern const char *stop_rec_foil[];        /* Stopping power for the recoil atom */
extern double foil_density[];              /* Foil density */
extern const char *elem_foil[][MAX_FOILS]; /* Elements */
extern double con_foil[][MAX_FOILS];       /* Element concentration */
extern double mass_foil[][MAX_FOILS];      /* Masses of elements */

/* Recoil material distribution parameters */
extern double rdist_x[];                   /* Depth array */
extern double rdist_y[];                   /* Distribution array */

/* Pre simulation parameters */
extern double presimu_x[];                 /* recoiling solid angle fitted parameters */
extern double presimu_y[];
extern int use_presimu;                    /* Using presimu (=1) or not (=0) */

/* Espe parameters (for get_espe.c) */
typedef struct {

  int virtual; /* virtual (TRUE) or real (FALSE) */
  double vsize; /* virtual detector size */
  int vnum; /* number of depth channels in virtual scaling */
  
  /* if dsize is limited (TRUE) and dx, dy size */
  int dsize;
  double dx;
  double dy;
  
  int pnvd;
  
  /* wide (TRUE), narrow (FALSE) */
  int wide;
  
  /* if depth is limited (TRUE) and limits */
  int dwindow;
  double dmin;
  double dmax;
  
  /* if using distribution points (TRUE) */
  int ddist;
  
  /* if using mass (TRUE) and value for mass */
  int mass;
  double mass_value;
  
  /* time resolution (TRUE), energy resolution (FALSE) and values for them */
  int timeres;
  double tres;
  double eres;
  
  double toflen; /* TOF length */
  double chwidth; /* channel width */
  double scale; /* y scale */
  
} Espe;

extern Espe espe;
