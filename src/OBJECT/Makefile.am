#  GEOM OBJECT : interactive object for Geometry entities visualization
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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =				\
	GEOM_Actor.h				\
	GEOM_AssemblyBuilder.h			\
	GEOM_AISShape.hxx			\
	GEOM_AISShape.ixx			\
	GEOM_AISShape.jxx			\
	Handle_GEOM_AISShape.hxx		\
	GEOM_InteractiveObject.hxx		\
	GEOM_InteractiveObject.ixx		\
	GEOM_InteractiveObject.jxx		\
	Handle_GEOM_InteractiveObject.hxx	\
	GEOM_AISTrihedron.hxx			\
	GEOM_VTKTrihedron.hxx			\
	GEOM_AISVector.hxx			\
	GEOM_OBJECT_defs.hxx			\
	GEOM_OCCReader.h			\
	GEOM_SmartPtr.h				\
	GEOM_DeviceActor.h			\
	GEOM_EdgeSource.h			\
	GEOM_FaceSource.h			\
	GEOM_ShadingFace.h			\
	GEOM_VertexSource.h			\
	GEOM_WireframeFace.h

# Libraries targets
lib_LTLIBRARIES = libGEOMObject.la

dist_libGEOMObject_la_SOURCES =		\
	GEOM_Actor.cxx			\
	GEOM_OCCReader.cxx		\
	GEOM_AssemblyBuilder.cxx	\
	GEOM_AISShape.cxx		\
	GEOM_InteractiveObject.cxx	\
	GEOM_AISTrihedron.cxx		\
	GEOM_VTKTrihedron.cxx		\
	GEOM_AISVector.cxx		\
	GEOM_DeviceActor.cxx		\
	GEOM_EdgeSource.cxx		\
	GEOM_FaceSource.cxx		\
	GEOM_ShadingFace.cxx		\
	GEOM_VertexSource.cxx		\
	GEOM_WireframeFace.cxx


libGEOMObject_la_CPPFLAGS =	\
	$(CORBA_CXXFLAGS)	\
	$(CORBA_INCLUDES)	\
	$(QT_INCLUDES)		\
	$(PYTHON_INCLUDES)	\
	$(CAS_CPPFLAGS)		\
	$(VTK_INCLUDES)		\
	$(KERNEL_CXXFLAGS)	\
	$(GUI_CXXFLAGS) 
	
libGEOMObject_la_LDFLAGS  =			\
	$(KERNEL_LDFLAGS)  -lSALOMELocalTrace	\
	$(GUI_LDFLAGS) -lqtx -lVTKViewer -lSVTK -lSalomeObject
#	$(CORBA_LIBS) \
#	$(STDLIB) \
#	$(PYTHON_LIBS) \
#	$(QT_MT_LIBS) \
#	$(VTK_LIBS) \
#	$(OGL_LIBS)
