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

import rmdTrayPopup as iTP
import rmdMonitor as imon
import os,signal,popen2,fcntl
from rmdStrings import *
#values struct:

#0   fps
#1   mouse
#2   sound on/off
#3   full
#4   path
#5   area
#6   delay
#7   channels
#8   frequency
#9   device
#10  v_quality
#11  s_quality
#12  display
#13  shared memory
#14  drop-frames
#15  shared threshold
#16  quick subsampling


class trayIcon(object):
    #values=[15,0,0,1,os.path.join(os.getenv('HOME'),'out.ogg'),[-1,-1,-1,-1],0,
            #1,22050,'hw:0,0',63,10,"$DISPLAY",0,1,75,1]
    state=0#0 stopped,1 recording,2 paused
    rmdPid=None
    optionsOpen=[1]
    reopen=0
    timed_id=None

    def __set_icon__(self,widget,icon):
            widget.setIcon(QtGui.QIcon(self.__get_icon_location__(icon)))


    def __buttonPress__(self,ActivationReason):

        button=0
        if  ActivationReason==QtGui.QSystemTrayIcon.Trigger:
            button=1
        elif ActivationReason==QtGui.QSystemTrayIcon.Context:
            button=3
        #print button
        if button==1 and self.state>=0:
            if self.state == 0:
                if self.optionsOpen[0]==1:
                    self.parent.hide()
                    self.optionsOpen[0]=0
                    self.reopen=1
                self.__set_icon__(self.trayIcon,"stop")
                self.state=1
                self.__execRMD__()
            elif self.state== 1:
                self.__set_icon__(self.trayIcon,"start")
                self.state=0
                self.__stopRMD__()
            elif self.state == 2 :
                self.__set_icon__(self.trayIcon,"stop")
                self.state=1
                self.__pauseRMD__()


        elif button == 3 and self.state>=0:
            if self.state == 0:
                pass
                self.tray_popup.popupmenu.popup(QtGui.QCursor.pos())
            elif self.state == 1:
                self.__set_icon__(self.trayIcon,"pause")
                self.state=2
                self.__pauseRMD__()
            elif self.state ==2:
                self.__set_icon__(self.trayIcon,"stop")
                self.state=1
                self.__pauseRMD__()
    def record_ext(self,button=None):
        if self.state == 0:
            if self.optionsOpen[0]==1:
                self.parent.hide()
                self.optionsOpen[0]=0
                self.reopen=1
            self.__set_icon__(self.trayIcon,"stop")
            self.state=1
            self.__execRMD__()

    def __execRMD__(self):
        self.parent.close_advanced()
        self.parent.update()
        self.execargs=["recordmydesktop","-o",'%s'%self.parent.values[4],
                  "-fps","%d"%self.parent.values[0]]
        if self.parent.values[2]==False :
            self.execargs.append("--no-sound")
        if self.parent.values[1] == 1:
            self.execargs.append("-dummy-cursor")
            self.execargs.append("white")
        elif self.parent.values[1] == 2:
            self.execargs.append("-dummy-cursor")
            self.execargs.append("black")
        elif self.parent.values[1] == 3:
            self.execargs.append("--no-cursor")

        if self.parent.values[3] == 0:
            self.execargs.append("--full-shots")
        if self.parent.values[13] == 1:
            self.execargs.append("--no-shared")

        if self.parent.values[15]==1:
            if self.parent.values[5][0]>0  :
                self.execargs.append('-x')
                self.execargs.append('%d'%self.parent.values[5][0])
            if self.parent.values[5][1]>0:
                self.execargs.append('-y')
                self.execargs.append('%d'%self.parent.values[5][1])
        if self.parent.values[5][2]>0 and self.parent.values[5][3]>0:
            self.execargs.append('-width')
            self.execargs.append('%d'%(self.parent.values[5][2]-self.parent.values[5][0]))
            self.execargs.append('-height')
            self.execargs.append('%d'%(self.parent.values[5][3]-self.parent.values[5][1]))
            if(not self.parent.values[14]):
                for i in range(4):
                    self.parent.values[5][i]=-1
        if self.parent.values[6]>0:
            self.execargs.append('-delay')
            self.execargs.append('%d'%self.parent.values[6])
        self.execargs.append('-channels')
        self.execargs.append('%d'%self.parent.values[7])
        self.execargs.append('-freq')
        self.execargs.append('%d'%self.parent.values[8])
        if self.parent.values[9]!='DEFAULT':
            self.execargs.append('-device')
            self.execargs.append('%s'%self.parent.values[9])
        self.execargs.append('-v_quality')
        self.execargs.append('%d'%self.parent.values[10])
        self.execargs.append('-s_quality')
        self.execargs.append('%d'%self.parent.values[11])
        if self.parent.values[12] != "$DISPLAY":
            self.execargs.append('-display')
            self.execargs.append('%s'%self.parent.values[12])
        if self.parent.values[16] == 0:
            self.execargs.append('--quick-subsampling')
        self.execargs.append('-workdir')
        self.execargs.append('%s'%self.parent.values[17])
        if self.parent.values[18] == 0:
            self.execargs.append('--on-the-fly-encoding')
        if self.parent.values[19] == 1:
            self.execargs.append('--compress-cache')
        if self.parent.values[20] == True:
            self.execargs.append('--overwrite')
        if self.parent.values[15] == 0:
            self.execargs.append('--follow-mouse')
        if self.parent.values[22] == True:
            self.execargs.append('-use-jack')
            for i in self.parent.values[23]:
                self.execargs.append(i)
        if self.parent.values[26] != "":
            for i in self.parent.values[26].split(" "):
                if i!="":
                    self.execargs.append(i)
        #print execargs



        self.childP=popen2.Popen3(self.execargs,"t")
        flags = fcntl.fcntl(self.childP.childerr, fcntl.F_GETFL)
        fcntl.fcntl(self.childP.childerr, fcntl.F_SETFL, flags | os.O_NONBLOCK)
        self.rmdPid=self.childP.pid
        self.timed_id=QtCore.QTimer(None)
        self.timed_id.connect(self.timed_id,QtCore.SIGNAL("timeout()"),
                                self.__check_status__)
        self.timed_id.start(1000)

    def __exit_status_dialog(self,status):
        #first try to write down a log for examination
        try:
            error_log=open(os.path.join(os.getenv("HOME"),"qt-recordMyDesktop-crash.log"),"w")
            error_log.write("#This is the command given at initialization:\n")
            for i in self.execargs:
                error_log.write("%s "%i)
            error_log.write("\n\n\n#recordMyDesktop stderror output:\n")
            try:
                for err_line in self.childP.childerr.readlines():
                    error_log.write(err_line)
            except:
                    error_log.write("Couldn't write stderror of recordMyDesktop!\n")
            self.childP.childerr.close()
        except:
            print "Couldn't write error log.\n"
        self.dialog = QtGui.QWidget()
        self.label1=None
        try:
            self.label1 = QtGui.QLabel("\t\t"+TrayIconStr['RecFinishedKnown'] +
            ": %d\n"%(status) + TrayIconStr['ErrDesc'] + ":" +
            (rmdErrStr[int(status)]) + "\n")
        except:
            self.label1 = QtGui.QLabel("\t\t"+TrayIconStr['RecFinishedUnknown']+": %d \n"%(status))
        self.label1.setAlignment(QtCore.Qt.AlignHCenter)
        self.dialogbutton = QtGui.QPushButton("Ok",self.dialog)
        self.dialogbutton.connect(self.dialogbutton,QtCore.SIGNAL("clicked()"),self.dialog.close )
        self.dialogLayout=QtGui.QVBoxLayout()
        self.dialogLayout.addWidget(self.label1)
        self.dialogLayout.addWidget(self.dialogbutton)

        self.label1.show()
        self.dialogbutton.show()
        self.dialog.setLayout(self.dialogLayout)
        self.dialog.resize(300,128)
        self.dialog.show()

    def __pauseRMD__(self):
        os.kill(self.rmdPid,signal.SIGUSR1)

    def __stopRMD__(self):
        if self.timed_id!=None:
            self.timed_id.stop()
            self.timed_id=None
        exit_ret=os.waitpid(self.rmdPid,os.WNOHANG)
        if exit_ret[0] == 0:
            os.kill(self.rmdPid,signal.SIGTERM)
            self.state=-1
            monitor=imon.rmdMonitor(self.childP.fromchild,self.rmdPid,self.parent)
            monitor.exec_()

            self.state=0
        else:
            self.__exit_status_dialog(exit_ret[1])
            self.parent.show()
        self.rmdPid=None
        if self.reopen==1:
            self.optionsOpen[0]=1
            self.reopen=0

        #print exit_ret
    def __check_status__(self):
        if self.rmdPid!=None:
            exit_ret=os.waitpid(self.rmdPid,os.WNOHANG)
            if exit_ret[0] != 0:
                self.state=0
                self.__set_icon__(self.trayIcon,"start")
                self.__exit_status_dialog(exit_ret[1])
                self.rmdPid=None
                if self.reopen==1:
                    self.parent.show()
                    self.optionsOpen[0]=1
                    self.reopen=0
                self.timed_id.stop()
                return False
            else:
                return True
        else:
            self.timed_id.stop()
            return False
    def __get_icon_location__(self,name):
        if name=="start":
            return os.path.join(rmdConfig.pixmapdir,"qt-recordmydesktop_record.png")
        elif name=="stop":
            return os.path.join(rmdConfig.pixmapdir,"qt-recordmydesktop_stop.png")
        elif name=="pause":
            return os.path.join(rmdConfig.pixmapdir,"qt-recordmydesktop_pause.png")

    def __init__(self,parent):
        self.parent=parent
        self.trayIcon=QtGui.QSystemTrayIcon(None)
        self.__set_icon__(self.trayIcon,"start")
        self.tray_popup=iTP.TrayPopupMenu(self.parent,self.parent.values,self.optionsOpen)
        self.trayIcon.connect(self.trayIcon,QtCore.SIGNAL("activated(QSystemTrayIcon::ActivationReason)"),self.__buttonPress__)
        self.trayIcon.show()


if __name__ == "__main__":
    import sys

    app=QtGui.QApplication(sys.argv)
    app.values=rmdConfig.default_values
    w = trayIcon(app)
    app.exec_()






