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

#elif __BYTE_ORDER == __BIG_ENDIAN

#define __ABYTE 0
#define __RBYTE 1
#define __GBYTE 2
#define __BBYTE 3

#else
#error Only little-endian and big-endian systems are supported
#endif

#define __RVALUE(tmp_val) (((tmp_val)&0x00ff0000)>>16)
#define __GVALUE(tmp_val) (((tmp_val)&0x0000ff00)>>8)
#define __BVALUE(tmp_val) (((tmp_val)&0x000000ff))

//500 mb file size
#define CACHE_FILE_SIZE_LIMIT (500*1<<20)


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
    int overwrite;//overwite a previously existing file(do not add a .number postfix)
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
    FILE    *uncifp;    //uncompressed image data file pointer

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
                    libogg_mutex,//libogg is not thread safe,
//                     libtheora_mutex,//same for libtheora
//                     libvorbis_mutex,//and libvorbis.
                    yuv_mutex;//this might not be needed since we only have
                              //one read-only and  one write-only thread
                              //also on previous versions, y component was looped separately
                              //and then u and v so this was needed to avoid wrong coloring to render
                              //Currently this mutex only prevents the cursor from flickering
    pthread_cond_t  time_cond,//this gets a broadcast by the handler whenever it's time to get a screenshot
                    pause_cond,//this is blocks execution, when program is paused
                    sound_buffer_ready,//sound encoding finished
                    sound_data_read,//a buffer is ready for proccessing
                    image_buffer_ready,//image encoding finished
                    theora_lib_clean,//the flush_ogg thread cannot procceed to creating last
                    vorbis_lib_clean;//packages until these two libs are no longer used, by other threads
    int th_encoding_clean,//these indicate a wait condition on the above cond vars
        v_encoding_clean;
    int v_enc_thread_waiting,
        th_enc_thread_waiting;
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
    =(args)->zerocompression=(args)->nowmcheck\
    =(args)->overwrite=0;\
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
    ((data_array[(k_tm*width_img+i_tm)*4+offset]+data_array[((k_tm-1)*width_img+i_tm)*4+offset]\
    +data_array[(k_tm*width_img+i_tm-1)*4+offset]+data_array[((k_tm-1)*width_img+i_tm-1)*4+offset])/4)

#define UPDATE_YUV_BUFFER_SH(yuv,data,x_tm,y_tm,width_tm,height_tm){\
    int k,i;\
    register unsigned int t_val;\
    register unsigned int *datapi=(unsigned int*)data+x_tm+y_tm*yuv->y_width;\
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
        datapi+=yuv->y_width-width_tm;\
    }\
    datapi=(unsigned int*)data+x_tm+y_tm*yuv->y_width;\
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
        datapi+=(2*yuv->y_width-width_tm);\
    }\
}

#define UPDATE_YUV_BUFFER_SH_AVG(yuv,data,x_tm,y_tm,width_tm,height_tm){\
    int k,i;\
    register unsigned int t_val,t1,t2,t3,t4;\
    register unsigned int *datapi=(unsigned int*)data+x_tm+y_tm*yuv->y_width,\
                          *datapi_next=(unsigned int*)data+x_tm+(y_tm+1)*yuv->y_width;\
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
        datapi+=yuv->y_width-width_tm;\
    }\
    datapi=(unsigned int*)data+x_tm+y_tm*yuv->y_width;\
    for(k=0;k<height_tm;k+=2){\
        for(i=0;i<width_tm;i+=2){\
            t1=*datapi;\
            t2=*(datapi+1);\
            t3=*datapi_next;\
            t4=*(datapi_next+1);\
            t_val=((((t1&0xff000000) +(t2&0xff000000)+\
            (t3&0xff000000)+(t4&0xff000000))/4)&0xff000000) \
            +((((t1&0x00ff0000) +(t2&0x00ff0000)+\
            (t3&0x00ff0000)+(t4&0x00ff0000))/4)&0x00ff0000)\
            +((((t1&0x0000ff00) +(t2&0x0000ff00)+\
            (t3&0x0000ff00)+(t4&0x0000ff00))/4)&0x0000ff00)\
            +((((t1&0x000000ff) +(t2&0x000000ff)+\
            (t3&0x000000ff)+(t4&0x000000ff))/4)&0x000000ff);\
\
            *yuv_u=\
            _ur[__RVALUE(t_val)] + _ug[__GVALUE(t_val)] + _ub[__BVALUE(t_val)];\
            *yuv_v=\
            _vr[__RVALUE(t_val)] + _vg[__GVALUE(t_val)] + _vb[__BVALUE(t_val)];\
            datapi+=2;\
            datapi_next+=2;\
            yuv_u++;\
            yuv_v++;\
        }\
        yuv_u+=(yuv->y_width-width_tm)/2;\
        yuv_v+=(yuv->y_width-width_tm)/2;\
        datapi+=(2*yuv->y_width-width_tm);\
        datapi_next+=(2*yuv->y_width-width_tm);\
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
    int k,i;\
    register unsigned int t_val,t1,t2,t3,t4;\
    register unsigned int *datapi=(unsigned int*)data,\
                          *datapi_next=(unsigned int*)data+width_tm;\
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
            t1=*datapi;\
            t2=*(datapi+1);\
            t3=*datapi_next;\
            t4=*(datapi_next+1);\
            t_val=((((t1&0xff000000) +(t2&0xff000000)+\
            (t3&0xff000000)+(t4&0xff000000))/4)&0xff000000) \
            +((((t1&0x00ff0000) +(t2&0x00ff0000)+\
            (t3&0x00ff0000)+(t4&0x00ff0000))/4)&0x00ff0000)\
            +((((t1&0x0000ff00) +(t2&0x0000ff00)+\
            (t3&0x0000ff00)+(t4&0x0000ff00))/4)&0x0000ff00)\
            +((((t1&0x000000ff) +(t2&0x000000ff)+\
            (t3&0x000000ff)+(t4&0x000000ff))/4)&0x000000ff);\
\
            *yuv_u=\
            _ur[__RVALUE(t_val)] + _ug[__GVALUE(t_val)] + _ub[__BVALUE(t_val)];\
            *yuv_v=\
            _vr[__RVALUE(t_val)] + _vg[__GVALUE(t_val)] + _vb[__BVALUE(t_val)];\
            datapi+=2;\
            datapi_next+=2;\
            yuv_u++;\
            yuv_v++;\
        }\
        yuv_u+=(yuv->y_width-width_tm)/2;\
        yuv_v+=(yuv->y_width-width_tm)/2;\
        datapi+=width_tm;\
        datapi_next+=width_tm;\
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

/**
* Loop calling XNextEvent.Retrieve and place on
* list damage events that arive, create damage for new windows.
* \param pdata ProgData struct containing all program data
*/
void *PollDamage(ProgData *pdata);

/**
* Retrieve frame form xserver, and transform to a yuv buffer,
* either directly(full shots) or by calling UpdateImage.
* \param pdata ProgData struct containing all program data
*/
void *GetFrame(ProgData *pdata);

/**
* feed a yuv buffer to the theora encoder and submit outcome to
* the ogg stream.
* \param pdata ProgData struct containing all program data
*/
void *EncodeImageBuffer(ProgData *pdata);

/**
* Query theora and vorbis streams for ready packages and
* flush them on the disk
* \param pdata ProgData struct containing all program data
*/
void *FlushToOgg(ProgData *pdata);

/**
* Clean up a list of areas marked for update.
* \param root Root entry of the list
*/
void ClearList(RectArea **root);

/**
* Insert a new rectangle on the list, making sure it doesn't overlap
* with the existing ones
* \param root Root entry of the list
*
* \param wgeom New area to be inserted
*
* \returns Number of insertions during operation
*
* \note This function is reentrant and recursive. The number
* of insertions takes this into account.
*/
int RectInsert(RectArea **root,WGeometry *wgeom);

/**
* Collide two rectangles and dictate most sane action for insertion,
* as well as provide the updated rectangle(s)
* \param wgeom1 resident rectangle
*
* \param wgeom2 New rectangle
*
* \param wgeom_return Pointer to rectangles to be inserted
*
* \param ngeoms number of entries in wgeom_return
*
* \retval 0 No collision
*
* \retval 1 wgeom1 is covered by wgeom2
*
* \retval 2 wgeom2 is covered by wgeom1
*
* \retval -1 wgeom1 was broken (new is picked up in wgeom_return)
*
* \retval -2 wgeom2 was broken (new is picked up in wgeom_return)
*
* \retval -10 Grouping the two geoms is possible
*
*/
int CollideRects(WGeometry *wgeom1,WGeometry *wgeom2,WGeometry **wgeom_return,int *ngeoms);

/**
* Broadcast time condition variable, increment frame count.
*
* \param signum Number of signal received(unused, always SIGALRM)
*
*/
void SetExpired(int signum);

/**
* Set up all callbacks and signal handlers
* \param pdata ProgData struct containing all program data
*/
void RegisterCallbacks(ProgArgs *args);

/**
* Retrieve and apply all changes, if xdamage is used.
*
* \param dpy Connection to the server
*
* \param yuv yuv_buffer that is to be modified
*
* \param yuv_mutex lock on the buffer
*
* \param specs DisplaySpecs struct with information about the display to be recorded
*
* \param root Root entry of the list with damaged areas
*
* \param brwin BRWindow struct contaning the recording window specs
*
* \param enc Encoding options
*
* \param datatemp Buffer for pixel data to be retrieved before placed on the yuv buffer
*
* \param noshmem don't use MIT_Shm extension
*
* \param no_quick_subsample Don't do quick subsampling
*
*/
void UpdateImage(Display * dpy,
                yuv_buffer *yuv,
                pthread_mutex_t *yuv_mutex,
                DisplaySpecs *specs,
                RectArea **root,
                BRWindow *brwin,
                EncData *enc,
                char *datatemp,
                int noshmem,
                int no_quick_subsample);

/**
* Rerieve pixmap data from xserver
*
* \param dpy Connection to the server
*
* \param root root window of the display
*
* \param data (preallocated)buffer to place the data
*
* \param x x position of the screenshot
*
* \param y y position of the screenshot
*
* \param x x position of the screenshot
*
* \param width width of the screenshot
*
* \param height height position of the screenshot
*
* \param x x position of the screenshot
*
* \returns 0 on Success 1 on Failure
*/
int GetZPixmap(Display *dpy,Window root,char *data,int x,int y,int width,int height);

/**
* Fill ProgArgs struct with arguments entered at execution
*
* \param argc argc as entered  from main
*
* \param argv argv as entered  from main
*
* \param arg_return ProgArgs struct to be filled with the options
*
* \returns 0 on Success 1 on Failure
*/
int ParseArgs(int argc,char **argv,ProgArgs *arg_return);

/**
* Check if needed extensions are present
*
* \param dpy Connection to the server
*
* \param args ProgArgs struct containing the user-set options
*
* \param damage_event gets filled with damage event number
*
* \param damage_error gets filled with damage error number
*
* \note Can be an exit point if extensions are not found
*/
void QueryExtensions(Display *dpy,ProgArgs *args,int *damage_event,int *damage_error);

/**
* Check and align window size
*
* \param dpy Connection to the server
*
* \param brwin BRWindow struct contaning the initial and final window
*
* \param specs DisplaySpecs struct with information about the display to be recorded
*
* \param args ProgArgs struct containing the user-set options
*
* \returns 0 on Success 1 on Failure
*/
int SetBRWindow(Display *dpy,BRWindow *brwin,DisplaySpecs *specs,ProgArgs *args);

/**
* Create an array containing the data for the dummy pointer
*
* \param specs DisplaySpecs struct with information about the display to be recorded
*
* \param size  Pointer size, always square, always 16.(exists only for the possibility to create
*               more dummy cursors)
* \param color 0 white, 1 black
*
* \param type Always 0.(exists only for the possibility to create
*               more dummy cursors)
*
* \param npxl Return of pixel value that denotes non-drawing, while applying the cursor
*             on the target image
*
* \returns Pointer to pixel data of the cursor
*/
unsigned char *MakeDummyPointer(DisplaySpecs *specs,int size,int color,int type,unsigned char *npxl);

/**
* Sound capturing thread. Data are placed on a list to be picked up by other threads.
*
* \param pdata ProgData struct containing all program data
*/
void *CaptureSound(ProgData *pdata);

/**
* Sound encoding thread. Picks up data from the buffer queue , encodes and places them
* on the vorbis stream.
*
* \param pdata ProgData struct containing all program data
*/
void *EncodeSoundBuffer(ProgData *pdata);

/**
* Open sound device, with the desired parameters
*
* \param pcm_dev name of the device
*
* \param channels desired number of channels(gets modified with the acieved value)
*
* \param frequency desired frequency(gets modified with the acieved value)
*
* \param periodsize Size of a period
*
* \param periodtime Duration of a period
*
* \param hardpause Set to 1 when the device has to be stopped during pause
*                  and to 0 when it supports pausing
*
* \returns snd_pcm_t handle on success, NULL on failure
*/
snd_pcm_t *OpenDev( const char *pcm_dev,
                    unsigned int *channels,
                    unsigned int *frequency,
                    snd_pcm_uframes_t *periodsize,
                    unsigned int *periodtime,
                    int *hardpause);
/**
* Initialize theora,vorbis encoders, and their respective ogg streams.
*
* \param pdata ProgData struct containing all program data
*
* \param enc_data_t Encoding options
*
* \param buffer_ready when 1, the yuv buffer must be preallocated
*                     when 0 InitEncoder will alocate a new one
*
*/
void InitEncoder(ProgData *pdata,EncData *enc_data_t,int buffer_ready);

/**
* Fill Yr,Yg,Yb,Ur,Ug.Ub,Vr,Vg,Vb arrays(globals) with values.
*/
void MakeMatrices();
/**
*Align the recording window to a divisible by 2 pixel start and
*and a size divisible by 16.
*
* \param start x or y of the recording window
*
* \param size  width or height of the recording window
*
* \param limit  width or height of the Display
*
* \note This is called separately for width and height.
*/
void SizePack2_8_16(int *start,int *size,int limit);

/**
* Image caching thread. Copies the yuv buffer, compares with the last one and
* caches the result.
*
* \param pdata ProgData struct containing all program data
*
*/
void *CacheImageBuffer(ProgData *pdata);

/**
* Initializes paths and everything else needed to start caching
*
* \param pdata ProgData struct containing all program data
*
* \param enc_data_t Encoding options
*
* \param cache_data_t Caching options
*
*/
void InitCacheData(ProgData *pdata,EncData *enc_data_t,CacheData *cache_data_t);

/**
* Sound caching thread. Simply writes the pcm buffers on disk
*
* \param pdata ProgData struct containing all program data
*
*/
void *CacheSoundBuffer(ProgData *pdata);

/**
* Cache loading and processing thread
*
* \param pdata ProgData struct containing all program data
*
*/
void *LoadCache(ProgData *pdata);

/**
* As EncodeImageBuffer, only with the assumption that this is not a thread on it's own
*
* \param pdata ProgData struct containing all program data
*
*/
void SyncEncodeImageBuffer(ProgData *pdata);

/**
* Stop the timer
*/
void CancelTimer(void);

/**
* As EncodeSoundBuffer, only with the assumption that this is not a thread on it's own
*
* \param pdata ProgData struct containing all program data
*
*/
void SyncEncodeSoundBuffer(ProgData *pdata,signed char *buff);

/**
*Check current running window manager.
*
* \param dpy Connection to the server
*
* \param root root window of the display
*
* \returns Window manager name
*/
char *rmdWMCheck(Display *dpy,Window root);

/**
*Construct an number postfixed name
*
* \param name base name
*
* \param newname modified name
*
* \n number to be used as a postfix
*
*/
void CacheFileN(char *name,char **newname,int n);

/**
* Change file pointer to a new file while writting
* (file name is incremented with CacheFileN)
*
* \param name base file name
*
* \param n number to be used as a postfix
*
* \param fp File pointer if compression is used(must be NULL otherwise)
*
* \param ucfp File pointer if compression is NOT used(must be NULL otherwise)
*
* \returns 0 on Success 1 on Failure
*/
int SwapCacheFilesWrite(char *name,int n,gzFile **fp,FILE **ucfp);

/**
* Change file pointer to a new file while reading
* (file name is incremented with CacheFileN)
*
* \param name base file name
*
* \param n number to be used as a postfix
*
* \param fp File pointer if compression is used(must be NULL otherwise)
*
* \param ucfp File pointer if compression is NOT used(must be NULL otherwise)
*
* \returns 0 on Success 1 on Failure
*/
int SwapCacheFilesRead(char *name,int n,gzFile **fp,FILE **ucfp);

/**
* Delete all cache files
*
* \param cache_data_t Caching options(file names etc.)
*
* \returns 0 if all files and folders where deleted, 1 otherwise
*/
int PurgeCache(CacheData *cache_data_t,int sound);
#endif

