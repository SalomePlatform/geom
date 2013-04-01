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

# File    : Makefile.am
# Author  : Alexander BORODIN, Open CASCADE S.A.S. (alexander.borodin@opencascade.com)
# Package : GEOMToolsGUI
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libGEOMToolsGUI.la

# header files 
salomeinclude_HEADERS =				\
	GEOMToolsGUI.h                          \
	GEOM_ToolsGUI.hxx			\
	GEOMToolsGUI_NbIsosDlg.h		\
	GEOMToolsGUI_DeflectionDlg.h		\
	GEOMToolsGUI_TransparencyDlg.h		\
	GEOMToolsGUI_DeleteDlg.h		\
	GEOMToolsGUI_MarkerDlg.h                \
	GEOMToolsGUI_PublishDlg.h		\
	GEOMToolsGUI_MaterialPropertiesDlg.h    \
	GEOMToolsGUI_LineWidthDlg.h

dist_libGEOMToolsGUI_la_SOURCES =		\
	GEOMToolsGUI.cxx			\
	GEOMToolsGUI_1.cxx			\
	GEOMToolsGUI_TransparencyDlg.cxx	\
	GEOMToolsGUI_NbIsosDlg.cxx		\
	GEOMToolsGUI_DeflectionDlg.cxx		\
	GEOMToolsGUI_DeleteDlg.cxx		\
	GEOMToolsGUI_MarkerDlg.cxx              \
	GEOMToolsGUI_PublishDlg.cxx		\
	GEOMToolsGUI_MaterialPropertiesDlg.cxx  \
	GEOMToolsGUI_LineWidthDlg.cxx

MOC_FILES = 					\
	GEOMToolsGUI_TransparencyDlg_moc.cxx	\
	GEOMToolsGUI_NbIsosDlg_moc.cxx		\
	GEOMToolsGUI_DeflectionDlg_moc.cxx	\
	GEOMToolsGUI_DeleteDlg_moc.cxx		\
	GEOMToolsGUI_MarkerDlg_moc.cxx          \
	GEOMToolsGUI_PublishDlg_moc.cxx		\
	GEOMToolsGUI_MaterialPropertiesDlg_moc.cxx \
	GEOMToolsGUI_LineWidthDlg_moc.cxx

nodist_libGEOMToolsGUI_la_SOURCES =		\
	$(MOC_FILES)

# additional information to compile and link file

libGEOMToolsGUI_la_CPPFLAGS =		\
	$(QT_INCLUDES)			\
	$(VTK_INCLUDES)			\
	$(CAS_CPPFLAGS)			\
	$(PYTHON_INCLUDES)		\
	$(BOOST_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)		\
	$(GUI_CXXFLAGS)			\
	$(CORBA_CXXFLAGS)		\
	$(CORBA_INCLUDES)		\
	-I$(srcdir)/../OBJECT		\
	-I$(srcdir)/../GEOMBase		\
	-I$(srcdir)/../Material		\
	-I$(srcdir)/../GEOMGUI		\
	-I$(srcdir)/../GEOMClient	\
	-I$(srcdir)/../GEOMImpl		\
	-I$(top_builddir)/idl

libGEOMToolsGUI_la_LDFLAGS  = 		\
	../GEOMBase/libGEOMBase.la 	\
	../Material/libMaterial.la 	\
	../GEOMGUI/libGEOM.la 		\
	$(GUI_LDFLAGS) -lVTKViewer -lOCCViewer -lsuit -lSalomeApp -lPyConsole
