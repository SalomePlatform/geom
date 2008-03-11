#  GEOM DLGREF : 
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
lib_LTLIBRARIES = libDlgRef.la

# Sources files
dist_libDlgRef_la_SOURCES = \
	DlgRef_Skeleton_QTD.cxx \
	DlgRef_1Sel_QTD.cxx \
	DlgRef_2Sel_QTD.cxx \
	DlgRef_3Sel_QTD.cxx \
	DlgRef_1Sel1Check_QTD.cxx \
	DlgRef_1Sel3Check_QTD.cxx \
	DlgRef_1Sel1Check1List_QTD.cxx \
	DlgRef_1Sel1Spin_QTD.cxx \
	DlgRef_1Sel2Spin_QTD.cxx \
	DlgRef_1Sel3Spin_QTD.cxx \
	DlgRef_1Sel4Spin_QTD.cxx \
	DlgRef_1Sel5Spin_QTD.cxx \
	DlgRef_1Sel5Spin1Check_QTD.cxx \
	DlgRef_2Sel1Spin_QTD.cxx \
	DlgRef_2Sel2Spin_QTD.cxx \
	DlgRef_2Sel3Spin_QTD.cxx \
	DlgRef_1Sel1Spin1Check_QTD.cxx \
	DlgRef_2Sel1Spin2Check_QTD.cxx \
	DlgRef_2Sel2Spin1Check_QTD.cxx \
	DlgRef_2Sel4Spin1Check_QTD.cxx \
	DlgRef_3Sel4Spin2Check_QTD.cxx \
	DlgRef_2Sel3Spin2Rb_QTD.cxx \
	DlgRef_2Sel3Spin2Rb.cxx \
	DlgRef_4Sel1List_QTD.cxx \
	DlgRef_2Sel1List_QTD.cxx \
	DlgRef_2Sel1List1Check_QTD.cxx \
	DlgRef_1List1Spin1Btn_QTD.cxx \
	DlgRef_4Sel1List1Check_QTD.cxx \
	DlgRef_1Spin_QTD.cxx \
	DlgRef_2Spin_QTD.cxx \
	DlgRef_3Spin_QTD.cxx \
	DlgRef_3Spin1Check_QTD.cxx \
	DlgRef_3Sel3Spin1Check_QTD.cxx \
	DlgRef_3Sel3Spin2Check_QTD.cxx \	
	DlgRef_4Sel1Spin2Check_QTD.cxx \
	DlgRef_SpinBox.cxx \
	DlgRef_1Sel1Spin.cxx \
	DlgRef_1Sel2Spin.cxx \
	DlgRef_1Sel3Spin.cxx \
	DlgRef_1Sel4Spin.cxx \
	DlgRef_1Sel5Spin.cxx \
	DlgRef_1Sel5Spin1Check.cxx \
	DlgRef_2Sel1Spin.cxx \
	DlgRef_2Sel2Spin.cxx \
	DlgRef_2Sel3Spin.cxx \
	DlgRef_1Sel1Spin1Check.cxx \
	DlgRef_2Sel1Spin2Check.cxx \
	DlgRef_2Sel2Spin1Check.cxx \
	DlgRef_2Sel4Spin1Check.cxx \
	DlgRef_3Sel4Spin2Check.cxx \
	DlgRef_3Sel3Spin1Check.cxx \
	DlgRef_3Sel3Spin2Check.cxx \	
	DlgRef_1Spin.cxx \
	DlgRef_2Spin.cxx \
	DlgRef_3Spin.cxx \
	DlgRef_3Spin1Check.cxx \
	DlgRef_3Sel1Spin.cxx \
	DlgRef_3Check_QTD.cxx \
	DlgRef_6Sel_QTD.cxx \
	DlgRef_4Sel1Spin2Check.cxx \
	DlgRef_3Sel1Check_QTD.cxx

MOC_FILES = \
	DlgRef_Skeleton_QTD_moc.cxx \
	DlgRef_1Sel_QTD_moc.cxx \
	DlgRef_2Sel_QTD_moc.cxx \
	DlgRef_3Sel_QTD_moc.cxx \
	DlgRef_1Sel1Check_QTD_moc.cxx \
	DlgRef_1Sel3Check_QTD_moc.cxx \
	DlgRef_1Sel1Check1List_QTD_moc.cxx \
	DlgRef_1Sel1Spin_QTD_moc.cxx \
	DlgRef_1Sel2Spin_QTD_moc.cxx \
	DlgRef_1Sel3Spin_QTD_moc.cxx \
	DlgRef_1Sel4Spin_QTD_moc.cxx \
	DlgRef_1Sel5Spin_QTD_moc.cxx \
	DlgRef_1Sel5Spin1Check_QTD_moc.cxx \
	DlgRef_2Sel1Spin_QTD_moc.cxx \
	DlgRef_2Sel2Spin_QTD_moc.cxx \
	DlgRef_2Sel3Spin_QTD_moc.cxx \
	DlgRef_2Sel3Spin2Rb_QTD_moc.cxx \
	DlgRef_2Sel3Spin2Rb_moc.cxx \
	DlgRef_1Sel1Spin1Check_QTD_moc.cxx \
	DlgRef_2Sel1Spin2Check_QTD_moc.cxx \
	DlgRef_2Sel2Spin1Check_QTD_moc.cxx \
	DlgRef_2Sel4Spin1Check_QTD_moc.cxx \
	DlgRef_3Sel4Spin2Check_QTD_moc.cxx \
	DlgRef_4Sel1List_QTD_moc.cxx \
	DlgRef_2Sel1List_QTD_moc.cxx \
	DlgRef_2Sel1List1Check_QTD_moc.cxx \
	DlgRef_1List1Spin1Btn_QTD_moc.cxx \
	DlgRef_4Sel1List1Check_QTD_moc.cxx \
	DlgRef_1Spin_QTD_moc.cxx \
	DlgRef_2Spin_QTD_moc.cxx \
	DlgRef_3Spin_QTD_moc.cxx \
	DlgRef_3Spin1Check_QTD_moc.cxx \
	DlgRef_3Sel3Spin1Check_QTD_moc.cxx \
	DlgRef_3Sel3Spin2Check_QTD_moc.cxx \	
	DlgRef_4Sel1Spin2Check_QTD_moc.cxx \
	DlgRef_SpinBox_moc.cxx \
	DlgRef_1Sel1Spin_moc.cxx \
	DlgRef_1Sel2Spin_moc.cxx \
	DlgRef_1Sel3Spin_moc.cxx \
	DlgRef_1Sel4Spin_moc.cxx \
	DlgRef_1Sel5Spin_moc.cxx \
	DlgRef_1Sel5Spin1Check_moc.cxx \
	DlgRef_2Sel1Spin_moc.cxx \
	DlgRef_2Sel2Spin_moc.cxx \
	DlgRef_2Sel3Spin_moc.cxx \
	DlgRef_1Sel1Spin1Check_moc.cxx \
	DlgRef_2Sel1Spin2Check_moc.cxx \
	DlgRef_2Sel2Spin1Check_moc.cxx \
	DlgRef_2Sel4Spin1Check_moc.cxx \
	DlgRef_3Sel4Spin2Check_moc.cxx \
	DlgRef_3Sel3Spin1Check_moc.cxx \
	DlgRef_3Sel3Spin2Check_moc.cxx \	
	DlgRef_1Spin_moc.cxx \
	DlgRef_2Spin_moc.cxx \
	DlgRef_3Spin_moc.cxx \
	DlgRef_3Spin1Check_moc.cxx \
	DlgRef_3Sel1Spin_moc.cxx \
	DlgRef_1Sel_Ext_moc.cxx \
	DlgRef_3Check_QTD_moc.cxx \
	DlgRef_6Sel_QTD_moc.cxx \
	DlgRef_4Sel1Spin2Check_moc.cxx \
	DlgRef_3Sel1Check_QTD_moc.cxx

# header files 
salomeinclude_HEADERS = \
	DlgRef_Skeleton_QTD.h \
	DlgRef_1Sel_QTD.h \
	DlgRef_2Sel_QTD.h \
	DlgRef_3Sel_QTD.h \
	DlgRef_1Sel1Check_QTD.h \
	DlgRef_1Sel3Check_QTD.h \
	DlgRef_1Sel1Check1List_QTD.h \
	DlgRef_1Sel1Spin_QTD.h \
	DlgRef_1Sel2Spin_QTD.h \
	DlgRef_1Sel3Spin_QTD.h \
	DlgRef_1Sel4Spin_QTD.h \
	DlgRef_1Sel5Spin_QTD.h \
	DlgRef_1Sel5Spin1Check_QTD.h \
	DlgRef_2Sel1Spin_QTD.h \
	DlgRef_2Sel2Spin_QTD.h \
	DlgRef_2Sel3Spin_QTD.h \
	DlgRef_1Sel1Spin1Check_QTD.h \
	DlgRef_2Sel1Spin2Check_QTD.h \
	DlgRef_2Sel2Spin1Check_QTD.h \
	DlgRef_2Sel4Spin1Check_QTD.h \
	DlgRef_3Sel4Spin2Check_QTD.h \
	DlgRef_2Sel3Spin2Rb_QTD.h \
	DlgRef_2Sel3Spin2Rb.h \
	DlgRef_4Sel1List_QTD.h \
	DlgRef_2Sel1List_QTD.h \
	DlgRef_2Sel1List1Check_QTD.h \
	DlgRef_1List1Spin1Btn_QTD.h \
	DlgRef_4Sel1List1Check_QTD.h \
	DlgRef_1Spin_QTD.h \
	DlgRef_2Spin_QTD.h \
	DlgRef_3Spin_QTD.h \
	DlgRef_3Spin1Check_QTD.h \
	DlgRef_3Sel3Spin1Check_QTD.h \
	DlgRef_3Sel3Spin2Check_QTD.h \	
	DlgRef_4Sel1Spin2Check_QTD.h \
	DlgRef_SpinBox.h \
	DlgRef_1Sel1Spin.h \
	DlgRef_1Sel2Spin.h \
	DlgRef_1Sel3Spin.h \
	DlgRef_1Sel4Spin.h \
	DlgRef_1Sel5Spin.h \
	DlgRef_1Sel5Spin1Check.h \
	DlgRef_2Sel1Spin.h \
	DlgRef_2Sel2Spin.h \
	DlgRef_2Sel3Spin.h \
	DlgRef_1Sel1Spin1Check.h \
	DlgRef_2Sel1Spin2Check.h \
	DlgRef_2Sel2Spin1Check.h \
	DlgRef_2Sel4Spin1Check.h \
	DlgRef_3Sel4Spin2Check.h \
	DlgRef_3Sel3Spin1Check.h \
	DlgRef_3Sel3Spin2Check.h \	
	DlgRef_1Spin.h \
	DlgRef_2Spin.h \
	DlgRef_3Spin.h \
	DlgRef_3Spin1Check.h \
	DlgRef_3Sel1Spin.h \
	DlgRef_1Sel_Ext.h \
	DlgRef_3Check_QTD.h \
	DlgRef_6Sel_QTD.h \
	DlgRef_4Sel1Spin2Check.h \
	DlgRef_3Sel1Check_QTD.h \
	GEOM_DlgRef.hxx

nodist_libDlgRef_la_SOURCES= \
	$(MOC_FILES)

# additionnal information to compil and link file
libDlgRef_la_CPPFLAGS = \
	$(QT_INCLUDES) \
	$(KERNEL_CXXFLAGS) \
	$(GUI_CXXFLAGS)

libDlgRef_la_LDFLAGS  = \
	$(GUI_LDFLAGS) -lqtx \
	$(STDLIB) \
	$(QT_MT_LIBS) 
