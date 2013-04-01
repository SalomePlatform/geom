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

# GEOM MEASUREGUI : 
# File    : Makefile.am
# Author  : Alexander BORODIN, Open CASCADE S.A.S. (alexander.borodin@opencascade.com)
# Package : MeasureGUI

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =				\
	MeasureGUI.h				\
	MeasureGUI_Widgets.h			\
	MeasureGUI_Skeleton.h			\
	MeasureGUI_PropertiesDlg.h		\
	MeasureGUI_CenterMassDlg.h		\
	MeasureGUI_NormaleDlg.h			\
	MeasureGUI_InertiaDlg.h			\
	MeasureGUI_BndBoxDlg.h			\
	MeasureGUI_DistanceDlg.h		\
	MeasureGUI_AngleDlg.h			\
	MeasureGUI_MaxToleranceDlg.h		\
	MeasureGUI_WhatisDlg.h			\
	MeasureGUI_CheckShapeDlg.h		\
	MeasureGUI_CheckCompoundOfBlocksDlg.h	\
	MeasureGUI_GetNonBlocksDlg.h		\
	MeasureGUI_CheckSelfIntersectionsDlg.h	\
	MeasureGUI_PointDlg.h

# Libraries targets
lib_LTLIBRARIES = libMeasureGUI.la

dist_libMeasureGUI_la_SOURCES =			\
	MeasureGUI.cxx				\
	MeasureGUI_Widgets.cxx			\
	MeasureGUI_Skeleton.cxx			\
	MeasureGUI_PropertiesDlg.cxx		\
	MeasureGUI_CenterMassDlg.cxx		\
	MeasureGUI_NormaleDlg.cxx		\
	MeasureGUI_InertiaDlg.cxx		\
	MeasureGUI_BndBoxDlg.cxx		\
	MeasureGUI_DistanceDlg.cxx		\
	MeasureGUI_AngleDlg.cxx			\
	MeasureGUI_MaxToleranceDlg.cxx		\
	MeasureGUI_WhatisDlg.cxx		\
	MeasureGUI_CheckShapeDlg.cxx		\
	MeasureGUI_CheckCompoundOfBlocksDlg.cxx	\
	MeasureGUI_GetNonBlocksDlg.cxx		\
	MeasureGUI_CheckSelfIntersectionsDlg.cxx \
	MeasureGUI_PointDlg.cxx

MOC_FILES = 						\
	MeasureGUI_Widgets_moc.cxx			\
	MeasureGUI_Skeleton_moc.cxx			\
	MeasureGUI_PropertiesDlg_moc.cxx		\
	MeasureGUI_CenterMassDlg_moc.cxx		\
	MeasureGUI_NormaleDlg_moc.cxx			\
	MeasureGUI_InertiaDlg_moc.cxx			\
	MeasureGUI_BndBoxDlg_moc.cxx			\
	MeasureGUI_DistanceDlg_moc.cxx			\
	MeasureGUI_AngleDlg_moc.cxx			\
	MeasureGUI_MaxToleranceDlg_moc.cxx		\
	MeasureGUI_WhatisDlg_moc.cxx			\
	MeasureGUI_CheckShapeDlg_moc.cxx		\
	MeasureGUI_CheckCompoundOfBlocksDlg_moc.cxx	\
	MeasureGUI_GetNonBlocksDlg_moc.cxx		\
	MeasureGUI_CheckSelfIntersectionsDlg_moc.cxx	\
	MeasureGUI_PointDlg_moc.cxx

nodist_libMeasureGUI_la_SOURCES =			\
	$(MOC_FILES)

UIC_FILES =						\
	ui_MeasureGUI_1Sel12LineEdit_QTD.h		\
	ui_MeasureGUI_1Sel1TextView1Check_QTD.h		\
	ui_MeasureGUI_1Sel1TextView2ListBox_QTD.h	\
	ui_MeasureGUI_1Sel1TextView_QTD.h		\
	ui_MeasureGUI_1Sel3LineEdit_QTD.h		\
	ui_MeasureGUI_1Sel6LineEdit_QTD.h		\
	ui_MeasureGUI_2Sel1LineEdit_QTD.h		\
	ui_MeasureGUI_SkeletonBox_QTD.h

BUILT_SOURCES = $(UIC_FILES)

# additional information to compile and link file

libMeasureGUI_la_CPPFLAGS =		\
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
	-I$(srcdir)/../GEOM             \
	-I$(top_builddir)/src/DlgRef	\
	-I$(top_builddir)/idl

libMeasureGUI_la_LDFLAGS  =		\
	$(CAS_LDFLAGS) -lTKGeomBase \
	../GEOMBase/libGEOMBase.la ../DlgRef/libDlgRef.la
