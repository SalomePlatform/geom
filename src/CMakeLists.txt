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

##
# Common packages
##
SET(SUBDIRS_COMMON
  ARCHIMEDE BlockFix GEOMAlgo SKETCHER GEOM ShHealOper GEOMUtils XAO XAO_Swig 
  GEOMImpl GEOM_I GEOMClient GEOM_I_Superv GEOM_SWIG GEOM_PY
  AdvancedEngine
  STLPlugin BREPPlugin STEPPlugin IGESPlugin XAOPlugin Tools
  )

##
# OPENCV
##
IF(SALOME_GEOM_USE_OPENCV)
  SET(SUBDIRS_OPENCV 
    ShapeRecognition
    )
ENDIF()

##
# VTK
##
IF(SALOME_GEOM_USE_VTK)
  SET(SUBDIRS_VTK 
    OCC2VTK
    VTKPlugin
    )
ENDIF()

##
# GUI
##
IF(SALOME_BUILD_GUI)
  SET(SUBDIRS_GUI
    OBJECT DlgRef GEOMFiltersSelection Material GEOMGUI
    GEOMBase GEOMToolsGUI DisplayGUI BasicGUI PrimitiveGUI GenerationGUI
    CurveCreator MeasureGUI EntityGUI BuildGUI BooleanGUI TransformationGUI OperationGUI
    RepairGUI GroupGUI BlocksGUI AdvancedGUI
    GEOM_SWIG_WITHIHM
    )
  IF(SALOME_USE_GRAPHICSVIEW)
    LIST(APPEND SUBDIRS_GUI DependencyTree)
  ENDIF()
ENDIF()

SET(SUBDIRS
  ${SUBDIRS_COMMON}
  ${SUBDIRS_OPENCV}
  ${SUBDIRS_VTK}
  ${SUBDIRS_CC}
  ${SUBDIRS_GUI}
)

FOREACH(dir ${SUBDIRS})
  ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir ${SUBDIRS})
