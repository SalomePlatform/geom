#  GEOM MEASUREGUI : 
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
lib_LTLIBRARIES = libMeasureGUI.la

dist_libMeasureGUI_la_SOURCES = \
	MeasureGUI.cxx \
	MeasureGUI_Skeleton_QTD.cxx \
	MeasureGUI_1Sel3LineEdit_QTD.cxx \
	MeasureGUI_1Sel6LineEdit_QTD.cxx \
	MeasureGUI_1Sel12LineEdit_QTD.cxx \
	MeasureGUI_1Sel1TextView_QTD.cxx \
	MeasureGUI_2Sel1LineEdit_QTD.cxx \
	MeasureGUI_2Sel4LineEdit_QTD.cxx \
	MeasureGUI_Skeleton.cxx \
	MeasureGUI_PropertiesDlg.cxx \
	MeasureGUI_CenterMassDlg.cxx \
	MeasureGUI_NormaleDlg.cxx \
	MeasureGUI_InertiaDlg.cxx \
	MeasureGUI_BndBoxDlg.cxx \
	MeasureGUI_DistanceDlg.cxx \
	MeasureGUI_AngleDlg.cxx \
	MeasureGUI_MaxToleranceDlg.cxx \
	MeasureGUI_WhatisDlg.cxx \
	MeasureGUI_CheckShapeDlg.cxx \
	MeasureGUI_CheckCompoundOfBlocksDlg.cxx \
	MeasureGUI_PointDlg.cxx

MOC_FILES = \
	MeasureGUI_Skeleton_QTD_moc.cxx \
	MeasureGUI_1Sel3LineEdit_QTD_moc.cxx \
	MeasureGUI_1Sel6LineEdit_QTD_moc.cxx \
	MeasureGUI_1Sel12LineEdit_QTD_moc.cxx \
	MeasureGUI_1Sel1TextView_QTD_moc.cxx \
	MeasureGUI_2Sel1LineEdit_QTD_moc.cxx \
	MeasureGUI_2Sel4LineEdit_QTD_moc.cxx \
	MeasureGUI_Skeleton_moc.cxx \
	MeasureGUI_PropertiesDlg_moc.cxx \
	MeasureGUI_CenterMassDlg_moc.cxx \
	MeasureGUI_NormaleDlg_moc.cxx \
	MeasureGUI_InertiaDlg_moc.cxx \
	MeasureGUI_BndBoxDlg_moc.cxx \
	MeasureGUI_DistanceDlg_moc.cxx \
	MeasureGUI_AngleDlg_moc.cxx \
	MeasureGUI_MaxToleranceDlg_moc.cxx \
	MeasureGUI_WhatisDlg_moc.cxx \
	MeasureGUI_CheckShapeDlg_moc.cxx \
	MeasureGUI_CheckCompoundOfBlocksDlg_moc.cxx \
	MeasureGUI_PointDlg_moc.cxx    

nodist_libMeasureGUI_la_SOURCES= \
	$(MOC_FILES)

#LIB_CLIENT_IDL = SALOME_Exception.idl SALOME_GenericObj.idl SALOME_Component.idl

#LIB_SERVER_IDL =  

# additionnal information to compil and link file

libMeasureGUI_la_CPPFLAGS = \
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
	-I$(srcdir)/../GEOM \
	-I$(top_builddir)/idl \
	-I$(top_builddir)/salome_adm/unix

libMeasureGUI_la_LDFLAGS  = \
	../GEOMBase/libGEOMBase.la

