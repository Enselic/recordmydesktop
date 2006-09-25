AC_PREREQ(2.59)
AC_INIT(src/gtk-recordMyDesktop)

AM_INIT_AUTOMAKE(gtk-recordMyDesktop, 0.2.1)

AS_AC_EXPAND(LIBDIR, $libdir)
AC_MSG_NOTICE(Storing library files in $LIBDIR)
AC_SUBST(PYGTK_REQ, 2.4)

AS_AC_EXPAND(DATADIR, $datadir)
AC_MSG_NOTICE(Storing data files in $DATADIR)

AS_AC_EXPAND(SYSCONFDIR, $sysconfdir)
AC_MSG_NOTICE(Storing configuration files in $SYSCONFDIR)

AS_AC_EXPAND(LOCALSTATEDIR, $localstatedir)
AC_MSG_NOTICE(Using localstatedir $LOCALSTATEDIR)

dnl check for python
AM_PATH_PYTHON(2.3)

AS_AC_EXPAND(PYTHONDIR, $pythondir)
AC_MSG_NOTICE(Using pythondir $PYTHONDIR)

GETTEXT_PACKAGE=gtk-recordMyDesktop
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [The gettext package name])
AM_GLIB_GNU_GETTEXT

dnl check for pygtk
PKG_CHECK_MODULES(PYGTK, pygtk-2.0 >= $PYGTK_REQ,,AC_MSG_ERROR(You need pygtk >=2.4 and the appropriate development headers to proceed))
PYGTK_DIR="`$PKG_CONFIG --variable=pyexecdir pygtk-2.0`"
AC_SUBST(PYGTK_DIR)
AC_MSG_NOTICE(Using pygtk installed in $PYGTK_DIR)
PYGTK_VERSION="`$PKG_CONFIG --modversion pygtk-2.0`"

export PYTHONPATH=$PYGTK_DIR:$PYTHONPATH

PKG_CHECK_MODULES(GTK, gtk+-2.0, HAVE_GTK=yes, HAVE_GTK=no)

saved_PYTHONPATH=$PYTHONPATH
export PYTHONPATH=$PYGST_DIR:$PYTHONPATH

#dnl check for egg.trayicon from gnome-python-extras
#PKG_CHECK_MODULES(GNOME_PYTHON_EXTRAS, gnome-python-extras-2.0 >= 2.11.3,
#    HAVE_EGGTRAYICON=yes, HAVE_EGGTRAYICON=no)
#if test "x$HAVE_EGGTRAYICON" = "xno"; then
#  AC_MSG_ERROR(gnome-python-extras not found, please install >= 2.11.3)
#fi




AC_CONFIG_FILES([Makefile 
        src/Makefile
        src/rmdConfig.py
        po/Makefile.in])
AC_OUTPUT
