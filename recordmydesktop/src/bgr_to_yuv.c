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
            yuv->y[k]=min(abs(dtap[(k*4)+2] * 2104 + dtap[(k*4)+1] * 4130 + dtap[(k*4)] * 802 + 4096 + 131072) >> 13, 235);
    }
    
    for(i=0;i<(imgz->height);i+=2){
        for(k=0;k<(imgz->width);k+=2){    
            yuv->u[j]=min(abs(dtap[i*imgz->bytes_per_line+k*4+2] * -1214 + dtap[i*imgz->bytes_per_line+k*4+1] * -2384 + dtap[i*imgz->bytes_per_line+k*4] * 3598 + 4096 + 1048576) >> 13, 240);
            yuv->v[j]=min(abs(dtap[i*imgz->bytes_per_line+k*4+2] * 3598 + dtap[i*imgz->bytes_per_line+k*4+1] * -3013 + dtap[i*imgz->bytes_per_line+k*4] * -585 + 4096 + 1048576) >> 13, 240);
            j++;
        }
    }
}

void UpdateYUVBufferSh(yuv_buffer *yuv,unsigned char *data,int x,int y,int width,int height){
    int i,k;
    for(k=y;k<y+height;k++){
        for(i=x;i<x+width;i++){
            yuv->y[i+k*yuv->y_width]=min(abs(data[(i+k*yuv->y_width)*4+2] * 2104 + data[(i+k*yuv->y_width)*4+1] * 4130 + data[(i+k*yuv->y_width)*4] * 802 + 4096 + 131072) >> 13, 235);
            if((k%2)&&(i%2)){
                yuv->u[i/2+k/2*yuv->uv_width]=min(abs(data[(i+k*yuv->y_width)*4+2] * -1214 + data[(i+k*yuv->y_width)*4+1] * -2384 + data[(i+k*yuv->y_width)*4] * 3598 + 4096 + 1048576) >> 13, 240);
                yuv->v[i/2+k/2*yuv->uv_width]=min(abs(data[(i+k*yuv->y_width)*4+2] * 3598 + data[(i+k*yuv->y_width)*4+1] * -3013 + data[(i+k*yuv->y_width)*4] * -585 + 4096 + 1048576) >> 13, 240);
            }
        }
    }
}

void UpdateYUVBufferIm(yuv_buffer *yuv,unsigned char *data,int x,int y,int width,int height){
    int i,k,j=0;
    int x_2=x/2,y_2=y/2,y_width_2=yuv->y_width/2;
    for(k=0;k<height;k++){
        for(i=0;i<width;i++){
            yuv->y[x+i+(k+y)*yuv->y_width]=min(abs(data[(j*4)+2] * 2104 + data[(j*4)+1] * 4130 + data[(j*4)] * 802 + 4096 + 131072) >> 13, 235);
            if((k%2)&&(i%2)){
                yuv->u[x_2+i/2+(k/2+y_2)*y_width_2]=min(abs(data[(k*width+i)*4+2] * -1214 + data[(k*width+i)*4+1] * -2384 + data[(k*width+i)*4] * 3598 + 4096 + 1048576) >> 13, 240);
                yuv->v[x_2+i/2+(k/2+y_2)*y_width_2]=min(abs(data[(k*width+i)*4+2] * 3598 + data[(k*width+i)*4+1] * -3013 + data[(k*width+i)*4] * -585 + 4096 + 1048576) >> 13, 240);
            }
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

