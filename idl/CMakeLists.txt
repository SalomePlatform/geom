# Copyright (C) 2012-2024  CEA, EDF, OPEN CASCADE
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

INCLUDE(UseOmniORB)

INCLUDE_DIRECTORIES(
  ${KERNEL_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

IF(WIN32)
  ADD_DEFINITIONS(-DNOGDI)
ENDIF(WIN32)


SET(SalomeIDLGEOM_IDLSOURCES
  GEOM_Gen.idl
)

SET(SalomeIDLGEOMSuperv_IDLSOURCES
  GEOM_Superv.idl
)

SET(SalomeIDLAdvancedGEOM_IDLSOURCES
  AdvancedGEOM.idl
)

SET(SalomeIDLSTLPlugin_IDLSOURCES
  STLPlugin.idl
)

SET(SalomeIDLBREPPlugin_IDLSOURCES
  BREPPlugin.idl
)

SET(SalomeIDLSTEPPlugin_IDLSOURCES
  STEPPlugin.idl
)

SET(SalomeIDLIGESPlugin_IDLSOURCES
  IGESPlugin.idl
)

SET(SalomeIDLXAOPlugin_IDLSOURCES
  XAOPlugin.idl
)

SET(SalomeIDLVTKPlugin_IDLSOURCES
  VTKPlugin.idl
)

SET(IDL_INCLUDE_DIRS
  ${KERNEL_ROOT_DIR}/idl/salome
  ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(IDL_LINK_FLAGS
  ${KERNEL_SalomeIDLKernel}
)

SET(IDL_LINK_PLUGIN_FLAGS
  ${KERNEL_SalomeIDLKernel}
  SalomeIDLGEOM
)

SET(IDL_LINK_SUPERV_FLAGS
  ${KERNEL_SalomeIDLKernel}
  SalomeIDLGEOM
  SalomeIDLAdvancedGEOM
)

OMNIORB_ADD_MODULE(SalomeIDLGEOM "${SalomeIDLGEOM_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_FLAGS}")
INSTALL(TARGETS SalomeIDLGEOM EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

OMNIORB_ADD_MODULE(SalomeIDLAdvancedGEOM "${SalomeIDLAdvancedGEOM_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_PLUGIN_FLAGS}")
INSTALL(TARGETS SalomeIDLAdvancedGEOM EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

OMNIORB_ADD_MODULE(SalomeIDLSTLPlugin "${SalomeIDLSTLPlugin_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_PLUGIN_FLAGS}")
INSTALL(TARGETS SalomeIDLSTLPlugin EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

OMNIORB_ADD_MODULE(SalomeIDLBREPPlugin "${SalomeIDLBREPPlugin_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_PLUGIN_FLAGS}")
INSTALL(TARGETS SalomeIDLBREPPlugin EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

OMNIORB_ADD_MODULE(SalomeIDLSTEPPlugin "${SalomeIDLSTEPPlugin_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_PLUGIN_FLAGS}")
INSTALL(TARGETS SalomeIDLSTEPPlugin EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

OMNIORB_ADD_MODULE(SalomeIDLIGESPlugin "${SalomeIDLIGESPlugin_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_PLUGIN_FLAGS}")
INSTALL(TARGETS SalomeIDLIGESPlugin EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

OMNIORB_ADD_MODULE(SalomeIDLXAOPlugin "${SalomeIDLXAOPlugin_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_PLUGIN_FLAGS}")
INSTALL(TARGETS SalomeIDLXAOPlugin EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

IF(SALOME_GEOM_USE_VTK)
  OMNIORB_ADD_MODULE(SalomeIDLVTKPlugin "${SalomeIDLVTKPlugin_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_PLUGIN_FLAGS}")
  INSTALL(TARGETS SalomeIDLVTKPlugin EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
ENDIF(SALOME_GEOM_USE_VTK)

OMNIORB_ADD_MODULE(SalomeIDLGEOMSuperv "${SalomeIDLGEOMSuperv_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_SUPERV_FLAGS}")
INSTALL(TARGETS SalomeIDLGEOMSuperv EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
