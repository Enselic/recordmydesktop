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
#include "rmd_encode_cache.h"

#include "rmd_flush_to_ogg.h"
#include "rmd_init_encoder.h"
#include "rmd_load_cache.h"
#include "rmd_types.h"

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>



void rmdEncodeCache(ProgData *pdata){
    pthread_t   flush_to_ogg_t,
                load_cache_t;
    fprintf(stderr,"STATE:ENCODING\n");fflush(stderr);
    fprintf(stderr,"Encoding started!\nThis may take several minutes.\n"
    "Pressing Ctrl-C will cancel the procedure"
    " (resuming will not be possible, but\n"
    "any portion of the video, which is already encoded won't be deleted).\n"
    "Please wait...\n");
    pdata->running = TRUE;
    rmdInitEncoder(pdata,pdata->enc_data,1);
    //load encoding and flushing threads
    if(!pdata->args.nosound){
        //before we start loading again
        //we need to free any left-overs
        while(pdata->sound_buffer!=NULL){
            free(pdata->sound_buffer->data);
            pdata->sound_buffer=pdata->sound_buffer->next;
        }
    }
    pthread_create(&flush_to_ogg_t,NULL,(void *)rmdFlushToOgg,(void *)pdata);

    //start loading image and audio
    pthread_create(&load_cache_t,NULL,(void *)rmdLoadCache,(void *)pdata);

    //join and finish
    pthread_join(load_cache_t,NULL);
    fprintf(stderr,"Encoding finished!\nWait a moment please...\n");
    pthread_join(flush_to_ogg_t,NULL);

}


