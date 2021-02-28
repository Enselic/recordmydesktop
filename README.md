# recordMyDesktop
Official repository for recordMyDesktop. Migrated from https://sourceforge.net/projects/recordmydesktop/ for which I am the official and long-time maintainer.

For the new web page with news, visit https://enselic.github.io/recordmydesktop/

# Development

First do
```
% git clone https://github.com/Enselic/recordmydesktop.git
```

## CLI

```
% cd recordmydesktop
% sudo apt install autotools-dev zlib1g-dev libice-dev libsm-dev libxext-dev libxdamage-dev libogg-dev libvorbis-dev libtheora-dev libjack-dev libasound2-dev libpopt-dev
% cd recordmydesktop   # for CLI source code
% sh autogen.sh
% ./configure --prefix=/tmp/foo
% make
% make install
```

## GTK frontend
```
% cd gtk-recordmydesktop
% sudo apt install autotools-dev automake libglib2.0-dev
```
you also need to install pygtk >=2.4 which can be tricky since it is ancient. (Help with migrating to GTK 4 would be greatly appreciated.) After that, you are maybe able to do:
```
% sh autogen.sh
% ./configure --prefix=/tmp/foo
% make
% make install
```

## Qt frontend:
```
% cd qt-recordmydesktop
% sudo apt install autotools-dev automake libglib2.0-dev
```
you also need to install libQt4 >=4.2 which can be tricky since it is ancient. (Help with migrating to Qt 5 would be greatly appreciated.) After that, you are maybe able to do:
```
% sh autogen.sh
% ./configure --prefix=/tmp/foo
% make
% make install
```

Hack away and open PRs if you end up with something useful!
