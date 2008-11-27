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
#  GEOM BUILDGUI : 
#  File   : Makefile.in
#  Author : Julia DOROVSKIKH (OCC)
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#  $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 

# Libraries targets
lib_LTLIBRARIES = libBlocksGUI.la 

# Sources files
dist_libBlocksGUI_la_SOURCES = \
	BlocksGUI.cxx \
	BlocksGUI_QuadFaceDlg.cxx \
	BlocksGUI_BlockDlg.cxx \
	BlocksGUI_ExplodeDlg.cxx \
	BlocksGUI_PropagateDlg.cxx \
	BlocksGUI_TrsfDlg.cxx

MOC_FILES = \
	BlocksGUI_QuadFaceDlg_moc.cxx \
	BlocksGUI_BlockDlg_moc.cxx \
	BlocksGUI_ExplodeDlg_moc.cxx \
	BlocksGUI_PropagateDlg_moc.cxx \
	BlocksGUI_TrsfDlg_moc.cxx

nodist_libBlocksGUI_la_SOURCES= \
	$(MOC_FILES)

#LIB_CLIENT_IDL = SALOME_Exception.idl SALOME_GenericObj.idl SALOME_Component.idl

#LIB_SERVER_IDL =  

# additionnal information to compil and link file

libBlocksGUI_la_CPPFLAGS = \
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
	-I$(srcdir)/../DlgRef \
	-I$(srcdir)/../GEOMBase \
	-I$(srcdir)/../OBJECT \
	-I$(srcdir)/../GEOMClient \
	-I$(srcdir)/../GEOMImpl \
	-I$(srcdir)/../GEOMFiltersSelection \
	-I$(top_builddir)/idl \
	-I$(top_builddir)/salome_adm/unix

libBlocksGUI_la_LDFLAGS  = \
	../GEOMFiltersSelection/libGEOMFiltersSelection.la \
	../DlgRef/libDlgRef.la \
	../GEOMBase/libGEOMBase.la \
	../GEOMGUI/libGEOM.la \
	$(QT_MT_LIBS)
