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


void MakeChecksums(unsigned char *buf,int width,int height,int divisor,unsigned short int *checksums){
    int i,k,j,m;

    for(i=0;i<divisor;i++){
        for(k=0;k<divisor;k++){
            int A=1,B=0;
            for(j=0;j<height/divisor;j++){
                for(m=0;m<width/divisor;m++){
                    A+=buf[i*(width*height/divisor)+j*width+k*width/divisor+m];
                    B+=A;
                }
            }
            A=A%65521;
            B=B%65521;
            checksums[i*divisor+k]=B*65536+A;
//             A = (A & 0xffff) + (A >> 16) * (15);
//             B = (B & 0xffff) + (B >> 16) * (15);
//
//             if (A >= 65521)
//                A -= 65521;
//
//             B = (B & 0xffff) + (B >> 16) * (15);
//             if (B >= 65521)
//                     B -= 65521;
//             checksums[i*divisor+k]= ((B << 16) | A) ;


        }
    }
}

int CompareBlocks(unsigned char *incoming,unsigned char *old,int blockno,int width, int height,int divisor){
    int j,i,
        block_i=blockno/divisor,//place on the grid
        block_k=blockno%divisor;

    for(j=0;j<height/divisor;j++)//we copy rows
            for(i=0;i<width/divisor;i++)
                if(incoming[block_i*(width*height/divisor)+j*width+block_k*width/divisor+i]!=
                    old[block_i*(width*height/divisor)+j*width+block_k*width/divisor+i])
                    return 1;
    return 0;
}

void FlushBlock(unsigned char *buf,int blockno,int width, int height,int divisor,gzFile *fp){
    int j,
        block_i=blockno/divisor,//place on the grid
        block_k=blockno%divisor;

    for(j=0;j<height/divisor;j++)//we flush in rows
        gzwrite(fp,(void *)&buf[block_i*(width*height/divisor)+j*width+block_k*width/divisor],width/divisor);

}

void *CacheImageBuffer(void *pdata){
    pthread_mutex_t pmut,imut;
    pthread_mutex_init(&pmut,NULL);
    pthread_mutex_init(&imut,NULL);
    yuv_buffer yuv[2];
    gzFile *fp=((ProgData *)pdata)->cache_data->ifp;

    if(fp==NULL)exit(13);

    int i,current=0,divisor=16,firstrun=1,frameno=0;

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
        gzwrite(fp,(void*)&fheader,sizeof(FrameHeader));
        //flush indexes
        if(ynum)gzwrite(fp,yblocks,ynum);
        if(unum)gzwrite(fp,ublocks,unum);
        if(vnum)gzwrite(fp,vblocks,vnum);


        //flush the blocks for each buffer
        if(ynum)
            for(j=0;j<ynum;j++)
                FlushBlock(yuv[current].y,yblocks[j],yuv[current].y_width,yuv[current].y_height,divisor,fp);
        if(unum)
            for(j=0;j<unum;j++)
                FlushBlock(yuv[current].u,ublocks[j],yuv[current].uv_width,yuv[current].uv_height,divisor/2,fp);
        if(vnum)
            for(j=0;j<vnum;j++)
                FlushBlock(yuv[current].v,vblocks[j],yuv[current].uv_width,yuv[current].uv_height,divisor/2,fp);


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
    gzclose(fp);
    pthread_exit(&errno);
}
