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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "rmdtypes.h"

#include "rmd_timer.h"


void *rmdTimer(ProgData *pdata){
    
    long unsigned int secs_tw=1/pdata->args.fps;
    long unsigned int usecs_tw=(1000000)/pdata->args.fps-
                               secs_tw*1000000;

    while(pdata->timer_alive){

        if (pdata->pause_state_changed) {
            pdata->pause_state_changed = FALSE;

            if (!pdata->paused) {
                pdata->paused = TRUE;
                fprintf(stderr,"STATE:PAUSED\n");fflush(stderr);
            }
            else{
                pdata->paused = FALSE;
                fprintf(stderr,"STATE:RECORDING\n");fflush(stderr);
                pthread_mutex_lock(&pdata->pause_mutex);
                pthread_cond_broadcast(&pdata->pause_cond);
                pthread_mutex_unlock(&pdata->pause_mutex);
            }

        }

        if (!pdata->paused) {
            pdata->frames_total++;
            if (pdata->capture_busy) {
                pdata->frames_lost++;
            }
        }
        
        pthread_mutex_lock(&pdata->time_mutex);
        pthread_cond_broadcast(&pdata->time_cond);
        pthread_mutex_unlock(&pdata->time_mutex);
        

        if(secs_tw)
            sleep(secs_tw);
        usleep(usecs_tw);

    }


    pthread_exit(&errno);
}



