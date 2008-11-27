#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  GEOM DISPLAYGUI : 
#  File   : Makefile.in
#  Author : Damien COQUERET (OCC)
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#  $Header: 
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS = \
	DisplayGUI.h

# Libraries targets
lib_LTLIBRARIES = libDisplayGUI.la

dist_libDisplayGUI_la_SOURCES = DisplayGUI.cxx

#LIB_CLIENT_IDL = SALOME_Exception.idl SALOME_GenericObj.idl SALOME_Component.idl

#LIB_SERVER_IDL = 

# additionnal information to compil and link file

libDisplayGUI_la_CPPFLAGS = \
	$(QT_INCLUDES) \
	$(VTK_INCLUDES) \
	$(CAS_CPPFLAGS) \
	$(PYTHON_INCLUDES) \
	$(BOOST_CPPFLAGS) \
	$(KERNEL_CXXFLAGS) \
	$(GUI_CXXFLAGS) \
	$(CORBA_CXXFLAGS) \
	$(CORBA_INCLUDES) \
	-I$(srcdir)/../GEOMGUI \
	-I$(srcdir)/../GEOMBase \
	-I$(srcdir)/../OBJECT \
	-I$(srcdir)/../GEOMImpl \
	-I$(srcdir)/../GEOMClient \
	-I$(top_builddir)/idl \
	-I$(top_builddir)/salome_adm/unix

libDisplayGUI_la_LDFLAGS = \
	../GEOMGUI/libGEOM.la \
	../GEOMToolsGUI/libGEOMToolsGUI.la \
	$(GUI_LDFLAGS) -lsuit -lSVTK -lSOCC -lSalomePrs -lSalomeApp \
	$(VTK_LIBS) \
	$(QT_MT_LIBS)
