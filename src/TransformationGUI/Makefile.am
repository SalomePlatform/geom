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
# Package : TransformationGUI

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =					\
	TransformationGUI.h				\
	TransformationGUI_MultiTranslationDlg.h		\
	TransformationGUI_MultiRotationDlg.h		\
	TransformationGUI_TranslationDlg.h		\
	TransformationGUI_RotationDlg.h			\
	TransformationGUI_MirrorDlg.h			\
	TransformationGUI_ScaleDlg.h			\
	TransformationGUI_OffsetDlg.h			\
	TransformationGUI_ProjectionDlg.h		\
	TransformationGUI_PositionDlg.h

# Libraries targets
lib_LTLIBRARIES = libTransformationGUI.la

dist_libTransformationGUI_la_SOURCES =			\
	TransformationGUI.h				\
	TransformationGUI_MultiTranslationDlg.h		\
	TransformationGUI_MultiRotationDlg.h		\
	TransformationGUI_TranslationDlg.h		\
	TransformationGUI_RotationDlg.h			\
	TransformationGUI_MirrorDlg.h			\
	TransformationGUI_ScaleDlg.h			\
	TransformationGUI_OffsetDlg.h			\
	TransformationGUI_ProjectionDlg.h		\
	TransformationGUI_PositionDlg.h			\
							\
	TransformationGUI.cxx				\
	TransformationGUI_MultiTranslationDlg.cxx	\
	TransformationGUI_MultiRotationDlg.cxx		\
	TransformationGUI_TranslationDlg.cxx		\
	TransformationGUI_RotationDlg.cxx		\
	TransformationGUI_MirrorDlg.cxx			\
	TransformationGUI_ScaleDlg.cxx			\
	TransformationGUI_OffsetDlg.cxx			\
	TransformationGUI_ProjectionDlg.cxx		\
	TransformationGUI_PositionDlg.cxx

MOC_FILES =						\
	TransformationGUI_MultiTranslationDlg_moc.cxx	\
	TransformationGUI_MultiRotationDlg_moc.cxx	\
	TransformationGUI_TranslationDlg_moc.cxx	\
	TransformationGUI_RotationDlg_moc.cxx		\
	TransformationGUI_MirrorDlg_moc.cxx		\
	TransformationGUI_ScaleDlg_moc.cxx		\
	TransformationGUI_OffsetDlg_moc.cxx		\
	TransformationGUI_ProjectionDlg_moc.cxx		\
	TransformationGUI_PositionDlg_moc.cxx

nodist_libTransformationGUI_la_SOURCES =		\
	$(MOC_FILES)

# additional information to compile and link file

libTransformationGUI_la_CPPFLAGS =		\
	$(QT_INCLUDES)				\
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

libTransformationGUI_la_LDFLAGS  =				\
	../GEOMFiltersSelection/libGEOMFiltersSelection.la	\
	../GEOMBase/libGEOMBase.la				\
	$(CAS_LDPATH) -lTKShHealing
