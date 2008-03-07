#  GEOM ENTITYGUI : 
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

# Libraries targets
lib_LTLIBRARIES = libEntityGUI.la

# header files
salomeinclude_HEADERS = \
	GEOM_EntityGUI.hxx

dist_libEntityGUI_la_SOURCES = EntityGUI.cxx \
	EntityGUI_Skeleton_QTD.cxx \
	EntityGUI_Point_QTD.cxx \
	EntityGUI_Dir1_QTD.cxx \
	EntityGUI_Dir2_QTD.cxx \
	EntityGUI_1Sel_QTD.cxx \
	EntityGUI_1Spin_QTD.cxx \
	EntityGUI_2Spin_QTD.cxx \
	EntityGUI_3Spin_QTD.cxx \
	EntityGUI_4Spin_QTD.cxx \
	EntityGUI_1Spin.cxx \
	EntityGUI_2Spin.cxx \
	EntityGUI_3Spin.cxx \
	EntityGUI_4Spin.cxx \
	EntityGUI_SketcherDlg.cxx \
	EntityGUI_SubShapeDlg.cxx

MOC_FILES = \
	EntityGUI_Skeleton_QTD_moc.cxx \
	EntityGUI_Point_QTD_moc.cxx \
	EntityGUI_Dir1_QTD_moc.cxx \
	EntityGUI_Dir2_QTD_moc.cxx \
	EntityGUI_1Sel_QTD_moc.cxx \
	EntityGUI_1Spin_QTD_moc.cxx \
	EntityGUI_2Spin_QTD_moc.cxx \
	EntityGUI_3Spin_QTD_moc.cxx \
	EntityGUI_4Spin_QTD_moc.cxx \
	EntityGUI_1Spin_moc.cxx \
	EntityGUI_2Spin_moc.cxx \
	EntityGUI_3Spin_moc.cxx \
	EntityGUI_4Spin_moc.cxx \
	EntityGUI_SketcherDlg_moc.cxx \
	EntityGUI_SubShapeDlg_moc.cxx

nodist_libEntityGUI_la_SOURCES= \
	 $(MOC_FILES)

# LIB_CLIENT_IDL = SALOME_Exception.idl SALOME_GenericObj.idl SALOME_Component.idl

# LIB_SERVER_IDL =  

# additionnal information to compil and link file

libEntityGUI_la_CPPFLAGS = \
	$(QT_INCLUDES) \
	$(CAS_CPPFLAGS) \
	$(VTK_INCLUDES) \
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
	-I$(srcdir)/../SKETCHER \
	-I$(top_builddir)/idl \
	-I$(top_builddir)/salome_adm/unix

libEntityGUI_la_LDFLAGS  = \
	../GEOMFiltersSelection/libGEOMFiltersSelection.la \
	../GEOMBase/libGEOMBase.la \
	../SKETCHER/libGEOMSketcher.la \
	../GEOMGUI/libGEOM.la \
	$(QT_MT_LIBS) \
	$(CAS_TKTopAlgo) -lTKernel

