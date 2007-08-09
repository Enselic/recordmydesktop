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
import gobject

class rmdFrame:
    borderwidth=6
    outlinewidth=1

    def __init__(self,x,y,w,h,parent):
        self.window=gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.area=gtk.DrawingArea()
        self.x=x
        self.y=y
        self.w=w
        self.h=h
        self.parent=parent
        mask = gtk.gdk.Pixmap(None,
                              self.w+self.borderwidth*2,
                              self.h+self.borderwidth*2,
                              1)
        gc = mask.new_gc()
        gc.foreground = gtk.gdk.Color(0,0,0,1)
        mask.draw_rectangle(gc,True,0,0,
                            self.w+self.borderwidth*2,
                            self.h+self.borderwidth*2)
        gc.foreground = gtk.gdk.Color(0, 0, 0, 0)
        mask.draw_rectangle(gc,True,
                            self.borderwidth,
                            self.borderwidth,
                            self.w,self.h)

        self.area.show()
        self.window.stick()
        self.window.set_keep_above(True)
        self.area.connect("expose-event", self.__expose_cb)
        self.window.shape_combine_mask(mask,0,0)
        self.window.set_type_hint(gtk.gdk.WINDOW_TYPE_HINT_DOCK)
        self.window.set_title("gtk-recordMyDesktop frame")
        self.window.stick()
        self.window.set_keep_above(True)
        self.window.add(self.area)
        self.window.show()

        self.window.set_size_request(self.w+self.borderwidth*2,
                                     self.h+self.borderwidth*2)
        self.window.move(self.x-self.borderwidth,
                         self.y-self.borderwidth)
        self.window.set_resizable(False)
        self.disp=gtk.gdk.display_get_default()
        self.wroot = gtk.gdk.get_default_root_window()
        (self.wwidth, self.wheight) = self.wroot.get_size()
        self.timed_id=gobject.timeout_add(10,self.moveFrame)

    def moveFrame(self):
        if self.parent.values[15]==0:
            npos=gtk.gdk.Display.get_pointer(self.disp)
            x=npos[1]-self.w/2
            y=npos[2]-self.h/2
            x=(x>>1)<<1
            y=(y>>1)<<1
            if x<0:x=0
            if y<0:y=0
            if x+self.w>self.wwidth:x=self.wwidth-self.w
            if y+self.h>self.wheight:y=self.wheight-self.h
            if(x!=self.x or y!= self.y):
                self.x=x
                self.y=y
                self.window.move(self.x-(self.borderwidth),self.y-(self.borderwidth))
        return True


    def __expose_cb(self, widget, event):
        if widget==self.area:
            self.area.window.draw_rectangle(self.window.style.white_gc,
                                            True,0,0,self.w+self.borderwidth*2,
                                            self.h+self.borderwidth*2)
            self.area.window.draw_rectangle(self.window.style.black_gc,
                                            True,self.outlinewidth,
                                            self.outlinewidth,
                                            self.w+(self.borderwidth-
                                                    self.outlinewidth)*2,
                                            self.h+(self.borderwidth-
                                                    self.outlinewidth)*2)
            self.area.window.draw_rectangle(self.window.style.white_gc,
                                            True,
                                            self.borderwidth-self.outlinewidth,
                                            self.borderwidth-self.outlinewidth,
                                            self.w+self.outlinewidth*2,
                                            self.h+self.outlinewidth*2)
        else:
            pass

    def destroy(self):
        self.area.destroy()
        try:
            self.window.destroy()
        except:
            pass
