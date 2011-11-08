# Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

# GEOM GEOMBASE : 
# File    : Makefile.am
# Author  : Alexander BORODIN, Open CASCADE S.A.S. (alexander.borodin@opencascade.com)
# Package : GEOMBase
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libGEOMBase.la

# header files 
salomeinclude_HEADERS =		\
	GEOMBase.h		\
	GEOMBase_Skeleton.h	\
	GEOMBase_Helper.h	\
	GEOM_Operation.h	\
	GEOM_GEOMBase.hxx	\
	GEOM_GenericObjPtr.h

dist_libGEOMBase_la_SOURCES =		\
	GEOMBase.cxx			\
	GEOMBase_Skeleton.cxx		\
	GEOMBase_Helper.cxx		\
	GEOM_Operation.cxx		\
	GEOM_GenericObjPtr.cxx

MOC_FILES =				\
	GEOMBase_Skeleton_moc.cxx

nodist_libGEOMBase_la_SOURCES =	\
	$(MOC_FILES)

# additional information to compile and link file

libGEOMBase_la_CPPFLAGS =		\
	$(QT_INCLUDES)			\
	$(VTK_INCLUDES)			\
	$(CAS_CPPFLAGS)			\
	$(PYTHON_INCLUDES)		\
	$(BOOST_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)		\
	$(GUI_CXXFLAGS)			\
	$(CORBA_CXXFLAGS)		\
	$(CORBA_INCLUDES)		\
	$(OPENCV_INCLUDES)		\
	-I$(srcdir)/../OBJECT		\
	-I$(srcdir)/../GEOMClient	\
	-I$(srcdir)/../GEOMImpl		\
	-I$(srcdir)/../GEOMGUI		\
	-I$(srcdir)/../DlgRef		\
	-I$(top_builddir)/src/DlgRef	\
	-I$(top_builddir)/idl		

libGEOMBase_la_LDFLAGS  =		\
	../../idl/libSalomeIDLGEOM.la	\
	../GEOMGUI/libGEOM.la		\
	../DlgRef/libDlgRef.la		\
	$(KERNEL_LDFLAGS) -lOpUtil	\
	$(GUI_LDFLAGS) -lsuit -lOCCViewer -lVTKViewer -lSVTK -lSalomePrs -lSalomeApp -lCAM \
	$(CAS_LDPATH) -lTKPrim		\
	$(OPENCV_LIBS)
