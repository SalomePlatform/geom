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

from GEOM import IBREPOperations

# Engine Library Name
__libraryName__ = "BREPPluginEngine"

def GetBREPPluginOperations(self):
    anOp = self.GetPluginOperations(self.myStudyId, __libraryName__)
    return anOp._narrow(IBREPOperations)

## Export the given shape into a file with given name in BREP format.
#  @param theObject Shape to be stored in the file.
#  @param theFileName Name of the file to store the given shape in.
#  @ingroup l2_import_export
def ExportBREP(self, theObject, theFileName):
    """
    Export the given shape into a file with given name in BREP format.

    Parameters: 
        theObject Shape to be stored in the file.
        theFileName Name of the file to store the given shape in.
    """
    anOp = GetBREPPluginOperations(self)
    anOp.ExportBREP(theObject, theFileName)
    if anOp.IsDone() == 0:
        raise RuntimeError,  "Export : " + anOp.GetErrorCode()
        pass
    pass


## Import a shape from the BREP file
#  @param theFileName The file, containing the shape.
#  @param theName Object name; when specified, this parameter is used
#         for result publication in the study. Otherwise, if automatic
#         publication is switched on, default value is used for result name.
#
#  @return New GEOM.GEOM_Object, containing the imported shape.
#          If material names are imported it returns the list of
#          objects. The first one is the imported object followed by
#          material groups.
#  @note Auto publishing is allowed for the shape itself. Imported
#        material groups are not automatically published.
#  @ingroup l2_import_export
def ImportBREP(self, theFileName, theName=None):
    """
    Import a shape from the BREP file

    Parameters: 
        theFileName The file, containing the shape.
        theName Object name; when specified, this parameter is used
                for result publication in the study. Otherwise, if automatic
                publication is switched on, default value is used for result name.

    Returns:
        New GEOM.GEOM_Object, containing the imported shape.
        If material names are imported it returns the list of
        objects. The first one is the imported object followed by
        material groups.
    Note:
        Auto publishing is allowed for the shape itself. Imported
        material groups are not automatically published.
    """
    from salome.geom.geomBuilder import RaiseIfFailed
    anOp = GetBREPPluginOperations(self)
    aListObj = anOp.ImportBREP(theFileName)
    RaiseIfFailed("ImportBREP", anOp)
    aNbObj = len(aListObj)
    if aNbObj > 0:
        self._autoPublish(aListObj[0], theName, "imported")
    if aNbObj == 1:
        return aListObj[0]
    return aListObj
