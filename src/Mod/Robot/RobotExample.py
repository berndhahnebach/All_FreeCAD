# Example hwo to use the basic robot class Robot6Axis which represent a 6-Axis 
# industrial robot. The Robot Module is not dependend on Part or an other Module.
# It works only with the basic types Placement, Vector and Matrix. So we need 
# only:
import FreeCAD, Robot

# create the robot. If you not specify a other kinematic it becomes a Puma 560
rob = Robot.Robot6Axis()
print rob

# accessing the axis and the Tcp. Axis go from 1-6 and are in degrees:
Start = rob.Tcp
print Start
print rob.Axis1

# move the first Axis of the robot:
rob.Axis1 = 5.0
# the Tcp has changed (forward kinematic)
print rob.Tcp

# move the robot back to start position (reverse kinematic):
rob.Tcp = Start
print rob.Axis1

# the same with axis 2:
rob.Axis2 = 5.0
print rob.Tcp
rob.Tcp = Start
print rob.Axis2

# Waypoints:
w = Robot.Waypoint()

del rob,Start

# Working with the robot document objects:
# first creat a robot in the active document
App.activeDocument().addObject("Robot::RobotObject","Robot")
App.activeDocument().Robot.RobotVrmlFile = App.getResourceDir()+"Mod/Robot/Lib/Kuka/kr500_1.wrl"
App.activeDocument().Robot.Axis2 = -90
App.activeDocument().Robot.Axis3 = 90

# retrive the Tcp position 
pos = FreeCAD.getDocument("Unnamed").getObject("Robot").Tcp
# move the robot
pos.move(App.Vector(-10,0,0))
FreeCAD.getDocument("Unnamed").getObject("Robot").Tcp = pos

