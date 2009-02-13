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
# GEOM OPERATIONGUI : 
# File    : Makefile.am
# Author  : Alexander BORODIN, Open CASCADE S.A.S. (alexander.borodin@opencascade.com)
# Package : OperationGUI
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =				\
	OperationGUI.h				\
	OperationGUI_ArchimedeDlg.h		\
	OperationGUI_PartitionDlg.h		\
	OperationGUI_FilletDlg.h		\
	OperationGUI_Fillet2dDlg.h		\
	OperationGUI_ChamferDlg.h		\
	OperationGUI_GetShapesOnShapeDlg.h	\
	OperationGUI_ClippingDlg.h

# Libraries targets
lib_LTLIBRARIES = libOperationGUI.la

dist_libOperationGUI_la_SOURCES =		\
	OperationGUI.cxx			\
	OperationGUI_ArchimedeDlg.cxx		\
	OperationGUI_PartitionDlg.cxx		\
	OperationGUI_GetShapesOnShapeDlg.cxx	\
	OperationGUI_FilletDlg.cxx		\
	OperationGUI_Fillet2dDlg.cxx		\
	OperationGUI_ChamferDlg.cxx		\
	OperationGUI_ClippingDlg.cxx

MOC_FILES =					\
	OperationGUI_ArchimedeDlg_moc.cxx	\
	OperationGUI_PartitionDlg_moc.cxx	\
	OperationGUI_GetShapesOnShapeDlg_moc.cxx\
	OperationGUI_FilletDlg_moc.cxx		\
	OperationGUI_Fillet2dDlg_moc.cxx	\
	OperationGUI_ChamferDlg_moc.cxx		\
	OperationGUI_ClippingDlg_moc.cxx

nodist_libOperationGUI_la_SOURCES =		\
	$(MOC_FILES)

# additional information to compile and link file

libOperationGUI_la_CPPFLAGS =		\
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
	-I$(srcdir)/../DlgRef		\
	-I$(srcdir)/../GEOMBase		\
	-I$(srcdir)/../OBJECT		\
	-I$(srcdir)/../GEOMClient	\
	-I$(srcdir)/../GEOMImpl		\
	-I$(srcdir)/../GEOMAlgo		\
	-I$(top_builddir)/src/DlgRef	\
	-I$(top_builddir)/idl		\
	-I$(top_builddir)/salome_adm/unix

libOperationGUI_la_LDFLAGS =		\
	../GEOMBase/libGEOMBase.la	\
	$(CAS_LDPATH) -lTKFillet

###############################
# Obsolete files ?
###############################
# OperationGUI_MaterialDlg.h
# OperationGUI_MaterialDlg.cxx
