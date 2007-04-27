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
def htmlize(s):
    return "<html><body>"+s.replace("\n","<br>")+"</html></body>"
gettext.textdomain('qt-recordMyDesktop')
gettext.bindtextdomain('qt-recordMyDesktop',rmdConfig.locale_install_dir)
import os




class prefsWidget(object):
    labelStrings=[_('Overwite Existing Files'),_('Working Directory'),
                    _('Frames Per Second'),_('Encode On the Fly'),_('Zero Compression'),
                    _('Quick Subsampling'),_('Full shots at every frame'),
                    _('Channels'),_('Frequency'),_('Device'),_('Display'),_('Mouse Cursor'),
                    _('MIT-Shm extension'),_('Include Window Decorations'),_('Tooltips'),
                    _('Outline Capture Area On Screen'),
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
                    _('This option should be enabled when recording 3d windows.\nIt is also auto-enabled when recording a\nknown 3d compositing window manager.\n '),
                    _('Number of audio channels.'),
                    _('Sound frequency.'),
                    _('ALSA sound device, that is used for sound recording.'),
                    _('Connection to the Xserver.'),
                    _('The mouse cursor that will be drawn.\nNormal is for the real cursor that you see while recording,\nwhile black and white are fake cursors, drawn by the program.\nYou can also disable cursor drawing.'),
                    _('Use the MIT-Shared memory extension, whenever appropriate,\n depending on the rest of the program settings.\nDisabling this option is not recommended,\nas it may severely slow down the program.'),
                    _('When selecting a window via the "Select Window" button,\ninclude that window\'s decorations in the recording area.'),
                    _('Enable or disable tooltips, like this one.\n(Requires restart)'),
                    _('Draw a frame on the screen, around the area\nthat will get captured.\n(This frame will remain throughout the recording.)')]
    jacktip=_("Enable this option to record audio through\nJACK. The Jack server must be running in order to\nobtain the ports that will be recorded.\nThe audio recorded from each one\nwill be written on a channel of its own.\nrecordMyDesktop must be compiled with JACK\nsupport for this option to work.")
    def __tooltips__(self):
        for i in range(16):
            self.labels[i].setToolTip(htmlize(self.tooltipLabels[i]))
        self.jack_button.setToolTip(htmlize(self.jacktip))
        self.jack_lsp_label.setToolTip(htmlize(self.jacktip))
        self.jack_lss_label.setToolTip(htmlize(self.jacktip))
        self.jack_lsp_liststore.setToolTip(htmlize(self.jacktip))
        self.jack_lsp_listSave.setToolTip(htmlize(self.jacktip))

    def destroy(self,Event=None):
        self.values[0]=self.fpsSpinButton.value()
        self.values[1]=self.mouseComboBox.currentIndex()
        self.values[3]=self.fullComboBox.currentIndex()
        self.values[7]=self.channelsSpinButton.value()
        self.values[8]=self.freqSpinButton.value()
        self.values[9]=self.deviceEntry.text().replace(' ','')
        self.values[12]=self.displayEntry.text().replace(' ','')
        self.values[13]=self.sharedComboBox.currentIndex()
        self.values[16]=self.quickComboBox.currentIndex()
        self.values[17]=self.workdirEntry.text()
        self.values[18]=self.onTheFlyComboBox.currentIndex()
        self.values[19]=self.zeroCmpComboBox.currentIndex()
        if self.overwriteFilesButton.checkState()==QtCore.Qt.Checked:
            self.values[20]=True
        else:
            self.values[20]=False
        self.values[21]=self.winDecoComboBox.currentIndex()
        if self.jack_button.checkState()==QtCore.Qt.Checked:
            self.values[22]=True
        else:
            self.values[22]=False
        self.__getSelectedPorts__()
        self.values[24]=self.tooltipsComboBox.currentIndex()
        self.values[25]=self.rFrameComboBox.currentIndex()
        self.optionsOpen[0]=0

    def __getSelectedPorts__(self):
        iters_t=[]
        self.values[23]=[]
        sel_rows=self.jack_lsp_listSave.count()
        for i in range(sel_rows):
            self.values[23].append(self.jack_lsp_listSave.item(i).text())
        if sel_rows==0:
            self.values[22]=False

    def __subWidgets__(self):
        self.labels={}
        self.boxes={}
        self.labelbox={}
        self.tabWidgets={}
        self.notebook = QtGui.QTabWidget(self.window)
        for i in range(4):
            self.labelbox[i]=QtGui.QVBoxLayout()
            self.tabWidgets[i]=QtGui.QWidget()
#Files page
        for i in range(2):
            self.labels[i]=QtGui.QLabel(self.labelStrings[i],self.tabWidgets[0])
            self.labels[i].setAlignment(QtCore.Qt.AlignLeft)
            self.boxes[i]=QtGui.QHBoxLayout()
            self.boxes[i].insertWidget(-1,self.labels[i])
            self.labelbox[0].insertLayout(-1,self.boxes[i])
            self.labelbox[0].insertSpacing(-1,20)
        self.labelbox[0].setSizeConstraint(QtGui.QLayout.SetMaximumSize)

        self.overwriteFilesButton=QtGui.QCheckBox(self.tabWidgets[0])
        if self.values[20]:
            self.overwriteFilesButton.setCheckState(QtCore.Qt.Checked)
        else:
            self.overwriteFilesButton.setCheckState(QtCore.Qt.Unchecked)
        self.boxes[0].insertWidget(-1,self.overwriteFilesButton)

        self.workdirEntry= QtGui.QLineEdit(self.values[17],self.tabWidgets[0])
        self.boxes[1].insertWidget(-1,self.workdirEntry)

#Performance page
        for i in xrange(2,7):
            self.labels[i]=QtGui.QLabel(self.labelStrings[i],self.tabWidgets[1])
            self.labels[i].setAlignment(QtCore.Qt.AlignLeft)
            self.boxes[i]=QtGui.QHBoxLayout()
            self.boxes[i].insertWidget(-1,self.labels[i])
            self.labelbox[1].insertLayout(-1,self.boxes[i])
            self.labelbox[1].insertSpacing(-1,20)
        self.labelbox[1].setSizeConstraint(QtGui.QLayout.SetMaximumSize)

        self.fpsSpinButton= QtGui.QSpinBox(self.tabWidgets[1])
        self.fpsSpinButton.setRange(1,50)
        self.fpsSpinButton.setValue(self.values[0])
        self.boxes[2].insertWidget(-1,self.fpsSpinButton)

        self.onTheFlyComboBox = QtGui.QComboBox(self.tabWidgets[1])
        for i in range(2):
            self.onTheFlyComboBox.addItem(self.stateStrings[i])
        self.onTheFlyComboBox.setCurrentIndex(self.values[18])
        self.boxes[3].insertWidget(-1,self.onTheFlyComboBox)

        self.zeroCmpComboBox = QtGui.QComboBox(self.tabWidgets[1])
        for i in range(2):
            self.zeroCmpComboBox.addItem(self.stateStrings[i])
        self.zeroCmpComboBox.setCurrentIndex(self.values[19])
        self.boxes[4].insertWidget(-1,self.zeroCmpComboBox)

        self.quickComboBox = QtGui.QComboBox(self.tabWidgets[1])
        for i in range(2):
            self.quickComboBox.addItem(self.stateStrings[i])
        self.quickComboBox.setCurrentIndex(self.values[16])
        self.boxes[5].insertWidget(-1,self.quickComboBox)

        self.fullComboBox = QtGui.QComboBox(self.tabWidgets[1])
        for i in range(2):
            self.fullComboBox.addItem(self.stateStrings[i])
        self.fullComboBox.setCurrentIndex(self.values[3])
        self.boxes[6].insertWidget(-1,self.fullComboBox)

#sound page
        for i in xrange(7,10):
            self.labels[i]=QtGui.QLabel(self.labelStrings[i],self.tabWidgets[2])
            self.labels[i].setAlignment(QtCore.Qt.AlignLeft)
            self.boxes[i]=QtGui.QHBoxLayout()
            self.boxes[i].insertWidget(-1,self.labels[i])
            self.labelbox[2].insertLayout(-1,self.boxes[i])
            self.labelbox[2].insertSpacing(-1,20)
        self.labelbox[2].setSizeConstraint(QtGui.QLayout.SetMaximumSize)

        self.labelbox[2].insertSpacing(-1,15)
        self.jack_button= QtGui.QCheckBox(_("Use Jack for audio capture."),self.tabWidgets[2])
        self.jack_lsp_label=QtGui.QLabel(_("Available ports (double click to add):"),self.tabWidgets[2])
        self.jack_lss_label=QtGui.QLabel(_("Selected ports (double click to remove):"),self.tabWidgets[2])
        self.jack_lsp_liststore=QtGui.QListWidget(self.tabWidgets[2])
        self.jack_lsp_liststore.setFixedHeight(100)
        self.jack_lsp_listSave=QtGui.QListWidget(self.tabWidgets[2])
        self.jack_lsp_listSave.setFixedHeight(100)


        self.jack_lsp_refresh=QtGui.QPushButton(_("Refresh"),self.tabWidgets[2])
        self.jack_box=QtGui.QVBoxLayout()
        self.jack_box.insertWidget(-1,self.jack_button)
        self.jack_box.insertWidget(-1,self.jack_lsp_label)
        self.jack_box.insertWidget(-1,self.jack_lsp_liststore)
        self.jack_box.insertWidget(-1,self.jack_lss_label)
        self.jack_box.insertWidget(-1,self.jack_lsp_listSave)
        self.jack_box.insertWidget(-1,self.jack_lsp_refresh)
        self.labelbox[2].insertLayout(-1,self.jack_box)


        if self.values[22]==True:
            self.jack_button.setCheckState(QtCore.Qt.Checked)
        else:
            self.jack_button.setCheckState(QtCore.Qt.Unchecked)

        self.channelsSpinButton= QtGui.QSpinBox(self.tabWidgets[2])
        self.channelsSpinButton.setRange(1,2)
        self.channelsSpinButton.setValue(self.values[7])
        self.boxes[7].insertWidget(-1,self.channelsSpinButton)


        self.freqSpinButton= QtGui.QSpinBox(self.tabWidgets[2])
        self.freqSpinButton.setRange(0,44100)
        self.freqSpinButton.setSingleStep(100)
        self.freqSpinButton.setValue(self.values[8])
        self.boxes[8].insertWidget(-1,self.freqSpinButton)

        self.deviceEntry= QtGui.QLineEdit(self.values[9],self.tabWidgets[2])
        self.boxes[9].insertWidget(-1,self.deviceEntry)

#misc page
        for i in xrange(10,16):
            self.labels[i]=QtGui.QLabel(self.labelStrings[i],self.tabWidgets[3])
            self.labels[i].setAlignment(QtCore.Qt.AlignLeft)
            self.boxes[i]=QtGui.QHBoxLayout()
            self.boxes[i].insertWidget(-1,self.labels[i])
            self.labelbox[3].insertLayout(-1,self.boxes[i])
            self.labelbox[3].insertSpacing(-1,20)
        self.labelbox[3].setSizeConstraint(QtGui.QLayout.SetMaximumSize)

        self.displayEntry= QtGui.QLineEdit(self.values[12])
        self.boxes[10].insertWidget(-1,self.displayEntry)

        self.mouseComboBox = QtGui.QComboBox(self.tabWidgets[3])
        for i in range(4):
            self.mouseComboBox.addItem(self.mouseStrings[i])
        self.mouseComboBox.setCurrentIndex(self.values[1])
        self.boxes[11].insertWidget(-1,self.mouseComboBox)


        self.sharedComboBox = QtGui.QComboBox(self.tabWidgets[3])
        for i in range(2):
            self.sharedComboBox.addItem(self.stateStrings[i])
        self.sharedComboBox.setCurrentIndex(self.values[13])
        self.boxes[12].insertWidget(-1,self.sharedComboBox)

        self.winDecoComboBox = QtGui.QComboBox(self.tabWidgets[3])
        for i in range(2):
            self.winDecoComboBox.addItem(self.stateStrings[i])
        self.winDecoComboBox.setCurrentIndex(self.values[21])
        self.boxes[13].insertWidget(-1,self.winDecoComboBox)

        self.tooltipsComboBox = QtGui.QComboBox(self.tabWidgets[3])
        for i in range(2):
            self.tooltipsComboBox.addItem(self.stateStrings[i])
        self.tooltipsComboBox.setCurrentIndex(self.values[24])
        self.boxes[14].insertWidget(-1,self.tooltipsComboBox)


        self.rFrameComboBox = QtGui.QComboBox(self.tabWidgets[3])
        for i in range(2):
            self.rFrameComboBox.addItem(self.stateStrings[i])
        self.rFrameComboBox.setCurrentIndex(self.values[25])
        self.boxes[15].insertWidget(-1,self.rFrameComboBox)


        for i in range(4):
            self.tabWidgets[i].setLayout(self.labelbox[i])
            self.notebook.insertTab(-1,self.tabWidgets[i],self.tabStrings[i])
        self.notebook.show()
        return

    def addPort(self,lwi):
        pli=lwi.clone()
        self.jack_lsp_listSave.insertItem(-1,pli)
    def remPort(self,lwi):
        self.jack_lsp_listSave.takeItem(self.jack_lsp_listSave.row(lwi))

    def __runJackLSP__(self,button=None):
        self.ports=[]
        failed=0
        (stdin,stdout,stderr)=os.popen3(['jack_lsp'],'t')
        ports=stdout.readlines()
        stdin.close()
        stdout.close()
        stderr.close()
        self.jack_lsp_liststore.clear()
        if ports!=[]:
            for i in ports:
                self.ports.append(i.replace('\n',""))
            self.jack_lsp_liststore.setEnabled(True)
        else:
            failed=1
            self.ports.append(_("jack_lsp returned no ports."))
            self.ports.append(_("Make sure that jackd is running."))
            self.jack_lsp_liststore.setEnabled(False)
            self.jack_lsp_listSave.setEnabled(False)
            self.values[23]=[]
        for i in self.values[23]:
            self.jack_lsp_listSave.insertItem(-1,QtGui.QListWidgetItem(i))
        for i in self.ports:
            self.jack_lsp_liststore.insertItem(-1,QtGui.QListWidgetItem(i))


    def __makeCons__(self):
        self.jack_button.connect(self.jack_button,QtCore.SIGNAL("clicked()"),self.__jack_enabled_check__)
        self.jack_lsp_refresh.connect(self.jack_lsp_refresh,QtCore.SIGNAL("clicked()"),self.__runJackLSP__)
        self.jack_lsp_liststore.connect(self.jack_lsp_liststore,QtCore.SIGNAL("itemDoubleClicked(QListWidgetItem *)"),self.addPort)
        self.jack_lsp_listSave.connect(self.jack_lsp_listSave,QtCore.SIGNAL("itemDoubleClicked(QListWidgetItem *)"),self.remPort)

    def __jack_enabled_check__(self):
        val=False
        if self.jack_button.checkState()==QtCore.Qt.Checked:
            val=True
        else:
            val=False
        self.channelsSpinButton.setEnabled(not val)
        self.freqSpinButton.setEnabled(not val)
        self.deviceEntry.setEnabled(not val)
        self.jack_lsp_liststore.setEnabled(val)
        self.jack_lsp_listSave.setEnabled(val)
        self.jack_lsp_refresh.setEnabled(val)

    def __init__(self,values,optionsOpen):
        self.values=values
        self.ports=[]
        self.optionsOpen=optionsOpen
        self.window = QtGui.QWidget()
        self.window.closeEvent= self.destroy

        self.window.setWindowTitle("recordMyDesktop")
        self.__subWidgets__()
        self.__makeCons__()
        self.__runJackLSP__()
        self.__jack_enabled_check__()
        if self.values[24]==0:
            self.__tooltips__()
        self.window.show()










