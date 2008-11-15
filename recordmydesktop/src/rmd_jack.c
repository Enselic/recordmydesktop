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

#include "config.h"

#include <pthread.h>

#include "rmd_types.h"

#include "rmd_jack.h"


#ifdef HAVE_LIBJACK


/**
*   Callback for capture through jack
*
*   \param  nframes Number of frames captured
*
*   \param jdata_t  Pointer to JackData struct containing port
*                   and client information
*
*   \returns Zero always
*/
static int JackCapture(jack_nframes_t nframes,void *jdata_t) {
    int i=0;
    JackData *jdata=(JackData *)jdata_t;

    if (!jdata->pdata->running  || jdata->pdata->paused || !jdata->capture_started) {
        return 0;
    }

    for(i= 0;i<jdata->nports;i++)
        jdata->portbuf[i]=jack_port_get_buffer(jdata->ports[i],nframes);
//vorbis analysis buffer wants uninterleaved data
//so we are simply placing the buffers for every channel
//sequentially on the ringbuffer
    for(i=0;i<jdata->nports;i++)
        (*jack_ringbuffer_write)(jdata->sound_buffer,
                                   (void *)(jdata->portbuf[i]),
                                   nframes*
                                   sizeof(jack_default_audio_sample_t));
/*FIXME */
//This is not safe.
//cond_var signaling must move away from signal handlers
//alltogether (JackCapture, SetExpired, SetPaused).
//Better would be a set of pipes for each of these.
//The callback should write on the pipe and the main thread
//should perform a select over the fd's, signaling afterwards the
//appropriate cond_var.
    pthread_mutex_lock(jdata->snd_buff_ready_mutex);
    pthread_cond_signal(jdata->sound_data_read);
    pthread_mutex_unlock(jdata->snd_buff_ready_mutex);

    return 0;
}

/**
*   Register and Activate specified ports
*
*   \param jdata_t  Pointer to JackData struct containing port
*                   and client information
*
*   \returns 0 on Success, 1 on failure
*/
static int SetupPorts(JackData *jdata) {
    int i=0;
    jdata->ports=malloc(sizeof(jack_port_t *)*
                               jdata->nports);
    jdata->portbuf=malloc(sizeof(jack_default_audio_sample_t*)*
                          jdata->nports);
    memset(jdata->portbuf,0,sizeof(jack_default_audio_sample_t*)*
                            jdata->nports);

    for(i=0;i<jdata->nports;i++){
        char name[64];//recordMyDesktop:input_n<64 is enough for full name
        char num[8];
        strcpy(name,"input_");
        snprintf( num, 8, "%d", i+1 );
        strcat(name,num);
        if((jdata->ports[i]=jack_port_register(jdata->client,
                                                 name,
                                                 JACK_DEFAULT_AUDIO_TYPE,
                                                 JackPortIsInput,
                                                 0))==0){
            fprintf(stderr,"Cannot register input port \"%s\"!\n",name);
            return 1;
        }
        if(jack_connect(jdata->client,
                          jdata->port_names[i],
                          jack_port_name(jdata->ports[i]))){
            fprintf(stderr,"Cannot connect input port %s to %s\n",
                           jack_port_name(jdata->ports[i]),
                           jdata->port_names[i]);
            return 1;
        }
    }
    return 0;
}

//in case the jack server shuts down
//the program should stop recording,
//encode the result(if not on the fly)
//an exit cleanly.
static void JackShutdown(void *jdata_t) {
    JackData *jdata = (JackData *)jdata_t;

    jdata->pdata->running = FALSE;

    fprintf (stderr, "JACK shutdown\n");
}

int StartJackClient(JackData *jdata){
    float ring_buffer_size=0.0;
    int pid;
    char pidbuf[8];
    char rmd_client_name[32];

    //construct the jack client name
    //which is recordMyDesktop-pid
    //in order to allow multiple
    //instances of recordMyDesktop
    //to connetc to a Jack Server
    strcpy(rmd_client_name,"recordMyDesktop-");
    pid=getpid();
    snprintf( pidbuf, 8, "%d", pid );
    strcat(rmd_client_name,pidbuf);

    if ((jdata->client=(*jack_client_new)(rmd_client_name))==0){
        fprintf(stderr,"Could not create new client!\n"
                       "Make sure that Jack server is running!\n");
        return 15;
    }
//in contrast to ALSA and OSS, Jack dictates frequency
//and buffersize to the values it was launched with.
//Supposedly jack_set_buffer_size can set the buffersize
//but that causes some kind of halt and keeps giving me
//zero buffers.
//recordMyDesktop cannot handle buffer size changes.
//FIXME
//There is a callback for buffer size changes that I should use.
//It will provide clean exits, instead of ?segfaults? .
//Continuing though is not possible, with the current layout
//(it might be in some cases, but it will certainly be the cause
//of unpredicted problems). A clean exit is preferable
//and any recording up to that point will be encoded and saved.
    jdata->frequency=jack_get_sample_rate(jdata->client);
    jdata->buffersize=jack_get_buffer_size(jdata->client);
    ring_buffer_size=(jdata->ringbuffer_secs*
                      jdata->frequency*
                      sizeof(jack_default_audio_sample_t)*
                      jdata->nports);
    jdata->sound_buffer=
        (*jack_ringbuffer_create)((int)(ring_buffer_size+0.5));//round up
    jack_set_process_callback(jdata->client,JackCapture,jdata);
    jack_on_shutdown(jdata->client,JackShutdown,jdata);

    if (jack_activate(jdata->client)) {
        fprintf(stderr,"cannot activate client!\n");
        return 16;
    }
    if(SetupPorts(jdata)){
        jack_client_close(jdata->client);
        return 17;
    }

    return 0;
}

int StopJackClient(JackData *jdata){
    int ret=0;

    (*jack_ringbuffer_free)(jdata->sound_buffer);
    if(jack_client_close(jdata->client)){
        fprintf(stderr,"Cannot close Jack client!\n");
        ret=1;
    }

/*TODO*/
//I need to make some kind of program/thread
//flow diagram to see where it's safe to dlclose
//because here it causes a segfault.

//     if(dlclose(jdata->jack_lib_handle)){
//         fprintf(stderr,"Cannot unload Jack library!\n");
//         ret=1;
//     }
//     else fprintf(stderr,"Unloaded Jack library.\n");

    return ret;
}

#endif

