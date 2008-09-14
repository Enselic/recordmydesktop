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

#include "rmdtypes.h"

#include "encode_sound_buffer.h"
#include "rmd_jack.h"


void *EncodeSoundBuffer(ProgData *pdata){
    int sampread=pdata->periodsize;
#ifdef HAVE_JACK_H
    void *jackbuf=NULL;
    if(pdata->args.use_jack){
        jackbuf=malloc(pdata->sound_framesize*pdata->jdata->buffersize);
    }
#endif
    pdata->v_encoding_clean=0;
    while((pdata->running)){
        float **vorbis_buffer;
        int count=0,i,j;
        SndBuffer *buff=NULL;

        if (pdata->paused) {
            pthread_mutex_lock(&pdata->pause_mutex);
            pthread_cond_wait(&pdata->pause_cond, &pdata->pause_mutex);
            pthread_mutex_unlock(&pdata->pause_mutex);
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
            if(pdata->sound_buffer==NULL || !pdata->running)
                break;
            pthread_mutex_lock(&pdata->sound_buffer_mutex);
            buff=pdata->sound_buffer;
            //advance the list
            pdata->sound_buffer=pdata->sound_buffer->next;
            pthread_mutex_unlock(&pdata->sound_buffer_mutex);

            vorbis_buffer=vorbis_analysis_buffer(&pdata->enc_data->m_vo_dsp,
                                                 sampread);

            for(i=0;i<sampread;i++){
                for(j=0;j<pdata->args.channels;j++){
                    vorbis_buffer[j][i]=((buff->data[count+1]<<8)|
                                        (0x00ff&(int)buff->data[count]))/
                                        32768.f;
                    count+=2;
                }
            }
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
                vorbis_buffer=vorbis_analysis_buffer(&pdata->enc_data->m_vo_dsp,
                                                    sampread);
                for(j=0;j<pdata->args.channels;j++){
                    for(i=0;i<sampread;i++){
                        vorbis_buffer[j][i]=((float*)jackbuf)[count];
                        count++;
                    }
                }
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
        vorbis_analysis_wrote(&pdata->enc_data->m_vo_dsp,sampread);

        pthread_mutex_lock(&pdata->libogg_mutex);
        while(vorbis_analysis_blockout(&pdata->enc_data->m_vo_dsp,
                                       &pdata->enc_data->m_vo_block)==1){

            vorbis_analysis(&pdata->enc_data->m_vo_block,NULL);
            vorbis_bitrate_addblock(&pdata->enc_data->m_vo_block);

            while(vorbis_bitrate_flushpacket(&pdata->enc_data->m_vo_dsp,
                                             &pdata->enc_data->m_ogg_pckt2)){
                ogg_stream_packetin(&pdata->enc_data->m_ogg_vs,
                                    &pdata->enc_data->m_ogg_pckt2);
            }
        }
        pthread_mutex_unlock(&pdata->libogg_mutex);

        pdata->avd-=pdata->periodtime;


    }

    pdata->v_encoding_clean=1;
    pthread_mutex_lock(&pdata->vorbis_lib_mutex);
    pthread_cond_signal(&pdata->vorbis_lib_clean);
    pthread_mutex_unlock(&pdata->vorbis_lib_mutex);
    pthread_exit(&errno);
}

void SyncEncodeSoundBuffer(ProgData *pdata,signed char *buff){
    float **vorbis_buffer;
    int count=0,i,j;
    int sampread=(buff!=NULL)?pdata->periodsize:0;

    vorbis_buffer=vorbis_analysis_buffer(&pdata->enc_data->m_vo_dsp,sampread);

    if(!pdata->args.use_jack){
        for(i=0;i<sampread;i++){
            for(j=0;j<pdata->args.channels;j++){
                vorbis_buffer[j][i]=((buff[count+1]<<8)|
                                        (0x00ff&(int)buff[count]))/
                                    32768.f;
                count+=2;
            }
        }
    }
    else{
        for(j=0;j<pdata->args.channels;j++){
            for(i=0;i<sampread;i++){
                vorbis_buffer[j][i]=((float*)buff)[count];
                count++;
            }
        }
    }

    vorbis_analysis_wrote(&pdata->enc_data->m_vo_dsp,sampread);

    pthread_mutex_lock(&pdata->libogg_mutex);
    while(vorbis_analysis_blockout(&pdata->enc_data->m_vo_dsp,
                                   &pdata->enc_data->m_vo_block)==1){

        vorbis_analysis(&pdata->enc_data->m_vo_block,NULL);
        vorbis_bitrate_addblock(&pdata->enc_data->m_vo_block);

        while(vorbis_bitrate_flushpacket(&pdata->enc_data->m_vo_dsp,
                                         &pdata->enc_data->m_ogg_pckt2)){
            ogg_stream_packetin(&pdata->enc_data->m_ogg_vs,
                                &pdata->enc_data->m_ogg_pckt2);
        }
    }
    pthread_mutex_unlock(&pdata->libogg_mutex);

    if(!pdata->running)pdata->enc_data->m_ogg_vs.e_o_s=1;

    pdata->avd-=pdata->periodtime;
}

