/******************************************************************************
*                            recordMyDesktop                                  *
*******************************************************************************
*                                                                             *
*            Copyright (C) 2006,2007 John Varouhakis                          *
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


int FirstFrame(ProgData *pdata,XImage **image,XShmSegmentInfo *shminfo,
               char **pxl_data){

    if((pdata->args.noshared)){
        (*image)=XCreateImage(pdata->dpy,
                            pdata->specs.visual,
                            pdata->specs.depth,
                            ZPixmap,
                            0,
                            *pxl_data,
                            pdata->brwin.rgeom.width,
                            pdata->brwin.rgeom.height,
                            8,
                            0);
        XInitImage((*image));
        GetZPixmap(pdata->dpy,pdata->specs.root,
                   (*image)->data,
                   pdata->brwin.rgeom.x,
                   pdata->brwin.rgeom.y,
                   pdata->brwin.rgeom.width,
                   pdata->brwin.rgeom.height);
        *pxl_data=(char *)malloc(pdata->brwin.nbytes);
    }
    else{
        (*image)=XShmCreateImage(pdata->dpy,
                                     pdata->specs.visual,
                                     pdata->specs.depth,
                                     ZPixmap,
                                     *pxl_data,
                                     shminfo,
                                     pdata->brwin.rgeom.width,
                                     pdata->brwin.rgeom.height);
        (*shminfo).shmid=shmget(IPC_PRIVATE,
                                    (*image)->bytes_per_line*
                                    (*image)->height,
                                    IPC_CREAT|0777);
        if((*shminfo).shmid==-1){
            fprintf(stderr,"Failed to obtain Shared Memory segment!\n");
            return 12;
        }
        (*shminfo).shmaddr=(*image)->data=shmat((*shminfo).shmid,
                                                        NULL,0);
        (*shminfo).readOnly = False;
        if(!XShmAttach(pdata->dpy,shminfo)){
            fprintf(stderr,"Failed to attach shared memory to proccess.\n");
            return 12;
        }
        XShmGetImage(pdata->dpy,
                     pdata->specs.root,
                     (*image),
                     pdata->brwin.rgeom.x,
                     pdata->brwin.rgeom.y,
                     AllPlanes);
    }

    UPDATE_YUV_BUFFER((&pdata->enc_data->yuv),
            ((unsigned char*)((*image))->data),NULL,
            (pdata->enc_data->x_offset),(pdata->enc_data->y_offset),
            (pdata->brwin.rgeom.width),(pdata->brwin.rgeom.height),
            (pdata->args.no_quick_subsample),
            pdata->specs.depth);

    return 0;
}

//make a deep copy
void BRWinCpy(BRWindow *target,BRWindow *source){

    target->geom.x=source->geom.x;
    target->geom.y=source->geom.y;
    target->geom.width=source->geom.width;
    target->geom.height=source->geom.height;
    target->rgeom.x=source->rgeom.x;
    target->rgeom.y=source->rgeom.y;
    target->rgeom.width=source->rgeom.width;
    target->rgeom.height=source->rgeom.height;
    target->nbytes=source->nbytes;
    target->windowid=source->windowid;

}

void MoveCaptureArea(   BRWindow *brwin,
                        int cursor_x,
                        int cursor_y,
                        int width,
                        int height){
    int t_x=0,t_y=0;

    t_x=cursor_x-brwin->rgeom.width/2;
    t_x=(t_x>>1)<<1;
    brwin->rgeom.x=(t_x<0)?0:((t_x+brwin->rgeom.width>width)?
                               width-brwin->rgeom.width:t_x);
    t_y=cursor_y-brwin->rgeom.height/2;
    t_y=(t_y>>1)<<1;
    brwin->rgeom.y=(t_y<0)?0:((t_y+brwin->rgeom.height>height)?
                               height-brwin->rgeom.height:t_y);
}

void *GetFrame(ProgData *pdata){
    int i=0,
        tlist_sel=0,
        blocknum_x=pdata->enc_data->yuv.y_width/Y_UNIT_WIDTH,
        blocknum_y=pdata->enc_data->yuv.y_height/Y_UNIT_WIDTH;
    unsigned int msk_ret;
    WGeometry mouse_pos_abs,mouse_pos_rel,mouse_pos_temp;
    BRWindow temp_brwin;
    Window root_ret,
           child_ret,
           shaped_w=None; //Frame
    XFixesCursorImage *xcim=NULL;
    XImage *image=NULL,*image_back=NULL;          //the image that holds
                                        //the current full screenshot
    XShmSegmentInfo shminfo,shminfo_back;//info structure for the image above.
    int init_img1=0,init_img2=0,
        img_sel,d_buff;
    char *pxl_data=NULL,*pxl_data_back=NULL;




    img_sel=d_buff=pdata->args.full_shots;

    if((init_img1=FirstFrame(pdata,&image,&shminfo,&pxl_data)!=0)){
        if(pdata->args.encOnTheFly){
            if(remove(pdata->args.filename)){
                perror("Error while removing file:\n");
            }
            else{
                fprintf(stderr,"SIGABRT received,file %s removed\n",
                                pdata->args.filename);
            }
        }
        else{
            PurgeCache(pdata->cache_data,!pdata->args.nosound);
        }
        exit(init_img1);
    }
    if(d_buff){
        if((init_img2=FirstFrame(pdata,&image_back,&shminfo_back,
                                 &pxl_data_back)!=0)){
            if(pdata->args.encOnTheFly){
                if(remove(pdata->args.filename)){
                    perror("Error while removing file:\n");
                }
                else{
                    fprintf(stderr,"SIGABRT received,file %s removed\n",
                                    pdata->args.filename);
                }
            }
            else{
                PurgeCache(pdata->cache_data,!pdata->args.nosound);
            }
            exit(init_img2);
        }

    }

    if(!pdata->args.noframe){
        shaped_w=rmdFrameInit(pdata->dpy,
                              pdata->specs.screen,
                              pdata->specs.root,
                              pdata->brwin.rgeom.x,
                              pdata->brwin.rgeom.y,
                              pdata->brwin.rgeom.width,
                              pdata->brwin.rgeom.height);
    }

    mouse_pos_abs.x=mouse_pos_temp.x=0;
    mouse_pos_abs.y=mouse_pos_temp.y=0;
    mouse_pos_abs.width=mouse_pos_temp.width=pdata->dummy_p_size;
    mouse_pos_abs.height=mouse_pos_temp.height=pdata->dummy_p_size;

    while(pdata->running){

        //if we are left behind we must not wait.
        //also before actually pausing we must make sure the streams
        //are synced. sound stops so this should only happen quickly.
        if(pdata->avd>0 || pdata->args.nosound){
            pthread_mutex_lock(&time_mutex);
            pthread_cond_wait(&pdata->time_cond,&time_mutex);
            pthread_mutex_unlock(&time_mutex);
            if(Paused){
                pthread_mutex_lock(&pause_mutex);
                pthread_cond_wait(&pdata->pause_cond,&pause_mutex);
                pthread_mutex_unlock(&pause_mutex);
            }
        }
        if(d_buff)
            img_sel=(img_sel)?0:1;
        capture_busy=1;

        BRWinCpy(&temp_brwin,&pdata->brwin);
        //mutexes and lists with changes are useless when full_shots is enabled
        if(!pdata->args.full_shots){
            tlist_sel=pdata->list_selector;
            pdata->list_selector=((pdata->list_selector+1)%2);
            pthread_mutex_lock(&pdata->list_mutex[tlist_sel]);
        }
        if(pdata->args.xfixes_cursor ||
           pdata->args.have_dummy_cursor||
           pdata->args.follow_mouse){
            //pointer sequence
            //update previous_position
            //(if full_shots is enabled the new cursor is
            //entered on the list for update.
            //When taking full shots we keep it for further
            //bellow, to mark the area as dirty when dbuffering.
            CLIP_DUMMY_POINTER_AREA(mouse_pos_abs,&temp_brwin,
                                    &mouse_pos_temp);
            if(!pdata->args.full_shots){
                if((mouse_pos_temp.x>=0)&&
                    (mouse_pos_temp.y>=0)&&
                    (mouse_pos_temp.width>0)&&
                    (mouse_pos_temp.height>0))
                    RectInsert(&pdata->rect_root[tlist_sel],&mouse_pos_temp);
            }
            //find new one
            if(pdata->args.xfixes_cursor){
                xcim=XFixesGetCursorImage(pdata->dpy);
                mouse_pos_abs.x=xcim->x-xcim->xhot;
                mouse_pos_abs.y=xcim->y-xcim->yhot;
                mouse_pos_abs.width=xcim->width;
                mouse_pos_abs.height=xcim->height;
            }
            else{
                XQueryPointer(pdata->dpy,
                              pdata->specs.root,
                              &root_ret,&child_ret,
                              &mouse_pos_abs.x,&mouse_pos_abs.y,
                              &mouse_pos_rel.x,&mouse_pos_rel.y,&msk_ret);
            }
        }
        if(pdata->args.follow_mouse){
            MoveCaptureArea(&pdata->brwin,
                            mouse_pos_abs.x+
                            ((pdata->args.xfixes_cursor)?xcim->xhot:0),
                            mouse_pos_abs.y+
                            ((pdata->args.xfixes_cursor)?xcim->yhot:0),
                            pdata->specs.width,
                            pdata->specs.height);
            if(!pdata->args.noframe){
                rmdMoveFrame(pdata->dpy,
                             shaped_w,
                             temp_brwin.rgeom.x,
                             temp_brwin.rgeom.y);

                rmdDrawFrame(pdata->dpy,
                             pdata->specs.screen,  
                             shaped_w,
                             pdata->brwin.rgeom.width,
                             pdata->brwin.rgeom.height);
            }
        }

        if(!pdata->args.full_shots){
            pthread_mutex_lock(&pdata->yuv_mutex);
            UpdateImage(pdata->dpy,
                        &pdata->enc_data->yuv,
                        &pdata->specs,
                        &pdata->rect_root[tlist_sel],
                        &temp_brwin,
                        pdata->enc_data,
                        image->data,
                        pdata->args.noshared,
                        &shminfo,
                        pdata->shm_opcode,
                        pdata->args.no_quick_subsample);
            BlocksFromList(&pdata->rect_root[tlist_sel],
                           temp_brwin.rgeom.x,
                           temp_brwin.rgeom.y,
                           pdata->enc_data->yuv.y_width/Y_UNIT_WIDTH,
                           pdata->enc_data->yuv.y_height/Y_UNIT_WIDTH);
            pthread_mutex_unlock(&pdata->yuv_mutex);
        }
        else{
            unsigned char *front_buff=(!img_sel)?((unsigned char*)image->data):
                                      ((unsigned char*)image_back->data);
            unsigned char *back_buff=(!d_buff)?NULL:((img_sel)?
                                        ((unsigned char*)image->data):
                                        ((unsigned char*)image_back->data));

            if(!pdata->args.noshared){
                XShmGetImage(pdata->dpy,pdata->specs.root,
                            ((!img_sel)?image:image_back),
                            (temp_brwin.rgeom.x),
                            (temp_brwin.rgeom.y),AllPlanes);
            }
            if(pdata->args.noshared){
                GetZPixmap( pdata->dpy,
                            pdata->specs.root,
                            image->data,
                            temp_brwin.rgeom.x,
                            temp_brwin.rgeom.y,
                            temp_brwin.rgeom.width,
                            temp_brwin.rgeom.height);
            }
            pthread_mutex_lock(&pdata->yuv_mutex);
            for(i=0;i<blocknum_x*blocknum_y;i++){
                yblocks[i]=ublocks[i]=vblocks[i]=0;
            }
            UPDATE_YUV_BUFFER((&pdata->enc_data->yuv),
                              front_buff,back_buff,
                              (pdata->enc_data->x_offset),
                              (pdata->enc_data->y_offset),
                              (temp_brwin.rgeom.width),
                              (temp_brwin.rgeom.height),
                              pdata->args.no_quick_subsample,
                              pdata->specs.depth);
            pthread_mutex_unlock(&pdata->yuv_mutex);
        }
        if(pdata->args.xfixes_cursor ||
           pdata->args.have_dummy_cursor){
            int mouse_xoffset,
                mouse_yoffset;
            //avoid segfaults
            CLIP_DUMMY_POINTER_AREA(mouse_pos_abs,&temp_brwin,
                                    &mouse_pos_temp);
            mouse_xoffset=mouse_pos_temp.x-mouse_pos_abs.x;
            mouse_yoffset=mouse_pos_temp.y-mouse_pos_abs.y;
            if((mouse_xoffset<0) || (mouse_xoffset>mouse_pos_abs.width))
                mouse_xoffset=0;
            if((mouse_yoffset<0) || (mouse_yoffset>mouse_pos_abs.height))
                mouse_yoffset=0;
            //draw the cursor
            if((mouse_pos_temp.x>=0)&&
               (mouse_pos_temp.y>=0)&&
               (mouse_pos_temp.width>0)&&
               (mouse_pos_temp.height>0)){
                    if(pdata->args.xfixes_cursor){
                        XFIXES_POINTER_TO_YUV((&pdata->enc_data->yuv),
                                            ((unsigned char*)xcim->pixels),
                                            (mouse_pos_temp.x-
                                            temp_brwin.rgeom.x+
                                            pdata->enc_data->x_offset),
                                            (mouse_pos_temp.y-
                                            temp_brwin.rgeom.y+
                                            pdata->enc_data->y_offset),
                                            mouse_pos_temp.width,
                                            mouse_pos_temp.height,
                                            mouse_xoffset,
                                            mouse_yoffset,
                                            (xcim->width-mouse_pos_temp.width));
                    }
                    else{
                        DUMMY_POINTER_TO_YUV((&pdata->enc_data->yuv),
                                            pdata->dummy_pointer,
                                            (mouse_pos_temp.x-
                                            temp_brwin.rgeom.x+
                                            pdata->enc_data->x_offset),
                                            (mouse_pos_temp.y-
                                            temp_brwin.rgeom.y+
                                            pdata->enc_data->y_offset),
                                            mouse_pos_temp.width,
                                            mouse_pos_temp.height,
                                            mouse_xoffset,
                                            mouse_yoffset,
                                            pdata->npxl);
                    }
                if(d_buff){
                //make previous cursor position dirty
                    unsigned char *front_buff=
                                    (!img_sel)?((unsigned char*)image->data):
                                    ((unsigned char*)image_back->data);

                    MARK_BACK_BUFFER(   front_buff,
                                        (mouse_pos_temp.x-
                                        temp_brwin.rgeom.x+
                                        pdata->enc_data->x_offset),
                                        (mouse_pos_temp.y-
                                        temp_brwin.rgeom.y+
                                        pdata->enc_data->y_offset),
                                        mouse_pos_temp.width,
                                        mouse_pos_temp.height,
                                        (temp_brwin.rgeom.width),
                                        pdata->specs.depth)
                }

            }
            if(pdata->args.xfixes_cursor){
                XFree(xcim);
                xcim=NULL;
            }
        }
        if(!pdata->args.full_shots){
            ClearList(&pdata->rect_root[tlist_sel]);
            pthread_mutex_unlock(&pdata->list_mutex[tlist_sel]);
        }
        if(encoder_busy){
            frames_lost++;
        }
        pthread_mutex_lock(&pdata->img_buff_ready_mutex);
        pthread_cond_broadcast(&pdata->image_buffer_ready);
        pthread_mutex_unlock(&pdata->img_buff_ready_mutex);
        capture_busy=0;
    }

    if(!pdata->args.noframe){
        XDestroyWindow(pdata->dpy,shaped_w);
    }


    pthread_mutex_lock(&pdata->img_buff_ready_mutex);
    pthread_cond_broadcast(&pdata->image_buffer_ready);
    pthread_mutex_unlock(&pdata->img_buff_ready_mutex);
    if(!pdata->args.noshared){
        XShmDetach (pdata->dpy, &shminfo);
        shmdt (shminfo.shmaddr);
        shmctl (shminfo.shmid, IPC_RMID, 0);
        if(d_buff){
            XShmDetach (pdata->dpy, &shminfo_back);
            shmdt (shminfo_back.shmaddr);
            shmctl (shminfo_back.shmid, IPC_RMID, 0);
        }
    }
    pthread_exit(&errno);
}

