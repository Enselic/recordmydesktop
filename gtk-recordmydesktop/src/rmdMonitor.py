import pygtk
pygtk.require('2.0')
import gtk
import gobject
import locale, gettext
import rmdConfig
_ = gettext.gettext
gettext.textdomain('gtk-recordMyDesktop')
gettext.bindtextdomain('gtk-recordMyDesktop',rmdConfig.locale_install_dir)
import popen2
import os,fcntl,signal
from rmdStrings import *

class rmdMonitor(object):
    labeString=monStrings['PleaseWait']
    counter_fraction=0.0

    def destroy_and_kill(self,Event=None):
        self.destroy()
        self.stop_encoding()
    def destroy(self,Event=None):
        gobject.source_remove(self.timed_id)
        self.window.destroy()
        gtk.main_quit()
    def update_counter(self):
        strstdout=""
        try:
            strstdout=self.stdout.read()
            if strstdout =='':
                self.destroy()
        except:
            return True
        try:
            self.counter_fraction=float(strstdout.replace("[","").replace("%] ",""))
            percentage=self.counter_fraction/100.0
            if percentage>1.0:
                percentage=1.0
            self.progressbar.set_fraction(percentage)
            self.progressbar.set_text("%.2f%% "%(self.counter_fraction)+monStrings['complete'])
        except:
            self.counter_fraction=0.0

        return True
    def stop_encoding(self,Event=None):
        os.kill(self.rmdPid,signal.SIGINT)

    def __init__(self,out_stream,childPid):
        flags = fcntl.fcntl(out_stream, fcntl.F_GETFL)
        fcntl.fcntl(out_stream, fcntl.F_SETFL, flags | os.O_NONBLOCK)
        self.rmdPid=childPid
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", self.destroy_and_kill)
        self.window.set_border_width(10)
        self.window.set_title("recordMyDesktop-encoder")
        self.label=gtk.Label(self.labeString)
        self.label.set_justify(gtk.JUSTIFY_CENTER)
        self.label.show()
        self.progressbar=gtk.ProgressBar(adjustment=None)
        self.progressbar.set_fraction(self.counter_fraction)
        self.progressbar.set_text("0% "+monStrings['complete'])
        self.progressbar.show()
        self.stopbutton=gtk.Button(None,gtk.STOCK_CANCEL)
        self.stopbutton.connect("clicked",self.stop_encoding)
        self.box=gtk.VBox(homogeneous=False, spacing=20)
        self.box.pack_start(self.label,True,True)
        self.box.pack_start(self.progressbar,True,True)
        self.box.pack_start(self.stopbutton,expand=False,fill=False)
        self.box.show()
        self.stopbutton.show()
        self.window.add(self.box)
        self.window.show()
        self.stdout=out_stream
        self.timed_id=gobject.timeout_add(100,self.update_counter)
        gtk.main()
