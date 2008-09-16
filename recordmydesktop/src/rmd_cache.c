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

#include <sys/stat.h>

#include "rmd_types.h"

#include "rmd_cache.h"

#include "rmd_specsfile.h"


/**
*Construct an number postfixed name
*
* \param name base name
*
* \param newname modified name
*
* \n number to be used as a postfix
*
*/
static void CacheFileN(char *name, char **newname, int n) { // Nth cache file
    char numbuf[8];
    strcpy(*newname,name);
    strcat(*newname,".");
    snprintf( numbuf, 8, "%d", n );
    strcat(*newname,numbuf);
}

int SwapCacheFilesWrite(char *name,int n,gzFile **fp,FILE **ucfp){
    char *newname=malloc(strlen(name)+10);
    CacheFileN(name,&newname,n);
    if(*fp==NULL){
        fflush(*ucfp);
        fclose(*ucfp);
        *ucfp=fopen(newname,"wb");
    }
    else{
        gzflush(*fp,Z_FINISH);
        gzclose(*fp);
        *fp=gzopen(newname,"wb0f");
    }
    free(newname);
    return ((*fp==NULL)&&(*ucfp==NULL));
}

int SwapCacheFilesRead(char *name,int n,gzFile **fp,FILE **ucfp){
    char *newname=malloc(strlen(name)+10);
    CacheFileN(name,&newname,n);
    if(*fp==NULL){
        fclose(*ucfp);
        *ucfp=fopen(newname,"rb");
    }
    else{

        gzclose(*fp);
        *fp=gzopen(newname,"rb");
    }
    free(newname);
    return ((*fp==NULL)&&(*ucfp==NULL));
}

int PurgeCache(CacheData *cache_data_t,int sound){
    struct stat buff;
    char *fname;
    fname=malloc(strlen(cache_data_t->imgdata)+10);
    strcpy(fname,cache_data_t->imgdata);
    int exit_value=0;
    int nth_cache=1;

    while (stat(fname,&buff)==0){
        if(remove(fname)){
            fprintf(stderr,"Couldn't remove temporary file %s",
                           cache_data_t->imgdata);
            exit_value=1;
        }
        CacheFileN(cache_data_t->imgdata,&fname,nth_cache);
        nth_cache++;
    }
    free(fname);
    if(sound){
        if(remove(cache_data_t->audiodata)){
            fprintf(stderr,"Couldn't remove temporary file %s",
                           cache_data_t->audiodata);
            exit_value=1;
        }
    }
    if(remove(cache_data_t->specsfile)){
            fprintf(stderr,"Couldn't remove temporary file %s",
                           cache_data_t->specsfile);
            exit_value=1;
    }
    if(remove(cache_data_t->projname)){
        fprintf(stderr,"Couldn't remove temporary directory %s",
                       cache_data_t->projname);
        exit_value=1;
    }
    return exit_value;
}

void InitCacheData(ProgData *pdata,
                   EncData *enc_data_t,
                   CacheData *cache_data_t){
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

    snprintf( pidbuf, 8, "%d", pid );
    //names are stored relatively to current dir(i.e. no chdir)
    cache_data_t->projname=malloc(strlen(cache_data_t->workdir)+
                           12+strlen(pidbuf)+3);
    //projname
    strcpy(cache_data_t->projname,cache_data_t->workdir);
    strcat(cache_data_t->projname,"/");
    strcat(cache_data_t->projname,"rMD-session-");
    strcat(cache_data_t->projname,pidbuf);
    strcat(cache_data_t->projname,"/");
    //image data
    cache_data_t->imgdata=malloc(strlen(cache_data_t->projname)+11);
    strcpy(cache_data_t->imgdata,cache_data_t->projname);
    strcat(cache_data_t->imgdata,"img.out");
    //audio data
    cache_data_t->audiodata=malloc(strlen(cache_data_t->projname)+10);
    strcpy(cache_data_t->audiodata,cache_data_t->projname);
    strcat(cache_data_t->audiodata,"audio.pcm");
    //specsfile
    cache_data_t->specsfile=malloc(strlen(cache_data_t->projname)+10);
    strcpy(cache_data_t->specsfile,cache_data_t->projname);
    strcat(cache_data_t->specsfile,"specs.txt");

    //now that've got out buffers and our filenames we start
    //creating the needed files

    if(mkdir(cache_data_t->projname,0777)){
        fprintf(stderr,"Could not create temporary directory %s !!!\n",
                       cache_data_t->projname);
        exit(13);
    }
    if(!pdata->args.zerocompression){
        cache_data_t->ifp=gzopen(cache_data_t->imgdata,"wb0f");
        if(cache_data_t->ifp==NULL){
            fprintf(stderr,"Could not create temporary file %s !!!\n",
                           cache_data_t->imgdata);
            exit(13);
        }
    }
    else{
        cache_data_t->uncifp=fopen(cache_data_t->imgdata,"wb0f");
        if(cache_data_t->uncifp==NULL){
            fprintf(stderr,"Could not create temporary file %s !!!\n",
                           cache_data_t->imgdata);
            exit(13);
        }
    }
    if(!pdata->args.nosound){
        cache_data_t->afp=fopen(cache_data_t->audiodata,"wb");
        if(cache_data_t->afp==NULL){
           fprintf(stderr,"Could not create temporary file %s !!!\n",
                          cache_data_t->audiodata);
           exit(13);
        }
    }
    if(WriteSpecsFile(pdata)){
        fprintf(stderr,"Could not write specsfile %s !!!\n",
                        cache_data_t->specsfile);
        exit(13);
    }

}


