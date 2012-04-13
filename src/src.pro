# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS  = OBJECT
SUBDIRS += ARCHIMEDE
SUBDIRS += NMTDS
SUBDIRS += NMTTools
SUBDIRS += NMTAlgo
SUBDIRS += GEOMAlgo
SUBDIRS += SKETCHER
SUBDIRS += GEOM
SUBDIRS += BREPExport
SUBDIRS += BREPImport
SUBDIRS += IGESExport
SUBDIRS += IGESImport
SUBDIRS += STEPExport
SUBDIRS += STEPImport
SUBDIRS += STLExport
SUBDIRS += ShHealOper
SUBDIRS += GEOMImpl
SUBDIRS += GEOM_I
SUBDIRS += GEOMClient
SUBDIRS += DlgRef
SUBDIRS += GEOMFiltersSelection
SUBDIRS += GEOMGUI
SUBDIRS += GEOMBase
SUBDIRS += GEOMToolsGUI
SUBDIRS += DisplayGUI
SUBDIRS += BasicGUI
SUBDIRS += PrimitiveGUI
SUBDIRS += GenerationGUI
SUBDIRS += EntityGUI
SUBDIRS += BuildGUI
SUBDIRS += BooleanGUI
SUBDIRS += TransformationGUI
#SUBDIRS += OperationGUI
#SUBDIRS += RepairGUI
#SUBDIRS += MeasureGUI
#SUBDIRS += GroupGUI
#SUBDIRS += BlocksGUI
#SUBDIRS += GEOM_I_Superv
#SUBDIRS += GEOM_SWIG
