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
import os,sys

from rmdTrayIcon import *
import rmdPrefsWidget as pW
import rmdSelectThumb as sT

class simpleWidget(object):
    hidden=[0]
    labelStrings=[_('Video Quality'),_('Sound Quality')]
    buttonStrings=[_('Advanced'),_('Select Window')]
    tooltipLabels=[_('Click here to select a window to record'),
                   _('Click to start the recording.\nThis window will hide itself.'),
                   _('Click to choose a filename and location.\nDefault is out.ogg in your home folder.\nIf the file already exists, the new one\nwill have a number attached on its name\n(this behavior can be changed )'),
                   _('Click to exit the program.'),
                   _('Select the video quality of your recording.\n(Lower quality will require more proccessing power,\nso it\'s recommended, when encoding on the fly,\nto leave at 100)'),
                   _('Enable/Disable sound recording.'),
                   _('Select the audio quality of your recording.'),
                   _('Click here to access more options.')]
    tipLabelStrings=[_('\nLeft click and drag, on the preview image,\nto select an area for recording.\nRight click on it, to reset the area.')]

    options=None
    optionsOpen=[0]
    def __subWidgets__(self):
        self.labelbox=QtGui.QVBoxLayout()

        self.NBox=QtGui.QHBoxLayout()
        self.NEBox=QtGui.QVBoxLayout()
        self.NEVQBox=QtGui.QHBoxLayout()
        self.NESQBox=QtGui.QHBoxLayout()
        self.NETABox=QtGui.QVBoxLayout()
        self.NWBox=QtGui.QVBoxLayout()



        self.NBox.insertLayout(-1,self.NWBox)
        self.NBox.insertSpacing(-1,20)
        self.NBox.insertLayout(-1,self.NEBox)
        self.NEBox.insertLayout(-1,self.NEVQBox)
        self.NEBox.insertLayout(-1,self.NESQBox)
        self.NEBox.insertSpacing(-1,10)
        self.NEBox.insertLayout(-1,self.NETABox)

        self.SBox=QtGui.QHBoxLayout()
        self.SEBox=QtGui.QHBoxLayout()
        self.SWBox=QtGui.QHBoxLayout()
        self.SBox.insertLayout(-1,self.SWBox)
        self.SBox.insertStretch(-1,0)
        self.SBox.insertLayout(-1,self.SEBox)

        self.labelbox.insertLayout(-1,self.NBox)
        self.labelbox.insertLayout(-1,self.SBox)

        self.image=sT.QtThumbSelector(self.window,self,self.values[5],self.hidden)

        self.NWBox.insertWidget(-1,self.image)
        self.NWBox.setGeometry(QtCore.QRect(0,0,self.image.width(),self.image.height()))
        #quality
        self.v_quality=QtGui.QSlider(self.window)
        self.v_quality.setMaximum(100)
        self.v_quality.setMinimum(0)
        self.v_quality.setTracking(True)
        self.v_quality.setValue(int(round((float(self.values[10])*100.0)/63.0)))
        self.v_quality.setOrientation(QtCore.Qt.Horizontal)


        self.s_button= QtGui.QCheckBox(self.window)
        if self.values[2]:
            self.s_button.setCheckState(QtCore.Qt.Checked)
        else:
            self.s_button.setCheckState(QtCore.Qt.Unchecked)
        self.s_quality=QtGui.QSlider(self.window)
        self.s_quality.setMaximum(100)
        self.s_quality.setMinimum(0)
        self.s_quality.setTracking(True)
        self.s_quality.setValue(int(round(float(self.values[11])*10.0)))
        self.s_quality.setOrientation(QtCore.Qt.Horizontal)
        self.s_quality.setEnabled(self.values[2])

        self.v_label=QtGui.QLabel("         "+self.labelStrings[0],self.window)
        self.s_label=QtGui.QLabel(self.labelStrings[1],self.window)

        self.advanced_button=QtGui.QPushButton(self.buttonStrings[0],self.window)
        self.tipLabel=QtGui.QLabel(self.tipLabelStrings[0],self.window)

        self.NEVQBox.insertWidget(-1,self.v_label)
        self.NEVQBox.insertWidget(-1,self.v_quality)
        self.NESQBox.insertWidget(-1,self.s_button)
        self.NESQBox.insertWidget(-1,self.s_label)
        self.NESQBox.insertWidget(-1,self.s_quality)
        self.NETABox.insertWidget(-1,self.advanced_button)
        self.NETABox.insertWidget(-1,self.tipLabel)
        style=QtGui.QApplication.style()
        icon_rec_pxm=QtGui.QPixmap(os.path.join(rmdConfig.pixmapdir,"qt-recordmydesktop_record.png"))

        self.win_button=QtGui.QPushButton(self.buttonStrings[1],self.window)
        self.start_button=QtGui.QPushButton(QtGui.QIcon(icon_rec_pxm),_("Record"),self.window)
        self.file_button=QtGui.QPushButton(QtGui.QIcon(style.standardPixmap(QtGui.QStyle.SP_DialogSaveButton)),_("Save As"),self.window)
        self.quit_button=QtGui.QPushButton(QtGui.QIcon(style.standardPixmap(QtGui.QStyle.SP_DialogCloseButton)),_("Quit"),self.window)
        self.SWBox.insertWidget(-1,self.win_button)
        self.SWBox.insertWidget(-1,self.start_button)
        self.SEBox.insertWidget(-1,self.file_button)
        self.SEBox.insertWidget(-1,self.quit_button)


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

        self.window.setLayout(self.labelbox)
    def __tooltips__(self):
        self.win_button.setToolTip(htmlize(self.tooltipLabels[0]))
        self.start_button.setToolTip(htmlize(self.tooltipLabels[1]))
        self.file_button.setToolTip(htmlize(self.tooltipLabels[2]))
        self.quit_button.setToolTip(htmlize(self.tooltipLabels[3]))
        self.v_quality.setToolTip(htmlize(self.tooltipLabels[4]))
        self.v_label.setToolTip(htmlize(self.tooltipLabels[4]))
        self.s_button.setToolTip(htmlize(self.tooltipLabels[5]))
        self.s_quality.setToolTip(htmlize(self.tooltipLabels[6]))
        self.s_label.setToolTip(htmlize(self.tooltipLabels[6]))
        self.advanced_button.setToolTip(htmlize(self.tooltipLabels[7]))

    def __exit__(self,Event=None):
        if self.exited==0:
            self.update()
            self.exited=1
            self.optionsOpen[0]=0
            if self.save_prefs()==False:
                print "Warning!!!Couldn't save preferences"
            sys.exit()
    def hide(self,Event=None):
        self.window.hide()
        self.hidden[0]=1
    def show(self,Event=None):
        self.window.show()
        self.hidden[0]=0
    def __swap_visibility__(self,event=None):
        if event.type()==QtCore.QEvent.WindowStateChange:
            if self.window.windowState() == QtCore.Qt.WindowMinimized:
                self.hidden[0]=self.minimized=1
            else:
                self.hidden[0]=self.minimized=0

    def advanced(self,button=None):
        if self.optionsOpen[0] ==0:
            self.optionsOpen[0]=1
            self.options=pW.prefsWidget(self.values,self.optionsOpen)
        else:
            if self.options != None:
                self.options.window.close()
                self.optionsOpen[0]=0
    def close_advanced(self):
        if self.options != None:
            if self.optionsOpen[0] ==1:
                self.options.window.close()
                self.optionsOpen[0]=0

    def __fileSelect__(self,Event=None):
        tfilename=QtGui.QFileDialog.getSaveFileName(self.window,'',self.values[4],"*.ogg *.ogg.*;; *.*")
        if tfilename!= '':
            self.values[4]=tfilename
    def __select_window__(self,button=None):
        xwininfo_com=['xwininfo','-frame']
        if self.values[21]==1:
            xwininfo_com=['xwininfo']
        (stdin,stdout,stderr)=os.popen3(xwininfo_com,'t')
        wid=stdout.readlines()
        stdin.close()
        stdout.close()
        stderr.close()
        x=y=width=height=None
        for i in wid:
            if i.lstrip().startswith('Absolute upper-left X:'):
                x=int(i.split(' ')[len(i.split(' '))-1])
            elif i.lstrip().startswith('Absolute upper-left Y'):
                y=int(i.split(' ')[len(i.split(' '))-1])
            elif i.lstrip().startswith('Width:'):
                width=int(i.split(' ')[len(i.split(' '))-1])
            elif i.lstrip().startswith('Height:'):
                height=int(i.split(' ')[len(i.split(' '))-1])
        if x<0:
            width+=x
            x=0
        if y<0:
            height+=y
            y=0
        if width+x>self.wwidth: width=self.wwidth-x
        if height+y>self.wheight: height=self.wheight-y
        self.values[5][0]=x
        self.values[5][1]=y
        self.values[5][2]=width+x
        self.values[5][3]=height+y
        self.image.x1=-1
        self.image.update()

    def __makeCons__(self):
        self.advanced_button.connect(self.advanced_button,QtCore.SIGNAL("clicked()"),self.advanced)
        self.file_button.connect(self.file_button,QtCore.SIGNAL("clicked()"),self.__fileSelect__)
        self.quit_button.connect(self.quit_button,QtCore.SIGNAL("clicked()"),self.__exit__)
        self.start_button.connect(self.start_button,QtCore.SIGNAL("clicked()"),self.trayIcon.record_ext)
        self.s_button.connect(self.s_button,QtCore.SIGNAL("clicked()"),self.__sound_check__)
        self.win_button.connect(self.win_button,QtCore.SIGNAL("clicked()"),self.__select_window__)

    def __sound_check__(self,widget=None):
        self.s_quality.setEnabled(self.values[2])
        if self.s_button.checkState()==QtCore.Qt.Checked:
            self.values[2]=True
        else:
            self.values[2]=False

    def update(self)  :
        self.values[10]=int(round((float(self.v_quality.value())*63.0)/100.0))
        self.values[11]=int(round(float(self.s_quality.value())/10.0))
        if self.values[10]>63:
            self.values[10]=63
        if self.values[11]>10:
            self.values[11]=10
    def load_prefs(self):
        path=os.path.join(os.getenv("HOME"),".qt-recordmydesktop")
        try:
            self.values=[]
            savefile=open(path,"r")
            #the header
            savefile.readline()
            savefile.readline()
            #the options,each has a coment above
            for i in range(2):
                savefile.readline()
                self.values.append(int(savefile.readline()))
            savefile.readline()
            self.values.append(int(savefile.readline())==1)
            savefile.readline()
            self.values.append(int(savefile.readline()))
            savefile.readline()
            self.values.append(savefile.readline().replace("\n",""))
            #recording area is not saved
            self.values.append(rmdConfig.default_values[5])
            for i in range(3):
                savefile.readline()
                self.values.append(int(savefile.readline()))
            savefile.readline()
            self.values.append(savefile.readline().replace("\n",""))
            for i in range(2):
                savefile.readline()
                self.values.append(int(savefile.readline()))
            savefile.readline()
            self.values.append(savefile.readline().replace("\n",""))
            for i in range(4):
                savefile.readline()
                self.values.append(int(savefile.readline()))
            savefile.readline()
            self.values.append(savefile.readline().replace("\n",""))
            for i in range(2):
                savefile.readline()
                self.values.append(int(savefile.readline()))
            savefile.readline()
            self.values.append(int(savefile.readline())==1)
            #new options for 0.3.3
            p=savefile.readline()
            if p=='':
                self.values.append(rmdConfig.default_values[21])
            else:
                self.values.append(int(savefile.readline()))
            p=savefile.readline()
            if p=='':
                self.values.append(rmdConfig.default_values[22])
                self.values.append(rmdConfig.default_values[23])
            else:
                self.values.append(int(savefile.readline())==1)
            p=savefile.readline()
            if p=='':
                self.values.append(rmdConfig.default_values[23])
            else:
                ports_t=savefile.readline().split(' ')
                #get rid of the trailing \n
                ports_t.pop()
                self.values.append(ports_t)
            p=savefile.readline()
            if p=='':
                self.values.append(rmdConfig.default_values[24])
            else:
                self.values.append(int(savefile.readline()))
            p=savefile.readline()
            if p=='':
                self.values.append(rmdConfig.default_values[25])
            else:
                self.values.append(int(savefile.readline()))
            return True
        except:
            return False
    def save_prefs(self):
        try:
            path=os.path.join(os.getenv("HOME"),".qt-recordmydesktop")
            savefile=open(path,"w")
            savefile.write("#Order and overall number of lines must be preserved\n")
            savefile.write("#For this file to be read correctly\n")
            savefile.write("#Framerate\n")
            savefile.write("%d\n"%self.values[0])
            savefile.write("#Cursor(0 xfixes,1 white,2 black,3 none)\n")
            savefile.write("%d\n"%self.values[1])
            savefile.write("#Sound,0 disabled 1 enabled\n")
            savefile.write("%d\n"%self.values[2])
            savefile.write("#Full shots,1 disabled 0 enabled\n")
            savefile.write("%d\n"%self.values[3])
            savefile.write("#Filename\n")
            savefile.write("%s\n"%self.values[4])
            savefile.write("#Delay,seconds\n")
            savefile.write("%d\n"%self.values[6])
            savefile.write("#Number of channels\n")
            savefile.write("%d\n"%self.values[7])
            savefile.write("#Frequency\n")
            savefile.write("%d\n"%self.values[8])
            savefile.write("#Sound-device\n")
            savefile.write("%s\n"%self.values[9])
            savefile.write("#Video quality(0-63)\n")
            savefile.write("%d\n"%self.values[10])
            savefile.write("#Audio quality(0-10)\n")
            savefile.write("%d\n"%self.values[11])
            savefile.write("#Display\n")
            savefile.write("%s\n"%self.values[12])
            savefile.write("#Shared memory,1 disabled 0 enabled\n")
            savefile.write("%d\n"%self.values[13])
            savefile.write("#Drop frames(encoder option),1 disabled 0 enabled.Unused at 0.3.0, removed at 0.3.4, preserved for compatibility of prefs file.\n")
            savefile.write("%d\n"%self.values[14])
            savefile.write("#Shared memory threshold,0-100. Removed at 0.3.4,"
                           " preserved for compatibility of prefs file\n")
            savefile.write("%d\n"%self.values[15])
            savefile.write("#Quick subsampling,1 disabled 0 enabled\n")
            savefile.write("%d\n"%self.values[16])
            savefile.write("#Working directory(temporary files)\n")
            savefile.write("%s\n"%self.values[17])
            savefile.write("#On the fly encoding,1 disabled 0 enabled\n")
            savefile.write("%d\n"%self.values[18])
            savefile.write("#Zero compression,1 disabled 0 enabled\n")
            savefile.write("%d\n"%self.values[19])
            savefile.write("#overwrite existing files,0 disabled 1 enabled\n")
            savefile.write("%d\n"%self.values[20])
            savefile.write("#Include window decorations,1 disabled 0 enabled\n")
            savefile.write("%d\n"%self.values[21])
            savefile.write("#Use jack\n")
            savefile.write("%d\n"%self.values[22])
            savefile.write("#last used jack ports\n")
            for i in self.values[23]:
                savefile.write("%s "%i)
            savefile.write("\n")
            savefile.write("#Tooltips,1 disabled 0 enabled\n")
            savefile.write("%d\n"%self.values[24])
            savefile.write("#draw an on-screen frame, surrounding the capture area\n")
            savefile.write("%d\n"%self.values[25])
            savefile.flush()
            savefile.close()
            return True
        except:
            return False

    def __init__(self):
        if self.load_prefs()==False:
            self.values= rmdConfig.default_values
        self.optionsOpen=[0]
        self.minimized=0
        self.exited=0
        self.window = QtGui.QWidget()

        self.window.closeEvent= self.__exit__
        self.window.changeEvent=self.__swap_visibility__
        self.window.setWindowTitle("recordMyDesktop")

        siz=QtGui.QApplication.desktop().screenGeometry()
        (self.wwidth, self.wheight)=(siz.width(),siz.height())
        self.factor=1;
        twidth=self.wwidth
        while twidth>320 or self.factor<4:
            twidth/=2
            self.factor*=2

        self.__subWidgets__()

        self.trayIcon=trayIcon(self)
        self.__makeCons__()

        if self.values[24]==0:
            self.__tooltips__()

        self.window.show()



if __name__ == "__main__":
    import sys

    app=QtGui.QApplication(sys.argv)
    w = simpleWidget()
    app.exec_()








