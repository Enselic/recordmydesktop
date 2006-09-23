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
import gtk
import locale, gettext
_ = gettext.gettext
import os
from rmdPrefsWidget import *
from rmdTrayIcon import *
import gtk.gdk
import gobject
import gc
import rmdPrefsWidget as pW
import rmdSelectThumb as sT

class simpleWidget(object):
    hidden=[0]
    labelStrings=[_('Video Quality'),_('Sound Quality')]
    buttonStrings=[_('Advanced'),_('Select Window')]
    options=None
    optionsOpen=[0]
    def __subWidgets__(self):
        self.labelbox=gtk.VBox(homogeneous=False, spacing=20)
        #self.labelbox.set_size_request(400,272)
        self.NBox=gtk.HBox(homogeneous=False, spacing=20)
        self.NEBox=gtk.VBox(homogeneous=False, spacing=0)
        self.NEVQBox=gtk.HBox(homogeneous=False, spacing=10)
        self.NESQBox=gtk.HBox(homogeneous=False, spacing=10)
        self.NETABox=gtk.VBox(homogeneous=False, spacing=10)
        self.NWBox=gtk.VBox(homogeneous=False, spacing=0)
        
        
        
        self.NBox.pack_start(self.NWBox,expand=False,fill=False)
        self.NBox.pack_start(self.NEBox,expand=True,fill=True)
        self.NEBox.pack_start(self.NEVQBox,expand=False,fill=False)
        self.NEBox.pack_start(self.NESQBox,expand=False,fill=False)
        self.NEBox.pack_start(self.NETABox,expand=False,fill=False)
        
        self.SBox=gtk.HBox(homogeneous=True, spacing=0)
        self.SEBox=gtk.HBox(homogeneous=False, spacing=10)
        self.SWBox=gtk.HBox(homogeneous=False, spacing=10)
        self.SBox.pack_start(self.SWBox,expand=True,fill=True)
        self.SBox.pack_start(self.SEBox,expand=True,fill=True)        
        
        self.labelbox.pack_start(self.NBox,expand=False,fill=False)
        self.labelbox.pack_end(self.SBox,expand=False,fill=False)
        
        self.wroot = gtk.gdk.get_default_root_window()
        (self.wwidth, self.wheight) = self.wroot.get_size()
        
        ###############################################################
        #self.image = gtk.Image()
        self.factor=1;
        twidth=self.wwidth
        while twidth>320 or self.factor<4:
          twidth/=2
          self.factor*=2
        #self.thumb=gtk.gdk.Image(gtk.gdk.IMAGE_NORMAL,self.wroot.get_visual(),self.wwidth/self.factor,self.wheight/self.factor)
        
        
        #sroot = self.wroot.get_image(0, 0, self.wwidth, self.wheight)
        #self.__subsample__(sroot,self.wwidth,self.wheight,self.thumb,self.factor)
        #self.image.set_from_image(self.thumb,None)
        
        #self.image.set_size_request(self.wwidth/self.factor,self.wheight/self.factor)
        ##self.NWBox.pack_start(self.image,False,False)
        #self.image.show()
        ###############################################################
        self.image=sT.GtkThumbSelector(self.values[5],self.hidden,2000)
        self.image.show()

        self.NWBox.pack_start(self.image,False,False)
        #quality
        self.v_adj=gtk.Adjustment(value=(self.values[10]*100)/63, lower=0, upper=100, step_incr=1, page_incr=10, page_size=0)
        self.v_quality=gtk.HScale(self.v_adj)
        self.v_quality.set_value_pos(gtk.POS_LEFT)
        self.v_quality.set_digits(0)
        self.s_button= gtk.CheckButton(label=None)
        self.s_adj=gtk.Adjustment(value=(self.values[11]*10), lower=0, upper=100, step_incr=1, page_incr=10, page_size=0)
        #print (self.values[11]*10)
        self.s_quality=gtk.HScale(self.s_adj)
        self.s_quality.set_value_pos(gtk.POS_LEFT)
        self.s_quality.set_digits(0)
        self.v_label=gtk.Label("         "+self.labelStrings[0])
        self.s_label=gtk.Label(self.labelStrings[1])
        
        self.advanced_button=gtk.Button(self.buttonStrings[0])
        self.tipLabel=gtk.Label("")
        self.NEVQBox.pack_start(self.v_label,expand=False,fill=False)
        self.NEVQBox.pack_start(self.v_quality,expand=True,fill=True)
        self.NESQBox.pack_start(self.s_button,expand=False,fill=False)
        self.NESQBox.pack_start(self.s_label,expand=False,fill=False)
        self.NESQBox.pack_start(self.s_quality,expand=True,fill=True)
        self.NETABox.pack_start(self.advanced_button,expand=False,fill=False)
        self.NETABox.pack_start(self.tipLabel,expand=False,fill=False)
        
        self.win_button=gtk.Button(self.buttonStrings[1])
        self.start_button=gtk.Button(None,gtk.STOCK_MEDIA_RECORD)
        self.file_button=gtk.Button(None,gtk.STOCK_SAVE_AS)
        self.quit_button=gtk.Button(None,gtk.STOCK_QUIT)
        self.SWBox.pack_start(self.win_button,False,False)
        self.SWBox.pack_start(self.start_button,False,False)
        self.SEBox.pack_end(self.quit_button,False,False)
        self.SEBox.pack_end(self.file_button,False,False)
        
        self.win_button.show()
        self.start_button.show()
        self.file_button.show()
        self.quit_button.show()
        
        self.v_quality.show()
        self.s_button.show()
        self.s_quality.show()
        self.v_label.show()
        self.s_label.show()
        self.tipLabel.show()
        self.advanced_button.show()
        self.NEVQBox.show()
        self.NESQBox.show()
        self.NETABox.show()
        self.NEBox.show()
        self.NWBox.show()
        self.NBox.show()
        self.SEBox.show()
        self.SWBox.show()
        self.SBox.show()
        
        self.labelbox.show()
        self.window.add(self.labelbox)
        
    def __subsample__(self,im1,w,h,im2,stride,x=0,y=0):
        for i in xrange(y,h,stride):
            for k in xrange(x,w,stride):
                im2.put_pixel(k/stride,i/stride,im1.get_pixel(k,i))
    #def __draw_lines__(self,img,vals,factor):
        #for i in xrange(vals[0]/factor,vals[0]/factor+vals[2]/factor):
            #for k in range(2):
                #img.put_pixel(i,vals[1]/factor+k,0xff0000)
                #img.put_pixel(i,(vals[1]+vals[3]/2-k)/factor,0xff0000)
                #img.put_pixel(i,(vals[1]+vals[3]-1-k)/factor,0xff0000)
        #for i in xrange(vals[1]/factor,vals[1]/factor+vals[3]/factor):
            #for k in range(2):
                #img.put_pixel(vals[0]/factor+k,i,0xff0000)
                #img.put_pixel((vals[0]+vals[2]/2-k)/factor,i,0xff0000)
                #img.put_pixel((vals[0]+vals[2]-1-k)/factor,i,0xff0000)
      
    #def __update_image__(self):
        ##self.root.destroy()
        #if self.hidden[0]==0:
            #gc.collect()
            #sroot = self.wroot.get_image(0,0,self.wwidth,self.wheight)
            #self.__subsample__(sroot,self.wwidth,self.wheight,self.thumb,self.factor)
            #vals=[0,0, self.wwidth, self.wheight]
            #if self.values[5][0]>=0:
                #vals[0]=self.values[5][0]
            #if self.values[5][1]>=0:
                #vals[1]=self.values[5][1]
            #if self.values[5][2]>=0:
                #vals[2]=self.values[5][2]-self.values[5][0]
            #if self.values[5][3]>=0:
                #vals[3]=self.values[5][3]-self.values[5][1]
            #if vals[0] >0 or vals[1] >0 or vals[2] <self.wwidth or vals[3] <self.wheight:
                #self.__draw_lines__(self.thumb,vals,self.factor)
            #self.image.set_from_image(self.thumb,None)     
        #return True
    def __exit__(self,Event=None):
        gtk.main_quit()
        self.values[0]=-1
        self.optionsOpen[0]=0
        self.window.destroy()
    def hide(self,Event=None):
        self.window.hide()
        self.hidden[0]=1
    def show(self,Event=None):
        self.window.show()        
        self.hidden[0]=0
    def advanced(self,button=None):
        if self.optionsOpen[0] ==0:
            self.optionsOpen[0]=1
            self.options=pW.prefsWidget(self.values,self.optionsOpen)
            #self.parent.show()
        else:
            if self.options != None:
                self.options.window.destroy()
                self.optionsOpen[0]=0
    
    def __fileSelQuit__(self,Event=None):
        self.fileSel.destroy()
        
    def __fileSelOk__(self,Event=None):
        self.values[4]=self.fileSel.get_filename()
        #self.pathEntry.set_text(self.fileSel.get_filename())
        self.fileSel.destroy()

    def __fileSelect__(self,Event=None):
        self.fileSel = gtk.FileSelection(title=None)
        self.fileSel.ok_button.connect("clicked", self.__fileSelOk__) 
        self.fileSel.cancel_button.connect("clicked", self.__fileSelQuit__)
        self.fileSel.set_filename(self.values[4])
        self.fileSel.show() 
        
    def __makeCons__(self):
        self.advanced_button.connect("clicked",self.advanced)
        self.file_button.connect("clicked",self.__fileSelect__)
        self.quit_button.connect("clicked",self.__exit__)
        self.start_button.connect("clicked",self.trayIcon.record_ext)
        self.s_button.connect("clicked",self.__sound_check__)
    def __sound_check__(self,widget):
        self.s_quality.set_sensitive(widget.get_active())
        self.values[2]=widget.get_active()

    def update(self)  :
        self.values[10]=int((self.v_quality.get_value()*63)/100)
        self.values[11]=int(self.v_quality.get_value()/10)
        
    def __init__(self):
        self.values= [15,0,True,1,os.path.join(os.getenv('HOME'),'out.ogg'),[-1,-1,-1,-1],0,
            1,22050,'hw:0,0',63,10,"$DISPLAY",0,1,75,1]
        self.optionsOpen=[0]
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", self.__exit__)
        self.window.set_border_width(10)
        self.window.set_title("recordMyDesktop")
        self.__subWidgets__()
        
        #self.timed_id=gobject.timeout_add(2000,self.__update_image__)
        #if resolution is low let it decide size on it's own
        if self.wwidth>640:
            self.window.set_size_request(int(self.wwidth/self.factor*2.3),-1)
        self.window.set_resizable(False)
        
        self.trayIcon=trayIcon(self)
        self.__makeCons__()
        self.s_quality.set_sensitive(self.values[2])
        self.s_button.set_active(self.values[2])
        self.window.show()
        wh=self.window.size_request()
        self.window.set_size_request(wh[0],wh[1])
        gtk.main()











