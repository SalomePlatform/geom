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

# GEOM DISPLAYGUI : 
# File    : Makefile.am
# Author  : Alexander BORODIN, Open CASCADE S.A.S. (alexander.borodin@opencascade.com)
# Package : DisplayGUI
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =	\
	DisplayGUI.h

# Libraries targets
lib_LTLIBRARIES = libDisplayGUI.la

dist_libDisplayGUI_la_SOURCES = \
	DisplayGUI.h		\
	DisplayGUI.cxx

# additional information to compile and link file

libDisplayGUI_la_CPPFLAGS =		\
	$(QT_INCLUDES)			\
	$(VTK_INCLUDES)			\
	$(CAS_CPPFLAGS)			\
	$(PYTHON_INCLUDES)		\
	$(BOOST_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)		\
	$(GUI_CXXFLAGS)			\
	$(CORBA_CXXFLAGS)		\
	$(CORBA_INCLUDES)		\
	-I$(srcdir)/../GEOMGUI		\
	-I$(srcdir)/../OBJECT		\
	-I$(srcdir)/../GEOMImpl		\
	-I$(srcdir)/../GEOMClient	\
	-I$(top_builddir)/idl

libDisplayGUI_la_LDFLAGS =		\
	../GEOMGUI/libGEOM.la		\
	$(GUI_LDFLAGS) -lsuit -lSVTK	\
	-lSOCC -lSalomePrs -lSalomeApp	\
	$(VTK_LIBS)			\
	$(QT_MT_LIBS)
