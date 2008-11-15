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

#include <X11/Xatom.h>

#include "rmd_types.h"

#include "rmd_wm_check.h"

#include "rmd_wm_is_compositing.h"

boolean rmdWMIsCompositing( Display *dpy, int screen ) {
    
    Window win;
    Atom atom;
    char buf[32];
    char *window_manager=rmdWMCheck( dpy, 
                                     RootWindow( dpy, screen ) );
    
    //If the wm name is queried successfully the wm is compliant (source  
    //http://standards.freedesktop.org/wm-spec/1.4/ar01s03.html#id2568282 )
    //in which case we will also free() the allcoated string.
    
    if( window_manager == NULL )
        return FALSE;
    else
        free( window_manager ); 


    snprintf( buf, sizeof(buf), "_NET_WM_CM_S%d", screen);
    atom = XInternAtom(dpy, buf, True);
    if (atom == None) return FALSE;

    win = XGetSelectionOwner(dpy, atom);

    return win != None;



}

