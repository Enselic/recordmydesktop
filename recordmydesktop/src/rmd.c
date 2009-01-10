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

#include "rmd_cache.h"
#include "rmd_encode_cache.h"
#include "rmd_error.h"
#include "rmd_initialize_data.h"
#include "rmd_parseargs.h"
#include "rmd_queryextensions.h"
#include "rmd_rescue.h"
#include "rmd_setbrwindow.h"
#include "rmd_shortcuts.h"
#include "rmd_threads.h"
#include "rmd_wm_is_compositing.h"
#include "rmd_types.h"

#include <X11/Xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc,char **argv){
    ProgData pdata;
    int exit_status = 0;
    
    rmdSetupDefaultArgs(&pdata.args);

    if (!rmdParseArgs(argc, argv, &pdata.args)) {
        exit(1);
    }
    if (pdata.args.rescue_path != NULL) {
        exit(rmdRescue(pdata.args.rescue_path));
    }
    if(XInitThreads ()==0){
        fprintf(stderr,"Couldn't initialize thread support!\n");
        exit(7);
    }
    if(pdata.args.display!=NULL){
        pdata.dpy = XOpenDisplay(pdata.args.display);
        XSetErrorHandler(rmdErrorHandler);
    }
    else{
        fprintf(stderr,"No display specified for connection!\n");
        exit(8);
    }
    if (pdata.dpy == NULL) {
        fprintf(stderr, "Cannot connect to X server %s\n",pdata.args.display);
        exit(9);
    }
    else{
        EncData enc_data;
        CacheData cache_data;
#ifdef HAVE_LIBJACK
        JackData jdata;

        // Give jack access to program data, mainly for program state
        jdata.pdata = &pdata;
        pdata.jdata = &jdata;
#endif

        // Query display specs
        pdata.specs.screen = DefaultScreen(pdata.dpy);
        pdata.specs.width  = DisplayWidth(pdata.dpy, pdata.specs.screen);
        pdata.specs.height = DisplayHeight(pdata.dpy, pdata.specs.screen);
        pdata.specs.root   = RootWindow(pdata.dpy, pdata.specs.screen);
        pdata.specs.visual = DefaultVisual(pdata.dpy, pdata.specs.screen);
        pdata.specs.gc     = DefaultGC(pdata.dpy, pdata.specs.screen);
        pdata.specs.depth  = DefaultDepth(pdata.dpy, pdata.specs.screen);

        if((pdata.specs.depth!=32)&&
           (pdata.specs.depth!=24)&&
           (pdata.specs.depth!=16)){
            fprintf(stderr,"Only 32bpp,24bpp and 16bpp"
                           " color depth modes are currently supported.\n");
            exit(10);
        }
        if (!rmdSetBRWindow(pdata.dpy, &pdata.brwin, &pdata.specs, &pdata.args))
            exit(11);

        if( !pdata.args.nowmcheck && 
            rmdWMIsCompositing( pdata.dpy, pdata.specs.screen ) ) {

            fprintf(stderr,"\nDetected compositing window manager.\n"
                           "Reverting to full screen capture at every frame.\n"
                           "To disable this check run with --no-wm-check\n"
                           "(though that is not advised, since it will "
                           "probably produce faulty results).\n\n");
            pdata.args.full_shots=1;
            pdata.args.noshared=0;
        
        }

        rmdQueryExtensions(pdata.dpy,
                           &pdata.args,
                           &pdata.damage_event,
                           &pdata.damage_error,
                           &pdata.shm_opcode);


        if((exit_status=rmdInitializeData(&pdata,&enc_data,&cache_data))==0){

            if(!strcmp(pdata.args.pause_shortcut,
                      pdata.args.stop_shortcut)||
                rmdRegisterShortcut(pdata.dpy,
                                    pdata.specs.root,
                                    pdata.args.pause_shortcut,
                                    &(pdata.pause_key)) ||
                rmdRegisterShortcut(pdata.dpy,
                                    pdata.specs.root,
                                    pdata.args.stop_shortcut,
                                    &(pdata.stop_key))){

                fprintf(stderr,"Invalid shortcut,"
                               " or shortcuts are the same!\n\n"
                               "Using defaults.\n");

                rmdRegisterShortcut(pdata.dpy,
                                    pdata.specs.root,
                                    "Control+Mod1+p",
                                    &(pdata.pause_key));
                rmdRegisterShortcut(pdata.dpy,
                                    pdata.specs.root,
                                    "Control+Mod1+s",
                                    &(pdata.stop_key));
            }

            //this is where the capturing happens.
            rmdThreads(&pdata);

            XCloseDisplay(pdata.dpy);
            fprintf(stderr,".\n");

            //encode and then cleanup cache
            if(!pdata.args.encOnTheFly && !pdata.args.no_encode){
                if (!pdata.aborted) {
                    rmdEncodeCache(&pdata);
                }
                fprintf(stderr,"Cleanning up cache...\n");
                if(rmdPurgeCache(pdata.cache_data,!pdata.args.nosound))
                    fprintf(stderr,"Some error occured "
                                "while cleaning up cache!\n");
                fprintf(stderr,"Done!!!\n");
            }


            if (pdata.aborted && pdata.args.encOnTheFly) {
                if(remove(pdata.args.filename)){
                    perror("Error while removing file:\n");
                    return 1;
                }
                else{
                    fprintf(stderr,"SIGABRT received,file %s removed\n",
                                pdata.args.filename);
                    return 0;
                }
            }
            else
                fprintf(stderr,"Goodbye!\n");


            rmdCleanUp();
        }
    }

    return exit_status;
}
