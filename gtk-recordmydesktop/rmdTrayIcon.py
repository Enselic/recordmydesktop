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

import egg.trayicon
import rmdSelect as isel
import rmdTrayPopup as iTP
import os,signal

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
    values=[15,0,0,1,os.path.join(os.getenv('HOME'),'out.ogg'),[-1,-1,-1,-1],0,
            1,22050,'hw:0,0',63,10,"$DISPLAY",0,1,75,1]
    exit_status={
        0:'Success',
        1*256:'Error while parsing the arguments.',
        2*256:'Initializing the encoder failed(either vorbis or theora)',
        3*256:'Could not open/configure sound card.',
        4*256:'Xdamage extension not present.',
        5*256:'Shared memory extension not present.',
        6*256:'Xfixes extension not present.',
        7*256:'XInitThreads failed.',
        8*256:'No $DISPLAY environment variable and none specified as argument.',
        9*256:'Cannot connect to Xserver.',
        10*256:'Color depth is not 24bpp.',
        11*256:'Improper window specification.',
        12*256:'Cannot attach shared memory to proccess.',
        11:'Segmentation Fault'
        }
 
 
    state=0#0 stopped,1 recording,2 paused
    rmdPid=None
    optionsOpen=[0]
    timed_id=None
    
    def __buttonPress__(self,widget,event=None):
        if event.button==1 and self.optionsOpen[0]==0:
            if self.state == 0:
                self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_STOP,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.state=1
                self.__execRMD__()
            elif self.state== 1:
                self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_RECORD,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.state=0
                self.__stopRMD__()
            elif self.state == 2 :
                self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_STOP,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.state=1
                self.__pauseRMD__()
              

        elif event.button == 3:
            if self.state == 0:
                self.tray_popup.show()
            elif self.state == 1:
                self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_PAUSE,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.state=2
                self.__pauseRMD__()
            elif self.state ==2:
                self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_STOP,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.state=1
                self.__pauseRMD__()
        
    def __execRMD__(self):

        execargs=["recordmydesktop","-o",'%s'%self.values[4],
                  "-fps","%d"%self.values[0]]
        if self.values[2]==1 :
            execargs.append("--nosound")
        if self.values[1] == 1:
            execargs.append("-dummy-cursor")
            execargs.append("white")
        elif self.values[1] == 2:
            execargs.append("-dummy-cursor")
            execargs.append("black")
        elif self.values[1] == 3:
            execargs.append("--no-cursor")
        
        if self.values[3] == 0:
            execargs.append("--full-shots")
            if self.values[13] == 0:
                execargs.append("--with-shared")
        if self.values[3] == 1 and self.values[13] == 1 :
            execargs.append("--no-cond-shared")

        if self.values[5][0]>0  :
            execargs.append('-x')
            execargs.append('%d'%self.values[5][0])
        if self.values[5][1]>0:
            execargs.append('-y')
            execargs.append('%d'%self.values[5][1])
        if self.values[5][2]>0 and self.values[5][3]>0:
            execargs.append('-width')
            execargs.append('%d'%(self.values[5][2]-self.values[5][0]))
            execargs.append('-height')
            execargs.append('%d'%(self.values[5][3]-self.values[5][1]))
            for i in range(4):
                self.values[5][i]=-1
        if self.values[6]>0:
            execargs.append('-delay')
            execargs.append('%d'%self.values[6])
        execargs.append('-channels')
        execargs.append('%d'%self.values[7])
        execargs.append('-freq')
        execargs.append('%d'%self.values[8])
        execargs.append('-device')
        execargs.append('%s'%self.values[9])
        execargs.append('-v_quality')
        execargs.append('%d'%self.values[10])
        execargs.append('-s_quality')
        execargs.append('%d'%self.values[11])
        if self.values[12] != "$DISPLAY":
            execargs.append('-display')
            execargs.append('%s'%self.values[12])
        if self.values[14] == 0:
            execargs.append('--drop-frames')
        execargs.append('-shared-threshold')
        execargs.append('%d'%self.values[15])
        if self.values[16] == 0:
            execargs.append('--quick-subsampling')
        
        
        #print execargs

        self.rmdPid=os.fork()

        if self.rmdPid==0:
            res=os.execvp("recordmydesktop",execargs)
        else:
            self.timed_id=gobject.timeout_add(1000,self.__check_status__)
            
        
    def __exit_status_dialog(self,status):
        dialog = gtk.Dialog(title=None, parent=None, flags=0, buttons=None)
        label1=None
        try:
            label1 = gtk.Label("\t\tRecording is finished.\nrecordMyDesktop has exited with status %d\nDescription:%s\n"%(status,self.exit_status[int(status)]))
        except:
            label1 = gtk.Label("\t\tRecording is finished.\nrecordMyDesktop has exited with uknown\nerror code: %d \n"%(status))
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
            exit_ret=os.waitpid(self.rmdPid,0)
            #if exit_ret[0]==self.rmdPid:
            self.__exit_status_dialog(exit_ret[1])
        else:
            self.__exit_status_dialog(exit_ret[1])
        self.rmdPid=None
        #print exit_ret
    def __check_status__(self):
        if self.rmdPid!=None:
            exit_ret=os.waitpid(self.rmdPid,os.WNOHANG)
            if exit_ret[0] != 0:
                self.state=0
                self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_RECORD,gtk.ICON_SIZE_SMALL_TOOLBAR)
                self.__exit_status_dialog(exit_ret[1])
                self.rmdPid=None
                return False
            else:
                return True
        else:
            return False


    def __init__(self):
        self.event_box = gtk.EventBox()         
        self.trayIcon=gtk.Image()
        self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_RECORD, gtk.ICON_SIZE_SMALL_TOOLBAR)
        self.event_box.add(self.trayIcon)
        self.tray_container = egg.trayicon.TrayIcon("recordMyDesktop")
        self.tray_container.add(self.event_box)
        self.tray_popup=iTP.TrayPopupMenu(self.values,self.optionsOpen)
        self.event_box.connect("button-press-event", self.__buttonPress__)
        self.tray_container.show_all()
        gtk.main()










