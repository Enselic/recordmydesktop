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


#ifndef RECORDMYDESKTOP_H
#define RECORDMYDESKTOP_H 1

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

//header inclusion is completely fucked up
//I'll fix it, I promise
#include "rmdtypes.h"

//These are the cache blocks. They need to be accesible in the
//dbuf macros
u_int32_t   *yblocks,
            *ublocks,
            *vblocks;

#include "rmdmacro.h"
#include "rmdfunc.h"



/**Globals*/
//I've read somewhere that I'll go to hell for using globals...

int Paused,*Running,Aborted,PauseStateChanged;
pthread_cond_t  *time_cond,*pause_cond;
pthread_mutex_t pause_mutex,time_mutex;
unsigned char   Yr[256],Yg[256],Yb[256],
                Ur[256],Ug[256],UbVr[256],
                Vg[256],Vb[256];
//the following values are of no effect
//but they might be usefull later for profiling
unsigned int    frames_total,   //frames calculated by total time expirations
                frames_lost;    //the value of shame



//used to determine frame drop which can
//happen on failure to receive a signal over a condition variable
int capture_busy,
    encoder_busy;

#endif

