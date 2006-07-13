#include <recordmydesktop.h>

void *FlushToOgg(void *pdata){
    int videoflag=0,audioflag=0;
    double video_bytesout=0,audio_bytesout=0;
    ogg_page videopage,audiopage;
    int prev=0;
    while(((ProgData *)pdata)->running){
//         if(Paused)pthread_cond_wait(&((ProgData *)pdata)->pause_cond,&((ProgData *)pdata)->pause_cond_mutex);
//         if(!prev){
            videoflag=ogg_stream_pageout(&((ProgData *)pdata)->enc_data->m_ogg_ts,&videopage);
            if(videoflag){
                video_bytesout+=fwrite(videopage.header,1,videopage.header_len,((ProgData *)pdata)->enc_data->fp);
                video_bytesout+=fwrite(videopage.body,1,videopage.body_len,((ProgData *)pdata)->enc_data->fp);
                videoflag=0;
//                 prev=(!((ProgData *)pdata)->args.nosound);

            
//         }
        if(!((ProgData *)pdata)->args.nosound){
        
            audioflag=ogg_stream_pageout(&((ProgData *)pdata)->enc_data->m_ogg_vs,&audiopage);

            if(audioflag){
                audio_bytesout+=fwrite(audiopage.header,1,audiopage.header_len,((ProgData *)pdata)->enc_data->fp);
                audio_bytesout+=fwrite(audiopage.body,1,audiopage.body_len,((ProgData *)pdata)->enc_data->fp);
                audioflag=0;
                prev=0;
            }
        }
//         else
//             if(!did_one)
//                 usleep(10000);
            }
            else /*if(((ProgData *)pdata)->args.nosound)*/
                usleep(10000);
      
    }
    //last packages
    videoflag=ogg_stream_flush(&((ProgData *)pdata)->enc_data->m_ogg_ts,&videopage);
    if(videoflag){
        video_bytesout+=fwrite(videopage.header,1,videopage.header_len,((ProgData *)pdata)->enc_data->fp);
        video_bytesout+=fwrite(videopage.body,1,videopage.body_len,((ProgData *)pdata)->enc_data->fp);
        videoflag=0;
    }
    if(!((ProgData *)pdata)->args.nosound)
        audioflag=ogg_stream_flush(&((ProgData *)pdata)->enc_data->m_ogg_vs,&audiopage);
    if(audioflag){
        audio_bytesout+=fwrite(audiopage.header,1,audiopage.header_len,((ProgData *)pdata)->enc_data->fp);
        audio_bytesout+=fwrite(audiopage.body,1,audiopage.body_len,((ProgData *)pdata)->enc_data->fp);
        audioflag=0;
    }

    ogg_stream_clear(&((ProgData *)pdata)->enc_data->m_ogg_ts);
    if(!((ProgData *)pdata)->args.nosound)
        ogg_stream_clear(&((ProgData *)pdata)->enc_data->m_ogg_vs);
//this always gives me a segfault :(
//     theora_clear(&((ProgData *)pdata)->enc_data->m_th_st);
    
    if(((ProgData *)pdata)->enc_data->fp)fclose(((ProgData *)pdata)->enc_data->fp);

    fprintf(stderr,"\r   \nDone.\nWritten %.0f bytes\n(%.0f of which were video data and %.0f audio data)\n\n",video_bytesout+audio_bytesout,video_bytesout,audio_bytesout);
    pthread_exit(&errno);
}
