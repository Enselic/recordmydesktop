from distutils.core import setup
import os,sys

setup(author='John Varouhakis',
url='http://recordmydesktop.sourceforge.net',
author_email='johnvarouhakis@gmail.com',
description='A pyGTK frontend for recordMyDesktop',
license='GNU GENERAL PUBLIC LICENSE Version 2',
name='gtk-recordMyDesktop',version='0.1' ,
py_modules=['rmdPrefsWidget','rmdTrayIcon'],
scripts=['gtk-recordMyDesktop'],
data_files=[('/usr/share/applications/',['gtk-recordmydesktop.desktop']),
            ('/usr/share/pixmaps/',['gtk-recordmydesktop.png'])],
platforms='linux')

