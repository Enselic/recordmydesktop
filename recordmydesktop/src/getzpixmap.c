/******************************************************************************
*                            recordMyDesktop                                  *
*******************************************************************************
*                                                                             *
*            Copyright (C) 2006,2007 John Varouhakis                          *
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

int GetZPixmap(Display *dpy,
               Window root,
               char *data,
               int x,
               int y,
               int width,
               int height){
    xGetImageReply reply;
    xGetImageReq *request;
    long nbytes;

    LockDisplay(dpy);
    GetReq(GetImage,request);
    request->drawable=root;
    request->x=x;
    request->y=y;
    request->width=width;
    request->height=height;
    request->planeMask=AllPlanes;
    request->format=ZPixmap;
    if((!_XReply(dpy,(xReply *)&reply,0,xFalse))||(!reply.length)){
        UnlockDisplay(dpy);
        SyncHandle();
        return 1;
    }
    nbytes=(long)reply.length<<2;
    _XReadPad(dpy,data,nbytes);
    UnlockDisplay(dpy);
    SyncHandle();
    return 0;
}

