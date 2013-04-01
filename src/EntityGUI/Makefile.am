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

# GEOM ENTITYGUI : 
# File    : Makefile.am
# Author  : Alexander BORODIN, Open CASCADE S.A.S. (alexander.borodin@opencascade.com)
# Package : EntityGUI
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

if WITH_OPENCV
  FeatureDetectorDlg_Header = EntityGUI_FeatureDetectorDlg.h
  FeatureDetectorDlg_Sources = EntityGUI_FeatureDetectorDlg.cxx
  FeatureDetectorDlg_moc = EntityGUI_FeatureDetectorDlg_moc.cxx
  Opencv_libs = $(OPENCV_LIBS)
  Opencv_includes = $(OPENCV_INCLUDES)
  ShapeRec_dir = $(srcdir)/../ShapeRecognition
  ShapeRec_la = ../ShapeRecognition/libGEOMShapeRec.la  
endif

PictureImportDlg_Header = EntityGUI_PictureImportDlg.h
PictureImportDlg_Sources = EntityGUI_PictureImportDlg.cxx
PictureImportDlg_moc = EntityGUI_PictureImportDlg_moc.cxx

# Libraries targets
lib_LTLIBRARIES = libEntityGUI.la

# header files
salomeinclude_HEADERS =			\
	EntityGUI.h			\
	EntityGUI_Widgets.h		\
	EntityGUI_SketcherDlg.h		\
	EntityGUI_3DSketcherDlg.h	\
	EntityGUI_SubShapeDlg.h		\
	$(FeatureDetectorDlg_Header)	\
	$(PictureImportDlg_Header)

dist_libEntityGUI_la_SOURCES = 		\
	EntityGUI.cxx			\
	EntityGUI_Widgets.cxx		\
	EntityGUI_SketcherDlg.cxx	\
	EntityGUI_3DSketcherDlg.cxx	\
	EntityGUI_SubShapeDlg.cxx	\
	$(FeatureDetectorDlg_Sources)	\
	$(PictureImportDlg_Sources)

MOC_FILES =				\
	EntityGUI_Widgets_moc.cxx	\
	EntityGUI_SketcherDlg_moc.cxx	\
	EntityGUI_3DSketcherDlg_moc.cxx	\
	EntityGUI_SubShapeDlg_moc.cxx	\
	$(FeatureDetectorDlg_moc)	\
	$(PictureImportDlg_moc)	

nodist_libEntityGUI_la_SOURCES =	\
	 $(MOC_FILES)

UIC_FILES =				\
	ui_EntityGUI_1Sel_QTD.h		\
	ui_EntityGUI_2Sel1Check_QTD.h	\
	ui_EntityGUI_1Sel1Spin1Check_QTD.h	\
	ui_EntityGUI_1Spin_QTD.h	\
	ui_EntityGUI_2Spin_QTD.h	\
	ui_EntityGUI_3Spin_QTD.h	\
	ui_EntityGUI_4Spin_QTD.h	\
	ui_EntityGUI_3Spin1Check_QTD.h	\
	ui_EntityGUI_4Spin1Check_QTD.h	\
	ui_EntityGUI_Dir1_QTD.h		\
	ui_EntityGUI_Dir2_QTD.h		\
	ui_EntityGUI_Point_QTD.h	\
	ui_EntityGUI_Skeleton_QTD.h	\
	ui_EntityGUI_Type_QTD.h		\
	ui_EntityGUI_Angles_QTD.h	\
	ui_EntityGUI_Controls_QTD.h

BUILT_SOURCES = $(UIC_FILES)

# additional information to compile and link file

libEntityGUI_la_CPPFLAGS =			\
	$(QT_INCLUDES)				\
	$(CAS_CPPFLAGS)				\
	$(VTK_INCLUDES)				\
	$(PYTHON_INCLUDES)			\
	$(BOOST_CPPFLAGS)			\
	$(KERNEL_CXXFLAGS)			\
	$(GUI_CXXFLAGS)				\
	$(CORBA_CXXFLAGS)			\
	$(CORBA_INCLUDES)			\
	$(Opencv_includes)			\
	-I$(srcdir)/../GEOMGUI			\
	-I$(srcdir)/../DlgRef			\
	-I$(srcdir)/../GEOMBase 		\
	-I$(srcdir)/../OBJECT			\
	-I$(srcdir)/../GEOMClient		\
	-I$(srcdir)/../GEOMImpl			\
	-I$(srcdir)/../GEOMFiltersSelection	\
	-I$(srcdir)/../SKETCHER			\
	-I$(ShapeRec_dir)			\
	-I$(srcdir)/../GEOM			\
	-I$(top_builddir)/src/DlgRef		\
	-I$(top_builddir)/idl


libEntityGUI_la_LDFLAGS  =					\
	../GEOMFiltersSelection/libGEOMFiltersSelection.la	\
	../GEOMBase/libGEOMBase.la				\
	../SKETCHER/libGEOMSketcher.la				\
	../GEOMGUI/libGEOM.la 					\
	$(ShapeRec_la)						\
	$(QT_MT_LIBS)						\
	$(CAS_TKTopAlgo) -lTKernel				\
	$(Opencv_libs)
