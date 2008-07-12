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
from rmdStrings import *




class prefsWidget(object):
    def __tooltips__(self):
        self.tooltips=gtk.Tooltips()
        for i in range(19):
            self.tooltips.set_tip(self.eboxes[i],prefTooltipLabels[i])
        self.tooltips.set_tip(self.jack_ebox,prefJacktip)


    def destroy(self,Event=None):
        self.values[0]=self.fpsSpinButton.get_value_as_int()
        self.values[1]=self.mouseComboBox.get_active()
        self.values[3]=int(not self.fullComboBox.get_active())
        self.values[7]=self.channelsSpinButton.get_value_as_int()
        self.values[8]=self.freqSpinButton.get_value_as_int()
        self.values[9]=self.deviceEntry.get_text().replace(' ','')
        self.values[12]=self.displayEntry.get_text().replace(' ','')
        self.values[13]=int(not self.sharedComboBox.get_active())
        self.values[14]=int(not self.areaResetComboBox.get_active())
        self.values[15]=int(not self.followMouseComboBox.get_active())
        self.values[16]=int(not self.quickComboBox.get_active())
        self.values[17]=self.workdirEntry.get_text()
        self.values[18]=int(not self.onTheFlyComboBox.get_active())
        self.values[19]=int(not self.zeroCmpComboBox.get_active())
        self.values[20]=self.overwriteFilesButton.get_active()
        self.values[21]=int(not self.winDecoComboBox.get_active())
        self.values[22]=self.jack_button.get_active()
        self.__getSelectedPorts__()
        self.values[24]=int(not self.tooltipsComboBox.get_active())
        self.values[25]=int(not self.rFrameComboBox.get_active())
        self.values[26]=self.extraOptsEntry.get_text().strip()
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

    def __fileSelQuit__(self,Event=None):
        self.fileSel.destroy()

    def __fileSelOk__(self,Event=None):
        new_val=self.fileSel.get_filename()
        self.workdirEntry.set_text(new_val)
        self.fileSel.destroy()


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
            self.labels[i]=gtk.Label(prefLabelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i]=gtk.HBox(homogeneous=False, spacing=40)
            self.boxes[i].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            self.eboxes[i]=gtk.EventBox()
            self.eboxes[i].add(self.boxes[i])
            self.labelbox[0].pack_start(self.eboxes[i],expand=False,fill=False)

        self.workdir_layout=gtk.HBox(homogeneous=False, spacing=0)


        self.overwriteFilesButton=gtk.CheckButton(label=None)
        self.overwriteFilesButton.set_active(self.values[20])
        self.overwriteFilesButton.show()
        self.boxes[0].pack_end(self.overwriteFilesButton,expand=False,fill=False)

        self.workdirEntry= gtk.Entry(max=0)
        self.workdirEntry.set_text(self.values[17])
        self.workdirEntry.show()
        #self.boxes[1].pack_end(self.workdirEntry,expand=False,fill=False)
        #self.file_button=gtk.Button(None,gtk.STOCK_SAVE_AS)
        #self.file_button.show()
        self.workdir_button=gtk.Button(None,gtk.STOCK_SAVE_AS)
        self.workdir_button.show()


        self.workdir_layout.pack_start(self.workdirEntry,expand=True,fill=True)
        self.workdir_layout.pack_start(self.workdir_button,expand=False,fill=False)
        self.workdir_layout.show()

        self.labelbox[0].pack_start(self.workdir_layout,expand=False,fill=False)


#Performance page
        for i in xrange(2,7):
            self.labels[i]=gtk.Label(prefLabelStrings[i])
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

        self.onTheFlyComboBox = gtk.CheckButton(" ")
        self.onTheFlyComboBox.set_active(int(not self.values[18]))
        self.onTheFlyComboBox.show()
        self.boxes[3].pack_end(self.onTheFlyComboBox,expand=False,fill=False)

        self.zeroCmpComboBox = gtk.CheckButton(" ")
        self.zeroCmpComboBox.set_active(int(not self.values[19]))
        self.zeroCmpComboBox.show()
        self.boxes[4].pack_end(self.zeroCmpComboBox,expand=False,fill=False)

        self.quickComboBox = gtk.CheckButton(" ")
        self.quickComboBox.set_active(int(not self.values[16]))
        self.quickComboBox.show()
        self.boxes[5].pack_end(self.quickComboBox,expand=False,fill=False)

        self.fullComboBox = gtk.CheckButton(" ")
        self.fullComboBox.set_active(int(not self.values[3]))
        self.fullComboBox.show()
        self.boxes[6].pack_end(self.fullComboBox,expand=False,fill=False)


#sound page
        for i in xrange(7,10):
            self.labels[i]=gtk.Label(prefLabelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i]=gtk.HBox(homogeneous=False, spacing=0)
            self.boxes[i].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            self.eboxes[i]=gtk.EventBox()
            self.eboxes[i].add(self.boxes[i])
            self.labelbox[2].pack_start(self.eboxes[i],expand=False,fill=False)

        self.jack_button= gtk.CheckButton(prefStrings['UseJack'])
        self.jack_lsp_label=gtk.Label(prefStrings['SelectPorts'])

        self.jack_lsp_liststore=gtk.ListStore(str)

        self.jack_lsp_listview=gtk.TreeView(self.jack_lsp_liststore)
        self.jack_lsp_tvc = gtk.TreeViewColumn(prefStrings['AvailablePorts'])
        self.jack_lsp_listview.append_column(self.jack_lsp_tvc)
        self.cell = gtk.CellRendererText()
        self.jack_lsp_tvc.pack_start(self.cell, True)
        self.jack_lsp_tvc.add_attribute(self.cell, 'text', 0)
        self.jack_lsp_listview.set_search_column(0)
        self.jack_lsp_listview.set_reorderable(True)
        self.jack_lsp_listview.get_selection().set_mode(gtk.SELECTION_MULTIPLE)
        self.jack_scroll=gtk.ScrolledWindow(hadjustment=None, vadjustment=None)
        self.jack_scroll.add_with_viewport(self.jack_lsp_listview)

        self.jack_lsp_refresh=gtk.Button(None,gtk.STOCK_REFRESH)
        self.jack_box=gtk.VBox(homogeneous=False, spacing=10)
        self.jack_box.pack_start(self.jack_button,expand=False,fill=False)
        self.jack_box.pack_start(self.jack_lsp_label,expand=False,fill=False)
        self.jack_box.pack_start(self.jack_scroll,expand=True,fill=True)
        self.jack_box.pack_start(self.jack_lsp_refresh,expand=False,fill=False)



        self.jack_button.set_active(self.values[22])
        self.jack_button.show()
        self.jack_lsp_label.show()
        self.jack_lsp_listview.show()
        self.jack_scroll.set_size_request(-1,200)
        self.jack_scroll.show()
        self.jack_lsp_refresh.show()

        self.jack_ebox=gtk.EventBox()
        self.jack_ebox.add(self.jack_box)
        self.jack_box.show()

        sep1=gtk.HSeparator()

        self.labelbox[2].pack_start(sep1,expand=False,fill=False)
        self.labelbox[2].pack_start(self.jack_ebox,expand=True,fill=True)
        sep1.show()
        self.jack_ebox.show()


        self.channelsAdjustment=gtk.Adjustment(value=self.values[7], lower=1,upper=2, step_incr=1, page_incr=1, page_size=1)
        self.channelsSpinButton= gtk.SpinButton(self.channelsAdjustment, climb_rate=0.5, digits=0)
        self.channelsSpinButton.show()
        self.boxes[7].pack_end(self.channelsSpinButton,expand=False,fill=False)


        self.freqAdjustment=gtk.Adjustment(value=self.values[8], lower=0,upper=48000, step_incr=1, page_incr=100, page_size=0)
        self.freqSpinButton= gtk.SpinButton(self.freqAdjustment, climb_rate=0.5, digits=0)
        self.freqSpinButton.show()
        self.boxes[8].pack_end(self.freqSpinButton,expand=False,fill=False)


        self.deviceEntry= gtk.Entry(max=0)
        self.deviceEntry.set_text(self.values[9])
        self.deviceEntry.show()
        self.boxes[9].pack_end(self.deviceEntry,expand=False,fill=False)

#misc page
        for i in xrange(10,19):
            self.labels[i]=gtk.Label(prefLabelStrings[i])
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
        self.boxes[10].pack_end(self.displayEntry,expand=False,fill=False)


        self.mouseComboBox = gtk.combo_box_new_text()
        for i in range(4):
            self.mouseComboBox.append_text(prefMouseStrings[i])
        self.mouseComboBox.set_active(self.values[1])
        self.mouseComboBox.show()
        self.boxes[11].pack_end(self.mouseComboBox,expand=False,fill=False)

        self.followMouseComboBox = gtk.CheckButton(" ")
        self.followMouseComboBox.set_active(int(not self.values[15]))
        self.followMouseComboBox.show()
        self.boxes[12].pack_end(self.followMouseComboBox,expand=False,fill=False)

        self.sharedComboBox = gtk.CheckButton(" ")
        self.sharedComboBox.set_active(int(not self.values[13]))
        self.sharedComboBox.show()
        self.boxes[13].pack_end(self.sharedComboBox,expand=False,fill=False)

        self.winDecoComboBox = gtk.CheckButton(" ")
        self.winDecoComboBox.set_active(int(not self.values[21]))
        self.winDecoComboBox.show()
        self.boxes[14].pack_end(self.winDecoComboBox,expand=False,fill=False)

        self.tooltipsComboBox = gtk.CheckButton(" ")
        self.tooltipsComboBox.set_active(int(not self.values[24]))
        self.tooltipsComboBox.show()
        self.boxes[15].pack_end(self.tooltipsComboBox,expand=False,fill=False)


        self.rFrameComboBox = gtk.CheckButton(" ")
        self.rFrameComboBox.set_active(int(not self.values[25]))
        self.rFrameComboBox.show()
        self.boxes[16].pack_end(self.rFrameComboBox,expand=False,fill=False)

        self.areaResetComboBox = gtk.CheckButton(" ")
        self.areaResetComboBox.set_active(int(not self.values[14]))
        self.areaResetComboBox.show()
        self.boxes[17].pack_end(self.areaResetComboBox,expand=False,fill=False)

        self.extraOptsEntry= gtk.Entry(max=0)
        self.extraOptsEntry.set_text(self.values[26])
        self.extraOptsEntry.show()
        self.boxes[18].pack_end(self.extraOptsEntry,expand=False,fill=False)

        for i in range(19):
            self.boxes[i].show()
            self.eboxes[i].show()

#append and show
        for i in range(4):
            self.notebook.append_page(self.labelbox[i],gtk.Label(prefTabStrings[i]))
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
            self.ports.append(prefStrings['JackLspS1'])
            self.ports.append(prefStrings['JackLspS2'])
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

    def __workdirSelect__(self,Event=None):
        p=gtk.FileChooserDialog(title=None,
                                action=gtk.FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                buttons=(gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL,
                                         gtk.STOCK_OPEN,gtk.RESPONSE_OK)
                               )
        p.set_filename(self.workdirEntry.get_text())
        if p.run()==gtk.RESPONSE_OK:
            tpath=p.get_filename()
            if tpath!= '':
                self.workdirEntry.set_text(tpath)
        p.hide()
        p.destroy()

    def __makeCons__(self):
        self.jack_button.connect("clicked",self.__jack_enabled_check__)
        self.jack_lsp_refresh.connect("clicked",self.__runJackLSP__)
        self.workdir_button.connect("clicked",self.__workdirSelect__)

    def __register_shortcuts__(self):
        self.parent.advanced_button.add_accelerator("clicked",self.accel_group,
                                     ord('P'), gtk.gdk.CONTROL_MASK,
                                     gtk.ACCEL_VISIBLE)
        self.parent.advanced_button.add_accelerator("clicked",self.accel_group,
                                     gtk.gdk.keyval_from_name("Escape"),0,
                                     gtk.ACCEL_VISIBLE)

    def __jack_enabled_check__(self,widget):
        self.channelsSpinButton.set_sensitive(not widget.get_active())
        self.freqSpinButton.set_sensitive(not widget.get_active())
        self.deviceEntry.set_sensitive(not widget.get_active())
        self.jack_lsp_listview.set_sensitive(widget.get_active())
        self.jack_lsp_refresh.set_sensitive(widget.get_active())

    def __init__(self,parent,values,optionsOpen):
        self.values=values
        self.ports=[]
        self.parent=parent
        self.optionsOpen=optionsOpen
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", self.destroy)
        self.window.set_border_width(10)
        self.window.set_title("recordMyDesktop: "+smplButtonStrings[0])
        self.accel_group = gtk.AccelGroup()
        self.window.add_accel_group(self.accel_group)

        self.__subWidgets__()
        self.__makeCons__()
        self.__register_shortcuts__()
        self.__runJackLSP__()
        self.__jack_enabled_check__(self.jack_button)
        if self.values[24]==0:
            self.__tooltips__()
        self.window.show()

    def main(self):
        gtk.main()








