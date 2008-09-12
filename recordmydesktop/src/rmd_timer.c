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

#include "recordmydesktop.h"


void *rmdTimer(ProgData *pdata){
    
    long unsigned int secs_tw=1/pdata->args.fps;
    long unsigned int usecs_tw=(1000000)/pdata->args.fps-
                               secs_tw*1000000;

    while(pdata->timer_alive){

        if(PauseStateChanged){
            PauseStateChanged=0;

            if(!Paused){
                Paused=1;
                fprintf(stderr,"STATE:PAUSED\n");fflush(stderr);
            }
            else{
                Paused=0;
                fprintf(stderr,"STATE:RECORDING\n");fflush(stderr);
                pthread_mutex_lock(&pause_mutex);
                pthread_cond_broadcast(pause_cond);
                pthread_mutex_unlock(&pause_mutex);
            }

        }

        if(!Paused){
            frames_total++;
            if(capture_busy){
                frames_lost++;
            }
        }
        
        pthread_mutex_lock(&time_mutex);
        pthread_cond_broadcast(time_cond);
        pthread_mutex_unlock(&time_mutex);
        

        if(secs_tw)
            sleep(secs_tw);
        usleep(usecs_tw);

    }


    pthread_exit(&errno);
}



