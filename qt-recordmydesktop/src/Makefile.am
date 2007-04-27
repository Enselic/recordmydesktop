include $(top_srcdir)/m4/python.mk

bin_SCRIPTS=qt-recordMyDesktop

qt_recordMyDesktopdir=$(pythondir)/qt-recordMyDesktop

qt_recordMyDesktop_PYTHON = \
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

desktop_DATA =qt-recordmydesktop.desktop

pixmapdir = $(datadir)/pixmaps
pixmap_DATA = 	qt-recordmydesktop.png\
				qt-recordmydesktop_pause.png\
				qt-recordmydesktop_stop.png\
				qt-recordmydesktop_record.png



EXTRA_DIST=	qt-recordMyDesktop\
			qt-recordmydesktop.desktop\
		    qt-recordmydesktop.png\
			qt-recordmydesktop.svg\
			qt-recordmydesktop_pause.png\
			qt-recordmydesktop_stop.png\
			qt-recordmydesktop_record.png\
			qt-recordmydesktop_record.svg\
			qt-recordmydesktop_pause.svg\
			qt-recordmydesktop_stop.svg

clean-local:
	rm -rf *.pyc *.pyo
