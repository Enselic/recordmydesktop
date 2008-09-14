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

#include "config.h"

#include "rmd_yuv_utils.h"

// Keep these global (for performance reasons I assume).
unsigned char Yr[256], Yg[256], Yb[256],
              Ur[256], Ug[256], UbVr[256],
              Vg[256], Vb[256];

void MakeMatrices (void) {
    int i;

    for (i = 0; i < 256; i++)
        Yr[i] = (2104.0 * i) / 8192.0 + 8.0;
    for (i = 0; i < 256; i++)
        Yg[i] = (4130.0 * i) / 8192.0 + 8.0;
    for (i = 0; i < 256; i++)
        Yb[i] = (802.0 * i) / 8192.0;

    for (i = 0; i < 256; i++)
        Ur[i] = 37.8 - (1204.0 * i) / 8192.0 + 8.0;
    for (i = 0; i < 256; i++)
        Ug[i] = 74.2 - (2384.0 * i) / 8192.0 + 8.0;
    for (i = 0; i < 256; i++)
        UbVr[i] = (3598.0 * i) / 8192.0 ;

    for (i = 0; i < 256; i++)
        Vg[i] = 93.8 - (3013.0 * i) / 8192.0 + 8.0;
    for (i = 0; i < 256; i++)
        Vb[i] = 18.2 - (585.0 * i) / 8192.0 + 8.0;
}
