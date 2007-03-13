/******************************************************************************
*                            recordMyDesktop                                  *
*******************************************************************************
*                                                                             *
*            Copyright (C) 2006,2007 John Varouhakis                          *
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

#include <recordmydesktop.h>

unsigned char *MakeDummyPointer(DisplaySpecs *specs,
                                int size,
                                int color,
                                int type,
                                unsigned char *npxl){
    int i,k,o='.';
    unsigned long   b=(color)?'w':'b',
                    w=(color)?'b':'w';
    char pmask[1][16][16]={{
        {w,w,o,o,o,o,o,o,o,o,o,o,o,o,o,o},
        {w,b,w,o,o,o,o,o,o,o,o,o,o,o,o,o},
        {w,b,b,w,o,o,o,o,o,o,o,o,o,o,o,o},
        {w,b,b,b,w,o,o,o,o,o,o,o,o,o,o,o},
        {w,b,b,b,b,w,o,o,o,o,o,o,o,o,o,o},
        {w,b,b,b,b,b,w,o,o,o,o,o,o,o,o,o},
        {w,b,b,b,b,b,b,w,o,o,o,o,o,o,o,o},
        {w,b,b,b,b,b,b,b,w,o,o,o,o,o,o,o},
        {w,b,b,b,b,b,b,b,b,w,o,o,o,o,o,o},
        {w,b,b,b,b,b,w,w,w,w,o,o,o,o,o,o},
        {w,b,b,b,b,b,w,o,o,o,o,o,o,o,o,o},
        {w,b,b,w,w,b,b,w,o,o,o,o,o,o,o,o},
        {w,b,w,o,w,b,b,w,o,o,o,o,o,o,o,o},
        {w,w,o,o,o,w,b,b,w,o,o,o,o,o,o,o},
        {o,o,o,o,o,w,b,b,w,o,o,o,o,o,o,o},
        {o,o,o,o,o,o,w,w,o,o,o,o,o,o,o,o}}
    };
    unsigned char *ret=malloc(size*sizeof(char[size*4]));
    unsigned char wp[4]={
        ((specs->wpixel^0xff000000)>>24),
        ((specs->wpixel^0x00ff0000)>>16),
        ((specs->wpixel^0x0000ff00)>>8),
        ((specs->wpixel^0x000000ff))
    };
    unsigned char bp[4]={
        ((specs->bpixel^0xff000000)>>24),
        ((specs->bpixel^0x00ff0000)>>16),
        ((specs->bpixel^0x0000ff00)>>8),
        ((specs->bpixel^0x000000ff))
    };
    *npxl=((wp[0]-1)!=bp[0])?wp[0]-100:wp[0]-102;
    for(i=0;i<size;i++){
        for(k=0;k<size;k++){
            ret[(i*size+k)*4+__ABYTE]=(pmask[type][i][k]=='w')?
                                      wp[0]:(pmask[type][i][k]=='b')?
                                      bp[0]:*npxl;
            ret[(i*size+k)*4+__RBYTE]=(pmask[type][i][k]=='w')?
                                       wp[1]:(pmask[type][i][k]=='b')?
                                       bp[1]:*npxl;
            ret[(i*size+k)*4+__GBYTE]=(pmask[type][i][k]=='w')?
                                       wp[2]:(pmask[type][i][k]=='b')?
                                       bp[2]:*npxl;
            ret[(i*size+k)*4+__BBYTE]=(pmask[type][i][k]=='w')?
                                       wp[3]:(pmask[type][i][k]=='b')?
                                       bp[3]:*npxl;
        }
    }

    return ret;
}
