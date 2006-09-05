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
import os




class prefsWidget(object):
    labelStrings=['Frames Per Second','Mouse Cursor','Sound Recording','Full Shots At Every Frame']
    mouseStrings=['White','Black','None']
    stateStrings=['On','Off']

    
    def destroy(self,Event=None):
        #gtk.main_quit()
        self.values[0]=self.fpsSpinButton.get_value_as_int()
        self.values[1]=self.mouseComboBox.get_active()
        self.values[2]=self.soundComboBox.get_active()
        self.values[3]=self.fullComboBox.get_active()
        self.values[4]=self.pathEntry.get_text()
        self.window.destroy()
        self.optionsOpen[0]=0

    def __exit__(self,Event=None):
        gtk.main_quit()
        self.values[0]=-1
        self.optionsOpen[0]=0
        self.window.destroy()
        
    def __fileSelQuit__(self,Event=None):
        self.fileSel.destroy()
        
    def __fileSelOk__(self,Event=None):
        self.pathEntry.set_text(self.fileSel.get_filename())
        self.fileSel.destroy()

    def __fileSelect__(self,Event=None):
        self.fileSel = gtk.FileSelection(title=None)
        self.fileSel.ok_button.connect("clicked", self.__fileSelOk__) 
        self.fileSel.cancel_button.connect("clicked", self.__fileSelQuit__)
        self.fileSel.set_filename(self.values[4])
        self.fileSel.show() 
       
    def __subWidgets__(self):
        self.labels={}
        self.boxes={}
        self.labelbox=gtk.VBox(homogeneous=True, spacing=10)
        

        self.fpsAdjustment=gtk.Adjustment(value=self.values[0], lower=1, upper=50, step_incr=1, page_incr=5, page_size=0)
        self.fpsSpinButton= gtk.SpinButton(self.fpsAdjustment, climb_rate=0.5, digits=0)
        self.mouseComboBox = gtk.combo_box_new_text()
        for i in range(3):
            self.mouseComboBox.append_text(self.mouseStrings[i])
        self.mouseComboBox.set_active(self.values[1])

        self.soundComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.soundComboBox.append_text(self.stateStrings[i])
        self.soundComboBox.set_active(self.values[2])
        
        self.fullComboBox = gtk.combo_box_new_text()
        for i in range(2):
            self.fullComboBox.append_text(self.stateStrings[i])
        self.fullComboBox.set_active(self.values[3])


        self.pathEntry= gtk.Entry(max=0)
        self.pathEntry.set_text(self.values[4])
        self.pathButton = gtk.Button(None,gtk.STOCK_SAVE_AS)
        
        self.okButton=gtk.Button(None,gtk.STOCK_OK)
        self.quitButton=gtk.Button(None,gtk.STOCK_QUIT)

        
        for i in range(4):
            self.labels[i]=gtk.Label(self.labelStrings[i])
            self.labels[i].set_justify(gtk.JUSTIFY_LEFT)
            self.boxes[i]=gtk.HBox(homogeneous=False, spacing=0)
            self.boxes[i].pack_start(self.labels[i],expand=False,fill=False)
            self.labels[i].show()
            self.labelbox.pack_start(self.boxes[i])

        placeholder1=gtk.Label("")
        placeholder1.show()
        self.labelbox.pack_start(placeholder1)
        self.boxes[4]=gtk.HBox(homogeneous=False, spacing=20)
        self.labelbox.pack_start(self.boxes[4])                       
        placeholder2=gtk.Label("")
        placeholder2.show()
        self.labelbox.pack_start(placeholder2)
        self.boxes[5]=gtk.HBox(homogeneous=False, spacing=0)
        self.labelbox.pack_end(self.boxes[5])
    

    
        self.fpsSpinButton.show()
        self.boxes[0].pack_end(self.fpsSpinButton,expand=False,fill=False)
        self.mouseComboBox.show()
        self.boxes[1].pack_end(self.mouseComboBox,expand=False,fill=False)
        self.soundComboBox.show()
        self.boxes[2].pack_end(self.soundComboBox,expand=False,fill=False)
        self.fullComboBox.show()
        self.boxes[3].pack_end(self.fullComboBox,expand=False,fill=False)
        self.pathEntry.show()
        self.boxes[4].pack_start(self.pathEntry,expand=False,fill=False)
        self.pathButton.show()
        self.boxes[4].pack_end(self.pathButton,expand=False,fill=False)
       
        self.okButton.show()
        self.quitButton.show()
        self.boxes[5].pack_start(self.okButton,expand=False,fill=False)
        self.boxes[5].pack_end(self.quitButton,expand=False,fill=False)

        for i in range(6):
            self.boxes[i].show()
            
        self.window.add(self.labelbox)
        self.labelbox.show()


    def __makeCons__(self):
        self.pathButton.connect("clicked",self.__fileSelect__)
        self.okButton.connect("clicked",self.destroy)
        self.quitButton.connect("clicked",self.__exit__)

    def __init__(self,values,optionsOpen):
        self.values=values
        self.optionsOpen=optionsOpen
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", self.destroy)
        self.window.set_border_width(10)
        self.window.set_title("recordMyDesktop")
        self.__subWidgets__()
        self.__makeCons__()
        
        
        self.window.set_size_request(288,384)
        self.window.show()
        #self.values=values 
        
    def main(self):
        gtk.main()



#values=[15,0,0,1,os.path.join(os.getenv('HOME'),'out.ogg')]
#ps=prefsWidget(values)
#ps.main()
        
#print values





