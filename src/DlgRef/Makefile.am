# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

# File    : Makefile.am
# Author  : Alexander BORODIN, Open CASCADE S.A.S. (alexander.borodin@opencascade.com)
# Package : DlgRef

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libDlgRef.la

# Sources files
dist_libDlgRef_la_SOURCES = 	\
	DlgRef.h		\
	DlgRef.cxx

MOC_FILES =			\
	DlgRef_moc.cxx

UIC_FILES =					\
	ui_DlgRef_1List1Spin1Btn_QTD.h		\
	ui_DlgRef_1Sel1Check1List_QTD.h		\
	ui_DlgRef_1Sel1Check1Sel_QTD.h		\
	ui_DlgRef_1Sel1Check_QTD.h		\
	ui_DlgRef_1Sel1Frame_QTD.h		\
	ui_DlgRef_1Sel1List1Check3Btn_QTD.h	\
	ui_DlgRef_1Sel1Spin1Check_QTD.h		\
	ui_DlgRef_1Sel1Spin_QTD.h		\
	ui_DlgRef_1Sel2Spin1View1Check_QTD.h	\
	ui_DlgRef_1Sel2Spin_QTD.h		\
	ui_DlgRef_1Sel3Check_QTD.h		\
	ui_DlgRef_1Sel3Spin_QTD.h		\
	ui_DlgRef_1Sel3Spin1Check_QTD.h		\
	ui_DlgRef_1Sel3Spin2Check1Spin_QTD.h	\
	ui_DlgRef_1Sel5Spin1Check_QTD.h		\
	ui_DlgRef_1Sel_QTD.h			\
	ui_DlgRef_1SelExt_QTD.h			\
	ui_DlgRef_1Spin_QTD.h			\
	ui_DlgRef_2Sel1List1Check_QTD.h		\
	ui_DlgRef_2Sel1List2Check_QTD.h		\
	ui_DlgRef_2Sel1List_QTD.h		\
	ui_DlgRef_2Sel1Spin2Check_QTD.h		\
	ui_DlgRef_2Sel1Spin3Check1Spin_QTD.h	\
	ui_DlgRef_2Sel1Spin_QTD.h		\
	ui_DlgRef_2Sel1SpinInt_QTD.h		\
	ui_DlgRef_2Sel2List_QTD.h		\
	ui_DlgRef_2Sel2Spin1Check_QTD.h		\
	ui_DlgRef_2Sel2Spin3Check_QTD.h		\
	ui_DlgRef_2Sel2Spin_QTD.h		\
	ui_DlgRef_2Sel2Spin1Push_QTD.h		\
	ui_DlgRef_2Sel2Spin2Push_QTD.h		\
	ui_DlgRef_2Sel3Spin2Rb_QTD.h		\
	ui_DlgRef_2Sel3Spin_QTD.h		\
	ui_DlgRef_2Sel_QTD.h			\
	ui_DlgRef_2SelExt_QTD.h			\
	ui_DlgRef_2Spin_QTD.h			\
	ui_DlgRef_3Check_QTD.h			\
	ui_DlgRef_3Radio_QTD.h			\
	ui_DlgRef_3Radio1Sel1Spin_QTD.h		\
	ui_DlgRef_3Sel1Check_QTD.h		\
	ui_DlgRef_3Sel2Check3Spin_QTD.h		\
	ui_DlgRef_3Sel1Spin_QTD.h		\
	ui_DlgRef_3Sel2Spin_QTD.h		\
	ui_DlgRef_3Sel3Spin1Check_QTD.h		\
	ui_DlgRef_3Sel3Spin2Check_QTD.h		\
	ui_DlgRef_3Sel4Spin2Check_QTD.h		\
	ui_DlgRef_3Sel_QTD.h			\
	ui_DlgRef_3Spin1Check_QTD.h		\
	ui_DlgRef_3Spin_QTD.h			\
	ui_DlgRef_4Sel1List1Check_QTD.h		\
	ui_DlgRef_4Sel1List_QTD.h		\
	ui_DlgRef_4Sel1Spin2Check_QTD.h		\
	ui_DlgRef_4Sel1Spin3Check_QTD.h		\
	ui_DlgRef_4Spin_QTD.h			\
	ui_DlgRef_6Sel_QTD.h			\
	ui_DlgRef_Skeleton_QTD.h

ADVANCED_UIC_FILES  = 
##@@ insert new functions before this line @@ do not remove this line @@##

UIC_FILES += $(ADVANCED_UIC_FILES)

BUILT_SOURCES = $(UIC_FILES)

nodist_libDlgRef_la_SOURCES =	\
	$(MOC_FILES)		\
	$(UIC_FILES)

# additional information to compile and link file

libDlgRef_la_CPPFLAGS =		\
	$(QT_INCLUDES)		\
	$(GUI_CXXFLAGS)

libDlgRef_la_LDFLAGS  =		\
	$(STDLIB)		\
	$(QT_MT_LIBS)           \
	$(GUI_LDFLAGS) -lqtx -lSalomeApp
