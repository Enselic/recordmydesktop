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

void *GetFrame(void *pdata){
    int tlist_sel=0;
    pthread_mutex_t pmut,tmut;
    uint msk_ret;
    WGeometry mouse_pos_abs,mouse_pos_rel,mouse_pos_temp;
    Window root_ret,child_ret;
    int pixel_total=((ProgData *)pdata)->brwin.rgeom.width*((ProgData *)pdata)->brwin.rgeom.height;
    XFixesCursorImage *xcim=NULL;

    mouse_pos_abs.x=0;
    mouse_pos_abs.y=0;
    mouse_pos_abs.width=((ProgData *)pdata)->dummy_p_size;
    mouse_pos_abs.height=((ProgData *)pdata)->dummy_p_size;
    pthread_mutex_init(&pmut,NULL);
    pthread_mutex_init(&tmut,NULL);

    while(((ProgData *)pdata)->running){
        pthread_cond_wait(&((ProgData *)pdata)->time_cond,&tmut);
        if(Paused){
            pthread_cond_wait(&((ProgData *)pdata)->pause_cond,&pmut);
        }
        /*pthread_cond_wait(&((ProgData *)pdata)->pause_cond,&((ProgData *)pdata)->pause_cond_mutex);*/
        //mutexes and lists with changes are useless when full_shots is enabled
        if(!((ProgData *)pdata)->args.full_shots){
            tlist_sel=((ProgData *)pdata)->list_selector;
            ((ProgData *)pdata)->list_selector=((((ProgData *)pdata)->list_selector+1)%2);
            pthread_mutex_lock(&((ProgData *)pdata)->list_mutex[tlist_sel]);
        }
        //here we measure the list and decide which way we will go
        if(!((ProgData *)pdata)->args.nocondshared){
            int level=0;
            RectArea *temp=((ProgData *)pdata)->rect_root[tlist_sel];
            
            if(temp!=NULL){
                do{
                    level+=temp->geom.width*temp->geom.height;
                    temp=temp->next;
                }while(temp!=NULL);
                level*=100;
                level/=pixel_total;
                ((ProgData *)pdata)->args.noshared=(level<((ProgData *)pdata)->args.shared_thres);
//                 if(!((ProgData *)pdata)->args.noshared){
//                     fprintf(stderr,"shared screenshot with %d\n",level);
//                 }
            }
        }
        if(((ProgData *)pdata)->args.xfixes_cursor){
        //xfixes pointer sequence
        //update previous_position
            //(if full_shots is enabled this is skipped since it's pointless)
            if(!((ProgData *)pdata)->args.full_shots){
                CLIP_DUMMY_POINTER_AREA(mouse_pos_abs,&((ProgData *)pdata)->brwin,&mouse_pos_temp);
                if((mouse_pos_temp.x>=0)&&(mouse_pos_temp.y>=0)&&(mouse_pos_temp.width>0)&&(mouse_pos_temp.height>0))
                    RectInsert(&((ProgData *)pdata)->rect_root[tlist_sel],&mouse_pos_temp);        
            }
            xcim=XFixesGetCursorImage(((ProgData *)pdata)->dpy);
            mouse_pos_abs.x=xcim->x;
            mouse_pos_abs.y=xcim->y;
            mouse_pos_abs.width=xcim->width;
            mouse_pos_abs.height=xcim->height;
        }
        if(((ProgData *)pdata)->args.have_dummy_cursor){
        //dummy pointer sequence
        //update previous_position
        //(if full_shots is enabled this is skipped since it's pointless)
            if(!((ProgData *)pdata)->args.full_shots){
                CLIP_DUMMY_POINTER_AREA(mouse_pos_abs,&((ProgData *)pdata)->brwin,&mouse_pos_temp);
                if((mouse_pos_temp.x>=0)&&(mouse_pos_temp.y>=0)&&(mouse_pos_temp.width>0)&&(mouse_pos_temp.height>0))
                    RectInsert(&((ProgData *)pdata)->rect_root[tlist_sel],&mouse_pos_temp);        
            }
        //find new one
            XQueryPointer(((ProgData *)pdata)->dpy,
                            ((ProgData *)pdata)->specs.root,
                            &root_ret,&child_ret,
                            &mouse_pos_abs.x,&mouse_pos_abs.y,
                            &mouse_pos_rel.x,&mouse_pos_rel.y,&msk_ret);
        }
        if(!((ProgData *)pdata)->args.noshared)
            XShmGetImage(((ProgData *)pdata)->dpy,((ProgData *)pdata)->specs.root,((ProgData *)pdata)->shimage,(((ProgData *)pdata)->brwin.rgeom.x),(((ProgData *)pdata)->brwin.rgeom.y),AllPlanes);
        if(!((ProgData *)pdata)->args.full_shots)
            UpdateImage(((ProgData *)pdata)->dpy,
                        &((ProgData *)pdata)->enc_data->yuv,
                        &((ProgData *)pdata)->yuv_mutex,
                        &((ProgData *)pdata)->specs,
                        &((ProgData *)pdata)->rect_root[tlist_sel],
                        &((ProgData *)pdata)->brwin,
                        ((ProgData *)pdata)->enc_data,
                        ((((ProgData *)pdata)->args.noshared)?(((ProgData *)pdata)->datatemp):((ProgData *)pdata)->shimage->data),
                        ((ProgData *)pdata)->args.noshared,
                        ((ProgData *)pdata)->args.no_quick_subsample);
        else{
            if(((ProgData *)pdata)->args.noshared){
                GetZPixmap( ((ProgData *)pdata)->dpy,
                            ((ProgData *)pdata)->specs.root,
                            ((ProgData *)pdata)->image->data,
                            ((ProgData *)pdata)->brwin.rgeom.x,
                            ((ProgData *)pdata)->brwin.rgeom.y,
                            ((ProgData *)pdata)->brwin.rgeom.width,
                            ((ProgData *)pdata)->brwin.rgeom.height);
                pthread_mutex_lock(&((ProgData *)pdata)->yuv_mutex);
                if(((ProgData *)pdata)->args.no_quick_subsample){
                    UPDATE_YUV_BUFFER_IM_AVG((&((ProgData *)pdata)->enc_data->yuv),((unsigned char*)((ProgData *)pdata)->image->data),
                    (((ProgData *)pdata)->enc_data->x_offset),(((ProgData *)pdata)->enc_data->y_offset),
                    (((ProgData *)pdata)->brwin.rgeom.width),(((ProgData *)pdata)->brwin.rgeom.height));
                }
                else{
                    UPDATE_YUV_BUFFER_IM((&((ProgData *)pdata)->enc_data->yuv),((unsigned char*)((ProgData *)pdata)->image->data),
                    (((ProgData *)pdata)->enc_data->x_offset),(((ProgData *)pdata)->enc_data->y_offset),
                    (((ProgData *)pdata)->brwin.rgeom.width),(((ProgData *)pdata)->brwin.rgeom.height));
                }
                pthread_mutex_unlock(&((ProgData *)pdata)->yuv_mutex);
            }
            else{
                pthread_mutex_lock(&((ProgData *)pdata)->yuv_mutex);
                if(((ProgData *)pdata)->args.no_quick_subsample){
                    UPDATE_YUV_BUFFER_IM_AVG((&((ProgData *)pdata)->enc_data->yuv),((unsigned char*)((ProgData *)pdata)->shimage->data),
                    (((ProgData *)pdata)->enc_data->x_offset),(((ProgData *)pdata)->enc_data->y_offset),
                    (((ProgData *)pdata)->brwin.rgeom.width),(((ProgData *)pdata)->brwin.rgeom.height));
                }
                else{
                    UPDATE_YUV_BUFFER_IM((&((ProgData *)pdata)->enc_data->yuv),((unsigned char*)((ProgData *)pdata)->shimage->data),
                    (((ProgData *)pdata)->enc_data->x_offset),(((ProgData *)pdata)->enc_data->y_offset),
                    (((ProgData *)pdata)->brwin.rgeom.width),(((ProgData *)pdata)->brwin.rgeom.height));
                }

                pthread_mutex_unlock(&((ProgData *)pdata)->yuv_mutex);
            }
        }
        if(((ProgData *)pdata)->args.xfixes_cursor){
        //avoid segfaults
            CLIP_DUMMY_POINTER_AREA(mouse_pos_abs,&((ProgData *)pdata)->brwin,&mouse_pos_temp);
        //draw the cursor
            if((mouse_pos_temp.x>=0)&&(mouse_pos_temp.y>=0)&&(mouse_pos_temp.width>0)&&(mouse_pos_temp.height>0)){
                XFIXES_POINTER_TO_YUV((&((ProgData *)pdata)->enc_data->yuv),((unsigned char*)xcim->pixels),
                        (mouse_pos_temp.x-((ProgData *)pdata)->brwin.rgeom.x+((ProgData *)pdata)->enc_data->x_offset),
                        (mouse_pos_temp.y-((ProgData *)pdata)->brwin.rgeom.y+((ProgData *)pdata)->enc_data->y_offset),
                        mouse_pos_temp.width,
                        mouse_pos_temp.height,
                        (xcim->width-mouse_pos_temp.width));
            }
        }

        if(((ProgData *)pdata)->args.have_dummy_cursor){
        //avoid segfaults
            CLIP_DUMMY_POINTER_AREA(mouse_pos_abs,&((ProgData *)pdata)->brwin,&mouse_pos_temp);
        //draw the cursor
            if((mouse_pos_temp.x>=0)&&(mouse_pos_temp.y>=0)&&(mouse_pos_temp.width>0)&&(mouse_pos_temp.height>0)){
                DUMMY_POINTER_TO_YUV((&((ProgData *)pdata)->enc_data->yuv),
                                    ((ProgData *)pdata)->dummy_pointer,
                                    (mouse_pos_temp.x-((ProgData *)pdata)->brwin.rgeom.x+((ProgData *)pdata)->enc_data->x_offset),
                                    (mouse_pos_temp.y-((ProgData *)pdata)->brwin.rgeom.y+((ProgData *)pdata)->enc_data->y_offset),
                                    mouse_pos_temp.width,
                                    mouse_pos_temp.height,
                                    ((ProgData *)pdata)->npxl);
            }
        }
        if(!((ProgData *)pdata)->args.full_shots){
            ClearList(&((ProgData *)pdata)->rect_root[tlist_sel]);
            pthread_mutex_unlock(&((ProgData *)pdata)->list_mutex[tlist_sel]);
        }
        pthread_cond_broadcast(&((ProgData *)pdata)->image_buffer_ready);
    }
    pthread_cond_broadcast(&((ProgData *)pdata)->image_buffer_ready);
    pthread_exit(&errno);
}

