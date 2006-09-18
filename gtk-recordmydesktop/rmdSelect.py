# -*- Mode: Python -*-
# vi:si:et:sw=4:sts=4:ts=4
#
# Istanbul - a desktop session recorder
# Copyright (C) 2005 Zaheer Abbas Merali (zaheerabbas at merali dot org)
# All rights reserved.

# This file may be distributed and/or modified under the terms of
# the GNU General Public License version 2 as published by
# the Free Software Foundation.
# This file is distributed without any warranty; without even the implied
# warranty of merchantability or fitness for a particular purpose.
# See "LICENSE.GPL" in the source distribution for more information.

# Headers in this file shall remain intact.

#This file has been modified to be used in gtk-recordMyDesktop
#by John Varouhakis
#area_return list has been added to retrieve the values
#original file name is select.py in Istanbul-0.2.1
import gtk
import gtk.gdk

class GtkAreaSelector(gtk.Window):
    def __init__(self,area_return):
        self.area_return=area_return
        gtk.Window.__init__(self)
        self.set_app_paintable(True)
        self.fullscreen()
        self.connect("expose-event", self.expose_cb)
        self.connect("button-press-event", self.button_press_cb)
        self.connect("button-release-event", self.button_release_cb)
        self.connect("motion-notify-event", self.motion_notify_cb)
        self.connect("delete-event", self.delete_cb)
        self.connect("realize", self.realize_cb)
        root = gtk.gdk.get_default_root_window()
        (width, height) = root.get_size()
        self.root = root.get_image(0, 0, width, height)
        self.x1 = self.y1 = -1
        self.x2 = self.y2 = 0
        self.show_all()
    
    def realize_cb(self, widget):
        gdkwindow = self.window
        gdkwindow.set_events(gdkwindow.get_events() | 
            gtk.gdk.BUTTON_PRESS_MASK | gtk.gdk.BUTTON_RELEASE_MASK | 
            gtk.gdk.POINTER_MOTION_MASK);
        cursor = gtk.gdk.Cursor(gtk.gdk.CROSSHAIR)
        gdkwindow.set_cursor(cursor)
        gdkwindow.set_back_pixmap(None, False)
        
    def expose_cb(self, widget, event):
        dashes = [ 1.0, 2.0 ]
        cr = self.window.cairo_create()
        cr.rectangle(event.area.x, event.area.y, event.area.width, 
            event.area.height)
        cr.clip()
        self.window.draw_image(self.style.black_gc, self.root, event.area.x,
            event.area.y, event.area.x, event.area.y, event.area.width,
            event.area.height)
        cr.set_line_width(1.0)
        cr.set_source_rgba (1.0, 0.0, 0.0, 1.0)
        cr.set_dash (dashes, 0.0)
        cr.move_to (self.x1 - 0.5, 0.0)
        cr.line_to (self.x1 - 0.5, event.area.y + event.area.height) 
        cr.move_to (0.0, self.y1 - 0.5)
        cr.line_to (event.area.x + event.area.width, self.y1 - 0.5)
        cr.stroke ()

        if self.x1 >= 0:
            x = min(self.x1, self.x2)
            y = min(self.y1, self.y2)
            w = max(self.x1, self.x2) - x
            h = max(self.y1, self.y2) - y
            cr.set_dash ([], 0.0)
            cr.rectangle ( x, y, w, h)
            cr.fill()
            cr.set_source_rgba (0.0, 0.0, 0.5, 0.5)
            cr.rectangle (x + 0.5, y + 0.5, w - 1, h - 1)
            cr.stroke()
        
        return False

    def motion_notify_cb(self, widget, event):
        self.queue_draw()
        
        if self.x1 >= 0:
            rect = gtk.gdk.Rectangle()
            rect.x = min(self.x1, min(self.x2, event.x + 1))
            rect.width = max(self.x1, max(self.x2, event.x + 1)) - rect.x
            rect.y = min(self.y1, min(self.y2, event.y + 1))
            rect.height = max(self.y1, max(self.y2, event.y + 1)) - rect.y
            self.queue_draw_area(rect.x, rect.y, rect.width, rect.height)

        self.x2 = event.x + 1
        self.y2 = event.y + 1
        
        return True

    def button_release_cb(self, widget, event):
        if event.button == 1 and self.x1 >= 0:
            self.x2 = event.x + 1
            self.y2 = event.y + 1
            # stop
            self.area_return[0]=int(self.x1)
            self.area_return[1]=int(self.y1)
            self.area_return[2]=int(self.x2)
            self.area_return[3]=int(self.y2)
            dialog = gtk.Dialog(title=None, parent=None, flags=0, buttons=None)
            label1 = gtk.Label("Recording area for the next session\n\t\t\tis set to:")
            dialog.vbox.pack_start(label1, True, True, 0)
            label1.show()
            label2 = gtk.Label("(%d,%d) - (%d,%d)\n"%(self.area_return[0],self.area_return[1],self.area_return[2],self.area_return[3]))
            dialog.vbox.pack_start(label2, True, True, 0)
            label2.show()
            button = gtk.Button(label=None, stock=gtk.STOCK_OK)
            dialog.vbox.pack_start(button, True, True, 0)
            def __destroy(button):
                dialog.destroy()
            button.connect("clicked", __destroy)
            button.show()
            dialog.set_size_request(300,128)
            dialog.show()
            self.destroy()
        
        return True
        
    def button_press_cb(self, widget, event):
        if event.button != 1:
            self.x1 = self.y1 = -1
            # stop
            #print "(%d, %d) - (%d, %d)" % (self.x1, self.y1, self.x2, self.y2)
            #self.area_return[0]=self.x1
            #self.area_return[1]=self.y1
            #self.area_return[2]=self.x2
            #self.area_return[3]=self.y2
            #print self.area_return
            self.destroy()
            return True
        self.x1 = event.x
        self.y1 = event.y
        self.queue_draw()
        return True

    def delete_cb(self, widget):
        pass

if __name__ == "__main__":
    w = GtkAreaSelector()
    w.show()
    gtk.main()
            
        
