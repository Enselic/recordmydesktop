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
                    _('MIT-Shm extension'),_('Include Window Decorations'),_('Tooltips'),
                    _('Video Quality'),_('Sound Quality'),
                    _('Drop Frames(encoder)'),_('Startup Delay(secs)'),]
    mouseStrings=[_('Normal'),_('White'),_('Black'),_('None')]
    stateStrings=[_('Enabled'),_('Disabled')]#0,1
    tabStrings=[_('Files'),_('Performance'),_('Sound'),_('Misc')]
    tooltipLabels=[_('By default, if you try to save under a filename that already\nexists, the new file will be renamed\n(for example if you try to save as out.ogg and that file exists, your file will be named out.ogg.1).\nBy checking this box, this behavior is disabled and the old file is overwritten.\n'),
                    _('Directory where temporary files will be saved.'),
                    _('Frames Per Second'),
                    _('Encode simultaneously with the recording.\nThis will require a lot more processing power.\n'),
                    _('Do not apply compression on the temporary files.\nIf enabled, the program will need less processing power,\nin the expense of hard disk space.\nThis option is valid only when encoding on the fly is disabled.'),
                    _('This option, if enabled, will cause the program to require less\nprocessing power, but it might make the colors look a bit more blurry.'),
                    _('Amount of screen change, within a frame that triggers retrieval\nof the screenshot through the MIT-Shared memory extension.\nThis option has no effect when using the full shots option.\n'),
                    _('This option should be enabled when recording 3d windows.\nIt is also auto-enabled when recording a\nknown 3d compositing window manager.\n '),
                    _('Number of audio channels.'),
                    _('Sound frequency.'),
                    _('ALSA sound device, that is used for sound recording.'),
                    _('Connection to the Xserver.'),
                    _('The mouse cursor that will be drawn.\nNormal is for the real cursor that you see while recording,\nwhile black and white are fake cursors, drawn by the program.\nYou can also disable cursor drawing.'),
                    _('Use the MIT-Shared memory extension, whenever appropriate,\n depending on the rest of the program settings.\nDisabling this option is not recommended,\nas it may severely slow down the program.'),
                    _('When selecting a window via the "Select Window" button,\ninclude that window\'s decorations in the recording area.'),
                    _('Enable or disable tooltips, like this one.\n(Requires restart)')]
    jacktip=_("Enable this option to record audio through\nJACK. The Jack server must be running in order to\nobtain the ports that will be recorded.\nThe audio recorded from each one\nwill be written on a channel of its own.\nrecordMyDesktop must be compiled with JACK\nsupport for this option to work.")
    def __tooltips__(self):
        self.tooltips=gtk.Tooltips()
        for i in range(16):
            self.tooltips.set_tip(self.eboxes[i],self.tooltipLabels[i])
        self.tooltips.set_tip(self.jack_ebox,self.jacktip)

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
        self.values[21]=self.winDecoComboBox.get_active()
        self.values[22]=self.jack_button.get_active()
        self.__getSelectedPorts__()
        self.values[24]=self.tooltipsComboBox.get_active()
        self.window.destroy()
        self.optionsOpen[0]=0

    def __exit__(self,Event=None):
        gtk.main_quit()
        self.values[0]=-1
        self.optionsOpen[0]=0
        self.window.destroy()

    def __getSelectedPorts__(self):
        iters_t=[]
        self.values[23]=[]
        sel_rows=self.jack_lsp_listview.get_selection().get_selected_rows()
        for i in sel_rows[1]:
            iters_t.append(self.jack_lsp_listview.get_model().get_iter(i))
        for i in iters_t:
            self.values[23].append(self.jack_lsp_listview.get_model().get_value(i,0))

    def __subWidgets__(self):
        self.labels={}
        self.boxes={}
        self.labelbox={}
        self.eboxes={}
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
            self.eboxes[i]=gtk.EventBox()
            self.eboxes[i].add(self.boxes[i])
            self.labelbox[0].pack_start(self.eboxes[i],expand=False,fill=False)

        self.overwriteFilesButton=gtk.CheckButton(label=None)
        self.overwriteFilesButton.set_active(self.values[20])
        self.overwriteFilesButton.show()
        self.boxes[0].pack_end(self.overwriteFilesButton,expand=False,fill=False)

        self.workdirEntry= gtk.Entry(max=0)
        self.workdirEntry.set_text(self.values[17])
        self.workdirEntry.show()
        self.boxes[1].pack_end(self.workdirEntry,expand=True,fill=True)

#Performance page
        for i in xrange(2,8):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i]=gtk.HBox(homogeneous=False, spacing=40)
            self.boxes[i].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            self.eboxes[i]=gtk.EventBox()
            self.eboxes[i].add(self.boxes[i])
            self.labelbox[1].pack_start(self.eboxes[i],expand=False,fill=False)

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


#sound page
        for i in xrange(8,11):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i]=gtk.HBox(homogeneous=False, spacing=0)
            self.boxes[i].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            self.eboxes[i]=gtk.EventBox()
            self.eboxes[i].add(self.boxes[i])
            self.labelbox[2].pack_start(self.eboxes[i],expand=False,fill=False)

        self.jack_button= gtk.CheckButton(_("Use Jack for audio capture."))
        self.jack_lsp_label=gtk.Label(_("Select the ports you want to record from\n"
                                        "(hold Ctrl to select multiple entries):"))

        self.jack_lsp_liststore=gtk.ListStore(str)

        self.jack_lsp_listview=gtk.TreeView(self.jack_lsp_liststore)
        self.jack_lsp_tvc = gtk.TreeViewColumn(_("Available Ports"))
        self.jack_lsp_listview.append_column(self.jack_lsp_tvc)
        self.cell = gtk.CellRendererText()
        self.jack_lsp_tvc.pack_start(self.cell, True)
        self.jack_lsp_tvc.add_attribute(self.cell, 'text', 0)
        self.jack_lsp_listview.set_search_column(0)
        self.jack_lsp_listview.set_reorderable(True)
        self.jack_lsp_listview.get_selection().set_mode(gtk.SELECTION_MULTIPLE)

        self.jack_lsp_refresh=gtk.Button(None,gtk.STOCK_REFRESH)
        self.jack_box=gtk.VBox(homogeneous=False, spacing=10)
        self.jack_box.pack_start(self.jack_button,expand=False,fill=False)
        self.jack_box.pack_start(self.jack_lsp_label,expand=False,fill=False)
        self.jack_box.pack_start(self.jack_lsp_listview,expand=False,fill=False)
        self.jack_box.pack_start(self.jack_lsp_refresh,expand=False,fill=False)

        self.jack_button.set_active(self.values[22])
        self.jack_button.show()
        self.jack_lsp_label.show()
        self.jack_lsp_listview.show()
        self.jack_lsp_refresh.show()

        self.jack_ebox=gtk.EventBox()
        self.jack_ebox.add(self.jack_box)
        self.jack_box.show()

        sep1=gtk.HSeparator()

        self.labelbox[2].pack_start(sep1,expand=False,fill=False)
        self.labelbox[2].pack_start(self.jack_ebox,expand=False,fill=False)
        sep1.show()
        self.jack_ebox.show()


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

#misc page
        for i in xrange(11,16):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i]=gtk.HBox(homogeneous=False, spacing=0)
            self.boxes[i].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            self.eboxes[i]=gtk.EventBox()
            self.eboxes[i].add(self.boxes[i])
            self.labelbox[3].pack_start(self.eboxes[i],expand=False,fill=False)

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

        self.winDecoComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.winDecoComboBox.append_text(self.stateStrings[i])
        self.winDecoComboBox.set_active(self.values[21])
        self.winDecoComboBox.show()
        self.boxes[14].pack_end(self.winDecoComboBox,expand=False,fill=False)

        self.tooltipsComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.tooltipsComboBox.append_text(self.stateStrings[i])
        self.tooltipsComboBox.set_active(self.values[24])
        self.tooltipsComboBox.show()
        self.boxes[15].pack_end(self.tooltipsComboBox,expand=False,fill=False)

        for i in range(16):
            self.boxes[i].show()
            self.eboxes[i].show()

#append and show
        for i in range(4):
            self.notebook.append_page(self.labelbox[i],gtk.Label(self.tabStrings[i]))
        self.window.add(self.notebook)
        for i in range(4):
            self.labelbox[i].show()
        self.notebook.show()
    def __runJackLSP__(self,button=None):
        if button!=None:
            self.__getSelectedPorts__()
        self.ports=[]
        failed=0
        (stdin,stdout,stderr)=os.popen3(['jack_lsp'],'t')
        ports=stdout.readlines()
        stdin.close()
        stdout.close()
        stderr.close()
        self.jack_lsp_liststore.clear()
        self.jack_lsp_listview.get_selection().unselect_all()
        if ports!=[]:
            for i in ports:
                self.ports.append(i.replace('\n',""))
            self.jack_lsp_listview.get_selection().set_mode(gtk.SELECTION_MULTIPLE)
        else:
            failed=1
            self.ports.append(_("jack_lsp returned no ports."))
            self.ports.append(_("Make sure that jackd is running."))
            self.jack_lsp_listview.get_selection().set_mode(gtk.SELECTION_NONE)
            self.values[23]=[]
        k=0
        for i in self.values[23]:
            self.jack_lsp_liststore.append([i])
            self.jack_lsp_listview.get_selection().select_path((k,))
            k+=1
        for i in self.ports:
            if i not in self.values[23]:
                self.jack_lsp_liststore.append([i])
        #we are selecting the first one to prevent messing the args
        if (not failed) and self.values[23]==[]:
            self.jack_lsp_listview.get_selection().select_path((0,))
            #print self.jack_lsp_listview.get_selection().get_selected_rows()
    def __makeCons__(self):
        self.jack_button.connect("clicked",self.__jack_enabled_check__)
        self.jack_lsp_refresh.connect("clicked",self.__runJackLSP__)

    def __jack_enabled_check__(self,widget):
        self.channelsSpinButton.set_sensitive(not widget.get_active())
        self.freqSpinButton.set_sensitive(not widget.get_active())
        self.deviceEntry.set_sensitive(not widget.get_active())
        self.jack_lsp_listview.set_sensitive(widget.get_active())
        self.jack_lsp_refresh.set_sensitive(widget.get_active())

    def __init__(self,values,optionsOpen):
        self.values=values
        self.ports=[]
        self.optionsOpen=optionsOpen
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", self.destroy)
        self.window.set_border_width(10)
        self.window.set_title("recordMyDesktop")
        self.__subWidgets__()
        self.__makeCons__()
        self.__runJackLSP__()
        self.__jack_enabled_check__(self.jack_button)
        if self.values[24]==0:
            self.__tooltips__()
        self.window.show()

    def main(self):
        gtk.main()








