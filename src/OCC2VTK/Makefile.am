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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =		\
	OCC2VTK.h		\
	OCC2VTK_Tools.h		\
	GEOM_EdgeSource.h	\
	GEOM_FaceSource.h	\
	GEOM_ShadingFace.h	\
	GEOM_VertexSource.h	\
	GEOM_WireframeFace.h

# Libraries targets
lib_LTLIBRARIES = libOCC2VTK.la

dist_libOCC2VTK_la_SOURCES =	\
	OCC2VTK_Tools.cxx	\
	GEOM_EdgeSource.cxx	\
	GEOM_FaceSource.cxx	\
	GEOM_ShadingFace.cxx	\
	GEOM_VertexSource.cxx	\
	GEOM_WireframeFace.cxx

libOCC2VTK_la_CPPFLAGS =	\
	$(CAS_CPPFLAGS)		\
	$(VTK_INCLUDES)		\
	$(KERNEL_CXXFLAGS)

libOCC2VTK_la_LDFLAGS  =	\
	$(CAS_KERNEL) -lTKBRep -lTKG2d -lTKG3d -lTKV3d -lTKMesh -lTKGeomBase -lTKGeomAlgo -lTKTopAlgo -lTKService \
	$(VTK_LIBS) \
	$(KERNEL_LDFLAGS)  -lSALOMELocalTrace
