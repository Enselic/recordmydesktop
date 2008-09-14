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

#ifndef RMD_FRAME_H
#define RMD_FRAME_H 1

#include "rmd_types.h"


/*
 * Create a frame that marks the recording area.
 *
 * \param dpy Connection to the X Server
 *
 * \param screen Recorded screen
 *
 * \param root Root window of the display
 *
 * \param x X pos of the recorded area
 *
 * \param y Y pos of the recorded area
 *
 * \param width Width of the recorded area
 *
 * \param height Height of the recorded area
 *
 * \returns The WindowID of the frame
 *
 */
Window rmdFrameInit(Display *dpy,
                    int screen,
                    Window root,
                    int x,
                    int y,
                    int width,
                    int height);


/*
 * Move the frame (subtracts the borderwidth)
 *
 * \param dpy Connection to the X Server
 *
 * \param win WindowId of the frame
 *
 * \param x New X pos of the recorded area
 *
 * \param y New Y pos of the recorded area
 *
 */
void rmdMoveFrame(Display *dpy,
                  Window win,
                  int x,
                  int y);


/*
 * Redraw the frame that marks the recording area.
 *
 * \param dpy Connection to the X Server
 *
 * \param screen Recorded screen
 *
 * \param win WindoID of the frame
 *
 * \param width Width of the recorded area
 *
 * \param height Height of the recorded area
 *
 */
void rmdDrawFrame(Display *dpy,
                  int screen,
                  Window win,
                  int width,
                  int height);


#endif
