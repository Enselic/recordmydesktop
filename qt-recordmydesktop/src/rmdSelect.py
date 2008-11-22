#/******************************************************************************
#*                       qt-recordMyDesktop                                    *
#*******************************************************************************
#*                                                                             *
#*           Copyright (C) 2007, 2008  John Varouhakis                         *
#*                                                                             *
#*                                                                             *
#*   This program is free software: you can redistribute it and/or modify      *
#*   it under the terms of the GNU General Public License as published by      *
#*   the Free Software Foundation, either version 3 of the License, or         *
#*   (at your option) any later version.                                       *
#*                                                                             *
#*   This program is distributed in the hope that it will be useful,           *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
#*   GNU General Public License for more details.                              *
#*                                                                             *
#*   You should have received a copy of the GNU General Public License         *
#*   along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
#*                                                                             *
#*                                                                             *
#*  For further information contact me at johnvarouhakis@gmail.com             *
#******************************************************************************/


from PyQt4 import QtGui,QtCore

class QtAreaSelector(QtGui.QLabel):
    def __init__(self,area_return):
        self.area_return=area_return
        QtGui.QLabel.__init__(self,None,QtCore.Qt.WindowStaysOnTopHint)
        self.x1=self.y1=-1
        self.x2=self.y2=0
        self.pix=QtGui.QPixmap.grabWindow(QtGui.QX11Info().appRootWindow())
        siz=QtGui.QApplication.desktop().screenGeometry()
        (self.width, self.height)=(siz.width(),siz.height())
    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        painter.drawPixmap(0,0,self.pix)
        if self.x1 >= 0:
            x = min(self.x1, self.x2)
            y = min(self.y1, self.y2)
            w = max(self.x1, self.x2) - x
            h = max(self.y1, self.y2) - y
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
        if event.button() == 1 and self.x1 >= 0:
            self.x2 = event.x() + 1
            self.y2 = event.y() + 1
            self.area_return[0]=min(int(self.x1),int(self.x2))
            self.area_return[1]=min(int(self.y1),int(self.y2))
            self.area_return[2]=max(int(self.x1),int(self.x2))
            self.area_return[3]=max(int(self.y1),int(self.y2))
            self.close()
    def mousePressEvent(self, event):
        if event.button() != 1:
            self.x1 = self.y1 = -1
            self.close()
        self.x1 = event.x()
        self.y1 = event.y()


if __name__ == "__main__":
    import sys
    p=[[-1,-1,-1,-1]]
    app=QtGui.QApplication(sys.argv)
    w = QtAreaSelector(p[0])
    w.showFullScreen()
    app.exec_()
    print p[0]


