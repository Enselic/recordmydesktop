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

void InitCacheData(ProgData *pdata,EncData *enc_data_t,CacheData *cache_data_t){
    int width,height,offset_x,offset_y,pid;
    char pidbuf[8];

    //we set the buffer only since there's
    //no need to initialize the encoder from now.
    width=((pdata->brwin.rgeom.width + 15) >>4)<<4;
    height=((pdata->brwin.rgeom.height + 15) >>4)<<4;
    offset_x=((width-pdata->brwin.rgeom.width)/2)&~1;
    offset_y=((height-pdata->brwin.rgeom.height)/2)&~1;

    (pdata)->enc_data=enc_data_t;

    enc_data_t->yuv.y=(unsigned char *)malloc(height*width);
    enc_data_t->yuv.u=(unsigned char *)malloc(height*width/4);
    enc_data_t->yuv.v=(unsigned char *)malloc(height*width/4);
    enc_data_t->yuv.y_width=width;
    enc_data_t->yuv.y_height=height;
    enc_data_t->yuv.y_stride=width;

    enc_data_t->yuv.uv_width=width/2;
    enc_data_t->yuv.uv_height=height/2;
    enc_data_t->yuv.uv_stride=width/2;
    enc_data_t->x_offset=offset_x;
    enc_data_t->y_offset=offset_y;


    //now we set the cache files
    (pdata)->cache_data=cache_data_t;

    cache_data_t->workdir=(pdata->args).workdir;
    pid=getpid();

    I16TOA(pid,pidbuf)
    //names are stored relatively to current dir(i.e. no chdir)
    cache_data_t->projname=malloc(strlen(cache_data_t->workdir)+12+strlen(pidbuf)+3);
    //projname
    strcpy(cache_data_t->projname,cache_data_t->workdir);
    strcat(cache_data_t->projname,"/");
    strcat(cache_data_t->projname,"rMD-session-");
    strcat(cache_data_t->projname,pidbuf);
    strcat(cache_data_t->projname,"/");
    //image data
    cache_data_t->imgdata=malloc(strlen(cache_data_t->projname)+11);
    strcpy(cache_data_t->imgdata,cache_data_t->projname);
    strcat(cache_data_t->imgdata,"img.out.gz");
    //audio data
    cache_data_t->audiodata=malloc(strlen(cache_data_t->projname)+10);
    strcpy(cache_data_t->audiodata,cache_data_t->projname);
    strcat(cache_data_t->audiodata,"audio.pcm");

    //now that've got out buffers and our filenames we start
    //creating the needed files

    mkdir(cache_data_t->projname,0777);
    cache_data_t->ifp=gzopen(cache_data_t->imgdata,"wb1f");
    if(!pdata->args.nosound)
        cache_data_t->afp=fopen(cache_data_t->audiodata,"wb");

}
 

