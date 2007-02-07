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

void *GetFrame(ProgData *pdata){
    int tlist_sel=0;
    unsigned char *dtap=NULL;   //pointer switching among shared memory and
                                //normal buffer
    uint msk_ret;
    WGeometry mouse_pos_abs,mouse_pos_rel,mouse_pos_temp;
    Window root_ret,child_ret;
    int pixel_total=pdata->brwin.rgeom.width*pdata->brwin.rgeom.height;
    XFixesCursorImage *xcim=NULL;

    mouse_pos_abs.x=mouse_pos_temp.x=0;
    mouse_pos_abs.y=mouse_pos_temp.y=0;
    mouse_pos_abs.width=mouse_pos_temp.width=pdata->dummy_p_size;
    mouse_pos_abs.height=mouse_pos_temp.height=pdata->dummy_p_size;

    while(pdata->running){

        //if we are left behind we must not wait.
        //also before actually pausing we must make sure the streams
        //are synced. sound stops so this should only happen quickly.
        if(pdata->avd>0){
            pthread_mutex_lock(&time_mutex);
            pthread_cond_wait(&pdata->time_cond,&time_mutex);
            pthread_mutex_unlock(&time_mutex);
            if(Paused){
                pthread_mutex_lock(&pause_mutex);
                pthread_cond_wait(&pdata->pause_cond,&pause_mutex);
                pthread_mutex_unlock(&pause_mutex);
            }
        }
        capture_busy=1;
        //mutexes and lists with changes are useless when full_shots is enabled
        if(!pdata->args.full_shots){
            tlist_sel=pdata->list_selector;
            pdata->list_selector=((pdata->list_selector+1)%2);
            pthread_mutex_lock(&pdata->list_mutex[tlist_sel]);
        }
        //here we measure the list and decide which way we will go
        if(!pdata->args.nocondshared){
            int level=0;
            RectArea *temp=pdata->rect_root[tlist_sel];

            if(temp!=NULL){
                do{
                    level+=temp->geom.width*temp->geom.height;
                    temp=temp->next;
                }while(temp!=NULL);
                level*=100;
                level/=pixel_total;
                pdata->args.noshared=(level<pdata->args.shared_thres);
            }
        }
        if(pdata->args.xfixes_cursor){
        //xfixes pointer sequence
        //update previous_position
            //(if full_shots is enabled this is skipped since it's pointless)
            if(!pdata->args.full_shots){
                CLIP_DUMMY_POINTER_AREA(mouse_pos_abs,&pdata->brwin,
                                        &mouse_pos_temp);
                if((mouse_pos_temp.x>=0)&&
                   (mouse_pos_temp.y>=0)&&
                   (mouse_pos_temp.width>0)&&
                   (mouse_pos_temp.height>0))
                    RectInsert(&pdata->rect_root[tlist_sel],&mouse_pos_temp);
            }
            xcim=XFixesGetCursorImage(pdata->dpy);
            mouse_pos_abs.x=xcim->x;
            mouse_pos_abs.y=xcim->y;
            mouse_pos_abs.width=xcim->width;
            mouse_pos_abs.height=xcim->height;
        }
        if(pdata->args.have_dummy_cursor){
        //dummy pointer sequence
        //update previous_position
        //(if full_shots is enabled this is skipped since it's pointless)
            if(!pdata->args.full_shots){
                CLIP_DUMMY_POINTER_AREA(mouse_pos_abs,&pdata->brwin,
                                        &mouse_pos_temp);
                if((mouse_pos_temp.x>=0)&&
                   (mouse_pos_temp.y>=0)&&
                   (mouse_pos_temp.width>0)&&
                   (mouse_pos_temp.height>0))
                    RectInsert(&pdata->rect_root[tlist_sel],&mouse_pos_temp);
            }
        //find new one
            XQueryPointer(pdata->dpy,
                            pdata->specs.root,
                            &root_ret,&child_ret,
                            &mouse_pos_abs.x,&mouse_pos_abs.y,
                            &mouse_pos_rel.x,&mouse_pos_rel.y,&msk_ret);
        }
        if(!pdata->args.noshared)
            XShmGetImage(pdata->dpy,pdata->specs.root,pdata->shimage,
                         (pdata->brwin.rgeom.x),
                         (pdata->brwin.rgeom.y),AllPlanes);
        if(!pdata->args.full_shots)
            UpdateImage(pdata->dpy,
                        &pdata->enc_data->yuv,
                        &pdata->yuv_mutex,
                        &pdata->specs,
                        &pdata->rect_root[tlist_sel],
                        &pdata->brwin,
                        pdata->enc_data,
                        ((pdata->args.noshared)?
                         (pdata->datatemp):
                         (pdata->shimage->data)),
                        pdata->args.noshared,
                        pdata->args.no_quick_subsample);
        else{

            dtap=(((pdata->args.nocondshared)&&(!pdata->args.noshared))?
                 ((unsigned char*)pdata->shimage->data):
                 ((unsigned char*)pdata->image->data));
            if(pdata->args.noshared){
                GetZPixmap( pdata->dpy,
                            pdata->specs.root,
                            pdata->image->data,
                            pdata->brwin.rgeom.x,
                            pdata->brwin.rgeom.y,
                            pdata->brwin.rgeom.width,
                            pdata->brwin.rgeom.height);
            }
            pthread_mutex_lock(&pdata->yuv_mutex);
            UPDATE_YUV_BUFFER((&pdata->enc_data->yuv),dtap,
                                (pdata->enc_data->x_offset),
                                (pdata->enc_data->y_offset),
                                (pdata->brwin.rgeom.width),
                                (pdata->brwin.rgeom.height),
                                pdata->args.noshared,
                                pdata->args.no_quick_subsample,
                                pdata->specs.depth);
            pthread_mutex_unlock(&pdata->yuv_mutex);
        }
        if(pdata->args.xfixes_cursor){
        //avoid segfaults
            CLIP_DUMMY_POINTER_AREA(mouse_pos_abs,&pdata->brwin,
                                    &mouse_pos_temp);
        //draw the cursor
            if((mouse_pos_temp.x>=0)&&
               (mouse_pos_temp.y>=0)&&
               (mouse_pos_temp.width>0)&&
               (mouse_pos_temp.height>0)){
                XFIXES_POINTER_TO_YUV((&pdata->enc_data->yuv),
                                      ((unsigned char*)xcim->pixels),
                                      (mouse_pos_temp.x-
                                       pdata->brwin.rgeom.x+
                                       pdata->enc_data->x_offset),
                                      (mouse_pos_temp.y-
                                       pdata->brwin.rgeom.y+
                                       pdata->enc_data->y_offset),
                                      mouse_pos_temp.width,
                                      mouse_pos_temp.height,
                                      (xcim->width-mouse_pos_temp.width));
            }
            XFree(xcim);
            xcim=NULL;
        }

        if(pdata->args.have_dummy_cursor){
        //avoid segfaults
            CLIP_DUMMY_POINTER_AREA(mouse_pos_abs,&pdata->brwin,
                                    &mouse_pos_temp);
        //draw the cursor
            if((mouse_pos_temp.x>=0)&&
               (mouse_pos_temp.y>=0)&&
               (mouse_pos_temp.width>0)&&
               (mouse_pos_temp.height>0)){
                DUMMY_POINTER_TO_YUV((&pdata->enc_data->yuv),
                                     pdata->dummy_pointer,
                                     (mouse_pos_temp.x-
                                      pdata->brwin.rgeom.x+
                                      pdata->enc_data->x_offset),
                                     (mouse_pos_temp.y-
                                      pdata->brwin.rgeom.y+
                                      pdata->enc_data->y_offset),
                                     mouse_pos_temp.width,
                                     mouse_pos_temp.height,
                                     pdata->npxl);
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
    pthread_mutex_lock(&pdata->img_buff_ready_mutex);
    pthread_cond_broadcast(&pdata->image_buffer_ready);
    pthread_mutex_unlock(&pdata->img_buff_ready_mutex);
    pthread_exit(&errno);
}

