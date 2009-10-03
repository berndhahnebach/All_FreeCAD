
lib_LTLIBRARIES=libRobot.la Robot.la

BUILT_SOURCES=\
		Robot6AxisPy.cpp \
		TrajectoryPy.cpp \
		WaypointPy.cpp

libRobot_la_SOURCES=\
		kdl/utilities/error.h \
		kdl/utilities/error_stack.cxx \
		kdl/utilities/error_stack.h \
		kdl/utilities/kdl-config.h \
		kdl/utilities/rall1d.h \
		kdl/utilities/rall1d_io.h \
		kdl/utilities/rall2d.h \
		kdl/utilities/rall2d_io.h \
		kdl/utilities/rallNd.h \
		kdl/utilities/svd_eigen_HH.cpp \
		kdl/utilities/svd_eigen_HH.hpp \
		kdl/utilities/svd_eigen_Macie.hpp \
		kdl/utilities/svd_HH.cpp \
		kdl/utilities/svd_HH.hpp \
		kdl/utilities/traits.h \
		kdl/utilities/utility.cxx \
		kdl/utilities/utility.h \
		kdl/utilities/utility_io.cxx \
		kdl/utilities/utility_io.h \
		kdl/chain.cpp \
		kdl/chain.hpp \
		kdl/chainfksolver.hpp \
		kdl/chainfksolverpos_recursive.cpp \
		kdl/chainfksolverpos_recursive.hpp \
		kdl/chainfksolvervel_recursive.cpp \
		kdl/chainfksolvervel_recursive.hpp \
		kdl/chainidsolver.hpp \
		kdl/chainidsolver_recursive_newton_euler.cpp \
		kdl/chainidsolver_recursive_newton_euler.hpp \
		kdl/chainiksolver.hpp \
		kdl/chainiksolverpos_nr.cpp \
		kdl/chainiksolverpos_nr.hpp \
		kdl/chainiksolverpos_nr_jl.cpp \
		kdl/chainiksolverpos_nr_jl.hpp \
		kdl/chainiksolvervel_pinv.cpp \
		kdl/chainiksolvervel_pinv_givens.cpp \
		kdl/chainiksolvervel_pinv_givens.hpp \
		kdl/chainiksolvervel_pinv.hpp \
		kdl/chainiksolvervel_wdls.cpp \
		kdl/chainiksolvervel_wdls.hpp \
		kdl/chainjnttojacsolver.cpp \
		kdl/chainjnttojacsolver.hpp \
		kdl/frameacc.cpp \
		kdl/frameacc.hpp \
		kdl/frameacc.inl \
		kdl/frameacc_io.hpp \
		kdl/frames.cpp \
		kdl/frames.hpp \
		kdl/frames.inl \
		kdl/frames_io.cpp \
		kdl/frames_io.hpp \
		kdl/framevel.cpp \
		kdl/framevel.hpp \
		kdl/framevel.inl \
		kdl/framevel_io.hpp \
		kdl/jacobian.cpp \
		kdl/jacobian.hpp \
		kdl/jntarrayacc.cpp \
		kdl/jntarrayacc.hpp \
		kdl/jntarray.cpp \
		kdl/jntarray.hpp \
		kdl/jntarrayvel.cpp \
		kdl/jntarrayvel.hpp \
		kdl/joint.cpp \
		kdl/joint.hpp \
		kdl/kdl.hpp \
		kdl/kinfam.hpp \
		kdl/kinfam_io.cpp \
		kdl/kinfam_io.hpp \
		kdl/motion.hpp \
		kdl/path_circle.cpp \
		kdl/path_circle.hpp \
		kdl/path_composite.cpp \
		kdl/path_composite.hpp \
		kdl/path.cpp \
		kdl/path_cyclic_closed.cpp \
		kdl/path_cyclic_closed.hpp \
		kdl/path.hpp \
		kdl/path_line.cpp \
		kdl/path_line.hpp \
		kdl/path_point.cpp \
		kdl/path_point.hpp \
		kdl/path_roundedcomposite.cpp \
		kdl/path_roundedcomposite.hpp \
		kdl/rigidbodyinertia.cpp \
		kdl/rigidbodyinertia.hpp \
		kdl/rotationalinertia.cpp \
		kdl/rotationalinertia.hpp \
		kdl/rotational_interpolation.cpp \
		kdl/rotational_interpolation.hpp \
		kdl/rotational_interpolation_sa.cpp \
		kdl/rotational_interpolation_sa.hpp \
		kdl/segment.cpp \
		kdl/segment.hpp \
		kdl/stiffness.hpp \
		kdl/trajectory_composite.cpp \
		kdl/trajectory_composite.hpp \
		kdl/trajectory.cpp \
		kdl/trajectory.hpp \
		kdl/trajectory_segment.cpp \
		kdl/trajectory_segment.hpp \
		kdl/trajectory_stationary.cpp \
		kdl/trajectory_stationary.hpp \
		kdl/tree.cpp \
		kdl/treefksolver.hpp \
		kdl/treefksolverpos_recursive.cpp \
		kdl/treefksolverpos_recursive.hpp \
		kdl/tree.hpp \
		kdl/treeiksolver.hpp \
		kdl/treeiksolverpos_nr_jl.cpp \
		kdl/treeiksolverpos_nr_jl.hpp \
		kdl/treeiksolvervel_wdls.cpp \
		kdl/treeiksolvervel_wdls.hpp \
		kdl/treejnttojacsolver.cpp \
		kdl/treejnttojacsolver.hpp \
		kdl/velocityprofile.cpp \
		kdl/velocityprofile_dirac.cpp \
		kdl/velocityprofile_dirac.hpp \
		kdl/velocityprofile.hpp \
		kdl/velocityprofile_rect.cpp \
		kdl/velocityprofile_rect.hpp \
		kdl/velocityprofile_trap.cpp \
		kdl/velocityprofile_traphalf.cpp \
		kdl/velocityprofile_traphalf.hpp \
		kdl/velocityprofile_trap.hpp \
		\
                AppRobotPy.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		Robot6Axis.cpp \
		Robot6Axis.h \
		Robot6AxisPyImp.cpp \
		RobotAlgos.cpp \
		RobotAlgos.h \
		RobotObject.cpp \
		RobotObject.h \
		Trajectory.cpp \
		Trajectory.h \
		TrajectoryObject.cpp \
		TrajectoryObject.h \
		TrajectoryPyImp.cpp \
		Waypoint.cpp \
		Waypoint.h \
		WaypointPyImp.cpp 

%.cpp: %.xml $(top_srcdir)/src/Tools/generate.py
	$(PYTHON) $(top_srcdir)/src/Tools/generate.py --outputPath $(@D) $<

# the library search path.
libRobot_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) \
	-version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libRobot_la_CPPFLAGS = -DRobotAppExport=

libRobot_la_LIBADD   = \
		@BOOST_REGEX_LIB@ @BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp

#--------------------------------------------------------------------------------------
# Loader of libRobot

Robot_la_SOURCES=\
		AppRobot.cpp

# the library search path.
Robot_la_LDFLAGS = $(libRobot_la_LDFLAGS) -module -avoid-version
Robot_la_CPPFLAGS = $(libRobot_la_CPPFLAGS)

Robot_la_LIBADD   = \
		$(libRobot_la_LIBADD) \
		-lRobot

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= -I$(top_srcdir)/src -I$(top_builddir)/src -I$(top_srcdir)/src/Mod/Robot/App $(all_includes) -I/usr/include/eigen2


libdir = $(prefix)/Mod/Robot

EXTRA_DIST = \
		kdl/utilities/header.txt \
		kdl/CMakeLists.txt \
		kdl/kdl.pc.in \
		kdl/README.txt \
		kdl/TODO.txt \
		Robot6AxisPy.xml \
		TrajectoryPy.xml \
		WaypointPy.xml \
		CMakeLists.txt
