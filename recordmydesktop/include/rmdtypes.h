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
*   For further information contact me at johnvarouhakis@gmail.com            *
******************************************************************************/

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
#ifdef HAVE_MACHINE_ENDIAN_H
    #include <machine/endian.h>
#else
    #include <endian.h>
#endif
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
#include <X11/extensions/shmstr.h>
#include <theora/theora.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <ogg/ogg.h>

#ifdef HAVE_LIBASOUND
    #include <alsa/asoundlib.h>
#else
    #include <sys/ioctl.h>
    #include <sys/soundcard.h>
#endif

#ifdef HAVE_JACK_H
    #include <jack/jack.h>
    #include <jack/ringbuffer.h>
    #include <dlfcn.h>
#endif


//this type exists only
//for comparing the planes at caching.
//u_int64_t mught not be available everywhere.
//The performance gain comes from casting the unsigned char
//buffers to this type before comparing the two blocks.
//This is made possible by the fact that blocks
//for the Y plane are 16 bytes in width and blocks
//for the U,V planes are 8 bytes in width
#ifdef HAVE_U_INT64_T
typedef u_int64_t cmp_int_t;
#else
typedef u_int32_t cmp_int_t;
#endif

//type of pixel proccessing for the Cb,Cr planes
//when converting from full rgb to 4:2:2 Ycbcr
enum{
    __PXL_DISCARD,  //only select 1 pixel in every block of four
    __PXL_AVERAGE   //calculate the average of all four pixels
};

typedef struct _DisplaySpecs{   //this struct holds some basic information
    int screen;                 //about the display,needed mostly for
    unsigned int width;                 //validity checks at startup
    unsigned int height;
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
    Window windowid;        //id
}BRWindow;

//defaults in the following comment lines may be out of sync with reality
//check DEFAULT_ARGS macro further bellow
typedef struct _ProgArgs{
    int delay;          //start up delay
    Window windowid;    //window to record(default root)
    char *display;      //display to connect(default :0)
    int x,y;            //x,y offset(default 0,0)
    int width,height;   //defaults to window width and height
    char *filename;     //output file(default out.[ogg|*])
    int cursor_color;   //black or white=>1 or 0
    int have_dummy_cursor;  //disable/enable drawing of the dummy cursor
    int xfixes_cursor;      //disable/enable drawing of a cursor obtained
                            //through the xfixes extension
    float fps;              //desired framerate(default 15)
    unsigned int frequency;     //desired frequency (default 22050)
    unsigned int channels;      //no of channels(default 2)
    char *device;               //default sound device
#ifdef HAVE_LIBASOUND
    snd_pcm_uframes_t buffsize; //buffer size(in frames) for sound capturing
#else
    u_int32_t buffsize;
#endif
    int nosound;        //do not record sound(default 0)
    int noshared;       //do not use shared memory extension(default 0)
    int nowmcheck;      //do not check if there's a 3d comp window manager
                        //(which changes full-shots and with-shared to 1)
    int full_shots;     //do not poll damage, take full screenshots
    int follow_mouse;   //capture area follows the mouse(fullshots auto enabled)
    int no_encode;      //do not encode or delete the temporary files(debug opt)
    int no_quick_subsample;             //average pixels in chroma planes
    int v_bitrate,v_quality,s_quality;  //video bitrate,video-sound quality
    int encOnTheFly;    //encode while recording, no caching(default 0)
    char *workdir;      //directory to be used for cache files(default $HOME)
    char *pause_shortcut;   //pause/unpause shortcut sequence(Control+Alt+p)
    char *stop_shortcut;    //stop shortcut sequence(Control+Alt+s)
    int zerocompression;    //image data are always flushed uncompressed
    int overwrite;          //overwite a previously existing file
                            //(do not add a .number postfix)
    int use_jack;           //record audio with jack
    unsigned int jack_nports;
    char **jack_port_names;
    float jack_ringbuffer_secs;
}ProgArgs;

//this struct holds anything related to encoding AND
//writting out to file.
typedef struct _EncData{
    ogg_stream_state m_ogg_ts;  //theora
    ogg_stream_state m_ogg_vs;  //vorbis
    ogg_page         m_ogg_pg;  //this could be avoided since
                                // it is used only while initializing
    ogg_packet       m_ogg_pckt1;   //theora stream
    ogg_packet       m_ogg_pckt2;   //vorbis stream
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
    char    *workdir,   //The directory were the project
                        //will be stored, while recording.
                        //Since this will take a lot of space, the user must be
                        //able to change the location.
            *projname,  //This is the name of the folder that
                        //will hold the project.
                        //It is rMD-session-%d where %d is the pid
                        //of the current proccess.
                        //This way, running two instances
                        //will not create problems
                        //and also, a frontend can identify
                        //leftovers from a possible crash
                        //and delete them
            *specsfile, //workdir+projname+specs.txt
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

#ifdef HAVE_JACK_H
typedef struct _JackData{
    void *jack_lib_handle;      //handle for jack library (loaded with dlopen).
    jack_client_t   *client;
    unsigned int    buffersize, //buffer size for every port in frames.
                    frequency,  //samplerate with which jack server was started.
                    nports;     //number of ports.
    float           ringbuffer_secs;
    char **port_names;          //names of ports(as specified in args).
    jack_port_t **ports;        //connections to thes ports.
    jack_default_audio_sample_t **portbuf;  //retrieval of audio buffers.
    pthread_mutex_t *snd_buff_ready_mutex;  //mutex and cond_var
    pthread_cond_t *sound_data_read;        //in the pdata struct
    jack_ringbuffer_t *sound_buffer;        //data exchange happens through this
    int capture_started;        //used to hold recording in the beginning
}JackData;
#endif

typedef struct _HotKey{     //Hold info about the shortcuts
    int modnum;             //modnum is the number of modifier masks
    unsigned int mask[4];  //that should be checked (the initial
    int key;                //user requested modifier plus it's
}HotKey;                    //combinations with LockMask and NumLockMask).

//this structure holds any data related to the program
//It's usage is mostly to be given as an argument to the
//threads,so they will have access to the program data, avoiding
//at the same time usage of any globals.
typedef struct _ProgData{
/**recordMyDesktop specific structs*/
    ProgArgs args;          //the program arguments
    DisplaySpecs specs;     //Display specific information
    BRWindow brwin;         //recording window
    RectArea *rect_root[2]; //the interchanging list roots for storing
                            //the changed regions
    SndBuffer   *sound_buffer;
    EncData     *enc_data;
    CacheData   *cache_data;
    HotKey      pause_key,  //Shortcuts
                stop_key;
#ifdef HAVE_JACK_H
    JackData    *jdata;
#endif
/**X related info*/
    Display *dpy;           //curtrent display
/** Mutexes*/
    pthread_mutex_t list_mutex[2],  //mutexes for concurrency
                                    //protection of the lists
                    sound_buffer_mutex,
                    snd_buff_ready_mutex,
                    img_buff_ready_mutex,
                    theora_lib_mutex,
                    vorbis_lib_mutex,
                    libogg_mutex,   //libogg is not thread safe,
                    yuv_mutex;  //this might not be needed since we only have
                                //one read-only and  one write-only thread
                                //also on previous versions,
                                //y component was looped separately
                                //and then u and v so this was needed
                                //to avoid wrong coloring to render
                                //Currently this mutex only prevents
                                //the cursor from flickering
/**Condition Variables*/
    pthread_cond_t  time_cond,  //this gets a broadcast by the handler
                                //whenever it's time to get a screenshot
                    pause_cond, //this is blocks execution,
                                //when program is paused
                    sound_buffer_ready, //sound encoding finished
                    sound_data_read,    //a buffer is ready for proccessing
                    image_buffer_ready, //image encoding finished
                    theora_lib_clean,   //the flush_ogg thread cannot
                                        //procceed to creating last
                    vorbis_lib_clean;   //packages until these two libs
                                        //are no longer used, by other threads
/**Buffers,Flags and other vars*/
    unsigned char *dummy_pointer,   //a dummy pointer to be drawn
                                    //in every frame
                                    //data is casted to unsigned for
                                    //later use in YUV buffer
                  npxl;     //this is the no pixel convention
                            //when drawing the dummy pointer
    unsigned int periodtime,//time that a sound buffer lasts (microsecs)
                frametime;  //time that a frame lasts (microsecs)
    char    *window_manager;   //name of the window manager at program launch
    int list_selector,      //selector for the rect_roots
        damage_event,       //damage event base code
        damage_error,       //damage error base code
        shm_opcode,         //MIT-Shm opcode
        running,            //1 while the program is capturing/paused/encoding
        dummy_p_size,       //dummy pointer size,initially 16x16,always square
        th_encoding_clean,      //thread exit inidcator
        v_encoding_clean,       //  >>  >>
        v_enc_thread_waiting,   //these indicate a wait
        th_enc_thread_waiting,  //condition on the cond vars.
        hard_pause,         //if sound device doesn't support pause
                            //we have to close and reopen
        avd;                //syncronization among audio and video
#ifdef HAVE_LIBASOUND
    snd_pcm_t *sound_handle;
    snd_pcm_uframes_t periodsize;
#else
    int sound_handle;
    u_int32_t periodsize;
#endif
}ProgData;


//This is the header of every frame.
//Reconstruction will be correct only if made on
//the same platform.

//We need the total number of blocks
//for each plane.

//The number of the frame compared to the
//number of time expirations at the time of
//caching, will enable us to make up for lost frames.


typedef struct _FrameHeader{
    char        frame_prefix[4];    //always FRAM
    u_int32_t   frameno,            //number of frame(cached frames)
                current_total;      //number of frames that should have been
                                    //taken at time of caching this one
    u_int32_t   Ynum,   //number of changed blocks in the Y plane
                Unum,   //number of changed blocks in the U plane
                Vnum;   //number of changed blocks in the V plane
}FrameHeader;

//The frame after retrieval.
//Based on the Header information
//we can read the correct amount of bytes.


typedef struct _CachedFrame{
    FrameHeader *header;
    u_int32_t     *YBlocks,     //identifying number on the grid,
                  *UBlocks,     //starting at top left
                  *VBlocks;     //       >>      >>
    unsigned char *YData,   //pointer to data for the blocks that have changed,
                  *UData,   //which have to be remapped
                  *VData;   //on the buffer when reading
}CachedFrame;

#endif

