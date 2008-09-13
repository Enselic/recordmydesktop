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
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/keysym.h> 

#include "rmdtypes.h"
#include "shortcuts.h"

int RegisterShortcut(Display *dpy,
                     Window root,
                     const char *shortcut,
                     HotKey *hotkey){

    int keycode=0, 
        i, j ;
    KeySym key=0;
    unsigned int modifier_mask=0,
                 numlock_mask=0;
    char *keystr=NULL;
    XModifierKeymap *modmap;

    if(strstr(shortcut,"Shift"))
        modifier_mask = modifier_mask|ShiftMask;
    if(strstr(shortcut,"Control"))
        modifier_mask = modifier_mask|ControlMask;
    if(strstr(shortcut,"Mod1"))
        modifier_mask = modifier_mask|Mod1Mask;
    if(strstr(shortcut,"Mod2"))
        modifier_mask = modifier_mask|Mod2Mask;
    if(strstr(shortcut,"Mod3"))
        modifier_mask = modifier_mask|Mod3Mask;
    if(strstr(shortcut,"Mod4"))
        modifier_mask = modifier_mask|Mod4Mask;
    if(strstr(shortcut,"Mod5"))
        modifier_mask = modifier_mask|Mod5Mask;
    
    //we register the shortcut on the root
    //window, which means on every keypress event,
    //so I think it's neccessary to have at least one 
    //modifier.
    if(modifier_mask == 0)
        return 1;
    if((keystr=rindex(shortcut,'+'))!=NULL){
        keystr++;
        if((key=XStringToKeysym(keystr))==NoSymbol)
            return 1;
        else
            keycode=XKeysymToKeycode(dpy,key);
    }
    else
        return 1;


    /* Key grabbing stuff taken from tilda who took it from yeahconsole 
     * who took it from evilwm */

    modmap = XGetModifierMapping(dpy);
    for(i=0;i<8;i++){
        for(j=0;j<modmap->max_keypermod;j++){
            if(modmap->modifiermap[i*modmap->max_keypermod+j]==
               XKeysymToKeycode(dpy,XK_Num_Lock))
                numlock_mask=(1<<i);
        }
    }
    XFreeModifiermap(modmap);

    hotkey->modnum=0;
    hotkey->key=keycode;

    XGrabKey(dpy,
             keycode,
             modifier_mask,
             root,
             True,
             GrabModeAsync,
             GrabModeAsync);
    hotkey->mask[0]=modifier_mask;
    hotkey->modnum++;

    XGrabKey(dpy,
             keycode,
             LockMask | modifier_mask,
             root,
             True,
             GrabModeAsync,
             GrabModeAsync);
    hotkey->mask[1]=LockMask | modifier_mask;
    hotkey->modnum++;

    if(numlock_mask){

        XGrabKey(dpy,
                 keycode,
                 numlock_mask | modifier_mask,
                 root,
                 True,
                 GrabModeAsync,
                 GrabModeAsync);
        hotkey->mask[2]=numlock_mask | modifier_mask;
        hotkey->modnum++;

        XGrabKey(dpy,
                 keycode,
                 numlock_mask | LockMask | modifier_mask,
                 root,
                 True,
                 GrabModeAsync,
                 GrabModeAsync);
        hotkey->mask[3]=numlock_mask | LockMask | modifier_mask;
        hotkey->modnum++;

    }

    return 0;

}






