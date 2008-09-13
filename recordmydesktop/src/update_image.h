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

#ifndef UPDATE_IMAGE_H
#define UPDATE_IMAGE_H 1

#include "rmdtypes.h"


/**
* Retrieve and apply all changes, if xdamage is used.
*
* \param dpy Connection to the server
*
* \param yuv yuv_buffer that is to be modified
*
* \param specs DisplaySpecs struct with
*              information about the display to be recorded
*
* \param root Root entry of the list with damaged areas
*
* \param brwin BRWindow struct contaning the recording window specs
*
* \param enc Encoding options
*
* \param datatemp Buffer for pixel data to be
*                 retrieved before placed on the yuv buffer
*
* \param noshmem don't use MIT_Shm extension
*
* \param no_quick_subsample Don't do quick subsampling
*
*/
void UpdateImage(Display * dpy,
                yuv_buffer *yuv,
                DisplaySpecs *specs,
                RectArea **root,
                BRWindow *brwin,
                EncData *enc,
                char *datatemp,
                int noshmem,
                XShmSegmentInfo *shminfo,
                int shm_opcode,
                int no_quick_subsample);


#endif
