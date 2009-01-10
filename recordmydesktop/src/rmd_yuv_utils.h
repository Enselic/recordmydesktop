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

#ifndef YUV_UTILS_H
#define YUV_UTILS_H 1

#include "rmd_macro.h"
#include "rmd_types.h"


// The macros work directly on this data (for performance reasons I
// suppose) so we keep this global
extern unsigned char Yr[256], Yg[256], Yb[256],
                     Ur[256], Ug[256], UbVr[256],
                     Vg[256], Vb[256];


// We keep these global for now. FIXME: Isolate them.
extern u_int32_t *yblocks,
                 *ublocks,
                 *vblocks;

#define POINT_IN_BLOCK(xv,yv,widthv,blocksize) ((yv/blocksize)*\
                                                (widthv/blocksize)+\
                                                (xv/blocksize))

//when adding the r values, we go beyond
//the (16 bit)range of the t_val variable, but we are performing
//32 bit arithmetics, so there's no problem.
//(This note is useless, I'm just adding because
//the addition of the A components in CALC_TVAL_AVG_32,
//now removed as uneeded, produced an overflow which would have caused
//color distrtion, where it one of the R,G or B components)
#define CALC_TVAL_AVG_16(t_val,datapi,datapi_next){\
    register u_int16_t t1,t2,t3,t4;\
    t1=*datapi;\
    t2=*(datapi+1);\
    t3=*datapi_next;\
    t4=*(datapi_next+1);\
    t_val=((((t1&__R16_MASK) +(t2&__R16_MASK)+\
             (t3&__R16_MASK)+(t4&__R16_MASK))/4)&__R16_MASK)+\
          ((((t1&__G16_MASK) +(t2&__G16_MASK)+\
             (t3&__G16_MASK)+(t4&__G16_MASK))/4)&__G16_MASK)+\
          ((((t1&__B16_MASK) +(t2&__B16_MASK)+\
             (t3&__B16_MASK)+(t4&__B16_MASK))/4)&__B16_MASK);\
}

//the 4 most significant bytes represent the A component which
//does not need to be added on t_val, as it is always unused
#define CALC_TVAL_AVG_32(t_val,datapi,datapi_next){\
    register unsigned int t1,t2,t3,t4;\
    t1=*datapi;\
    t2=*(datapi+1);\
    t3=*datapi_next;\
    t4=*(datapi_next+1);\
    t_val=((((t1&0x00ff0000) +(t2&0x00ff0000)+\
            (t3&0x00ff0000)+(t4&0x00ff0000))/4)&0x00ff0000)+\
          ((((t1&0x0000ff00) +(t2&0x0000ff00)+\
            (t3&0x0000ff00)+(t4&0x0000ff00))/4)&0x0000ff00)+\
          ((((t1&0x000000ff) +(t2&0x000000ff)+\
            (t3&0x000000ff)+(t4&0x000000ff))/4)&0x000000ff);\
}

#define UPDATE_Y_PLANE(data,\
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
    register u_int##__bit_depth__##_t *datapi=(u_int##__bit_depth__##_t *)data;\
    for(k=0;k<height_tm;k++){\
        for(i=0;i<width_tm;i++){\
            t_val=*datapi;\
            *yuv_y=_yr[__RVALUE_##__bit_depth__(t_val)] +\
                   _yg[__GVALUE_##__bit_depth__(t_val)] +\
                   _yb[__BVALUE_##__bit_depth__(t_val)] ;\
            datapi++;\
            yuv_y++;\
        }\
        yuv_y+=yuv->y_width-width_tm;\
    }\
}

#define UPDATE_A_UV_PIXEL(yuv_u,\
                          yuv_v,\
                          t_val,\
                          datapi,\
                          datapi_next,\
                          _ur,_ug,_ubvr,_vg,_vb,\
                          __sampling_type,\
                          __bit_depth__)\
            if(__sampling_type==__PXL_AVERAGE){\
                CALC_TVAL_AVG_##__bit_depth__(t_val,datapi,datapi_next)\
            }\
            else\
                t_val=*datapi;\
            *yuv_u=\
            _ur[__RVALUE_##__bit_depth__(t_val)] +\
            _ug[__GVALUE_##__bit_depth__(t_val)] +\
            _ubvr[__BVALUE_##__bit_depth__(t_val)];\
            *yuv_v=\
            _ubvr[__RVALUE_##__bit_depth__(t_val)] +\
            _vg[__GVALUE_##__bit_depth__(t_val)] +\
            _vb[__BVALUE_##__bit_depth__(t_val)];\


#define UPDATE_UV_PLANES(data,\
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
                                      *datapi_next=NULL;\
    if(__sampling_type==__PXL_AVERAGE){\
        datapi_next=datapi+width_tm;\
    }\
    for(k=0;k<height_tm;k+=2){\
        for(i=0;i<width_tm;i+=2){\
            UPDATE_A_UV_PIXEL(  yuv_u,\
                                yuv_v,\
                                t_val,\
                                datapi,\
                                datapi_next,\
                                _ur,_ug,_ubvr,_vg,_vb,\
                                __sampling_type,\
                                __bit_depth__)\
            datapi+=2;\
            if(__sampling_type==__PXL_AVERAGE)\
                datapi_next+=2;\
            yuv_u++;\
            yuv_v++;\
        }\
        yuv_u+=(yuv->y_width-width_tm)/2;\
        yuv_v+=(yuv->y_width-width_tm)/2;\
        datapi+=width_tm;\
        if(__sampling_type==__PXL_AVERAGE)\
            datapi_next+=width_tm;\
    }\
}

#define UPDATE_YUV_BUFFER(yuv,\
                          data,\
                          data_back,\
                          x_tm,\
                          y_tm,\
                          width_tm,\
                          height_tm,\
                          __sampling_type,\
                          __color_depth){\
    if(data_back==NULL){\
        if((__color_depth==24)||(__color_depth==32)){\
            UPDATE_Y_PLANE(data,x_tm,y_tm,height_tm,width_tm,yuv,32)\
            UPDATE_UV_PLANES(data,x_tm,y_tm,height_tm,width_tm,\
                             yuv,__sampling_type,32)\
        }\
        else if(__color_depth==16){\
            UPDATE_Y_PLANE(data,x_tm,y_tm,height_tm,width_tm,yuv,16)\
            UPDATE_UV_PLANES(data,x_tm,y_tm,height_tm,width_tm,\
                            yuv,__sampling_type,16)\
        }\
    }\
    else{\
        if((__color_depth==24)||(__color_depth==32)){\
            UPDATE_Y_PLANE_DBUF(data,data_back,x_tm,y_tm,\
                                height_tm,width_tm,yuv,32)\
            UPDATE_UV_PLANES_DBUF(data,data_back,x_tm,y_tm,height_tm,width_tm,\
                                  yuv,__sampling_type,32)\
        }\
        else if(__color_depth==16){\
            UPDATE_Y_PLANE_DBUF(data,data_back,x_tm,y_tm,\
                                height_tm,width_tm,yuv,16)\
            UPDATE_UV_PLANES_DBUF(data,data_back,x_tm,y_tm,height_tm,width_tm,\
                                  yuv,__sampling_type,16)\
        }\
    }\
}

#define DUMMY_POINTER_TO_YUV(yuv,\
                             data_tm,\
                             x_tm,\
                             y_tm,\
                             width_tm,\
                             height_tm,\
                             x_offset,\
                             y_offset,\
                             no_pixel){\
    int i,k,j=0;\
    int x_2=x_tm/2,y_2=y_tm/2,y_width_2=(yuv)->y_width/2;\
    for(k=y_offset;k<y_offset+height_tm;k++){\
        for(i=x_offset;i<x_offset+width_tm;i++){\
            j=k*16+i;\
            if(data_tm[(j*4)]!=(no_pixel)){\
                (yuv)->y[x_tm+(i-x_offset)+((k-y_offset)+y_tm)*(yuv)->y_width]=\
                    Yr[data_tm[(j*4)+__RBYTE]] +\
                    Yg[data_tm[(j*4)+__GBYTE]] +\
                    Yb[data_tm[(j*4)+__BBYTE]];\
                if((k%2)&&(i%2)){\
                    yuv->u[x_2+(i-x_offset)/2+((k-y_offset)/2+y_2)*y_width_2]=\
                        Ur[data_tm[(k*width_tm+i)*4+__RBYTE]] +\
                        Ug[data_tm[(k*width_tm+i)*4+__GBYTE]] +\
                        UbVr[data_tm[(k*width_tm+i)*4+__BBYTE]];\
                    yuv->v[x_2+(i-x_offset)/2+((k-y_offset)/2+y_2)*y_width_2]=\
                        UbVr[data_tm[(k*width_tm+i)*4+__RBYTE]] +\
                        Vg[data_tm[(k*width_tm+i)*4+__GBYTE]] +\
                        Vb[data_tm[(k*width_tm+i)*4+__BBYTE]] ;\
                }\
            }\
        }\
    }\
}

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

/**
* Fill Yr,Yg,Yb,Ur,Ug.Ub,Vr,Vg,Vb arrays(globals) with values.
*/
void rmdMakeMatrices(void);


#endif
