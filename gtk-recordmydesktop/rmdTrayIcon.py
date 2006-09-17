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
pygtk.require('2.4')
import gtk

import egg.trayicon
import istanbulSelect as isel
import istanbulTrayPopup as iTP
import os

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
    values=[15,0,0,1,os.path.join(os.getenv('HOME'),'out.ogg'),[-1,-1,-1,-1],0]
    event_box = gtk.EventBox() 
    state=0#0 stopped,1 recording,2 paused
    rmdPid=None
    optionsOpen=[0]
    
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

        execargs=["recordmydesktop","-o","%s"%self.values[4],
                  "--no-quick-subsampling",
                  "-fps","%d"%self.values[0]]
        if self.values[2]==1 :
            execargs.append("--nosound")
        if self.values[1] == 0:
            execargs.append("-dummy-cursor")
            execargs.append("white")

        elif self.values[1] == 1:
            execargs.append("-dummy-cursor")
            execargs.append("black")
        else:
            execargs.append("--no-dummy-cursor")
        
        if self.values[3] == 0:
            execargs.append("--full-shots")
            execargs.append("--with-shared")

        self.rmdPid=os.fork()

        if self.rmdPid==0:
            res=os.execvp("recordmydesktop",execargs)
            print res

        
    def __pauseRMD__(self):
        os.system("kill -s USR1 %d"%self.rmdPid)
        
    def __stopRMD__(self):
        os.system("kill -s TERM %d"%self.rmdPid)
        self.rmdPid=None


    def __init__(self):
        
        self.trayIcon=gtk.Image()
        self.trayIcon.set_from_stock(gtk.STOCK_MEDIA_RECORD, gtk.ICON_SIZE_SMALL_TOOLBAR)
        self.event_box.add(self.trayIcon)
        self.tray_container = egg.trayicon.TrayIcon("recordMyDesktop")
        self.tray_container.add(self.event_box)
        self.tray_popup=iTP.TrayPopupMenu(self.values,self.optionsOpen)
        self.event_box.connect("button-press-event", self.__buttonPress__)
        self.tray_container.show_all()
        gtk.main()










