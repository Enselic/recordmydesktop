/******************************************************************************
*                            recordMyDesktop                                  *
*******************************************************************************
*                                                                             *
*            Copyright (C) 2006,2007,2008 John Varouhakis                     *
*                                                                             *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 2 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with this program; if not, write to the Free Software               *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
*                                                                             *
*                                                                             *
*                                                                             *
*   For further information contact me at johnvarouhakis@gmail.com            *
******************************************************************************/

#ifndef RMDMACRO_H
#define RMDMACRO_H 1

#include "config.h"

#include "rmd_types.h"

#include <limits.h>


//define which way we are reading a pixmap
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define __ABYTE 3
#define __RBYTE 2
#define __GBYTE 1
#define __BBYTE 0

#elif __BYTE_ORDER == __BIG_ENDIAN

#define __ABYTE 0
#define __RBYTE 1
#define __GBYTE 2
#define __BBYTE 3

#else
#error Only little-endian and big-endian systems are supported
#endif

#define __RVALUE_32(tmp_val) (((tmp_val)&0x00ff0000)>>16)
#define __GVALUE_32(tmp_val) (((tmp_val)&0x0000ff00)>>8)
#define __BVALUE_32(tmp_val) (((tmp_val)&0x000000ff))

#define __R16_MASK  0xf800
#define __G16_MASK  0x7e0
#define __B16_MASK  0x1f

#define __RVALUE_16(tmp_val) ((((tmp_val)&__R16_MASK)>>11)*8)
#define __GVALUE_16(tmp_val) ((((tmp_val)&__G16_MASK)>>5)*4)
#define __BVALUE_16(tmp_val) ((((tmp_val)&__B16_MASK))*8)

//xfixes pointer data are written as unsigned long
//(even though the server returns CARD32)
//so we need to set the step accordingly to
//avoid problems (amd64 has 8byte ulong)
#define RMD_ULONG_SIZE_T (sizeof(unsigned long))

//The width, in bytes, of the blocks
//on which the y,u and v planes are broken.
//These blocks are square.
#define Y_UNIT_WIDTH    0x0010
#define UV_UNIT_WIDTH   0x0008

#endif

