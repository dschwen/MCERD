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


/* This is working only in Linux */

#if 0

#include <fpu_control.h>

/*
 *  For intel-compatible fpu-processors default mode is 0x137f which
 *  means that all the interrupts are masked. With value 0x1372 invalid
 *  operation, zero division, overflow and underflow are not masked and
 *  the program crashes when any of these occurs. This is of course a
 *  Good Thing.
 */

#define FPU_MODE 0x1372
#define FPU_MASK 0x137f

void fpu(void);
void fpu_mask(void);

void fpu(void)
{
   fpu_control_t fpu_cw = FPU_MODE;

   _FPU_SETCW(fpu_cw);

}

void fpu_mask(void)
{
   fpu_control_t fpu_cw = FPU_MASK;

   _FPU_SETCW(fpu_cw);

}

#endif
