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
#include "rmd_queryextensions.h"

#include "rmd_types.h"

#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xdamage.h>
#include <stdlib.h>



void rmdQueryExtensions(Display *dpy,
                        ProgArgs *args,
                        int *damage_event,
                        int *damage_error,
                        int *shm_opcode){
    int xf_event_basep,
        xf_error_basep,
        shm_event_base,
        shm_error_base,
        shape_event_base,
        shape_error_base;

    if((!(args->full_shots))&&(!XDamageQueryExtension( dpy, damage_event, damage_error))){
        fprintf(stderr,"XDamage extension not found!!!\n"
                       "Try again using the --full-shots option, though\n"
                       "enabling XDamage is highly recommended,\n"
                       "for performance reasons.\n");
        exit(4);
    }
    if((!args->noshared)&&(!XQueryExtension(dpy,
                                           "MIT-SHM",
                                           shm_opcode,
                                           &shm_event_base,
                                           &shm_error_base))){
        args->noshared=1;
        fprintf(stderr,"Shared Memory extension not present!\n"
                       "Try again using the --no-shared option\n");
        exit(5);
    }
    if((args->xfixes_cursor)&&
       (XFixesQueryExtension(dpy,&xf_event_basep,&xf_error_basep)==False)){
        args->xfixes_cursor=0;
        fprintf(stderr,"Xfixes extension not present!\n"
                       "Please run with the -dummy-cursor or"
                       " --no-cursor option.\n");
        exit(6);
    }
    if((!args->noframe)&&
       (!XShapeQueryExtension(dpy,&shape_event_base,&shape_error_base))){
        fprintf(stderr,"XShape Not Found!!!\n"
                       "Frame won't be available.\n");

        args->noframe=1;
    }

}
