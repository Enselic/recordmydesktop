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


int main(int argc,char **argv){
    ProgData pdata;
    int exit_status=0;
    if(XInitThreads ()==0){
        fprintf(stderr,"Couldn't initialize thread support!\n");
        exit(7);
    }
    DEFAULT_ARGS(&pdata.args);
    if(ParseArgs(argc,argv,&pdata.args)){
        exit(1);
    }
    if(pdata.args.display!=NULL)
        pdata.dpy = XOpenDisplay(pdata.args.display);
    else{
        fprintf(stderr,"No display specified for connection!\n");
        exit(8);
    }
    if (pdata.dpy == NULL) {
        fprintf(stderr, "Cannot connect to X server %s\n",pdata.args.display);
        exit(9);
    }
    else{
        EncData enc_data;
        CacheData cache_data;
        pthread_t   poll_damage_t,
                    image_capture_t,
                    image_encode_t,
                    image_cache_t,
                    sound_capture_t,
                    sound_encode_t,
                    flush_to_ogg_t;
        XShmSegmentInfo shminfo;
        int i;

        QUERY_DISPLAY_SPECS(pdata.dpy,&pdata.specs);
        if(pdata.specs.depth!=24){
            fprintf(stderr,"Only 24bpp color depth mode is currently supported.\n");
            exit(10);
        }
        if(SetBRWindow(pdata.dpy,&pdata.brwin,&pdata.specs,&pdata.args))
            exit(11);
        
        QueryExtensions(pdata.dpy,&pdata.args,&pdata.damage_event, &pdata.damage_error);

        //init data


        //these are globals, look for them at the header
        frames_total=frames_lost=encoder_busy=capture_busy=0;

        fprintf(stderr,"Initializing...\n");
        MakeMatrices();
        if(pdata.args.have_dummy_cursor){
            pdata.dummy_pointer=MakeDummyPointer(&pdata.specs,16,pdata.args.cursor_color,0,&pdata.npxl);
            pdata.dummy_p_size=16;
        }

        if((pdata.args.noshared))
            pdata.datamain=(char *)malloc(pdata.brwin.nbytes);
        
        if(pdata.args.noshared)
            pdata.datatemp=(char *)malloc(pdata.brwin.nbytes);
        pdata.rect_root[0]=pdata.rect_root[1]=NULL;
        pthread_mutex_init(&pdata.list_mutex[0],NULL);
        pthread_mutex_init(&pdata.list_mutex[1],NULL);
        pthread_mutex_init(&pdata.sound_buffer_mutex,NULL);
        pthread_mutex_init(&pdata.libogg_mutex,NULL);
        pthread_mutex_init(&pdata.yuv_mutex,NULL);

        pthread_cond_init(&pdata.time_cond,NULL);
        pthread_cond_init(&pdata.pause_cond,NULL);
        pthread_cond_init(&pdata.image_buffer_ready,NULL);
        pthread_cond_init(&pdata.sound_buffer_ready,NULL);
        pthread_cond_init(&pdata.sound_data_read,NULL);
        pdata.list_selector=Paused=Aborted=pdata.avd=0;
        pdata.running=1;
        time_cond=&pdata.time_cond;
        pause_cond=&pdata.pause_cond;
        Running=&pdata.running;
    
        if((pdata.args.noshared)){
            pdata.image=XCreateImage(pdata.dpy, pdata.specs.visual, pdata.specs.depth, ZPixmap, 0,pdata.datamain,pdata.brwin.rgeom.width,
                        pdata.brwin.rgeom.height, 8, 0);
            XInitImage(pdata.image);
            GetZPixmap(pdata.dpy,pdata.specs.root,pdata.image->data,pdata.brwin.rgeom.x,pdata.brwin.rgeom.y,
                        pdata.brwin.rgeom.width,pdata.brwin.rgeom.height);
        }
        if((!pdata.args.noshared)||(!pdata.args.nocondshared)){
            pdata.shimage=XShmCreateImage (pdata.dpy,pdata.specs.visual,pdata.specs.depth,ZPixmap,pdata.datash, 
                         &shminfo, pdata.brwin.rgeom.width,pdata.brwin.rgeom.height);
            shminfo.shmid = shmget (IPC_PRIVATE,
                                    pdata.shimage->bytes_per_line * pdata.shimage->height,
                                    IPC_CREAT|0777);
            shminfo.shmaddr = pdata.shimage->data = shmat (shminfo.shmid, 0, 0);
            shminfo.readOnly = False;
            if(!XShmAttach(pdata.dpy,&shminfo)){
                fprintf(stderr,"Failed to attach shared memory to proccess.\n");
                exit(12);
            }
            XShmGetImage(pdata.dpy,pdata.specs.root,pdata.shimage,pdata.brwin.rgeom.x,pdata.brwin.rgeom.y,AllPlanes);
        }
        if(!pdata.args.nosound){
            pdata.sound_handle=OpenDev(pdata.args.device,&pdata.args.channels,&pdata.args.frequency,&pdata.periodsize,            &pdata.periodtime,&pdata.hard_pause);
            if(pdata.sound_handle==NULL){
                fprintf(stderr,"Error while opening/configuring soundcard %s\nTry running with the --nosound or specify a correct device.\n",pdata.args.device);
                exit(3);
            }
        }
        
        if(pdata.args.encOnTheFly)
            InitEncoder(&pdata,&enc_data,0);
        else
            InitCacheData(&pdata,&enc_data,&cache_data);

        for(i=0;i<(pdata.enc_data->yuv.y_width*pdata.enc_data->yuv.y_height);i++)
            pdata.enc_data->yuv.y[i]=0;
        for(i=0;i<(pdata.enc_data->yuv.uv_width*pdata.enc_data->yuv.uv_height);i++){
            pdata.enc_data->yuv.v[i]=pdata.enc_data->yuv.u[i]=127;
        }

        if((pdata.args.nocondshared)&&(!pdata.args.noshared)){
            if(pdata.args.no_quick_subsample){
                UPDATE_YUV_BUFFER_IM_AVG((&pdata.enc_data->yuv),((unsigned char*)pdata.shimage->data),
                (pdata.enc_data->x_offset),(pdata.enc_data->y_offset),
                (pdata.brwin.rgeom.width),(pdata.brwin.rgeom.height));
            }
            else{
                UPDATE_YUV_BUFFER_IM((&pdata.enc_data->yuv),((unsigned char*)pdata.shimage->data),
                (pdata.enc_data->x_offset),(pdata.enc_data->y_offset),
                (pdata.brwin.rgeom.width),(pdata.brwin.rgeom.height));
            }
        }
        else{
            if(pdata.args.no_quick_subsample){
                UPDATE_YUV_BUFFER_IM_AVG((&pdata.enc_data->yuv),((unsigned char*)pdata.image->data),
                (pdata.enc_data->x_offset),(pdata.enc_data->y_offset),
                (pdata.brwin.rgeom.width),(pdata.brwin.rgeom.height));
            }
            else{
                UPDATE_YUV_BUFFER_IM((&pdata.enc_data->yuv),((unsigned char*)pdata.image->data),
                (pdata.enc_data->x_offset),(pdata.enc_data->y_offset),
                (pdata.brwin.rgeom.width),(pdata.brwin.rgeom.height));
            }
        }

        pdata.frametime=(1000000)/pdata.args.fps;

        if(pdata.args.delay>0){
            fprintf(stderr,"Will sleep for %d seconds now.\n",pdata.args.delay);
            sleep(pdata.args.delay);
        }

        /*start threads*/
        if(!pdata.args.full_shots)
            pthread_create(&poll_damage_t,NULL,PollDamage,(void *)&pdata);
        pthread_create(&image_capture_t,NULL,GetFrame,(void *)&pdata);
//         pthread_create(&image_encode_t,NULL,EncodeImageBuffer,(void *)&pdata);
        pthread_create(&image_cache_t,NULL,CacheImageBuffer,(void *)&pdata);

        if(!pdata.args.nosound){
            pthread_create(&sound_capture_t,NULL,CaptureSound,(void *)&pdata);
//             pthread_create(&sound_encode_t,NULL,EncodeSoundBuffer,(void *)&pdata);
        }
//         pthread_create(&flush_to_ogg_t,NULL,FlushToOgg,(void *)&pdata);
        

        RegisterCallbacks(&pdata.args);
        fprintf(stderr,"Capturing!\n");

        //wait all threads to finish
        
        pthread_join(image_capture_t,NULL);
        fprintf(stderr,"Shutting down.");
//         pthread_join(image_encode_t,NULL);
        fprintf(stderr,".");
        if(!pdata.args.nosound){
            int *snd_exit;
            pthread_join(sound_capture_t,(void *)(&snd_exit));
            fprintf(stderr,".");
//             if(!(*snd_exit))
//                 pthread_join(sound_encode_t,NULL);
//             else{
//                 pthread_cancel(sound_encode_t);
//                 exit_status=*snd_exit;
//             }
            fprintf(stderr,".");
        }
        else
            fprintf(stderr,"..");
//         pthread_join(flush_to_ogg_t,NULL);
        fprintf(stderr,".");
        if(!pdata.args.full_shots)
            pthread_join(poll_damage_t,NULL);
        fprintf(stderr,".");
        if((!pdata.args.noshared)||(!pdata.args.nocondshared)){
            XShmDetach (pdata.dpy, &shminfo);
            shmdt (&shminfo.shmaddr);
            shmctl (shminfo.shmid, IPC_RMID, 0);
        }
        fprintf(stderr,"\n");
        XCloseDisplay(pdata.dpy);
        if(Aborted){
            if(remove(pdata.args.filename)){
                perror("Error while removing file:\n");
                return 1;
            }
            else{
                fprintf(stderr,"SIGABRT received,file %s removed\n",pdata.args.filename);
                return 0;
            }
        }
        else
            fprintf(stderr,"Goodbye!\n");
    }
    return exit_status;
}















