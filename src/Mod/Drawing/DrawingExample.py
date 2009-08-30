# exampel how to use the scripting API of the drawing module
# 
# first of all you need the Part and the Drawing module:
import Part, Drawing

# create a small sample part
Part.show(Part.makeBox(100,100,100).cut(Part.makeCylinder(80,100)).cut(Part.makeBox(90,40,100)).cut(Part.makeBox(20,85,100)))

# direct projection
Shape = App.ActiveDocument.Shape.Shape
[visibly,hidden] = Drawing.project(Shape)
print "visible edges:", len(visibly.Edges)
print "hidden edges:", len(hidden.Edges)
# all was projected on the Z-plane:
print "Bnd Box shape: X=",Shape.BoundBox.XLength," Y=",Shape.BoundBox.YLength," Z=",Shape.BoundBox.ZLength
print "Bnd Box project: X=",visibly.BoundBox.XLength," Y=",visibly.BoundBox.YLength," Z=",visibly.BoundBox.ZLength

# And now the parametric way
# 
# insert a Page object and assign a template
App.activeDocument().addObject('Drawing::FeaturePage','Page')
App.activeDocument().Page.Template = App.ConfigGet('AppHomePath')+'Mod/Drawing/Templates/A3_Landscape.svg'

# create a view on the "Shape" object, define the position and scale and assign it to a Page
App.activeDocument().addObject('Drawing::FeatureViewPart','View')
App.activeDocument().View.Source = App.activeDocument().Shape
App.activeDocument().View.Direction = (0.0,0.0,1.0)
App.activeDocument().View.X = 10.0
App.activeDocument().View.Y = 10.0
App.activeDocument().Page.addObject(App.activeDocument().View)

# create a second view on the same object but the view is 
# rotatet 90 degrees.
App.activeDocument().addObject('Drawing::FeatureViewPart','ViewRot')
App.activeDocument().ViewRot.Source = App.activeDocument().Shape
App.activeDocument().ViewRot.Direction = (0.0,0.0,1.0)
App.activeDocument().ViewRot.X = 290.0
App.activeDocument().ViewRot.Y = 30.0
App.activeDocument().ViewRot.Scale = 1.0
App.activeDocument().ViewRot.Rotation = 90.0
App.activeDocument().Page.addObject(App.activeDocument().ViewRot) 

# create a third view on the same object but with an isometric
# view direction. Also the hidden lines are activated.

App.activeDocument().addObject('Drawing::FeatureViewPart','ViewIso')
App.activeDocument().ViewIso.Source = App.activeDocument().Shape
App.activeDocument().ViewIso.Direction = (1.0,1.0,1.0)
App.activeDocument().ViewIso.X = 335.0
App.activeDocument().ViewIso.Y = 140.0
App.activeDocument().ViewIso.ShowHiddenLines = True
App.activeDocument().Page.addObject(App.activeDocument().ViewIso) 

# change something and update.
# The update process change the view and the page
App.activeDocument().View.X = 30.0
App.activeDocument().View.Y = 30.0
App.activeDocument().View.Scale = 1.5
App.activeDocument().recompute()

# Accessing the bits and peaces:
# get the SVG fragment of a single view
ViewSVG = App.activeDocument().View.ViewResult
print ViewSVG

# get the hole result page (its a file in the document temp dir, only read allowed)
print "Resulting SVG document: ",App.activeDocument().Page.PageResult
file = open(App.activeDocument().Page.PageResult,"r")
print "Result page is ",len(file.readlines())," lines long"
# important, give free the file!
del file

# insert a view with your own content:
App.activeDocument().addObject('Drawing::FeatureView','ViewSelf')
App.activeDocument().ViewSelf.ViewResult = """<g id="ViewSelf"
  stroke="rgb(0, 0, 0)"
  stroke-width="0.35"
  stroke-linecap="butt"
  stroke-linejoin="miter"
  transform="translate(30,30)"
  fill="#00cc00"
 >

<ellipse cx="40" cy="40" rx="30" ry="15"/>
</g>
"""
App.activeDocument().Page.addObject(App.activeDocument().ViewSelf)

App.activeDocument().recompute()

del Shape,ViewSVG

