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

#ifndef RMD_CACHE_H
#define RMD_CACHE_H 1

#include "rmd_types.h"

#include <zlib.h>

#include <stdio.h>



/**
* Change file pointer to a new file while writting
* (file name is incremented with CacheFileN)
*
* \param name base file name
*
* \param n number to be used as a postfix
*
* \param fp File pointer if compression is used(must be NULL otherwise)
*
* \param ucfp File pointer if compression is NOT used(must be NULL otherwise)
*
* \returns 0 on Success 1 on Failure
*/
int SwapCacheFilesWrite(char *name,int n,gzFile **fp,FILE **ucfp);

/**
* Change file pointer to a new file while reading
* (file name is incremented with CacheFileN)
*
* \param name base file name
*
* \param n number to be used as a postfix
*
* \param fp File pointer if compression is used(must be NULL otherwise)
*
* \param ucfp File pointer if compression is NOT used(must be NULL otherwise)
*
* \returns 0 on Success 1 on Failure
*/
int SwapCacheFilesRead(char *name,int n,gzFile **fp,FILE **ucfp);

/**
* Delete all cache files
*
* \param cache_data_t Caching options(file names etc.)
*
* \returns 0 if all files and folders where deleted, 1 otherwise
*/
int PurgeCache(CacheData *cache_data_t,int sound);

/**
* Initializes paths and everything else needed to start caching
*
* \param pdata ProgData struct containing all program data
*
* \param enc_data_t Encoding options
*
* \param cache_data_t Caching options
*
*/
void InitCacheData(ProgData *pdata,
                   EncData *enc_data_t,
                   CacheData *cache_data_t);


#endif
