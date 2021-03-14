# recordMyDesktop
Official repository for recordMyDesktop. Migrated from https://sourceforge.net/projects/recordmydesktop/ for which I am the official and long-time (but for a decade inactive) maintainer.

For the new web page with news, visit https://enselic.github.io/recordmydesktop/

# Development

First do
```
% git clone https://github.com/Enselic/recordmydesktop.git
```

## CLI

```
% cd recordmydesktop
% sudo apt install autotools-dev zlib1g-dev libice-dev libsm-dev libxext-dev libxdamage-dev libogg-dev libvorbis-dev libtheora-dev libjack-jackd2-dev libasound2-dev libpopt-dev
% cd recordmydesktop   # for CLI source code
% sh autogen.sh
% ./configure --prefix=/tmp/foo
% make
% make install
```

## GTK frontend
Ported to Python 3 and mostly to GTK 3, but some things remain. Search
in the code for
```
TODO: Port to GTK 3
```
which currently will give you the following list of things that needs to be
done:
* Port from "expose-event" to "draw" in three places.
* Port from Gdk.Image to cairo
* Port the USE_EGG code to GTK 3

You can however already now get a window with some controls to show, by
just doing
```
% cd gtk-recordmydesktop
% sudo apt install autotools-dev gettext python3-gi python3-gi-cairo gir1.2-gtk-3.0
% sh autogen.sh
% ./configure --prefix=/tmp/foo
% make
% make install
% /tmp/foo/bin/gtk-recordMyDesktop
```

## Qt frontend:
Ported to Python 3, but not from Qt4 to Qt5 yet. Help with migrating would be greatly appreciated.
```
% cd qt-recordmydesktop
% sudo apt install autotools-dev gettext # TODO: And some Qt5 libs
% sh autogen.sh
% ./configure --prefix=/tmp/foo
% make
% make install
```

Hack away and open PRs if you end up with something useful!
