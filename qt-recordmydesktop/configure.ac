AC_PREREQ(2.59)
AC_INIT(src/rmdSimple.py)

AM_INIT_AUTOMAKE(qt-recordmydesktop, 0.2)

AS_AC_EXPAND(LIBDIR, $libdir)
AC_MSG_NOTICE(Storing library files in $LIBDIR)
AC_SUBST(PYQT4_REQ, 4.1)
AC_SUBST(QT4_REQ, 4.2)

#The following test is needed for the expansion bellow
if test "x$prefix" == "xNONE"; then
    export prefix="/usr/local/"
else
    echo "prefix set to $prefix"
fi

AS_AC_EXPAND(PREFIX,$prefix)

AS_AC_EXPAND(DATADIR, "$datadir")
AC_MSG_NOTICE(Storing data files in $DATADIR)

AS_AC_EXPAND(SYSCONFDIR, $sysconfdir)
AC_MSG_NOTICE(Storing configuration files in $SYSCONFDIR)

AS_AC_EXPAND(LOCALSTATEDIR, $localstatedir)
AC_MSG_NOTICE(Using localstatedir $LOCALSTATEDIR)

dnl check for python
AM_PATH_PYTHON(2.3)

AS_AC_EXPAND(PYTHONDIR, $pythondir)
AC_MSG_NOTICE(Using pythondir $PYTHONDIR)

GETTEXT_PACKAGE=qt-recordMyDesktop
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [The gettext package name])
AM_GLIB_GNU_GETTEXT

dnl check for pyqt
PKG_CHECK_MODULES(QtCore, [QtCore >= $QT4_REQ],,AC_MSG_ERROR(You need libQt4 >=4.2 and the appropriate development headers to proceed))
PKG_CHECK_MODULES(QtGui, [QtGui >= $QT4_REQ],,AC_MSG_ERROR(You need libQt4 >=4.2 and the appropriate development headers to proceed))

export PYTHONPATH=$PYTHONPATH

export PYQT4_VERSION=`python -c 'import PyQt4; from PyQt4 import QtCore; print QtCore.PYQT_VERSION_STR'  2>>/dev/null `
if test x$PYQT4_VERSION == x; then
    AC_MSG_ERROR(You need PyQt4>=4.1 installed to procceed);
fi
b=$(echo "$PYQT4_VERSION" | awk 'BEGIN{ FS="." } { print $1 "\n" $2 "\n" $3 }')
c=($b)


export PYQT4_MAJOR=${c[[0]]}
export PYQT4_MINOR=${c[[1]]}

if test $PYQT4_MAJOR != 4; then
    AC_MSG_ERROR(PyQt4 Major version number not 4!!!);
fi

if test $PYQT4_MINOR -lt 1; then
    AC_MSG_ERROR(You need PyQt4>=4.1 installed to procceed
    (earlier version found installed));
fi


AC_CONFIG_FILES([Makefile
        src/Makefile
        src/rmdConfig.py
        src/qt-recordMyDesktop
        po/Makefile.in
        m4/Makefile])
AC_OUTPUT
