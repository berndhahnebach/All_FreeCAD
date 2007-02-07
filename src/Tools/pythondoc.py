# Create HTML documentation from FreeCAD's Python modules and classes.
# (c) 2007 Werner Mayer
#
#***************************************************************************
#*   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
#*                                                                         *
#*   This file is part of the FreeCAD CAx development system.              *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   FreeCAD is distributed in the hope that it will be useful,            *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with FreeCAD; if not, write to the Free Software        *
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#***************************************************************************

import pydoc, sys, os, dircache, zipfile

# Get the path to the FreeCAD module relative to this directory
path = __file__[:-12] + "../../bin"
print path
sys.path.append(path)
os.chdir(path)

import FreeCAD

ModDir = FreeCAD.ConfigGet("HomePath")+'Mod'
ModDirs = dircache.listdir(ModDir)

# Search for module paths and append them to system path 
#for Dir in ModDirs:
#	if (Dir != '__init__.py'):
#		sys.path.append( os.path.join(ModDir,Dir) )

print 'Write documentation for module FreeCAD'
pydoc.writedoc(FreeCAD)
print ''

# walk through the module paths again and try loading the modules to create HTML files 
for Dir in ModDirs:
	dest = os.path.join(ModDir,Dir)
	print 'Write documentation for module ' + dest
	if (Dir != '__init__.py'):
		pydoc.writedocs(dest)
		print ''

# create a ZIP archive from all HTML files
print 'Creating ZIP archive \'docs.zip\'...'
zip = zipfile.ZipFile('docs.zip', 'w')
for file in os.listdir('.'):
	if not os.path.isdir(file):
		if file.find('.html') > 0:
			print '  Adding file ' + file + ' to archive'
			zip.write(file)

print 'done.'
zip.close()

print 'Cleaning up HTML files...'
for file in os.listdir('.'):
	if not os.path.isdir(file):
		if file.find('.html') > 0:
			print '  Removing ' + file
			os.remove(file)

print 'done.'

