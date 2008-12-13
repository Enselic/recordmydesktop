/******************************************************************************
*                      recordMyDesktop - rmd_yuv_utils.c                      *
*******************************************************************************
*                                                                             *
*            Copyright (C) 2006,2007,2008 John Varouhakis                     *
*            Copyright (C) 2008 Luca Bonavita                                 * 
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

#include "config.h"
#include "rmd_yuv_utils.h"

#include "rmd_math.h"


// Keep these global (for performance reasons I assume).
unsigned char Yr[256], Yg[256], Yb[256],
              Ur[256], Ug[256], UbVr[256],
              Vg[256], Vb[256];

// FIXME: These globals are modified in other source files! We keep
// thsee here for now. These are the cache blocks. They need to be
// accesible in the dbuf macros
u_int32_t *yblocks,
          *ublocks,
          *vblocks;

void MakeMatrices (void) {
    
    int i;
 
 	/* assuming 8-bit precision */
 	float Yscale = 219.0, Yoffset = 16.0;
 	float Cscale = 224.0, Coffset = 128.0;
 	float RGBscale = 255.0;
 
 	float r, g, b;
 	float yr, yg, yb;
 	float ur, ug, ub;
 	float     vg, vb;	/* vr intentionally missing */
 
 	/* as for ITU-R BT-601-6 specifications: */
 	r = 0.299;
 	b = 0.114;
 	g = 1.0 - r - b;
 
 	/*	as a note, here are the coefficients
 		as for ITU-R BT-709 specifications:
 		r=0.2126;	b=0.0722;	g=1.0-r-b; */
 
 	yr = r * Yscale / RGBscale;
 	yg = g * Yscale / RGBscale;
 	yb = b * Yscale / RGBscale;
 	ur = ( -0.5 * r / ( 1 - b ) ) * Cscale / RGBscale;
 	ug = ( -0.5 * g / ( 1 - b ) ) * Cscale / RGBscale;
 	ub = ( 0.5 * Cscale / RGBscale);
 	/* vr = ub so UbVr = ub*i = vr*i */
 	vg = ( -0.5 * g / ( 1 - r ) ) * Cscale / RGBscale;
 	vb = ( -0.5 * b / ( 1 - r ) ) * Cscale / RGBscale;
 
     for( i = 0 ; i < 256 ; i++ ) {

         Yr[i] = (unsigned char) rmdRoundf( Yoffset + yr * i );
         Yg[i] = (unsigned char) rmdRoundf( yg * i );
         Yb[i] = (unsigned char) rmdRoundf( yb * i );

         Ur[i] = (unsigned char) rmdRoundf( Coffset + ur * i );
         Ug[i] = (unsigned char) rmdRoundf( ug * i );
         UbVr[i] = (unsigned char) rmdRoundf( ub * i );

         Vg[i] = (unsigned char) rmdRoundf( vg * i );
         Vb[i] = (unsigned char) rmdRoundf( Coffset + vb * i );

    }

}

