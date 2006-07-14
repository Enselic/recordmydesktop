#include <recordmydesktop.h>

void *EncodeImageBuffer(void *pdata){
    pthread_mutex_t pmut,imut;
    pthread_mutex_init(&pmut,NULL);
    pthread_mutex_init(&imut,NULL);

    while(((ProgData *)pdata)->running){
        pthread_cond_wait(&((ProgData *)pdata)->image_buffer_ready,&imut);
        if(Paused)
            pthread_cond_wait(&((ProgData *)pdata)->pause_cond,&pmut);//this may not be needed
        pthread_mutex_lock(&((ProgData *)pdata)->yuv_mutex);
        int za=theora_encode_YUVin(&((ProgData *)pdata)->enc_data->m_th_st,&((ProgData *)pdata)->enc_data->yuv);
        if(za){
            fprintf(stderr,"Encoder not ready %d!\n",za);
        }
        pthread_mutex_unlock(&((ProgData *)pdata)->yuv_mutex);
        theora_encode_packetout(&((ProgData *)pdata)->enc_data->m_th_st,0,&((ProgData *)pdata)->enc_data->m_ogg_pckt1);
        ogg_stream_packetin(&((ProgData *)pdata)->enc_data->m_ogg_ts,&((ProgData *)pdata)->enc_data->m_ogg_pckt1);
        avd+=((ProgData *)pdata)->frametime*2*((ProgData *)pdata)->args.channels;
    }
    //last packet
    if(theora_encode_YUVin(&((ProgData *)pdata)->enc_data->m_th_st,&((ProgData *)pdata)->enc_data->yuv)){
            fprintf(stderr,"Encoder not ready!\n");
    }

    theora_encode_packetout(&((ProgData *)pdata)->enc_data->m_th_st,1,&((ProgData *)pdata)->enc_data->m_ogg_pckt1);

//     ogg_stream_packetin(&((ProgData *)pdata)->enc_data->m_ogg_ts,&((ProgData *)pdata)->enc_data->m_ogg_pckt);

    pthread_exit(&errno);
}


