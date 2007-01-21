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

int InitializeData(ProgData *pdata,
                   EncData *enc_data,
                   CacheData *cache_data){
    int i;
    unsigned char *dtap=NULL;   //pointer switching among shared memory and
                                //normal buffer

    //these are globals, look for them at the header
    frames_total=frames_lost=encoder_busy=capture_busy=0;

    fprintf(stderr,"Initializing...\n");
    MakeMatrices();
    if(pdata->args.have_dummy_cursor){
        pdata->dummy_pointer=MakeDummyPointer(&pdata->specs,16,pdata->args.cursor_color,0,&pdata->npxl);
        pdata->dummy_p_size=16;
    }

    if((pdata->args.noshared))
        pdata->datamain=(char *)malloc(pdata->brwin.nbytes);

    if(pdata->args.noshared)
        pdata->datatemp=(char *)malloc(pdata->brwin.nbytes);
    pdata->rect_root[0]=pdata->rect_root[1]=NULL;
    pthread_mutex_init(&pdata->list_mutex[0],NULL);
    pthread_mutex_init(&pdata->list_mutex[1],NULL);
    pthread_mutex_init(&pdata->sound_buffer_mutex,NULL);
    pthread_mutex_init(&pdata->libogg_mutex,NULL);
    pthread_mutex_init(&pdata->yuv_mutex,NULL);

    pthread_cond_init(&pdata->time_cond,NULL);
    pthread_cond_init(&pdata->pause_cond,NULL);
    pthread_cond_init(&pdata->image_buffer_ready,NULL);
    pthread_cond_init(&pdata->sound_buffer_ready,NULL);
    pthread_cond_init(&pdata->sound_data_read,NULL);
    pthread_cond_init(&pdata->theora_lib_clean,NULL);
    pthread_cond_init(&pdata->vorbis_lib_clean,NULL);
    pdata->th_encoding_clean=pdata->v_encoding_clean=1;
    pdata->list_selector=Paused=Aborted=pdata->avd=0;
    pdata->sound_buffer=NULL;
    pdata->running=1;
    time_cond=&pdata->time_cond;
    pause_cond=&pdata->pause_cond;
    Running=&pdata->running;

    if((pdata->args.noshared)){
        pdata->image=XCreateImage(pdata->dpy, pdata->specs.visual, pdata->specs.depth, ZPixmap, 0,pdata->datamain,pdata->brwin.rgeom.width,
                    pdata->brwin.rgeom.height, 8, 0);
        XInitImage(pdata->image);
        GetZPixmap(pdata->dpy,pdata->specs.root,pdata->image->data,pdata->brwin.rgeom.x,pdata->brwin.rgeom.y,
                    pdata->brwin.rgeom.width,pdata->brwin.rgeom.height);
    }
    if((!pdata->args.noshared)||(!pdata->args.nocondshared)){
        pdata->shimage=XShmCreateImage (pdata->dpy,pdata->specs.visual,pdata->specs.depth,ZPixmap,pdata->datash,
                        &pdata->shminfo, pdata->brwin.rgeom.width,pdata->brwin.rgeom.height);
        pdata->shminfo.shmid = shmget (IPC_PRIVATE,
                                pdata->shimage->bytes_per_line * pdata->shimage->height,
                                IPC_CREAT|0777);
        pdata->shminfo.shmaddr = pdata->shimage->data = shmat (pdata->shminfo.shmid, 0, 0);
        pdata->shminfo.readOnly = False;
        if(!XShmAttach(pdata->dpy,&pdata->shminfo)){
            fprintf(stderr,"Failed to attach shared memory to proccess.\n");
            return 12;
        }
        XShmGetImage(pdata->dpy,pdata->specs.root,pdata->shimage,pdata->brwin.rgeom.x,pdata->brwin.rgeom.y,AllPlanes);
    }
    if(!pdata->args.nosound){
        pdata->sound_handle=OpenDev( pdata->args.device,
                                    &pdata->args.channels,
                                    &pdata->args.frequency,
                                    &pdata->args.buffsize,
                                    &pdata->periodsize,
                                    &pdata->periodtime,
                                    &pdata->hard_pause);
        if(pdata->sound_handle==NULL){
            fprintf(stderr,"Error while opening/configuring soundcard %s\nTry running with the --no-sound or specify a correct device.\n",pdata->args.device);
            return 3;
        }
    }

    if(pdata->args.encOnTheFly)
        InitEncoder(pdata,enc_data,0);
    else
        InitCacheData(pdata,enc_data,cache_data);

    for(i=0;i<(pdata->enc_data->yuv.y_width*pdata->enc_data->yuv.y_height);i++)
        pdata->enc_data->yuv.y[i]=0;
    for(i=0;i<(pdata->enc_data->yuv.uv_width*pdata->enc_data->yuv.uv_height);i++){
        pdata->enc_data->yuv.v[i]=pdata->enc_data->yuv.u[i]=127;
    }

    dtap=(((pdata->args.nocondshared)&&(!pdata->args.noshared))?
         ((unsigned char*)pdata->shimage->data):
         ((unsigned char*)pdata->image->data));


    UPDATE_YUV_BUFFER((&pdata->enc_data->yuv),dtap,
            (pdata->enc_data->x_offset),(pdata->enc_data->y_offset),
            (pdata->brwin.rgeom.width),(pdata->brwin.rgeom.height),
            __X_IPC,(pdata->args.no_quick_subsample),
            pdata->specs.depth);

    pdata->frametime=(1000000)/pdata->args.fps;

    return 0;

}
