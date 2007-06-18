#/******************************************************************************
#*                       qt-recordMyDesktop                                    *
#*******************************************************************************
#*                                                                             *
#*           Copyright (C) 2007  John Varouhakis                               *
#*                                                                             *
#*                                                                             *
#*  This program is free software; you can redistribute it and/or modify       *
#*  it under the terms of the GNU General Public License as published by       *
#*  the Free Software Foundation; either version 2 of the License, or          *
#*  (at your option) any later version.                                        *
#*                                                                             *
#*  This program is distributed in the hope that it will be useful,            *
#*  but WITHOUT ANY WARRANTY; without even the implied warranty of             *
#*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
#*  GNU General Public License for more details.                               *
#*                                                                             *
#*  You should have received a copy of the GNU General Public License          *
#*  along with this program; if not, write to the Free Software                *
#*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  *
#*                                                                             *
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
import popen2
import os,fcntl,signal
from rmdStrings import *

class rmdMonitor(QtGui.QDialog):
    labeString=monStrings['PleaseWait']

    counter_fraction=0.0

    def closeEvent(self,Event=None):
        self.timed_id.stop()
        self.stop_encoding()
    def destroy(self,Event=None):
        self.close()
        exit_ret=os.waitpid(self.rmdPid,0)
        self.parent.show()
    def update_counter(self):
        strstdout=""
        try:
            strstdout=self.stdout.read()
            if strstdout =='':
                self.destroy()
        except:
            return True
        try:
            string=strstdout.replace("[","").replace("%] ","").replace("\n","").split("\r")
            self.counter_fraction=max(self.counter_fraction,int(string.pop()))
            if self.counter_fraction>100:
                self.counter_fraction=100
            self.progressbar.setValue(self.counter_fraction)
            self.progressbar.update()
        except:
            pass

        return True
    def stop_encoding(self,Event=None):
        try:
            os.kill(self.rmdPid,signal.SIGINT)
        except:
            pass
    def __init__(self,out_stream,childPid,parent):
        self.parent=parent
        flags = fcntl.fcntl(out_stream, fcntl.F_GETFL)
        fcntl.fcntl(out_stream, fcntl.F_SETFL, flags | os.O_NONBLOCK)
        self.rmdPid=childPid
        self.stdout=out_stream

        #self.window = QtGui.QDialog()
        QtGui.QDialog.__init__(self)
        #self.closeEvent=self.destroy_and_kill

        self.setWindowTitle("recordMyDesktop-encoder")

        #self.frame=QtGui.QFrame()
        self.label=QtGui.QLabel(self.labeString)

        self.label.setAlignment(QtCore.Qt.AlignHCenter)

        self.progressbar=QtGui.QProgressBar()
        self.counter_fraction=0
        self.progressbar.setValue(self.counter_fraction)
        self.stopbutton=QtGui.QPushButton(monStrings['Cancel'],self)
        self.stopbutton.connect(self.stopbutton,QtCore.SIGNAL("clicked()"),
                                self.stop_encoding)
        self.box=QtGui.QVBoxLayout()
        self.box.addWidget(self.label,0)
        self.box.addWidget(self.progressbar,0)
        self.box.addWidget(self.stopbutton,0)


        #self.stopbutton.show()
        self.setLayout(self.box)
        #self.window.setCentralWidget(self.frame)
        #self.label.show()
        #self.progressbar.show()
        #self.window.show()

        self.timed_id=QtCore.QTimer(None)
        self.timed_id.connect(self.timed_id,QtCore.SIGNAL("timeout()"),
                                self.update_counter)
        self.timed_id.start(100)






