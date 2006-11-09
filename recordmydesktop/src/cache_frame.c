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

void FlushBlock(unsigned char *buf,int blockno,int width, int height,int divisor,gzFile *fp,FILE *ucfp){
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
}

void *CacheImageBuffer(void *pdata){
    pthread_mutex_t pmut,imut;
    pthread_mutex_init(&pmut,NULL);
    pthread_mutex_init(&imut,NULL);
    yuv_buffer yuv[2];
    gzFile *fp=NULL;
    FILE *ucfp=NULL;
    int i,current=0,divisor=16,firstrun=1,frameno=0;

    if(!((ProgData *)pdata)->args.zerocompression){
        fp=((ProgData *)pdata)->cache_data->ifp;
        if(fp==NULL)exit(13);
    }
    else{
        ucfp=((ProgData *)pdata)->cache_data->uncifp;
        if(ucfp==NULL)exit(13);
    }


    for(i=0;i<2;i++){
        yuv[i].y_width=((ProgData *)pdata)->enc_data->yuv.y_width;
        yuv[i].y_height=((ProgData *)pdata)->enc_data->yuv.y_height;
        yuv[i].uv_width=((ProgData *)pdata)->enc_data->yuv.uv_width;
        yuv[i].uv_height=((ProgData *)pdata)->enc_data->yuv.uv_height;

        yuv[i].y=(unsigned char *)malloc(yuv[i].y_width*yuv[i].y_height);
        yuv[i].u=(unsigned char *)malloc(yuv[i].uv_width*yuv[i].uv_height);
        yuv[i].v=(unsigned char *)malloc(yuv[i].uv_width*yuv[i].uv_height);
    }


    while(((ProgData *)pdata)->running){
        int prev;
        int j;
        unsigned short ynum,unum,vnum;
        unsigned char yblocks[256],ublocks[64],vblocks[64];
        FrameHeader fheader;
        ynum=unum=vnum=0;

        pthread_cond_wait(&((ProgData *)pdata)->image_buffer_ready,&imut);
        if(Paused)
            pthread_cond_wait(&((ProgData *)pdata)->pause_cond,&pmut);
        pthread_mutex_lock(&((ProgData *)pdata)->yuv_mutex);

        //rotate buffers
        prev=current;
        current=(current)?0:1;
        //copy incoming
        memcpy(yuv[current].y,((ProgData *)pdata)->enc_data->yuv.y,yuv[current].y_width*yuv[current].y_height);
        memcpy(yuv[current].u,((ProgData *)pdata)->enc_data->yuv.u,yuv[current].uv_width*yuv[current].uv_height);
        memcpy(yuv[current].v,((ProgData *)pdata)->enc_data->yuv.v,yuv[current].uv_width*yuv[current].uv_height);
        //release main buffer
        pthread_mutex_unlock(&((ProgData *)pdata)->yuv_mutex);
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
        if(!((ProgData *)pdata)->args.zerocompression){
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
        if(!((ProgData *)pdata)->args.zerocompression){
            gzwrite(fp,(void*)&fheader,sizeof(FrameHeader));
            //flush indexes
            if(ynum)gzwrite(fp,yblocks,ynum);
            if(unum)gzwrite(fp,ublocks,unum);
            if(vnum)gzwrite(fp,vblocks,vnum);
        }
        else{
            fwrite((void*)&fheader,sizeof(FrameHeader),1,ucfp);
            //flush indexes
            if(ynum)fwrite(yblocks,ynum,1,ucfp);
            if(unum)fwrite(ublocks,unum,1,ucfp);
            if(vnum)fwrite(vblocks,vnum,1,ucfp);
        }
        //flush the blocks for each buffer
        if(ynum)
            for(j=0;j<ynum;j++)
                FlushBlock(yuv[current].y,yblocks[j],yuv[current].y_width,yuv[current].y_height,divisor,fp,ucfp);
        if(unum)
            for(j=0;j<unum;j++)
                FlushBlock(yuv[current].u,ublocks[j],yuv[current].uv_width,yuv[current].uv_height,divisor/2,fp,ucfp);
        if(vnum)
            for(j=0;j<vnum;j++)
                FlushBlock(yuv[current].v,vblocks[j],yuv[current].uv_width,yuv[current].uv_height,divisor/2,fp,ucfp);


        /**@________________@**/
        ((ProgData *)pdata)->avd+=((ProgData *)pdata)->frametime*2*((ProgData *)pdata)->args.channels;

    }

    //clean up since we're not finished
    for(i=0;i<2;i++){
        free(yuv[i].y);
        free(yuv[i].u);
        free(yuv[i].v);
    }
    fprintf(stderr,"Saved %d frames in a total of %d requests\n",frameno,frames_total);
    if(!((ProgData *)pdata)->args.zerocompression)
        gzclose(fp);
    else
        fclose(ucfp);
    pthread_exit(&errno);
}
