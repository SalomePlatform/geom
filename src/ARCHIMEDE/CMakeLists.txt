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
  ${PTHREAD_INCLUDE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS( 
  ${CAS_DEFINITIONS} 
  )

# libraries to link to
SET(_link_LIBRARIES 
  ${CAS_KERNEL} ${CAS_MODELER} ${CAS_TKMesh} 
  ${KERNEL_SALOMELocalTrace}
  )

# --- headers ---

SET(Archimede_HEADERS Archimede_VolumeSection.hxx)

# --- sources ---

SET(Archimede_SOURCES Archimede_VolumeSection.cxx)

# --- rules ---

ADD_LIBRARY(GEOMArchimede ${Archimede_SOURCES})
TARGET_LINK_LIBRARIES(GEOMArchimede ${_link_LIBRARIES})
INSTALL(TARGETS GEOMArchimede EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

#INSTALL(FILES ${Archimede_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
