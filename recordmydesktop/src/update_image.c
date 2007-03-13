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

void UpdateImage(Display * dpy,
                yuv_buffer *yuv,
                pthread_mutex_t *yuv_mutex,
                DisplaySpecs *specs,
                RectArea **root,
                BRWindow *brwin,
                EncData *enc,
                char *datatemp,
                int noshmem,
                XShmSegmentInfo *shminfo,
                int shm_opcode,
                int no_quick_subsample){
    RectArea *temp;
    unsigned char *dtap=(unsigned char*)datatemp;
    temp=*root;

    if(temp!=NULL){
        do{
            if(noshmem){
                GetZPixmap( dpy,
                            specs->root,
                            datatemp,
                            temp->geom.x,
                            temp->geom.y,
                            temp->geom.width,
                            temp->geom.height);
            }
            else{
                GetZPixmapSHM(dpy,
                              specs->root,
                              shminfo,
                              shm_opcode,
                              datatemp,temp->geom.x,
                              temp->geom.y,
                              temp->geom.width,
                              temp->geom.height);
            }
            pthread_mutex_lock(yuv_mutex);
            UPDATE_YUV_BUFFER(yuv,dtap,
                                (temp->geom.x-brwin->rgeom.x+enc->x_offset),
                                (temp->geom.y-brwin->rgeom.y+enc->y_offset),
                                (temp->geom.width),(temp->geom.height),
                                no_quick_subsample,
                                specs->depth);
            pthread_mutex_unlock(yuv_mutex);
            temp=temp->next;
        }while(temp!=NULL);
    }
}

