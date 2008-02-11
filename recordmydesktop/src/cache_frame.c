/******************************************************************************
*                            recordMyDesktop                                  *
*******************************************************************************
*                                                                             *
*            Copyright (C) 2006,2007 John Varouhakis                          *
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

#include <recordmydesktop.h>


int FlushBlock(unsigned char *buf,
               int blockno,
               int width,
               int height,
               int blockwidth,
               gzFile *fp,
               FILE *ucfp,
               int flush){
    int j,i,
        bytes_written=0,
        block_i=(!blockwidth)?0:(blockno/(width/blockwidth)),//place on the grid
        block_k=(!blockwidth)?0:(blockno%(width/blockwidth));
    register unsigned char *buf_reg=(&buf[(block_i*
                                         width+
                                         block_k)*blockwidth]);
    static unsigned char out_buffer[CACHE_OUT_BUFFER_SIZE];
    static unsigned int out_buffer_bytes=0;

    if(out_buffer_bytes+pow(blockwidth,2)>=CACHE_OUT_BUFFER_SIZE ||
       (flush && out_buffer_bytes)){
        if(ucfp==NULL)
            gzwrite(fp,(void *)out_buffer,out_buffer_bytes);
        else
            fwrite((void *)out_buffer,1,out_buffer_bytes,ucfp);
        bytes_written=out_buffer_bytes;
        out_buffer_bytes=0;
    }
    if(!flush){
        register unsigned char *out_buf_reg=&out_buffer[out_buffer_bytes];
        for(j=0;j<blockwidth;j++){
            for(i=0;i<blockwidth;i++)
                (*out_buf_reg++)=(*buf_reg++);
            out_buffer_bytes+=blockwidth;
            buf_reg+=width-blockwidth;
        }
    }

    return bytes_written;
}

void *CacheImageBuffer(ProgData *pdata){

    gzFile *fp=NULL;
    FILE *ucfp=NULL;
    int index_entry_size=sizeof(u_int32_t),
        blocknum_x=pdata->enc_data->yuv.y_width/Y_UNIT_WIDTH,
        blocknum_y=pdata->enc_data->yuv.y_height/Y_UNIT_WIDTH,
        firstrun=1,
        frameno=0,
        nbytes=0,
        nth_cache=1;
    unsigned int total_bytes=0,
                 total_received_bytes=0;
    u_int32_t   ynum,unum,vnum,
                y_short_blocks[blocknum_x*blocknum_y],
                u_short_blocks[blocknum_x*blocknum_y],
                v_short_blocks[blocknum_x*blocknum_y];

    if(!pdata->args.zerocompression){
        fp=pdata->cache_data->ifp;
        if(fp==NULL)exit(13);
    }
    else{
        ucfp=pdata->cache_data->uncifp;
        if(ucfp==NULL)exit(13);
    }


    while(pdata->running){
        int j;
        FrameHeader fheader;
        ynum=unum=vnum=0;

        pdata->th_enc_thread_waiting=1;
        pthread_mutex_lock(&pdata->img_buff_ready_mutex);
        pthread_cond_wait(&pdata->image_buffer_ready,
                          &pdata->img_buff_ready_mutex);
        pthread_mutex_unlock(&pdata->img_buff_ready_mutex);
        pdata->th_enc_thread_waiting=0;

        if(Paused){
            pthread_mutex_lock(&pause_mutex);
            pthread_cond_wait(&pdata->pause_cond,&pause_mutex);
            pthread_mutex_unlock(&pause_mutex);
        }

        pthread_mutex_lock(&pdata->yuv_mutex);

        //find and flush different blocks
        if(firstrun){
            firstrun=0;
            for(j=0;j<blocknum_x*blocknum_y;j++){
                    ynum++;
                    yblocks[ynum-1]=1;
                    y_short_blocks[ynum-1]=j;
                    unum++;
                    ublocks[unum-1]=1;
                    u_short_blocks[ynum-1]=j;
                    vnum++;
                    vblocks[vnum-1]=1;
                    v_short_blocks[ynum-1]=j;
            }
        }
        else{
            /**COMPRESS ARRAYS*/
            for(j=0;j<blocknum_x*blocknum_y;j++){
                if(yblocks[j]){
                    ynum++;
                    y_short_blocks[ynum-1]=j;
                }
                if(ublocks[j]){
                    unum++;
                    u_short_blocks[unum-1]=j;
                }
                if(vblocks[j]){
                    vnum++;
                    v_short_blocks[vnum-1]=j;
                }
            }
        }

        /**WRITE FRAME TO DISK*/
        if(!pdata->args.zerocompression){
            if(ynum*4+unum+vnum>(blocknum_x*blocknum_y*6)/10)
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
        if(!pdata->args.zerocompression){
            nbytes+=gzwrite(fp,(void*)&fheader,sizeof(FrameHeader));
            //flush indexes
            if(ynum)nbytes+=gzwrite(fp,
                                    (void*)y_short_blocks,
                                    ynum*index_entry_size);
            if(unum)nbytes+=gzwrite(fp,
                                    (void*)u_short_blocks,
                                    unum*index_entry_size);
            if(vnum)nbytes+=gzwrite(fp,
                                    (void*)v_short_blocks,
                                    vnum*index_entry_size);
        }
        else{
            nbytes+=sizeof(FrameHeader)*
                    fwrite((void*)&fheader,sizeof(FrameHeader),1,ucfp);
            //flush indexes
            if(ynum)nbytes+=index_entry_size*
                            fwrite(y_short_blocks,index_entry_size,ynum,ucfp);
            if(unum)nbytes+=index_entry_size*
                            fwrite(u_short_blocks,index_entry_size,unum,ucfp);
            if(vnum)nbytes+=index_entry_size*
                            fwrite(v_short_blocks,index_entry_size,vnum,ucfp);
        }
        //flush the blocks for each buffer
        if(ynum){
            for(j=0;j<ynum;j++)
                nbytes+=FlushBlock( pdata->enc_data->yuv.y,y_short_blocks[j],
                                    pdata->enc_data->yuv.y_width,
                                    pdata->enc_data->yuv.y_height,
                                    Y_UNIT_WIDTH,
                                    fp,
                                    ucfp,
                                    0);
        }
        if(unum){
            for(j=0;j<unum;j++)
                nbytes+=FlushBlock( pdata->enc_data->yuv.u,u_short_blocks[j],
                                    pdata->enc_data->yuv.uv_width,
                                    pdata->enc_data->yuv.uv_height,
                                    UV_UNIT_WIDTH,
                                    fp,
                                    ucfp,
                                    0);
        }
        if(vnum){
            for(j=0;j<vnum;j++)
                nbytes+=FlushBlock( pdata->enc_data->yuv.v,v_short_blocks[j],
                                    pdata->enc_data->yuv.uv_width,
                                    pdata->enc_data->yuv.uv_height,
                                    UV_UNIT_WIDTH,
                                    fp,
                                    ucfp,
                                    0);
        }
        //release main buffer
        pthread_mutex_unlock(&pdata->yuv_mutex);

        nbytes+=FlushBlock(NULL,0,0,0,0,fp,ucfp,1);
        /**@________________@**/
        pdata->avd+=pdata->frametime;
        if(nbytes>CACHE_FILE_SIZE_LIMIT){
            if(SwapCacheFilesWrite(pdata->cache_data->imgdata,
                                   nth_cache,&fp,&ucfp)){
                fprintf(stderr,"New cache file could not be created.\n"
                               "Ending recording...\n");
                fflush(stderr);
                raise(SIGINT);  //if for some reason we cannot make a new file
                                //we have to stop. If we are out of space,
                                //which means
                                //that encoding cannot happen either,
                                //InitEncoder will cause an abrupt end with an
                                //error code and the cache will remain intact.
                                //If we've chosen separate two-stages,
                                //the program will make a
                                //clean exit.
                                //In either case data will be preserved so if
                                //space is freed the recording
                                //can be proccessed later.
            }
            total_bytes+=(nbytes>>20);
            nth_cache++;
            nbytes=0;
        }
    }
    total_bytes+=(nbytes>>20);
    total_received_bytes=((frameno*((pdata->specs.depth>=24)?4:2)*
                   pdata->brwin.rgeom.width*pdata->brwin.rgeom.height)>>20);
    if(total_received_bytes){
        fprintf(stderr,"\n*********************************************\n"
                       "\nCached %u MB, from %u MB that were received.\n"
                       "Average cache compression ratio: %u %%\n"
                       "\n*********************************************\n",
                       total_bytes,
                       total_received_bytes,
                       (total_bytes*100)/total_received_bytes);

    }

    fprintf(stderr,"Saved %d frames in a total of %d requests\n",
                   frameno,frames_total);fflush(stderr);
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
