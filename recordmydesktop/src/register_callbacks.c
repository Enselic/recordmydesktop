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
void SetExpired(int signum){
    if(!Paused){
        frames_total++;
        if(capture_busy){
            frames_lost++;
        }
        pthread_mutex_lock(&time_mutex);
        pthread_cond_broadcast(time_cond);  //sig handlers should
                                            //not call this func
                                            //could be a set_expired
                                            // and main thread
                                            //doing a while(running)
                                            //if set_expired broadcast
                                            //else usleep(n)
        pthread_mutex_unlock(&time_mutex);
    }
}

void SetPaused(int signum){
    if(!Paused)
        Paused=1;
    else{
        Paused=0;
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

void CancelTimer(void){
    struct itimerval value;
    value.it_interval.tv_sec=
    value.it_value.tv_sec=
    value.it_interval.tv_usec=
    value.it_value.tv_usec=0;

    setitimer(ITIMER_REAL,&value,NULL);
}

void RegisterCallbacks(ProgArgs *args){

    struct itimerval value;
    struct sigaction time_act,pause_act,end_act;


    value.it_interval.tv_sec=value.it_value.tv_sec=0;
    value.it_interval.tv_usec=value.it_value.tv_usec=(1000000)/args->fps;
    setitimer(ITIMER_REAL,&value,NULL);
    time_act.sa_handler=SetExpired;
    pause_act.sa_handler=SetPaused;
    end_act.sa_handler=SetRunning;
    sigfillset(&(time_act.sa_mask));
    sigfillset(&(pause_act.sa_mask));
    sigfillset(&(end_act.sa_mask));
    time_act.sa_flags=pause_act.sa_flags=end_act.sa_flags=0;
    sigaction(SIGALRM,&time_act,NULL);
    sigaction(SIGUSR1,&pause_act,NULL);
    sigaction(SIGINT,&end_act,NULL);
    sigaction(SIGTERM,&end_act,NULL);
    sigaction(SIGABRT,&end_act,NULL);
}

