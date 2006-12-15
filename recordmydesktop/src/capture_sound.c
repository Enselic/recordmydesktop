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

void *CaptureSound(ProgData *pdata){

    int frames=pdata->periodsize;
    int framesize=((snd_pcm_format_width(SND_PCM_FORMAT_S16_LE))/8)*pdata->args.channels;
    pthread_mutex_t pmut;
    pthread_mutex_init(&pmut,NULL);

    //start capturing only after first frame is taken
    usleep(pdata->frametime);

    while(pdata->running){
        int sret=0;
        SndBuffer *newbuf,*tmp;
        if(Paused){
            if(!pdata->hard_pause){
                snd_pcm_pause(pdata->sound_handle,1);
                pthread_cond_wait(&pdata->pause_cond,&pmut);
                snd_pcm_pause(pdata->sound_handle,0);
            }
            else{//device doesn't support pause(is this the norm?mine doesn't)
                snd_pcm_close(pdata->sound_handle);
                pthread_cond_wait(&pdata->pause_cond,&pmut);

                pdata->sound_handle=
                    OpenDev(pdata->args.device,
                            &pdata->args.channels,
                            &pdata->args.frequency,
                            &pdata->args.buffsize,
                            NULL,
                            NULL,
                            NULL//let's hope that the device capabilities didn't magically change
                            );
                if(pdata->sound_handle==NULL){
                    fprintf(stderr,"Couldn't reopen sound device.Exiting\n");
                    pdata->running=0;
                    errno=3;
                    pthread_exit(&errno);
                }
            }
        }

        //create new buffer
        newbuf=(SndBuffer *)malloc(sizeof(SndBuffer *));
        newbuf->data=(signed char *)malloc(frames*framesize);
        newbuf->next=NULL;

        //read data into new buffer
        while(sret<frames){
            int temp_sret=snd_pcm_readi(pdata->sound_handle,
                                newbuf->data+framesize*sret,
                                frames-sret);
            if(temp_sret==-EPIPE){
                fprintf(stderr,"%s: Underrun occurred.\n",snd_strerror(temp_sret));
                snd_pcm_prepare(pdata->sound_handle);
            }
            else if (temp_sret<0){
                fprintf(stderr,"An error occured while reading sound data:\n %s\n",snd_strerror(temp_sret));
                snd_pcm_prepare(pdata->sound_handle);
            }
            else
                sret+=temp_sret;
        }

        //queue the new buffer
        pthread_mutex_lock(&pdata->sound_buffer_mutex);
        tmp=pdata->sound_buffer;
        if(pdata->sound_buffer==NULL)
                pdata->sound_buffer=newbuf;
        else{
            while(tmp->next!=NULL)
                tmp=tmp->next;
            tmp->next=newbuf;
        }
        pthread_mutex_unlock(&pdata->sound_buffer_mutex);


        //signal that there are data to be proccessed
        pthread_cond_signal(&pdata->sound_data_read);
    }
    snd_pcm_close(pdata->sound_handle);
    pthread_exit(&errno);
}



