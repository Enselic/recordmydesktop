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


#ifndef RMDFUNC_H
#define RMDFUNC_H 1

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "rmdtypes.h"


/**Function prototypes*/

/**
* Start listening to damage and substructure notify events
* (needed before EventLoop call)
* \param pdata ProgData struct containing all program data
*/
void InitEventsPolling(ProgData *pdata);


/**
* Loop calling XNextEvent.Retrieve and place on
* list damage events that arive, create damage for new windows
* and pickup key events for shortcuts.
* \param pdata ProgData struct containing all program data
*/
void EventLoop(ProgData *pdata);

/**
* Loop ,signal timer cond var,sleep-\ 
*  ^                                 |
*  |________________________________/
*   
*
* \param pdata ProgData struct containing all program data
*/
void *rmdTimer(ProgData *pdata);

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
* \param specs DisplaySpecs struct with
*              information about the display to be recorded
*
* \param root Root entry of the list with damaged areas
*
* \param brwin BRWindow struct contaning the recording window specs
*
* \param enc Encoding options
*
* \param datatemp Buffer for pixel data to be
*                 retrieved before placed on the yuv buffer
*
* \param noshmem don't use MIT_Shm extension
*
* \param no_quick_subsample Don't do quick subsampling
*
*/
void UpdateImage(Display * dpy,
                yuv_buffer *yuv,
                DisplaySpecs *specs,
                RectArea **root,
                BRWindow *brwin,
                EncData *enc,
                char *datatemp,
                int noshmem,
                XShmSegmentInfo *shminfo,
                int shm_opcode,
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
* \returns 0 on Success 1 on Failure
*/
int GetZPixmap(Display *dpy,
               Window root,
               char *data,
               int x,
               int y,
               int width,
               int height);

/**
* Rerieve pixmap data from xserver through the MIT-Shm extension
*
* \param dpy Connection to the server
*
* \param root root window of the display
*
* \param shminfo Info for the shared memory segment
*
* \param shm_opcode Opcode of Shm extension
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
* \returns 0 on Success 1 on Failure
*/
int GetZPixmapSHM(Display *dpy,
                  Window root,
                  XShmSegmentInfo *shminfo,
                  int shm_opcode,
                  char *data,
                  int x,
                  int y,
                  int width,
                  int height);

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
void QueryExtensions(Display *dpy,
                     ProgArgs *args,
                     int *damage_event,
                     int *damage_error,
                     int *shm_opcode);

/**
* Check and align window size
*
* \param dpy Connection to the server
*
* \param brwin BRWindow struct contaning the initial and final window
*
* \param specs DisplaySpecs struct with
*              information about the display to be recorded
*
* \param args ProgArgs struct containing the user-set options
*
* \returns 0 on Success 1 on Failure
*/
int SetBRWindow(Display *dpy,
                BRWindow *brwin,
                DisplaySpecs *specs,
                ProgArgs *args);

/**
* Create an array containing the data for the dummy pointer
*
* \param specs DisplaySpecs struct with
*              information about the display to be recorded
*
* \param size  Pointer size, always square, always 16.(exists only
*              for the possibility to create more dummy cursors)
* \param color 0 white, 1 black
*
* \param type Always 0.(exists only for the possibility to create
*               more dummy cursors)
*
* \param npxl Return of pixel value that denotes non-drawing, while
*             applying the cursor on the target image
*
* \returns Pointer to pixel data of the cursor
*/
unsigned char *MakeDummyPointer(DisplaySpecs *specs,
                                int size,
                                int color,
                                int type,
                                unsigned char *npxl);

/**
* Sound capturing thread. Data are placed on a
* list to be picked up by other threads.
*
* \param pdata ProgData struct containing all program data
*/
void *CaptureSound(ProgData *pdata);

/**
* Sound encoding thread. Picks up data from the buffer queue ,
* encodes and places them on the vorbis stream.
*
* \param pdata ProgData struct containing all program data
*/
void *EncodeSoundBuffer(ProgData *pdata);
#ifdef HAVE_LIBASOUND
/**
* Try to open (alsa) sound device, with the desired parameters,
* and place the obtained ones on their place
*
* \param pcm_dev name of the device
*
* \param channels desired number of channels
*                 (gets modified with the acieved value)
*
* \param frequency desired frequency(gets modified with the acieved value)
*
* \param buffsize Size of buffer
*
* \param periodsize Size of a period(can be NULL)
*
* \param periodtime Duration of a period(can be NULL)
*
* \param hardpause Set to 1 when the device has to be stopped during pause
*                  and to 0 when it supports pausing
*                  (can be NULL)
*
* \returns snd_pcm_t handle on success, NULL on failure
*/
snd_pcm_t *OpenDev( const char *pcm_dev,
                    unsigned int *channels,
                    unsigned int *frequency,
                    snd_pcm_uframes_t *buffsize,
                    snd_pcm_uframes_t *periodsize,
                    unsigned int *periodtime,
                    int *hardpause);
#else
/**
* Try to open (OSS) sound device, with the desired parameters.
*
*
* \param pcm_dev name of the device
*
* \param channels desired number of channels
*
* \param frequency desired frequency
*
*
* \returns file descriptor of open device,-1 on failure
*/
int OpenDev( const char *pcm_dev,
             unsigned int channels,
             unsigned int frequency);
#endif
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
void InitCacheData(ProgData *pdata,
                   EncData *enc_data_t,
                   CacheData *cache_data_t);

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
* As EncodeImageBuffer, only with the assumption that
* this is not a thread on it's own
*
* \param pdata ProgData struct containing all program data
*
*/
void SyncEncodeImageBuffer(ProgData *pdata);


/**
* As EncodeSoundBuffer, only with the assumption that
* this is not a thread on it's own
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

/**
* Encode cache into an
* ogg stream.
* \param pdata ProgData struct containing all program data
*/
void EncodeCache(ProgData *pdata);

/**
* Launch and wait capture threads.
* Also creates and waits the encoding threads when
* encode-on-the-fly is enabled.
*
* \param pdata ProgData struct containing all program data
*/
void rmdThreads(ProgData *pdata);

/**
* initialize lists,mutexes,image buffers, take first screenshot,
* and anything else needed before launching the capture threads.
*
* \param pdata ProgData struct containing all program data
*
* \param enc_data reference to enc_data structure
*
* \param cache_data reference to cache_data structure
*
* \returns 0 on success, other values must cause the program to exit
*/
int InitializeData(ProgData *pdata,
                   EncData *enc_data,
                   CacheData *cache_data);

#ifdef HAVE_JACK_H

size_t (*jack_ringbuffer_read_p)(jack_ringbuffer_t *rb,
                                 char *dest, size_t cnt);
size_t (*jack_ringbuffer_read_space_p)(const jack_ringbuffer_t *rb);

/**
*   Load libjack, create and activate client,register ports
*
*   \param jdata_t  Pointer to JackData struct containing port
*                   and client information
*
*   \returns 0 on Success, error code on failure
*            (depending on where the error occured)
*/
int StartJackClient(JackData *jdata);

/**
*   Close Jack Client
*
*   \param jdata_t  Pointer to JackData struct containing port
*                   and client information
*
*   \returns 0 on Success, 1 on failure
*/
int StopJackClient(JackData *jdata);

#endif


/**
*   Currently only frees some memory
*   (y,u,v blocks)
*
*
*/
void CleanUp(void);


/*
 * Check a shortcut combination and if valid, 
 * register it, on the root window.
 *
 * \param dpy Connection to the X Server
 *
 * \param root Root window id
 *
 * \param shortcut String represantation of the shortcut
 *
 * \param HotKey Pre-allocated struct that is filled with the 
 *               modifiers and the keycode, for checks on incoming 
 *               keypress events. Left untouched if the call fails.
 *
 *  
 *  \returns 0 on Success, 1 on Failure. 
 *
 */
int RegisterShortcut(Display *dpy,
                     Window root,
                     const char *shortcut,
                     HotKey *hotkey);

/*
 * Handling of X errors.
 * Ignores, bad access when registering shortcuts
 * and BadWindow on XQueryTree
 *
 * \param dpy Connection to the X Server
 *
 * \param e XErrorEvent struct containing error info
 *
 * \returns 0 on the two ignored cases, calls exit(1)
 *            otherwise.
 *
 */
int rmdErrorHandler(Display *dpy,XErrorEvent *e);

/*
 * Create a frame that marks the recording area.
 *
 * \param dpy Connection to the X Server
 *
 * \param screen Recorded screen
 *
 * \param root Root window of the display
 *
 * \param x X pos of the recorded area
 *
 * \param y Y pos of the recorded area
 *
 * \param width Width of the recorded area
 *
 * \param height Height of the recorded area
 *
 * \returns The WindowID of the frame
 *
 */
Window rmdFrameInit(Display *dpy,
                    int screen,
                    Window root,
                    int x,
                    int y,
                    int width,
                    int height);


/*
 * Move the frame (subtracts the borderwidth)
 *
 * \param dpy Connection to the X Server
 *
 * \param win WindowId of the frame
 *
 * \param x New X pos of the recorded area
 *
 * \param y New Y pos of the recorded area
 *
 */
void rmdMoveFrame(Display *dpy,
                  Window win,
                  int x,
                  int y);


/*
 * Redraw the frame that marks the recording area.
 *
 * \param dpy Connection to the X Server
 *
 * \param screen Recorded screen
 *
 * \param win WindoID of the frame
 *
 * \param width Width of the recorded area
 *
 * \param height Height of the recorded area
 *
 */
void rmdDrawFrame(Display *dpy,
                  int screen,
                  Window win,
                  int width,
                  int height);


/*
 * Create a text file that holds the required
 * capture attributes, in the cache directory.
 *
 * \param pdata ProgData struct containing all program data
 *
 * \returns 0 on Success, 1 on failure
 *
 */ 
int WriteSpecsFile(ProgData *pdata);



/*
 * Read the  text file that holds the required
 * capture attributes, in the cache directory.
 *
 * \param pdata ProgData struct that will be fille 
 *        with  all program data
 *
 * \returns 0 on Success, 1 on failure
 *
 */ 
int ReadSpecsFile(ProgData *pdata);


/*
 * Rescue a previous recording, found in 
 * the given path.
 *
 * \param path Path to the cache folder.
 *
 * \returns 0 on Success, 1 on failure
 *
 */
int rmdRescue(const char *path);

#endif






