#!/bin/sh
# autogen.sh
# Run this script to generate all initial makefiles.

# Create Version.h with Python script
#echo "creating Version.h"
#python src/Tools/SubWCRev.py

echo "calling libtoolize"
libtoolize --force --copy

echo "calling aclocal"
aclocal

echo "calling autoheader"
autoheader

echo "calling automake"
automake --add-missing --copy

echo "calling autoconf"
autoconf

echo "Done"
echo "Please run configure."
