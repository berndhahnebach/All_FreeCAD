#!/bin/sh

# In the Debian rules file we expect to get the OpenCascade installation path
# by the environment variable CASROOT.
export CASROOT=${CASROOT:-$HOME/Projects/OpenCASCADE6.2.0/ros}
make debian-package

