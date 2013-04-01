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

#  GEOM ShHealOper : Shape Healing
#  File   : Makefile.in
#  Author : Alexander SLADKOV
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =				\
	ShHealOper_CloseContour.hxx		\
	ShHealOper_EdgeDivide.hxx		\
	ShHealOper_FillHoles.hxx		\
	ShHealOper_RemoveFace.hxx		\
	ShHealOper_RemoveInternalWires.hxx	\
	ShHealOper_Sewing.hxx			\
	ShHealOper_ShapeProcess.hxx		\
	ShHealOper_SpiltCurve2d.hxx		\
	ShHealOper_SplitCurve2d.hxx		\
	ShHealOper_SplitCurve3d.hxx		\
	ShHealOper_ChangeOrientation.hxx	\
	ShHealOper_Tool.hxx

# Libraries targets
lib_LTLIBRARIES = libShHealOper.la

dist_libShHealOper_la_SOURCES =			\
	ShHealOper_CloseContour.cxx		\
	ShHealOper_EdgeDivide.cxx		\
	ShHealOper_FillHoles.cxx		\
	ShHealOper_RemoveFace.cxx		\
	ShHealOper_RemoveInternalWires.cxx	\
	ShHealOper_Sewing.cxx			\
	ShHealOper_ShapeProcess.cxx		\
	ShHealOper_SplitCurve2d.cxx		\
	ShHealOper_SplitCurve3d.cxx		\
	ShHealOper_ChangeOrientation.cxx	\
	ShHealOper_Tool.cxx

# additional information to compile and link file

libShHealOper_la_CPPFLAGS =	\
	$(CAS_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)

libShHealOper_la_LDFLAGS  =	\
	$(STDLIB)		\
	$(CAS_KERNEL)		\
	$(CAS_MATH)		\
	$(CAS_TKTopAlgo)	\
	$(CAS_LDPATH) -lTKBool -lTKShHealing -lTKCAF -lTKLCAF
