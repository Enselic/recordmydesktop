/*********************************************************************************
*                             recordMyDesktop                                    *
**********************************************************************************
*                                                                                *
*             Copyright (C) 2006  John Varouhakis                                *
*                                                                                *
*                                                                                *
*    This program is free software; you can redistribute it and/or modify        *
*    it under the terms of the GNU General Public License as published by        *
*    the Free Software Foundation; either version 2 of the License, or           *
*    (at your option) any later version.                                         *
*                                                                                *
*    This program is distributed in the hope that it will be useful,             *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
*    GNU General Public License for more details.                                *
*                                                                                *
*    You should have received a copy of the GNU General Public License           *
*    along with this program; if not, write to the Free Software                 *
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA   *
*                                                                                *
*                                                                                *
*                                                                                *
*    For further information contact me at biocrasher@gmail.com                  *
**********************************************************************************/


#include <recordmydesktop.h>

void XImageToYUV(XImage *imgz,yuv_buffer *yuv){
    unsigned char *dtap=(unsigned char *)imgz->data;
    int i,k,j=0;

    for(k=0;k<(imgz->width*imgz->height);k++){
            yuv->y[k]=Yr[dtap[(k*4)+2]] + Yg[dtap[(k*4)+1]] + Yb[dtap[(k*4)]];
    }
    
    for(i=0;i<(imgz->height);i+=2){
        for(k=0;k<(imgz->width);k+=2){    
            yuv->u[j]=Ur[dtap[i*imgz->bytes_per_line+k*4+2]] + Ug[dtap[i*imgz->bytes_per_line+k*4+1]] + Ub[dtap[i*imgz->bytes_per_line+k*4]];
            yuv->v[j]=Vr[dtap[i*imgz->bytes_per_line+k*4+2]] + Vg[dtap[i*imgz->bytes_per_line+k*4+1]] + Vb[dtap[i*imgz->bytes_per_line+k*4]] ;
            j++;
        }
    }
}


void MakeMatrices(){
    int i;
    for(i=0;i<256;i++)
        Yr[i]=0.299*i;
    for(i=0;i<256;i++)
        Yg[i]=0.587*i;
    for(i=0;i<256;i++)
        Yb[i]=0.114*i;

    for(i=0;i<256;i++)
        Ur[i]=43.1-0.169*i;
    for(i=0;i<256;i++)
        Ug[i]=84.41-0.331*i;
    for(i=0;i<256;i++)
        Ub[i]=0.5*i;

    for(i=0;i<256;i++)
        Vr[i]=0.5*i;
    for(i=0;i<256;i++)
        Vg[i]=107-0.419*i;
    for(i=0;i<256;i++)
        Vb[i]=21-0.081*i;
}

