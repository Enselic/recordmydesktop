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
#include "rmd_register_callbacks.h"

#include "rmd_types.h"

#include <signal.h>


// There seem to be no way of passing user data to the signal handler,
// so hack around not being able to pass ProgData to them
static int *pdata_running             = NULL;
static int *pdata_paused              = NULL;
static int *pdata_aborted             = NULL;
static int *pdata_pause_state_changed = NULL;


static void SetPaused(int signum) {

    *pdata_pause_state_changed = TRUE;
}

static void SetRunning(int signum) {

    if (!*pdata_paused) {

        *pdata_running = FALSE;

        if (signum == SIGABRT) {
            *pdata_aborted = TRUE;
        }
    }
}

void RegisterCallbacks(ProgData *pdata) {

    struct sigaction pause_act;
    struct sigaction end_act;

    // Is there some way to pass pdata to the signal handlers?
    pdata_running             = &pdata->running;
    pdata_paused              = &pdata->paused;
    pdata_aborted             = &pdata->aborted;
    pdata_pause_state_changed = &pdata->pause_state_changed;
    
    // Setup pause_act
    sigfillset(&pause_act.sa_mask);
    pause_act.sa_flags   = SA_RESTART;
    pause_act.sa_handler = SetPaused;

    sigaction(SIGUSR1, &pause_act, NULL);

    // Setup end_act
    sigfillset(&end_act.sa_mask);
    end_act.sa_flags   = SA_RESTART;
    end_act.sa_handler = SetRunning;

    sigaction(SIGINT,  &end_act, NULL);
    sigaction(SIGTERM, &end_act, NULL);
    sigaction(SIGABRT, &end_act, NULL);
}
