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

from GEOM import IVTKOperations

# Engine Library Name
__libraryName__ = "VTKPluginEngine"

def GetVTKPluginOperations(self):
    anOp = self.GetPluginOperations(self.myStudyId, __libraryName__)
    return anOp._narrow(IVTKOperations)

## Export the given shape into a file with given name in VTK format.
#  @param theObject Shape to be stored in the file.
#  @param theFileName Name of the file to store the given shape in.
#  @param theDeflection Deflection of the given shape.
#  @ingroup l2_import_export
def ExportVTK(self, theObject, theFileName, theDeflection=0.001):
    """
    Export the given shape into a file with given name in VTK format.

    Parameters: 
        theObject Shape to be stored in the file.
        theFileName Name of the file to store the given shape in.
        theIsASCII The format of the exported file (ASCII or Binary).
        theDeflection Deflection of the given shape.
    """
    anOp = GetVTKPluginOperations(self)
    anOp.ExportVTK(theObject, theFileName, theDeflection)
    if anOp.IsDone() == 0:
        raise RuntimeError,  "Export : " + anOp.GetErrorCode()
        pass
    pass
