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


#include <recordmydesktop.h>

char *rmdWMCheck(Display *dpy,Window root){

    Window  *wm_child=NULL;
    Atom    nwm_atom,
            utf8_string,
            wm_name_atom,
            rt;
    unsigned long   nbytes,
                    nitems;

    char *wm_name_str=NULL;
    int fmt;

    utf8_string = XInternAtom(dpy, "UTF8_STRING", False);

    nwm_atom =XInternAtom(dpy,"_NET_SUPPORTING_WM_CHECK",True);
    wm_name_atom =XInternAtom(dpy,"_NET_WM_NAME",True);

    if(nwm_atom!=None && wm_name_atom!=None){
        if(XGetWindowProperty(  dpy,root,nwm_atom,0,100,
                                False,XA_WINDOW,
                                &rt,&fmt,&nitems, &nbytes,
                                (unsigned char **)((void*)&wm_child))
                            != Success ){
            fprintf(stderr,"Error while trying to get a"
                           " window to identify the window manager.\n");
        }
        if((wm_child == NULL)||
           (XGetWindowProperty(dpy,*wm_child,wm_name_atom,0,100,
                               False,utf8_string,&rt,
                               &fmt,&nitems, &nbytes,
                               (unsigned char **)((void*)&wm_name_str))
            !=Success)){
            fprintf(stderr,"Warning!!!\nYour window manager appears"
                           " to be non-compliant!\n");
        }
    }
    fprintf(stderr,"Your window manager appears to be %s\n\n",
                    ((wm_name_str!=NULL)?wm_name_str:"Unknown"));


    return wm_name_str;
}

