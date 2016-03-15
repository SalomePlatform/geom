#  -*- coding: iso-8859-1 -*-
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
from GEOM import IAdvancedOperations

# Engine Library Name
__libraryName__ = "AdvancedEngine"

def GetAdvancedOperations(self):
    anOp = self.GetPluginOperations(self.myStudyId, __libraryName__)
    return anOp._narrow(IAdvancedOperations)

## Create a T-shape object with specified caracteristics for the main
#  and the incident pipes (radius, width, half-length).
#  The extremities of the main pipe are located on junctions points P1 and P2.
#  The extremity of the incident pipe is located on junction point P3.
#  If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
#  the main plane of the T-shape is XOY.
#
#  @param theR1 Internal radius of main pipe
#  @param theW1 Width of main pipe
#  @param theL1 Half-length of main pipe
#  @param theR2 Internal radius of incident pipe (R2 < R1)
#  @param theW2 Width of incident pipe (R2+W2 < R1+W1)
#  @param theL2 Half-length of incident pipe
#
#  @param theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
#  @param theP1 1st junction point of main pipe
#  @param theP2 2nd junction point of main pipe
#  @param theP3 Junction point of incident pipe
#
#  @param theRL Internal radius of left thickness reduction
#  @param theWL Width of left thickness reduction
#  @param theLtransL Length of left transition part
#  @param theLthinL Length of left thin part
#
#  @param theRR Internal radius of right thickness reduction
#  @param theWR Width of right thickness reduction
#  @param theLtransR Length of right transition part
#  @param theLthinR Length of right thin part
#
#  @param theRI Internal radius of incident thickness reduction
#  @param theWI Width of incident thickness reduction
#  @param theLtransI Length of incident transition part
#  @param theLthinI Length of incident thin part
#
#  @param theName Object name; when specified, this parameter is used
# for result publication in the study. Otherwise, if automatic
# publication is switched on, default value is used for result name.
#
#  @return List of GEOM.GEOM_Object, containing the created shape and propagation groups.
#
#  @ref tui_creation_pipetshape "Example"
#  @ingroup l4_advanced
def MakePipeTShape (self, theR1, theW1, theL1, theR2, theW2, theL2,
    theHexMesh=True, theP1=None, theP2=None, theP3=None,
    theRL=0, theWL=0, theLtransL=0, theLthinL=0,
    theRR=0, theWR=0, theLtransR=0, theLthinR=0,
    theRI=0, theWI=0, theLtransI=0, theLthinI=0,
    theName=None):
    """
    Create a T-shape object with specified caracteristics for the main
    and the incident pipes (radius, width, half-length).
    The extremities of the main pipe are located on junctions points P1 and P2.
    The extremity of the incident pipe is located on junction point P3.
    If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
    the main plane of the T-shape is XOY.

    Parameters:
        theR1 Internal radius of main pipe
        theW1 Width of main pipe
        theL1 Half-length of main pipe
        theR2 Internal radius of incident pipe (R2 < R1)
        theW2 Width of incident pipe (R2+W2 < R1+W1)
        theL2 Half-length of incident pipe
        theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
        theP1 1st junction point of main pipe
        theP2 2nd junction point of main pipe
        theP3 Junction point of incident pipe
        
        theRL Internal radius of left thickness reduction
        theWL Width of left thickness reduction
        theLtransL Length of left transition part
        theLthinL Length of left thin part
        
        theRR Internal radius of right thickness reduction
        theWR Width of right thickness reduction
        theLtransR Length of right transition part
        theLthinR Length of right thin part
        
        theRI Internal radius of incident thickness reduction
        theWI Width of incident thickness reduction
        theLtransI Length of incident transition part
        theLthinI Length of incident thin part
        
        theName Object name; when specified, this parameter is used
        for result publication in the study. Otherwise, if automatic
        publication is switched on, default value is used for result name.
    
    Returns:
    List of GEOM_Object, containing the created shape and propagation groups.
    
    Example of usage:
    # create PipeTShape object
    pipetshape = geompy.MakePipeTShape(80.0, 20.0, 200.0, 50.0, 20.0, 200.0)
    # create PipeTShape object with position
    pipetshape_position = geompy.MakePipeTShape(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, True, P1, P2, P3)
    # create PipeTShape object with left thickness reduction
    pipetshape_thr = geompy.MakePipeTShape(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, theRL=60, theWL=20, theLtransL=40, theLthinL=20)
    """
    from salome.geom.geomBuilder import ParseParameters, RaiseIfFailed, _toListOfNames
    theR1, theW1, theL1, theR2, theW2, theL2, theRL, theWL, theLtransL, theLthinL, theRR, theWR, theLtransR, theLthinR, theRI, theWI, theLtransI, theLthinI, Parameters = ParseParameters(theR1, theW1, theL1, theR2, theW2, theL2, theRL, theWL, theLtransL, theLthinL, theRR, theWR, theLtransR, theLthinR, theRI, theWI, theLtransI, theLthinI)
    anOp = GetAdvancedOperations(self)
    if (theP1 and theP2 and theP3):
        anObj = anOp.MakePipeTShapeTRWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                  theRL, theWL, theLtransL, theLthinL,
                                                  theRR, theWR, theLtransR, theLthinR,
                                                  theRI, theWI, theLtransI, theLthinI,
                                                  theHexMesh, theP1, theP2, theP3)
    else:
        anObj = anOp.MakePipeTShapeTR(theR1, theW1, theL1, theR2, theW2, theL2,
                                      theRL, theWL, theLtransL, theLthinL,
                                      theRR, theWR, theLtransR, theLthinR,
                                      theRI, theWI, theLtransI, theLthinI,
                                      theHexMesh)
        pass
    RaiseIfFailed("MakePipeTShape", anOp)
    if Parameters: anObj[0].SetParameters(Parameters)
    self.def_names = [ "pipeTShape" ] + [ "pipeTShape_grp_%d" % i for i in range(1, len(anObj)) ]
    self._autoPublish(anObj, _toListOfNames(theName, len(anObj)), self.def_names)
    anOp.UnRegister()
    return anObj

## Create a T-shape object with chamfer and with specified caracteristics for the main
#  and the incident pipes (radius, width, half-length). The chamfer is
#  created on the junction of the pipes.
#  The extremities of the main pipe are located on junctions points P1 and P2.
#  The extremity of the incident pipe is located on junction point P3.
#  If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
#  the main plane of the T-shape is XOY.
#  @param theR1 Internal radius of main pipe
#  @param theW1 Width of main pipe
#  @param theL1 Half-length of main pipe
#  @param theR2 Internal radius of incident pipe (R2 < R1)
#  @param theW2 Width of incident pipe (R2+W2 < R1+W1)
#  @param theL2 Half-length of incident pipe
#  @param theH Height of the chamfer.
#  @param theW Width of the chamfer.
#  @param theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
#  @param theP1 1st junction point of main pipe
#  @param theP2 2nd junction point of main pipe
#  @param theP3 Junction point of incident pipe
#
#  @param theRL Internal radius of left thickness reduction
#  @param theWL Width of left thickness reduction
#  @param theLtransL Length of left transition part
#  @param theLthinL Length of left thin part
#
#  @param theRR Internal radius of right thickness reduction
#  @param theWR Width of right thickness reduction
#  @param theLtransR Length of right transition part
#  @param theLthinR Length of right thin part
#
#  @param theRI Internal radius of incident thickness reduction
#  @param theWI Width of incident thickness reduction
#  @param theLtransI Length of incident transition part
#  @param theLthinI Length of incident thin part
#
#  @param theName Object name; when specified, this parameter is used
#         for result publication in the study. Otherwise, if automatic
#         publication is switched on, default value is used for result name.
#
#  @return List of GEOM.GEOM_Object, containing the created shape and propagation groups.
#
#  @ref tui_creation_pipetshape "Example"
#  @ingroup l4_advanced
def MakePipeTShapeChamfer (self, theR1, theW1, theL1, theR2, theW2, theL2,
                           theH, theW, theHexMesh=True, theP1=None, theP2=None, theP3=None,
                           theRL=0, theWL=0, theLtransL=0, theLthinL=0,
                           theRR=0, theWR=0, theLtransR=0, theLthinR=0,
                           theRI=0, theWI=0, theLtransI=0, theLthinI=0,
                           theName=None):
    """
    Create a T-shape object with chamfer and with specified caracteristics for the main
    and the incident pipes (radius, width, half-length). The chamfer is
    created on the junction of the pipes.
    The extremities of the main pipe are located on junctions points P1 and P2.
    The extremity of the incident pipe is located on junction point P3.
    If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
    the main plane of the T-shape is XOY.
    
    Parameters:
        theR1 Internal radius of main pipe
        theW1 Width of main pipe
        theL1 Half-length of main pipe
        theR2 Internal radius of incident pipe (R2 < R1)
        theW2 Width of incident pipe (R2+W2 < R1+W1)
        theL2 Half-length of incident pipe
        theH Height of the chamfer.
        theW Width of the chamfer.
        theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
        theP1 1st junction point of main pipe
        theP2 2nd junction point of main pipe
        theP3 Junction point of incident pipe
        
        theRL Internal radius of left thickness reduction
        theWL Width of left thickness reduction
        theLtransL Length of left transition part
        theLthinL Length of left thin part
        
        theRR Internal radius of right thickness reduction
        theWR Width of right thickness reduction
        theLtransR Length of right transition part
        theLthinR Length of right thin part
        
        theRI Internal radius of incident thickness reduction
        theWI Width of incident thickness reduction
        theLtransI Length of incident transition part
        theLthinI Length of incident thin part
        
        theName Object name; when specified, this parameter is used
        for result publication in the study. Otherwise, if automatic
        publication is switched on, default value is used for result name.
        
        Returns:
            List of GEOM_Object, containing the created shape and propagation groups.

        Example of usage:
            # create PipeTShape with chamfer object
            pipetshapechamfer = geompy.MakePipeTShapeChamfer(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 20.0, 20.0)
            # create PipeTShape with chamfer object with position
            pipetshapechamfer_position = geompy.MakePipeTShapeChamfer(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 20.0, 20.0, True, P1, P2, P3)
            # create PipeTShape with chamfer object with left thickness reduction
            pipetshapechamfer_thr = geompy.MakePipeTShapeChamfer(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 20.0, 20.0, theRL=60, theWL=20, theLtransL=40, theLthinL=20)
    """
    from salome.geom.geomBuilder import ParseParameters, RaiseIfFailed, _toListOfNames
    theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, theRL, theWL, theLtransL, theLthinL, theRR, theWR, theLtransR, theLthinR, theRI, theWI, theLtransI, theLthinI, Parameters = ParseParameters(theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, theRL, theWL, theLtransL, theLthinL, theRR, theWR, theLtransR, theLthinR, theRI, theWI, theLtransI, theLthinI)
    anOp = GetAdvancedOperations(self)
    if (theP1 and theP2 and theP3):
        anObj = anOp.MakePipeTShapeTRChamferWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                         theRL, theWL, theLtransL, theLthinL,
                                                         theRR, theWR, theLtransR, theLthinR,
                                                         theRI, theWI, theLtransI, theLthinI,
                                                         theH, theW, theHexMesh, theP1, theP2, theP3)
    else:
        anObj = anOp.MakePipeTShapeTRChamfer(theR1, theW1, theL1, theR2, theW2, theL2,
                                             theRL, theWL, theLtransL, theLthinL,
                                             theRR, theWR, theLtransR, theLthinR,
                                             theRI, theWI, theLtransI, theLthinI,
                                             theH, theW, theHexMesh)
    pass

    RaiseIfFailed("MakePipeTShapeChamfer", anOp)
    if Parameters: anObj[0].SetParameters(Parameters)
    self.def_names = [ "pipeTShape" ] + [ "pipeTShape_grp_%d" % i for i in range(1, len(anObj)) ]
    self._autoPublish(anObj, _toListOfNames(theName, len(anObj)), self.def_names)
    anOp.UnRegister()
    return anObj

## Create a T-shape object with fillet and with specified caracteristics for the main
#  and the incident pipes (radius, width, half-length). The fillet is
#  created on the junction of the pipes.
#  The extremities of the main pipe are located on junctions points P1 and P2.
#  The extremity of the incident pipe is located on junction point P3.
#  If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
#  the main plane of the T-shape is XOY.
#  @param theR1 Internal radius of main pipe
#  @param theW1 Width of main pipe
#  @param theL1 Half-length of main pipe
#  @param theR2 Internal radius of incident pipe (R2 < R1)
#  @param theW2 Width of incident pipe (R2+W2 < R1+W1)
#  @param theL2 Half-length of incident pipe
#  @param theRF Radius of curvature of fillet.
#  @param theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
#  @param theP1 1st junction point of main pipe
#  @param theP2 2nd junction point of main pipe
#  @param theP3 Junction point of incident pipe
#
#  @param theRL Internal radius of left thickness reduction
#  @param theWL Width of left thickness reduction
#  @param theLtransL Length of left transition part
#  @param theLthinL Length of left thin part
#
#  @param theRR Internal radius of right thickness reduction
#  @param theWR Width of right thickness reduction
#  @param theLtransR Length of right transition part
#  @param theLthinR Length of right thin part
#
#  @param theRI Internal radius of incident thickness reduction
#  @param theWI Width of incident thickness reduction
#  @param theLtransI Length of incident transition part
#  @param theLthinI Length of incident thin part
#
#  @param theName Object name; when specified, this parameter is used
#         for result publication in the study. Otherwise, if automatic
#         publication is switched on, default value is used for result name.
#
#  @return List of GEOM.GEOM_Object, containing the created shape and propagation groups.
#
#  @ref tui_creation_pipetshape "Example"
#  @ingroup l4_advanced
def MakePipeTShapeFillet (self, theR1, theW1, theL1, theR2, theW2, theL2,
                          theRF, theHexMesh=True, theP1=None, theP2=None, theP3=None,
                          theRL=0, theWL=0, theLtransL=0, theLthinL=0,
                          theRR=0, theWR=0, theLtransR=0, theLthinR=0,
                          theRI=0, theWI=0, theLtransI=0, theLthinI=0,
                          theName=None):
    """
    Create a T-shape object with fillet and with specified caracteristics for the main
    and the incident pipes (radius, width, half-length). The fillet is
    created on the junction of the pipes.
    The extremities of the main pipe are located on junctions points P1 and P2.
    The extremity of the incident pipe is located on junction point P3.
    
    Parameters:
        If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
        the main plane of the T-shape is XOY.
        theR1 Internal radius of main pipe
        theW1 Width of main pipe
        heL1 Half-length of main pipe
        theR2 Internal radius of incident pipe (R2 < R1)
        theW2 Width of incident pipe (R2+W2 < R1+W1)
        theL2 Half-length of incident pipe
        theRF Radius of curvature of fillet.
        theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
        theP1 1st junction point of main pipe
        theP2 2nd junction point of main pipe
        theP3 Junction point of incident pipe
        
        theRL Internal radius of left thickness reduction
        theWL Width of left thickness reduction
        theLtransL Length of left transition part
        theLthinL Length of left thin part
        
        theRR Internal radius of right thickness reduction
        theWR Width of right thickness reduction
        theLtransR Length of right transition part
        theLthinR Length of right thin part
        
        theRI Internal radius of incident thickness reduction
        theWI Width of incident thickness reduction
        theLtransI Length of incident transition part
        theLthinI Length of incident thin part
        
        theName Object name; when specified, this parameter is used
        for result publication in the study. Otherwise, if automatic
        publication is switched on, default value is used for result name.
                
        Returns:
            List of GEOM_Object, containing the created shape and propagation groups.
                
        Example of usage:
            # create PipeTShape with fillet object
            pipetshapefillet = geompy.MakePipeTShapeFillet(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 5.0)
            # create PipeTShape with fillet object with position
            pipetshapefillet_position = geompy.MakePipeTShapeFillet(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 5.0, True, P1, P2, P3)
            # create PipeTShape with fillet object with left thickness reduction
            pipetshapefillet_thr = geompy.MakePipeTShapeFillet(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 5.0, theRL=60, theWL=20, theLtransL=40, theLthinL=20)
    """
    from salome.geom.geomBuilder import ParseParameters, RaiseIfFailed, _toListOfNames
    theR1, theW1, theL1, theR2, theW2, theL2, theRF, theRL, theWL, theLtransL, theLthinL, theRR, theWR, theLtransR, theLthinR, theRI, theWI, theLtransI, theLthinI, Parameters = ParseParameters(theR1, theW1, theL1, theR2, theW2, theL2, theRF, theRL, theWL, theLtransL, theLthinL, theRR, theWR, theLtransR, theLthinR, theRI, theWI, theLtransI, theLthinI)
    anOp = GetAdvancedOperations(self)    
    if (theP1 and theP2 and theP3):
        anObj = anOp.MakePipeTShapeTRFilletWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                        theRL, theWL, theLtransL, theLthinL,
                                                        theRR, theWR, theLtransR, theLthinR,
                                                        theRI, theWI, theLtransI, theLthinI,
                                                        theRF, theHexMesh, theP1, theP2, theP3)
    else:
        anObj = anOp.MakePipeTShapeTRFillet(theR1, theW1, theL1, theR2, theW2, theL2,
                                                  theRL, theWL, theLtransL, theLthinL,
                                                  theRR, theWR, theLtransR, theLthinR,
                                                  theRI, theWI, theLtransI, theLthinI,
                                                  theRF, theHexMesh)
        pass
    RaiseIfFailed("MakePipeTShapeFillet", anOp)
    if Parameters: anObj[0].SetParameters(Parameters)
    self.def_names = [ "pipeTShape" ] + [ "pipeTShape_grp_%d" % i for i in range(1, len(anObj)) ]
    self._autoPublish(anObj, _toListOfNames(theName, len(anObj)), self.def_names)
    anOp.UnRegister()
    return anObj

## This function allows creating a disk already divided into blocks. It
#  can be used to create divided pipes for later meshing in hexaedra.
#  @param theR Radius of the disk
#  @param theOrientation Orientation of the plane on which the disk will be built
#         1 = XOY, 2 = OYZ, 3 = OZX
#  @param thePattern Division pattern. It can be GEOM.SQUARE or GEOM.HEXAGON
#  @param theName Object name; when specified, this parameter is used
#         for result publication in the study. Otherwise, if automatic
#         publication is switched on, default value is used for result name.
#
#  @return New GEOM_Object, containing the created shape.
#
#  @ref tui_creation_divideddisk "Example"
#  @ingroup l4_advanced
def MakeDividedDisk(self, theR, theOrientation, thePattern, theName=None):
    """
    Creates a disk, divided into blocks. It can be used to create divided pipes
    for later meshing in hexaedra.
    
    Parameters:
        theR Radius of the disk
        theOrientation Orientation of the plane on which the disk will be built:
        1 = XOY, 2 = OYZ, 3 = OZX
        thePattern Division pattern. It can be GEOM.SQUARE or GEOM.HEXAGON
        theName Object name; when specified, this parameter is used
        for result publication in the study. Otherwise, if automatic
        publication is switched on, default value is used for result name.
        
        Returns:
            New GEOM_Object, containing the created shape.
    """
    from salome.geom.geomBuilder import ParseParameters, RaiseIfFailed, _toListOfNames
    theR, Parameters = ParseParameters(theR)
    anOp = GetAdvancedOperations(self)
    anObj = anOp.MakeDividedDisk(theR, 67.0, theOrientation, thePattern)
    RaiseIfFailed("MakeDividedDisk", anOp)
    if Parameters: anObj.SetParameters(Parameters)
    self._autoPublish(anObj, theName, "dividedDisk")
    anOp.UnRegister()
    return anObj
            
## This function allows creating a disk already divided into blocks. It
#  can be used to create divided pipes for later meshing in hexaedra.
#  @param theCenter Center of the disk
#  @param theVector Normal vector to the plane of the created disk
#  @param theRadius Radius of the disk
#  @param thePattern Division pattern. It can be GEOM.SQUARE or GEOM.HEXAGON
#  @param theName Object name; when specified, this parameter is used
#         for result publication in the study. Otherwise, if automatic
#         publication is switched on, default value is used for result name.
#
#  @return New GEOM_Object, containing the created shape.
#
#  @ref tui_creation_divideddisk "Example"
#  @ingroup l4_advanced
def MakeDividedDiskPntVecR(self, theCenter, theVector, theRadius, thePattern, theName=None):
    """
    Creates a disk already divided into blocks. It can be used to create divided pipes
    for later meshing in hexaedra.
    
    Parameters:
        theCenter Center of the disk
        theVector Normal vector to the plane of the created disk
        theRadius Radius of the disk
        thePattern Division pattern. It can be GEOM.SQUARE or GEOM.HEXAGON
        theName Object name; when specified, this parameter is used
        for result publication in the study. Otherwise, if automatic
        publication is switched on, default value is used for result name.

    Returns:
        New GEOM_Object, containing the created shape.
    """
    from salome.geom.geomBuilder import ParseParameters, RaiseIfFailed, _toListOfNames
    theRadius, Parameters = ParseParameters(theRadius)
    anOp = GetAdvancedOperations(self)
    anObj = anOp.MakeDividedDiskPntVecR(theCenter, theVector, theRadius, 67.0, thePattern)
    RaiseIfFailed("MakeDividedDiskPntVecR", anOp)
    if Parameters: anObj.SetParameters(Parameters)
    self._autoPublish(anObj, theName, "dividedDisk")
    anOp.UnRegister()
    return anObj

## Builds a cylinder prepared for hexa meshes
#  @param theR Radius of the cylinder
#  @param theH Height of the cylinder
#  @param thePattern Division pattern. It can be GEOM.SQUARE or GEOM.HEXAGON
#  @param theName Object name; when specified, this parameter is used
#         for result publication in the study. Otherwise, if automatic
#         publication is switched on, default value is used for result name.
#
#  @return New GEOM_Object, containing the created shape.
#
#  @ref tui_creation_dividedcylinder "Example"
#  @ingroup l4_advanced
def MakeDividedCylinder(self, theR, theH, thePattern, theName=None):
    """
    Builds a cylinder prepared for hexa meshes
    
    Parameters:
        theR Radius of the cylinder
        theH Height of the cylinder
        thePattern Division pattern. It can be GEOM.SQUARE or GEOM.HEXAGON
        theName Object name; when specified, this parameter is used
        for result publication in the study. Otherwise, if automatic
        publication is switched on, default value is used for result name.
        
        Returns:
        New GEOM_Object, containing the created shape.
    """
    from salome.geom.geomBuilder import ParseParameters, RaiseIfFailed, _toListOfNames
    theR, theH, Parameters = ParseParameters(theR, theH)
    anOp = GetAdvancedOperations(self)
    anObj = anOp.MakeDividedCylinder(theR, theH, thePattern)
    RaiseIfFailed("MakeDividedCylinder", anOp)
    if Parameters: anObj.SetParameters(Parameters)
    self._autoPublish(anObj, theName, "dividedCylinder")
    anOp.UnRegister()
    return anObj

## Create a surface from a cloud of points
#  @param thelPoints list of points. Compounds of points are
#         accepted as well.
#  @param theNbMax maximum number of Bezier pieces in the resulting
#         surface.
#  @param theDegMax maximum degree of the resulting BSpline surface.
#  @param theDMax 3D tolerance of initial approximation.
#  @param theName Object name; when specified, this parameter is used
#         for result publication in the study. Otherwise, if automatic
#         publication is switched on, default value is used for result name.
#  @return New GEOM_Object, containing the created shape.
#  @note 3D tolerance of initial approximation represents a tolerance of
#        initial plate surface approximation. If this parameter is equal
#        to 0 (default value) it is computed. In this case an error of
#        initial plate surface computation is used as the approximation
#        tolerance. This error represents a maximal distance between
#        computed plate surface and given points.
#
#  @ref tui_creation_smoothingsurface "Example"
#  @ingroup l4_advanced
def MakeSmoothingSurface(self, thelPoints, theNbMax=2, theDegMax=8,
                         theDMax=0.0, theName=None):
    """
    Create a surface from a cloud of points
    
    Parameters:
        thelPoints list of points. Compounds of points are
                   accepted as well.
        theNbMax maximum number of Bezier pieces in the resulting
                 surface.
        theDegMax maximum degree of the resulting BSpline surface.
        theDMax 3D tolerance of initial approximation.
        theName Object name; when specified, this parameter is used
                for result publication in the study. Otherwise, if automatic
                publication is switched on, default value is used for result name.

    Returns:
        New GEOM_Object, containing the created shape.

    Note:
        3D tolerance of initial approximation represents a tolerance of
        initial plate surface approximation. If this parameter is equal
        to 0 (default value) it is computed. In this case an error of
        initial plate surface computation is used as the approximation
        tolerance. This error represents a maximal distance between
        computed plate surface and given points.
    """
    from salome.geom.geomBuilder import RaiseIfFailed
    anOp = GetAdvancedOperations(self)
    anObj = anOp.MakeSmoothingSurface(thelPoints, theNbMax,
                                      theDegMax, theDMax)
    RaiseIfFailed("MakeSmoothingSurface", anOp)
    self._autoPublish(anObj, theName, "smoothing")
    anOp.UnRegister()
    return anObj
