#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# File    : Makefile.am
# Author  : Alexander BORODIN, Open CASCADE S.A.S. (alexander.borodin@opencascade.com)
# Package : src (source files directory)
#
SUBDIRS = ARCHIMEDE NMTDS NMTTools GEOMAlgo SKETCHER OCC2VTK GEOM	\
          BREPExport BREPImport IGESExport IGESImport STEPExport	\
          STEPImport STLExport VTKExport ShHealOper GEOMImpl GEOM_I	\
          GEOMClient GEOM_I_Superv GEOM_SWIG GEOM_PY

if GEOM_ENABLE_GUI
  SUBDIRS += OBJECT DlgRef GEOMFiltersSelection GEOMGUI GEOMBase GEOMToolsGUI	\
             DisplayGUI BasicGUI PrimitiveGUI GenerationGUI EntityGUI BuildGUI	\
             BooleanGUI TransformationGUI OperationGUI RepairGUI MeasureGUI	\
             GroupGUI BlocksGUI AdvancedGUI GEOM_SWIG_WITHIHM
endif

DIST_SUBDIRS = ARCHIMEDE NMTDS NMTTools GEOMAlgo SKETCHER OCC2VTK GEOM BREPExport\
               BREPImport IGESExport IGESImport STEPExport STEPImport STLExport \
               VTKExport ShHealOper GEOMImpl GEOM_I GEOMClient GEOM_I_Superv	\
               GEOM_SWIG OBJECT DlgRef GEOMFiltersSelection GEOMGUI GEOMBase	\
               GEOMToolsGUI DisplayGUI BasicGUI PrimitiveGUI GenerationGUI	\
               EntityGUI BuildGUI BooleanGUI TransformationGUI OperationGUI	\
               RepairGUI MeasureGUI GroupGUI BlocksGUI AdvancedGUI 		\
               GEOM_SWIG_WITHIHM GEOM_PY
