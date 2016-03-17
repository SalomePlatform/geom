# Copyright (C) 2012-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  vtkFiltersCore
  ${CAS_KERNEL} 
  ${CAS_TKMesh} ${CAS_TKTopAlgo}
  ${CAS_MODELER}
  ${KERNEL_SALOMELocalTrace}
  GEOMUtils
  )

# --- headers ---

SET(OCC2VTK_HEADERS
  OCC2VTK.h		
  OCC2VTK_Tools.h		
  GEOM_EdgeSource.h	
  GEOM_FaceSource.h	
  GEOM_ShadingFace.h	
  GEOM_VertexSource.h	
  GEOM_WireframeFace.h
  )

# --- sources ---

SET(OCC2VTK_SOURCES
  OCC2VTK_Tools.cxx	
  GEOM_EdgeSource.cxx	
  GEOM_FaceSource.cxx	
  GEOM_ShadingFace.cxx	
  GEOM_VertexSource.cxx	
  GEOM_WireframeFace.cxx
  )

# --- rules ---

ADD_LIBRARY(OCC2VTK ${OCC2VTK_SOURCES})
TARGET_LINK_LIBRARIES(OCC2VTK ${_link_LIBRARIES})
INSTALL(TARGETS OCC2VTK EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${OCC2VTK_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})


