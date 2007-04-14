#/*********************************************************************************
#*                         gtk-recordMyDesktop                                    *
#**********************************************************************************
#*                                                                                *
#*             Copyright (C) 2006  John Varouhakis                                *
#*                                                                                *
#*                                                                                *
#*    This program is free software; you can redistribute it and/or modify        *
#*    it under the terms of the GNU General Public License as published by        *
#*    the Free Software Foundation; either version 2 of the License, or           *
#*    (at your option) any later version.                                         *
#*                                                                                *
#*    This program is distributed in the hope that it will be useful,             *
#*    but WITHOUT ANY WARRANTY; without even the implied warranty of              *
#*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
#*    GNU General Public License for more details.                                *
#*                                                                                *
#*    You should have received a copy of the GNU General Public License           *
#*    along with this program; if not, write to the Free Software                 *
#*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA   *
#*                                                                                *
#*                                                                                *
#*                                                                                *
#*    For further information contact me at johnvarouhakis@gmail.com              *
#**********************************************************************************/


import pygtk
pygtk.require('2.0')
import gtk,gobject
import locale, gettext
import rmdConfig
_ = gettext.gettext
gettext.textdomain('gtk-recordMyDesktop')
gettext.bindtextdomain('gtk-recordMyDesktop',rmdConfig.locale_install_dir)
USE_EGG=1
if gtk.pygtk_version[0]==2 and gtk.pygtk_version[1]>=10:
    USE_EGG=0
if USE_EGG==1:
    import egg.trayicon
import rmdSelect as isel
import rmdTrayPopup as iTP
import rmdMonitor as imon
import os,signal,popen2,fcntl

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
    exit_status={
        0:_('Success'),
        1*256:_('Error while parsing the arguments.'),
        2*256:_('Initializing the encoder failed(either vorbis or theora)'),
        3*256:_('Could not open/configure sound card.'),
        4*256:_('Xdamage extension not present.'),
        5*256:_('Shared memory extension not present.'),
        6*256:_('Xfixes extension not present.'),
        7*256:_('XInitThreads failed.'),
        8*256:_('No $DISPLAY environment variable and none specified as argument.'),
        9*256:_('Cannot connect to Xserver.'),
        10*256:_('Color depth is not 24bpp.'),
        11*256:_('Improper window specification.'),
        12*256:_('Cannot attach shared memory to proccess.'),
        13*256:_('Cannot open file for writting.'),
        14*256:_('Cannot load the Jack library (dlopen/dlsym error on libjack.so).'),
        15*256:_('Cannot create new client.'),
        16*256:_('Cannot activate client.'),
        17*256:_('Port registration/connection failure.'),
        11:_('Segmentation Fault')
        }


    state=0#0 stopped,1 recording,2 paused
    rmdPid=None
    optionsOpen=[1]
    reopen=0
    timed_id=None
    def __buttonPressNoEGG_Activate__(self,widget):
        self.__buttonPress__(1)
    def __buttonPressNoEGG_Popup__(self,widget,button,activate_time):
        self.__buttonPress__(3)
    def __buttonPressEGG__(self,widget,event=None):
        self.__buttonPress__(event.button)
    def __set_icon__(self,widget,icon):
        if USE_EGG==1:
            widget.set_from_stock(icon,gtk.ICON_SIZE_SMALL_TOOLBAR)
        else:
            widget.set_from_stock(icon)


    def __buttonPress__(self,button):
        if button==1 and self.state>=0:
            if self.state == 0:
                if self.optionsOpen[0]==1:
                    self.parent.hide()
                    self.optionsOpen[0]=0
                    self.reopen=1
                #self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_STOP,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.__set_icon__(self.trayIcon,gtk.STOCK_MEDIA_STOP)
                self.state=1
                self.__execRMD__()
            elif self.state== 1:
                #self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_RECORD,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.__set_icon__(self.trayIcon,gtk.STOCK_MEDIA_RECORD)
                self.state=0
                self.__stopRMD__()
            elif self.state == 2 :
                #self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_STOP,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.__set_icon__(self.trayIcon,gtk.STOCK_MEDIA_STOP)
                self.state=1
                self.__pauseRMD__()


        elif button == 3 and self.state>=0:
            if self.state == 0:
                self.tray_popup.show()
            elif self.state == 1:
                #self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_PAUSE,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.__set_icon__(self.trayIcon,gtk.STOCK_MEDIA_PAUSE)
                self.state=2
                self.__pauseRMD__()
            elif self.state ==2:
                #self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_STOP,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.__set_icon__(self.trayIcon,gtk.STOCK_MEDIA_STOP)
                self.state=1
                self.__pauseRMD__()
    def record_ext(self,button=None):
        if self.state == 0:
            if self.optionsOpen[0]==1:
                self.parent.hide()
                self.optionsOpen[0]=0
                self.reopen=1
            #self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_STOP,gtk.ICON_SIZE_SMALL_TOOLBAR)
            self.__set_icon__(self.trayIcon,gtk.STOCK_MEDIA_STOP)
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
        if self.parent.values[19] == 0:
            self.execargs.append('--zero-compression')
        if self.parent.values[20] == True:
            self.execargs.append('--overwrite')
        if self.parent.values[22] == True:
            self.execargs.append('-use-jack')
            portstring=""
            for i in self.parent.values[23]:
                portstring+=i+","
            portstring=portstring.rstrip(",")
            self.execargs.append(portstring)
        #print execargs



        self.childP=popen2.Popen3(self.execargs,"t")
        flags = fcntl.fcntl(self.childP.childerr, fcntl.F_GETFL)
        fcntl.fcntl(self.childP.childerr, fcntl.F_SETFL, flags | os.O_NONBLOCK)
        self.rmdPid=self.childP.pid
        self.timed_id=gobject.timeout_add(1000,self.__check_status__)


    def __exit_status_dialog(self,status):
        #first try to write down a log for examination
        try:
            error_log=open(os.path.join(os.getenv("HOME"),"gtk-recordMyDesktop-crash.log"),"w")
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
        dialog = gtk.Dialog(title=None, parent=None, flags=0, buttons=None)
        label1=None
        try:
            label1 = gtk.Label("\t\t"+_("Recording is finished.\nrecordMyDesktop has exited with status")+
            ": %d\n"%(status)+_("Description")+":%s\n"%(self.exit_status[int(status)]))
        except:
            label1 = gtk.Label("\t\t"+_("Recording is finished.\nrecordMyDesktop has exited with uknown\nerror code")+": %d \n"%(status))
        dialog.vbox.pack_start(label1, True, True, 0)
        label1.show()
        button = gtk.Button(label=None, stock=gtk.STOCK_OK)
        dialog.vbox.pack_start(button, True, True, 0)
        def __destroy(button):
            dialog.destroy()
        button.connect("clicked", __destroy)
        button.show()
        dialog.set_size_request(300,128)
        dialog.show()

    def __pauseRMD__(self):
        os.kill(self.rmdPid,signal.SIGUSR1)

    def __stopRMD__(self):
        if self.timed_id!=None:
            gobject.source_remove(self.timed_id)
            self.timed_id=None
        exit_ret=os.waitpid(self.rmdPid,os.WNOHANG)
        if exit_ret[0] == 0:
            os.kill(self.rmdPid,signal.SIGTERM)
            self.state=-1
            monitor=imon.rmdMonitor(self.childP.fromchild,self.rmdPid)

            exit_ret=os.waitpid(self.rmdPid,0)

            self.state=0
                #os.slee
            #if exit_ret[0]==self.rmdPid:
            #self.__exit_status_dialog(exit_ret[1])
        else:
            self.__exit_status_dialog(exit_ret[1])
        self.rmdPid=None
        if self.reopen==1:
            self.parent.show()
            self.optionsOpen[0]=1
            self.reopen=0

        #print exit_ret
    def __check_status__(self):
        if self.rmdPid!=None:
            exit_ret=os.waitpid(self.rmdPid,os.WNOHANG)
            if exit_ret[0] != 0:
                self.state=0
                #self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_RECORD,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.__set_icon__(self.trayIcon,gtk.STOCK_MEDIA_RECORD)
                self.__exit_status_dialog(exit_ret[1])
                self.rmdPid=None
                if self.reopen==1:
                    self.parent.show()
                    self.optionsOpen[0]=1
                    self.reopen=0
                return False
            else:
                return True
        else:
            return False


    def __init__(self,parent):
        self.parent=parent
        #self.parent.values=values
        if USE_EGG==1:
            self.event_box = gtk.EventBox()
            self.trayIcon=gtk.Image()
            self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_RECORD, gtk.ICON_SIZE_SMALL_TOOLBAR)
            self.event_box.add(self.trayIcon)
            self.tray_container = egg.trayicon.TrayIcon("recordMyDesktop")
            self.tray_container.add(self.event_box)
            self.tray_popup=iTP.TrayPopupMenu(self.parent,self.parent.values,self.optionsOpen)
            self.event_box.connect("button-press-event", self.__buttonPressEGG__)
            self.tray_container.show_all()
        else:
            self.trayIcon=gtk.StatusIcon()
            self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_RECORD)
            self.tray_popup=iTP.TrayPopupMenu(self.parent,self.parent.values,self.optionsOpen)
            self.trayIcon.connect("activate",self.__buttonPressNoEGG_Activate__)
            self.trayIcon.connect("popup-menu",self.__buttonPressNoEGG_Popup__)
        #gtk.main()










