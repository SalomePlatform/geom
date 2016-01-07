#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2014-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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

from GEOM import IXAOOperations

# Engine Library Name
__libraryName__ = "XAOPluginEngine"

def GetXAOPluginOperations(self):
    anOp = self.GetPluginOperations(self.myStudyId, __libraryName__)
    return anOp._narrow(IXAOOperations)

## Export a shape to XAO format
#  @param shape The shape to export
#  @param groups The list of groups to export
#  @param fields The list of fields to export
#  @param author The author of the export
#  @param fileName The name of the file to export
#  @return boolean
#
#  @ingroup l2_import_export
def ExportXAO(self, shape, groups, fields, author, fileName):
    from salome.geom.geomBuilder import RaiseIfFailed
    anOp = GetXAOPluginOperations(self)
    res = anOp.ExportXAO(shape, groups, fields, author, fileName)
    RaiseIfFailed("ExportXAO", anOp)
    return res

## Import a shape from XAO format
#  @param fileName The name of the file to import
#  @return tuple (res, shape, subShapes, groups, fields)
#       res Flag indicating if the import was successful
#       shape The imported shape
#       subShapes The list of imported subShapes
#       groups The list of imported groups
#       fields The list of imported fields
#
#  @ingroup l2_import_export
def ImportXAO(self, fileName):
    from salome.geom.geomBuilder import RaiseIfFailed
    anOp = GetXAOPluginOperations(self)
    res = anOp.ImportXAO(fileName)
    RaiseIfFailed("ImportXAO", anOp)
    return res
