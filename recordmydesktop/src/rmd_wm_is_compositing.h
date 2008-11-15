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

#ifndef RMD_WM_IS_COMPOSITING_H
#define RMD_WM_IS_COMPOSITING_H 1

#include "rmd_types.h"


/**
*Check if the window manager is compositing (duh!).
*
* \param dpy Connection to the server
*
* \param screen screen number/id that the window manager runs on
*
* \returns TRUE if compositing, false otherwise or when 
*          the window manager doesn't support the required
*          freedesktop.org hints for the test to be done 
*          succesfully.
*/

boolean rmdWMIsCompositing( Display *dpy, int screen) ; 

#endif
