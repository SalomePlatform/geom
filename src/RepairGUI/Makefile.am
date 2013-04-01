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
# Package : RepairGUI

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =				\
	RepairGUI.h				\
	RepairGUI_SewingDlg.h			\
	RepairGUI_SuppressFacesDlg.h		\
	RepairGUI_ShapeProcessDlg.h		\
	RepairGUI_CloseContourDlg.h		\
	RepairGUI_RemoveIntWiresDlg.h		\
	RepairGUI_RemoveHolesDlg.h		\
	RepairGUI_DivideEdgeDlg.h		\
	RepairGUI_FreeBoundDlg.h		\
	RepairGUI_FreeFacesDlg.h		\
	RepairGUI_ChangeOrientationDlg.h	\
	RepairGUI_GlueDlg.h			\
	RepairGUI_LimitToleranceDlg.h		\
	RepairGUI_RemoveExtraEdgesDlg.h		\
	RepairGUI_FuseEdgesDlg.h

# Libraries targets
lib_LTLIBRARIES = libRepairGUI.la

dist_libRepairGUI_la_SOURCES =			\
	RepairGUI.h				\
	RepairGUI_SewingDlg.h			\
	RepairGUI_SuppressFacesDlg.h		\
	RepairGUI_ShapeProcessDlg.h		\
	RepairGUI_CloseContourDlg.h		\
	RepairGUI_RemoveIntWiresDlg.h		\
	RepairGUI_RemoveHolesDlg.h		\
	RepairGUI_DivideEdgeDlg.h		\
	RepairGUI_FreeBoundDlg.h		\
	RepairGUI_FreeFacesDlg.h		\
	RepairGUI_ChangeOrientationDlg.h	\
	RepairGUI_GlueDlg.h			\
	RepairGUI_LimitToleranceDlg.h		\
	RepairGUI_RemoveExtraEdgesDlg.h		\
	RepairGUI_FuseEdgesDlg.h		\
						\
	RepairGUI.cxx				\
	RepairGUI_SewingDlg.cxx			\
	RepairGUI_SuppressFacesDlg.cxx		\
	RepairGUI_ShapeProcessDlg.cxx		\
	RepairGUI_CloseContourDlg.cxx		\
	RepairGUI_RemoveIntWiresDlg.cxx		\
	RepairGUI_RemoveHolesDlg.cxx		\
	RepairGUI_DivideEdgeDlg.cxx		\
	RepairGUI_FreeBoundDlg.cxx		\
	RepairGUI_FreeFacesDlg.cxx		\
	RepairGUI_ChangeOrientationDlg.cxx	\
	RepairGUI_GlueDlg.cxx			\
	RepairGUI_LimitToleranceDlg.cxx		\
	RepairGUI_RemoveExtraEdgesDlg.cxx	\
	RepairGUI_FuseEdgesDlg.cxx

MOC_FILES = 					\
	RepairGUI_SewingDlg_moc.cxx		\
	RepairGUI_SuppressFacesDlg_moc.cxx	\
	RepairGUI_ShapeProcessDlg_moc.cxx	\
	RepairGUI_CloseContourDlg_moc.cxx	\
	RepairGUI_RemoveIntWiresDlg_moc.cxx	\
	RepairGUI_RemoveHolesDlg_moc.cxx	\
	RepairGUI_DivideEdgeDlg_moc.cxx 	\
	RepairGUI_FreeBoundDlg_moc.cxx		\
	RepairGUI_FreeFacesDlg_moc.cxx		\
	RepairGUI_ChangeOrientationDlg_moc.cxx	\
	RepairGUI_GlueDlg_moc.cxx     		\
	RepairGUI_LimitToleranceDlg_moc.cxx	\
	RepairGUI_RemoveExtraEdgesDlg_moc.cxx	\
	RepairGUI_FuseEdgesDlg_moc.cxx

nodist_libRepairGUI_la_SOURCES =		\
	$(MOC_FILES)

# additional information to compile and link file

libRepairGUI_la_CPPFLAGS =			\
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
	-I$(top_builddir)/src/DlgRef		\
	-I$(top_builddir)/idl

libRepairGUI_la_LDFLAGS = \
	../GEOMBase/libGEOMBase.la
