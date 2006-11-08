/*********************************************************************************
*                             recordMyDesktop                                    *
**********************************************************************************
*                                                                                *
*             Copyright (C) 2006  John Varouhakis                                *
*                                                                                *
*                                                                                *
*    This program is free software; you can redistribute it and/or modify        *
*    it under the terms of the GNU General Public License as published by        *
*    the Free Software Foundation; either version 2 of the License, or           *
*    (at your option) any later version.                                         *
*                                                                                *
*    This program is distributed in the hope that it will be useful,             *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
*    GNU General Public License for more details.                                *
*                                                                                *
*    You should have received a copy of the GNU General Public License           *
*    along with this program; if not, write to the Free Software                 *
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA   *
*                                                                                *
*                                                                                *
*                                                                                *
*    For further information contact me at johnvarouhakis@gmail.com              *
**********************************************************************************/


#include <recordmydesktop.h>

void *PollDamage(void *pdata){
    Window root_return,
           parent_return,
           *children;
    unsigned int i,
                 nchildren,
                 inserts=0;
    XEvent event;

    XSelectInput (((ProgData *)pdata)->dpy,((ProgData *)pdata)->specs.root, SubstructureNotifyMask);

    XQueryTree (((ProgData *)pdata)->dpy,
                ((ProgData *)pdata)->specs.root,
                &root_return,
                &parent_return,
                &children,
                &nchildren);

    for (i = 0; i < nchildren; i++){
        XWindowAttributes attribs;
        if (XGetWindowAttributes (((ProgData *)pdata)->dpy,children[i],&attribs)){
            if (!attribs.override_redirect && attribs.depth==((ProgData *)pdata)->specs.depth)
                XDamageCreate (((ProgData *)pdata)->dpy, children[i],XDamageReportRawRectangles);
        }
    }

    XDamageCreate( ((ProgData *)pdata)->dpy, ((ProgData *)pdata)->brwin.windowid, XDamageReportRawRectangles);


    while(((ProgData *)pdata)->running){
        //damage polling doesn't stop,eventually full image may be needed
        //30/10/2006 : when and why did I write the above line? what did I mean?
        XNextEvent(((ProgData *)pdata)->dpy,&event);
        if (event.type == MapNotify ){
            XWindowAttributes attribs;
            if (!((XMapEvent *)(&event))->override_redirect && XGetWindowAttributes (((ProgData *)pdata)->dpy,
                                        event.xcreatewindow.window,
                                        &attribs)){
                if (!attribs.override_redirect && attribs.depth==((ProgData *)pdata)->specs.depth)
                    XDamageCreate (((ProgData *)pdata)->dpy,event.xcreatewindow.window,XDamageReportRawRectangles);
            }
        }
        else if(event.type == ((ProgData *)pdata)->damage_event + XDamageNotify ){
            XDamageNotifyEvent *e =(XDamageNotifyEvent *)( &event );
            WGeometry wgeom;
            CLIP_EVENT_AREA(e,&(((ProgData *)pdata)->brwin),&wgeom);
            if((wgeom.x>=0)&&(wgeom.y>=0)&&(wgeom.width>0)&&(wgeom.height>0))
            {
                int tlist_sel=((ProgData *)pdata)->list_selector;
                pthread_mutex_lock(&((ProgData *)pdata)->list_mutex[tlist_sel]);
                inserts+=RectInsert(&((ProgData *)pdata)->rect_root[tlist_sel],&wgeom);
                pthread_mutex_unlock(&((ProgData *)pdata)->list_mutex[tlist_sel]);
            }
        }

    }
    pthread_exit(&errno);
}

