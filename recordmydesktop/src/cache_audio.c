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

#include <recordmydesktop.h>
void *CacheSoundBuffer(ProgData *pdata){
//We are simply going to throw sound on the disk.
//It's sound is tiny compared to that of image, so
//compressing would reducethe overall size by only an
//insignificant fraction.
    int framesize=((snd_pcm_format_width(SND_PCM_FORMAT_S16_LE))/8)*
                  pdata->args.channels;
    pthread_mutex_t smut;
    pthread_mutex_init(&smut,NULL);
    while((pdata->running)){
        SndBuffer *buff;

        if(Paused){
            pthread_mutex_t tmut;
            pthread_mutex_init(&tmut,NULL);
            pthread_cond_wait(&pdata->pause_cond,&tmut);
        }
        if(pdata->sound_buffer==NULL){
            pdata->v_enc_thread_waiting=1;
            pthread_cond_wait(&pdata->sound_data_read,&smut);
            pdata->v_enc_thread_waiting=0;
        }
        if(pdata->sound_buffer==NULL || !pdata->running)
            break;

        pthread_mutex_lock(&pdata->sound_buffer_mutex);
        buff=pdata->sound_buffer;
        //advance the list
        pdata->sound_buffer=pdata->sound_buffer->next;
        pthread_mutex_unlock(&pdata->sound_buffer_mutex);

        fwrite(buff->data,pdata->periodsize*framesize,1,
               pdata->cache_data->afp);



        pdata->avd-=pdata->periodtime;

        free(buff->data);
        free(buff);
    }

    fclose(pdata->cache_data->afp);
    pthread_exit(&errno);
}


