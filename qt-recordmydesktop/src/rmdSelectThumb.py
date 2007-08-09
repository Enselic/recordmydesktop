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
import sys
import rmdConfig
from rmdFrame import *

class QtThumbSelector(QtGui.QLabel):
    def __init__(self,parent,caller,area_return,hidden,update_interval=500):
        self.area_return=area_return
        self.selecting=0
        self.realFrame=None
        self.caller=caller
        self.hidden=hidden
        QtGui.QLabel.__init__(self,parent)
        self.x1=self.y1=-1
        self.x2 = self.y2 = -1
        self.root=QtGui.QApplication.desktop().winId()
        self.pix=QtGui.QPixmap.grabWindow(self.root)
        siz=QtGui.QApplication.desktop().screenGeometry()
        (self.wwidth, self.wheight)=(siz.width(),siz.height())

        self.factor=1;
        twidth=self.wwidth
        while twidth>320 or self.factor<4:
            twidth/=2
            self.factor*=2
        self.setFixedSize(self.wwidth/self.factor,self.wheight/self.factor)
        self.setSizePolicy(QtGui.QSizePolicy.Preferred,QtGui.QSizePolicy.Preferred)
        self.timed_id=QtCore.QTimer(None)
        self.timed_id.connect(self.timed_id,QtCore.SIGNAL("timeout()"),
                                self.update_image)
        self.timed_id.start(update_interval)
        self.capPos=0
        self.pixt=None
        self.pixflag=0
        self.fragSize=64
    def update_image(self):
        if self.selecting==0 and self.hidden[0]==0:
            self.pixt=QtGui.QPixmap.grabWindow(self.root,0,self.capPos,self.wwidth,self.fragSize)
            self.pixflag=1
            self.update()

    def update(self):
        QtGui.QLabel.update(self)
        if self.realFrame!=None:
            if (((self.area_return[0]!=self.realFrame.x or
               self.area_return[1]!=self.realFrame.y) and
               self.caller.values[15]==1) or
               self.area_return[2]-self.area_return[0]!=self.realFrame.w or
               self.area_return[3]-self.area_return[1]!=self.realFrame.h or
               self.caller.values[25]):
               self.realFrame.destroy()
               self.realFrame=None
        if self.realFrame==None and self.caller.values[25]==0 :
            self.__draw_frame__()

    def paintEvent(self, event):

        if self.pixt!=None and self.pixflag==1:
            self.pixflag=0
            painter2 = QtGui.QPainter(self.pix)
            painter2.drawPixmap(0,self.capPos,self.wwidth,self.fragSize,self.pixt)
            if self.capPos<self.wheight:
                self.capPos+=self.fragSize
                if self.capPos>self.wheight:
                    self.capPos=self.wheight-self.fragSize
            else:
                self.capPos=0

        painter = QtGui.QPainter(self)
        painter.drawPixmap(0,0,self.wwidth/self.factor,self.wheight/self.factor,self.pix)
        if self.selecting==1 or  self.caller.values[15]==1:
            if self.x1 >= 0:
                x = min(self.x1, self.x2)
                y = min(self.y1, self.y2)
                w = max(self.x1, self.x2) - x
                h = max(self.y1, self.y2) - y
                painter.fillRect(x,y,w,h,QtGui.QBrush(QtCore.Qt.red,QtCore.Qt.CrossPattern))
            elif self.area_return[0]>=0 or self.area_return[1]>=0 or self.area_return[2]>=0 or self.area_return[3]>=0:
                x = max(0, self.area_return[0])/self.factor
                y = max(0, self.area_return[1])/self.factor
                w = min(self.wwidth, self.area_return[2])/self.factor - x
                h = min(self.wheight, self.area_return[3])/self.factor - y
                painter.fillRect(x,y,w,h,QtGui.QBrush(QtCore.Qt.red,QtCore.Qt.CrossPattern))
    def mouseMoveEvent(self, event):
        if self.x1 >= 0:
            rect = QtCore.QRect()
            rect.x = min(self.x1, min(self.x2, event.x() + 1))
            rect.width = max(self.x1, max(self.x2, event.x() + 1)) - rect.x
            rect.y = min(self.y1, min(self.y2, event.y() + 1))
            rect.height = max(self.y1, max(self.y2, event.y() + 1)) - rect.y

        self.x2 = event.x() + 1
        self.y2 = event.y() + 1
        self.update()
    def mouseReleaseEvent(self, event):
        if event.button() == 1:
            self.selecting=0
            if self.x1 >= 0:
                self.x2 = event.x() + 1
                self.y2 = event.y() + 1
                self.area_return[0]=max(0,min(int(self.x1),int(self.x2))*self.factor)
                self.area_return[1]=max(0,min(int(self.y1),int(self.y2))*self.factor)
                self.area_return[2]=min(self.wwidth,max(int(self.x1),int(self.x2))*self.factor)
                self.area_return[3]=min(self.wheight,max(int(self.y1),int(self.y2))*self.factor)
            self.update()
    def mousePressEvent(self, event):
        if event.button() != 1:
            self.x1 = self.y1 =-1
            self.area_return[0]=-1
            self.area_return[1]=-1
            self.area_return[2]=-1
            self.area_return[3]=-1
            self.update()
        elif event.button() == 1:
            self.selecting=1
            self.x1 = event.x()
            self.y1 = event.y()
    def __draw_frame__(self):
        if self.area_return[0]!=-1:
            self.realFrame=rmdFrame(self.area_return[0],
                                    self.area_return[1],
                                    self.area_return[2]-self.area_return[0],
                                    self.area_return[3]-self.area_return[1],
                                    self.caller)
    def closeEvent(self,event):
        self.realFrame.destroy()
        QtGui.QLabel.close(self)

if __name__ == "__main__":
    import sys
    p=[[-1,-1,-1,-1]]
    hidden=[0]
    app=QtGui.QApplication(sys.argv)
    app.values=rmdConfig.default_values
    w = QtThumbSelector(None,app,p[0],hidden)
    w.show()
    app.exec_()
    print p[0]



