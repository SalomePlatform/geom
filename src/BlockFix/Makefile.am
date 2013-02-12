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

# GEOM BlockFix : tools for Blocks algorithms
# File   : Makefile.am
# Author : Julia DOROVSKIKH
# Module : GEOM

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libBlockFix.la

dist_libBlockFix_la_SOURCES =			\
	BlockFix_BlockFixAPI.cxx		\
	BlockFix_CheckTool.cxx			\
	BlockFix.cxx				\
	BlockFix_PeriodicSurfaceModifier.cxx	\
	BlockFix_SphereSpaceModifier.cxx	\
	BlockFix_UnionEdges.cxx			\
	BlockFix_UnionFaces.cxx

# header files
salomeinclude_HEADERS =					\
	BlockFix.hxx					\
	BlockFix_BlockFixAPI.hxx			\
	BlockFix_CheckTool.hxx				\
	BlockFix_PeriodicSurfaceModifier.hxx		\
	BlockFix_SphereSpaceModifier.hxx		\
	BlockFix_UnionEdges.hxx				\
	BlockFix_UnionFaces.hxx

libBlockFix_la_CPPFLAGS =	\
	$(CAS_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)

libBlockFix_la_LDFLAGS  =		\
	$(STDLIB)			\
	$(CAS_LDPATH) -lTKBO -lTKShHealing -lTKBool -lTKMesh -lTKOffset \
	$(KERNEL_LDFLAGS) -lSALOMELocalTrace
