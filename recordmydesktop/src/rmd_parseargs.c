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
#include "rmd_parseargs.h"

#include "rmd_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void PrintConfig(void) {
    fprintf(stderr,"\nrecordMyDesktop was compiled with"
                   " the following options:\n\n");
#ifdef HAVE_LIBJACK
    fprintf(stdout,"Jack\t\t\t:Enabled\n");
#else
    fprintf(stdout,"Jack\t\t\t:Disabled\n");
#endif
#ifdef HAVE_LIBASOUND
    fprintf(stdout,"Default Audio Backend\t:ALSA\n");
#else
    fprintf(stdout,"Default Audio Backend\t:OSS\n");
#endif
    fprintf(stderr,"\n\n");
}

/**
 * Still has backwards compatible options like -this, but --this is
 * what is documented.
 */
boolean ParseArgs(int argc, char **argv, ProgArgs *arg_return) {
    int i;
    char *usage="\nUsage:\n"
    "\trecordmydesktop [OPTIONS]^filename\n\n\n"

    "General Options:\n"
    "\t-h or --help\t\tPrint this help and exit.\n"
    "\t--version\t\tPrint program version and exit.\n"
    "\t--print-config\t\tPrint info about options "
    "selected during compilation and exit.\n\n"

    "Image Options:\n"
    "\t--windowid id_of_window\tid of window to be recorded.\n"
    "\t--display DISPLAY\tDisplay to connect to.\n"
    "\t-x X\t\t\tOffset in x direction.\n"
    "\t-y Y\t\t\tOffset in y direction.\n"
    "\t--width N\t\tWidth of recorded window.\n"
    "\t--height N\t\tHeight of recorded window.\n\n"

    "\t--dummy-cursor color\tColor of the dummy cursor [black|white]\n"
    "\t--no-cursor\t\tDisable drawing of the cursor.\n"
    "\t--no-shared\t\tDisable usage of"
    " MIT-shared memory extension(Not Recommended!).\n"

    "\t--full-shots\t\tTake full screenshot at every frame(Not recomended!).\n"
    "\t--quick-subsampling\tDo subsampling"
    " of the chroma planes by discarding,not averaging.\n"

    "\t--fps N(number>0.0)\tA positive number denoting desired framerate.\n\n"

    "Sound Options:\n"
    "\t--channels N\t\t\tA positive number denoting"
    " desired sound channels in recording.\n"

    "\t--freq N\t\t\t\tA positive number denoting desired sound frequency.\n"
    "\t--buffer-size N\t\t\tA positive number denoting the desired"
    " sound buffer size (in frames,when using ALSA or OSS)\n"
    "\t--ring-buffer-size N\t\tA float number denoting the desired"
    " ring buffer size (in seconds,when using JACK only).\n"

    "\t--device SOUND_DEVICE\t\tSound device(default "
    DEFAULT_AUDIO_DEVICE
    ").\n"
    "\t--use-jack port1 port2... portn\tRecord audio from the specified\n"
    "\t\t\t\t\tlist of space-separated jack ports.\n"
    "\t--no-sound\t\t\tDo not record sound.\n\n"

    "Encoding Options\n"
    "\t--on-the-fly-encoding\tEncode the audio-video data, while recording.\n"
    "\t--v_quality n\t\tA number from 0 to 63 for"
    " desired encoded video quality(default 63).\n"

    "\t--v_bitrate n\t\tA number from 45000 to 2000000"
    " for desired encoded video bitrate(default 45000).\n"

    "\t--s_quality n\t\tDesired audio quality(-1 to 10).\n\n"

    "Misc Options:\n"
    "\t--rescue path_to_data\tEncode data from a previous, crashed, session.\n"
    "\t--no-wm-check\t\tDo not try to detect"
    " the window manager(and set options according to it)\n"

    "\t--pause-shortcut MOD+KEY\tShortcut that will be used for (un)pausing" 
    "(default Control+Mod1+p).\n"
    "\t--stop-shortcut MOD+KEY\tShortcut that will be used to stop the "
    "recording (default Control+Mod1+s).\n" 

    "\t--compress-cache\tImage data are cached with light compression.\n"
    "\t--workdir DIR\t\tLocation where a temporary directory"
    " will be created to hold project files(default $HOME).\n"

    "\t--delay n[H|h|M|m]\tNumber of secs(default),minutes or hours"
    " before capture starts(number can be float)\n"

    "\t--overwrite\t\tIf there is already a file with the same name,"
    " delete it\n"

    "\t\t\t\t(default is to add a number postfix to the new one).\n"
    "\t-o filename\t\tName of recorded video(default out.ogv).\n"
    "\n\tIf no other options are specified, filename can be given "
    "without the -o switch.\n\n\n";

    if(argc==2){
        if(argv[1][0]!='-'){
            free(arg_return->filename);
            arg_return->filename=malloc(strlen(argv[1])+1);
            strcpy(arg_return->filename,argv[1]);
            return TRUE;
        }
    }
    for(i=1;i<argc;i++){
        if (strcmp(argv[i], "--delay") == 0 ||
            strcmp(argv[i], "-delay")  == 0) {
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
                    fprintf(stderr,"Argument Usage: --delay n[H|h|M|m]\n"
                                   "where n is a float number\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: --delay n[H|h|M|m]\n"
                               "where n is a float number\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--windowid") == 0 ||
                 strcmp(argv[i], "-windowid")  == 0) {
            if(i+1<argc){
                Window num=strtod(argv[i+1],NULL);
                if(num>0)
                    arg_return->windowid=num;
                else{
                    fprintf(stderr,"Argument Usage:"
                                   " --windowid id_of_window(number)\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage:"
                               " --windowid id_of_window(number)\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--display") == 0 ||
                 strcmp(argv[i], "-display")  == 0) {
            if(i+1<argc){
                if(arg_return->display!=NULL)
                    free(arg_return->display);
                arg_return->display=malloc(strlen(argv[i+1])+1);
                strcpy(arg_return->display,argv[i+1]);
            }
            else{
                fprintf(stderr,"Argument Usage: --display DISPLAY\n");
                return FALSE;
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
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -x X(number>0)\n");
                return FALSE;
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
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: -y Y(number>0)\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--width") == 0 ||
                 strcmp(argv[i], "-width")  == 0) {
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if(num>0)
                    arg_return->width=num;
                else{
                    fprintf(stderr,"Argument Usage: --width N(number>0)\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: --width N(number>0)\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--height") == 0 ||
                 strcmp(argv[i], "-height")  == 0) {
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if(num>0)
                    arg_return->height=num;
                else{
                    fprintf(stderr,"Argument Usage: --height N(number>0)\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: --height N(number>0)\n");
                return FALSE;
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
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--fps") == 0 ||
                 strcmp(argv[i], "-fps")  == 0) {
            if(i+1<argc){
                float num=atof(argv[i+1]);
                if(num>0.0)
                    arg_return->fps=num;
                else{
                    fprintf(stderr,"Argument Usage: --fps N(number>0)\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: --fps N(number>0)\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--v_quality") == 0 ||
                 strcmp(argv[i], "-v_quality")  == 0) {
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if((num>=0)&&(num<64))
                    arg_return->v_quality=num;
                else{
                    fprintf(stderr,"Argument Usage:"
                                   " --v_quality n(number 0-63)\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage:"
                               " --v_quality n(number 0-63)\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--v_bitrate") == 0 ||
                 strcmp(argv[i], "-v_bitrate")  == 0) {
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if((num>=45000)&&(num<=2000000))
                    arg_return->v_bitrate=num;
                else{
                    fprintf(stderr,"Argument Usage:"
                                   " --v_bitrate n(number 45000-2000000)\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage:"
                               " --v_bitrate n(number 45000-2000000)\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--dummy-cursor") == 0 ||
                 strcmp(argv[i], "-dummy-cursor")  == 0) {
            if(i+1<argc){
                if(!strcmp(argv[i+1],"white"))
                    arg_return->cursor_color=0;
                else if(!strcmp(argv[i+1],"black"))
                    arg_return->cursor_color=1;
                else{
                    fprintf(stderr,"Argument Usage:"
                                   " --dummy-cursor [black|white]\n");
                    return FALSE;
                }
                arg_return->have_dummy_cursor=1;
                arg_return->xfixes_cursor=0;
            }
            else{
                fprintf(stderr,"Argument Usage:"
                               " --dummy-cursor [black|white]\n");
                return FALSE;
            }
            i++;
        }
        else if(!strcmp(argv[i],"--no-cursor"))
            arg_return->xfixes_cursor=0;
        else if (strcmp(argv[i], "--freq") == 0 ||
                 strcmp(argv[i], "-freq")  == 0) {
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if(num>0)
                    arg_return->frequency=num;
                else{
                    fprintf(stderr,"Argument Usage: --freq N(number>0)\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: --freq N(number>0)\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--channels") == 0 ||
                 strcmp(argv[i], "-channels")  == 0) {
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if(num>0)
                    arg_return->channels=num;
                else{
                    fprintf(stderr,"Argument Usage: --channels N(number>0)\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: --channels N(number>0)\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--s_quality") == 0 ||
                 strcmp(argv[i], "-s_quality")  == 0) {
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if((num>=-1)&&(num<=10))
                    arg_return->s_quality=num;
                else{
                    fprintf(stderr,"Argument Usage:"
                                   " --s_quality n(number -1 to 10)\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage:"
                               " --s_quality n(number -1 to 10)\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--device") == 0 ||
                 strcmp(argv[i], "-device")  == 0) {
            if(i+1<argc){
                free(arg_return->device);
                arg_return->device=malloc(strlen(argv[i+1])+1);
                strcpy(arg_return->device,argv[i+1]);
            }
            else{
                fprintf(stderr,"Argument Usage: -device SOUND_DEVICE\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--workdir") == 0 ||
                 strcmp(argv[i], "-workdir")  == 0) {
            if(i+1<argc){
                free(arg_return->workdir);
                arg_return->workdir=malloc(strlen(argv[i+1])+1);
                strcpy(arg_return->workdir,argv[i+1]);
            }
            else{
                fprintf(stderr,"Argument Usage: --workdir DIR\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--pause-shortcut") == 0 ||
                 strcmp(argv[i], "-pause-shortcut")  == 0) {
            if(i+1<argc){
                free(arg_return->pause_shortcut);
                arg_return->pause_shortcut=malloc(strlen(argv[i+1])+1);
                strcpy(arg_return->pause_shortcut,argv[i+1]);
            }
            else{
                fprintf(stderr,"Argument Usage: --pause-shortcut MOD+KEY\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--stop-shortcut") == 0 ||
                 strcmp(argv[i], "-stop-shortcut")  == 0) {
            if(i+1<argc){
                free(arg_return->stop_shortcut);
                arg_return->stop_shortcut=malloc(strlen(argv[i+1])+1);
                strcpy(arg_return->stop_shortcut,argv[i+1]);
            }
            else{
                fprintf(stderr,"Argument Usage: --stop-shortcut MOD+KEY\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--buffer-size") == 0 ||
                 strcmp(argv[i], "-buffer-size")  == 0) {
            if(i+1<argc){
                int num=atoi(argv[i+1]);
                if(num>0)
                    arg_return->buffsize=num;
                else{
                    fprintf(stderr,"Argument Usage:"
                                   " --buffer-size N(number>0)\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: --buffer-size N(number>0)\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--use-jack") == 0 ||
                 strcmp(argv[i], "-use-jack")  == 0) {
            if(i+1<argc){
#ifdef HAVE_LIBJACK
                int k=i+1;
                arg_return->jack_nports=0;
                while((k<argc)&&(argv[k][0]!='-')){
                    arg_return->jack_nports++;
                    k++;
                }
                if(arg_return->jack_nports>0){
                    arg_return->jack_port_names=malloc(sizeof(char*)*
                                                       arg_return->jack_nports);
                    for(k=i+1;k<i+1+arg_return->jack_nports;k++){
                        arg_return->jack_port_names[k-i-1]=
                            malloc(strlen(argv[k])+1);
                        strcpy(arg_return->jack_port_names[k-i-1],
                               argv[k]);
                    }
                    i+=arg_return->jack_nports;
                    arg_return->use_jack=1;
                }
                else{
                    fprintf(stderr,"Argument Usage: --use-jack port1"
                                   " port2... portn\n");
                    return FALSE;
                }
#else
                fprintf(stderr,"recordMyDesktop is not compiled"
                               " with Jack support!\n");
                return FALSE;
#endif
            }
            else{
                fprintf(stderr,"Argument Usage: --use-jack port1"
                               " port2... portn\n");
                return FALSE;
            }
        }
        else if (strcmp(argv[i], "--ring-buffer-size") == 0 ||
                 strcmp(argv[i], "-ring-buffer-size")  == 0) {
            if(i+1<argc){
                float num=atof(argv[i+1]);
                if(num>0.0)
                    arg_return->jack_ringbuffer_secs=num;
                else{
                    fprintf(stderr,"Argument Usage: --ring-buffer-size"
                                   " N(floating point number>0.0)\n");
                    return FALSE;
                }
            }
            else{
                fprintf(stderr,"Argument Usage: --ring-buffer-size"
                                " N(floating point number>0.0)\n");
                return FALSE;
            }
            i++;
        }
        else if (strcmp(argv[i], "--rescue") == 0 ||
                 strcmp(argv[i], "-rescue") == 0) {
            if(i+1<argc){
                arg_return->rescue_path = argv[i + 1];
            }
            else{
                fprintf(stderr,"Argument Usage: --rescue path_to_data\n");
                return FALSE;
            }
            i++;
        }
        else if(!strcmp(argv[i],"--no-sound"))
            arg_return->nosound=1;
        else if(!strcmp(argv[i],"--no-shared")){
            arg_return->noshared=1;
        }
        else if(!strcmp(argv[i],"--full-shots")){
            arg_return->full_shots=1;
        }
        else if(!strcmp(argv[i],"--no-frame")){
            arg_return->noframe=1;
        }
        else if(!strcmp(argv[i],"--follow-mouse")){
            arg_return->full_shots=1;
            arg_return->follow_mouse=1;
        }
        else if(!strcmp(argv[i],"--no-encode"))
            arg_return->no_encode=1;
        else if(!strcmp(argv[i],"--quick-subsampling")){
            arg_return->no_quick_subsample=0;
        }
        else if(!strcmp(argv[i],"--on-the-fly-encoding")){
            arg_return->encOnTheFly=1;
        }
        else if(!strcmp(argv[i],"--overwrite"))
            arg_return->overwrite=1;
        else if(!strcmp(argv[i],"--no-wm-check"))
            arg_return->nowmcheck=1;
        else if(!strcmp(argv[i],"--compress-cache")){
            arg_return->zerocompression=0;
        }
        else if(!strcmp(argv[i],"--help")||!strcmp(argv[i],"-h")){
            fprintf(stderr,"%s",usage);
            exit(0);
        }
        else if(!strcmp(argv[i],"--version")){
            fprintf(stderr,"recordMyDesktop v%s\n\n",VERSION);
            exit(0);
        }
        else if(!strcmp(argv[i],"--print-config")){
            PrintConfig();
            exit(0);
        }
        else{
            fprintf(stderr,"\n\tError parsing arguments.\n\t"
                           "Type --help or -h for usage.\n\n");
            return FALSE;
        }
    }
    
    return TRUE;
}
