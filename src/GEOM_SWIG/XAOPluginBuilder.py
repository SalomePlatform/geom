#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
#  @param author The author of the file
#  @param fileName The name of the file to export
#  @return True if operation is successful or False otherwise
#
#  @ingroup l2_import_export
def ExportXAO(self, shape, groups, fields, author, fileName):
    """
    Export a shape to XAO format
    
    Parameters:
        shape The shape to export
        groups The list of groups to export
        fields The list of fields to export
        author The author of the file
        fileName The name of the file to export

    Returns:
        True if operation is successful or False otherwise
    """
    from salome.geom.geomBuilder import RaiseIfFailed
    anOp = GetXAOPluginOperations(self)
    res = anOp.ExportXAO(shape, groups, fields, author, fileName)
    RaiseIfFailed("ExportXAO", anOp)
    return res

## Import a shape from XAO format
#  @param fileName The name of the file to import
#  @param theName Object name; when specified, this parameter is used
#         for result publication in the study. Otherwise, if automatic
#         publication is switched on, default value is used for result name.
# 
#  @return tuple (\a res, \a shape, \a subShapes, \a groups, \a fields)
#       \a res Flag indicating if the import was successful
#       \a shape The imported shape
#       \a subShapes The list of imported subShapes
#       \a groups The list of imported groups
#       \a fields The list of imported fields
#
#  @ingroup l2_import_export
def ImportXAO(self, fileName, theName=None):
    """
    Import a shape from XAO format
    
    Parameters:
        fileName The name of the file to import
        theName Object name; when specified, this parameter is used
                for result publication in the study. Otherwise, if automatic
                publication is switched on, default value is used for result name.

    Returns:
        A tuple (res, shape, subShapes, groups, fields):
        - res: Flag indicating if the import was successful
        - shape: The imported shape
        - subShapes: The list of imported subShapes
        - groups: The list of imported groups
        - fields: The list of imported fields
    """
    from salome.geom.geomBuilder import RaiseIfFailed
    anOp = GetXAOPluginOperations(self)
    res = anOp.ImportXAO(fileName)
    RaiseIfFailed("ImportXAO", anOp)
    self._autoPublish(res[1], theName, "imported")
    return res
