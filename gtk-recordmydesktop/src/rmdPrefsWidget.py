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
import rmdConfig
_ = gettext.gettext
gettext.textdomain('gtk-recordMyDesktop')
gettext.bindtextdomain('gtk-recordMyDesktop',rmdConfig.locale_install_dir)
import os




class prefsWidget(object):
    labelStrings=[_('Overwite Existing Files'),_('Working Directory'),
                    _('Frames Per Second'),_('Encode On the Fly'),_('Zero Compression'),
                    _('Quick Subsampling'),_('Shared Threshold'),_('Full shots at every frame'),
                    _('Channels'),_('Frequency'),_('Device'),_('Display'),_('Mouse Cursor'),
                    _('MIT-Shm extension'),_('Video Quality'),_('Sound Quality'),
                    _('Drop Frames(encoder)'),_('Startup Delay(secs)'),]
    mouseStrings=[_('Normal'),_('White'),_('Black'),_('None')]
    stateStrings=[_('Enabled'),_('Disabled')]#0,1
    tabStrings=[_('Files'),_('Performance'),_('Sound'),_('Misc')]

    def destroy(self,Event=None):
        self.values[0]=self.fpsSpinButton.get_value_as_int()
        self.values[1]=self.mouseComboBox.get_active()
        self.values[3]=self.fullComboBox.get_active()
        self.values[7]=self.channelsSpinButton.get_value_as_int()
        self.values[8]=self.freqSpinButton.get_value_as_int()
        self.values[9]=self.deviceEntry.get_text().replace(' ','')
        self.values[12]=self.displayEntry.get_text().replace(' ','')
        self.values[13]=self.sharedComboBox.get_active()
        self.values[15]=self.thresSpinButton.get_value_as_int()
        self.values[16]=self.quickComboBox.get_active()
        self.values[17]=self.workdirEntry.get_text()
        self.values[18]=self.onTheFlyComboBox.get_active()
        self.values[19]=self.zeroCmpComboBox.get_active()
        self.values[20]=self.overwriteFilesButton.get_active()

        self.window.destroy()
        self.optionsOpen[0]=0

    def __exit__(self,Event=None):
        gtk.main_quit()
        self.values[0]=-1
        self.optionsOpen[0]=0
        self.window.destroy()



    def __subWidgets__(self):
        self.labels={}
        self.boxes={}
        self.labelbox={}
        for i in range(4):
            self.labelbox[i]=gtk.VBox(homogeneous=False, spacing=20)
        self.notebook = gtk.Notebook()

#Files page
        for i in range(2):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i]=gtk.HBox(homogeneous=False, spacing=40)
            self.boxes[i].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            self.labelbox[0].pack_start(self.boxes[i],expand=False,fill=False)

        self.overwriteFilesButton=gtk.CheckButton(label=None)
        self.overwriteFilesButton.set_active(self.values[20])
        self.overwriteFilesButton.show()
        self.boxes[0].pack_end(self.overwriteFilesButton,expand=False,fill=False)

        self.workdirEntry= gtk.Entry(max=0)
        self.workdirEntry.set_text(self.values[17])
        self.workdirEntry.show()
        self.boxes[1].pack_end(self.workdirEntry,expand=True,fill=True)
        for i in range(2):
            self.boxes[i].show()
#Performance page
        for i in xrange(2,8):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i]=gtk.HBox(homogeneous=False, spacing=40)
            self.boxes[i].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            self.labelbox[1].pack_start(self.boxes[i],expand=False,fill=False)

        self.fpsAdjustment=gtk.Adjustment(value=self.values[0], lower=1, upper=50, step_incr=1, page_incr=5, page_size=0)
        self.fpsSpinButton= gtk.SpinButton(self.fpsAdjustment, climb_rate=0.5, digits=0)
        self.fpsSpinButton.show()
        self.boxes[2].pack_end(self.fpsSpinButton,expand=False,fill=False)

        self.onTheFlyComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.onTheFlyComboBox.append_text(self.stateStrings[i])
        self.onTheFlyComboBox.set_active(self.values[18])
        self.onTheFlyComboBox.show()
        self.boxes[3].pack_end(self.onTheFlyComboBox,expand=False,fill=False)

        self.zeroCmpComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.zeroCmpComboBox.append_text(self.stateStrings[i])
        self.zeroCmpComboBox.set_active(self.values[19])
        self.zeroCmpComboBox.show()
        self.boxes[4].pack_end(self.zeroCmpComboBox,expand=False,fill=False)

        self.quickComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.quickComboBox.append_text(self.stateStrings[i])
        self.quickComboBox.set_active(self.values[16])
        self.quickComboBox.show()
        self.boxes[5].pack_end(self.quickComboBox,expand=False,fill=False)

        self.thresAdjustment=gtk.Adjustment(value=self.values[15], lower=0,upper=100, step_incr=1, page_incr=5, page_size=1)
        self.thresSpinButton= gtk.SpinButton(self.thresAdjustment, climb_rate=0.5, digits=0)
        self.thresSpinButton.show()
        self.boxes[6].pack_end(self.thresSpinButton,expand=False,fill=False)

        self.fullComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.fullComboBox.append_text(self.stateStrings[i])
        self.fullComboBox.set_active(self.values[3])
        self.fullComboBox.show()
        self.boxes[7].pack_end(self.fullComboBox,expand=False,fill=False)


        for i in xrange(2,8):
            self.boxes[i].show()
#sound page
        for i in xrange(8,11):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i]=gtk.HBox(homogeneous=False, spacing=0)
            self.boxes[i].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            self.labelbox[2].pack_start(self.boxes[i],expand=False,fill=False)


        self.channelsAdjustment=gtk.Adjustment(value=self.values[7], lower=1,upper=2, step_incr=1, page_incr=1, page_size=1)
        self.channelsSpinButton= gtk.SpinButton(self.channelsAdjustment, climb_rate=0.5, digits=0)
        self.channelsSpinButton.show()
        self.boxes[8].pack_end(self.channelsSpinButton,expand=False,fill=False)


        self.freqAdjustment=gtk.Adjustment(value=self.values[8], lower=0,upper=44100, step_incr=1, page_incr=100, page_size=0)
        self.freqSpinButton= gtk.SpinButton(self.freqAdjustment, climb_rate=0.5, digits=0)
        self.freqSpinButton.show()
        self.boxes[9].pack_end(self.freqSpinButton,expand=False,fill=False)


        self.deviceEntry= gtk.Entry(max=0)
        self.deviceEntry.set_text(self.values[9])
        self.deviceEntry.show()
        self.boxes[10].pack_end(self.deviceEntry,expand=False,fill=False)

        for i in xrange(8,11):
            self.boxes[i].show()
#misc page
        for i in xrange(11,14):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i]=gtk.HBox(homogeneous=False, spacing=0)
            self.boxes[i].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            self.labelbox[3].pack_start(self.boxes[i],expand=False,fill=False)

        self.displayEntry= gtk.Entry(max=0)
        self.displayEntry.set_text(self.values[12])
        self.displayEntry.show()
        self.boxes[11].pack_end(self.displayEntry,expand=False,fill=False)


        self.mouseComboBox = gtk.combo_box_new_text()
        for i in range(4):
            self.mouseComboBox.append_text(self.mouseStrings[i])
        self.mouseComboBox.set_active(self.values[1])
        self.mouseComboBox.show()
        self.boxes[12].pack_end(self.mouseComboBox,expand=False,fill=False)


        self.sharedComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.sharedComboBox.append_text(self.stateStrings[i])
        self.sharedComboBox.set_active(self.values[13])
        self.sharedComboBox.show()
        self.boxes[13].pack_end(self.sharedComboBox,expand=False,fill=False)

        for i in xrange(11,14):
            self.boxes[i].show()


#append and show
        for i in range(4):
            self.notebook.append_page(self.labelbox[i],gtk.Label(self.tabStrings[i]))
        self.window.add(self.notebook)
        for i in range(4):
            self.labelbox[i].show()
        self.notebook.show()


    def __init__(self,values,optionsOpen):
        self.values=values
        self.optionsOpen=optionsOpen
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", self.destroy)
        self.window.set_border_width(10)
        self.window.set_title("recordMyDesktop")
        self.__subWidgets__()

        #self.window.set_size_request(350,-1)
        self.window.show()

    def main(self):
        gtk.main()








