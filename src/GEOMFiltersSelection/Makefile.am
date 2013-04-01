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
#

# GEOM GEOMFiltersSelection : filter selector for the viewer
# File    : Makefile.am
# Author  : Alexander BORODIN, Open CASCADE S.A.S. (alexander.borodin@opencascade.com)
# Package : GEOMFiltersSelection
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libGEOMFiltersSelection.la

# header files
salomeinclude_HEADERS = 	\
	GEOM_SelectionFilter.h	\
	GEOM_EdgeFilter.h	\
	GEOM_FaceFilter.h	\
	GEOM_TypeFilter.h	\
	GEOM_PreviewFilter.h	\
	GEOM_LogicalFilter.h	\
	GEOM_OCCFilter.h	\
	GEOM_CompoundFilter.h

# Sources
dist_libGEOMFiltersSelection_la_SOURCES = \
	GEOM_SelectionFilter.cxx	\
	GEOM_EdgeFilter.cxx		\
	GEOM_FaceFilter.cxx		\
	GEOM_TypeFilter.cxx		\
	GEOM_PreviewFilter.cxx		\
	GEOM_LogicalFilter.cxx		\
	GEOM_OCCFilter.cxx		\
	GEOM_CompoundFilter.cxx

# additional information to compile and link file

libGEOMFiltersSelection_la_CPPFLAGS =	\
	$(QT_INCLUDES)			\
	$(CAS_CPPFLAGS)			\
	$(BOOST_CPPFLAGS)		\
	$(GUI_CXXFLAGS)			\
	$(KERNEL_CXXFLAGS)		\
	$(CORBA_CXXFLAGS)		\
	$(CORBA_INCLUDES)		\
	-I$(srcdir)/../GEOMClient	\
	-I$(top_builddir)/idl

libGEOMFiltersSelection_la_LDFLAGS  = \
	$(QT_MT_LIBS) \
	$(CAS_KERNEL) -lTKG3d -lTKV3d -lTKService \
	../../idl/libSalomeIDLGEOM.la				\
	../GEOMClient/libGEOMClient.la				\
	$(GUI_LDFLAGS) -lsuit -lSalomeApp -lSalomeSession -lSalomeObject -lLightApp \
	$(KERNEL_LDFLAGS) -lSalomeLifeCycleCORBA -lSalomeContainer -lTOOLSDS
