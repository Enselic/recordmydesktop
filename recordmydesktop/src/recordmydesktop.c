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


int main(int argc,char **argv){
    ProgData pdata;

    exit_status=0;
    if(XInitThreads ()==0){
        fprintf(stderr,"Couldn't initialize thread support!\n");
        exit(7);
    }
    DEFAULT_ARGS(&pdata.args);
    if(ParseArgs(argc,argv,&pdata.args)){
        exit(1);
    }
    if(pdata.args.display!=NULL)
        pdata.dpy = XOpenDisplay(pdata.args.display);
    else{
        fprintf(stderr,"No display specified for connection!\n");
        exit(8);
    }
    if (pdata.dpy == NULL) {
        fprintf(stderr, "Cannot connect to X server %s\n",pdata.args.display);
        exit(9);
    }
    else{
        EncData enc_data;
        CacheData cache_data;
#ifdef HAVE_JACK_H
        JackData jdata;
        pdata.jdata=&jdata;
#endif
        QUERY_DISPLAY_SPECS(pdata.dpy,&pdata.specs);
        if((pdata.specs.depth!=32)&&
           (pdata.specs.depth!=24)&&
           (pdata.specs.depth!=16)){
            fprintf(stderr,"Only 32bpp,24bpp and 16bpp"
                           " color depth modes are currently supported.\n");
            exit(10);
        }
        if(SetBRWindow(pdata.dpy,&pdata.brwin,&pdata.specs,&pdata.args))
            exit(11);

        //check if we are under compiz or beryl,
        //in which case we must enable full-shots
        //and with it use of shared memory.User can override this
        pdata.window_manager=((pdata.args.nowmcheck)?
                              NULL:rmdWMCheck(pdata.dpy,pdata.specs.root));
        if(pdata.window_manager==NULL){
            fprintf(stderr,"Not taking window manager into account.\n");
        }
        //Right now only wm's that I know of performing
        //3d compositing are beryl and compiz.
        //No, the blue screen in metacity doesn't count :)
        //names can be compiz for compiz and beryl/beryl-co/beryl-core
        //for beryl(so it's strncmp )
        else if(!strcmp(pdata.window_manager,"compiz") ||
                !strncmp(pdata.window_manager,"beryl",5)){
            fprintf(stderr,"\nDetected 3d compositing window manager.\n"
                           "Reverting to full screen capture at every frame.\n"
                           "To disable this check run with --no-wm-check\n"
                           "(though that is not advised, since it will "
                           "probably produce faulty results).\n\n");
            pdata.args.full_shots=1;
            pdata.args.noshared=0;
        }

        QueryExtensions(pdata.dpy,
                        &pdata.args,
                        &pdata.damage_event,
                        &pdata.damage_error,
                        &pdata.shm_opcode);


        if((exit_status=InitializeData(&pdata,&enc_data,&cache_data))==0){

            u_int32_t   y_t[(pdata.enc_data->yuv.y_width/Y_UNIT_WIDTH)*
                        (pdata.enc_data->yuv.y_height/Y_UNIT_WIDTH)],
                        u_t[(pdata.enc_data->yuv.y_width/Y_UNIT_WIDTH)*
                        (pdata.enc_data->yuv.y_height/Y_UNIT_WIDTH)],
                        v_t[(pdata.enc_data->yuv.y_width/Y_UNIT_WIDTH)*
                        (pdata.enc_data->yuv.y_height/Y_UNIT_WIDTH)];
            yblocks=y_t;
            ublocks=u_t;
            vblocks=v_t;


            //this is where the capturing happens.
            rmdThreads(&pdata);


            fprintf(stderr,".");

            fprintf(stderr,"\n");

            //encode and then cleanup cache
            if(!pdata.args.encOnTheFly){
                if(!Aborted){
                    EncodeCache(&pdata);
                }
                fprintf(stderr,"Cleanning up cache...\n");
                if(PurgeCache(pdata.cache_data,!pdata.args.nosound))
                    fprintf(stderr,"Some error occured "
                                "while cleaning up cache!\n");
                fprintf(stderr,"Done!!!\n");
            }

            //This can happen earlier, but in some cases it might get stuck.
            //So we must make sure the recording is not wasted.
            XCloseDisplay(pdata.dpy);

            if(Aborted && pdata.args.encOnTheFly){
                if(remove(pdata.args.filename)){
                    perror("Error while removing file:\n");
                    return 1;
                }
                else{
                    fprintf(stderr,"SIGABRT received,file %s removed\n",
                                pdata.args.filename);
                    return 0;
                }
            }
            else
                fprintf(stderr,"Goodbye!\n");
        }
    }
    return exit_status;
}















