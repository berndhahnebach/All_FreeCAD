#!/bin/sh
# cvsbuild.sh
# Run this to generate all initial makefiles.

#aclocal
#autoheader
#automake --add-missing
#autoconf configure.ac > configure && chmod ug+x configure
#./configure

echo "calling libtoolize"
libtoolize --force --copy


echo "calling aclocal"
aclocal

echo "calling autoconf"
autoconf

echo "calling automake"
automake

echo "Done"
echo "Please run configure."
