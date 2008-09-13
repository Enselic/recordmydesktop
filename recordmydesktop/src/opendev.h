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

#ifndef OPENDEV_H
#define OPENDEV_H 1

#include "rmdtypes.h"


#ifdef HAVE_LIBASOUND
/**
* Try to open (alsa) sound device, with the desired parameters,
* and place the obtained ones on their place
*
* \param pcm_dev name of the device
*
* \param channels desired number of channels
*                 (gets modified with the acieved value)
*
* \param frequency desired frequency(gets modified with the acieved value)
*
* \param buffsize Size of buffer
*
* \param periodsize Size of a period(can be NULL)
*
* \param periodtime Duration of a period(can be NULL)
*
* \param hardpause Set to 1 when the device has to be stopped during pause
*                  and to 0 when it supports pausing
*                  (can be NULL)
*
* \returns snd_pcm_t handle on success, NULL on failure
*/
snd_pcm_t *OpenDev( const char *pcm_dev,
                    unsigned int *channels,
                    unsigned int *frequency,
                    snd_pcm_uframes_t *buffsize,
                    snd_pcm_uframes_t *periodsize,
                    unsigned int *periodtime,
                    int *hardpause);
#else
/**
* Try to open (OSS) sound device, with the desired parameters.
*
*
* \param pcm_dev name of the device
*
* \param channels desired number of channels
*
* \param frequency desired frequency
*
*
* \returns file descriptor of open device,-1 on failure
*/
int OpenDev( const char *pcm_dev,
             unsigned int channels,
             unsigned int frequency);
#endif


#endif
