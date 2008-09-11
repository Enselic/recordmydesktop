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
    #include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/extensions/Xdamage.h>
#include <pthread.h>
#include <rmdfunc.h>
#include <rmdtypes.h>
#include <rmdmacro.h>


#define CLIP_EVENT_AREA(e,brwin,wgeom){\
    if(((e)->area.x<=(brwin)->rgeom.x)&&((e)->area.y<=(brwin)->rgeom.y)&&\
        ((e)->area.width>=(brwin)->rgeom.width)&&\
        ((e)->area.height<(brwin)->rgeom.height)){\
        (wgeom)->x=(brwin)->rgeom.x;\
        (wgeom)->y=(brwin)->rgeom.y;\
        (wgeom)->width=(brwin)->rgeom.width;\
        (wgeom)->height=(brwin)->rgeom.height;\
    }\
    else{\
        (wgeom)->x=((((e)->area.x+(e)->area.width>=(brwin)->rgeom.x)&&\
        ((e)->area.x<=(brwin)->rgeom.x+(brwin)->rgeom.width))?\
        (((e)->area.x<=(brwin)->rgeom.x)?(brwin)->rgeom.x:(e)->area.x):-1);\
    \
        (wgeom)->y=((((e)->area.y+(e)->area.height>=(brwin)->rgeom.y)&&\
        ((e)->area.y<=(brwin)->rgeom.y+(brwin)->rgeom.height))?\
        (((e)->area.y<=(brwin)->rgeom.y)?(brwin)->rgeom.y:(e)->area.y):-1);\
    \
        (wgeom)->width=((e)->area.x<=(brwin)->rgeom.x)?\
        (e)->area.width-((brwin)->rgeom.x-(e)->area.x):\
        ((e)->area.x<=(brwin)->rgeom.x+(brwin)->rgeom.width)?\
        (((brwin)->rgeom.width-(e)->area.x+(brwin)->rgeom.x<(e)->area.width)?\
        (brwin)->rgeom.width-(e)->area.x+(brwin)->rgeom.x:e->area.width):-1;\
    \
        (wgeom)->height=((e)->area.y<=(brwin)->rgeom.y)?\
        (e)->area.height-((brwin)->rgeom.y-(e)->area.y):\
        ((e)->area.y<=(brwin)->rgeom.y+(brwin)->rgeom.height)?\
        (((brwin)->rgeom.height-(e)->area.y+\
         (brwin)->rgeom.y<(e)->area.height)?\
         (brwin)->rgeom.height-(e)->area.y+\
         (brwin)->rgeom.y:(e)->area.height):-1;\
    \
        if((wgeom)->width>(brwin)->rgeom.width)\
            (wgeom)->width=(brwin)->rgeom.width;\
        if((wgeom)->height>(brwin)->rgeom.height)\
            (wgeom)->height=(brwin)->rgeom.height;\
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
                             pdata->brwin.rgeom.width,
                             pdata->brwin.rgeom.height);
                
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
                WGeometry wgeom;
                CLIP_EVENT_AREA(e,&(pdata->brwin),&wgeom);
                if((wgeom.x>=0)&&(wgeom.y>=0)&&
                   (wgeom.width>0)&&(wgeom.height>0)){

                    inserts+=RectInsert(&pdata->rect_root,&wgeom);

                }
            }
        }

    }


}


