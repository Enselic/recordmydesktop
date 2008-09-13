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

#ifndef ENCODE_IMAGE_BUFFER_H
#define ENCODE_IMAGE_BUFFER_H 1

#include "rmdtypes.h"


/**
* feed a yuv buffer to the theora encoder and submit outcome to
* the ogg stream.
* \param pdata ProgData struct containing all program data
*/
void *EncodeImageBuffer(ProgData *pdata);

/**
* As EncodeImageBuffer, only with the assumption that
* this is not a thread on it's own
*
* \param pdata ProgData struct containing all program data
*
*/
void SyncEncodeImageBuffer(ProgData *pdata);


#endif
