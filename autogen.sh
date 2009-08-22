#!/bin/sh
# autogen.sh
# Run this script to generate all initial makefiles.

# Get version and revision number
MAJ=0
MIN=9
REV=0

if svn --xml info >/dev/null 2>&1; then
	REV=`svn --xml info | tr -d '\r\n' | sed -e 's/.*<commit.*revision="\([0-9]*\)".*<\/commit>.*/\1/'`
elif svn --version --quiet >/dev/null 2>&1; then
	REV=`svn info | grep "^Revision:" | cut -d" " -f2`
fi

# if revision.m4 does not exist, create it
REV_FILE=revision.m4
if [ -f $REV_FILE ]; then
	echo "$REV_FILE found"
else
	echo "m4_define([FREECAD_MAJOR], $MAJ)" > $REV_FILE
	echo "m4_define([FREECAD_MINOR], $MIN)" >> $REV_FILE
	echo "m4_define([FREECAD_MICRO], $REV)" >> $REV_FILE
	echo "$REV_FILE created"
fi

if which glibtoolize > /dev/null 2>&1; then
	echo "calling glibtoolize"
	glibtoolize --force --copy
else
	echo "calling libtoolize"
	libtoolize --force --copy
fi

# http://www.gnu.org/software/hello/manual/automake/Macro-Search-Path.html
if [ -d /usr/local/share/aclocal ]; then
	echo "calling aclocal -I /usr/local/share/aclocal"
	aclocal -I /usr/local/share/aclocal
else
	echo "calling aclocal"
	aclocal
fi

echo "calling autoheader"
autoheader

echo "calling automake"
automake --add-missing --copy

echo "calling autoconf"
autoconf

echo "Done"
echo "Please run configure."
