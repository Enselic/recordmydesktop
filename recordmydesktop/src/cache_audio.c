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


#include "recordmydesktop.h"


void *CacheSoundBuffer(ProgData *pdata){
//We are simply going to throw sound on the disk.
//It's sound is tiny compared to that of image, so
//compressing would reducethe overall size by only an
//insignificant fraction.
#ifdef HAVE_JACK_H
    void *jackbuf=NULL;
    if(pdata->args.use_jack){
        jackbuf=malloc(pdata->sound_framesize*pdata->jdata->buffersize);
    }
#endif
    while((pdata->running)){
        SndBuffer *buff=NULL;

        if(Paused){
            pthread_mutex_lock(&pause_mutex);
            pthread_cond_wait(&pdata->pause_cond,&pause_mutex);
            pthread_mutex_unlock(&pause_mutex);
        }
        if(!pdata->args.use_jack){
            if(pdata->sound_buffer==NULL){
                pdata->v_enc_thread_waiting=1;
                pthread_mutex_lock(&pdata->snd_buff_ready_mutex);
                pthread_cond_wait(&pdata->sound_data_read,
                                &pdata->snd_buff_ready_mutex);
                pthread_mutex_unlock(&pdata->snd_buff_ready_mutex);
                pdata->v_enc_thread_waiting=0;
            }
            if(pdata->sound_buffer==NULL || !pdata->running){
                break;
            }
            pthread_mutex_lock(&pdata->sound_buffer_mutex);
            buff=pdata->sound_buffer;
            //advance the list
            pdata->sound_buffer=pdata->sound_buffer->next;
            pthread_mutex_unlock(&pdata->sound_buffer_mutex);
            fwrite(buff->data,1,pdata->periodsize*pdata->sound_framesize,
                   pdata->cache_data->afp);
            free(buff->data);
            free(buff);
        }
        else{
#ifdef HAVE_JACK_H
            if((*jack_ringbuffer_read_space_p)(pdata->jdata->sound_buffer)>=
               (pdata->sound_framesize*pdata->jdata->buffersize)){
                (*jack_ringbuffer_read_p)(pdata->jdata->sound_buffer,
                                          jackbuf,
                                          (pdata->sound_framesize*
                                           pdata->jdata->buffersize));
                fwrite(jackbuf,1,(pdata->sound_framesize*
                                  pdata->jdata->buffersize),
                       pdata->cache_data->afp);
            }
            else{
                pdata->v_enc_thread_waiting=1;
                pthread_mutex_lock(&pdata->snd_buff_ready_mutex);
                pthread_cond_wait(&pdata->sound_data_read,
                                &pdata->snd_buff_ready_mutex);
                pthread_mutex_unlock(&pdata->snd_buff_ready_mutex);
                pdata->v_enc_thread_waiting=0;
                continue;
            }
#endif
        }
        pdata->avd-=pdata->periodtime;
    }

    fclose(pdata->cache_data->afp);
    pthread_exit(&errno);
}


