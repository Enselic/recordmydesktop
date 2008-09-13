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

#ifndef INITIALIZE_DATA_H
#define INITIALIZE_DATA_H 1

#include "rmdtypes.h"


/**
* initialize lists,mutexes,image buffers, take first screenshot,
* and anything else needed before launching the capture threads.
*
* \param pdata ProgData struct containing all program data
*
* \param enc_data reference to enc_data structure
*
* \param cache_data reference to cache_data structure
*
* \returns 0 on success, other values must cause the program to exit
*/
int InitializeData(ProgData *pdata,
                   EncData *enc_data,
                   CacheData *cache_data);

/**
* Sets up the ProgArgs structure to default values.
*/
void SetupDefaultArgs(ProgArgs *args);

/**
*   Currently only frees some memory
*   (y,u,v blocks)
*
*/
void CleanUp(void);


#endif
