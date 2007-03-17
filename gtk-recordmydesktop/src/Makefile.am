include $(top_srcdir)/m4/python.mk

bin_SCRIPTS=gtk-recordMyDesktop

gtk_recordMyDesktopdir=$(pythondir)/recordMyDesktop

gtk_recordMyDesktop_PYTHON = \
							rmdSelect.py\
						    rmdTrayIcon.py\
							rmdSelectThumb.py\
							rmdTrayPopup.py\
							rmdPrefsWidget.py\
							rmdSimple.py\
							rmdConfig.py\
							rmdMonitor.py\
							rmdFrame.py\
							__init__.py

desktopdir = $(datadir)/applications

desktop_DATA =gtk-recordmydesktop.desktop

pixmapdir = $(datadir)/pixmaps
pixmap_DATA = gtk-recordmydesktop.png

EXTRA_DIST=gtk-recordMyDesktop gtk-recordmydesktop.desktop gtk-recordmydesktop.png gtk-recordmydesktop.svg

clean-local:
	rm -rf *.pyc *.pyo
