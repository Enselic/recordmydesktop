 #                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(src/recordmydesktop.c)
AM_INIT_AUTOMAKE(recordmydesktop,0.3.1,)

AC_CONFIG_SRCDIR([src/recordmydesktop.c])
AM_CONFIG_HEADER(config.h)



# Checks for programs.

AC_PROG_CC

# Checks for header files.

AC_HEADER_DIRENT
AC_HEADER_STDC

AC_PATH_X
AC_PATH_XTRA

if test "x$x_libraries" != "x" && test "x$x_libraries" != xNONE ; then
    echo "X libraries are found in $x_libraries"
    LIBS="-L$x_libraries  $LIBS";
fi

if test "x$x_includes" != "x" && test "x$x_includes" != xNONE ; then
    echo "X includes are found in $x_includes"
    CFLAGS="-I$x_includes  $CFLAGS";
fi



AC_CHECK_HEADER([alsa/asoundlib.h])
AC_CHECK_HEADERS([sys/time.h unistd.h vorbis/vorbisfile.h ])

# Checks for libraries.

AC_CHECK_LIB([m],[isnan],,)
AC_CHECK_LIB([z],[deflate],,)
AC_CHECK_LIB([ICE],[IceOpenConnection],,AC_MSG_ERROR([Can't find libICE]),)
AC_CHECK_LIB([SM],[SmcOpenConnection],,AC_MSG_ERROR([Can't find libSM]),)
AC_CHECK_LIB([X11],[XOpenDisplay],,AC_MSG_ERROR([Can't find libX11]),
            -L$x_libraries $X_PRE_LIBS)
AC_CHECK_LIB([Xext],[XShmQueryVersion],,AC_MSG_ERROR([Can't find libXext]))
AC_CHECK_LIB([Xfixes], [XFixesQueryExtension],,AC_MSG_ERROR([Can't find libXfixes]))
AC_CHECK_LIB([Xdamage], [XDamageQueryExtension],,AC_MSG_ERROR([Can't find libXdamage]))
AC_CHECK_LIB([vorbis],[vorbis_info_clear],,AC_MSG_ERROR([Can't find libvorbis]))
AC_CHECK_LIB([vorbisfile],[ov_open],,AC_MSG_ERROR([Can't find libvorbisfile]),-lvorbis)
AC_CHECK_LIB([vorbisenc],[vorbis_encode_init],,AC_MSG_ERROR([Can't find libvorbisenc]),-lvorbis)
AC_CHECK_LIB([ogg],[ogg_stream_init],,AC_MSG_ERROR([Can't find libogg]))
AC_CHECK_LIB([theora],[theora_encode_YUVin],,AC_MSG_ERROR([Can't find libtheora]))
AC_CHECK_LIB([pthread],[pthread_mutex_lock],,AC_MSG_ERROR([Can't find libpthread]))
AC_CHECK_LIB([asound],[snd_pcm_drain],,AC_MSG_ERROR([Can't find libasound]))



# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
        src/Makefile
        include/Makefile
        doc/Makefile ])
AC_OUTPUT

