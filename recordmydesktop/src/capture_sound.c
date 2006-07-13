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
*    For further information contact me at biocrasher@gmail.com                  *
**********************************************************************************/


#include <recordmydesktop.h>

void *CaptureSound(void *pdata){

    int frames=((ProgData *)pdata)->periodsize>>((ProgData *)pdata)->args.channels;
//     fprintf(stderr,"fr %d  ps %d\n",frames,((ProgData *)pdata)->periodsize);fflush(stderr);
    pthread_mutex_t pmut;
    pthread_mutex_init(&pmut,NULL);
    ((ProgData *)pdata)->sound_buffer=NULL;

    while(((ProgData *)pdata)->running){
        int sret=0;
        SndBuffer *newbuf,*tmp;
        if(Paused){
            if(!((ProgData *)pdata)->hard_pause){
                snd_pcm_pause(((ProgData *)pdata)->sound_handle,1);
                pthread_cond_wait(&((ProgData *)pdata)->pause_cond,&pmut);
                snd_pcm_pause(((ProgData *)pdata)->sound_handle,0);
            }
            else{//device doesn't support pause(is this the norm?mine doesn't)
                snd_pcm_close(((ProgData *)pdata)->sound_handle);
                pthread_cond_wait(&((ProgData *)pdata)->pause_cond,&pmut);

                ((ProgData *)pdata)->sound_handle=
                    OpenDev(((ProgData *)pdata)->args.device,
                            ((ProgData *)pdata)->args.channels,
                            &((ProgData *)pdata)->args.frequency,
                            NULL,
                            NULL,
                            NULL//let's hope that the device capabilities didn't magically change
                            );
                if(((ProgData *)pdata)->sound_handle==NULL){
                    fprintf(stderr,"Couldn't reopen sound device.\nThere will be no sound data from this point on.\n");
                    pthread_exit(&errno);
                }                
            }
        }

        //create new buffer
        newbuf=(SndBuffer *)malloc(sizeof(SndBuffer *));
        newbuf->data=(signed char *)malloc(((ProgData *)pdata)->periodsize);
        newbuf->next=NULL;
       
        //read data into new buffer 
        while(sret<frames){
            int tsret=snd_pcm_readi(((ProgData *)pdata)->sound_handle,
                                newbuf->data+2*((ProgData *)pdata)->args.channels*sret,
                                frames-sret);
            if(tsret==-EPIPE)
                snd_pcm_prepare(((ProgData *)pdata)->sound_handle);
            else if (tsret<0){
                fprintf(stderr,"An error occured while reading sound data:\n %s\n",snd_strerror(sret));
            }
            else
                sret+=tsret;
        }

        //queue the new buffer
        pthread_mutex_lock(&((ProgData *)pdata)->sound_buffer_mutex);
        tmp=((ProgData *)pdata)->sound_buffer;
        if(((ProgData *)pdata)->sound_buffer==NULL)
                ((ProgData *)pdata)->sound_buffer=newbuf;
        else{
            while(tmp->next!=NULL)
                tmp=tmp->next;
            tmp->next=newbuf;
        }
        pthread_mutex_unlock(&((ProgData *)pdata)->sound_buffer_mutex);


        //signal that there are data to be proccessed
        pthread_cond_signal(&((ProgData *)pdata)->sound_data_read);
    }
    snd_pcm_close(((ProgData *)pdata)->sound_handle);
    pthread_exit(&errno);
} 



