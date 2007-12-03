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

void SetPaused(int signum){
    if(!Paused){
        Paused=1;
        fprintf(stderr,"STATE:PAUSED\n");
    }
    else{
        Paused=0;
        fprintf(stderr,"STATE:RECORDING\n");
/*FIXME */
//This is not safe.
//cond_var signaling must move away from signal handlers
//alltogether (JackCapture, SetExpired, SetPaused).
//Better would be a set of pipes for each of these.
//The callback should write on the pipe and the main thread
//should perform a select over the fd's, signaling afterwards the
//appropriate cond_var.
        pthread_mutex_lock(&pause_mutex);
        pthread_cond_broadcast(pause_cond);
        pthread_mutex_unlock(&pause_mutex);
    }
}


void SetRunning(int signum){
    *Running=0;
    if(signum==SIGABRT)
        Aborted=1;
}


void RegisterCallbacks(ProgArgs *args){

    struct sigaction pause_act,end_act;
    
    pause_act.sa_handler=SetPaused;
    end_act.sa_handler=SetRunning;
    sigfillset(&(pause_act.sa_mask));
    sigfillset(&(end_act.sa_mask));
    pause_act.sa_flags=end_act.sa_flags=SA_RESTART;
    sigaction(SIGUSR1,&pause_act,NULL);
    sigaction(SIGINT,&end_act,NULL);
    sigaction(SIGTERM,&end_act,NULL);
    sigaction(SIGABRT,&end_act,NULL);
}

