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

void *EncodeSoundBuffer(ProgData *pdata){
#ifdef HAVE_LIBASOUND
    int sampread=pdata->periodsize;
#else
    int sampread=pdata->args.buffsize>>1;
#endif
    pthread_mutex_t smut;
    pthread_mutex_init(&smut,NULL);
    pdata->v_encoding_clean=0;
    while((pdata->running)){
        float **vorbis_buffer;
        int count=0,i,j;
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

        vorbis_buffer=vorbis_analysis_buffer(&pdata->enc_data->m_vo_dsp,
                                             sampread);
        for(i=0;i<sampread;i++){
            for(j=0;j<pdata->args.channels;j++){
                vorbis_buffer[j][i]=((buff->data[count+1]<<8)|
                                     (0x00ff&(int)buff->data[count]))/32768.f;
                count+=2;
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

        pdata->avd-=pdata->periodtime;

        free(buff->data);
        free(buff);
    }

    pdata->v_encoding_clean=1;
    pthread_cond_signal(&pdata->vorbis_lib_clean);
    pthread_exit(&errno);
}

void SyncEncodeSoundBuffer(ProgData *pdata,signed char *buff){
    float **vorbis_buffer;
    int count=0,i,j;
#ifdef HAVE_LIBASOUND
    int sampread=(buff!=NULL)?pdata->periodsize:0;
#else
    int sampread=(buff!=NULL)?(pdata->args.buffsize>>1):0;
#endif
    vorbis_buffer=vorbis_analysis_buffer(&pdata->enc_data->m_vo_dsp,sampread);
    for(i=0;i<sampread;i++){
        for(j=0;j<pdata->args.channels;j++){
            vorbis_buffer[j][i]=((buff[count+1]<<8)|
                                    (0x00ff&(int)buff[count]))/32768.f;
            count+=2;
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

