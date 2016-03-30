#Linux makefile for mcerd.
#Version 17.12.2010.
#Type `make' to compile the program.

CC=gcc

CFLAGS=-O2 -DLinux
#LDFLAGS=

LIB=-lm `pkg-config --cflags --libs gtk+-2.0`

ZBLLIB=libzbl96.a

GTKEXTRALIB=libgtkextra-x11-2.0.a

PROG=mcerd

GUISRCS=check_buttons.c check_param.c detector_param.c dialogs.c espe_param.c file_handling.c \
foil_param.c format_values.c generate_sptables.c main.c menu.c mt19937ar.c \
presimu_param.c quit.c read_param.c recdist_param.c \
scale_param.c simulation_param.c target_param.c widgets.c write.c

SRCS=cross_section.c debug.c elsto.c erd_detector.c erd_scattering.c finalize.c finish_ion.c fpu.c \
init_detector.c init_params.c init_simu.c ion_simu.c misc.c output.c potential.c pre_simulation.c \
print_data.c random.c read_functions.c read_input.c rotate.c run_simu.c scattering_angle.c virtual_detector.c

ESPESRCS=get_espe.c plot_espe.c

$(PROG): $(GUISRCS) $(SRCS) $(ESPESRCS) $(ZBLLIB) $(GTKEXTRALIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $(GUISRCS) $(SRCS) $(ESPESRCS) $(ZBLLIB) $(GTKEXTRALIB) -o $(PROG) $(LIB)
