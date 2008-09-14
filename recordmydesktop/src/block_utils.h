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

#ifndef BLOCK_UTILS_H
#define BLOCK_UTILS_H 1

#include "rmdtypes.h"
#include "rmdmacro.h"
#include "yuv_utils.h"


// We keep these global for now. FIXME: Isolate them.
extern u_int32_t *yblocks,
                 *ublocks,
                 *vblocks;

#define POINT_IN_BLOCK(xv,yv,widthv,blocksize) ((yv/blocksize)*\
                                                (widthv/blocksize)+\
                                                (xv/blocksize))

#define UPDATE_Y_PLANE_DBUF(data,\
                            data_back,\
                            x_tm,\
                            y_tm,\
                            height_tm,\
                            width_tm,\
                            yuv,\
                            __bit_depth__){ \
    int k,i;\
    register u_int##__bit_depth__##_t t_val;\
    register unsigned char  *yuv_y=yuv->y+x_tm+y_tm*yuv->y_width,\
                            *_yr=Yr,*_yg=Yg,*_yb=Yb;\
    register u_int##__bit_depth__##_t *datapi=(u_int##__bit_depth__##_t *)data,\
                            *datapi_back=(u_int##__bit_depth__##_t *)data_back;\
    for(k=0;k<height_tm;k++){\
        for(i=0;i<width_tm;i++){\
            if(*datapi!=*datapi_back){\
                t_val=*datapi;\
                *yuv_y=_yr[__RVALUE_##__bit_depth__(t_val)] +\
                    _yg[__GVALUE_##__bit_depth__(t_val)] +\
                    _yb[__BVALUE_##__bit_depth__(t_val)] ;\
                yblocks[POINT_IN_BLOCK(i,k,width_tm,Y_UNIT_WIDTH)]=1;\
            }\
            datapi++;\
            datapi_back++;\
            yuv_y++;\
        }\
        yuv_y+=yuv->y_width-width_tm;\
    }\
}

#define UPDATE_UV_PLANES_DBUF(  data,\
                                data_back,\
                                x_tm,\
                                y_tm,\
                                height_tm,\
                                width_tm,\
                                yuv,\
                                __sampling_type,\
                                __bit_depth__){  \
    int k,i;\
    register u_int##__bit_depth__##_t t_val;\
    register unsigned char  *yuv_u=yuv->u+x_tm/2+(y_tm*yuv->uv_width)/2,\
                            *yuv_v=yuv->v+x_tm/2+(y_tm*yuv->uv_width)/2,\
                            *_ur=Ur,*_ug=Ug,*_ubvr=UbVr,\
                            *_vg=Vg,*_vb=Vb;\
    register u_int##__bit_depth__##_t *datapi=(u_int##__bit_depth__##_t *)data,\
                                      *datapi_next=NULL,\
                            *datapi_back=(u_int##__bit_depth__##_t *)data_back,\
                            *datapi_back_next=NULL;\
    if(__sampling_type==__PXL_AVERAGE){\
        datapi_next=datapi+width_tm;\
        datapi_back_next=datapi_back+width_tm;\
        for(k=0;k<height_tm;k+=2){\
            for(i=0;i<width_tm;i+=2){\
                if(( (*datapi!=*datapi_back) ||\
                    (*(datapi+1)!=*(datapi_back+1)) ||\
                    (*datapi_next!=*datapi_back_next) ||\
                    (*(datapi_next+1)!=*(datapi_back_next+1)))){\
                    UPDATE_A_UV_PIXEL(  yuv_u,\
                                        yuv_v,\
                                        t_val,\
                                        datapi,\
                                        datapi_next,\
                                        _ur,_ug,_ubvr,_vg,_vb,\
                                        __sampling_type,\
                                        __bit_depth__)\
                    ublocks[POINT_IN_BLOCK(i,k,width_tm,Y_UNIT_WIDTH)]=1;\
                    vblocks[POINT_IN_BLOCK(i,k,width_tm,Y_UNIT_WIDTH)]=1;\
                }\
                datapi+=2;\
                datapi_back+=2;\
                if(__sampling_type==__PXL_AVERAGE){\
                    datapi_next+=2;\
                    datapi_back_next+=2;\
                }\
                yuv_u++;\
                yuv_v++;\
            }\
            yuv_u+=(yuv->y_width-width_tm)/2;\
            yuv_v+=(yuv->y_width-width_tm)/2;\
            datapi+=width_tm;\
            datapi_back+=width_tm;\
            if(__sampling_type==__PXL_AVERAGE){\
                datapi_next+=width_tm;\
                datapi_back_next+=width_tm;\
            }\
        }\
    }\
    else{\
        for(k=0;k<height_tm;k+=2){\
            for(i=0;i<width_tm;i+=2){\
                if ((*datapi!=*datapi_back)){\
                    UPDATE_A_UV_PIXEL(  yuv_u,\
                                        yuv_v,\
                                        t_val,\
                                        datapi,\
                                        datapi_next,\
                                        _ur,_ug,_ubvr,_vg,_vb,\
                                        __sampling_type,\
                                        __bit_depth__)\
                    ublocks[POINT_IN_BLOCK(i,k,width_tm,Y_UNIT_WIDTH)]=1;\
                    vblocks[POINT_IN_BLOCK(i,k,width_tm,Y_UNIT_WIDTH)]=1;\
                }\
                datapi+=2;\
                datapi_back+=2;\
                if(__sampling_type==__PXL_AVERAGE){\
                    datapi_next+=2;\
                    datapi_back_next+=2;\
                }\
                yuv_u++;\
                yuv_v++;\
            }\
            yuv_u+=(yuv->y_width-width_tm)/2;\
            yuv_v+=(yuv->y_width-width_tm)/2;\
            datapi+=width_tm;\
            datapi_back+=width_tm;\
            if(__sampling_type==__PXL_AVERAGE){\
                datapi_next+=width_tm;\
                datapi_back_next+=width_tm;\
            }\
        }\
    }\
}


#endif
