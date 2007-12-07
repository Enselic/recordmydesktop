from PyQt4 import QtGui,QtCore
import rmdConfig
import locale, gettext
def _(s):
    return QtCore.QString.fromUtf8(gettext.gettext(s))

gettext.textdomain('qt-recordMyDesktop')
gettext.bindtextdomain('qt-recordMyDesktop',rmdConfig.locale_install_dir)

TpmStr={'ShowHide':_("Show/hide Main Window"),
        'SelectOnScreen':_("_Select Area On Screen"),
        'Quit':_("Quit"),
        'ProgDesc':_('a graphical frontend for recordMyDesktop'),
        'Copyright': _('Copyright (C) John Varouhakis\nPortions Copyright (C) 2005-6 Zaheer Abbas Merali, John N. Laliberte\nPortions Copyright (C) Fluendo S.L.'),
        'TranslatorCredits': _('Translator Credits And Information(Replace with your info)')
    }

rmdErrStr={
        0:_('Success'),
        1*256:_('Error while parsing the arguments.'),
        2*256:_('Initializing the encoder failed(either vorbis or theora)'),
        3*256:_('Could not open/configure sound card.'),
        4*256:_('Xdamage extension not present.'),
        5*256:_('Shared memory extension not present.'),
        6*256:_('Xfixes extension not present.'),
        7*256:_('XInitThreads failed.'),
        8*256:_('No $DISPLAY environment variable and none specified as argument.'),
        9*256:_('Cannot connect to Xserver.'),
        10*256:_('Color depth is not 24bpp.'),
        11*256:_('Improper window specification.'),
        12*256:_('Cannot attach shared memory to proccess.'),
        13*256:_('Cannot open file for writting.'),
        14*256:_('Cannot load the Jack library (dlopen/dlsym error on libjack.so).'),
        15*256:_('Cannot create new client.'),
        16*256:_('Cannot activate client.'),
        17*256:_('Port registration/connection failure.'),
        11:_('Segmentation Fault')
    }
TrayIconStr={
        'RecFinishedKnown':_("Recording is finished.\nrecordMyDesktop has exited with status"),
        'ErrDesc':_("Description"),
        'RecFinishedUnknown':_("Recording is finished.\nrecordMyDesktop has exited with uknown\nerror code")
   }


smplLabelStrings=[_('Video Quality'),_('Sound Quality')]
smplButtonStrings=[_('Advanced'),_('Select Window')]
smplTooltipLabels=[_('Click here to select a window to record'),
                _('Click to start the recording.\nThis window will hide itself.'),
                _('Click to choose a filename and location.\nDefault is out.ogv in your home folder.\nIf the file already exists, the new one\nwill have a number attached on its name\n(this behavior can be changed )'),
                _('Click to exit the program.'),
                _('Select the video quality of your recording.\n(Lower quality will require more proccessing power,\nso it\'s recommended, when encoding on the fly,\nto leave at 100)'),
                _('Enable/Disable sound recording.'),
                _('Select the audio quality of your recording.'),
                _('Click here to access more options.')]
smplTipLabelStrings=[_('\nLeft click and drag, on the preview image,\nto select an area for recording.\nRight click on it, to reset the area.')]

smplStrings={   'Record':_("Record"),
                'SaveAs':_("Save As"),
                'Quit':_("Quit")
    }

monStrings={'Cancel':_("Cancel"),
            'PleaseWait':_("Please wait while your recording is being encoded\nWARNING!!!\nIf you press Cancel or close this window,\nthis proccess cannot be resumed!"),
            'complete':_("complete")
    }

prefLabelStrings=[_('Overwite Existing Files'),_('Working Directory'),
                _('Frames Per Second'),_('Encode On the Fly'),_('Zero Compression'),
                _('Quick Subsampling'),_('Full shots at every frame'),
                _('Channels'),_('Frequency'),_('Device'),_('Display'),_('Mouse Cursor'),
                _('Follow Mouse'),_('MIT-Shm extension'),_('Include Window Decorations'),
                _('Tooltips'),_('Outline Capture Area On Screen'),
                _('Reset Capture Area'),
                _('Extra Options'),
                _('Video Quality'),_('Sound Quality'),
                _('Drop Frames(encoder)'),_('Startup Delay(secs)'),]
prefMouseStrings=[_('Normal'),_('White'),_('Black'),_('None')]
prefStateStrings=[_('Enabled'),_('Disabled')]#0,1
prefTabStrings=[_('Files'),_('Performance'),_('Sound'),_('Misc')]
prefTooltipLabels=[_('By default, if you try to save under a filename that already\nexists, the new file will be renamed\n(for example if you try to save as out.ogv and that file exists, your file will be named out.ogv.1).\nBy checking this box, this behavior is disabled and the old file is overwritten.\n'),
                _('Directory where temporary files will be saved.'),
                _('Frames Per Second'),
                _('Encode simultaneously with the recording.\nThis will require a lot more processing power.\n'),
                _('Do not apply compression on the temporary files.\nIf enabled, the program will need less processing power,\nin the expense of hard disk space.\nThis option is valid only when encoding on the fly is disabled.'),
                _('This option, if enabled, will cause the program to require less\nprocessing power, but it might make the colors look a bit more blurry.'),
                _('This option should be enabled when recording 3d windows.\nIt is also auto-enabled when recording a\nknown 3d compositing window manager.\n '),
                _('Number of audio channels.'),
                _('Sound frequency.'),
                _('ALSA sound device, that is used for sound recording.'),
                _('Connection to the Xserver.'),
                _('The mouse cursor that will be drawn.\nNormal is for the real cursor that you see while recording,\nwhile black and white are fake cursors, drawn by the program.\nYou can also disable cursor drawing.'),
                _('When This option is enabled, the capture\narea will follow your mouse cursor.\nEnabling this option will also enable full shots at\nevery frame, so it\'s not recommended\nfor large capture areas.'),
                _('Use the MIT-Shared memory extension, whenever appropriate,\n depending on the rest of the program settings.\nDisabling this option is not recommended,\nas it may severely slow down the program.'),
                _('When selecting a window via the "Select Window" button,\ninclude that window\'s decorations in the recording area.'),
                _('Enable or disable tooltips, like this one.\n(Requires restart)'),
                _('Draw a frame on the screen, around the area\nthat will get captured.\n(This frame will remain throughout the recording.)'),
                _('Reset capture area after every recording\nand at program shutdown.'),
                _('Extra commandline options that will get\npassed to recordMyDesktop during initialization.\nThese options must be entered the same way\nthey would in the commandline (i.e. separated with spaces).\nConsult the manpage for more information on using them.')]
prefJacktip=_("Enable this option to record audio through\nJACK. The Jack server must be running in order to\nobtain the ports that will be recorded.\nThe audio recorded from each one\nwill be written on a channel of its own.\nrecordMyDesktop must be compiled with JACK\nsupport for this option to work.")

prefStrings={   'UseJack':_("Use Jack for audio capture."),
                'SelectPorts':_("Select the ports you want to record from\n"
                                        "(hold Ctrl to select multiple entries):"),
                'AvailablePorts':_("Available Ports"),
                'JackLspS1':_("jack_lsp returned no ports."),
                'JackLspS2':_("Make sure that jackd is running."),
                'AvailablePortsAdd':_("Available ports (double click to add):"),
                'SelectedPortsRemove':_("Selected ports (double click to remove):"),
                'Refresh':_("Refresh")
}

