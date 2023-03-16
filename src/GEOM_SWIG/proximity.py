#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2022-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

## @defgroup proximity ShapeProximity - tool to check the proximity distance between two shapes
#  @{ 
#  @details
#  The tool provides the user a possibility to compute the proximity distance between two shapes:
#  * a minimal diameter of a tube containing both edges (for edge-edge proximity);
#  * a minimal thickness of a shell containing both faces (for face-face proximity).
#
#  In other words, this tool calculate maximal of all possible distances between pair of objects.
#  It is supported to compute distance between two edges or between two faces.
#  Other combinations of shapes are prohibited.
#  @}

"""
    \namespace geompy
    \brief ShapeProximity interface
"""

## A class to compute proximity value between two shapes.
#  Use geompy.ShapeProximity() method to obtain an instance of this class.
#
#  @ref tui_proximity_page "Example"
#  @ingroup proximity
class ShapeProximity():
    """
    ShapeProximity interface.

    Example of usage:
        prox = geompy.ShapeProximity()
        value = prox.proximity(shape1, shape2)
    """

    def __init__(self, geompyD):
        self.myCommand = "ShapeProximity"
        self.myOp = geompyD.GetIMeasureOperations()
        pass

    ## Computes proximity between two shapes of the same type
    def proximity(self, shape1, shape2):
        self.setShapes(shape1, shape2)
        #self.coarseProximity()
        return self.preciseProximity()
        pass

    ## Customize object with the input shapes
    def setShapes(self, shape1, shape2):
        self.myProximityValue = None
        from salome.geom.geomBuilder import RaiseIfFailed
        self.myCalc = self.myOp.ShapeProximityCalculator(shape1, shape2)
        RaiseIfFailed(self.myCommand, self.myOp)
        pass

    ## Define the minimal number of sample points for the given shape,
    #  which should be used for raw computation of proximity value
    def setSampling(self, shape, nbSamples):
        self.myOp.SetShapeSampling(self.myCalc, shape, nbSamples)
        pass

    ## Find rough proximity value based on polygon/tessellation representation
    def coarseProximity(self):
        from salome.geom.geomBuilder import RaiseIfFailed
        self.myProximityValue = self.myOp.GetCoarseProximity(self.myCalc)
        RaiseIfFailed(self.myCommand, self.myOp)
        return self.myProximityValue
        pass

    ## Find precise proximity value based on exact shapes
    def preciseProximity(self):
        from salome.geom.geomBuilder import RaiseIfFailed
        self.myProximityValue = self.myOp.GetPreciseProximity(self.myCalc)
        RaiseIfFailed(self.myCommand, self.myOp)
        return self.myProximityValue
        pass
