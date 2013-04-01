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

# GEOM GENERATIONGUI : 
# File    : Makefile.am
# Author  : Alexander BORODIN, Open CASCADE S.A.S. (alexander.borodin@opencascade.com)
# Package : GenerationGUI

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libGenerationGUI.la

# header files 
salomeinclude_HEADERS =			\
	GenerationGUI.h			\
	GenerationGUI_PrismDlg.h	\
	GenerationGUI_RevolDlg.h	\
	GenerationGUI_FillingDlg.h	\
	GenerationGUI_PipeDlg.h		\
	GenerationGUI_PipePathDlg.h

dist_libGenerationGUI_la_SOURCES =	\
	GenerationGUI.h			\
	GenerationGUI_PrismDlg.h	\
	GenerationGUI_RevolDlg.h	\
	GenerationGUI_FillingDlg.h	\
	GenerationGUI_PipeDlg.h		\
	GenerationGUI_PipePathDlg.h	\
					\
	GenerationGUI.cxx		\
	GenerationGUI_PrismDlg.cxx	\
	GenerationGUI_RevolDlg.cxx	\
	GenerationGUI_FillingDlg.cxx	\
	GenerationGUI_PipeDlg.cxx	\
	GenerationGUI_PipePathDlg.cxx

MOC_FILES = 					\
	GenerationGUI_PrismDlg_moc.cxx		\
	GenerationGUI_RevolDlg_moc.cxx		\
	GenerationGUI_FillingDlg_moc.cxx	\
	GenerationGUI_PipeDlg_moc.cxx		\
	GenerationGUI_PipePathDlg_moc.cxx

nodist_libGenerationGUI_la_SOURCES =	\
	$(MOC_FILES)

# additional information to compile and link file

libGenerationGUI_la_CPPFLAGS =			\
	$(QT_INCLUDES)				\
	$(VTK_INCLUDES)				\
	$(CAS_CPPFLAGS)				\
	$(PYTHON_INCLUDES)			\
	$(BOOST_CPPFLAGS)			\
	$(KERNEL_CXXFLAGS)			\
	$(GUI_CXXFLAGS)				\
	$(CORBA_CXXFLAGS)			\
	$(CORBA_INCLUDES)			\
	-I$(srcdir)/../GEOMGUI			\
	-I$(srcdir)/../DlgRef			\
	-I$(srcdir)/../GEOMBase			\
	-I$(srcdir)/../OBJECT			\
	-I$(srcdir)/../GEOMClient		\
	-I$(srcdir)/../GEOMImpl			\
	-I$(srcdir)/../GEOMFiltersSelection	\
	-I$(top_builddir)/src/DlgRef		\
	-I$(top_builddir)/idl

libGenerationGUI_la_LDFLAGS  =					\
	../GEOMFiltersSelection/libGEOMFiltersSelection.la	\
	../GEOMBase/libGEOMBase.la				\
	$(CAS_LDPATH) -lTKOffset 
