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

#ifndef RMDMACRO_H
#define RMDMACRO_H 1

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include "rmdtypes.h"

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

//size of stride when comparing planes(depending on type)
//this is just to avoid thousands of sizeof's
#ifdef HAVE_U_INT64_T
    #define COMPARE_STRIDE  8
#else
    #define COMPARE_STRIDE  4
#endif

//500 mb file size
#define CACHE_FILE_SIZE_LIMIT (500*1<<20)
//minimize hard disk access
#define CACHE_OUT_BUFFER_SIZE 4096


//The width, in bytes, of the blocks
//on which the y,u and v planes are broken.
//These blocks are square.
#define Y_UNIT_WIDTH    0x0010
#define UV_UNIT_WIDTH   0x0008

//The number of bytes for every
//sub-block of the y,u and v planes.
//Since the blocks are square
//these are obviously the squares
//of the widths(specified above),
//but the definitions bellow are only
//for convenience anyway.
#define Y_UNIT_BYTES    0x0100
#define UV_UNIT_BYTES   0x0040

#ifdef HAVE_LIBASOUND
    #define DEFAULT_AUDIO_DEVICE "hw:0,0"
#else
    #define DEFAULT_AUDIO_DEVICE "/dev/dsp"
#endif



#define CLIP_EVENT_AREA(e,brwin,wgeom){\
    if(((e)->area.x<=(brwin)->rgeom.x)&&((e)->area.y<=(brwin)->rgeom.y)&&\
        ((e)->area.width>=(brwin)->rgeom.width)&&\
        ((e)->area.height<(brwin)->rgeom.height)){\
        (wgeom)->x=(brwin)->rgeom.x;\
        (wgeom)->y=(brwin)->rgeom.y;\
        (wgeom)->width=(brwin)->rgeom.width;\
        (wgeom)->height=(brwin)->rgeom.height;\
    }\
    else{\
        (wgeom)->x=((((e)->area.x+(e)->area.width>=(brwin)->rgeom.x)&&\
        ((e)->area.x<=(brwin)->rgeom.x+(brwin)->rgeom.width))?\
        (((e)->area.x<=(brwin)->rgeom.x)?(brwin)->rgeom.x:(e)->area.x):-1);\
    \
        (wgeom)->y=((((e)->area.y+(e)->area.height>=(brwin)->rgeom.y)&&\
        ((e)->area.y<=(brwin)->rgeom.y+(brwin)->rgeom.height))?\
        (((e)->area.y<=(brwin)->rgeom.y)?(brwin)->rgeom.y:(e)->area.y):-1);\
    \
        (wgeom)->width=((e)->area.x<=(brwin)->rgeom.x)?\
        (e)->area.width-((brwin)->rgeom.x-(e)->area.x):\
        ((e)->area.x<=(brwin)->rgeom.x+(brwin)->rgeom.width)?\
        (((brwin)->rgeom.width-(e)->area.x+(brwin)->rgeom.x<(e)->area.width)?\
        (brwin)->rgeom.width-(e)->area.x+(brwin)->rgeom.x:e->area.width):-1;\
    \
        (wgeom)->height=((e)->area.y<=(brwin)->rgeom.y)?\
        (e)->area.height-((brwin)->rgeom.y-(e)->area.y):\
        ((e)->area.y<=(brwin)->rgeom.y+(brwin)->rgeom.height)?\
        (((brwin)->rgeom.height-(e)->area.y+\
         (brwin)->rgeom.y<(e)->area.height)?\
         (brwin)->rgeom.height-(e)->area.y+\
         (brwin)->rgeom.y:(e)->area.height):-1;\
    \
        if((wgeom)->width>(brwin)->rgeom.width)\
            (wgeom)->width=(brwin)->rgeom.width;\
        if((wgeom)->height>(brwin)->rgeom.height)\
            (wgeom)->height=(brwin)->rgeom.height;\
    }\
}

#define CLIP_DUMMY_POINTER_AREA(dummy_p_area,brwin,wgeom){\
    (wgeom)->x=((((dummy_p_area).x+\
                (dummy_p_area).width>=(brwin)->rgeom.x)&&\
                ((dummy_p_area).x<=(brwin)->rgeom.x+\
                (brwin)->rgeom.width))?\
                (((dummy_p_area).x<=(brwin)->rgeom.x)?\
                (brwin)->rgeom.x:(dummy_p_area).x):-1);\
    (wgeom)->y=((((dummy_p_area).y+\
                (dummy_p_area).height>=(brwin)->rgeom.y)&&\
                ((dummy_p_area).y<=(brwin)->rgeom.y+\
                (brwin)->rgeom.height))?\
                (((dummy_p_area).y<=(brwin)->rgeom.y)?\
                (brwin)->rgeom.y:(dummy_p_area).y):-1);\
    (wgeom)->width=((dummy_p_area).x<=(brwin)->rgeom.x)?\
                (dummy_p_area).width-\
                ((brwin)->rgeom.x-(dummy_p_area).x):\
                ((dummy_p_area).x<=(brwin)->rgeom.x+\
                (brwin)->rgeom.width)?\
                ((brwin)->rgeom.width-(dummy_p_area).x+\
                (brwin)->rgeom.x<(dummy_p_area).width)?\
                (brwin)->rgeom.width-(dummy_p_area).x+\
                (brwin)->rgeom.x:(dummy_p_area).width:-1;\
    (wgeom)->height=((dummy_p_area).y<=(brwin)->rgeom.y)?\
                (dummy_p_area).height-\
                ((brwin)->rgeom.y-(dummy_p_area).y):\
                ((dummy_p_area).y<=(brwin)->rgeom.y+\
                (brwin)->rgeom.height)?\
                ((brwin)->rgeom.height-(dummy_p_area).y+\
                (brwin)->rgeom.y<(dummy_p_area).height)?\
                (brwin)->rgeom.height-(dummy_p_area).y+\
                (brwin)->rgeom.y:(dummy_p_area).height:-1;\
    if((wgeom)->width>(brwin)->rgeom.width)\
        (wgeom)->width=(brwin)->rgeom.width;\
    if((wgeom)->height>(brwin)->rgeom.height)\
        (wgeom)->height=(brwin)->rgeom.height;\
}



#define DEFAULT_ARGS(args){\
    (args)->delay=0;\
    if(getenv("DISPLAY")!=NULL){\
        (args)->display=(char *)malloc(strlen(getenv("DISPLAY"))+1);\
        strcpy((args)->display,getenv("DISPLAY"));\
    }\
    else\
        (args)->display=NULL;\
    (args)->windowid=\
    (args)->x=\
    (args)->y=\
    (args)->width=\
    (args)->height=\
    (args)->quietmode=\
    (args)->nosound=\
    (args)->full_shots=\
    (args)->encOnTheFly=\
    (args)->zerocompression=\
    (args)->nowmcheck=\
    (args)->dropframes=\
    (args)->overwrite=\
    (args)->use_jack=\
    (args)->jack_nports=\
    (args)->nocondshared=0;\
    (args)->jack_ringbuffer_secs=3.0;\
    (args)->jack_port_names=NULL;\
    (args)->no_quick_subsample=\
    (args)->noshared=1;\
    (args)->filename=(char *)malloc(8);\
    strcpy((args)->filename,"out.ogg");\
    (args)->cursor_color=1;\
    (args)->shared_thres=75;\
    (args)->have_dummy_cursor=0;\
    (args)->xfixes_cursor=1;\
    (args)->device=(char *)malloc(strlen(DEFAULT_AUDIO_DEVICE)+1);\
    strcpy((args)->device,DEFAULT_AUDIO_DEVICE);\
    (args)->fps=15;\
    (args)->channels=1;\
    (args)->frequency=22050;\
    (args)->buffsize=4096;\
    (args)->v_bitrate=45000;\
    (args)->v_quality=63;\
    (args)->s_quality=10;\
    (args)->workdir=(char *)malloc(5);\
    strcpy((args)->workdir,"/tmp");\
}

#define QUERY_DISPLAY_SPECS(display,specstruct){\
    (specstruct)->screen=DefaultScreen(display);\
    (specstruct)->width=DisplayWidth(display,(specstruct)->screen);\
    (specstruct)->height=DisplayHeight(display,(specstruct)->screen);\
    (specstruct)->root=RootWindow(display,(specstruct)->screen);\
    (specstruct)->visual=DefaultVisual(display,(specstruct)->screen);\
    (specstruct)->gc=DefaultGC(display,(specstruct)->screen);\
    (specstruct)->depth=DefaultDepth(display,(specstruct)->screen);\
    (specstruct)->bpixel=XBlackPixel(display,(specstruct)->screen);\
    (specstruct)->wpixel=XWhitePixel(display,(specstruct)->screen);\
}

#define AVG_4_PIXELS(data_array,width_img,k_tm,i_tm,offset)\
    ((data_array[(k_tm*width_img+i_tm)*RMD_ULONG_SIZE_T+offset]+\
    data_array[((k_tm-1)*width_img+i_tm)*RMD_ULONG_SIZE_T+offset]+\
    data_array[(k_tm*width_img+i_tm-1)*RMD_ULONG_SIZE_T+offset]+\
    data_array[((k_tm-1)*width_img+i_tm-1)*RMD_ULONG_SIZE_T+offset])/4)

#define CALC_TVAL_AVG_32(t_val,datapi,datapi_next){\
    register unsigned int t1,t2,t3,t4;\
    t1=*datapi;\
    t2=*(datapi+1);\
    t3=*datapi_next;\
    t4=*(datapi_next+1);\
    t_val=((((t1&0xff000000) +(t2&0xff000000)+\
            (t3&0xff000000)+(t4&0xff000000))/4)&0xff000000)+\
          ((((t1&0x00ff0000) +(t2&0x00ff0000)+\
            (t3&0x00ff0000)+(t4&0x00ff0000))/4)&0x00ff0000)+\
          ((((t1&0x0000ff00) +(t2&0x0000ff00)+\
            (t3&0x0000ff00)+(t4&0x0000ff00))/4)&0x0000ff00)+\
          ((((t1&0x000000ff) +(t2&0x000000ff)+\
            (t3&0x000000ff)+(t4&0x000000ff))/4)&0x000000ff);\
}

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

#define UPDATE_Y_PLANE(data,\
                       x_tm,\
                       y_tm,\
                       height_tm,\
                       width_tm,\
                       yuv,\
                       __copy_type,\
                       __bit_depth__){ \
    int k,i;\
    register u_int##__bit_depth__##_t t_val;\
    register unsigned char  *yuv_y=yuv->y+x_tm+y_tm*yuv->y_width,\
                            *_yr=Yr,*_yg=Yg,*_yb=Yb;\
    register u_int##__bit_depth__##_t *datapi=(u_int##__bit_depth__##_t *)data\
                                              +((__copy_type==__X_SHARED)?\
                                              (x_tm+y_tm*yuv->y_width):0);\
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
        if(__copy_type==__X_SHARED)\
            datapi+=yuv->y_width-width_tm;\
    }\
}

#define UPDATE_UV_PLANES(data,\
                         x_tm,\
                         y_tm,\
                         height_tm,\
                         width_tm,\
                         yuv,\
                         __copy_type,\
                         __sampling_type,\
                         __bit_depth__){  \
    int k,i;\
    register u_int##__bit_depth__##_t t_val;\
    register unsigned char  *yuv_u=yuv->u+x_tm/2+(y_tm*yuv->uv_width)/2,\
                            *yuv_v=yuv->v+x_tm/2+(y_tm*yuv->uv_width)/2,\
                            *_ur=Ur,*_ug=Ug,*_ub=Ub,\
                            *_vr=Vr,*_vg=Vg,*_vb=Vb;\
    register u_int##__bit_depth__##_t *datapi=(u_int##__bit_depth__##_t *)data\
                                               +((__copy_type==__X_SHARED)?\
                                                (x_tm+y_tm*yuv->y_width):0),\
                                      *datapi_next=NULL;\
    if(__sampling_type==__PXL_AVERAGE){\
        datapi_next=datapi+\
        ((__copy_type==__X_SHARED)?(yuv->y_width):(width_tm));\
    }\
    for(k=0;k<height_tm;k+=2){\
        for(i=0;i<width_tm;i+=2){\
            if(__sampling_type==__PXL_AVERAGE){\
                CALC_TVAL_AVG_##__bit_depth__(t_val,datapi,datapi_next)\
            }\
            else\
                t_val=*datapi;\
            *yuv_u=\
            _ur[__RVALUE_##__bit_depth__(t_val)] +\
            _ug[__GVALUE_##__bit_depth__(t_val)] +\
            _ub[__BVALUE_##__bit_depth__(t_val)];\
            *yuv_v=\
            _vr[__RVALUE_##__bit_depth__(t_val)] +\
            _vg[__GVALUE_##__bit_depth__(t_val)] +\
            _vb[__BVALUE_##__bit_depth__(t_val)];\
            datapi+=2;\
            if(__sampling_type==__PXL_AVERAGE)\
                datapi_next+=2;\
            yuv_u++;\
            yuv_v++;\
        }\
        yuv_u+=(yuv->y_width-width_tm)/2;\
        yuv_v+=(yuv->y_width-width_tm)/2;\
        datapi+=((__copy_type==__X_SHARED)?\
                 (2*yuv->y_width-width_tm):width_tm);\
        if(__sampling_type==__PXL_AVERAGE)\
            datapi_next+=((__copy_type==__X_SHARED)?\
                          (2*yuv->y_width-width_tm):width_tm);\
    }\
}

#define UPDATE_YUV_BUFFER(yuv,\
                          data,\
                          x_tm,\
                          y_tm,\
                          width_tm,\
                          height_tm,\
                          __copy_type,\
                          __sampling_type,\
                          __color_depth){\
    if((__color_depth==24)||(__color_depth==32)){\
        UPDATE_Y_PLANE(data,x_tm,y_tm,height_tm,width_tm,yuv,__copy_type,32)\
        UPDATE_UV_PLANES(data,x_tm,y_tm,height_tm,width_tm,\
                         yuv,__copy_type,__sampling_type,32)\
    }\
    else if(__color_depth==16){\
        UPDATE_Y_PLANE(data,x_tm,y_tm,height_tm,width_tm,yuv,__copy_type,16)\
        UPDATE_UV_PLANES(data,x_tm,y_tm,height_tm,width_tm,\
                         yuv,__copy_type,__sampling_type,16)\
    }\
}



#define XFIXES_POINTER_TO_YUV(yuv,\
                              data,\
                              x_tm,\
                              y_tm,\
                              width_tm,\
                              height_tm,\
                              column_discard_stride){\
    int i,k,j=0;\
    unsigned char avg0,avg1,avg2,avg3;\
    int x_2=x_tm/2,y_2=y_tm/2;\
    for(k=0;k<height_tm;k++){\
        for(i=0;i<width_tm;i++){\
                yuv->y[x_tm+i+(k+y_tm)*yuv->y_width]=\
                    (yuv->y[x_tm+i+(k+y_tm)*yuv->y_width]*\
                    (UCHAR_MAX-data[(j*RMD_ULONG_SIZE_T)+__ABYTE])+\
                    (Yr[data[(j*RMD_ULONG_SIZE_T)+__RBYTE]]+\
                    Yg[data[(j*RMD_ULONG_SIZE_T)+__GBYTE]] +\
                    Yb[data[(j*RMD_ULONG_SIZE_T)+__BBYTE]])*\
                data[(j*RMD_ULONG_SIZE_T)+__ABYTE])/UCHAR_MAX ;\
                if((k%2)&&(i%2)){\
                    avg3=AVG_4_PIXELS(data,\
                                      (width_tm+column_discard_stride),\
                                      k,i,__ABYTE);\
                    avg2=AVG_4_PIXELS(data,\
                                      (width_tm+column_discard_stride),\
                                      k,i,__RBYTE);\
                    avg1=AVG_4_PIXELS(data,\
                                      (width_tm+column_discard_stride),\
                                      k,i,__GBYTE);\
                    avg0=AVG_4_PIXELS(data,\
                                      (width_tm+column_discard_stride),\
                                      k,i,__BBYTE);\
                    yuv->u[x_2+i/2+(k/2+y_2)*yuv->uv_width]=\
                    (yuv->u[x_2+i/2+(k/2+y_2)*yuv->uv_width]*(UCHAR_MAX-avg3)+\
                    (Ur[avg2] + Ug[avg1] +Ub[avg0])*avg3)/UCHAR_MAX;\
                    yuv->v[x_2+i/2+(k/2+y_2)*yuv->uv_width]=\
                    (yuv->v[x_2+i/2+(k/2+y_2)*yuv->uv_width]*(UCHAR_MAX-avg3)+\
                    (Vr[avg2] + Vg[avg1] +Vb[avg0])*avg3)/UCHAR_MAX;\
                }\
            j++;\
        }\
        j+=column_discard_stride;\
    }\
}

#define DUMMY_POINTER_TO_YUV(yuv,\
                             data_tm,\
                             x_tm,\
                             y_tm,\
                             width_tm,\
                             height_tm,\
                             no_pixel){\
    int i,k,j=0;\
    int x_2=x_tm/2,y_2=y_tm/2,y_width_2=(yuv)->y_width/2;\
    for(k=0;k<height_tm;k++){\
        for(i=0;i<width_tm;i++){\
            if(data_tm[(j*4)]!=(no_pixel)){\
                (yuv)->y[x_tm+i+(k+y_tm)*(yuv)->y_width]=\
                    Yr[data_tm[(j*4)+__RBYTE]] +\
                    Yg[data_tm[(j*4)+__GBYTE]] +\
                    Yb[data_tm[(j*4)+__BBYTE]];\
                if((k%2)&&(i%2)){\
                    yuv->u[x_2+i/2+(k/2+y_2)*y_width_2]=\
                        Ur[data_tm[(k*width_tm+i)*4+__RBYTE]] +\
                        Ug[data_tm[(k*width_tm+i)*4+__GBYTE]] +\
                        Ub[data_tm[(k*width_tm+i)*4+__BBYTE]];\
                    yuv->v[x_2+i/2+(k/2+y_2)*y_width_2]=\
                        Vr[data_tm[(k*width_tm+i)*4+__RBYTE]] +\
                        Vg[data_tm[(k*width_tm+i)*4+__GBYTE]] +\
                        Vb[data_tm[(k*width_tm+i)*4+__BBYTE]] ;\
                }\
            }\
            j++;\
        }\
        j+=16-width_tm;\
    }\
}


#define I16TOA(number,buffer){\
    int t_num=(number),__k=0,__i=0;\
    char *t_buf=malloc(8);\
    t_num=t_num&((2<<15)-1);\
    while(t_num>0){\
        int digit=t_num%10;\
        t_buf[__k]=digit+48;\
        t_num-=digit;\
        t_num/=10;\
        __k++;\
    }\
    while(__k>0)\
        (buffer)[__i++]=t_buf[--__k];\
    (buffer)[__i]='\0';\
    free(t_buf);\
};\

#define INIT_FRAME(frame_t,fheader_t,yuv_t,\
                   YBlocks_t,UBlocks_t,VBlocks_t){\
    (frame_t)->header=(fheader_t);\
    (frame_t)->YBlocks=YBlocks_t;\
    (frame_t)->UBlocks=UBlocks_t;\
    (frame_t)->VBlocks=VBlocks_t;\
    (frame_t)->YData=malloc((yuv_t)->y_width*(yuv_t)->y_height);\
    (frame_t)->UData=malloc((yuv_t)->uv_width*(yuv_t)->uv_height);\
    (frame_t)->VData=malloc((yuv_t)->uv_width*(yuv_t)->uv_height);\
};

#define CLEAR_FRAME(frame_t){\
    free((frame_t)->YData);\
    free((frame_t)->UData);\
    free((frame_t)->VData);\
};

#ifdef HAVE_JACK_H

#define CHECK_DLERRORS_FATAL(__error_p)\
    if((__error_p=dlerror())!=NULL){\
        fprintf(stderr,"%s\n",__error_p);\
        return 1;\
    }

#define DLSYM_AND_CHECK(lib_handle,__call_name__,__error_p)\
    __call_name__##_p=dlsym(lib_handle,#__call_name__);\
    CHECK_DLERRORS_FATAL(__error_p)

#endif

#endif

