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




class prefsWidget(object):
    labelStrings=[_('Frames Per Second'),_('Sound Recording'),_('Startup Delay(secs)'),
                    _('Channels'),_('Frequency'),_('Device'),_('Video Quality'),_('Sound Quality'),
                    _('Display'),_('Mouse Cursor'),_('Full shots at every frame'),_('MIT-Shm extension'),
                    _('Drop Frames(encoder)'),_('Shared Threshold'),_('Quick Subsampling')]
    mouseStrings=[_('Normal'),_('White'),_('Black'),_('None')]
    stateStrings=[_('Enabled'),_('Disabled')]
    tabStrings=[_('Basic'),_('Sound'),_('Encoding'),_('Advanced')]
    
    def destroy(self,Event=None):
        self.values[0]=self.fpsSpinButton.get_value_as_int()
        self.values[1]=self.mouseComboBox.get_active()
        self.values[2]=self.soundComboBox.get_active()
        self.values[3]=self.fullComboBox.get_active()
        self.values[4]=self.pathEntry.get_text()
        #self.values[5]=self.are
        self.values[6]=self.delaySpinButton.get_value_as_int()
        self.values[7]=self.channelsSpinButton.get_value_as_int()
        self.values[8]=self.freqSpinButton.get_value_as_int()
        self.values[9]=self.deviceEntry.get_text().replace(' ','')
        self.values[10]=self.vidqSpinButton.get_value_as_int()
        self.values[11]=self.soundqSpinButton.get_value_as_int()
        self.values[12]=self.displayEntry.get_text().replace(' ','')
        self.values[13]=self.sharedComboBox.get_active()
        self.values[14]=self.frdropComboBox.get_active()
        self.values[15]=self.thresSpinButton.get_value_as_int()
        self.values[16]=self.quickComboBox.get_active()
        
        self.window.destroy()
        self.optionsOpen[0]=0

    def __exit__(self,Event=None):
        gtk.main_quit()
        self.values[0]=-1
        self.optionsOpen[0]=0
        self.window.destroy()
        
    def __fileSelQuit__(self,Event=None):
        self.fileSel.destroy()
        
    def __fileSelOk__(self,Event=None):
        self.pathEntry.set_text(self.fileSel.get_filename())
        self.fileSel.destroy()

    def __fileSelect__(self,Event=None):
        self.fileSel = gtk.FileSelection(title=None)
        self.fileSel.ok_button.connect("clicked", self.__fileSelOk__) 
        self.fileSel.cancel_button.connect("clicked", self.__fileSelQuit__)
        self.fileSel.set_filename(self.values[4])
        self.fileSel.show() 
       
    def __subWidgets__(self):
        self.labels={}
        self.boxes={}
        self.labelbox={}
        for i in range(4):
            self.labelbox[i]=gtk.VBox(homogeneous=False, spacing=20)
        self.notebook = gtk.Notebook()

#basic page
        self.fpsAdjustment=gtk.Adjustment(value=self.values[0], lower=1, upper=50, step_incr=1, page_incr=5, page_size=0)
        self.fpsSpinButton= gtk.SpinButton(self.fpsAdjustment, climb_rate=0.5, digits=0)


        self.soundComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.soundComboBox.append_text(self.stateStrings[i])
        self.soundComboBox.set_active(self.values[2])
        
        self.delayAdjustment=gtk.Adjustment(value=self.values[6], lower=0,upper=10000, step_incr=1, page_incr=5, page_size=0)
        self.delaySpinButton= gtk.SpinButton(self.delayAdjustment, climb_rate=0.5, digits=0)

        self.pathEntry= gtk.Entry(max=0)
        self.pathEntry.set_text(self.values[4])
        self.pathButton = gtk.Button(None,gtk.STOCK_SAVE_AS)
        
        self.okButton=gtk.Button(None,gtk.STOCK_OK)
        
        for i in range(3):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i]=gtk.HBox(homogeneous=False, spacing=0)
            self.boxes[i].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            self.labelbox[0].pack_start(self.boxes[i],expand=False,fill=False)

        self.boxes[3]=gtk.HBox(homogeneous=False, spacing=20)
        self.labelbox[0].pack_start(self.boxes[3],expand=False,fill=False)                       
        placeholder2=gtk.Label("")
        placeholder2.show()
        self.labelbox[0].pack_start(placeholder2)
        self.boxes[4]=gtk.HBox(homogeneous=False, spacing=0)
        self.labelbox[0].pack_end(self.boxes[4])
    
        self.fpsSpinButton.show()
        self.boxes[0].pack_end(self.fpsSpinButton,expand=False,fill=False)
        self.soundComboBox.show()
        self.boxes[1].pack_end(self.soundComboBox,expand=False,fill=False)
        self.delaySpinButton.show()
        self.boxes[2].pack_end(self.delaySpinButton,expand=False,fill=False)
        self.pathEntry.show()
        self.boxes[3].pack_start(self.pathEntry,expand=False,fill=False)
        self.pathButton.show()
        self.boxes[3].pack_end(self.pathButton,expand=False,fill=False)
        self.okButton.show()
        self.boxes[4].pack_start(self.okButton,expand=True,fill=True)
        for i in range(5):
            self.boxes[i].show()
#sound page
        for i in xrange(3,6):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i+5]=gtk.HBox(homogeneous=False, spacing=0)
            self.boxes[i+5].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            #self.boxes[i+5].show()
            self.labelbox[1].pack_start(self.boxes[i+5],expand=False,fill=False)
        self.channelsAdjustment=gtk.Adjustment(value=self.values[7], lower=1,upper=2, step_incr=1, page_incr=1, page_size=1)
        self.channelsSpinButton= gtk.SpinButton(self.channelsAdjustment, climb_rate=0.5, digits=0)
        self.boxes[8].pack_end(self.channelsSpinButton,expand=False,fill=False)
        self.channelsSpinButton.show()
        self.boxes[8].show()
        
        self.freqAdjustment=gtk.Adjustment(value=self.values[8], lower=0,upper=44100, step_incr=1, page_incr=100, page_size=0)
        self.freqSpinButton= gtk.SpinButton(self.freqAdjustment, climb_rate=0.5, digits=0)
        self.boxes[9].pack_end(self.freqSpinButton,expand=False,fill=False)
        self.freqSpinButton.show()
        self.boxes[9].show()
        
        self.deviceEntry= gtk.Entry(max=0)
        self.deviceEntry.set_text(self.values[9])
        self.boxes[10].pack_end(self.deviceEntry,expand=False,fill=False)
        self.deviceEntry.show()
        self.boxes[10].show()
        
        
#encoding page        
        for i in xrange(6,8):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i+5]=gtk.HBox(homogeneous=False, spacing=0)
            self.boxes[i+5].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            #self.boxes[i+5].show()
            self.labelbox[2].pack_start(self.boxes[i+5],expand=False,fill=False)
        self.vidqAdjustment=gtk.Adjustment(value=self.values[10], lower=0,upper=63, step_incr=1, page_incr=1, page_size=1)
        self.vidqSpinButton= gtk.SpinButton(self.vidqAdjustment, climb_rate=0.5, digits=0)
        self.boxes[11].pack_end(self.vidqSpinButton,expand=False,fill=False)
        self.vidqSpinButton.show()
        self.boxes[11].show()
        self.soundqAdjustment=gtk.Adjustment(value=self.values[11], lower=-1,upper=10, step_incr=1, page_incr=1, page_size=1)
        self.soundqSpinButton= gtk.SpinButton(self.soundqAdjustment, climb_rate=0.5, digits=0)
        self.boxes[12].pack_end(self.soundqSpinButton,expand=False,fill=False)
        self.soundqSpinButton.show()
        self.boxes[12].show()

#misc page
        for i in xrange(8,15):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i+5]=gtk.HBox(homogeneous=False, spacing=0)
            self.boxes[i+5].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            #self.boxes[i+5].show()
            self.labelbox[3].pack_start(self.boxes[i+5],expand=False,fill=False)
        self.displayEntry= gtk.Entry(max=0)
        self.displayEntry.set_text(self.values[12])
        self.boxes[13].pack_end(self.displayEntry,expand=False,fill=False)
        self.displayEntry.show()
        self.boxes[13].show()

        self.mouseComboBox = gtk.combo_box_new_text()
        for i in range(4):
            self.mouseComboBox.append_text(self.mouseStrings[i])
        self.mouseComboBox.set_active(self.values[1])
        self.boxes[14].pack_end(self.mouseComboBox,expand=False,fill=False)
        self.mouseComboBox.show()
        self.boxes[14].show()
        
        self.fullComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.fullComboBox.append_text(self.stateStrings[i])
        self.fullComboBox.set_active(self.values[3])
        self.boxes[15].pack_end(self.fullComboBox,expand=False,fill=False)
        self.fullComboBox.show()
        self.boxes[15].show()
        
        self.sharedComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.sharedComboBox.append_text(self.stateStrings[i])
        self.sharedComboBox.set_active(self.values[13])
        self.boxes[16].pack_end(self.sharedComboBox,expand=False,fill=False)
        self.sharedComboBox.show()
        self.boxes[16].show()
        
        
        self.frdropComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.frdropComboBox.append_text(self.stateStrings[i])
        self.frdropComboBox.set_active(self.values[14])
        self.boxes[17].pack_end(self.frdropComboBox,expand=False,fill=False)
        self.frdropComboBox.show()
        self.boxes[17].show()
        
        self.thresAdjustment=gtk.Adjustment(value=self.values[15], lower=0,upper=100, step_incr=1, page_incr=5, page_size=1)
        self.thresSpinButton= gtk.SpinButton(self.thresAdjustment, climb_rate=0.5, digits=0)
        self.boxes[18].pack_end(self.thresSpinButton,expand=False,fill=False)
        self.thresSpinButton.show()
        self.boxes[18].show()

        self.quickComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.quickComboBox.append_text(self.stateStrings[i])
        self.quickComboBox.set_active(self.values[16])
        self.boxes[19].pack_end(self.quickComboBox,expand=False,fill=False)
        self.quickComboBox.show()
        self.boxes[19].show()        
        
#append and show        
        for i in range(4):
            self.notebook.append_page(self.labelbox[i],gtk.Label(self.tabStrings[i]))
        self.window.add(self.notebook)
        for i in range(4):
            self.labelbox[i].show()
        self.notebook.show()

    def __makeCons__(self):
        self.pathButton.connect("clicked",self.__fileSelect__)
        self.okButton.connect("clicked",self.destroy)

    def __init__(self,values,optionsOpen):
        self.values=values
        self.optionsOpen=optionsOpen
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", self.destroy)
        self.window.set_border_width(10)
        self.window.set_title("recordMyDesktop")
        self.__subWidgets__()
        self.__makeCons__()
        
        
        self.window.set_size_request(288,384)
        self.window.show()
        
    def main(self):
        gtk.main()








