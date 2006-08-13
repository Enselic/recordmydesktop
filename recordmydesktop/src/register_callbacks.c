/*********************************************************************************
*                             recordMyDesktop                                    *
**********************************************************************************
*                                                                                *
*             Copyright (C) 2006  John Varouhakis                                *
*                                                                                *
*                                                                                *
*    This program is free software; you can redistribute it and/or modify        *
*    it under the terms of the GNU General Public License as published by        *
*    the Free Software Foundation; either version 2 of the License, or           *
*    (at your option) any later version.                                         *
*                                                                                *
*    This program is distributed in the hope that it will be useful,             *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
*    GNU General Public License for more details.                                *
*                                                                                *
*    You should have received a copy of the GNU General Public License           *
*    along with this program; if not, write to the Free Software                 *
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA   *
*                                                                                *
*                                                                                *
*                                                                                *
*    For further information contact me at johnvarouhakis@gmail.com              *
**********************************************************************************/


#include <recordmydesktop.h> 
void SetExpired(int signum){
    pthread_cond_broadcast(time_cond);
}

void SetPaused(int signum){
    if(!Paused)
        Paused=1;
    else{
//         pthread_cond_broadcast(pause_cond);//thsi should work, but it doesn't
        int i;//this is a bug
        Paused=0;//normally with the broadcast all the threads should restart, but sound capture thread
        for(i=0;i<15;i++)//remains dead. If a bunch of signals, restarts all threads, why can't a broadcast do the same?
            pthread_cond_signal(pause_cond);//if you have any idea please contact me.
                                            //(misses the signal?)
    }
}


void SetRunning(int signum){
    *Running=0;
    if(signum==SIGABRT)
        Aborted=1;
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

