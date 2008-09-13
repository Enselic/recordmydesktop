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

#ifndef GETZPIXMAP_H
#define GETZPIXMAP_H 1

#include "rmdtypes.h"


/**
* Rerieve pixmap data from xserver
*
* \param dpy Connection to the server
*
* \param root root window of the display
*
* \param data (preallocated)buffer to place the data
*
* \param x x position of the screenshot
*
* \param y y position of the screenshot
*
* \param x x position of the screenshot
*
* \param width width of the screenshot
*
* \param height height position of the screenshot
*
* \returns 0 on Success 1 on Failure
*/
int GetZPixmap(Display *dpy,
               Window root,
               char *data,
               int x,
               int y,
               int width,
               int height);

/**
* Rerieve pixmap data from xserver through the MIT-Shm extension
*
* \param dpy Connection to the server
*
* \param root root window of the display
*
* \param shminfo Info for the shared memory segment
*
* \param shm_opcode Opcode of Shm extension
*
* \param data (preallocated)buffer to place the data
*
* \param x x position of the screenshot
*
* \param y y position of the screenshot
*
* \param x x position of the screenshot
*
* \param width width of the screenshot
*
* \param height height position of the screenshot
*
* \returns 0 on Success 1 on Failure
*/
int GetZPixmapSHM(Display *dpy,
                  Window root,
                  XShmSegmentInfo *shminfo,
                  int shm_opcode,
                  char *data,
                  int x,
                  int y,
                  int width,
                  int height);


#endif
