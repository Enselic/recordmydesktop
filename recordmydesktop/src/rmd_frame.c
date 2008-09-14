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
#include <X11/extensions/shape.h>

#include "rmd_frame.h"


#define BORDER_WIDTH 6
#define OUTLINE_WIDTH 1


void rmdDrawFrame(Display *dpy,
                  int screen,  
                  Window win,
                  int width,
                  int height){

    GC gc;
    XGCValues gcv;
    XColor white, white_e,
           black, black_e ;
    unsigned long gcmask=GCForeground;

    XAllocNamedColor(dpy,DefaultColormap(dpy, screen),"white", &white, &white_e);
    XAllocNamedColor(dpy,DefaultColormap(dpy, screen),"black", &black, &black_e);

    gcv.foreground = black.pixel;
    gc = XCreateGC(dpy,win, gcmask,&gcv);
    XFillRectangle(dpy,
                   win,
                   gc,
                   OUTLINE_WIDTH,
                   OUTLINE_WIDTH,
                   width+(BORDER_WIDTH-OUTLINE_WIDTH)*2,
                   height+(BORDER_WIDTH-OUTLINE_WIDTH)*2);
    gcv.foreground = white.pixel;
    XChangeGC(dpy,gc,gcmask,&gcv);
    XFillRectangle(dpy,
                   win,
                   gc,
                   BORDER_WIDTH-OUTLINE_WIDTH,
                   BORDER_WIDTH-OUTLINE_WIDTH,
                   width+OUTLINE_WIDTH*2,
                   height+OUTLINE_WIDTH*2);

    XFreeGC(dpy, gc);

}


void rmdMoveFrame(Display *dpy,
                  Window win,
                  int x,
                  int y){

    XMoveWindow(dpy,
                win,
                x-BORDER_WIDTH,
                y-BORDER_WIDTH);
    
//    XSync(pdata->dpy,False);

}

Window rmdFrameInit(Display *dpy,
                    int screen,
                    Window root,
                    int x,
                    int y,
                    int width,
                    int height){

    XSetWindowAttributes attribs;
    XColor white, white_e;
    Window win;
    unsigned long valuemask=CWBackPixmap|CWBackPixel|
                            CWSaveUnder|CWOverrideRedirect|CWColormap;

    XAllocNamedColor(dpy,DefaultColormap(dpy, screen),"white", &white, &white_e);

    attribs.background_pixmap=None;
    attribs.background_pixel=white.pixel;
    attribs.save_under=True;
    attribs.override_redirect=True;
    attribs.colormap=DefaultColormap(dpy,screen);

    win = XCreateWindow(dpy,
                        root,
                        x-BORDER_WIDTH,
                        y-BORDER_WIDTH,
                        width+BORDER_WIDTH*2,
                        height+BORDER_WIDTH*2,
                        0,
                        CopyFromParent,
                        InputOutput,
                        CopyFromParent,
                        valuemask,
                        &attribs);


    XRectangle rect;
    rect.x=rect.y=BORDER_WIDTH;
    rect.width=width;
    rect.height=height;

    XShapeCombineRectangles(dpy,
                            win,
                            ShapeBounding,
                            0,
                            0,
                            &rect,
                            1,
                            ShapeSubtract,
                            0);

    XMapWindow(dpy, win);

    rmdDrawFrame(dpy,screen,win,width,height);

    return win;
}








