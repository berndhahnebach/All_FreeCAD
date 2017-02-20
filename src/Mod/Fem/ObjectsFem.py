# ***************************************************************************
# *                                                                         *
# *   Copyright (c) 2016 - Bernd Hahnebach <bernd@bimstatik.org>            *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with this program; if not, write to the Free Software   *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# ***************************************************************************

__title__ = "Objects FEM"
__author__ = "Bernd Hahnebach"
__url__ = "http://www.freecadweb.org"

## \addtogroup FEM
#  @{

import FreeCAD


########## analysis ##########
def makeAnalysis(name):
    '''makeAnalysis(name): makes a Fem Analysis object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::FemAnalysisPython", name)
    return obj


########## solvers ##########
def makeSolverCalculix(name="CalculiX"):
    '''makeSolverCalculix(name): makes a Calculix solver object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::FemSolverObjectPython", name)
    import _FemSolverCalculix
    _FemSolverCalculix._FemSolverCalculix(obj)
    if FreeCAD.GuiUp:
        import _ViewProviderFemSolverCalculix
        _ViewProviderFemSolverCalculix._ViewProviderFemSolverCalculix(obj.ViewObject)
    return obj


def makeSolverZ88(name="Z88"):
    '''makeSolverZ88(name): makes a Z88 solver object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::FemSolverObjectPython", name)
    import _FemSolverZ88
    _FemSolverZ88._FemSolverZ88(obj)
    if FreeCAD.GuiUp:
        import _ViewProviderFemSolverZ88
        _ViewProviderFemSolverZ88._ViewProviderFemSolverZ88(obj.ViewObject)
    return obj


########## element geometry definitions ##########
def makeBeamSection(sectiontype='Rectangular', width=10.0, height=25.0, name="BeamSection"):
    '''makeBeamSection([width], [height], [name]): creates an beamsection object to define a cross section'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::FeaturePython", name)
    import _FemBeamSection
    _FemBeamSection._FemBeamSection(obj)
    sec_types = _FemBeamSection._FemBeamSection.known_beam_types
    if sectiontype not in sec_types:
        FreeCAD.Console.PrintError("Section type is not known. Set to " + sec_types[0] + " \n")
        obj.SectionType = sec_types[0]
    else:
        obj.SectionType = sectiontype
    obj.RectWidth = width
    obj.RectHeight = height
    obj.CircDiameter = height
    obj.PipeDiameter = height
    obj.PipeThickness = width
    if FreeCAD.GuiUp:
        import _ViewProviderFemBeamSection
        _ViewProviderFemBeamSection._ViewProviderFemBeamSection(obj.ViewObject)
    return obj


def makeShellThickness(thickness=20.0, name="ShellThickness"):
    '''makeShellThickness([thickness], [name]): creates an shellthickness object to define a plate thickness'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::FeaturePython", name)
    import _FemShellThickness
    _FemShellThickness._FemShellThickness(obj)
    obj.Thickness = thickness
    if FreeCAD.GuiUp:
        import _ViewProviderFemShellThickness
        _ViewProviderFemShellThickness._ViewProviderFemShellThickness(obj.ViewObject)
    return obj


########## constraints ##########
def makeConstraintBearing(name):
    '''makeConstraintBearing(name): makes a Fem ConstraintBearing object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintBearing", name)
    return obj


def makeConstraintContact(name):
    '''makeConstraintContact(name): makes a Fem ConstraintContact object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintContact", name)
    return obj


def makeConstraintDisplacement(name):
    '''makeConstraintDisplacement(name): makes a Fem ConstraintDisplacement object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintDisplacement", name)
    return obj


def makeConstraintFixed(name):
    '''makeConstraintFixed(name): makes a Fem ConstraintFixed object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintFixed", name)
    return obj


def makeConstraintFluidBoundary(name):
    '''makeConstraintFluidBoundary(name): makes a Fem ConstraintForce object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintForce", name)
    return obj


def makeConstraintForce(name):
    '''makeConstraintForce(name): makes a Fem ConstraintForce object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintForce", name)
    return obj


def makeConstraintGear(name):
    '''makeConstraintGear(name): makes a Fem ConstraintGear object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintGear", name)
    return obj


def makeConstraintHeatflux(name):
    '''makeConstraintHeatflux(name): makes a Fem ConstraintHeatflux object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintHeatflux", name)
    return obj


def makeConstraintInitialTemperature(name):
    '''makeConstraintInitialTemperature(name): makes a Fem ConstraintInitialTemperature object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintInitialTemperature", name)
    return obj


def makeConstraintPlaneRotation(name):
    '''makeConstraintPlaneRotation(name): makes a Fem ConstraintPlaneRotation object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintPlaneRotation", name)
    return obj


def makeConstraintPressure(name):
    '''makeConstraintPressure(name): makes a Fem ConstraintPressure object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintPressure", name)
    return obj


def makeConstraintPulley(name):
    '''makeConstraintPulley(name): makes a Fem ConstraintPulley object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintPulley", name)
    return obj


def makeConstraintTemperature(name):
    '''makeConstraintTemperature(name): makes a Fem ConstraintTemperature object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintTemperature", name)
    return obj


def makeConstraintTransform(name):
    '''makeConstraintTransform(name): makes a Fem ConstraintTransform object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::ConstraintTransform", name)
    return obj


########## meshes ##########
def makeMeshShapeNetgenObject(name):
    '''makeMeshShapeNetgenObject(name): makes a Fem MeshShapeNetgenObject object'''
    obj = FreeCAD.ActiveDocument.addObject("Fem::FemMeshShapeNetgenObject", name)
    return obj


'''
# print supportedTypes
App.newDocument()
module = 'Fem'
FreeCADGui.doCommand('import ' + module)
for s in sorted(App.ActiveDocument.supportedTypes()):
    if s.startswith(module):
        print s
'''

#  @}
