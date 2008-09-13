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


#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#include "rmdtypes.h"
#include "specsfile.h"


int WriteSpecsFile(ProgData *pdata){
    
    FILE *fp;


    fp=fopen(pdata->cache_data->specsfile,"wb");
    if(fp==NULL)
        return 1;
    else{

        fprintf(fp,"recordMyDesktop = %s\n",VERSION);
        fprintf(fp,"Width = %d\n",pdata->brwin.rgeom.width);
        fprintf(fp,"Height = %d\n",pdata->brwin.rgeom.height);
        fprintf(fp,"Filename = %s\n",pdata->args.filename);
        fprintf(fp,"FPS = %f\n",pdata->args.fps);
        fprintf(fp,"NoSound = %d\n",pdata->args.nosound);
        fprintf(fp,"Frequency = %d\n",pdata->args.frequency);
        fprintf(fp,"Channels = %d\n",pdata->args.channels);
        fprintf(fp,"BufferSize = %lu\n",pdata->args.buffsize);
        fprintf(fp,"SoundFrameSize = %d\n",pdata->sound_framesize);
        fprintf(fp,"PeriodSize = %lu\n",pdata->periodsize);
        fprintf(fp,"UsedJack = %d\n",pdata->args.use_jack);
        fprintf(fp,"v_bitrate = %d\n",pdata->args.v_bitrate);
        fprintf(fp,"v_quality = %d\n",pdata->args.v_quality);
        fprintf(fp,"s_quality = %d\n",pdata->args.s_quality);
        fprintf(fp,"ZeroCompression = %d\n",pdata->args.zerocompression);
        
    
    }

    fclose(fp);
    return 0;

}

int ReadSpecsFile(ProgData *pdata){


    FILE *fp;


    fp=fopen(pdata->cache_data->specsfile,"rb");
    if(fp==NULL)
        return 1;
    else{
    
        char Cached_Version[256];
        free(pdata->args.filename);
        pdata->args.filename=malloc(512);

        //Take that single-point-of-exit advocates !!!
        //15 points of exit, just for your pleasure.
        //Also, Vi(m) rules, emacs sucks.

        if(fscanf(fp,"recordMyDesktop = %s\n",Cached_Version)!=1){
            fprintf(stderr,"Error reading VERSION attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"Width = %d\n",&pdata->brwin.rgeom.width)!=1){
            fprintf(stderr,"Error reading Width attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"Height = %d\n",&pdata->brwin.rgeom.height)!=1){
            fprintf(stderr,"Error reading Height attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"Filename = %s\n",pdata->args.filename)!=1){
            fprintf(stderr,"Error reading Filename attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"FPS = %f\n",&pdata->args.fps)!=1){
            fprintf(stderr,"Error reading FPS attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"NoSound = %d\n",&pdata->args.nosound)!=1){
            fprintf(stderr,"Error reading NoSound attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"Frequency = %d\n",&pdata->args.frequency)!=1){
            fprintf(stderr,"Error reading Frequency attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"Channels = %d\n",&pdata->args.channels)!=1){
            fprintf(stderr,"Error reading Channels attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"BufferSize = %lu\n",&pdata->args.buffsize)!=1){
            fprintf(stderr,"Error reading BufferSize attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"SoundFrameSize = %d\n",&pdata->sound_framesize)!=1){
            fprintf(stderr,"Error reading SoundFrameSize attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"PeriodSize = %lu\n",&pdata->periodsize)!=1){
            fprintf(stderr,"Error reading PeriodSize attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"UsedJack = %u\n",&pdata->args.use_jack)!=1){
            fprintf(stderr,"Error reading UsedJack attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"v_bitrate = %d\n",&pdata->args.v_bitrate)!=1){
            fprintf(stderr,"Error reading v_bitrate attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"v_quality = %d\n",&pdata->args.v_quality)!=1){
            fprintf(stderr,"Error reading v_quality attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"s_quality = %d\n",&pdata->args.s_quality)!=1){
            fprintf(stderr,"Error reading s_quality attribute!!!\n");
            return 1;
        }
        if(fscanf(fp,"ZeroCompression = %d\n",&pdata->args.zerocompression)!=1){
            fprintf(stderr,"Error reading ZeroCompression attribute!!!\n");
            return 1;
        }


    }

    fclose(fp);
    return 0;

}






