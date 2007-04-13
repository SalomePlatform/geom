#  GEOM BUILDGUI : 
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#
#
#  File   : Makefile.in
#  Author : Damien COQUERET (OCC)
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#  $Header: 

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =

# Libraries targets
lib_LTLIBRARIES = libBuildGUI.la 

# Sources files
dist_libBuildGUI_la_SOURCES = \
	BuildGUI.cxx \
	BuildGUI_EdgeDlg.cxx \
	BuildGUI_WireDlg.cxx \
	BuildGUI_FaceDlg.cxx \
	BuildGUI_ShellDlg.cxx \
	BuildGUI_SolidDlg.cxx \
	BuildGUI_CompoundDlg.cxx

MOC_FILES = \
	BuildGUI_EdgeDlg_moc.cxx \
	BuildGUI_WireDlg_moc.cxx \
	BuildGUI_FaceDlg_moc.cxx \
	BuildGUI_ShellDlg_moc.cxx \
	BuildGUI_SolidDlg_moc.cxx \
	BuildGUI_CompoundDlg_moc.cxx

nodist_libBuildGUI_la_SOURCES= \
	$(MOC_FILES)

# LIB_CLIENT_IDL = SALOME_Exception.idl SALOME_GenericObj.idl SALOME_Component.idl

# LIB_SERVER_IDL =  

# additionnal information to compil and link file

libBuildGUI_la_CPPFLAGS = \
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

libBuildGUI_la_LDFLAGS  = \
	../GEOMFiltersSelection/libGEOMFiltersSelection.la \
	../GEOMBase/libGEOMBase.la
