:: windows bat-file for compiling mcerd. Run winmake_mcerd to compile.
:: version 17.12.2010
::
@ECHO OFF
pkg-config --cflags --libs gtk+-2.0 >gtkinclude.txt
SET /p gtkinclude=<gtkinclude.txt
SET srcs=cross_section.c debug.c elsto.c erd_detector.c erd_scattering.c finalize.c finish_ion.c init_detector.c init_params.c init_simu.c ion_simu.c misc.c output.c potential.c pre_simulation.c print_data.c random.c read_functions.c read_input.c rotate.c run_simu.c scattering_angle.c virtual_detector.c
SET guisrcs=check_buttons.c check_param.c detector_param.c dialogs.c espe_param.c file_handling.c foil_param.c format_values.c generate_sptables.c main.c menu.c mt19937ar.c presimu_param.c quit.c read_param.c recdist_param.c scale_param.c simulation_param.c target_param.c widgets.c write.c
SET espesrcs=get_espe.c plot_espe.c
SET libs=-lm
SET zbllib=libzbl96.a
SET gtkextralib=libgtkextra-win32-2.1.a
SET flags=-O2 -Wl,--stack,16777216
SET program=mcerd
ECHO.
ECHO compiling mcerd binary..
ECHO.
gcc %flags% %srcs% %guisrcs% %espesrcs% %zbllib% %gtkextralib% -o %program% %libs% %gtkinclude%
ECHO.
SET gtkinclude=
SET srcs=
SET guisrcs=
SET espesrcs=
SET libs=
SET zbllib=
SET gtkextralib=
SET flags=
SET program=
ECHO.
ECHO compiling generate_sptables binary..
ECHO.
SET spsrcs=generate_sptables.c main_generate.c
SET spflags=-O2
SET spprogram=generate_sptables
gcc %spflags% %spsrcs% -o %spprogram%
SET spsrcs=
SET spflags=
SET spprogram=
ECHO mcerd ready.
