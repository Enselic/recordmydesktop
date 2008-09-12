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




// #include "recordmydesktop.h"
#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef HAVE_LIBASOUND
    #include <alsa/asoundlib.h>
#else
    #include <sys/ioctl.h>
    #include <sys/soundcard.h>
#endif

#ifdef HAVE_LIBASOUND

snd_pcm_t *OpenDev( const char *pcm_dev,
                    unsigned int *channels,
                    unsigned int *frequency,
                    snd_pcm_uframes_t *buffsize,
                    snd_pcm_uframes_t *periodsize,
                    unsigned int *periodtime,
                    int *hard_pause){

    snd_pcm_t *mhandle;
    snd_pcm_hw_params_t *hwparams;
    unsigned int periods=2;
    unsigned int exactrate = *frequency;
    snd_pcm_hw_params_alloca(&hwparams);


    if (snd_pcm_open(&mhandle,pcm_dev,SND_PCM_STREAM_CAPTURE,SND_PCM_ASYNC)<0){
        fprintf(stderr, "Couldn't open PCM device %s\n", pcm_dev);
        return NULL;
    }
    else
        fprintf(stderr, "Opened PCM device %s\n", pcm_dev);
    if (snd_pcm_hw_params_any(mhandle, hwparams)<0){
        fprintf(stderr, "Couldn't configure PCM device.\n");
        return NULL;
    }
    if (snd_pcm_hw_params_set_access(mhandle,
                                     hwparams,
                                     SND_PCM_ACCESS_RW_INTERLEAVED)<0){
        fprintf(stderr, "Couldn't set access.\n");
        return NULL;
    }
    if (snd_pcm_hw_params_set_format(mhandle,
                                     hwparams,
                                     SND_PCM_FORMAT_S16_LE)<0){
        fprintf(stderr, "Couldn't set format.\n");
        return NULL;
    }
    if (snd_pcm_hw_params_set_rate_near(mhandle, hwparams, &exactrate, 0)<0){
        fprintf(stderr, "Couldn't set frequency.\n");
        return NULL;
    }
    if (*frequency != exactrate){
        fprintf(stderr, "Playback frequency %dHz is not available...\n"
                        "Using %dHz instead.\n",*frequency,exactrate);
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
    if (snd_pcm_hw_params_set_periods_near(mhandle,hwparams,&periods,0)<0){
        fprintf(stderr, "Couldn't set periods.\n");
        return NULL;
    }

    if (snd_pcm_hw_params_set_buffer_size_near(mhandle,hwparams,buffsize)<0){
        fprintf(stderr, "Couldn't set buffer size.\n");
        return NULL;
    }
    if (snd_pcm_hw_params(mhandle,hwparams)<0){
        fprintf(stderr, "Couldn't set hardware parameters.\n");
        return NULL;
    }
    if(hard_pause!=NULL)
        if(!snd_pcm_hw_params_can_pause(hwparams)){
            *hard_pause=1;
        }
    if(periodsize!=NULL)
        snd_pcm_hw_params_get_period_size(hwparams,periodsize,0);

    if(periodtime!=NULL)
        snd_pcm_hw_params_get_period_time(hwparams,periodtime,0);
    fprintf(stderr,"Recording on device %s is set to:\n%d channels at %dHz\n",
                   pcm_dev,*channels,*frequency);
    snd_pcm_prepare(mhandle);

    return mhandle;
}

#else

int OpenDev( const char *pcm_dev,
             unsigned int channels,
             unsigned int frequency){
    int fd ;
    fd=open(pcm_dev,O_RDONLY);

    if(fd!=-1){
        unsigned int value;

        if(ioctl(fd,SNDCTL_DSP_GETFMTS,&value)<0){
            fprintf(stderr,"Couldn't get audio format list\n");
            return -1;
        }
        if(value & AFMT_S16_LE){
            value=AFMT_S16_LE;
        }
        else if(value & AFMT_S16_BE){
            value=AFMT_S16_BE;
        }
        else{
            fprintf(stderr,"Soundcard doesn't support signed 16-bit-data\n");
            return -1;
        }
        if(ioctl(fd,SNDCTL_DSP_SETFMT,&value)<0){
            fprintf(stderr,"Couldn't set audio format\n" );
            return -1;
        }
        value = channels;
        if(ioctl(fd,SNDCTL_DSP_CHANNELS,&value)<0){
            fprintf(stderr,"Cannot set the number of channels\n" );
            return -1;
        }
        value = frequency;
        if(ioctl(fd,SNDCTL_DSP_SPEED,&value)<0){
            fprintf(stderr,"Couldn't set audio frequency\n" );
            return -1;
        }
        if(fcntl(fd,F_SETFL,fcntl(fd,F_GETFL) & ~O_NONBLOCK)<0){
            fprintf(stderr,"Couldn't set audio blocking mode\n" );
            return -1;
        }
    }
    return fd;
}
#endif
