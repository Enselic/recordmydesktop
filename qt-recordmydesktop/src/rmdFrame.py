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

class rmdFrame:
    borderwidth=6
    outlinewidth=1

    def __init__(self,x,y,w,h):
        self.window=QtGui.QWidget(None,QtCore.Qt.X11BypassWindowManagerHint|QtCore.Qt.WindowStaysOnTopHint)
        self.x=x
        self.y=y
        self.w=w
        self.h=h

        self.mask = QtGui.QBitmap(self.w+self.borderwidth*2,
                             self.h+self.borderwidth*2)
        self.mask.fill(QtCore.Qt.color1)

        self.window.paintEvent=self.paintEvent
        self.window.setWindowTitle("qt-recordMyDesktop frame")

        self.window.setFixedSize(self.w+self.borderwidth*2,
                                     self.h+self.borderwidth*2)

        self.window.move(self.x-self.borderwidth,
                         self.y-self.borderwidth)
        self.window.show()

    def paintEvent(self,Event):

        painterw= QtGui.QPainter(self.window)
        painterw.fillRect(0,0,self.w+self.borderwidth*2,
                         self.h+self.borderwidth*2,
                         QtGui.QBrush(QtCore.Qt.white,
                         QtCore.Qt.SolidPattern))
        painterw.fillRect(self.outlinewidth,
                         self.outlinewidth,
                         self.w+(self.borderwidth-
                            self.outlinewidth)*2,
                         self.h+(self.borderwidth-
                            self.outlinewidth)*2,
                         QtGui.QBrush(QtCore.Qt.black,
                         QtCore.Qt.SolidPattern))
        painterw.fillRect(self.borderwidth-self.outlinewidth,
                         self.borderwidth-self.outlinewidth,
                         self.w+self.outlinewidth*2,
                         self.h+self.outlinewidth*2,
                         QtGui.QBrush(QtCore.Qt.white,
                         QtCore.Qt.SolidPattern))
        if self.window.mask().isEmpty():
            painter = QtGui.QPainter(self.mask)
            painter.fillRect(self.borderwidth,
                            self.borderwidth,
                            self.w,self.h,
                            QtGui.QBrush(QtCore.Qt.color0,
                            QtCore.Qt.SolidPattern))
            self.window.setMask(self.mask)
    def pass_e(self):
        pass

    def destroy(self):
        #self.area.destroy()
        try:
            self.window.close()
        except:
            pass

if __name__ == "__main__":
    import sys

    app=QtGui.QApplication(sys.argv)
    app.values=[[-1,-1,-1,-1]]
    w = rmdFrame(100,100,340,260)
    app.exec_()
