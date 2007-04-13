#  GEOM TRANSFORMATIONGUI : 
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
lib_LTLIBRARIES = libTransformationGUI.la

dist_libTransformationGUI_la_SOURCES = \
	TransformationGUI.cxx \
	TransformationGUI_MultiTranslationDlg.cxx \
	TransformationGUI_MultiRotationDlg.cxx \
	TransformationGUI_TranslationDlg.cxx \
	TransformationGUI_RotationDlg.cxx \
	TransformationGUI_MirrorDlg.cxx \
	TransformationGUI_ScaleDlg.cxx \
	TransformationGUI_OffsetDlg.cxx \
	TransformationGUI_PositionDlg.cxx

MOC_FILES = \
	TransformationGUI_MultiTranslationDlg_moc.cxx \
	TransformationGUI_MultiRotationDlg_moc.cxx \
	TransformationGUI_TranslationDlg_moc.cxx \
	TransformationGUI_RotationDlg_moc.cxx \
	TransformationGUI_MirrorDlg_moc.cxx \
	TransformationGUI_ScaleDlg_moc.cxx \
	TransformationGUI_OffsetDlg_moc.cxx \
	TransformationGUI_PositionDlg_moc.cxx

nodist_libTransformationGUI_la_SOURCES= \
	$(MOC_FILES)

#LIB_CLIENT_IDL = SALOME_GenericObj.idl SALOME_Component.idl SALOME_Exception.idl

#LIB_SERVER_IDL = 

# additionnal information to compil and link file

libTransformationGUI_la_CPPFLAGS = \
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

libTransformationGUI_la_LDFLAGS  = \
	../GEOMFiltersSelection/libGEOMFiltersSelection.la \
	../GEOMBase/libGEOMBase.la
