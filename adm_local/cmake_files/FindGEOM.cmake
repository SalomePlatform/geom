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

SET(GEOM_CXXFLAGS -I${GEOM_ROOT_DIR}/include/salome)

FIND_LIBRARY(AdvancedGUI AdvancedGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(BasicGUI BasicGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(BlocksGUI BlocksGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(BooleanGUI BooleanGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(BREPExport BREPExport ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(BREPImport BREPImport ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(BuildGUI BuildGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(DisplayGUI DisplayGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(DlgRef DlgRef ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(EntityGUI EntityGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GenerationGUI GenerationGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOMAlgo GEOMAlgo ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOMArchimede GEOMArchimede ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOMBase GEOMBase ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOMbasic GEOMbasic ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOMClient GEOMClient ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOMEngine GEOMEngine ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOMFiltersSelection GEOMFiltersSelection ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOMimpl GEOMimpl ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOMObject GEOMObject ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOMSketcher GEOMSketcher ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM GEOM ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOM_SupervEngine GEOM_SupervEngine ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GEOMToolsGUI GEOMToolsGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(GroupGUI GroupGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(IGESExport IGESExport ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(IGESImport IGESImport ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MeasureGUI MeasureGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(NMTDS NMTDS ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(NMTTools NMTTools ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(OCC2VTK OCC2VTK ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(OperationGUI OperationGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(PrimitiveGUI PrimitiveGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(RepairGUI RepairGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(SalomeIDLGEOM SalomeIDLGEOM ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(ShHealOper ShHealOper ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(STEPExport STEPExport ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(STEPImport STEPImport ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(STLExport STLExport ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(TransformationGUI TransformationGUI ${GEOM_ROOT_DIR}/lib/salome)
FIND_LIBRARY(VTKExport VTKExport ${GEOM_ROOT_DIR}/lib/salome)
