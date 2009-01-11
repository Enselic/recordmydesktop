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

#include "config.h"
#include "rmd_get_frame.h"

#include "rmd_cache.h"
#include "rmd_frame.h"
#include "rmd_getzpixmap.h"
#include "rmd_poll_events.h"
#include "rmd_rectinsert.h"
#include "rmd_update_image.h"
#include "rmd_yuv_utils.h"
#include "rmd_types.h"

#include <X11/extensions/Xfixes.h>
#include <X11/extensions/XShm.h>

#include <limits.h>
#include <pthread.h>
#include <sys/shm.h>
#include <errno.h>


#define AVG_4_PIXELS(data_array,width_img,k_tm,i_tm,offset)\
    ((data_array[(k_tm*width_img+i_tm)*RMD_ULONG_SIZE_T+offset]+\
    data_array[((k_tm-1)*width_img+i_tm)*RMD_ULONG_SIZE_T+offset]+\
    data_array[(k_tm*width_img+i_tm-1)*RMD_ULONG_SIZE_T+offset]+\
    data_array[((k_tm-1)*width_img+i_tm-1)*RMD_ULONG_SIZE_T+offset])/4)

#define CLIP_DUMMY_POINTER_AREA(dummy_p_area,brwin,xrect){\
    (xrect)->x=((((dummy_p_area).x+\
                (dummy_p_area).width>=(brwin)->rrect.x)&&\
                ((dummy_p_area).x<=(brwin)->rrect.x+\
                (brwin)->rrect.width))?\
                (((dummy_p_area).x<=(brwin)->rrect.x)?\
                (brwin)->rrect.x:(dummy_p_area).x):-1);\
    (xrect)->y=((((dummy_p_area).y+\
                (dummy_p_area).height>=(brwin)->rrect.y)&&\
                ((dummy_p_area).y<=(brwin)->rrect.y+\
                (brwin)->rrect.height))?\
                (((dummy_p_area).y<=(brwin)->rrect.y)?\
                (brwin)->rrect.y:(dummy_p_area).y):-1);\
    (xrect)->width=((dummy_p_area).x<=(brwin)->rrect.x)?\
                (dummy_p_area).width-\
                ((brwin)->rrect.x-(dummy_p_area).x):\
                ((dummy_p_area).x<=(brwin)->rrect.x+\
                (brwin)->rrect.width)?\
                ((brwin)->rrect.width-(dummy_p_area).x+\
                (brwin)->rrect.x<(dummy_p_area).width)?\
                (brwin)->rrect.width-(dummy_p_area).x+\
                (brwin)->rrect.x:(dummy_p_area).width:0;\
    (xrect)->height=((dummy_p_area).y<=(brwin)->rrect.y)?\
                (dummy_p_area).height-\
                ((brwin)->rrect.y-(dummy_p_area).y):\
                ((dummy_p_area).y<=(brwin)->rrect.y+\
                (brwin)->rrect.height)?\
                ((brwin)->rrect.height-(dummy_p_area).y+\
                (brwin)->rrect.y<(dummy_p_area).height)?\
                (brwin)->rrect.height-(dummy_p_area).y+\
                (brwin)->rrect.y:(dummy_p_area).height:0;\
    if((xrect)->width>(brwin)->rrect.width)\
        (xrect)->width=(brwin)->rrect.width;\
    if((xrect)->height>(brwin)->rrect.height)\
        (xrect)->height=(brwin)->rrect.height;\
}

#define XFIXES_POINTER_TO_YUV(yuv,\
                              data,\
                              x_tm,\
                              y_tm,\
                              width_tm,\
                              height_tm,\
                              x_offset,\
                              y_offset,\
                              column_discard_stride){\
    int i,k,j=0;\
    unsigned char avg0,avg1,avg2,avg3;\
    int x_2=x_tm/2,y_2=y_tm/2;\
    for(k=y_offset;k<y_offset+height_tm;k++){\
        for(i=x_offset;i<x_offset+width_tm;i++){\
                j=k*(width_tm+column_discard_stride)+i;\
                yuv->y[x_tm+(i-x_offset)+(k+y_tm-y_offset)*yuv->y_width]=\
                    (yuv->y[x_tm+(i-x_offset)+(k-y_offset+y_tm)*yuv->y_width]*\
                    (UCHAR_MAX-data[(j*RMD_ULONG_SIZE_T)+__ABYTE])+\
                    ( ( Yr[data[(j*RMD_ULONG_SIZE_T)+__RBYTE]]+\
                        Yg[data[(j*RMD_ULONG_SIZE_T)+__GBYTE]] +\
                        Yb[data[(j*RMD_ULONG_SIZE_T)+__BBYTE]] ) % \
                      ( UCHAR_MAX + 1 ) ) * \
                data[(j*RMD_ULONG_SIZE_T)+__ABYTE])/UCHAR_MAX ;\
                if((k%2)&&(i%2)){\
                    avg3=AVG_4_PIXELS(data,\
                                      (width_tm+column_discard_stride),\
                                      k,i,__ABYTE);\
                    avg2=AVG_4_PIXELS(data,\
                                      (width_tm+column_discard_stride),\
                                      k,i,__RBYTE);\
                    avg1=AVG_4_PIXELS(data,\
                                      (width_tm+column_discard_stride),\
                                      k,i,__GBYTE);\
                    avg0=AVG_4_PIXELS(data,\
                                      (width_tm+column_discard_stride),\
                                      k,i,__BBYTE);\
                    yuv->u[x_2+(i-x_offset)/2+((k-y_offset)/2+y_2)*\
                           yuv->uv_width]=\
                    (yuv->u[x_2+(i-x_offset)/2+((k-y_offset)/2+y_2)*\
                            yuv->uv_width]*\
                    (UCHAR_MAX-avg3)+\
                    ( (Ur[avg2] + Ug[avg1] + UbVr[avg0]) % \
                      ( UCHAR_MAX + 1 ) ) * avg3 ) / \
                        UCHAR_MAX;\
                    yuv->v[x_2+(i-x_offset)/2+((k-y_offset)/2+y_2)*\
                           yuv->uv_width]=\
                    (yuv->v[x_2+(i-x_offset)/2+((k-y_offset)/2+y_2)*\
                            yuv->uv_width]*\
                    (UCHAR_MAX-avg3)+\
                    ( ( UbVr[avg2] + Vg[avg1] + Vb[avg0] ) % \
                      ( UCHAR_MAX + 1 ) ) * avg3 ) / \
                        UCHAR_MAX; \
                }\
        }\
    }\
}

#define MARK_BUFFER_AREA_C( data,\
                            x_tm,\
                            y_tm,\
                            width_tm,\
                            height_tm,\
                            buffer_width,\
                            __bit_depth__){\
    int k,i;\
    register u_int##__bit_depth__##_t\
        *datapi=\
            ((u_int##__bit_depth__##_t *)data)+y_tm*buffer_width+x_tm;\
    for(k=0;k<height_tm;k++){\
        for(i=0;i<width_tm;i++){\
            *datapi+=1;\
            datapi++;\
        }\
        datapi+=buffer_width-width_tm;\
    }\
}

#define MARK_BUFFER_AREA(   data,\
                            x_tm,\
                            y_tm,\
                            width_tm,\
                            height_tm,\
                            buffer_width,\
                            __bit_depth__){\
    if((__bit_depth__==24)||(__bit_depth__==32)){\
        MARK_BUFFER_AREA_C( data,\
                            x_tm,\
                            y_tm,\
                            width_tm,\
                            height_tm,\
                            buffer_width,\
                            32)\
    }\
    else{\
        MARK_BUFFER_AREA_C( data,\
                            x_tm,\
                            y_tm,\
                            width_tm,\
                            height_tm,\
                            buffer_width,\
                            16)\
    }\
}\


//besides taking the first screenshot, this functions primary purpose is to 
//initialize the structures and memory.
static int rmdFirstFrame(ProgData *pdata,
                         XImage **image,
                         XShmSegmentInfo *shminfo,
                         char **pxl_data) {

    if((pdata->args.noshared)){

        *pxl_data=(char *)malloc(pdata->brwin.nbytes);
        
        (*image)=XCreateImage(pdata->dpy,
                            pdata->specs.visual,
                            pdata->specs.depth,
                            ZPixmap,
                            0,
                            *pxl_data,
                            pdata->brwin.rrect.width,
                            pdata->brwin.rrect.height,
                            8,
                            0);
        XInitImage((*image));
        rmdGetZPixmap(pdata->dpy,pdata->specs.root,
                      (*image)->data,
                      pdata->brwin.rrect.x,
                      pdata->brwin.rrect.y,
                      pdata->brwin.rrect.width,
                      pdata->brwin.rrect.height);
    }
    else{
        (*image)=XShmCreateImage(pdata->dpy,
                                     pdata->specs.visual,
                                     pdata->specs.depth,
                                     ZPixmap,
                                     *pxl_data,
                                     shminfo,
                                     pdata->brwin.rrect.width,
                                     pdata->brwin.rrect.height);
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
                     pdata->brwin.rrect.x,
                     pdata->brwin.rrect.y,
                     AllPlanes);
    }

    UPDATE_YUV_BUFFER((&pdata->enc_data->yuv),
            ((unsigned char*)((*image))->data),NULL,
            (pdata->enc_data->x_offset),(pdata->enc_data->y_offset),
            (pdata->brwin.rrect.width),(pdata->brwin.rrect.height),
            (pdata->args.no_quick_subsample),
            pdata->specs.depth);

    return 0;
}

//make a deep copy
static void rmdBRWinCpy(BRWindow *target, BRWindow *source) {

    target->rect.x=source->rect.x;
    target->rect.y=source->rect.y;
    target->rect.width=source->rect.width;
    target->rect.height=source->rect.height;
    target->rrect.x=source->rrect.x;
    target->rrect.y=source->rrect.y;
    target->rrect.width=source->rrect.width;
    target->rrect.height=source->rrect.height;
    target->nbytes=source->nbytes;
    target->windowid=source->windowid;

}

//recenters the capture area to the mouse
//without exiting the display bounding box
static void rmdMoveCaptureArea(BRWindow *brwin,
                               int cursor_x,
                               int cursor_y,
                               int width,
                               int height) {
    int t_x=0,t_y=0;

    t_x=cursor_x-brwin->rrect.width/2;
    t_x=(t_x>>1)<<1;
    brwin->rrect.x=(t_x<0)?0:((t_x+brwin->rrect.width>width)?
                               width-brwin->rrect.width:t_x);
    t_y=cursor_y-brwin->rrect.height/2;
    t_y=(t_y>>1)<<1;
    brwin->rrect.y=(t_y<0)?0:((t_y+brwin->rrect.height>height)?
                               height-brwin->rrect.height:t_y);
}

/**
*   Extract cache blocks from damage list
*
* \param root Root entry of the list with damaged areas
*
* \param x_offset left x of the recording area
*
* \param x_offset upper y of the recording area
*
* \param blocknum_x Width of image in blocks
*
* \param blocknum_y Height of image in blocks
*/
static void rmdBlocksFromList (RectArea   **root,
                               unsigned int x_offset,
                               unsigned int y_offset,
                               unsigned int blocknum_x,
                               unsigned int blocknum_y) {

  RectArea    *temp;
  int i,
      j,
      blockno,
      row_start,
      row_end,
      column_start,
      column_end;

  temp = *root;

  for (i = 0; i < blocknum_x * blocknum_y; i++) {
    yblocks[i] = ublocks[i] = vblocks[i] = 0;
  }

  while (temp != NULL) {

    column_start = ((int)(temp->rect.x - x_offset)) / Y_UNIT_WIDTH;
    column_end   = ((int)(temp->rect.x + (temp->rect.width - 1) - x_offset)) / Y_UNIT_WIDTH;
    row_start    = ((int)(temp->rect.y - y_offset)) / Y_UNIT_WIDTH;
    row_end      = ((int)(temp->rect.y + (temp->rect.height - 1) - y_offset)) / Y_UNIT_WIDTH;

    for (i = row_start; i < row_end + 1; i++) {
      for (j = column_start; j < column_end + 1; j++) {
        blockno          = i * blocknum_x + j;
        yblocks[blockno] = 1;
        ublocks[blockno] = 1;
        vblocks[blockno] = 1;
      }
    }
    
    temp = temp->next;
  }
}

void *rmdGetFrame(ProgData *pdata){
    int i=0,
        blocknum_x=pdata->enc_data->yuv.y_width/Y_UNIT_WIDTH,
        blocknum_y=pdata->enc_data->yuv.y_height/Y_UNIT_WIDTH;
    unsigned int msk_ret;
    XRectangle mouse_pos_abs,mouse_pos_rel,mouse_pos_temp;
    BRWindow temp_brwin;
    Window root_ret,
           child_ret; //Frame
    XFixesCursorImage *xcim=NULL;
    XImage *image=NULL,*image_back=NULL;          //the image that holds
                                        //the current full screenshot
    XShmSegmentInfo shminfo,shminfo_back;//info structure for the image above.
    int init_img1=0,init_img2=0,
        img_sel,d_buff;
    char *pxl_data=NULL,*pxl_data_back=NULL;




    img_sel=d_buff=pdata->args.full_shots;

    if((init_img1=rmdFirstFrame(pdata,&image,&shminfo,&pxl_data)!=0)){
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
            rmdPurgeCache(pdata->cache_data,!pdata->args.nosound);
        }
        exit(init_img1);
    }
    if(d_buff){
        if((init_img2=rmdFirstFrame(pdata,&image_back,&shminfo_back,
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
                rmdPurgeCache(pdata->cache_data,!pdata->args.nosound);
            }
            exit(init_img2);
        }

    }

    if(!pdata->args.noframe){
        pdata->shaped_w=rmdFrameInit(pdata->dpy,
                              pdata->specs.screen,
                              pdata->specs.root,
                              pdata->brwin.rrect.x,
                              pdata->brwin.rrect.y,
                              pdata->brwin.rrect.width,
                              pdata->brwin.rrect.height);
        XSelectInput(pdata->dpy,pdata->shaped_w,ExposureMask);
    }

    mouse_pos_abs.x=mouse_pos_temp.x=0;
    mouse_pos_abs.y=mouse_pos_temp.y=0;
    mouse_pos_abs.width=mouse_pos_temp.width=pdata->dummy_p_size;
    mouse_pos_abs.height=mouse_pos_temp.height=pdata->dummy_p_size;
    
    //This is the the place where we call XSelectInput
    //and arrange so that we listen for damage on all 
    //windows
    rmdInitEventsPolling(pdata);

    while(pdata->running){

        //if we are left behind we must not wait.
        //also before actually pausing we must make sure the streams
        //are synced. sound stops so this should only happen quickly.
        if(pdata->avd>0 || pdata->args.nosound){
            pthread_mutex_lock(&pdata->time_mutex);
            pthread_cond_wait(&pdata->time_cond, &pdata->time_mutex);
            pthread_mutex_unlock(&pdata->time_mutex);
            if (pdata->paused) {
                //this is necessary since event loop processes
                //the shortcuts which will unpause the program
                rmdEventLoop(pdata);
                continue;
            }
        }
        //read all events and construct list with damage 
        //events (if not full_shots)
        rmdEventLoop(pdata);

        //switch back and front buffers (full_shots only)
        if(d_buff)
            img_sel=(img_sel)?0:1;
        pdata->capture_busy = TRUE;

        rmdBRWinCpy(&temp_brwin,&pdata->brwin);


        if(pdata->args.xfixes_cursor ||
           pdata->args.have_dummy_cursor||
           pdata->args.follow_mouse){


            // Pointer sequence:
            // * Mark previous position as dirty with rmdRectInsert()
            // * Update to new position
            // * Mark new position as dirty with rmdRectInsert()
            if (!pdata->args.full_shots &&
                mouse_pos_temp.x >=0 &&
                mouse_pos_temp.y >=0 &&
                mouse_pos_temp.width > 0 &&
                mouse_pos_temp.height > 0) {
                rmdRectInsert(&pdata->rect_root,&mouse_pos_temp);
            }
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
                              (int *)&mouse_pos_abs.x,(int *)&mouse_pos_abs.y,
                              (int *)&mouse_pos_rel.x,(int *)&mouse_pos_rel.y,&msk_ret);
            }
            
            CLIP_DUMMY_POINTER_AREA(mouse_pos_abs, &temp_brwin, &mouse_pos_temp);
            if( mouse_pos_temp.x >=0 &&
                mouse_pos_temp.y >=0 &&
                mouse_pos_temp.width > 0 &&
                mouse_pos_temp.height > 0) {

                //there are 3 capture scenarios:
                // * Xdamage
                // * full-shots with double buffering 
                // * full-shots on a single buffer
                //The last one cannot be reached through
                //this code (see above how the d_buf variable is set), but 
                //even if it could, it would not be of interest regarding the 
                //marking of the cursor area. Single buffer means full repaint 
                //on every frame so there is no need for marking at all.

                if (!pdata->args.full_shots) { 

                    rmdRectInsert(&pdata->rect_root,&mouse_pos_temp);

                }
                else if(d_buff){
                        unsigned char *back_buff=
                                        (img_sel)?((unsigned char*)image->data):
                                        ((unsigned char*)image_back->data);

                        MARK_BUFFER_AREA(   back_buff,
                                            (mouse_pos_temp.x-
                                            temp_brwin.rrect.x+
                                            pdata->enc_data->x_offset),
                                            (mouse_pos_temp.y-
                                            temp_brwin.rrect.y+
                                            pdata->enc_data->y_offset),
                                            mouse_pos_temp.width,
                                            mouse_pos_temp.height,
                                            (temp_brwin.rrect.width),
                                            pdata->specs.depth)
                }
            }
        }
        if(pdata->args.follow_mouse){
            rmdMoveCaptureArea(&pdata->brwin,
                               mouse_pos_abs.x+
                               ((pdata->args.xfixes_cursor)?xcim->xhot:0),
                               mouse_pos_abs.y+
                               ((pdata->args.xfixes_cursor)?xcim->yhot:0),
                               pdata->specs.width,
                               pdata->specs.height);
            if(!pdata->args.noframe){
                rmdMoveFrame(pdata->dpy,
                             pdata->shaped_w,
                             temp_brwin.rrect.x,
                             temp_brwin.rrect.y);

            }
        }

        if(!pdata->args.full_shots){
            pthread_mutex_lock(&pdata->yuv_mutex);
            rmdUpdateImage(pdata->dpy,
                           &pdata->enc_data->yuv,
                           &pdata->specs,
                           &pdata->rect_root,
                           &temp_brwin,
                           pdata->enc_data,
                           image->data,
                           pdata->args.noshared,
                           &shminfo,
                           pdata->shm_opcode,
                           pdata->args.no_quick_subsample);
            rmdBlocksFromList(&pdata->rect_root,
                              temp_brwin.rrect.x,
                              temp_brwin.rrect.y,
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
                            (temp_brwin.rrect.x),
                            (temp_brwin.rrect.y),AllPlanes);
            }
            if(pdata->args.noshared){
                rmdGetZPixmap( pdata->dpy,
                               pdata->specs.root,
                               image->data,
                               temp_brwin.rrect.x,
                               temp_brwin.rrect.y,
                               temp_brwin.rrect.width,
                               temp_brwin.rrect.height);
            }
            pthread_mutex_lock(&pdata->yuv_mutex);
            for(i=0;i<blocknum_x*blocknum_y;i++){
                yblocks[i]=ublocks[i]=vblocks[i]=0;
            }
            UPDATE_YUV_BUFFER((&pdata->enc_data->yuv),
                              front_buff,back_buff,
                              (pdata->enc_data->x_offset),
                              (pdata->enc_data->y_offset),
                              (temp_brwin.rrect.width),
                              (temp_brwin.rrect.height),
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
                                            temp_brwin.rrect.x+
                                            pdata->enc_data->x_offset),
                                            (mouse_pos_temp.y-
                                            temp_brwin.rrect.y+
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
                                            temp_brwin.rrect.x+
                                            pdata->enc_data->x_offset),
                                            (mouse_pos_temp.y-
                                            temp_brwin.rrect.y+
                                            pdata->enc_data->y_offset),
                                            mouse_pos_temp.width,
                                            mouse_pos_temp.height,
                                            mouse_xoffset,
                                            mouse_yoffset,
                                            pdata->npxl);
                    }
                if(d_buff){
                    //make previous cursor position dirty
                    //on the currently front buffer (which 
                    //will be the back buffer next time it's 
                    //used) 
                    unsigned char *front_buff=
                                    (!img_sel)?((unsigned char*)image->data):
                                    ((unsigned char*)image_back->data);

                    MARK_BUFFER_AREA(   front_buff,
                                        (mouse_pos_temp.x-
                                        temp_brwin.rrect.x+
                                        pdata->enc_data->x_offset),
                                        (mouse_pos_temp.y-
                                        temp_brwin.rrect.y+
                                        pdata->enc_data->y_offset),
                                        mouse_pos_temp.width,
                                        mouse_pos_temp.height,
                                        (temp_brwin.rrect.width),
                                        pdata->specs.depth)
                }

            }
            if(pdata->args.xfixes_cursor){
                XFree(xcim);
                xcim=NULL;
            }
        }
        if(!pdata->args.full_shots){
            rmdClearList(&pdata->rect_root);
        }
        if (pdata->encoder_busy) {
            pdata->frames_lost++;
        }
        pthread_mutex_lock(&pdata->img_buff_ready_mutex);
        pthread_cond_broadcast(&pdata->image_buffer_ready);
        pthread_mutex_unlock(&pdata->img_buff_ready_mutex);
        pdata->capture_busy = FALSE;
    }

    if(!pdata->args.noframe){
        XDestroyWindow(pdata->dpy,pdata->shaped_w);
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

