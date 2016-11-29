# Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)
INCLUDE(CMakeDependentOption)

PROJECT(SalomeGEOM C CXX)

# Ensure a proper linker behavior:
CMAKE_POLICY(SET CMP0003 NEW)
IF(WIN32)
  CMAKE_POLICY(SET CMP0020 OLD) # disable automatic linking to qtmain.lib 
ENDIF(WIN32)

# Versioning
# ===========
# Project name, upper case
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)

SET(${PROJECT_NAME_UC}_MAJOR_VERSION 8)
SET(${PROJECT_NAME_UC}_MINOR_VERSION 2)
SET(${PROJECT_NAME_UC}_PATCH_VERSION 0)
SET(${PROJECT_NAME_UC}_VERSION
  ${${PROJECT_NAME_UC}_MAJOR_VERSION}.${${PROJECT_NAME_UC}_MINOR_VERSION}.${${PROJECT_NAME_UC}_PATCH_VERSION})
SET(${PROJECT_NAME_UC}_VERSION_DEV 0)

# Common CMake macros
# ===================
SET(CONFIGURATION_ROOT_DIR $ENV{CONFIGURATION_ROOT_DIR} CACHE PATH "Path to the Salome CMake configuration files")
IF(EXISTS ${CONFIGURATION_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${CONFIGURATION_ROOT_DIR}/cmake")
  INCLUDE(SalomeMacros)
ELSE()
  MESSAGE(FATAL_ERROR "We absolutely need the Salome CMake configuration files, please define CONFIGURATION_ROOT_DIR !")
ENDIF()

# Find KERNEL
# ===========
SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "Path to the Salome KERNEL")
IF(EXISTS ${KERNEL_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${KERNEL_ROOT_DIR}/salome_adm/cmake_files")
  FIND_PACKAGE(SalomeKERNEL REQUIRED)
  KERNEL_WITH_CORBA() #check whether KERNEL builded with CORBA
  ADD_DEFINITIONS(${KERNEL_DEFINITIONS})
  INCLUDE_DIRECTORIES(${KERNEL_INCLUDE_DIRS})
ELSE(EXISTS ${KERNEL_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a Salome KERNEL, please define KERNEL_ROOT_DIR")
ENDIF(EXISTS ${KERNEL_ROOT_DIR})

# Platform setup
# ==============
INCLUDE(SalomeSetupPlatform)   # From KERNEL
# Always build libraries as shared objects:
SET(BUILD_SHARED_LIBS TRUE)
# Local macros:
LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/adm_local/cmake_files")

# User options 
# ============
OPTION(SALOME_BUILD_DOC "Generate SALOME GEOM documentation" ON)
OPTION(SALOME_BUILD_TESTS "Build SALOME tests" ON)

# Advanced options:
OPTION(SALOME_BUILD_GUI "Enable GUI" ON)
CMAKE_DEPENDENT_OPTION(SALOME_GEOM_USE_OPENCV "Enable shape recognition from picture" OFF
                       "SALOME_BUILD_GUI" OFF)
CMAKE_DEPENDENT_OPTION(SALOME_GEOM_USE_VTK "Enable VTK-dependent functionality" ON
                       "NOT SALOME_BUILD_GUI" ON)
MARK_AS_ADVANCED(SALOME_BUILD_GUI SALOME_GEOM_USE_OPENCV SALOME_GEOM_USE_VTK)

# Prerequisites
# =============
# Find "big" prerequisites first - they reference themselves many others
#   -> this can help finding the smaller prerequisites and detect conficts.
# In our case KERNEL has already loaded many prereq:

##
## From KERNEL:
##

# Various
FIND_PACKAGE(SalomePythonInterp REQUIRED)
FIND_PACKAGE(SalomePythonLibs REQUIRED)
FIND_PACKAGE(SalomePThread REQUIRED)
FIND_PACKAGE(SalomeSWIG REQUIRED)
FIND_PACKAGE(SalomeBoost REQUIRED)
FIND_PACKAGE(SalomeOmniORB REQUIRED)
FIND_PACKAGE(SalomeOmniORBPy REQUIRED)
FIND_PACKAGE(SalomeLibXml2 REQUIRED)
FIND_PACKAGE(SalomeHDF5 REQUIRED COMPONENTS C)

# Other KERNEL optionals: 
IF(SALOME_BUILD_TESTS)
  ENABLE_TESTING()
  FIND_PACKAGE(SalomeCppUnit)
  SALOME_LOG_OPTIONAL_PACKAGE(CppUnit SALOME_BUILD_TESTS)
ENDIF()
IF(SALOME_BUILD_DOC)
  FIND_PACKAGE(SalomeDoxygen)
  FIND_PACKAGE(SalomeGraphviz)
  SALOME_LOG_OPTIONAL_PACKAGE(Doxygen SALOME_BUILD_DOC)
  SALOME_LOG_OPTIONAL_PACKAGE(Graphviz SALOME_BUILD_DOC)
ENDIF()

# Find GUI (optional)
IF(SALOME_BUILD_GUI)
  SET(GUI_ROOT_DIR $ENV{GUI_ROOT_DIR} CACHE PATH "Path to the Salome GUI")
  IF(EXISTS ${GUI_ROOT_DIR})
    LIST(APPEND CMAKE_MODULE_PATH "${GUI_ROOT_DIR}/adm_local/cmake_files")
    FIND_PACKAGE(SalomeGUI)
    SALOME_GUI_WITH_CORBA() #check whether GUI builded with CORBA
    SALOME_GUI_MODE(SALOME_USE_OCCVIEWER SALOME_USE_VTKVIEWER SALOME_USE_SALOMEOBJECT 
                    OPTIONAL SALOME_USE_PLOT2DVIEWER SALOME_USE_GRAPHICSVIEW SALOME_USE_PYCONSOLE)
    ADD_DEFINITIONS(${GUI_DEFINITIONS})
    INCLUDE_DIRECTORIES(${GUI_INCLUDE_DIRS})
  ELSE(EXISTS ${GUI_ROOT_DIR})
    MESSAGE(STATUS "GUI_ROOT_DIR is not well defined, GEOM will be built without GUI!!!")
  ENDIF(EXISTS ${GUI_ROOT_DIR})
  SALOME_LOG_OPTIONAL_PACKAGE(SalomeGUI SALOME_BUILD_GUI)
ENDIF(SALOME_BUILD_GUI)

##
## Prerequisites From GUI:
##

IF(SALOME_BUILD_GUI)
  IF(NOT SALOME_GUI_BUILD_WITH_QT5)
    # Qt
    FIND_PACKAGE(SalomeQt4 REQUIRED COMPONENTS QtCore QtGui QtXml QtWebKit QtOpenGL QtNetwork)
    # PyQt
    FIND_PACKAGE(SalomePyQt4 REQUIRED)
  ELSE()
    # Qt
    FIND_PACKAGE(SalomeQt5 REQUIRED)
    # PyQt
    FIND_PACKAGE(SalomePyQt5 REQUIRED)
  ENDIF()
ENDIF(SALOME_BUILD_GUI)

##
## GEOM specifics
##

FIND_PACKAGE(SalomeCAS REQUIRED)

IF(SALOME_GEOM_USE_VTK)
  FIND_PACKAGE(SalomeVTK REQUIRED)
  ADD_DEFINITIONS(-DWITH_VTK)
ENDIF(SALOME_GEOM_USE_VTK)

# OpenCV
IF(SALOME_GEOM_USE_OPENCV)
  FIND_PACKAGE(SalomeOpenCV)
  SALOME_LOG_OPTIONAL_PACKAGE(OpenCV SALOME_GEOM_USE_OPENCV)
  ADD_DEFINITIONS(-DWITH_OPENCV)
ENDIF(SALOME_GEOM_USE_OPENCV)

# Detection summary:
SALOME_PACKAGE_REPORT_AND_CHECK()

# Directories
# (default values taken from KERNEL)
# ===========
SET(SALOME_INSTALL_BINS "${SALOME_INSTALL_BINS}" CACHE PATH "Install path: SALOME binaries")
SET(SALOME_INSTALL_LIBS "${SALOME_INSTALL_LIBS}" CACHE PATH "Install path: SALOME libs")
SET(SALOME_INSTALL_IDLS "${SALOME_INSTALL_IDLS}" CACHE PATH "Install path: SALOME IDL files")
SET(SALOME_INSTALL_HEADERS "${SALOME_INSTALL_HEADERS}" CACHE PATH "Install path: SALOME headers")
SET(SALOME_INSTALL_SCRIPT_SCRIPTS "${SALOME_INSTALL_SCRIPT_SCRIPTS}" CACHE PATH 
   "Install path: SALOME scripts")
SET(SALOME_INSTALL_SCRIPT_DATA "${SALOME_INSTALL_SCRIPT_DATA}" CACHE PATH 
   "Install path: SALOME script data")
SET(SALOME_INSTALL_SCRIPT_PYTHON "${SALOME_INSTALL_SCRIPT_PYTHON}" CACHE PATH 
   "Install path: SALOME Python scripts")
SET(SALOME_INSTALL_PYTHON "${SALOME_INSTALL_PYTHON}" CACHE PATH "Install path: SALOME Python stuff")
SET(SALOME_INSTALL_PYTHON_SHARED "${SALOME_INSTALL_PYTHON_SHARED}" CACHE PATH 
   "Install path: SALOME Python shared modules")
SET(SALOME_INSTALL_CMAKE "${SALOME_INSTALL_CMAKE}" CACHE PATH "Install path: SALOME CMake files")
SET(SALOME_INSTALL_CMAKE_LOCAL "${SALOME_INSTALL_CMAKE_LOCAL}" CACHE PATH 
    "Install path: local SALOME CMake files") 
SET(SALOME_INSTALL_AMCONFIG_LOCAL "${SALOME_INSTALL_AMCONFIG_LOCAL}" CACHE PATH
  "Install path: local SALOME config files (obsolete, to be removed)")

SET(SALOME_INSTALL_RES "${SALOME_INSTALL_RES}" CACHE PATH "Install path: SALOME resources")
SET(SALOME_INSTALL_DOC "${SALOME_INSTALL_DOC}" CACHE PATH "Install path: SALOME documentation")

# Specific to GEOM:
SET(SALOME_GEOM_INSTALL_RES_DATA "${SALOME_INSTALL_RES}/geom" CACHE PATH 
    "Install path: SALOME GEOM specific data")
SET(SALOME_GEOM_INSTALL_RES_SCRIPTS "${SALOME_INSTALL_RES}/geom" CACHE PATH 
    "Install path: SALOME GEOM scripts")
SET(SALOME_GEOM_INSTALL_PLUGINS share/salome/plugins/geom CACHE PATH 
    "Install path: SALOME GEOM plugins")

MARK_AS_ADVANCED(SALOME_INSTALL_BINS SALOME_INSTALL_LIBS SALOME_INSTALL_IDLS SALOME_INSTALL_HEADERS)
MARK_AS_ADVANCED(SALOME_INSTALL_SCRIPT_SCRIPTS SALOME_INSTALL_SCRIPT_DATA SALOME_INSTALL_SCRIPT_PYTHON)
MARK_AS_ADVANCED(SALOME_INSTALL_APPLISKEL_SCRIPTS  SALOME_INSTALL_APPLISKEL_PYTHON SALOME_INSTALL_CMAKE SALOME_INSTALL_CMAKE_LOCAL SALOME_INSTALL_RES)
MARK_AS_ADVANCED(SALOME_INSTALL_PYTHON SALOME_INSTALL_PYTHON_SHARED)
MARK_AS_ADVANCED(SALOME_GEOM_INSTALL_RES_DATA SALOME_GEOM_INSTALL_RES_SCRIPTS SALOME_GEOM_INSTALL_PLUGINS)
MARK_AS_ADVANCED(SALOME_INSTALL_AMCONFIG_LOCAL SALOME_INSTALL_DOC)

# Accumulate environment variables for GEOM module
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_BINS}
                                                 ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_PYTHON}
                                                 ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_PYTHON_SHARED})
SALOME_ACCUMULATE_ENVIRONMENT(LD_LIBRARY_PATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_LIBS}) 

# Sources 
# ========
ADD_SUBDIRECTORY(idl)

ADD_SUBDIRECTORY(adm_local)
ADD_SUBDIRECTORY(resources)
ADD_SUBDIRECTORY(bin)
ADD_SUBDIRECTORY(src)
IF(SALOME_BUILD_DOC)
  ADD_SUBDIRECTORY(doc)
ENDIF()

# Header configuration
# ====================
SALOME_XVERSION(${PROJECT_NAME})
SALOME_CONFIGURE_FILE(GEOM_version.h.in GEOM_version.h INSTALL ${SALOME_INSTALL_HEADERS})

# Configuration export
# (here only the level 1 prerequisites are exposed)
# ====================
INCLUDE(CMakePackageConfigHelpers)

# List of targets in this project we want to make visible to the rest of the world.
# They all have to be INSTALL'd with the option "EXPORT ${PROJECT_NAME}TargetGroup"
SET(_${PROJECT_NAME}_exposed_targets 
  GEOMArchimede BlockFix GEOMbasic GEOMAlgo GEOMClient GEOMImpl
  GEOMUtils GEOMEngine GEOM_SupervEngine GEOMSketcher
  SalomeIDLGEOM SalomeIDLGEOMSuperv SalomeIDLAdvancedGEOM ShHealOper XAO AdvancedEngine 
  SalomeIDLSTLPlugin SalomeIDLBREPPlugin SalomeIDLSTEPPlugin SalomeIDLIGESPlugin SalomeIDLXAOPlugin
  STLPluginEngine BREPPluginEngine STEPPluginEngine IGESPluginEngine XAOPluginEngine 
)
IF(SALOME_BUILD_GUI)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
    AdvancedGUI BasicGUI BlocksGUI BooleanGUI BuildGUI DisplayGUI DlgRef CurveCreator EntityGUI GEOMBase
    GEOMFiltersSelection GEOM GEOMToolsGUI GenerationGUI GroupGUI Material MeasureGUI GEOMObject
    OperationGUI PrimitiveGUI RepairGUI TransformationGUI
    STLPluginGUI BREPPluginGUI STEPPluginGUI IGESPluginGUI XAOPluginGUI VTKPluginGUI
    )
  IF(SALOME_USE_GRAPHICSVIEW)
    LIST(APPEND _${PROJECT_NAME}_exposed_targets DependencyTree)
  ENDIF(SALOME_USE_GRAPHICSVIEW)
ENDIF(SALOME_BUILD_GUI)

IF(SALOME_GEOM_USE_VTK)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
    OCC2VTK
    VTKPluginEngine
    SalomeIDLVTKPlugin
    )
ENDIF(SALOME_GEOM_USE_VTK)

IF(SALOME_GEOM_USE_OPENCV)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
    GEOMShapeRec
    )
ENDIF(SALOME_GEOM_USE_OPENCV)

# Add all targets to the build-tree export set
EXPORT(TARGETS ${_${PROJECT_NAME}_exposed_targets}
  FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)

# Create the configuration files:
#   - in the build tree:

#      Ensure the variables are always defined for the configure:
SET(KERNEL_ROOT_DIR "${KERNEL_ROOT_DIR}")
SET(GUI_ROOT_DIR "${GUI_ROOT_DIR}")
SET(CAS_ROOT_DIR "${CAS_ROOT_DIR}")
SET(VTK_ROOT_DIR "${VTK_ROOT_DIR}")
SET(OPENCV_ROOT_DIR "${OPENCV_ROOT_DIR}")

SET(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include")

# Build variables that will be expanded when configuring Salome<MODULE>Config.cmake:
SALOME_CONFIGURE_PREPARE(OpenCV CAS VTK)
                         
CONFIGURE_PACKAGE_CONFIG_FILE(${PROJECT_NAME}Config.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}"
    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE_LOCAL CMAKE_INSTALL_PREFIX
    KERNEL_ROOT_DIR GUI_ROOT_DIR CAS_ROOT_DIR VTK_ROOT_DIR OPENCV_ROOT_DIR)

#   - in the install tree (VSR 16/08/2013: TEMPORARILY COMMENT THIS - TO REMOVE?):
#       Get the relative path of the include directory so 
#       we can register it in the generated configuration files:
#SET(CONF_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")
#CONFIGURE_PACKAGE_CONFIG_FILE(adm_local/cmake_files/${PROJECT_NAME}Config.cmake.in 
#    ${PROJECT_BINARY_DIR}/to_install/${PROJECT_NAME}Config.cmake
#    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE}"
#    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE CMAKE_INSTALL_PREFIX 
#    KERNEL_ROOT_DIR GUI_ROOT_DIR CAS_ROOT_DIR VTK_ROOT_DIR OPENCV_ROOT_DIR)

WRITE_BASIC_PACKAGE_VERSION_FILE(${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${${PROJECT_NAME_UC}_VERSION}
    COMPATIBILITY AnyNewerVersion)
  
# Install the CMake configuration files:
INSTALL(FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}")

# Install the export set for use with the install-tree
INSTALL(EXPORT ${PROJECT_NAME}TargetGroup DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}" 
        FILE ${PROJECT_NAME}Targets.cmake)
