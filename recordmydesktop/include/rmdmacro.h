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

#ifdef HAVE_CONFIG_H
    #include "config.h"
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



//The width, in bytes, of the blocks
//on which the y,u and v planes are broken.
//These blocks are square.
#define Y_UNIT_WIDTH    0x0010
#define UV_UNIT_WIDTH   0x0008

#ifdef HAVE_LIBASOUND
    #define DEFAULT_AUDIO_DEVICE "hw:0,0"
#else
    #define DEFAULT_AUDIO_DEVICE "/dev/dsp"
#endif


#define AVG_4_PIXELS(data_array,width_img,k_tm,i_tm,offset)\
    ((data_array[(k_tm*width_img+i_tm)*RMD_ULONG_SIZE_T+offset]+\
    data_array[((k_tm-1)*width_img+i_tm)*RMD_ULONG_SIZE_T+offset]+\
    data_array[(k_tm*width_img+i_tm-1)*RMD_ULONG_SIZE_T+offset]+\
    data_array[((k_tm-1)*width_img+i_tm-1)*RMD_ULONG_SIZE_T+offset])/4)


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

#define CLEAR_FRAME(frame_t){\
    free((frame_t)->YData);\
    free((frame_t)->UData);\
    free((frame_t)->VData);\
};

#endif

