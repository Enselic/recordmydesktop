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


#include <recordmydesktop.h>

#ifdef HAVE_LIBASOUND
void FixBufferSize(snd_pcm_uframes_t *buffsize){
    snd_pcm_uframes_t buffsize_t=*buffsize,
#else
void FixBufferSize(u_int32_t *buffsize){
    u_int32_t buffsize_t=*buffsize,
#endif
                          buffsize_ret=1;
    while(buffsize_t>1){
        buffsize_t>>=1;
        buffsize_ret<<=1;
    }
    fprintf(stderr,"Buffer size adjusted to %d from %d frames.\n",
                   (int)buffsize_ret,(int)*buffsize);
}

int InitializeData(ProgData *pdata,
                   EncData *enc_data,
                   CacheData *cache_data){
    int i;
    //these are globals, look for them at the header
    frames_total=frames_lost=encoder_busy=capture_busy=0;

    fprintf(stderr,"Initializing...\n");
    MakeMatrices();
    if(pdata->args.have_dummy_cursor){
        pdata->dummy_pointer=MakeDummyPointer(&pdata->specs,16,
                                              pdata->args.cursor_color,0,
                                              &pdata->npxl);
        pdata->dummy_p_size=16;
    }
    else
        pdata->dummy_p_size=0;


    pdata->rect_root=NULL;
    pthread_mutex_init(&pdata->sound_buffer_mutex,NULL);
    pthread_mutex_init(&pdata->snd_buff_ready_mutex,NULL);
    pthread_mutex_init(&pdata->img_buff_ready_mutex,NULL);
    pthread_mutex_init(&pdata->theora_lib_mutex,NULL);
    pthread_mutex_init(&pdata->vorbis_lib_mutex,NULL);
    pthread_mutex_init(&pdata->libogg_mutex,NULL);
    pthread_mutex_init(&pdata->yuv_mutex,NULL);
    pthread_mutex_init(&pause_mutex,NULL);
    pthread_mutex_init(&time_mutex,NULL);
    pthread_cond_init(&pdata->time_cond,NULL);
    pthread_cond_init(&pdata->pause_cond,NULL);
    pthread_cond_init(&pdata->image_buffer_ready,NULL);
    pthread_cond_init(&pdata->sound_buffer_ready,NULL);
    pthread_cond_init(&pdata->sound_data_read,NULL);
    pthread_cond_init(&pdata->theora_lib_clean,NULL);
    pthread_cond_init(&pdata->vorbis_lib_clean,NULL);
    pdata->th_encoding_clean=pdata->v_encoding_clean=1;
    Paused=Aborted=pdata->avd=0;
    pdata->sound_buffer=NULL;
    pdata->running=1;
    time_cond=&pdata->time_cond;
    pause_cond=&pdata->pause_cond;
    Running=&pdata->running;
    PauseStateChanged=0;

    if(!pdata->args.nosound){
        if(!pdata->args.use_jack){
            FixBufferSize(&pdata->args.buffsize);
#ifdef HAVE_LIBASOUND
            pdata->sound_handle=OpenDev( pdata->args.device,
                                        &pdata->args.channels,
                                        &pdata->args.frequency,
                                        &pdata->args.buffsize,
                                        &pdata->periodsize,
                                        &pdata->periodtime,
                                        &pdata->hard_pause);
            pdata->sound_framesize=((snd_pcm_format_width(
                                     SND_PCM_FORMAT_S16_LE))/8)*
                                     pdata->args.channels;

            if(pdata->sound_handle==NULL){
#else
            pdata->sound_handle=OpenDev(pdata->args.device,
                                        pdata->args.channels,
                                        pdata->args.frequency);
            pdata->periodtime=(1000000*pdata->args.buffsize)/
                            ((pdata->args.channels<<1)*pdata->args.frequency);
            //when using OSS periodsize serves as an alias of buffsize
            pdata->periodsize=pdata->args.buffsize;
            pdata->sound_framesize=pdata->args.channels<<1;
            if(pdata->sound_handle<0){
#endif
                fprintf(stderr,"Error while opening/configuring soundcard %s\n"
                            "Try running with the --no-sound or specify a "
                            "correct device.\n",
                            pdata->args.device);
                return 3;
            }
        }
        else{
#ifdef HAVE_JACK_H
            int jack_error=0;
            pdata->jdata->port_names=pdata->args.jack_port_names;
            pdata->jdata->nports=pdata->args.jack_nports;
            pdata->jdata->ringbuffer_secs=pdata->args.jack_ringbuffer_secs;
            pdata->jdata->snd_buff_ready_mutex=&pdata->snd_buff_ready_mutex;
            pdata->jdata->sound_data_read=&pdata->sound_data_read;
            pdata->jdata->capture_started=0;

            if((jack_error=StartJackClient(pdata->jdata))!=0)
                return jack_error;

            pdata->args.buffsize=pdata->jdata->buffersize;
            pdata->periodsize=pdata->args.buffsize;
            pdata->args.frequency=pdata->jdata->frequency;
            pdata->args.channels=pdata->jdata->nports;
            pdata->periodtime=(1000000*pdata->args.buffsize)/
                              pdata->args.frequency;
            pdata->sound_framesize=sizeof(jack_default_audio_sample_t)*
                                   pdata->jdata->nports;

#else
            fprintf(stderr,"Should not be here!\n");
            exit(-1);
#endif
        }
    }

    if(pdata->args.encOnTheFly)
        InitEncoder(pdata,enc_data,0);
    else
        InitCacheData(pdata,enc_data,cache_data);

    for(i=0;i<(pdata->enc_data->yuv.y_width*pdata->enc_data->yuv.y_height);i++)
        pdata->enc_data->yuv.y[i]=0;
    for(i=0;
        i<(pdata->enc_data->yuv.uv_width*pdata->enc_data->yuv.uv_height);
        i++){
        pdata->enc_data->yuv.v[i]=pdata->enc_data->yuv.u[i]=127;
    }

    yblocks=malloc(sizeof(u_int32_t)*(pdata->enc_data->yuv.y_width/Y_UNIT_WIDTH)*
                (pdata->enc_data->yuv.y_height/Y_UNIT_WIDTH));
    ublocks=malloc(sizeof(u_int32_t)*(pdata->enc_data->yuv.y_width/Y_UNIT_WIDTH)*
                (pdata->enc_data->yuv.y_height/Y_UNIT_WIDTH));
    vblocks=malloc(sizeof(u_int32_t)*(pdata->enc_data->yuv.y_width/Y_UNIT_WIDTH)*
                (pdata->enc_data->yuv.y_height/Y_UNIT_WIDTH));

    pdata->frametime=(1000000)/pdata->args.fps;
    return 0;

}
