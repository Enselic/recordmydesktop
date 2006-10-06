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


snd_pcm_t *OpenDev(const char *pcm_dev,unsigned int *channels,unsigned int *frequency,snd_pcm_uframes_t *periodsize,unsigned int *periodtime,int *hard_pause){
    
    snd_pcm_t *mhandle;
    snd_pcm_hw_params_t *hwparams;
    unsigned int periods=2;
    unsigned int exactrate = *frequency;

    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_uframes_t buffsize=4096;

    if (snd_pcm_open(&mhandle, pcm_dev, SND_PCM_STREAM_CAPTURE, 0)<0){
        fprintf(stderr, "Couldn't open PCM device %s\n", pcm_dev);
        return NULL;
    }
    if (snd_pcm_hw_params_any(mhandle, hwparams)<0){
        fprintf(stderr, "Couldn't configure PCM device.\n");
        return NULL;
    }
    if (snd_pcm_hw_params_set_access(mhandle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)<0)    {
        fprintf(stderr, "Couldn't set access.\n");
        return NULL;
    }
    if (snd_pcm_hw_params_set_format(mhandle, hwparams, SND_PCM_FORMAT_S16_LE)<0){
        fprintf(stderr, "Couldn't set format.\n");
        return NULL;
    }
    if (snd_pcm_hw_params_set_rate_near(mhandle, hwparams, &exactrate, 0)<0){
        fprintf(stderr, "Couldn't set frequency.\n");
        return NULL;
    }
    if (*frequency != exactrate){
        fprintf(stderr, "Playback frequency %dHz is not available...\nUsing %dHz instead.\n",*frequency,exactrate);
        *frequency=exactrate;
    }
    if (snd_pcm_hw_params_set_channels_near(mhandle, hwparams, channels)<0){
        fprintf(stderr, "Couldn't set channels number.\n");
        return NULL;
    }
    if(*channels>2){
        fprintf(stderr,"Channels number should be 1(mono) or 2(stereo).\n");
        return NULL;
    }
    if (snd_pcm_hw_params_set_periods_near(mhandle, hwparams, &periods,0)<0)    {
        fprintf(stderr, "Couldn't set periods.\n");
        return NULL;
    }
    buffsize=(exactrate*(*channels))/2;
    if (snd_pcm_hw_params_set_buffer_size_near(mhandle, hwparams,&buffsize)<0){
        fprintf(stderr, "Couldn't set buffer size.\n");
        return NULL;
    }
    if (snd_pcm_hw_params(mhandle, hwparams)<0){
        fprintf(stderr, "Couldn't set hardware parameters.\n");
        return NULL;
    }
    if(hard_pause!=NULL)
        if(!snd_pcm_hw_params_can_pause(hwparams)){
//             fprintf(stderr, "Current sound device doesn't seem to support pausing!\nI will attempt to close/reopen device in case you opt to pause during recording.\n");
            *hard_pause=1;
        }
    if(periodsize!=NULL)
        snd_pcm_hw_params_get_period_size(hwparams,periodsize,0);
    if(periodtime!=NULL)
        snd_pcm_hw_params_get_period_time(hwparams,periodtime,0);
    fprintf(stderr,"Recording on device %s is set to:\n%d channels at %dHz\n",pcm_dev,*channels,*frequency);
    snd_pcm_prepare(mhandle);

    return mhandle;
}


