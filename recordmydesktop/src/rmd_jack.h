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

#ifndef RMD_JACK_H
#define RMD_JACK_H 1

#include "rmdtypes.h"


#ifdef HAVE_JACK_H

size_t (*jack_ringbuffer_read_p)(jack_ringbuffer_t *rb,
                                 char *dest, size_t cnt);
size_t (*jack_ringbuffer_read_space_p)(const jack_ringbuffer_t *rb);

/**
*   Load libjack, create and activate client,register ports
*
*   \param jdata_t  Pointer to JackData struct containing port
*                   and client information
*
*   \returns 0 on Success, error code on failure
*            (depending on where the error occured)
*/
int StartJackClient(JackData *jdata);

/**
*   Close Jack Client
*
*   \param jdata_t  Pointer to JackData struct containing port
*                   and client information
*
*   \returns 0 on Success, 1 on failure
*/
int StopJackClient(JackData *jdata);

#endif


#endif
