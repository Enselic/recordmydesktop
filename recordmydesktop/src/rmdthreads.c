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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cache_audio.h"
#include "cache_frame.h"
#include "capture_sound.h"
#include "encode_image_buffer.h"
#include "encode_sound_buffer.h"
#include "flush_to_ogg.h"
#include "get_frame.h"
#include "register_callbacks.h"
#include "rmd_jack.h"
#include "rmd_timer.h"
#include "rmdthreads.h"
#include "rmdtypes.h"


void rmdThreads(ProgData *pdata){
    pthread_t   image_capture_t,
                image_encode_t,
                image_cache_t,
                sound_capture_t,
                sound_encode_t,
                sound_cache_t,
                flush_to_ogg_t,
                timer_t;

    if(pdata->args.delay>0){
        fprintf(stderr,"Will sleep for %d seconds now.\n",pdata->args.delay);
        sleep(pdata->args.delay);
    }

    /*start threads*/
    pthread_create(&image_capture_t,
                   NULL,
                   (void *)GetFrame,
                   (void *)pdata);
    if(pdata->args.encOnTheFly)
        pthread_create(&image_encode_t,
                       NULL,
                       (void *)EncodeImageBuffer,
                       (void *)pdata);
    else
        pthread_create(&image_cache_t,
                       NULL,
                       (void *)CacheImageBuffer,
                       (void *)pdata);

    if(!pdata->args.nosound){
        if(!pdata->args.use_jack)
            pthread_create(&sound_capture_t,
                        NULL,
                        (void *)CaptureSound,
                        (void *)pdata);
        if(pdata->args.encOnTheFly)
            pthread_create(&sound_encode_t,
                           NULL,
                           (void *)EncodeSoundBuffer,
                           (void *)pdata);
        else
            pthread_create(&sound_cache_t,
                           NULL,
                           (void *)CacheSoundBuffer,
                           (void *)pdata);
    }
    if(pdata->args.encOnTheFly)
        pthread_create(&flush_to_ogg_t,
                       NULL,
                       (void *)FlushToOgg,
                       (void *)pdata);

    RegisterCallbacks(pdata);
    pdata->timer_alive=1;
    pthread_create(&timer_t,
                   NULL,
                   (void *)rmdTimer,
                   (void *)pdata);
    fprintf(stderr,"Capturing!\n");
#ifdef HAVE_JACK_H
    if(pdata->args.use_jack){
        pdata->jdata->capture_started=1;
    }
#endif
    //wait all threads to finish

    pthread_join(image_capture_t,NULL);
    fprintf(stderr,"Shutting down.");
    //if no damage events have been received the thread will get stuck
    while(!pdata->th_enc_thread_waiting && !pdata->th_encoding_clean){
        usleep(10000);
        pthread_mutex_lock(&pdata->img_buff_ready_mutex);
        pthread_cond_signal(&pdata->image_buffer_ready);
        pthread_mutex_unlock(&pdata->img_buff_ready_mutex);
    }

    if(pdata->args.encOnTheFly){
        pthread_join(image_encode_t,NULL);
    }
    else
        pthread_join(image_cache_t,NULL);
    fprintf(stderr,".");

    if(!pdata->args.nosound){
#ifdef HAVE_JACK_H
        if(pdata->args.use_jack)
            StopJackClient(pdata->jdata);
#endif
        if(!pdata->args.use_jack)
            pthread_join(sound_capture_t,NULL);
        fprintf(stderr,".");
        while(pdata->v_enc_thread_waiting || !pdata->v_encoding_clean){
            usleep(10000);
            pthread_mutex_lock(&pdata->snd_buff_ready_mutex);
            pthread_cond_signal(&pdata->sound_data_read);
            pthread_mutex_unlock(&pdata->snd_buff_ready_mutex);
        }
        if(pdata->args.encOnTheFly){
            pthread_join(sound_encode_t,NULL);
        }
        else{
            pthread_join(sound_cache_t,NULL);
        }
    }
    else
        fprintf(stderr,"..");

    if(pdata->args.encOnTheFly)
        pthread_join(flush_to_ogg_t,NULL);
    fprintf(stderr,".");

    //Now that we are done with recording we cancel the timer
    pdata->timer_alive=0;
    pthread_join(timer_t,NULL);


}
