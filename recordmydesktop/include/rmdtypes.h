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

#ifndef RMDTYPES_H
#define RMDTYPES_H 1

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
    int cursor_color;   //black or white=>1 or 0
    int have_dummy_cursor;//disable/enable drawing of the dummy cursor
    int xfixes_cursor;   //disable/enable drawing of a cursor obtained
                        //through the xfixes extension
    float fps;            //desired framerate(default 15)
    unsigned int frequency;      //desired frequency (default 22050)
    unsigned int channels;       //no of channels(default 2)
    char *device;       //default sound device(default according to alsa or oss)
    snd_pcm_uframes_t buffsize;//buffer size(in frames) for sound capturing
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

#endif

