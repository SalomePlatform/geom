# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

SET(GEOM_CXXFLAGS -I${GEOM_ROOT_DIR}/include/salome) # to be removed
SET(GEOM_INCLUDE_DIRS ${GEOM_ROOT_DIR}/include/salome)

FIND_LIBRARY(GEOM_GEOMArchimede GEOMArchimede ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_BlockFix BlockFix ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMbasic GEOMbasic ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMAlgo GEOMAlgo ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMClient GEOMClient ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMimpl GEOMimpl ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMUtils GEOMUtils ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMEngine GEOMEngine ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_SupervEngine GEOM_SupervEngine ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMSketcher GEOMSketcher ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_SalomeIDLGEOM SalomeIDLGEOM ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_SalomeIDLGEOMSuperv SalomeIDLGEOMSuperv ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_SalomeIDLSTLPlugin SalomeIDLSTLPlugin ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_SalomeIDLBREPPlugin SalomeIDLBREPPlugin ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_SalomeIDLSTEPPlugin SalomeIDLSTEPPlugin ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_SalomeIDLIGESPlugin SalomeIDLIGESPlugin ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_SalomeIDLVTKPlugin SalomeIDLVTKPlugin ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_SalomeIDLXAOPlugin SalomeIDLXAOPlugin ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_ShHealOper ShHealOper ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_XAO XAO ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_AdvancedEngine AdvancedEngine ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_AdvancedGUI AdvancedGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_BasicGUI BasicGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_BlocksGUI BlocksGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_BooleanGUI BooleanGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_BuildGUI BuildGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_DisplayGUI DisplayGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_DlgRef DlgRef ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_EntityGUI EntityGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMBase GEOMBase ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMFiltersSelection GEOMFiltersSelection ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOM GEOM ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMToolsGUI GEOMToolsGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_DependencyTree DependencyTree ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GenerationGUI GenerationGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GroupGUI GroupGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_Material Material ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_MeasureGUI MeasureGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMObject GEOMObject ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_OCC2VTK OCC2VTK ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_OperationGUI OperationGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_PrimitiveGUI PrimitiveGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_RepairGUI RepairGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_TransformationGUI TransformationGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_STLPluginGUI STLPluginGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_STLPluginEngine STLPluginEngine ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_BREPPluginGUI BREPPluginGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_BREPPluginEngine BREPPluginEngine ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_STEPPluginGUI STEPPluginGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_STEPPluginEngine STEPPluginEngine ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_IGESPluginGUI IGESPluginGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_IGESPluginEngine IGESPluginEngine ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_XAOPluginGUI XAOPluginGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_XAOPluginEngine XAOPluginEngine ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_VTKPluginGUI VTKPluginGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_VTKPluginEngine VTKPluginEngine ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_GEOMShapeRec GEOMShapeRec ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_CurveCreator CurveCreator ${GEOM_ROOT_DIR}/lib/salome)
