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

#  File   : Makefile.am
#  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =			\
	AdvancedGUI.h

ADVANCED_INCLUDES  =
ADVANCED_INCLUDES += AdvancedGUI_PipeTShapeDlg.h
ADVANCED_INCLUDES += AdvancedGUI_DividedDiskDlg.h
ADVANCED_INCLUDES += AdvancedGUI_DividedCylinderDlg.h
##@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@##

salomeinclude_HEADERS += $(ADVANCED_INCLUDES)

# Libraries targets
lib_LTLIBRARIES = libAdvancedGUI.la

dist_libAdvancedGUI_la_SOURCES =	\
	AdvancedGUI.h			\
	AdvancedGUI.cxx

ADVANCED_SOURCES  =
ADVANCED_SOURCES += AdvancedGUI_PipeTShapeDlg.h AdvancedGUI_PipeTShapeDlg.cxx
ADVANCED_SOURCES += AdvancedGUI_DividedDiskDlg.h AdvancedGUI_DividedDiskDlg.cxx
ADVANCED_SOURCES += AdvancedGUI_DividedCylinderDlg.h AdvancedGUI_DividedCylinderDlg.cxx
##@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@##

dist_libAdvancedGUI_la_SOURCES += $(ADVANCED_SOURCES)

MOC_FILES =

ADVANCED_MOC_FILES  =
ADVANCED_MOC_FILES += AdvancedGUI_PipeTShapeDlg_moc.cxx
ADVANCED_MOC_FILES += AdvancedGUI_DividedDiskDlg_moc.cxx
ADVANCED_MOC_FILES += AdvancedGUI_DividedCylinderDlg_moc.cxx
##@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@##

MOC_FILES += $(ADVANCED_MOC_FILES)

nodist_libAdvancedGUI_la_SOURCES =	\
	$(MOC_FILES)

# additional information to compile and link file

libAdvancedGUI_la_CPPFLAGS =			\
	$(QT_INCLUDES) 				\
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

libAdvancedGUI_la_LDFLAGS = 					\
	../GEOMFiltersSelection/libGEOMFiltersSelection.la	\
	../GEOMBase/libGEOMBase.la
