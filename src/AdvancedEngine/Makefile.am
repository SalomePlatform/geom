# Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libAdvancedEngine.la

# header files
salomeinclude_HEADERS =				\
	AdvancedEngine_Types.hxx		\
	AdvancedEngine_OperationsCreator.hh	\
	GEOM_AdvancedEngine.hxx			\
	GEOMImpl_IAdvancedOperations.hxx	\
	GEOM_IAdvancedOperations_i.hh

ADVANCED_INCLUDES  =
ADVANCED_INCLUDES += GEOMImpl_IPipeTShape.hxx GEOMImpl_PipeTShapeDriver.hxx
ADVANCED_INCLUDES += GEOMImpl_IDividedDisk.hxx GEOMImpl_DividedDiskDriver.hxx
##ADVANCED_INCLUDES += GEOMImpl_IDividedCylinder.hxx GEOMImpl_DividedCylinderDriver.hxx
ADVANCED_INCLUDES += GEOMImpl_ISmoothingSurface.hxx GEOMImpl_SmoothingSurfaceDriver.hxx
##@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@##

salomeinclude_HEADERS += $(ADVANCED_INCLUDES)

dist_libAdvancedEngine_la_SOURCES =		\
	AdvancedEngine.cxx			\
	AdvancedEngine_OperationsCreator.cc	\
	GEOMImpl_IAdvancedOperations.cxx	\
	GEOM_IAdvancedOperations_i.cc

ADVANCED_SOURCES  =
ADVANCED_SOURCES += GEOMImpl_PipeTShapeDriver.cxx
ADVANCED_SOURCES += GEOMImpl_DividedDiskDriver.cxx
##ADVANCED_SOURCES += GEOMImpl_DividedCylinderDriver.cxx
ADVANCED_SOURCES += GEOMImpl_SmoothingSurfaceDriver.cxx
##@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@##

dist_libAdvancedEngine_la_SOURCES += $(ADVANCED_SOURCES)

# additional information to compile and link file

libAdvancedEngine_la_CPPFLAGS =		\
	$(CORBA_CXXFLAGS)		\
	$(CORBA_INCLUDES)		\
	$(CAS_CPPFLAGS)			\
	$(BOOST_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)		\
	-I$(srcdir)/../GEOMUtils	\
	-I$(srcdir)/../NMTDS		\
	-I$(srcdir)/../NMTTools		\
	-I$(srcdir)/../GEOMAlgo		\
	-I$(srcdir)/../GEOM		\
	-I$(srcdir)/../GEOMImpl		\
	-I$(srcdir)/../GEOM_I		\
	-I$(top_builddir)/idl		\
	-I$(top_builddir)

libAdvancedEngine_la_LDFLAGS  =			\
	../../idl/libSalomeIDLGEOM.la		\
	../GEOMUtils/libGEOMUtils.la		\
	../GEOMAlgo/libGEOMAlgo.la		\
	../GEOM/libGEOMbasic.la			\
	../GEOMImpl/libGEOMimpl.la		\
	../GEOM_I/libGEOMEngine.la		\
	$(KERNEL_LDFLAGS) -lOpUtil -lSalomeNS -lSalomeContainer -lSalomeGenericObj -lTOOLSDS	\
	$(CAS_DATAEXCHANGE)			\
	$(CAS_LDPATH) -lTKFillet -lTKOffset
