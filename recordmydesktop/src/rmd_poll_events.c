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

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/extensions/Xdamage.h>
#include <signal.h>
#include <pthread.h>

#include "rmd_frame.h"
#include "rmd_macro.h"
#include "rmd_poll_events.h"
#include "rmd_rectinsert.h"
#include "rmd_types.h"


#define CLIP_EVENT_AREA(e,brwin,xrect){\
    if(((e)->area.x<=(brwin)->rrect.x)&&((e)->area.y<=(brwin)->rrect.y)&&\
        ((e)->area.width>=(brwin)->rrect.width)&&\
        ((e)->area.height<(brwin)->rrect.height)){\
        (xrect)->x=(brwin)->rrect.x;\
        (xrect)->y=(brwin)->rrect.y;\
        (xrect)->width=(brwin)->rrect.width;\
        (xrect)->height=(brwin)->rrect.height;\
    }\
    else{\
        (xrect)->x=((((e)->area.x+(e)->area.width>=(brwin)->rrect.x)&&\
        ((e)->area.x<=(brwin)->rrect.x+(brwin)->rrect.width))?\
        (((e)->area.x<=(brwin)->rrect.x)?(brwin)->rrect.x:(e)->area.x):-1);\
    \
        (xrect)->y=((((e)->area.y+(e)->area.height>=(brwin)->rrect.y)&&\
        ((e)->area.y<=(brwin)->rrect.y+(brwin)->rrect.height))?\
        (((e)->area.y<=(brwin)->rrect.y)?(brwin)->rrect.y:(e)->area.y):-1);\
    \
        (xrect)->width=((e)->area.x<=(brwin)->rrect.x)?\
        (e)->area.width-((brwin)->rrect.x-(e)->area.x):\
        ((e)->area.x<=(brwin)->rrect.x+(brwin)->rrect.width)?\
        (((brwin)->rrect.width-(e)->area.x+(brwin)->rrect.x<(e)->area.width)?\
        (brwin)->rrect.width-(e)->area.x+(brwin)->rrect.x:e->area.width):0;\
    \
        (xrect)->height=((e)->area.y<=(brwin)->rrect.y)?\
        (e)->area.height-((brwin)->rrect.y-(e)->area.y):\
        ((e)->area.y<=(brwin)->rrect.y+(brwin)->rrect.height)?\
        (((brwin)->rrect.height-(e)->area.y+\
         (brwin)->rrect.y<(e)->area.height)?\
         (brwin)->rrect.height-(e)->area.y+\
         (brwin)->rrect.y:(e)->area.height):0;\
    \
        if((xrect)->width>(brwin)->rrect.width)\
            (xrect)->width=(brwin)->rrect.width;\
        if((xrect)->height>(brwin)->rrect.height)\
            (xrect)->height=(brwin)->rrect.height;\
    }\
}


void InitEventsPolling(ProgData *pdata){
    Window root_return,
           parent_return,
           *children;
    unsigned int i,
                 nchildren;


    XSelectInput (pdata->dpy,pdata->specs.root, SubstructureNotifyMask);


    if(!pdata->args.full_shots){
        XQueryTree (pdata->dpy,
                    pdata->specs.root,
                    &root_return,
                    &parent_return,
                    &children,
                    &nchildren);

        for (i = 0; i < nchildren; i++){
            XWindowAttributes attribs;
            if (XGetWindowAttributes (pdata->dpy,children[i],&attribs)){
                if(!attribs.override_redirect && 
                   attribs.depth==pdata->specs.depth)
                    XDamageCreate(pdata->dpy,
                                  children[i],
                                  XDamageReportRawRectangles);
            }
        }
        XFree(children);
        XDamageCreate(pdata->dpy,
                      pdata->specs.root,
                      XDamageReportRawRectangles);
    }



}


void EventLoop(ProgData *pdata){
    int inserts=0;

    XEvent event;

    while(XPending(pdata->dpy)){
        XNextEvent(pdata->dpy,&event);
        if(event.type == KeyPress){
            XKeyEvent *e=(XKeyEvent *)(&event);
            if(e->keycode == pdata->pause_key.key){
                int i=0;
                int found=0;
                for(i=0;i<pdata->pause_key.modnum;i++){
                    if(pdata->pause_key.mask[i]==e->state){
                        found=1;
                        break;
                    }
                }
                if(found){
                    raise(SIGUSR1);
                    continue;
                }
            }
            if(e->keycode == pdata->stop_key.key){
                int i=0;
                int found=0;
                for(i=0;i<pdata->stop_key.modnum;i++){
                    if(pdata->stop_key.mask[i]==e->state){
                        found=1;
                        break;
                    }
                }
                if(found){
                    raise(SIGINT);
                    continue;
                }
            }
        }
        else if(event.type == Expose){
            
            if(event.xexpose.count!=0)
                continue;
            else if(!pdata->args.noframe){
                rmdDrawFrame(pdata->dpy,
                             pdata->specs.screen,  
                             pdata->shaped_w,
                             pdata->brwin.rrect.width,
                             pdata->brwin.rrect.height);
                
            }

        }
        else if(!pdata->args.full_shots){
            if(event.type == MapNotify ){
                XWindowAttributes attribs;
                if (!((XMapEvent *)(&event))->override_redirect&&
                    XGetWindowAttributes(pdata->dpy,
                                         event.xcreatewindow.window,
                                         &attribs)){
                    if(!attribs.override_redirect&&
                       attribs.depth==pdata->specs.depth)
                        XDamageCreate(pdata->dpy,
                                      event.xcreatewindow.window,
                                      XDamageReportRawRectangles);
                }
            }
            else if(event.type == pdata->damage_event + XDamageNotify ){
                XDamageNotifyEvent *e =(XDamageNotifyEvent *)( &event );
                XRectangle xrect;
                CLIP_EVENT_AREA(e,&(pdata->brwin),&xrect);
                if((xrect.x>=0)&&(xrect.y>=0)&&
                   (xrect.width>0)&&(xrect.height>0)){

                    inserts+=RectInsert(&pdata->rect_root,&xrect);

                }
            }
        }

    }


}


