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

#ifndef SHORTCUTS_H
#define CHORTCUTS_H 1

#include "rmdtypes.h"


/*
 * Check a shortcut combination and if valid, 
 * register it, on the root window.
 *
 * \param dpy Connection to the X Server
 *
 * \param root Root window id
 *
 * \param shortcut String represantation of the shortcut
 *
 * \param HotKey Pre-allocated struct that is filled with the 
 *               modifiers and the keycode, for checks on incoming 
 *               keypress events. Left untouched if the call fails.
 *
 *  
 *  \returns 0 on Success, 1 on Failure. 
 *
 */
int RegisterShortcut(Display *dpy,
                     Window root,
                     const char *shortcut,
                     HotKey *hotkey);


#endif
