#!/bin/sh
# cvsbuild.sh
# Run this to generate all initial makefiles.

# Make executable due to Debian package policy
chmod u+x Mod/Test/unittestgui.py

#aclocal
#autoheader
#automake --add-missing
#autoconf configure.ac > configure && chmod ug+x configure
#./configure

echo "calling aclocal"
aclocal

echo "calling libtoolize"
libtoolize --force --copy

echo "calling automake"
automake --add-missing

echo "calling autoconf"
autoconf

echo "Done"
echo "Please run configure."
