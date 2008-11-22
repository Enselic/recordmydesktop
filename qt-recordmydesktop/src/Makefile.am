
bin_SCRIPTS=qt-recordMyDesktop

qt_recordMyDesktopdir=$(pythondir)/qt_recordMyDesktop

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
							rmdStrings.py\
							__init__.py

desktopdir = $(datadir)/applications

desktop_DATA =qt-recordmydesktop.desktop

pixmapdir = $(datadir)/pixmaps
pixmap_DATA = 	qt-recordmydesktop.png\
				media-playback-pause.svg\
				media-playback-stop.svg\
				media-record.svg



EXTRA_DIST=	qt-recordMyDesktop.in\
			qt-recordmydesktop.desktop\
			media-playback-pause.svg\
			media-playback-stop.svg\
			media-record.svg

clean-local:
	rm -rf *.pyc *.pyo
