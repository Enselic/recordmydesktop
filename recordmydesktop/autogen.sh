#!/bin/sh


aclocal 
autoheader
automake --copy --add-missing
autoconf