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

#include <recordmydesktop.h>
void *EncodeImageBuffer(void *pdata){
    pthread_mutex_t pmut,imut;
    pthread_mutex_init(&pmut,NULL);
    pthread_mutex_init(&imut,NULL);
    while(((ProgData *)pdata)->running){
        pthread_cond_wait(&((ProgData *)pdata)->image_buffer_ready,&imut);
        encoder_busy=1;
        if(Paused)
            pthread_cond_wait(&((ProgData *)pdata)->pause_cond,&pmut);//this may not be needed
        pthread_mutex_lock(&((ProgData *)pdata)->yuv_mutex);
        if(theora_encode_YUVin(&((ProgData *)pdata)->enc_data->m_th_st,&((ProgData *)pdata)->enc_data->yuv)){
            fprintf(stderr,"Encoder not ready!\n");
            pthread_mutex_unlock(&((ProgData *)pdata)->yuv_mutex);
        }
        else{
            pthread_mutex_unlock(&((ProgData *)pdata)->yuv_mutex);
            if(theora_encode_packetout(&((ProgData *)pdata)->enc_data->m_th_st,0,&((ProgData *)pdata)->enc_data->m_ogg_pckt1)==1){
                pthread_mutex_lock(&((ProgData *)pdata)->libogg_mutex);
                ogg_stream_packetin(&((ProgData *)pdata)->enc_data->m_ogg_ts,&((ProgData *)pdata)->enc_data->m_ogg_pckt1);
                pthread_mutex_unlock(&((ProgData *)pdata)->libogg_mutex);
                ((ProgData *)pdata)->avd+=((ProgData *)pdata)->frametime*2*((ProgData *)pdata)->args.channels;
            }
        }
        encoder_busy=0;
    }
    //last packet
    if(theora_encode_YUVin(&((ProgData *)pdata)->enc_data->m_th_st,&((ProgData *)pdata)->enc_data->yuv)){
            fprintf(stderr,"Encoder not ready!\n");
    }

    theora_encode_packetout(&((ProgData *)pdata)->enc_data->m_th_st,1,&((ProgData *)pdata)->enc_data->m_ogg_pckt1);

//     ogg_stream_packetin(&((ProgData *)pdata)->enc_data->m_ogg_ts,&((ProgData *)pdata)->enc_data->m_ogg_pckt);

    pthread_exit(&errno);
}

//this function is meant to be called normally
//not through a thread of it's own
void SyncEncodeImageBuffer(ProgData *pdata){
    if(theora_encode_YUVin(&pdata->enc_data->m_th_st,
                            &pdata->enc_data->yuv)){
        fprintf(stderr,"Encoder not ready!\n");
    }
    else{
        if(theora_encode_packetout(&pdata->enc_data->m_th_st,0,
                                    &pdata->enc_data->m_ogg_pckt1)==1){
            pthread_mutex_lock(&pdata->libogg_mutex);
            ogg_stream_packetin(&pdata->enc_data->m_ogg_ts,
                                &pdata->enc_data->m_ogg_pckt1);
            pthread_mutex_unlock(&pdata->libogg_mutex);
            pdata->avd+=pdata->frametime*2*pdata->args.channels;
        }
    }
}

