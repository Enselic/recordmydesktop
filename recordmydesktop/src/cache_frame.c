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


int CompareBlocks(unsigned char *incoming,unsigned char *old,int blockno,int width, int height,int divisor){
    int j,i,
        block_i=blockno/divisor,//place on the grid
        block_k=blockno%divisor;
    register unsigned char *incoming_reg=&(incoming[block_i*(width*height/divisor)+block_k*width/divisor]),
                           *old_reg=&(old[block_i*(width*height/divisor)+block_k*width/divisor]);

    for(j=0;j<height/divisor;j++){
        for(i=0;i<width/divisor;i++){
            if((*(incoming_reg++))!=(*(old_reg++)))
                return 1;
        }
        incoming_reg+=(width-width/divisor);
        old_reg+=(width-width/divisor);
    }

    return 0;
}

int FlushBlock(unsigned char *buf,int blockno,int width, int height,int divisor,gzFile *fp,FILE *ucfp){
    int j,
        block_i=blockno/divisor,//place on the grid
        block_k=blockno%divisor;
    register unsigned char *buf_reg=(&buf[block_i*(width*height/divisor)+block_k*width/divisor]);
    if(ucfp==NULL){
        for(j=0;j<height/divisor;j++){//we flush in rows
            gzwrite(fp,(void *)buf_reg,width/divisor);
            buf_reg+=width;
        }
    }
    else{
        for(j=0;j<height/divisor;j++){//we flush in rows
            fwrite((void *)buf_reg,width/divisor,1,ucfp);
            buf_reg+=width;
        }
    }
    return ((height*width)/pow(divisor,2));
}

void *CacheImageBuffer(ProgData *pdata){
    pthread_mutex_t pmut,imut;
    pthread_mutex_init(&pmut,NULL);
    pthread_mutex_init(&imut,NULL);
    yuv_buffer yuv[2];
    gzFile *fp=NULL;
    FILE *ucfp=NULL;
    int i,
        current=0,
        divisor=16,
        firstrun=1,
        frameno=0,
        nbytes=0,
        nth_cache=1;

    if(!pdata->args.zerocompression){
        fp=pdata->cache_data->ifp;
        if(fp==NULL)exit(13);
    }
    else{
        ucfp=pdata->cache_data->uncifp;
        if(ucfp==NULL)exit(13);
    }


    for(i=0;i<2;i++){
        yuv[i].y_width=pdata->enc_data->yuv.y_width;
        yuv[i].y_height=pdata->enc_data->yuv.y_height;
        yuv[i].uv_width=pdata->enc_data->yuv.uv_width;
        yuv[i].uv_height=pdata->enc_data->yuv.uv_height;

        yuv[i].y=(unsigned char *)malloc(yuv[i].y_width*yuv[i].y_height);
        yuv[i].u=(unsigned char *)malloc(yuv[i].uv_width*yuv[i].uv_height);
        yuv[i].v=(unsigned char *)malloc(yuv[i].uv_width*yuv[i].uv_height);
    }


    while(pdata->running){
        int prev;
        int j;
        unsigned short ynum,unum,vnum;
        unsigned char yblocks[256],ublocks[64],vblocks[64];
        FrameHeader fheader;
        ynum=unum=vnum=0;

        pthread_cond_wait(&pdata->image_buffer_ready,&imut);
        if(Paused)
            pthread_cond_wait(&pdata->pause_cond,&pmut);
        pthread_mutex_lock(&pdata->yuv_mutex);

        //rotate buffers
        prev=current;
        current=(current)?0:1;
        //copy incoming
        memcpy(yuv[current].y,pdata->enc_data->yuv.y,yuv[current].y_width*yuv[current].y_height);
        memcpy(yuv[current].u,pdata->enc_data->yuv.u,yuv[current].uv_width*yuv[current].uv_height);
        memcpy(yuv[current].v,pdata->enc_data->yuv.v,yuv[current].uv_width*yuv[current].uv_height);
        //release main buffer
        pthread_mutex_unlock(&pdata->yuv_mutex);
        //get checksums for new

        //find and flush different blocks
        if(firstrun){
            firstrun=0;
            for(j=0;j<pow(divisor,2);j++){
                    ynum++;
                    yblocks[ynum-1]=j;
            }
            for(j=0;j<pow(divisor/2,2);j++){
                    unum++;
                    ublocks[unum-1]=j;
            }
            for(j=0;j<pow(divisor/2,2);j++){
                    vnum++;
                    vblocks[vnum-1]=j;
            }

        }
        else{
            for(j=0;j<pow(divisor,2);j++){
                if(CompareBlocks(yuv[current].y,yuv[prev].y,j,yuv[current].y_width,yuv[current].y_height,divisor)){
                    ynum++;
                    yblocks[ynum-1]=j;
                }
            }
            for(j=0;j<pow(divisor/2,2);j++){
                if(CompareBlocks(yuv[current].u,yuv[prev].u,j,yuv[current].uv_width,yuv[current].uv_height,divisor/2)){
                    unum++;
                    ublocks[unum-1]=j;
                }
            }
            for(j=0;j<pow(divisor/2,2);j++){
                if(CompareBlocks(yuv[current].v,yuv[prev].v,j,yuv[current].uv_width,yuv[current].uv_height,divisor/2)){
                    vnum++;
                    vblocks[vnum-1]=j;
                }
            }

        }
        /**WRITE FRAME TO DISK*/
        if(!pdata->args.zerocompression){
            if(ynum+unum+vnum>(pow(divisor,2)+pow(divisor/2,2)*2)/10)
                gzsetparams (fp,1,Z_FILTERED);
            else
                gzsetparams (fp,0,Z_FILTERED);
        }

        strncpy(fheader.frame_prefix,"FRAM",4);
        fheader.frameno=++frameno;
        fheader.current_total=frames_total;
        fheader.Ynum=ynum;
        fheader.Unum=unum;
        fheader.Vnum=vnum;
        fheader.pad=0;
        if(!pdata->args.zerocompression){
            nbytes+=gzwrite(fp,(void*)&fheader,sizeof(FrameHeader));
            //flush indexes
            if(ynum)nbytes+=gzwrite(fp,yblocks,ynum);
            if(unum)nbytes+=gzwrite(fp,ublocks,unum);
            if(vnum)nbytes+=gzwrite(fp,vblocks,vnum);
        }
        else{
            nbytes+=sizeof(FrameHeader)*fwrite((void*)&fheader,sizeof(FrameHeader),1,ucfp);
            //flush indexes
            if(ynum)nbytes+=ynum*fwrite(yblocks,ynum,1,ucfp);
            if(unum)nbytes+=unum*fwrite(ublocks,unum,1,ucfp);
            if(vnum)nbytes+=vnum*fwrite(vblocks,vnum,1,ucfp);
        }
        //flush the blocks for each buffer
        if(ynum)
            for(j=0;j<ynum;j++)
                nbytes+=FlushBlock( yuv[current].y,yblocks[j],
                                    yuv[current].y_width,
                                    yuv[current].y_height,
                                    divisor,
                                    fp,
                                    ucfp);
        if(unum)
            for(j=0;j<unum;j++)
                nbytes+=FlushBlock( yuv[current].u,ublocks[j],
                                    yuv[current].uv_width,
                                    yuv[current].uv_height,
                                    divisor/2,
                                    fp,
                                    ucfp);
        if(vnum)
            for(j=0;j<vnum;j++)
                nbytes+=FlushBlock( yuv[current].v,vblocks[j],
                                    yuv[current].uv_width,
                                    yuv[current].uv_height,
                                    divisor/2,
                                    fp,
                                    ucfp);


        /**@________________@**/
        pdata->avd+=pdata->frametime;
        if(nbytes>CACHE_FILE_SIZE_LIMIT){
            if(SwapCacheFilesWrite(pdata->cache_data->imgdata,nth_cache,&fp,&ucfp)){
                fprintf(stderr,"New cache file could not be created.\nEnding recording...\n");
                fflush(stderr);
                raise(SIGINT);  //if for some reason we cannot make a new file
                                //we have to stop. If we are out of space,which means
                                //that encoding cannot happen either,
                                //InitEncoder will cause an abrupt end with an
                                //error code and the cache will remain intact.
                                //If we've chosen separate two-stages, the program will make a
                                //clean exit.
                                //In either case data will be preserved so if
                                //space is freed the recording can be proccessed later.
            }
            nth_cache++;
            nbytes=0;
        }
    }

    //clean up since we're not finished
    for(i=0;i<2;i++){
        free(yuv[i].y);
        free(yuv[i].u);
        free(yuv[i].v);
    }
    fprintf(stderr,"Saved %d frames in a total of %d requests\n",frameno,frames_total);
    if(!pdata->args.zerocompression){
        gzflush(fp,Z_FINISH);
        gzclose(fp);
    }
    else{
        fflush(ucfp);
        fclose(ucfp);
    }
    pthread_exit(&errno);
}
