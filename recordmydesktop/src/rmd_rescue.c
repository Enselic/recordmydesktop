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

#include "recordmydesktop.h"
#include "register_callbacks.h"


int rmdRescue(const char *path){

    int i=0,
        width,
        height,
        offset_x,
        offset_y;

    ProgData pdata;
    EncData enc_data;
    CacheData cache_data;


    DEFAULT_ARGS(&pdata.args);

    pdata.enc_data=&enc_data;
    pdata.cache_data=&cache_data;

    //projname
    cache_data.projname=malloc(strlen(path)+2);
    strcpy(cache_data.projname,path);
    strcat(cache_data.projname,"/");//having two of these doesn't hurt...
    //image data
    cache_data.imgdata=malloc(strlen(cache_data.projname)+11);
    strcpy(cache_data.imgdata,cache_data.projname);
    strcat(cache_data.imgdata,"img.out");
    //audio data
    cache_data.audiodata=malloc(strlen(cache_data.projname)+10);
    strcpy(cache_data.audiodata,cache_data.projname);
    strcat(cache_data.audiodata,"audio.pcm");
    //specsfile
    cache_data.specsfile=malloc(strlen(cache_data.projname)+10);
    strcpy(cache_data.specsfile,cache_data.projname);
    strcat(cache_data.specsfile,"specs.txt");
    

    if(ReadSpecsFile(&pdata))
        return 1;

        
    width=((pdata.brwin.rgeom.width + 15) >>4)<<4;
    height=((pdata.brwin.rgeom.height + 15) >>4)<<4;
    offset_x=((width-pdata.brwin.rgeom.width)/2)&~1;
    offset_y=((height-pdata.brwin.rgeom.height)/2)&~1;

    
    enc_data.yuv.y=(unsigned char *)malloc(height*width);
    enc_data.yuv.u=(unsigned char *)malloc(height*width/4);
    enc_data.yuv.v=(unsigned char *)malloc(height*width/4);
    enc_data.yuv.y_width=width;
    enc_data.yuv.y_height=height;
    enc_data.yuv.y_stride=width;

    enc_data.yuv.uv_width=width/2;
    enc_data.yuv.uv_height=height/2;
    enc_data.yuv.uv_stride=width/2;
    enc_data.x_offset=offset_x;
    enc_data.y_offset=offset_y;

    for(i=0;i<(enc_data.yuv.y_width*enc_data.yuv.y_height);i++)
        enc_data.yuv.y[i]=0;
    for(i=0;i<(enc_data.yuv.uv_width*enc_data.yuv.uv_height);i++){
        enc_data.yuv.v[i]=enc_data.yuv.u[i]=127;
    }

    yblocks=malloc(sizeof(u_int32_t)*(enc_data.yuv.y_width/Y_UNIT_WIDTH)*
                   (enc_data.yuv.y_height/Y_UNIT_WIDTH));
    ublocks=malloc(sizeof(u_int32_t)*(enc_data.yuv.y_width/Y_UNIT_WIDTH)*
                   (enc_data.yuv.y_height/Y_UNIT_WIDTH));
    vblocks=malloc(sizeof(u_int32_t)*(enc_data.yuv.y_width/Y_UNIT_WIDTH)*
                   (enc_data.yuv.y_height/Y_UNIT_WIDTH));

    pdata.frametime=(1000000)/pdata.args.fps;

    pthread_mutex_init(&pdata.theora_lib_mutex,NULL);
    pthread_mutex_init(&pdata.vorbis_lib_mutex,NULL);
    pthread_mutex_init(&pdata.libogg_mutex,NULL);
    pthread_cond_init(&pdata.theora_lib_clean,NULL);
    pthread_cond_init(&pdata.vorbis_lib_clean,NULL);
    pdata.th_encoding_clean=pdata.v_encoding_clean=1;
    Aborted=pdata.avd=0;
    pdata.sound_buffer=NULL;
    pdata.running=1;

    RegisterCallbacks(&pdata);
    fprintf(stderr,"Restoring %s!!!\n",path);
    
    EncodeCache(&pdata);

    fprintf(stderr,"Done!!!\n");
    fprintf(stderr,"Goodbye!\n");
    CleanUp();
    
    return 0;
}















