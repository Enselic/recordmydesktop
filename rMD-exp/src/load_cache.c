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

void LoadBlock(unsigned char *dest,unsigned char *source,int blockno,int width, int height,int divisor){
    int j,
        block_i=blockno/divisor,//place on the grid
        block_k=blockno%divisor;
    
    for(j=0;j<height/divisor;j++)//we copy rows
        memcpy( &dest[block_i*(width*height/divisor)+j*width+block_k*width/divisor],
                &source[j*width/divisor],
                width/divisor);
//         gzread(fp,(void *)&dest[block_i*(width*height/divisor)+j*width+block_k*width/divisor],width/divisor);
// ;
}


void *LoadCache(void *pdata){

    yuv_buffer *yuv=&((ProgData *)pdata)->enc_data->yuv;
    gzFile *ifp=((ProgData *)pdata)->cache_data->ifp;
    FILE *afp=((ProgData *)pdata)->cache_data->afp;
    FrameHeader fheader;
    CachedFrame frame;
    int j=0,
        thread_exit=0,//0 success, -1 couldn't find files,1 couldn't remove
        divisor=16,
        blockszy=0,//size of y plane block in bytes
        blockszuv=0;//size of u,v plane blocks in bytes
    //we allocate the frame that we will use
    INIT_FRAME(&frame,&fheader,yuv)
    //and the we open our files
    ifp=gzopen(((ProgData *)pdata)->cache_data->imgdata,"rb");
    if(ifp==NULL){
        thread_exit=-1;
        pthread_exit(&thread_exit);
    }
       
    if(!((ProgData *)pdata)->args.nosound){
        afp=fopen(((ProgData *)pdata)->cache_data->audiodata,"rb");
        if(afp==NULL){
            thread_exit=-1;
            pthread_exit(&thread_exit);
        }
    }
//     int first=10;

    //recalculate the divisor since it is not saved
    while(((((ProgData *)pdata)->brwin.rgeom.width*
            ((ProgData *)pdata)->brwin.rgeom.width )
            /pow(divisor,2))
            <=1024){
        divisor/=2;
        if(divisor==2)
            break;
    }
    //these two are likely to be the same, but not guaranteed, especially on
    //low resolutions
    blockszy=(yuv->y_width*yuv->y_height )/pow(divisor,2);
    blockszuv=(yuv->uv_width*yuv->uv_height)/pow(divisor/2,2);


    while(((ProgData *)pdata)->running){
            int framesize=0;
//         if(first){
            if(gzread(ifp,frame.header,sizeof(FrameHeader))==sizeof(FrameHeader)){
//                 fprintf(stderr,"1:%c%c%c%c \n1: %d \n1: %d \n1: %d \n1: %d \n1: %d \n1: %d\n",
//                             frame.header->frame_prefix[0],frame.header->frame_prefix[1],frame.header->frame_prefix[2],frame.header->frame_prefix[3],
//                             frame.header->frameno,frame.header->current_total
//                             ,frame.header->Ynum,frame.header->Unum,frame.header->Vnum,frame.header->pad);fflush(stderr);
                fprintf(stderr,"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b %d of %d",frame.header->frameno,frame.header->current_total);
                if( (frame.header->Ynum<=pow(divisor,2)) &&
                    (frame.header->Unum<=pow(divisor/2,2)) &&
                    (frame.header->Vnum<=pow(divisor/2,2)) &&
                    (gzread(ifp,frame.YBlocks,frame.header->Ynum)==frame.header->Ynum) &&
                    (gzread(ifp,frame.UBlocks,frame.header->Unum)==frame.header->Unum) &&
                    (gzread(ifp,frame.VBlocks,frame.header->Vnum)==frame.header->Vnum) &&
                    (gzread(ifp,frame.YData,blockszy*frame.header->Ynum)==blockszy*frame.header->Ynum) &&
                    (gzread(ifp,frame.UData,(blockszuv*frame.header->Unum))==(blockszuv*frame.header->Unum)) &&
                    (gzread(ifp,frame.VData,(blockszuv*frame.header->Vnum))==(blockszuv*frame.header->Vnum))){
                        framesize+=sizeof(FrameHeader)
                                        +frame.header->Ynum+frame.header->Unum+frame.header->Vnum
                                        +blockszy*frame.header->Ynum+(blockszuv*frame.header->Unum)
                                        +(blockszuv*frame.header->Vnum);
//                         fprintf(stderr,"OK! %d \n%d %d %d\n",framesize,blockszy,blockszuv,sizeof(FrameHeader));fflush(stderr);

                        pthread_mutex_lock(&((ProgData *)pdata)->yuv_mutex);

                        //load the blocks for each buffer
                        if(frame.header->Ynum)
                            for(j=0;j<frame.header->Ynum;j++)
                                LoadBlock(  yuv->y,
                                            &frame.YData[j*blockszy],
                                            frame.YBlocks[j],
                                            yuv->y_width,
                                            yuv->y_height,
                                            divisor);
                        if(frame.header->Unum)
                            for(j=0;j<frame.header->Unum;j++)
                                LoadBlock(  yuv->u,
                                            &frame.UData[j*blockszuv],
                                            frame.UBlocks[j],
                                            yuv->uv_width,
                                            yuv->uv_height,
                                            divisor/2);
                        if(frame.header->Vnum)
                            for(j=0;j<frame.header->Vnum;j++)
                                LoadBlock(  yuv->v,
                                            &frame.VData[j*blockszuv],
                                            frame.VBlocks[j],
                                            yuv->uv_width,
                                            yuv->uv_height,
                                            divisor/2);
                        pthread_mutex_unlock(&((ProgData *)pdata)->yuv_mutex);

                        while(encoder_busy){
                            usleep(100);
                        }
                        pthread_cond_broadcast(&((ProgData *)pdata)->image_buffer_ready);
                        while(encoder_busy){
                            usleep(100);
                        }
                    }
                else{
//                     first=0;
                    raise(SIGINT);
                    continue;
                }
            }
            else{
//                 first=0;
                raise(SIGINT);
                continue;
            }

//         }


                //call loading func{
                    //if avd>=0
                    //load image from cache
                    //signal buffer
                    ////if avd<0
                    //load sound from cache
                    //signal buffer
                //}

//         sleep(1);
    }

    CLEAR_FRAME(&frame)
    gzclose(ifp);

    if(remove(((ProgData *)pdata)->cache_data->imgdata)){
        fprintf(stderr,"Couldn't remove temporary file %s",((ProgData *)pdata)->cache_data->imgdata);
        thread_exit=1;
    }
    if(!((ProgData *)pdata)->args.nosound){
        fclose(afp);
        if(remove(((ProgData *)pdata)->cache_data->audiodata)){
            fprintf(stderr,"Couldn't remove temporary file %s",((ProgData *)pdata)->cache_data->audiodata);
            thread_exit=1;
        }
    }
    if(remove(((ProgData *)pdata)->cache_data->projname)){
        fprintf(stderr,"Couldn't remove temporary directory %s",((ProgData *)pdata)->cache_data->projname);
        thread_exit=1;
    }

    pthread_exit(&thread_exit);


}
 
