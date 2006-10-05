#!/bin/sh
# autogen.sh
# Run this to generate all initial makefiles.

# Create Version.h with Python script
echo "creating Version.h"
python Tools/SubWCRev.py

echo "calling aclocal"
aclocal

echo "calling autoheader"
autoheader

echo "calling autoconf"
autoconf

echo "calling automake"
automake --add-missing --copy

echo "calling libtoolize"
libtoolize --force --copy

echo "Done"
echo "Please run configure."
