#/******************************************************************************
#*                       qt-recordMyDesktop                                    *
#*******************************************************************************
#*                                                                             *
#*           Copyright (C) 2007, 2008  John Varouhakis                         *
#*                                                                             *
#*                                                                             *
#*   This program is free software: you can redistribute it and/or modify      *
#*   it under the terms of the GNU General Public License as published by      *
#*   the Free Software Foundation, either version 3 of the License, or         *
#*   (at your option) any later version.                                       *
#*                                                                             *
#*   This program is distributed in the hope that it will be useful,           *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
#*   GNU General Public License for more details.                              *
#*                                                                             *
#*   You should have received a copy of the GNU General Public License         *
#*   along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
#*                                                                             *
#*                                                                             *
#*  For further information contact me at johnvarouhakis@gmail.com             *
#******************************************************************************/


from PyQt4 import QtGui,QtCore
import locale, gettext
import rmdConfig
def _(s):
    return QtCore.QString.fromUtf8(gettext.gettext(s))
gettext.textdomain('qt-recordMyDesktop')
gettext.bindtextdomain('qt-recordMyDesktop',rmdConfig.locale_install_dir)

import rmdSelect as iSel
from rmdStrings import *
import sys
#import rmdPrefsWidget as pW

class TrayPopupMenu:
    options=None
    def __init__(self,parent,values,optionsOpen):
        self.parent=parent
        self.optionsOpen=optionsOpen
        self.values=values
        self._setup_popup_menu()
        self.selector = None
    def _setup_popup_menu(self):
        self.popupmenu = QtGui.QMenu()
        self.popupmenu.addAction(TpmStr['ShowHide'],self._prefs_widget)
        self.popupmenu.addAction(TpmStr['SelectOnScreen'].replace("_",""),self._select_area_cb)
        self.popupmenu.addAction(TpmStr['Quit'],self.parent.window.close)

    def _prefs_widget(self):
        if self.optionsOpen[0] ==0:
            self.optionsOpen[0]=1
            self.parent.show()
        else:
            self.parent.hide()
            self.optionsOpen[0]=0

    def _select_area_cb(self):
        self.popupmenu.hide()
        self.selector = iSel.QtAreaSelector(self.values[5])
        self.selector.showFullScreen()
        self.parent.image.x1=-1
    def show(self):
        QtGui.QMenu.show(self.popupmenu)
    def hide(self):
        self.popupmenu.hide()

