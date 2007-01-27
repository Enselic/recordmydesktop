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

void LoadBlock(unsigned char *dest,
               unsigned char *source,
               int blockno,
               int width,
               int height,
               int blockwidth){
    int j,
        block_i=blockno/(width/blockwidth),//place on the grid
        block_k=blockno%(width/blockwidth);
    for(j=0;j<blockwidth;j++)//we copy rows
        memcpy( &dest[(block_i*width+block_k)*blockwidth+j*width],
                &source[j*blockwidth],
                blockwidth);
}

//returns number of bytes
int ReadZF(void * buffer,size_t size,size_t nmemb,FILE *ucfp,gzFile *ifp){
    if((ifp!=NULL && ucfp!=NULL)||
       (ifp==NULL && ucfp==NULL))
        return -1;
    else if(ucfp!=NULL){
        return (size*fread(buffer,size,nmemb,ucfp));
    }
    else
        return gzread(ifp,buffer,size*nmemb);
}

int ReadFrame(CachedFrame *frame,FILE *ucfp,gzFile *ifp){
    int index_entry_size=sizeof(u_int32_t);
    if(frame->header->Ynum>0){
        if(ReadZF(frame->YBlocks,
                index_entry_size,
                frame->header->Ynum,
                ucfp,
                ifp)!=index_entry_size*frame->header->Ynum){
            return -1;
        }
    }
    if(frame->header->Unum>0){
        if(ReadZF(frame->UBlocks,
                index_entry_size,
                frame->header->Unum,
                ucfp,
                ifp)!=index_entry_size*frame->header->Unum){
            return -1;
        }
    }
    if(frame->header->Vnum>0){
        if(ReadZF(frame->VBlocks,
                index_entry_size,
                frame->header->Vnum,
                ucfp,
                ifp)!=index_entry_size*frame->header->Vnum){
            return -1;
        }
    }
    if(frame->header->Ynum>0){
        if(ReadZF(frame->YData,
                  Y_UNIT_BYTES,
                  frame->header->Ynum,
                  ucfp,
                  ifp)!=Y_UNIT_BYTES*frame->header->Ynum){
            return -2;
        }
    }
    if(frame->header->Unum>0){
        if(ReadZF(frame->UData,
                  UV_UNIT_BYTES,
                  frame->header->Unum,
                  ucfp,
                  ifp)!=UV_UNIT_BYTES*frame->header->Unum){
            return -2;
        }
    }
    if(frame->header->Vnum>0){
        if(ReadZF(frame->VData,
                  UV_UNIT_BYTES,
                  frame->header->Vnum,
                  ucfp,
                  ifp)!=UV_UNIT_BYTES*frame->header->Vnum){
            return -2;
        }
    }
    return 0;
}

void *LoadCache(ProgData *pdata){

    yuv_buffer *yuv=&pdata->enc_data->yuv;
    gzFile *ifp=NULL;
    FILE *ucfp=NULL;
    FILE *afp=pdata->cache_data->afp;
    FrameHeader fheader;
    CachedFrame frame;
    int j=0,
        nth_cache=1,
        audio_end=0,
        extra_frames=0,//total number of duplicated frames
        missing_frames=0,//if this is found >0 current run will not load
                        //a frame but it will proccess the previous
        thread_exit=0,//0 success, -1 couldn't find files,1 couldn't remove
        blocknum_x=pdata->enc_data->yuv.y_width/Y_UNIT_WIDTH,
        blocknum_y=pdata->enc_data->yuv.y_height/Y_UNIT_WIDTH,
        blockszy=Y_UNIT_BYTES,//size of y plane block in bytes
        blockszuv=UV_UNIT_BYTES,//size of u,v plane blocks in bytes
        framesize=((snd_pcm_format_width(SND_PCM_FORMAT_S16_LE))/8)*
                  pdata->args.channels;//audio frame size
    signed char *sound_data=(signed char *)malloc(pdata->periodsize*framesize);
    u_int32_t YBlocks[(yuv->y_width*yuv->y_height)/Y_UNIT_BYTES],
              UBlocks[(yuv->uv_width*yuv->uv_height)/UV_UNIT_BYTES],
              VBlocks[(yuv->uv_width*yuv->uv_height)/UV_UNIT_BYTES];
    //we allocate the frame that we will use
    INIT_FRAME(&frame,&fheader,yuv,
                YBlocks,UBlocks,VBlocks)
    //and the we open our files
    if(!pdata->args.zerocompression){
        ifp=gzopen(pdata->cache_data->imgdata,"rb");
        if(ifp==NULL){
            thread_exit=-1;
            pthread_exit(&thread_exit);
        }
    }
    else{
        ucfp=fopen(pdata->cache_data->imgdata,"rb");
        if(ucfp==NULL){
            thread_exit=-1;
            pthread_exit(&thread_exit);
        }
    }


    if(!pdata->args.nosound){
        afp=fopen(pdata->cache_data->audiodata,"rb");
        if(afp==NULL){
            thread_exit=-1;
            pthread_exit(&thread_exit);
        }
    }

    //this will be used now to define if we proccess audio or video
    //on any given loop.
    pdata->avd=0;
    //If sound finishes first,we go on with the video.
    //If video ends we will do one more run to flush audio in the ogg file
    while(pdata->running){
        //video load and encoding
        if(pdata->avd<=0 || pdata->args.nosound || audio_end){
            if(missing_frames>0){
                extra_frames++;
                missing_frames--;
                SyncEncodeImageBuffer(pdata);
            }
            else if(((!pdata->args.zerocompression)&&
                     (gzread(ifp,frame.header,sizeof(FrameHeader))==
                      sizeof(FrameHeader) ))||
                    ((pdata->args.zerocompression)&&
                    (fread(frame.header,sizeof(FrameHeader),1,ucfp)==1))){
                //sync
                missing_frames+=frame.header->current_total-
                                (extra_frames+frame.header->frameno);
                fprintf(stdout,"\r[%d%%] ",
                ((frame.header->frameno+extra_frames)*100)/frames_total);
                fflush(stdout);
                if( (frame.header->Ynum<=blocknum_x*blocknum_y) &&
                    (frame.header->Unum<=blocknum_x*blocknum_y) &&
                    (frame.header->Vnum<=blocknum_x*blocknum_y) &&
                    (!ReadFrame(&frame,
                                ((pdata->args.zerocompression)?ucfp:NULL),
                                ((pdata->args.zerocompression)?NULL:ifp)))
                        ){
                        //load the blocks for each buffer
                        if(frame.header->Ynum)
                            for(j=0;j<frame.header->Ynum;j++)
                                LoadBlock(  yuv->y,
                                            &frame.YData[j*blockszy],
                                            frame.YBlocks[j],
                                            yuv->y_width,
                                            yuv->y_height,
                                            Y_UNIT_WIDTH);
                        if(frame.header->Unum)
                            for(j=0;j<frame.header->Unum;j++)
                                LoadBlock(  yuv->u,
                                            &frame.UData[j*blockszuv],
                                            frame.UBlocks[j],
                                            yuv->uv_width,
                                            yuv->uv_height,
                                            UV_UNIT_WIDTH);
                        if(frame.header->Vnum)
                            for(j=0;j<frame.header->Vnum;j++)
                                LoadBlock(  yuv->v,
                                            &frame.VData[j*blockszuv],
                                            frame.VBlocks[j],
                                            yuv->uv_width,
                                            yuv->uv_height,
                                            UV_UNIT_WIDTH);
                        //encode. This is not made in a thread since
                        //now blocking is not a problem
                        //and this way sync problems
                        //can be avoided more easily.
                        SyncEncodeImageBuffer(pdata);
                }
                else{
                    raise(SIGINT);
                    continue;
                }
            }
            else{
                if(SwapCacheFilesRead(pdata->cache_data->imgdata,
                                      nth_cache,
                                      &ifp,
                                      &ucfp)){
                    raise(SIGINT);
                }
                else{
                    fprintf(stderr,"\t[Cache File %d]",nth_cache);
                    nth_cache++;
                }
                continue;
            }
        }
        //audio load and encoding
        else{
            if(!audio_end){
                int nbytes=fread(sound_data,pdata->periodsize*framesize,1,afp);
                if(nbytes<=0)
                    audio_end=1;
                else
                    SyncEncodeSoundBuffer(pdata,sound_data);
            }
        }
    }

    pdata->v_encoding_clean=pdata->th_encoding_clean=1;
    pthread_cond_signal(&pdata->theora_lib_clean);
    pthread_cond_signal(&pdata->vorbis_lib_clean);
    fprintf(stdout,"\n");
    CLEAR_FRAME(&frame)
    free(sound_data);

    if(!pdata->args.nosound){
        fclose(afp);
    }

    pthread_exit(&thread_exit);


}

