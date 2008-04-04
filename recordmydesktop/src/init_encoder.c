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


#include <recordmydesktop.h>
#include <string.h>
#include <skeleton.h>

void m_add_fishead_packet(ogg_stream_state *m_ogg_state){

    fishead_packet skel_fp;
    
    skel_fp.ptime_n=skel_fp.btime_n=0;
    skel_fp.ptime_d=skel_fp.btime_d=1000;
    
    add_fishead_to_stream(m_ogg_state,&skel_fp);
    

}


int IncrementalNaming(char **name){
    struct stat buff;
    char *base_name__;
    int i=0,
        fname_length=strlen(*name)-4;

    base_name__=malloc(fname_length+1);
    strncpy(base_name__,*name,fname_length);
    base_name__[fname_length]='\0';


    //this will go on an endless loop if you have 65536?
    //files with the same name
    //or it will crash and die.anyone interested in trying ?
    while (stat(*name,&buff)==0){
        //create new name
        char *tname=malloc(strlen(*name)+10);
        char numbuf[8];

        strcpy(tname,base_name__);
        strcat(tname,"-");
        i++;
        I16TOA((i),numbuf)
        strcat(tname,numbuf);
        strcat(tname,".ogv");
        //save new name

        free(*name);
        *name=malloc(strlen(tname)+1);
        strcpy(*name,tname);
        free(tname);
    }

    free(base_name__);
    return 0;
}

void InitEncoder(ProgData *pdata,EncData *enc_data_t,int buffer_ready){
    
    int y0,
        y1,
        y2,
        fname_length;
    ogg_stream_state m_ogg_skel;
    ogg_page skel_og_pg;
    fisbone_packet skel_fbv,    //video fisbone packet
                   skel_fba ;   //audio fisbone packet

    (pdata)->enc_data=enc_data_t;

    fname_length=strlen(pdata->args.filename);
    if(!(fname_length>4 &&
       pdata->args.filename[fname_length-4] == '.' &&
       (pdata->args.filename[fname_length-3] == 'o' ||
        pdata->args.filename[fname_length-3] == 'O') &&
       (pdata->args.filename[fname_length-2] == 'g' ||
        pdata->args.filename[fname_length-2] == 'G') &&
       (pdata->args.filename[fname_length-1] == 'v' ||
        pdata->args.filename[fname_length-1] == 'V'))){
    
        char *new_name=malloc(fname_length+5);
        strcpy(new_name,pdata->args.filename);
        strcat(new_name,".ogv");
        
        free(pdata->args.filename);
        pdata->args.filename=new_name;


    }
        
    if(!pdata->args.overwrite)
        IncrementalNaming(&(pdata)->args.filename);
    enc_data_t->fp=fopen((pdata)->args.filename,"w");
    if(enc_data_t->fp==NULL){
        fprintf(stderr,"Cannot open file %s for writting!\n",
                       (pdata)->args.filename);
        exit(13);
    }

    //each stream must have a unique 
    srand(time(NULL));
    y0=rand()+1;
    y1=rand()+1;
    y2=rand()+1;
    y2+=(y1==y2);
    y0=(((y0==y1)||(y0==y2))?(y1+y2):y0);

    //init ogg streams
    //skeleton first
    ogg_stream_init(&m_ogg_skel,y0);
    m_add_fishead_packet(&m_ogg_skel);
	if(ogg_stream_pageout(&m_ogg_skel,&skel_og_pg)!= 1){
        fprintf (stderr, "Internal Ogg library error.\n");
        exit (2);
    }
    fwrite(skel_og_pg.header,1,skel_og_pg.header_len,enc_data_t->fp);
    fwrite(skel_og_pg.body,1,skel_og_pg.body_len,enc_data_t->fp);
    


    ogg_stream_init(&enc_data_t->m_ogg_ts,y1);
    if(!pdata->args.nosound)
        ogg_stream_init(&enc_data_t->m_ogg_vs,y2);



    theora_info_init(&enc_data_t->m_th_inf);
    enc_data_t->m_th_inf.frame_width=(pdata)->brwin.rgeom.width;
    enc_data_t->m_th_inf.frame_height=(pdata)->brwin.rgeom.height;
    enc_data_t->m_th_inf.width=((enc_data_t->m_th_inf.frame_width+15)>>4)<<4;
    enc_data_t->m_th_inf.height=((enc_data_t->m_th_inf.frame_height+15)>>4)<<4;
    enc_data_t->m_th_inf.offset_x=((enc_data_t->m_th_inf.width-
                                     enc_data_t->m_th_inf.frame_width)/2)&~1;
    enc_data_t->m_th_inf.offset_y=((enc_data_t->m_th_inf.height-
                                     enc_data_t->m_th_inf.frame_height)/2)&~1;
    enc_data_t->m_th_inf.fps_numerator=((pdata)->args.fps*100.0);
    enc_data_t->m_th_inf.fps_denominator=100;
    enc_data_t->m_th_inf.aspect_numerator=1;//(pdata)->brwin.rgeom.width;
    enc_data_t->m_th_inf.aspect_denominator=1;//(pdata)->brwin.rgeom.height;
    enc_data_t->m_th_inf.colorspace=OC_CS_UNSPECIFIED;
    enc_data_t->m_th_inf.pixelformat=OC_PF_420;
    enc_data_t->m_th_inf.target_bitrate=(pdata)->args.v_bitrate;
    enc_data_t->m_th_inf.quality=(pdata)->args.v_quality;
    enc_data_t->m_th_inf.dropframes_p=0;
    enc_data_t->m_th_inf.quick_p=1;
    enc_data_t->m_th_inf.keyframe_auto_p=1;
    enc_data_t->m_th_inf.keyframe_frequency=64;
    enc_data_t->m_th_inf.keyframe_frequency_force=64;
    enc_data_t->m_th_inf.keyframe_data_target_bitrate=
    enc_data_t->m_th_inf.quality*1.5;

    enc_data_t->m_th_inf.keyframe_auto_threshold=80;
    enc_data_t->m_th_inf.keyframe_mindistance=8;
    enc_data_t->m_th_inf.noise_sensitivity=1;
    enc_data_t->m_th_inf.sharpness=2;

    theora_encode_init(&enc_data_t->m_th_st,&enc_data_t->m_th_inf);



    if(!pdata->args.nosound){
        int ret;
        vorbis_info_init(&enc_data_t->m_vo_inf);
        ret = vorbis_encode_init_vbr(&enc_data_t->m_vo_inf,
                                     pdata->args.channels,
                                     pdata->args.frequency,
                                     (float)pdata->args.s_quality*0.1);
        if(ret){
            fprintf(stderr,"Error while setting up vorbis stream quality!\n");
            exit(2);
        }
        vorbis_comment_init(&enc_data_t->m_vo_cmmnt);
        vorbis_analysis_init(&enc_data_t->m_vo_dsp,&enc_data_t->m_vo_inf);
        vorbis_block_init(&enc_data_t->m_vo_dsp,&enc_data_t->m_vo_block);
    }


    theora_encode_header(&enc_data_t->m_th_st,&enc_data_t->m_ogg_pckt1);
    ogg_stream_packetin(&enc_data_t->m_ogg_ts,&enc_data_t->m_ogg_pckt1);
    if(ogg_stream_pageout(&enc_data_t->m_ogg_ts,&enc_data_t->m_ogg_pg)!=1){
        fprintf(stderr,"Internal Ogg library error.\n");
        exit(2);
    }
    fwrite(enc_data_t->m_ogg_pg.header,1,
           enc_data_t->m_ogg_pg.header_len,
           enc_data_t->fp);
    fwrite(enc_data_t->m_ogg_pg.body,1,
           enc_data_t->m_ogg_pg.body_len,
           enc_data_t->fp);

    theora_comment_init(&enc_data_t->m_th_cmmnt);
    theora_comment_add_tag(&enc_data_t->m_th_cmmnt,"recordMyDesktop",VERSION);
    theora_encode_comment(&enc_data_t->m_th_cmmnt,&enc_data_t->m_ogg_pckt1);
    ogg_stream_packetin(&enc_data_t->m_ogg_ts,&enc_data_t->m_ogg_pckt1);
    theora_encode_tables(&enc_data_t->m_th_st,&enc_data_t->m_ogg_pckt1);
    ogg_stream_packetin(&enc_data_t->m_ogg_ts,&enc_data_t->m_ogg_pckt1);


    if(!pdata->args.nosound){
        ogg_packet header;
        ogg_packet header_comm;
        ogg_packet header_code;

        vorbis_analysis_headerout(&enc_data_t->m_vo_dsp,
                                  &enc_data_t->m_vo_cmmnt,
                                  &header,&header_comm,
                                  &header_code);
        ogg_stream_packetin(&enc_data_t->m_ogg_vs,&header);
        if(ogg_stream_pageout(&enc_data_t->m_ogg_vs,&enc_data_t->m_ogg_pg)!=1){
            fprintf(stderr,"Internal Ogg library error.\n");
            exit(2);
        }
        fwrite(enc_data_t->m_ogg_pg.header,1,
               enc_data_t->m_ogg_pg.header_len,
               enc_data_t->fp);
        fwrite(enc_data_t->m_ogg_pg.body,1,
               enc_data_t->m_ogg_pg.body_len,
               enc_data_t->fp);

        ogg_stream_packetin(&enc_data_t->m_ogg_vs,&header_comm);
        ogg_stream_packetin(&enc_data_t->m_ogg_vs,&header_code);
    }

    //fishbone packets go here
    memset(&skel_fbv,0,sizeof(skel_fbv));
    skel_fbv.serial_no=enc_data_t->m_ogg_ts.serialno;
    skel_fbv.nr_header_packet=3;
    skel_fbv.granule_rate_n=enc_data_t->m_th_inf.fps_numerator;
    skel_fbv.granule_rate_d=enc_data_t->m_th_inf.fps_denominator;
    skel_fbv.start_granule=0;
    skel_fbv.preroll=0;
    skel_fbv.granule_shift=theora_granule_shift(&enc_data_t->m_th_inf);
    add_message_header_field(&skel_fbv,
                             "Content-Type",
                             "video/theora");

    add_fisbone_to_stream(&m_ogg_skel,&skel_fbv);

    if(!pdata->args.nosound){

        memset(&skel_fba,0,sizeof(skel_fba));
        skel_fba.serial_no=enc_data_t->m_ogg_vs.serialno;
        skel_fba.nr_header_packet=3;
        skel_fba.granule_rate_n=pdata->args.frequency;
        skel_fba.granule_rate_d=(ogg_int64_t)1;
        skel_fba.start_granule=0;
        skel_fba.preroll=2;
        skel_fba.granule_shift=0;
        add_message_header_field(&skel_fba,
                                 "Content-Type",
                                 "audio/vorbis");

        add_fisbone_to_stream(&m_ogg_skel,&skel_fba);
    
    }

    while(1){
        int result = ogg_stream_flush(&m_ogg_skel, &skel_og_pg);
        if(result<0){
            fprintf (stderr, "Internal Ogg library error.\n");
            exit(2);
        }
        if(result==0)
            break;
        fwrite(skel_og_pg.header,1,skel_og_pg.header_len,enc_data_t->fp);
        fwrite(skel_og_pg.body,1,skel_og_pg.body_len,enc_data_t->fp);
	}



    while(1){
        int result = ogg_stream_flush(&enc_data_t->m_ogg_ts,
                                      &enc_data_t->m_ogg_pg);
        if(result<0){
            fprintf(stderr,"Internal Ogg library error.\n");
            exit(2);
        }
        if(result==0)break;
        fwrite(enc_data_t->m_ogg_pg.header,1,
               enc_data_t->m_ogg_pg.header_len,
               enc_data_t->fp);
        fwrite(enc_data_t->m_ogg_pg.body,1,
               enc_data_t->m_ogg_pg.body_len,
               enc_data_t->fp);
    }

    if(!pdata->args.nosound){
        while(1){
            int result=ogg_stream_flush(&enc_data_t->m_ogg_vs,
                                        &enc_data_t->m_ogg_pg);
            if(result<0){
                fprintf(stderr,"Internal Ogg library error.\n");
                exit(2);
            }
            if(result==0)break;
            fwrite(enc_data_t->m_ogg_pg.header,1,
                   enc_data_t->m_ogg_pg.header_len,
                   enc_data_t->fp);
            fwrite(enc_data_t->m_ogg_pg.body,1,
                   enc_data_t->m_ogg_pg.body_len,
                   enc_data_t->fp);
        }
    }
    
    //skeleton eos
    add_eos_packet_to_stream(&m_ogg_skel);
	if(ogg_stream_flush(&m_ogg_skel,&skel_og_pg)<0){
        fprintf(stderr,"Internal Ogg library error.\n");
        exit(2);
    }
    fwrite(skel_og_pg.header,1,skel_og_pg.header_len,enc_data_t->fp);
    fwrite(skel_og_pg.body,1,skel_og_pg.body_len,enc_data_t->fp);


    //theora buffer allocation, if any
    if(!buffer_ready){
        enc_data_t->yuv.y=(unsigned char *)malloc(enc_data_t->m_th_inf.height*
                          enc_data_t->m_th_inf.width);
        enc_data_t->yuv.u=(unsigned char *)malloc(enc_data_t->m_th_inf.height*
                          enc_data_t->m_th_inf.width/4);
        enc_data_t->yuv.v=(unsigned char *)malloc(enc_data_t->m_th_inf.height*
                          enc_data_t->m_th_inf.width/4);
        enc_data_t->yuv.y_width=enc_data_t->m_th_inf.width;
        enc_data_t->yuv.y_height=enc_data_t->m_th_inf.height;
        enc_data_t->yuv.y_stride=enc_data_t->m_th_inf.width;

        enc_data_t->yuv.uv_width=enc_data_t->m_th_inf.width/2;
        enc_data_t->yuv.uv_height=enc_data_t->m_th_inf.height/2;
        enc_data_t->yuv.uv_stride=enc_data_t->m_th_inf.width/2;
        enc_data_t->x_offset=enc_data_t->m_th_inf.offset_x;
        enc_data_t->y_offset=enc_data_t->m_th_inf.offset_y;
    }
    theora_info_clear(&enc_data_t->m_th_inf);

}




