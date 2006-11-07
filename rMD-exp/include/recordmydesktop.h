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
*    For further information contact me at johnvarouhakis@gmail.com              *
**********************************************************************************/


#ifndef RECORDMYDESKTOP_H
#define RECORDMYDESKTOP_H 1

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <endian.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <zlib.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/XShm.h>
#include <theora/theora.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <ogg/ogg.h>
#include <alsa/asoundlib.h>


//define whcih way we are reading a pixmap
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define __ABYTE 3
#define __RBYTE 2
#define __GBYTE 1
#define __BBYTE 0

#define __RVALUE(tmp_val) (((tmp_val)&0x00ff0000)>>16)
#define __GVALUE(tmp_val) (((tmp_val)&0x0000ff00)>>8)
#define __BVALUE(tmp_val) (((tmp_val)&0x000000ff))

#elif __BYTE_ORDER == __BIG_ENDIAN

#define __ABYTE 0
#define __RBYTE 1
#define __GBYTE 2
#define __BBYTE 3

#define __RVALUE(tmp_val) (((tmp_val)&0x0000ff00)>>8)
#define __GVALUE(tmp_val) (((tmp_val)&0x00ff0000)>>16)
#define __BVALUE(tmp_val) (((tmp_val)&0xff000000)>>24)

#else
#error Only little-endian and big-endian systems are supported
#endif


//do not be confused
//this is useless and obsolete.
//There are no plans for other fotmats
enum {UNSPECIFIED,OGG_THEORA_VORBIS};


/**Structs*/

typedef struct _DisplaySpecs{   //this struct holds some basic information
    int screen;                 //about the display,needed mostly for
    uint width;                 //validity checks at startup
    uint height;
    Window root;
    Visual *visual;
    GC gc;
    int depth;
    unsigned long bpixel;
    unsigned long wpixel;
}DisplaySpecs;

typedef struct _WGeometry{  //basic geometry of a window or area
    int x;
    int y;
    int width;
    int height;
}WGeometry;

typedef struct _RectArea{   //an area that has been damaged gets stored
    WGeometry geom;         //in a list comprised of structs of this type
    struct _RectArea *prev,*next;
}RectArea;

typedef struct _BRWindow{   //'basic recorded window' specs
    WGeometry geom;         //window attributes
    WGeometry rgeom;        //part of window that is recorded
    int nbytes;             //size of zpixmap when screenshoting
    Window windowid;           //id
}BRWindow;

//defaults in the following comment lines may be out of sync with reality
//check DEFAULT_ARGS macro further bellow
typedef struct _ProgArgs{
    int delay;          //start up delay
    Window windowid;    //window to record(default root)
    char *display;      //display to connect(default :0)
    int x,y;            //x,y offset(default 0,0)
    int width,height;   //defaults to window width and height
    int quietmode;      //no messages to stderr,stdout
    char *filename;     //output file(default out.[ogg|*])
    int encoding;       //encoding(default OGG_THEORA_VORBIS)
    int cursor_color;   //black or white=>1 or 0
    int have_dummy_cursor;//disable/enable drawing of the dummy cursor
    int xfixes_cursor;   //disable/enable drawing of a cursor obtained
                        //through the xfixes extension
    float fps;            //desired framerate(default 15)
    unsigned int frequency;      //desired frequency (default 22050)
    unsigned int channels;       //no of channels(default 2)
    char *device;       //default sound device(default according to alsa or oss)
    int nosound;        //do not record sound(default 0)
    int noshared;       //do not use shared memory extension(default 1)
    int nocondshared;   //do not use shared memory on large image aquititions
    int nowmcheck;      //do not check if there's a 3d comp window manager
                        //(which changes full-shots and with-shared to 1)
    int shared_thres;   //threshold to use shared memory
    int full_shots;     //do not poll damage, take full screenshots
    int no_quick_subsample;//average pixels in chroma planes
    int v_bitrate,v_quality,s_quality;//video bitrate,video-sound quality
    int dropframes;     //option for theora encoder
    int encOnTheFly;    //encode while recording, no caching(default 0)
    char *workdir;      //directory to be used for cache files(default $HOME)
    int zerocompression;//image data are always flushed uncompressed
}ProgArgs;


//this struct holds anything related to encoding AND
//writting out to file.
typedef struct _EncData{
    ogg_stream_state m_ogg_ts;//theora
    ogg_stream_state m_ogg_vs;//vorbis
    ogg_page         m_ogg_pg;//this could be avoided since
                              // it is used only while initializing
    ogg_packet       m_ogg_pckt1;//theora stream
    ogg_packet       m_ogg_pckt2;//vorbis stream
//theora data
    theora_state     m_th_st;
    theora_info      m_th_inf;
    theora_comment   m_th_cmmnt;
    yuv_buffer       yuv;
//vorbis data
    vorbis_info      m_vo_inf;
    vorbis_comment   m_vo_cmmnt;
    vorbis_dsp_state m_vo_dsp;
    vorbis_block     m_vo_block;
//these should be 0, since area is quantized
//before input
    int             x_offset,
                    y_offset;
//our file
    FILE            *fp;
}EncData;

//this struct will hold a few basic
//information, needed for caching the frames.
typedef struct _CacheData{
    char    *workdir,  //The directory were the project will be stored, while recording.
                        //Since this will take a lot of space, the user must be
                        //able to change the location.
            *projname,  //This is the name of the folder that will hold the project.
                        //It is rMD-session-%d where %d is the pid of the current proccess.
                        //This way, running two instances will not create problems
                        //and also, a frontend can identify leftovers from a possible crash
                        //and delete them
            *imgdata,   //workdir+projname+img.out.gz
            *audiodata; //workdir+projname+audio.pcm

    gzFile  *ifp;       //image data file pointer
    FILE    *afp;       //audio data file pointer

}CacheData;

//sound buffer
//sound keeps coming so we que it in this list
//which we then traverse
typedef struct _SndBuffer{
    signed char *data;
    struct _SndBuffer *next;
}SndBuffer;

//this structure holds any data related to the program
//It's usage is mostly to be given as an argument to the
//threads,so they will have access to the program data, avoiding
//at the same time usage of any globals.
typedef struct _ProgData{
    ProgArgs args;//the program arguments
    DisplaySpecs specs;//Display specific information
    BRWindow brwin;//recording window
    Display *dpy;//curtrent display
    char *window_manager;//name of the window manager at program launch
    XImage *image;//the image that holds the current full screenshot
    XImage *shimage;//the image that holds the current full screenshot(shared memory)
    unsigned char *dummy_pointer;//a dummy pointer to be drawn in every frame
                                //data is casted to unsigned for later use in YUV buffer
    int dummy_p_size;//initially 16x16,always square
    unsigned char npxl;//this is the no pixel convention when drawing the dummy pointer
    char    *datamain,//the data of  image
            *datash,//the data of shimage
            *datatemp;//buffer for the temporary image,which will be
                      //preallocated in case shared memory is not used.
    RectArea *rect_root[2];//the interchanging list roots for storing the changed regions
    int list_selector,//selector for the above
        damage_event,//damage event base code
        damage_error,//damage error base code
        running;
    SndBuffer *sound_buffer;
    EncData *enc_data;
    CacheData *cache_data;
    int hard_pause;//if sound device doesn't support pause
                    //we have to close and reopen
    int avd;//syncronization among audio and video
    unsigned int periodtime,
                frametime;
    pthread_mutex_t list_mutex[2],//mutexes for concurrency protection of the lists
                    sound_buffer_mutex,
                    libogg_mutex,//libogg is not thread safe
                    yuv_mutex;//this might not be needed since we only have
                              //one read-only and  one write-only thread
                              //also on previous versions, y component was looped separately
                              //and then u and v so this was needed to avoid wrong coloring to render
                              //Currently this mutex only prevents the cursor from flickering
    pthread_cond_t  time_cond,//this gets a broadcast by the handler whenever it's time to get a screenshot
                    pause_cond,//this is blocks execution, when program is paused
                    sound_buffer_ready,//sound encoding finished
                    sound_data_read,//a buffer is ready for proccessing
                    image_buffer_ready;//image encoding finished
    snd_pcm_t *sound_handle;
    snd_pcm_uframes_t periodsize;
}ProgData;


//This is the header of every frame.
//Reconstruction will be correct only if made on
//the same platform.

//We need the total number of blocks
//for each plane.

//The number of the frame compared to the
//number of time expirations at the time of
//caching, will enable us to make up for lost frames.

//default 4+4+2+2+2=14!bad!
//me add pad, make god of 2 happy!
typedef struct _FrameHeader{
    char        frame_prefix[4];//always FRAM
    u_int32_t   frameno,//number of frame(cached frames)
                current_total;//number of frames that should have been
                                  //taken at time of caching this one
    u_int16_t   Ynum,//number of changed blocks in the Y plane
                Unum,//number of changed blocks in the U plane
                Vnum;//number of changed blocks in the V plane
    u_int16_t   pad;//always zero

}FrameHeader;

//The frame after retrieval.
//Based on the Header information
//we can read the correct amount of bytes.


typedef struct _CachedFrame{
    FrameHeader *header;
    unsigned char *YBlocks;//identifying number on the grid, starting at top left
    unsigned char *UBlocks;//       >>      >>
    unsigned char *VBlocks;//       >>      >>
    unsigned char *YData;//pointer to data for the blocks that have changed,
    unsigned char *UData;//which have to be remapped on the buffer when reading
    unsigned char *VData;
}CachedFrame;


/**Globals*/
//I've read somewhere that I'll go to hell for using globals...

int Paused,*Running,Aborted;
pthread_cond_t  *time_cond,*pause_cond;
unsigned char   Yr[256],Yg[256],Yb[256],
                Ur[256],Ug[256],Ub[256],
                Vr[256],Vg[256],Vb[256];
//the following values are of no effect
//but they might be usefull later for profiling
unsigned int    frames_total,//frames calculated by total time expirations
                frames_lost;//the value of shame
//used to determine frame drop which can
//happen on failure to receive a signal over a condition variable
int capture_busy,
    encoder_busy;


/**Macros*/

#define CLIP_EVENT_AREA(e,brwin,wgeom){\
    if(((e)->area.x<=(brwin)->rgeom.x)&&((e)->area.y<=(brwin)->rgeom.y)&&\
        ((e)->area.width>=(brwin)->rgeom.width)&&((e)->area.height<(brwin)->rgeom.height)){\
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
        (((brwin)->rgeom.height-(e)->area.y+(brwin)->rgeom.y<(e)->area.height)?\
        (brwin)->rgeom.height-(e)->area.y+(brwin)->rgeom.y:(e)->area.height):-1;\
    \
        if((wgeom)->width>(brwin)->rgeom.width)(wgeom)->width=(brwin)->rgeom.width;\
        if((wgeom)->height>(brwin)->rgeom.height)(wgeom)->height=(brwin)->rgeom.height;\
    }\
}

#define CLIP_DUMMY_POINTER_AREA(dummy_p_area,brwin,wgeom){\
    (wgeom)->x=((((dummy_p_area).x+(dummy_p_area).width>=(brwin)->rgeom.x)&&\
    ((dummy_p_area).x<=(brwin)->rgeom.x+(brwin)->rgeom.width))?\
    (((dummy_p_area).x<=(brwin)->rgeom.x)?(brwin)->rgeom.x:(dummy_p_area).x):-1);\
    (wgeom)->y=((((dummy_p_area).y+(dummy_p_area).height>=(brwin)->rgeom.y)&&\
    ((dummy_p_area).y<=(brwin)->rgeom.y+(brwin)->rgeom.height))?\
    (((dummy_p_area).y<=(brwin)->rgeom.y)?(brwin)->rgeom.y:(dummy_p_area).y):-1);\
    (wgeom)->width=((dummy_p_area).x<=(brwin)->rgeom.x)?\
    (dummy_p_area).width-((brwin)->rgeom.x-(dummy_p_area).x):\
    ((dummy_p_area).x<=(brwin)->rgeom.x+(brwin)->rgeom.width)?\
    ((brwin)->rgeom.width-(dummy_p_area).x+(brwin)->rgeom.x<(dummy_p_area).width)?\
    (brwin)->rgeom.width-(dummy_p_area).x+(brwin)->rgeom.x:(dummy_p_area).width:-1;\
    (wgeom)->height=((dummy_p_area).y<=(brwin)->rgeom.y)?\
    (dummy_p_area).height-((brwin)->rgeom.y-(dummy_p_area).y):\
    ((dummy_p_area).y<=(brwin)->rgeom.y+(brwin)->rgeom.height)?\
    ((brwin)->rgeom.height-(dummy_p_area).y+(brwin)->rgeom.y<(dummy_p_area).height)?\
    (brwin)->rgeom.height-(dummy_p_area).y+(brwin)->rgeom.y:(dummy_p_area).height:-1;\
    if((wgeom)->width>(brwin)->rgeom.width)(wgeom)->width=(brwin)->rgeom.width;\
    if((wgeom)->height>(brwin)->rgeom.height)(wgeom)->height=(brwin)->rgeom.height;\
}



#define DEFAULT_ARGS(args){\
    (args)->delay=0;\
    if(getenv("DISPLAY")!=NULL){\
        (args)->display=(char *)malloc(strlen(getenv("DISPLAY"))+1);\
        strcpy((args)->display,getenv("DISPLAY"));\
    }\
    else\
        (args)->display=NULL;\
    (args)->windowid=(args)->x=(args)->y\
    =(args)->width=(args)->height=(args)->quietmode\
    =(args)->nosound=(args)->full_shots=(args)->encOnTheFly\
    =(args)->zerocompression=(args)->nowmcheck=0;\
    (args)->noshared=1;\
    (args)->dropframes=(args)->nocondshared=0;\
    (args)->no_quick_subsample=1;\
    (args)->filename=(char *)malloc(8);\
    strcpy((args)->filename,"out.ogg");\
    (args)->encoding=OGG_THEORA_VORBIS;\
    (args)->cursor_color=1;\
    (args)->shared_thres=75;\
    (args)->have_dummy_cursor=0;\
    (args)->xfixes_cursor=1;\
    (args)->device=(char *)malloc(8);\
    strcpy((args)->device,"hw:0,0");\
    (args)->fps=15;\
    (args)->channels=1;\
    (args)->frequency=22050;\
    (args)->v_bitrate=45000;\
    (args)->v_quality=63;\
    (args)->s_quality=10;\
    (args)->workdir=(char *)malloc(strlen(getenv("HOME"))+1);\
    strcpy((args)->workdir,getenv("HOME"));\
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
    ((data_array[(k_tm*width_img+i_tm)*4+offset]+data_array[((k_tm-1)*width_img+i_tm)*4+offset]\
    +data_array[(k_tm*width_img+i_tm-1)*4+offset]+data_array[((k_tm-1)*width_img+i_tm-1)*4+offset])/4)

#define UPDATE_YUV_BUFFER_SH(yuv,data,x_tm,y_tm,width_tm,height_tm){\
    int i,k;\
    for(k=y_tm;k<y_tm+height_tm;k++){\
        for(i=x_tm;i<x_tm+width_tm;i++){\
            yuv->y[i+k*yuv->y_width]=Yr[data[(i+k*yuv->y_width)*4+__RBYTE]] + Yg[data[(i+k*yuv->y_width)*4+__GBYTE]] + Yb[data[(i+k*yuv->y_width)*4+__BBYTE]];\
            if((k%2)&&(i%2)){\
                yuv->u[i/2+k/2*yuv->uv_width]=Ur[data[(i+k*yuv->y_width)*4+__RBYTE]] + Ug[data[(i+k*yuv->y_width)*4+__GBYTE]] + Ub[data[(i+k*yuv->y_width)*4+__BBYTE]] ;\
                yuv->v[i/2+k/2*yuv->uv_width]=Vr[data[(i+k*yuv->y_width)*4+__RBYTE]] + Vg[data[(i+k*yuv->y_width)*4+__GBYTE]] + Vb[data[(i+k*yuv->y_width)*4+__BBYTE]] ;\
            }\
        }\
    }\
}

#define UPDATE_YUV_BUFFER_SH_AVG(yuv,data,x_tm,y_tm,width_tm,height_tm){\
    int i,k;\
    unsigned char avg0,avg1,avg2;\
    for(k=y_tm;k<y_tm+height_tm;k++){\
        for(i=x_tm;i<x_tm+width_tm;i++){\
            yuv->y[i+k*yuv->y_width]=Yr[data[(i+k*yuv->y_width)*4+__RBYTE]] + Yg[data[(i+k*yuv->y_width)*4+__GBYTE]] + Yb[data[(i+k*yuv->y_width)*4+__BBYTE]];\
            if((k%2)&&(i%2)){\
                avg2=AVG_4_PIXELS(data,(yuv->y_width),k,i,__RBYTE);\
                avg1=AVG_4_PIXELS(data,(yuv->y_width),k,i,__GBYTE);\
                avg0=AVG_4_PIXELS(data,(yuv->y_width),k,i,__BBYTE);\
                yuv->u[i/2+k/2*yuv->uv_width]=Ur[avg2] +\
                Ug[avg1] +\
                Ub[avg0] ;\
                yuv->v[i/2+k/2*yuv->uv_width]=Vr[avg2] +\
                Vg[avg1] +\
                Vb[avg0] ;\
            }\
        }\
    }\
}

#define UPDATE_YUV_BUFFER_IM(yuv,data,x_tm,y_tm,width_tm,height_tm){\
    int k,i;\
    register unsigned int t_val;\
    register unsigned int *datapi=(unsigned int*)data;\
    register unsigned char  *yuv_y=yuv->y+x_tm+y_tm*yuv->y_width,\
                            *yuv_u=yuv->u+x_tm/2+(y_tm*yuv->uv_width)/2,\
                            *yuv_v=yuv->v+x_tm/2+(y_tm*yuv->uv_width)/2,\
                            *_yr=Yr,*_yg=Yg,*_yb=Yb,\
                            *_ur=Ur,*_ug=Ug,*_ub=Ub,\
                            *_vr=Vr,*_vg=Vg,*_vb=Vb;\
\
    for(k=0;k<height_tm;k++){\
        for(i=0;i<width_tm;i++){\
            t_val=*datapi;\
            *yuv_y=_yr[__RVALUE(t_val)] + _yg[__GVALUE(t_val)] + _yb[__BVALUE(t_val)] ;\
            datapi++;\
            yuv_y++;\
        }\
        yuv_y+=yuv->y_width-width_tm;\
    }\
    datapi=(unsigned int*)data;\
    for(k=0;k<height_tm;k+=2){\
        for(i=0;i<width_tm;i+=2){\
            t_val=*datapi;\
            *yuv_u=\
            _ur[__RVALUE(t_val)] + _ug[__GVALUE(t_val)] + _ub[__BVALUE(t_val)];\
            *yuv_v=\
            _vr[__RVALUE(t_val)] + _vg[__GVALUE(t_val)] + _vb[__BVALUE(t_val)];\
            datapi+=2;\
            yuv_u++;\
            yuv_v++;\
        }\
        yuv_u+=(yuv->y_width-width_tm)/2;\
        yuv_v+=(yuv->y_width-width_tm)/2;\
        datapi+=width_tm;\
    }\
}


#define UPDATE_YUV_BUFFER_IM_AVG(yuv,data,x_tm,y_tm,width_tm,height_tm){\
    int i,k,j=0;\
    unsigned char avg0,avg1,avg2;\
    int x_2=x_tm/2,y_2=y_tm/2;\
    for(k=0;k<height_tm;k++){\
        for(i=0;i<width_tm;i++){\
            yuv->y[x_tm+i+(k+y_tm)*yuv->y_width]=Yr[data[(j*4)+__RBYTE]] + Yg[data[(j*4)+__GBYTE]] + Yb[data[(j*4)+__BBYTE]] ;\
            if((k%2)&&(i%2)){\
                avg2=AVG_4_PIXELS(data,width_tm,k,i,__RBYTE);\
                avg1=AVG_4_PIXELS(data,width_tm,k,i,__GBYTE);\
                avg0=AVG_4_PIXELS(data,width_tm,k,i,__BBYTE);\
                yuv->u[x_2+i/2+(k/2+y_2)*yuv->uv_width]=\
                Ur[avg2] + Ug[avg1] +\
                Ub[avg0];\
                yuv->v[x_2+i/2+(k/2+y_2)*yuv->uv_width]=\
                Vr[avg2] + Vg[avg1] +\
                Vb[avg0];\
            }\
            \
            j++;\
        }\
    }\
}

#define XFIXES_POINTER_TO_YUV(yuv,data,x_tm,y_tm,width_tm,height_tm,column_discard_stride){\
    int i,k,j=0;\
    unsigned char avg0,avg1,avg2,avg3;\
    int x_2=x_tm/2,y_2=y_tm/2;\
    for(k=0;k<height_tm;k++){\
        for(i=0;i<width_tm;i++){\
                yuv->y[x_tm+i+(k+y_tm)*yuv->y_width]=\
                (yuv->y[x_tm+i+(k+y_tm)*yuv->y_width]*(UCHAR_MAX-data[(j*4)+__ABYTE])+\
                (Yr[data[(j*4)+__RBYTE]] + Yg[data[(j*4)+__GBYTE]] + Yb[data[(j*4)+__BBYTE]])*data[(j*4)+__ABYTE])/UCHAR_MAX ;\
                if((k%2)&&(i%2)){\
                    avg3=AVG_4_PIXELS(data,(width_tm+column_discard_stride),k,i,__ABYTE);\
                    avg2=AVG_4_PIXELS(data,(width_tm+column_discard_stride),k,i,__RBYTE);\
                    avg1=AVG_4_PIXELS(data,(width_tm+column_discard_stride),k,i,__GBYTE);\
                    avg0=AVG_4_PIXELS(data,(width_tm+column_discard_stride),k,i,__BBYTE);\
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

#define DUMMY_POINTER_TO_YUV(yuv,data_tm,x_tm,y_tm,width_tm,height_tm,no_pixel){\
    int i,k,j=0;\
    int x_2=x_tm/2,y_2=y_tm/2,y_width_2=(yuv)->y_width/2;\
    for(k=0;k<height_tm;k++){\
        for(i=0;i<width_tm;i++){\
            if(data_tm[(j*4)]!=(no_pixel)){\
                (yuv)->y[x_tm+i+(k+y_tm)*(yuv)->y_width]=Yr[data_tm[(j*4)+__RBYTE]] + Yg[data_tm[(j*4)+__GBYTE]] + Yb[data_tm[(j*4)+__BBYTE]];\
                if((k%2)&&(i%2)){\
                    yuv->u[x_2+i/2+(k/2+y_2)*y_width_2]=Ur[data_tm[(k*width_tm+i)*4+__RBYTE]] + Ug[data_tm[(k*width_tm+i)*4+__GBYTE]] + Ub[data_tm[(k*width_tm+i)*4+__BBYTE]];\
                    yuv->v[x_2+i/2+(k/2+y_2)*y_width_2]=Vr[data_tm[(k*width_tm+i)*4+__RBYTE]] + Vg[data_tm[(k*width_tm+i)*4+__GBYTE]] + Vb[data_tm[(k*width_tm+i)*4+__BBYTE]] ;\
                }\
            }\
            j++;\
        }\
        j+=16-width_tm;\
    }\
}


#define I16TOA(number,buffer){\
    int t_num=(number),k=0,i=0;\
    char *t_buf=malloc(8);\
    t_num=t_num&((2<<15)-1);\
    while(t_num>0){\
        int digit=t_num%10;\
        t_buf[k]=digit+48;\
        t_num-=digit;\
        t_num/=10;\
        k++;\
    }\
    while(k>0)\
        (buffer)[i++]=t_buf[--k];\
    (buffer)[i]='\0';\
    free(t_buf);\
};\

#define INIT_FRAME(frame_t,fheader_t,yuv_t){\
    (frame_t)->header=(fheader_t);\
    (frame_t)->YBlocks=malloc(256);\
    (frame_t)->UBlocks=malloc(64);\
    (frame_t)->VBlocks=malloc(64);\
    (frame_t)->YData=malloc((yuv_t)->y_width*(yuv_t)->y_height);\
    (frame_t)->UData=malloc((yuv_t)->uv_width*(yuv_t)->uv_height);\
    (frame_t)->VData=malloc((yuv_t)->uv_width*(yuv_t)->uv_height);\
};

#define CLEAR_FRAME(frame_t){\
    free((frame_t)->YBlocks);\
    free((frame_t)->UBlocks);\
    free((frame_t)->VBlocks);\
    free((frame_t)->YData);\
    free((frame_t)->UData);\
    free((frame_t)->VData);\
};

/**Function prototypes*/

void *PollDamage(void *pdata);
void *GetFrame(void *pdata);
void *EncodeImageBuffer(void *pdata);
void *FlushToOgg(void *pdata);
void UpdateYUVBuffer(yuv_buffer *yuv,unsigned char *data,int x,int y,int width,int height);
void ClearList(RectArea **root);
int RectInsert(RectArea **root,WGeometry *wgeom);
int CollideRects(WGeometry *wgeom1,WGeometry *wgeom2,WGeometry **wgeom_return,int *ngeoms);
void SetExpired(int signum);
void RegisterCallbacks(ProgArgs *args);
void UpdateImage(Display * dpy,yuv_buffer *yuv,pthread_mutex_t *yuv_mutex,DisplaySpecs *specs,RectArea **root,BRWindow *brwin,EncData *enc,char *datatemp,int noshmem,int no_quick_subsample);
int GetZPixmap(Display *dpy,Window root,char *data,int x,int y,int width,int height);
int ParseArgs(int argc,char **argv,ProgArgs *arg_return);
void QueryExtensions(Display *dpy,ProgArgs *args,int *damage_event,int *damage_error);
int SetBRWindow(Display *dpy,BRWindow *brwin,DisplaySpecs *specs,ProgArgs *args);
unsigned char *MakeDummyPointer(DisplaySpecs *specs,int size,int color,int type,unsigned char *npxl);
void *CaptureSound(void *pdata);
void *EncodeSoundBuffer(void *pdata);
snd_pcm_t *OpenDev(const char *pcm_dev,unsigned int *channels,unsigned int *frequency,snd_pcm_uframes_t *periodsize,unsigned int *periodtime,int *hardpause);
void InitEncoder(ProgData *pdata,EncData *enc_data_t,int buffer_ready);
void MakeMatrices();
void SizePack2_8_16(int *start,int *size,int limit);
void *CacheImageBuffer(void *pdata);
void InitCacheData(ProgData *pdata,EncData *enc_data_t,CacheData *cache_data_t);
void *CacheSoundBuffer(void *pdata);
void *LoadCache(void *pdata);
void SyncEncodeImageBuffer(ProgData *pdata);
void CancelTimer(void);
void SyncEncodeSoundBuffer(ProgData *pdata,signed char *buff);
char *rmdWMCheck(Display *dpy,Window root);
#endif

