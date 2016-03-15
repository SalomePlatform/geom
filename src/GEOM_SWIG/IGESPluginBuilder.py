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

from GEOM import IIGESOperations

# Engine Library Name
__libraryName__ = "IGESPluginEngine"

def GetIGESPluginOperations(self):
    anOp = self.GetPluginOperations(self.myStudyId, __libraryName__)
    return anOp._narrow(IIGESOperations)

## Export the given shape into a file with given name in IGES format.
#  @param theObject Shape to be stored in the file.
#  @param theFileName Name of the file to store the given shape in.
#  @param theVersion Version of IGES format which defines, whether to write 
#                    only faces (5.1 IGES format) or shells and solids also (5.3 IGES format).
#  @ingroup l2_import_export
def ExportIGES(self, theObject, theFileName, theVersion="5.1"):
    """
    Export the given shape into a file with given name in IGES format.

    Parameters: 
        theObject Shape to be stored in the file.
        theFileName Name of the file to store the given shape in.
        theVersion Version of IGES format which defines, whether to write 
        only faces (5.1 IGES format) or shells and solids also (5.3 IGES format).
    """
    anOp = GetIGESPluginOperations(self)
    anOp.ExportIGES(theObject, theFileName, theVersion)
    if anOp.IsDone() == 0:
        raise RuntimeError,  "Export : " + anOp.GetErrorCode()
        pass
    pass

## Import a shape from the IGES file with given name.
#  @param theFileName The file, containing the shape.
#  @param theIsIgnoreUnits If True, file length units will be ignored (set to 'meter')
#         and result model will be scaled, if its units are not meters.
#         If False (default), file length units will be taken into account.
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
#
#  @ref swig_Import_Export "Example"
#  @ingroup l2_import_export
def ImportIGES(self, theFileName, theIsIgnoreUnits = False, theName=None):
    """
    Import a shape from the IGES file with given name.

    Parameters:
        theFileName The file, containing the shape.
        ignoreUnits If True, file length units will be ignored (set to 'meter')
                    and result model will be scaled, if its units are not meters.
                    If False (default), file length units will be taken into account.
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
    # Example: see GEOM_TestOthers.py
    from salome.geom.geomBuilder import RaiseIfFailed
    anOp = GetIGESPluginOperations(self)
    
    anIsIgnoreUnits = theIsIgnoreUnits
    aName = theName
    if isinstance( theIsIgnoreUnits, basestring ):
        anIsIgnoreUnits = False
        aName = theIsIgnoreUnits
        pass

    aListObj = anOp.ImportIGES(theFileName,anIsIgnoreUnits)
    RaiseIfFailed("ImportIGES", anOp)
    aNbObj = len(aListObj)
    if aNbObj > 0:
        self._autoPublish(aListObj[0], aName, "imported")
    if aNbObj == 1:
        return aListObj[0]
    return aListObj

## Return length unit from given IGES file
#  @param theFileName The file, containing the shape.
#  @return String, containing the units name.
#
#  @ref swig_Import_Export "Example"
#  @ingroup l2_import_export
def GetIGESUnit(self, theFileName):
    """
    Return length units from given IGES file

    Parameters:
        theFileName The file, containing the shape.

    Returns:
        String, containing the units name.
    """
    # Example: see GEOM_TestOthers.py
    anOp = GetIGESPluginOperations(self)
    aUnitName = anOp.ReadValue( theFileName, "LEN_UNITS")
    return aUnitName
