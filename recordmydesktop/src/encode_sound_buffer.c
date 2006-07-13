#include <recordmydesktop.h>

void *EncodeSoundBuffer(void *pdata){

    int sampread=((ProgData *)pdata)->periodsize/(2*((ProgData *)pdata)->args.channels);
    pthread_mutex_t smut;
    pthread_mutex_init(&smut,NULL);
    while((((ProgData *)pdata)->running)){
        float **vorbis_buffer;
        int count=0,i,j;
        SndBuffer *buff;

        if(Paused){
            pthread_mutex_t tmut;
            pthread_mutex_init(&tmut,NULL);
            pthread_cond_wait(&((ProgData *)pdata)->pause_cond,&tmut);
        }

        if(((ProgData *)pdata)->sound_buffer==NULL)
            pthread_cond_wait(&((ProgData *)pdata)->sound_data_read,&smut);

        pthread_mutex_lock(&((ProgData *)pdata)->sound_buffer_mutex);
        buff=((ProgData *)pdata)->sound_buffer;
        //advance the list
        ((ProgData *)pdata)->sound_buffer=((ProgData *)pdata)->sound_buffer->next;
        pthread_mutex_unlock(&((ProgData *)pdata)->sound_buffer_mutex);
                
        if (avd>0){
            vorbis_buffer=vorbis_analysis_buffer(&((ProgData *)pdata)->enc_data->m_vo_dsp,sampread);

            for(i=0;i<sampread;i++){
                for(j=0;j<((ProgData *)pdata)->args.channels;j++){
                    vorbis_buffer[j][i]=((buff->data[count+1]<<8)|
                                            (0x00ff&(int)buff->data[count]))/32768.f;
                    count+=2;
                }
            }
            vorbis_analysis_wrote(&((ProgData *)pdata)->enc_data->m_vo_dsp,sampread);
    
            while(vorbis_analysis_blockout(&((ProgData *)pdata)->enc_data->m_vo_dsp,&((ProgData *)pdata)->enc_data->m_vo_block)==1){
                
                vorbis_analysis(&((ProgData *)pdata)->enc_data->m_vo_block,NULL);
                vorbis_bitrate_addblock(&((ProgData *)pdata)->enc_data->m_vo_block);
                
                while(vorbis_bitrate_flushpacket(&((ProgData *)pdata)->enc_data->m_vo_dsp,&((ProgData *)pdata)->enc_data->m_ogg_pckt2))
                    ogg_stream_packetin(&((ProgData *)pdata)->enc_data->m_ogg_vs,&((ProgData *)pdata)->enc_data->m_ogg_pckt2);
            }
            avd-=((ProgData *)pdata)->periodtime;
        }
        free(buff);
    }

    vorbis_analysis_wrote(&((ProgData *)pdata)->enc_data->m_vo_dsp,0);
//     while(vorbis_analysis_blockout(&((ProgData *)pdata)->enc_data->m_vo_dsp,&((ProgData *)pdata)->enc_data->m_vo_block)==1){
//         vorbis_analysis(&((ProgData *)pdata)->enc_data->m_vo_block,NULL);
//         vorbis_bitrate_addblock(&((ProgData *)pdata)->enc_data->m_vo_block);
//         while(vorbis_bitrate_flushpacket(&((ProgData *)pdata)->enc_data->m_vo_dsp,&((ProgData *)pdata)->enc_data->m_ogg_pckt2))
//             ogg_stream_packetin(&((ProgData *)pdata)->enc_data->m_ogg_vs,&((ProgData *)pdata)->enc_data->m_ogg_pckt2);
//     }

    pthread_exit(&errno);
}

 
