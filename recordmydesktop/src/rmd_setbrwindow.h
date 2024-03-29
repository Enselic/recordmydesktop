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
******************************************************************************/

#ifndef SETBRWINDOW_H
#define SETBRWINDOW_H 1

#include "rmd_types.h"


/**
* Check and align window size
*
* \param dpy Connection to the server
*
* \param brwin BRWindow struct containing the initial and final window
*
* \param specs DisplaySpecs struct with
*              information about the display to be recorded
*
* \param args ProgArgs struct containing the user-set options
*
* \returns 0 on Success 1 on Failure
*/
int rmdSetBRWindow(Display *dpy,
                   BRWindow *brwin,
                   DisplaySpecs *specs,
                   ProgArgs *args);


#endif
