/******************************************************************************
*                            recordMyDesktop                                  *
*******************************************************************************
*                                                                             *
*            Copyright (C) 2006,2007,2008 John Varouhakis                     *
*            Copyright (C) 2009           Martin Nordholts                    *
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

#include <popt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef HAVE_LIBJACK
#define RMD_LIBJACK_STATUS "Enabled"
#define RMD_USE_JACK_EXTRA_FLAG 0
#else
#define RMD_LIBJACK_STATUS "Disabled"
#define RMD_USE_JACK_EXTRA_FLAG POPT_ARGFLAG_DOC_HIDDEN
#endif

#ifdef HAVE_LIBASOUND
#define RMD_LIBASOUND_STATUS "ALSA"
#else
#define RMD_LIBASOUND_STATUS "OSS"
#endif

#define RMD_ARG_DELAY 1
#define RMD_ARG_DUMMY_CURSOR 2
#define RMD_ARG_USE_JACK 3

#define RMD_OPTION_TABLE(name, table) \
        { NULL, '\0', \
          POPT_ARG_INCLUDE_TABLE, (table), \
          0, (name), NULL }


static void rmdPrintAndExit(poptContext con, enum poptCallbackReason reason, const struct poptOption *opt, const char *arg, const void *data);
static boolean rmdValidateArguments(const ProgArgs *args);


boolean rmdParseArgs(int argc, char **argv, ProgArgs *arg_return) {
    poptContext popt_context = NULL;
    boolean no_cursor = FALSE;
    boolean quick_subsampling = FALSE;
    boolean compress_cache = FALSE;
    boolean success = TRUE;
    int arg_id = 0;

    // Setup the option tables
    struct poptOption generic_options[] = {
        { NULL, '\0',
          POPT_ARG_CALLBACK, (void *)rmdPrintAndExit, 0,
          NULL, NULL },

        { "help", 'h',
          POPT_ARG_NONE, NULL, 0,
          "Print this help and exit.",
          NULL },

        { "version", '\0',
          POPT_ARG_NONE, NULL, 0,
          "Print program version and exit.",
          NULL },

        { "print-config", '\0',
          POPT_ARG_NONE, NULL, 0,
          "Print info about options selected during compilation and exit.",
          NULL },

        POPT_TABLEEND };

    struct poptOption image_options[] = {
        { "windowid", '\0',
          POPT_ARG_INT, &arg_return->windowid, 0,
          "id of window to be recorded.",
          "id_of_window" },

        { "display", '\0',
          POPT_ARG_STRING, &arg_return->display, 0,
          "Display to connect to.",
          "DISPLAY" },

        { "x", 'x',
          POPT_ARG_INT, &arg_return->x, 0,
          "Offset in x direction.",
          "N>=0" },

        { "x", 'y',
          POPT_ARG_INT, &arg_return->y, 0,
          "Offset in y direction.",
          "N>=0" },

        { "width", '\0',
          POPT_ARG_INT, &arg_return->width, 0,
          "Width of recorded window.",
          "N>0" },

        { "height", '\0',
          POPT_ARG_INT, &arg_return->height, 0,
          "Height of recorded window.",
          "N>0" },

        { "dummy-cursor", '\0',
          POPT_ARG_STRING, NULL, RMD_ARG_DUMMY_CURSOR,
          "Color of the dummy cursor [black|white]",
          "color" },

        { "no-cursor", '\0',
          POPT_ARG_NONE, &no_cursor, 0,
          "Disable drawing of the cursor.",
          NULL },

        { "no-shared", '\0',
          POPT_ARG_NONE, &arg_return->noshared, 0,
          "Disable usage of MIT-shared memory extension(Not Recommended!).",
          NULL },

        { "full-shots", '\0',
          POPT_ARG_NONE, &arg_return->full_shots, 0,
          "Take full screenshot at every frame(Not recomended!).",
          NULL },

        { "follow-mouse", '\0',
          POPT_ARG_NONE, &arg_return->follow_mouse, 0,
          "Makes the capture area follow the mouse cursor. Autoenables --full-shots.",
          NULL },

        { "quick-subsampling", '\0',
          POPT_ARG_NONE, &quick_subsampling, 0,
          "Do subsampling of the chroma planes by discarding, not averaging.",
          NULL },

        { "fps", '\0',
          POPT_ARG_FLOAT, &arg_return->fps, 0,
          "A positive number denoting desired framerate.",
          "N(number>0.0)" },

        POPT_TABLEEND };

    struct poptOption sound_options[] = {
        { "channels", '\0',
          POPT_ARG_INT, &arg_return->channels, 0,
          "A positive number denoting desired sound channels in recording.",
          "N" },

        { "freq", '\0',
          POPT_ARG_INT, &arg_return->frequency, 0,
          "A positive number denoting desired sound frequency.",
          "N" },

        { "buffer-size", '\0',
          POPT_ARG_INT, &arg_return->buffsize, 0,
          "A positive number denoting the desired sound buffer size (in frames,when using ALSA or OSS)",
          "N" },

        { "ring-buffer-size", '\0',
          POPT_ARG_FLOAT, &arg_return->jack_ringbuffer_secs, 0,
          "A float number denoting the desired ring buffer size (in seconds,when using JACK only).",
          "N" },

        { "device", '\0',
          POPT_ARG_STRING, &arg_return->device, 0,
          "Sound device(default " DEFAULT_AUDIO_DEVICE ").",
          "SOUND_DEVICE" },

        { "use-jack", '\0',
          POPT_ARG_STRING | RMD_USE_JACK_EXTRA_FLAG, &arg_return->x, RMD_ARG_USE_JACK,
          "Record audio from the specified list of space-separated jack ports.",
          "port1 port2... portn" },

        { "no-sound", '\0',
          POPT_ARG_NONE, &arg_return->nosound, 0,
          "Do not record sound.",
          NULL },

        POPT_TABLEEND };

    struct poptOption encoding_options[] = {
        { "on-the-fly-encoding", '\0',
          POPT_ARG_NONE, &arg_return->encOnTheFly, 0,
          "Encode the audio-video data, while recording.",
          NULL },

        { "v_quality", '\0',
          POPT_ARG_INT, &arg_return->v_quality, 0,
          "A number from 0 to 63 for desired encoded video quality(default 63).",
          "n" },

        { "v_bitrate", '\0',
          POPT_ARG_INT, &arg_return->v_bitrate, 0,
          "A number from 45000 to 2000000 for desired encoded video bitrate(default 45000).",
          "n" },

        { "s_quality", '\0',
          POPT_ARG_INT, &arg_return->s_quality, 0,
          "Desired audio quality(-1 to 10).",
          "n" },

        POPT_TABLEEND };

    struct poptOption misc_options[] = {
        { "rescue", '\0',
          POPT_ARG_STRING, &arg_return->rescue_path, 0,
          "Encode data from a previous, crashed, session.",
          "path_to_data" },

        { "no-encode", '\0',
          POPT_ARG_NONE | POPT_ARGFLAG_DOC_HIDDEN, &arg_return->no_encode, 0,
          "Do not encode any data after recording is complete. This is instead done manually afterwards with --rescue.",
          NULL },

        { "no-wm-check", '\0',
          POPT_ARG_NONE, &arg_return->nowmcheck, 0,
          "Do not try to detect the window manager(and set options according to it)",
          NULL },

        { "no-frame", '\0',
          POPT_ARG_NONE, &arg_return->noframe, 0,
          "Don not show the frame that visualizes the recorded area.",
          NULL },

        { "pause-shortcut", '\0',
          POPT_ARG_STRING, &arg_return->pause_shortcut, 0,
          "Shortcut that will be used for (un)pausing (default Control+Mod1+p).",
          "MOD+KEY" },

        { "stop-shortcut", '\0',
          POPT_ARG_STRING, &arg_return->stop_shortcut, 0,
          "Shortcut that will be used to stop the recording (default Control+Mod1+s).",
          "MOD+KEY" },

        { "compress-cache", '\0',
          POPT_ARG_NONE, &compress_cache, 0,
          "Image data are cached with light compression.",
          NULL },

        { "workdir", '\0',
          POPT_ARG_STRING, &arg_return->workdir, 0,
          "Location where a temporary directory will be created to hold project files(default $HOME).",
          "DIR" },

        { "delay", '\0',
          POPT_ARG_STRING, &arg_return->delay, RMD_ARG_DELAY,
          "Number of secs(default),minutes or hours before capture starts(number can be float)",
          "n[H|h|M|m]" },

        { "overwrite", '\0',
          POPT_ARG_NONE, &arg_return->overwrite, 0,
          "If there is already a file with the same name, delete it (default is to add a number postfix to the new one).",
          NULL },

        { "output", 'o',
          POPT_ARG_STRING, &arg_return->filename, 0,
          "Name of recorded video(default out.ogv).",
          "filename" },

        POPT_TABLEEND };

    struct poptOption rmd_args[] = {
        RMD_OPTION_TABLE("Generic Options", generic_options),
        RMD_OPTION_TABLE("Image Options", image_options),
        RMD_OPTION_TABLE("Sound Options", sound_options),
        RMD_OPTION_TABLE("Encoding Options", encoding_options),
        RMD_OPTION_TABLE("Misc Options", misc_options),
        POPT_TABLEEND };


    // Some trivial setup
    popt_context = poptGetContext(NULL, argc, (const char **)argv, rmd_args, 0);
    poptSetOtherOptionHelp(popt_context, "[OPTIONS]^filename");


    // Parse the arguments
    while ((arg_id = poptGetNextOpt(popt_context)) > 0) {
        char *arg = poptGetOptArg(popt_context);

        // Most arguments are handled completely by libpopt but we
        // handle some by ourself, namely those in this switch case
        switch (arg_id) {
            case RMD_ARG_DELAY:
            {
                float num = atof(arg);

                if (num > 0.0) {
                    int k;
                    for (k = 0; k < strlen(arg); k++) {
                        if ((arg[k] == 'M') || (arg[k] == 'm')) {
                            num *= 60.0;
                            break;
                        }
                        else if ((arg[k] == 'H') || (arg[k] == 'h')) {
                            num *= 3600.0;
                            break;
                        }
                    }
                    arg_return->delay = (int) num;
                }
                else {
                    fprintf(stderr,
                            "Argument Usage: --delay n[H|h|M|m]\n"
                            "where n is a float number\n");
                    success = FALSE;
                }
            }
            break;

            case RMD_ARG_DUMMY_CURSOR:
            {
                if (!strcmp(arg, "white"))
                    arg_return->cursor_color = 0;
                else if (!strcmp(arg, "black"))
                    arg_return->cursor_color = 1;
                else {
                    fprintf(stderr,
                            "Argument Usage:\n"
                            " --dummy-cursor [black|white]\n");
                    success = FALSE;
                }
                arg_return->have_dummy_cursor = TRUE;
                arg_return->xfixes_cursor = FALSE;
            }
            break;

            case RMD_ARG_USE_JACK:
            {
                arg_return->jack_nports = 0;

                while (arg) {

                    arg_return->jack_nports++;

                    arg_return->jack_port_names[arg_return->jack_nports - 1] = malloc(strlen(arg) + 1);
                    strcpy(arg_return->jack_port_names[arg_return->jack_nports - 1], arg);

                    arg = poptGetOptArg(popt_context);
                }

                if (arg_return->jack_nports > 0) {
                    arg_return->use_jack = 1;
                }
                else {
                    fprintf(stderr,
                            "Argument Usage: --use-jack port1 port2... portn\n");
                    success = FALSE;
                }

                if (RMD_USE_JACK_EXTRA_FLAG == POPT_ARGFLAG_DOC_HIDDEN) {
                    fprintf(stderr,
                            "Warning, will ignore --use-jack flags, no libjack support in build.\n");
                }
            }                

            default:
                break;
        }
    }

    if (arg_id == -1) {
        // Parsing is complete, perform final adjustments
        if (no_cursor)
            arg_return->xfixes_cursor = FALSE;

        if (quick_subsampling)
            arg_return->no_quick_subsample = FALSE;

        if (compress_cache)
            arg_return->zerocompression = FALSE;

        if (arg_return->follow_mouse)
            arg_return->full_shots = TRUE;

        if (!arg_return->filename)
            arg_return->filename = strdup(poptGetArg(popt_context));
    }
    else {
        // Parsing error, say what went wrong
        const char *str = poptBadOption(popt_context, 0);

        success = FALSE;

        fprintf(stderr,
                "Error when parsing `%s': ", str);

        switch (arg_id) {
            case POPT_ERROR_NOARG:
                fprintf(stderr, "Missing argument\n");
                break;
                        
            case POPT_ERROR_BADNUMBER:
                fprintf(stderr, "Bad number\n");
                break;
                        
            default:
                fprintf(stderr, "libpopt error: %d\n", arg_id);
                break;
        }
    }
    
    // Make sure argument ranges are valid
    success = success && rmdValidateArguments(arg_return);

    // Clean up
    poptFreeContext(popt_context);
    
    return success;
}

static boolean rmdValidateArguments(const ProgArgs *args)
{
    boolean success = TRUE;

    if (args->x < 0) {
        fprintf(stdout, "-x must not be less than 0.\n");
        success = FALSE;
    }
    if (args->y < 0) {
        fprintf(stdout, "-y must not be less than 0.\n");
        success = FALSE;
    }
    if (args->width < 0) {
        fprintf(stdout, "--width must be larger than 0.\n");
        success = FALSE;
    }
    if (args->height < 0) {
        fprintf(stdout, "--height must be larger than 0.\n");
        success = FALSE;
    }
    if (args->fps <= 0) {
        fprintf(stdout, "--fps must be larger than 0.\n");
        success = FALSE;
    }
    if (args->v_quality < 0 || args->v_quality > 63) {
        fprintf(stdout, "--v_quality must be within the inclusive range [0-63].\n");
        success = FALSE;
    }
    if (args->v_bitrate < 45000 || args->v_quality > 2000000) {
        fprintf(stdout, "--v_bitrate must be within the inclusive range [45000-2000000].\n");
        success = FALSE;
    }
    if (args->frequency <= 0) {
        fprintf(stdout, "--frequency must be larger than 0.\n");
        success = FALSE;
    }
    if (args->channels <= 0) {
        fprintf(stdout, "--channels must be larger than 0.\n");
        success = FALSE;
    }
    if (args->buffsize <= 0) {
        fprintf(stdout, "--buffer-size must be larger than 0.\n");
        success = FALSE;
    }
    if (args->jack_ringbuffer_secs <= 0) {
        fprintf(stdout, "--jack-buffer-size must be larger than 0.\n");
        success = FALSE;
    }

    return success;
}

static void rmdPrintAndExit(poptContext con, 
                            enum poptCallbackReason reason,
                            const struct poptOption *opt,
                            const char *arg,
                            const void *data)
{
    if (strcmp(opt->longName, "version") == 0) {
        fprintf(stderr,"recordMyDesktop v%s\n\n",VERSION);
    }
    else if (strcmp(opt->longName, "help") == 0) {
        poptPrintHelp(con, stdout, 0);
        fprintf(stdout,
                "\n"
                "\tIf no other options are specified, filename can be given without the -o switch.\n"
                "\n"
                "\n");
    }
    else if (strcmp(opt->longName, "print-config") == 0) {
        fprintf(stdout,
                "\n"
                "recordMyDesktop was compiled with the following options:\n"
                "\n"
                "Jack:\t\t\t" RMD_LIBJACK_STATUS "\n"
                "Default Audio Backend:\t" RMD_LIBASOUND_STATUS "\n"
                "\n"
                "\n");
    }

    exit(0);
}
