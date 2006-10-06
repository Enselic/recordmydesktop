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


int ParseArgs(int argc,char **argv,ProgArgs *arg_return){
    int i;
    char *usage="\nUsage:\n"
    "\trecordmydesktop [-h| --help| --version| -delay n[H|h|M|m]| -windowid id_of_window|\n"
    "\t-display DISPLAY| -x X| -y Y|-width N| -height N| -fps N(number>0)|\n"
    "\t -v_quality n| -s_quality n| -v_bitrate n| --no-framedrop| -dummy-cursor color|\n"
    "\t --no-cursor| -freq N(number>0)| -channels N(number>0)| -device SOUND_DEVICE|\n"
    "\t --nosound| --with-shared| --no-cond-shared| -shared-threshold n| --full-shots|\n"
    "\t --quick-subsampling| --scshot| -scale-shot N| -o filename]^filename\n\n\n"

    "General Options:\n"
    "\t-h or --help\t\tPrint this help and exit.\n"
    "\t--version\t\tPrint program version and exit.\n\n"

    "Image Options:\n"
    "\t-windowid id_of_window\tid of window to be recorded.\n"
    "\t-display DISPLAY\tDisplay to connect to.\n"
    "\t-x X\t\t\tOffset in x direction.\n"
    "\t-y Y\t\t\tOffset in y direction.\n"
    "\t-width N\t\tWidth of recorded window.\n"
    "\t-height N\t\tHeight of recorded window.\n\n"

    "\t-dummy-cursor color\tColor of the dummy cursor [black|white]\n"
    "\t--no-cursor\tDisable drawing of the cursor.\n"
    "\t--with-shared\t\tEnable usage of MIT-shared memory extension at all times.\n"
    "\t--no-cond-shared\tDo not use the MIT-shared memory extension when aquiring large areas.\n"
    "\t-shared-threshold n\tThreshold over which shared memory is used(default 75).\n"
    "\t--full-shots\t\tTake full screenshot at every frame(Not recomended!).\n"
    "\t--quick-subsampling\tDo subsampling of the chroma planes by discarding,not averaging.\n"
    "\t-fps N(number>0.0)\tA positive number denoting desired framerate.\n\n"

    "Sound Options:\n"
    "\t-channels N(number>0)\tA positive number denoting desired sound channels in recording.\n"
    "\t-freq N(number>0)\tA positive number denoting desired sound frequency.\n"
    "\t-device SOUND_DEVICE\tSound device(default hw0:0).\n"
    "\t--nosound\t\tDo not record sound.\n\n"

    "Encoding Options\n"
    "\t-v_quality n\t\tA number from 0 to 63 for desired encoded video quality(default 63).\n"
    "\t-v_bitrate n\t\tA number from 45000 to 2000000 for desired encoded video bitrate(default 45000).\n"
    "\t--drop-frames\t\tAllow theora encoder to drop frames.\n"
    "\t-s_quality n\t\tDesired audio quality(-1 to 10).\n\n"

    "Misc Options:\n"
    "\t-delay n[H|h|M|m]\tNumber of secs(default),minutes or hours before capture starts(number can be float)\n"
    "\t--scshot\t\tTake a bitmap screenshot(default rmdout.bmp) and exit.\n"
    "\t-scale-shot N\t\tFactor by which screenshot is scaled down(1<=number<=64,power of 2).\n"
    "\t-o filename\t\tName of recorded video(default out.ogg).\n"
    "\n\tIf no other options are specified, filename can be given without the -o switch.\n\n\n";

    if(argc==2){
        if(argv[1][0]!='-'){
            free(arg_return->filename);
            arg_return->filename=malloc(strlen(argv[1])+1);
            strcpy(arg_return->filename,argv[1]);
            return 0;
        }
    }
    for(i=1;i<argc;i++){
        if(!strcmp(argv[i],"-delay")){
            if(i+1<argc){
                float num=atof(argv[i+1]);
                if(num>0.0){
                    int k;
                    for(k=0;k<strlen(argv[i+1]);k++){
                        if((argv[i+1][k]=='M')||(argv[i+1][k]=='m')){
                            num*=60.0;
                            break;
                        }
                        else if((argv[i+1][k]=='H')||(argv[i+1][k]=='h')){
                            num*=3600.0;
                            break;
                        }
                    }
                    arg_return->delay=(int)num;
                }
                else{
                    fprintf(stderr,"Argument Usage: -delay n[H|h|M|m]\nwhere n is a float number\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -delay n[H|h|M|m]\nwhere n is a float number\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-windowid")){
            if(i+1<argc){
                Window num=strtod(argv[i+1],NULL);
                if(num>0)
                    arg_return->windowid=num;
                else{
                    fprintf(stderr,"Argument Usage: -windowid id_of_window(number)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -windowid id_of_window(number)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-display")){
            if(i+1<argc){
                if(arg_return->display!=NULL)
                    free(arg_return->display);
                arg_return->display=malloc(strlen(argv[i+1])+1);
                strcpy(arg_return->display,argv[i+1]);
            }
            else{
                fprintf(stderr,"Argument Usage: -display DISPLAY\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-x")){
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if(num>0)
                    arg_return->x=num;
                else{
                    fprintf(stderr,"Argument Usage: -x X(number>0)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -x X(number>0)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-y")){
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if(num>0)
                    arg_return->y=num;
                else{
                    fprintf(stderr,"Argument Usage: -y Y(number>0)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -y Y(number>0)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-width")){
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if(num>0)
                    arg_return->width=num;
                else{
                    fprintf(stderr,"Argument Usage: -width N(number>0)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -width N(number>0)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-height")){
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if(num>0)
                    arg_return->height=num;
                else{
                    fprintf(stderr,"Argument Usage: -height N(number>0)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -height N(number>0)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-o")){
            if(i+1<argc){
                free(arg_return->filename);
                arg_return->filename=malloc(strlen(argv[i+1])+1);
                strcpy(arg_return->filename,argv[i+1]);
            }
            else{
                fprintf(stderr,"Argument Usage: -o filename\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-fps")){
            if(i+1<argc){
                float num=atof(argv[i+1]);
                if(num>0.0)
                    arg_return->fps=num;
                else{
                    fprintf(stderr,"Argument Usage: -fps N(number>0)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -fps N(number>0)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-v_quality")){
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if((num>=0)&&(num<64))
                    arg_return->v_quality=num;
                else{
                    fprintf(stderr,"Argument Usage: -v_quality n(number 0-63)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -v_quality n(number 0-63)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-v_bitrate")){
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if((num>=45000)&&(num<=2000000))
                    arg_return->v_bitrate=num;
                else{
                    fprintf(stderr,"Argument Usage: -v_bitrate n(number 45000-2000000)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -v_bitrate n(number 45000-2000000)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-dummy-cursor")){
            if(i+1<argc){
                if(!strcmp(argv[i+1],"white"))
                    arg_return->cursor_color=0;
                else if(!strcmp(argv[i+1],"black"))
                    arg_return->cursor_color=1;
                else{
                    fprintf(stderr,"Argument Usage: -dummy-cursor [black|white]\n");
                    return 1;
                }
                arg_return->have_dummy_cursor=1;
                arg_return->xfixes_cursor=0;    
            }
            else{
                fprintf(stderr,"Argument Usage: -dummy-cursor [black|white]\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"--no-cursor"))
            arg_return->xfixes_cursor=0;
        else if(!strcmp(argv[i],"-freq")){
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if(num>0)
                    arg_return->frequency=num;
                else{
                    fprintf(stderr,"Argument Usage: -freq N(number>0)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -freq N(number>0)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-channels")){
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if(num>0)
                    arg_return->channels=num;
                else{
                    fprintf(stderr,"Argument Usage: -channels N(number>0)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -channels N(number>0)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-s_quality")){
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if((num>=-1)&&(num<=10))
                    arg_return->s_quality=num;
                else{
                    fprintf(stderr,"Argument Usage: -s_quality n(number -1 to 10)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -s_quality n(number -1 to 10)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-shared-threshold")){
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if((num>0)&&(num<100))
                    arg_return->shared_thres=num;
                else{
                    fprintf(stderr,"Argument Usage: -shared-threshold N(0<number<100)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -shared-threshold N(0<number<100)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-scale-shot")){
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if((num==1)||(num==2)||(num==4)||(num==8)
                ||(num==16)||(num==32)||(num==64)){
                    arg_return->scale_shot=num;
                }
                else{
                    fprintf(stderr,"Argument Usage: -scale-shot N(0<number<64,power of 2)\n");
                    return 1;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -scale-shot N(0<number<64,power of 2)\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"-device")){
            if(i+1<argc){
                free(arg_return->device);
                arg_return->device=malloc(strlen(argv[i+1])+1);
                strcpy(arg_return->device,argv[i+1]);
            }
            else{
                fprintf(stderr,"Argument Usage: -device SOUND_DEVICE\n");
                return 1;
            }
            i++;
        }
        else if(!strcmp(argv[i],"--nosound"))
            arg_return->nosound=1;
        else if(!strcmp(argv[i],"--drop-frames"))
            arg_return->dropframes=1;
        else if(!strcmp(argv[i],"--with-shared")){
            arg_return->noshared=0;
            arg_return->nocondshared=1;
        }
        else if(!strcmp(argv[i],"--no-cond-shared"))
            arg_return->nocondshared=1;
        else if(!strcmp(argv[i],"--full-shots")){
            arg_return->full_shots=1;
            arg_return->nocondshared=1;
        }
        else if(!strcmp(argv[i],"--scshot")){
            arg_return->scshot=1;
            arg_return->nocondshared=1;
        }
        else if(!strcmp(argv[i],"--quick-subsampling")){
            arg_return->no_quick_subsample=0;
        }
        else if(!strcmp(argv[i],"--help")||!strcmp(argv[i],"-h")){
            fprintf(stderr,"%s",usage);
            return 1;
        }
        else if(!strcmp(argv[i],"--version")){
            fprintf(stderr,"recordMyDesktop v%s\n\n",VERSION);
            return 1;
        }
        else{
            fprintf(stderr,"\n\tError parsing arguments.\n\tType --help or -h for usage.\n\n");
            return 1;
        }    
    }
    return 0;
}
