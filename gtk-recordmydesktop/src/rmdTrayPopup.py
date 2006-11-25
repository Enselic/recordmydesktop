# -*- Mode: Python -*-
# vi:si:et:sw=4:sts=4:ts=4
#
# Istanbul - A desktop recorder
# Copyright (C) 2005 Zaheer Abbas Merali (zaheerabbas at merali dot org)
# Copyright (C) 2006 John N. Laliberte (allanonjl@gentoo.org) (jlaliberte@gmail.com)
# Portions Copyright (C) 2004,2005 Fluendo, S.L. (www.fluendo.com).
# All rights reserved.


# This file may be distributed and/or modified under the terms of
# the GNU General Public License version 2 as published by
# the Free Software Foundation.
# This file is distributed without any warranty; without even the implied
# warranty of merchantability or fitness for a particular purpose.
# See "COPYING" in the source distribution for more information.

#gtk-recordMyDesktop - a graphical frontend for recordMyDesktop
#Copyright (C) 2006 John Varouhakis
#This file has been modified to be used in gtk-recordMyDesktop
#original file name is tray_popup.py in istanbul-0.2.1


#25/11/2006: picked aboutdialog close-button fix from upstream Istanbul

import gtk
import locale, gettext
import rmdConfig
_ = gettext.gettext
gettext.textdomain('gtk-recordMyDesktop')
gettext.bindtextdomain('gtk-recordMyDesktop',rmdConfig.locale_install_dir)
import rmdSelect as iSel
import rmdPrefsWidget as pW

class TrayPopupMenu:
    options=None
    def __init__(self,parent,values,optionsOpen):
        self.parent=parent
        self.optionsOpen=optionsOpen
        self.values=values
        self._setup_popup_menu()
        self.selector = None

    def _setup_popup_menu(self):
        self.popupmenu = gtk.Menu()
        self._setup_about()
        self._setup_prefs_widget()
        self._setup_select_area()
        self._setup_quit()

    def _prefs_widget(self,button):
        if self.optionsOpen[0] ==0:
            self.optionsOpen[0]=1
            #self.options=pW.prefsWidget(self.values,self.optionsOpen)
            self.parent.show()
        else:
            #if self.options != None:
            self.parent.hide()
            self.optionsOpen[0]=0
    def _setup_prefs_widget(self):
        self.popupmenu_prefs_widget = gtk.ImageMenuItem(
               gtk.STOCK_PREFERENCES)
        self.popupmenu_prefs_widget.connect('activate', self._prefs_widget)
        self.popupmenu.add(self.popupmenu_prefs_widget)

    def _setup_about(self):
        self.popupmenu_aboutitem = gtk.ImageMenuItem(gtk.STOCK_ABOUT)
        self.popupmenu_aboutitem.connect('activate', self._about)
        self.popupmenu.add(self.popupmenu_aboutitem)

    def _about(self, button):
        aboutdialog = gtk.AboutDialog()
        aboutdialog.set_name(_('gtk-recordMyDesktop'))
        aboutdialog.set_version(rmdConfig.VERSION)
        aboutdialog.set_comments(_('a graphical frontend for recordMyDesktop'))
        aboutdialog.set_copyright(_('Copyright (C) John Varouhakis\nPortions Copyright (C) 2005-6 Zaheer Abbas Merali, John N. Laliberte\nPortions Copyright (C) Fluendo S.L.'))
        aboutdialog.set_authors(['recordMyDesktop and gtk-recordMyDesktop\nis written by John Varouhakis','Portions of gtk-recordMyDesktop\nare taken from Istanbul\n(http://live.gnome.org/Istanbul)\nwritten by\nZaheer Abbas Merali and John N. Laliberte'])
        aboutdialog.set_website('http://recordmydesktop.sourceforge.net')
        aboutdialog.set_license('GPL-2')
        aboutdialog.set_translator_credits(_('Translator Credits And Information(Replace with your info)'))
        aboutdialog.connect('response', lambda widget, response: widget.destroy())
        aboutdialog.show_all()

    def _setup_select_area(self):
        self.popupmenu_selectarea = gtk.ImageMenuItem(
            _("_Select Area On Screen"))
        self.popupmenu.add(self.popupmenu_selectarea)
        self.popupmenu_selectarea.connect("activate", self._select_area_cb)

    def _select_area_cb(self, menuitem):
        self.popupmenu.hide()
        self.selector = iSel.GtkAreaSelector(self.values[5])
        self.selector.show()

    def _setup_quit(self):
        self.popupmenu_quititem = gtk.ImageMenuItem(gtk.STOCK_QUIT)
        self.popupmenu_quititem.connect('activate', self._quit)
        self.popupmenu.add(self.popupmenu_quititem)

    def _quit(self, button):
        gtk.main_quit()

    def show(self):
        self.popupmenu.show_all()
	    self.popupmenu.popup(None, None, None, 3, gtk.get_current_event_time())

