# recordMyDesktop
Official repository for recordMyDesktop. Migrated from https://sourceforge.net/projects/recordmydesktop/ for which I am the official and long-time maintainer.

For the new web page with news, visit https://enselic.github.io/recordmydesktop/

# Development

To work on the CLI:
```
% git clone https://github.com/Enselic/recordmydesktop.git
% cd recordmydesktop
% sudo apt install autotools-dev zlib1g-dev libice-dev libsm-dev libxext-dev libxdamage-dev libogg-dev libvorbis-dev libtheora-dev libjack-dev libasound2-dev libpopt-dev
% cd recordmydesktop   # for CLI source code
% sh autogen.sh
% ./configure --prefix=/tmp/foo
% make
% make install
```

Hack away and open PRs if you end up with something useful!