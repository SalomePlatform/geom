# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

##
# Common packages
##
SET(SUBDIRS_COMMON
  ARCHIMEDE BlockFix GEOMAlgo SKETCHER GEOM BREPExport
  BREPImport IGESExport IGESImport STEPExport STEPImport STLExport
  ShHealOper GEOMUtils XAO XAO_Swig GEOMImpl GEOM_I GEOMClient GEOM_I_Superv GEOM_SWIG GEOM_PY
  AdvancedEngine OCC2VTK VTKExport
  )

##
# Curve creator
##
IF(SALOME_GEOM_BUILD_CC)
  SET(SUBDIRS_CC
    CurveCreator
    )
ENDIF()

##
# OPENCV
##
IF(SALOME_GEOM_USE_OPENCV)
  SET(SUBDIRS_OPENCV 
    ShapeRecognition
    )
ENDIF()

##
# GUI
##
IF(SALOME_BUILD_GUI)
  SET(SUBDIRS_GUI
    OBJECT DlgRef GEOMFiltersSelection Material GEOMGUI
    GEOMBase GEOMToolsGUI DisplayGUI BasicGUI PrimitiveGUI GenerationGUI
    EntityGUI BuildGUI BooleanGUI TransformationGUI OperationGUI
    RepairGUI MeasureGUI GroupGUI BlocksGUI AdvancedGUI ImportExportGUI
    GEOM_SWIG_WITHIHM
    )
ENDIF()

SET(SUBDIRS
  ${SUBDIRS_COMMON}
  ${SUBDIRS_OPENCV}
  ${SUBDIRS_CC}
  ${SUBDIRS_GUI}
)

FOREACH(dir ${SUBDIRS})
  ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir ${SUBDIRS})
