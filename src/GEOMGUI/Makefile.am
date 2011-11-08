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

# GEOM GEOMGUI : GUI for Geometry component
# File    : Makefile.am
# Author  : Alexander BORODIN, Open CASCADE S.A.S. (alexander.borodin@opencascade.com)
# Package : GEOMGUI
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libGEOM.la

# header files 
salomeinclude_HEADERS = 	\
	GeometryGUI.h		\
	GeometryGUI_Operations.h\
	GEOMGUI.h		\
	GEOM_Displayer.h	\
	GEOMGUI_OCCSelector.h	\
	GEOMGUI_Selection.h	\
	GeometryGUI_Swig.hxx	\
	GeometryGUI_Swig.i	\
	GEOM_GEOMGUI.hxx

dist_libGEOM_la_SOURCES =	\
	GeometryGUI.cxx		\
	GEOMGUI.cxx		\
	GEOM_Displayer.cxx	\
        GEOMGUI_OCCSelector.cxx	\
	GEOMGUI_Selection.cxx	\
	GeometryGUI_Swig.cxx

MOC_FILES =			\
	GeometryGUI_moc.cxx

nodist_libGEOM_la_SOURCES =	\
	$(MOC_FILES)

# resources files
nodist_salomeres_DATA =	\
	GEOM_images.qm	\
	GEOM_msg_en.qm	\
	GEOM_msg_fr.qm

# additional information to compile and link file

libGEOM_la_CPPFLAGS =				\
	$(QT_INCLUDES)				\
	$(VTK_INCLUDES)				\
	$(CAS_CPPFLAGS)				\
	$(PYTHON_INCLUDES)			\
	$(BOOST_CPPFLAGS)			\
	$(KERNEL_CXXFLAGS)			\
	$(GUI_CXXFLAGS)				\
	$(CORBA_CXXFLAGS)			\
	$(CORBA_INCLUDES)			\
	$(OPENCV_INCLUDES)			\
	-I$(srcdir)/../OBJECT			\
	-I$(srcdir)/../GEOMFiltersSelection	\
	-I$(srcdir)/../GEOMClient		\
	-I$(srcdir)/../GEOMImpl			\
	-I$(top_builddir)/idl			\
	-I$(top_builddir)

libGEOM_la_LDFLAGS =						\
	$(PYTHON_LIBS)						\
	../GEOMFiltersSelection/libGEOMFiltersSelection.la	\
	../GEOMClient/libGEOMClient.la				\
	../OBJECT/libGEOMObject.la				\
	$(KERNEL_LDFLAGS) -lSalomeLifeCycleCORBA -lSalomeNS -lSalomeDSClient		\
	$(GUI_LDFLAGS) -lOCCViewer -lVTKViewer -lCAM -lSOCC -lSVTK -lEvent -lSalomePrs -lstd	\
	$(OPENCV_LIBS)

# -lSalomeApp 
# -lToolsGUI
#	$(CORBA_LIBS) \
#	$(STDLIB)
