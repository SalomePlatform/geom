# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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
#

#  File   : Makefile.in
#  Author : Damien COQUERET (OCC)
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets

lib_LTLIBRARIES = libGEOMbasic.la

dist_libGEOMbasic_la_SOURCES =		\
	GEOM_Application.cxx		\
	GEOM_Engine.cxx			\
	GEOM_Function.cxx		\
	GEOM_Object.cxx			\
	GEOM_IOperations.cxx		\
	GEOM_Solver.cxx			\
	GEOM_SubShapeDriver.cxx		\
	GEOM_PythonDump.cxx		\
	GEOM_DataMapNodeOfDataMapOfAsciiStringTransient_0.cxx		\
	GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient_0.cxx	\
	GEOM_DataMapOfAsciiStringTransient_0.cxx

# header files 
salomeinclude_HEADERS =			\
	GEOM_Application.hxx		\
	GEOM_Application.ixx		\
	GEOM_Application.jxx		\
	Handle_GEOM_Application.hxx	\
	GEOM_Engine.hxx			\
	GEOM_Function.hxx		\
	GEOM_Object.hxx			\
	GEOM_SubShapeDriver.hxx		\
	GEOM_IOperations.hxx		\
	GEOM_ISubShape.hxx		\
	GEOM_Solver.hxx			\
	GEOM_PythonDump.hxx		\
	GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient.hxx		\
	GEOM_DataMapNodeOfDataMapOfAsciiStringTransient.hxx		\
	GEOM_DataMapOfAsciiStringTransient.hxx				\
	Handle_GEOM_DataMapNodeOfDataMapOfAsciiStringTransient.hxx

# additional information to compile and link file

libGEOMbasic_la_CPPFLAGS =	\
	$(CAS_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)	\
	$(BOOST_CPPFLAGS)	\
	-I$(top_builddir)/idl

libGEOMbasic_la_LDFLAGS  =			\
	$(KERNEL_LDFLAGS) -lSALOMELocalTrace -lSALOMEBasics	\
	$(STDLIB)				\
	$(CAS_LDPATH) -lTKXSBase -lTKG3d  	\
	$(CAS_OCAF)				\
	$(CAS_OCAFVIS) 
