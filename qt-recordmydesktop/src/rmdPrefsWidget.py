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
from rmdStrings import *




class prefsWidget(object):
    def __tooltips__(self):
        for i in range(19):
            self.labels[i].setToolTip(htmlize(prefTooltipLabels[i]))
        self.jack_button.setToolTip(htmlize(prefJacktip))
        self.jack_lsp_label.setToolTip(htmlize(prefJacktip))
        self.jack_lss_label.setToolTip(htmlize(prefJacktip))
        self.jack_lsp_liststore.setToolTip(htmlize(prefJacktip))
        self.jack_lsp_listSave.setToolTip(htmlize(prefJacktip))

    def __tButToBox__(self,checkbox):
        if checkbox.checkState()==QtCore.Qt.Checked:
            return 0
        else:
            return 1
    def __rCheckState__(self,val):
        if val==0:
            return QtCore.Qt.Checked
        else:
            return QtCore.Qt.Unchecked


    def destroy(self,Event=None):
        self.values[0]=self.fpsSpinButton.value()
        self.values[1]=self.mouseComboBox.currentIndex()
        self.values[3]=self.__tButToBox__(self.fullComboBox)
        self.values[7]=self.channelsSpinButton.value()
        self.values[8]=self.freqSpinButton.value()
        self.values[9]=self.deviceEntry.text().replace(' ','')
        self.values[12]=self.displayEntry.text().replace(' ','')
        self.values[13]=self.__tButToBox__(self.sharedComboBox)
        self.values[14]=self.__tButToBox__(self.areaResetComboBox)
        self.values[15]=self.__tButToBox__(self.followMouseComboBox)
        self.values[16]=self.__tButToBox__(self.quickComboBox)
        self.values[17]=self.workdirEntry.text()
        self.values[18]=self.__tButToBox__(self.onTheFlyComboBox)
        self.values[19]=self.__tButToBox__(self.zeroCmpComboBox)
        if self.overwriteFilesButton.checkState()==QtCore.Qt.Checked:
            self.values[20]=True
        else:
            self.values[20]=False
        self.values[21]=self.__tButToBox__(self.winDecoComboBox)
        if self.jack_button.checkState()==QtCore.Qt.Checked:
            self.values[22]=True
        else:
            self.values[22]=False
        self.__getSelectedPorts__()
        self.values[24]=self.__tButToBox__(self.tooltipsComboBox)
        self.values[25]=self.__tButToBox__(self.rFrameComboBox)
        self.values[26]=str(self.extraOptsEntry.text().trimmed())
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
        self.baseLayout=QtGui.QVBoxLayout(self.window)
        self.baseLayout.setSpacing(0)
        try:
            self.baseLayout.setContentsMargins(0,9,0,0)
        except:
            self.baseLayout.setMargin(0)
        self.window.setLayout(self.baseLayout)

        for i in range(4):
            self.tabWidgets[i]=QtGui.QWidget(self.notebook)
            self.labelbox[i]=QtGui.QVBoxLayout(self.tabWidgets[i])
#Files page
        for i in range(2):
            self.labels[i]=QtGui.QLabel(prefLabelStrings[i],self.tabWidgets[0])
            self.labels[i].setAlignment(QtCore.Qt.AlignLeft)
            self.boxes[i]=QtGui.QHBoxLayout()
            self.boxes[i].insertWidget(-1,self.labels[i])
            self.boxes[i].insertStretch(-1,0)
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
            self.labels[i]=QtGui.QLabel(prefLabelStrings[i],self.tabWidgets[1])
            self.labels[i].setAlignment(QtCore.Qt.AlignLeft)
            self.boxes[i]=QtGui.QHBoxLayout()
            self.boxes[i].insertWidget(-1,self.labels[i])
            self.boxes[i].insertStretch(-1,0)
            self.labelbox[1].insertLayout(-1,self.boxes[i])
            self.labelbox[1].insertSpacing(-1,20)
        self.labelbox[1].setSizeConstraint(QtGui.QLayout.SetMaximumSize)

        self.fpsSpinButton= QtGui.QSpinBox(self.tabWidgets[1])
        self.fpsSpinButton.setRange(1,50)
        self.fpsSpinButton.setValue(self.values[0])
        self.boxes[2].insertWidget(-1,self.fpsSpinButton)

        self.onTheFlyComboBox = QtGui.QCheckBox(self.tabWidgets[1])
        self.onTheFlyComboBox.setCheckState(self.__rCheckState__(self.values[18]))
        self.boxes[3].insertWidget(-1,self.onTheFlyComboBox)

        self.zeroCmpComboBox = QtGui.QCheckBox(self.tabWidgets[1])
        self.zeroCmpComboBox.setCheckState(self.__rCheckState__(self.values[19]))
        self.boxes[4].insertWidget(-1,self.zeroCmpComboBox)

        self.quickComboBox = QtGui.QCheckBox(self.tabWidgets[1])
        self.quickComboBox.setCheckState(self.__rCheckState__(self.values[16]))
        self.boxes[5].insertWidget(-1,self.quickComboBox)

        self.fullComboBox = QtGui.QCheckBox(self.tabWidgets[1])
        self.fullComboBox.setCheckState(self.__rCheckState__(self.values[3]))
        self.boxes[6].insertWidget(-1,self.fullComboBox)

#sound page
        for i in xrange(7,10):
            self.labels[i]=QtGui.QLabel(prefLabelStrings[i],self.tabWidgets[2])
            self.labels[i].setAlignment(QtCore.Qt.AlignLeft)
            self.boxes[i]=QtGui.QHBoxLayout()
            self.boxes[i].insertWidget(-1,self.labels[i])
            self.boxes[i].insertStretch(-1,0)
            self.labelbox[2].insertLayout(-1,self.boxes[i])
            self.labelbox[2].insertSpacing(-1,20)
        self.labelbox[2].setSizeConstraint(QtGui.QLayout.SetMaximumSize)

        self.labelbox[2].insertSpacing(-1,15)
        self.jack_button= QtGui.QCheckBox(prefStrings['UseJack'],self.tabWidgets[2])
        self.jack_lsp_label=QtGui.QLabel(prefStrings['AvailablePortsAdd'],self.tabWidgets[2])
        self.jack_lss_label=QtGui.QLabel(prefStrings['SelectedPortsRemove'],self.tabWidgets[2])
        self.jack_lsp_liststore=QtGui.QListWidget(self.tabWidgets[2])
        self.jack_lsp_liststore.setFixedHeight(100)
        self.jack_lsp_listSave=QtGui.QListWidget(self.tabWidgets[2])
        self.jack_lsp_listSave.setFixedHeight(100)


        self.jack_lsp_refresh=QtGui.QPushButton(prefStrings['Refresh'],self.tabWidgets[2])
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
        self.freqSpinButton.setRange(0,48000)
        self.freqSpinButton.setSingleStep(100)
        self.freqSpinButton.setValue(self.values[8])
        self.boxes[8].insertWidget(-1,self.freqSpinButton)

        self.deviceEntry= QtGui.QLineEdit(self.values[9],self.tabWidgets[2])
        self.boxes[9].insertWidget(-1,self.deviceEntry)

#misc page
        for i in xrange(10,19):
            self.labels[i]=QtGui.QLabel(prefLabelStrings[i],self.tabWidgets[3])
            self.labels[i].setAlignment(QtCore.Qt.AlignLeft)
            self.boxes[i]=QtGui.QHBoxLayout()
            self.boxes[i].insertWidget(-1,self.labels[i])
            self.boxes[i].insertStretch(-1,0)
            self.labelbox[3].insertLayout(-1,self.boxes[i])
            self.labelbox[3].insertSpacing(-1,20)
        self.labelbox[3].setSizeConstraint(QtGui.QLayout.SetMaximumSize)

        self.displayEntry= QtGui.QLineEdit(self.values[12])
        self.boxes[10].insertWidget(-1,self.displayEntry)

        self.mouseComboBox = QtGui.QComboBox(self.tabWidgets[3])
        for i in range(4):
            self.mouseComboBox.addItem(prefMouseStrings[i])
        self.mouseComboBox.setCurrentIndex(self.values[1])
        self.boxes[11].insertWidget(-1,self.mouseComboBox)

        self.followMouseComboBox = QtGui.QCheckBox(self.tabWidgets[3])
        self.followMouseComboBox.setCheckState(self.__rCheckState__(self.values[15]))
        self.boxes[12].insertWidget(-1,self.followMouseComboBox)

        self.sharedComboBox = QtGui.QCheckBox(self.tabWidgets[3])
        self.sharedComboBox.setCheckState(self.__rCheckState__(self.values[13]))
        self.boxes[13].insertWidget(-1,self.sharedComboBox)

        self.winDecoComboBox = QtGui.QCheckBox(self.tabWidgets[3])
        self.winDecoComboBox.setCheckState(self.__rCheckState__(self.values[21]))
        self.boxes[14].insertWidget(-1,self.winDecoComboBox)

        self.tooltipsComboBox = QtGui.QCheckBox(self.tabWidgets[3])
        self.tooltipsComboBox.setCheckState(self.__rCheckState__(self.values[24]))
        self.boxes[15].insertWidget(-1,self.tooltipsComboBox)


        self.rFrameComboBox = QtGui.QCheckBox(self.tabWidgets[3])
        self.rFrameComboBox.setCheckState(self.__rCheckState__(self.values[25]))
        self.boxes[16].insertWidget(-1,self.rFrameComboBox)

        self.areaResetComboBox = QtGui.QCheckBox(self.tabWidgets[3])
        self.areaResetComboBox.setCheckState(self.__rCheckState__(self.values[14]))
        self.boxes[17].insertWidget(-1,self.areaResetComboBox)

        self.extraOptsEntry= QtGui.QLineEdit(self.values[26])
        self.boxes[18].insertWidget(-1,self.extraOptsEntry)

        for i in range(4):
            self.tabWidgets[i].setLayout(self.labelbox[i])
            self.notebook.insertTab(-1,self.tabWidgets[i],prefTabStrings[i])

        self.baseLayout.addWidget(self.notebook)
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
            self.ports.append(prefStrings['JackLspS1'])
            self.ports.append(prefStrings['JackLspS2'])
            self.jack_lsp_liststore.setEnabled(False)
            self.jack_lsp_listSave.setEnabled(False)
            self.values[23]=[]
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
        if(val):
            self.__runJackLSP__()

    def __init__(self,values,optionsOpen):
        self.values=values
        self.ports=[]
        self.optionsOpen=optionsOpen
        self.window = QtGui.QWidget()
        self.window.closeEvent= self.destroy

        self.window.setWindowTitle("recordMyDesktop: "+smplButtonStrings[0])
        self.__subWidgets__()
        self.__makeCons__()
        self.__runJackLSP__()
        self.__jack_enabled_check__()
        for i in self.values[23]:
            self.jack_lsp_listSave.insertItem(-1,QtGui.QListWidgetItem(i))


        if self.values[24]==0:
            self.__tooltips__()
        self.window.show()










