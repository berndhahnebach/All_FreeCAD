#!/bin/sh

aclocal
autoheader
automake --add-missing
autoconf configure.ac > configure && chmod ug+x configure
./configure
