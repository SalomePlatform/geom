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
# Package : BasicGUI
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 


salomeinclude_HEADERS =			\
	BasicGUI.h			\
	BasicGUI_ArcDlg.h		\
	BasicGUI_CircleDlg.h		\
	BasicGUI_CurveDlg.h		\
	BasicGUI_EllipseDlg.h		\
	BasicGUI_LineDlg.h		\
	BasicGUI_MarkerDlg.h		\
	BasicGUI_PlaneDlg.h		\
	BasicGUI_PointDlg.h		\
	BasicGUI_VectorDlg.h            \
	BasicGUI_ParamCurveWidget.h
#
# OBSOLETE: BasicGUI_WorkingPlaneDlg.h
#

# Libraries targets

lib_LTLIBRARIES = libBasicGUI.la

# Sources files
dist_libBasicGUI_la_SOURCES =		\
	BasicGUI.cxx			\
	BasicGUI_PointDlg.cxx		\
	BasicGUI_LineDlg.cxx		\
	BasicGUI_CircleDlg.cxx		\
	BasicGUI_EllipseDlg.cxx		\
	BasicGUI_ArcDlg.cxx		\
	BasicGUI_VectorDlg.cxx		\
	BasicGUI_PlaneDlg.cxx		\
	BasicGUI_CurveDlg.cxx		\
	BasicGUI_MarkerDlg.cxx          \
	BasicGUI_ParamCurveWidget.cxx
#
# OBSOLETE: BasicGUI_WorkingPlaneDlg.cxx
#

MOC_FILES =					\
	BasicGUI_PointDlg_moc.cxx		\
	BasicGUI_LineDlg_moc.cxx		\
	BasicGUI_CircleDlg_moc.cxx		\
	BasicGUI_EllipseDlg_moc.cxx		\
	BasicGUI_ArcDlg_moc.cxx			\
	BasicGUI_VectorDlg_moc.cxx		\
	BasicGUI_PlaneDlg_moc.cxx		\
	BasicGUI_CurveDlg_moc.cxx		\
	BasicGUI_MarkerDlg_moc.cxx		\
	BasicGUI_ParamCurveWidget_moc.cxx
#
# OBSOLETE: BasicGUI_WorkingPlaneDlg_moc.cxx
#

nodist_libBasicGUI_la_SOURCES =			\
	$(MOC_FILES)

# additional information to compile and link file

libBasicGUI_la_CPPFLAGS =			\
	$(QT_INCLUDES)				\
	$(VTK_INCLUDES)				\
	$(CAS_CPPFLAGS)				\
	$(PYTHON_INCLUDES)			\
	$(BOOST_CPPFLAGS)			\
	$(KERNEL_CXXFLAGS)			\
	$(GUI_CXXFLAGS)				\
	$(CORBA_CXXFLAGS)			\
	$(CORBA_INCLUDES)			\
	$(OPENCV_INCLUDES)			\
	-I$(srcdir)/../OBJECT			\
	-I$(srcdir)/../DlgRef			\
	-I$(srcdir)/../GEOMGUI			\
	-I$(srcdir)/../GEOMFiltersSelection	\
	-I$(srcdir)/../GEOMBase			\
	-I$(srcdir)/../GEOMImpl			\
	-I$(srcdir)/../GEOMClient		\
	-I$(top_builddir)/src/DlgRef		\
	-I$(top_builddir)/idl


libBasicGUI_la_LDFLAGS  =					\
	../GEOMFiltersSelection/libGEOMFiltersSelection.la	\
	../GEOMBase/libGEOMBase.la				\
	../GEOMGUI/libGEOM.la					\
	$(CAS_LDFLAGS) -lTKGeomBase                             \
	$(GUI_LDFLAGS) -lsuit					\
	$(OPENCV_LIBS) 
