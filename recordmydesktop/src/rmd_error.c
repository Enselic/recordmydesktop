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


#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>

#include "rmdfunc.h"


int rmdErrorHandler( Display *dpy, XErrorEvent *e )
{
    char error_desc[1024];
    XGetErrorText(dpy,e->error_code,error_desc,sizeof(error_desc));
    fprintf(stderr,"X Error: %s\n",error_desc);
    fflush(stderr);
    if((e->error_code==BadWindow)&&(e->request_code==X_GetWindowAttributes)){
        fprintf(stderr,"BadWindow on XGetWindowAttributes.\nIgnoring...\n");
        fflush(stderr);
        return 0;
    }
    else if((e->error_code==BadAccess)&&(e->request_code==X_GrabKey)){
        fprintf(stderr,"Bad Access on XGrabKey.\n"
                        "Shortcut already assigned.\n");
        fflush(stderr);
        return 0;
    }
    else
        exit(1);
}



