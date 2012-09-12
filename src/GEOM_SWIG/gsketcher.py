#  -*- coding: iso-8859-1 -*-
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
#  File   : gsketcher.py
#  Author : Julia DOROVSKIKH, Open CASCADE S.A.S.
#  Module : GEOM_SWIG

"""
    \namespace geompy
    \brief 3D Sketcher interface
"""

# This method is used by 3D Sketcher functionality
def printVar (var):
    if isinstance(var, str):
        return "\'%s\'"%var
    else:
        return "%.7f"%var

## An interface to build a 3D Sketcher step-by-step.
#  Use geompy.Sketcher3D() method to obtain an instance of this class.
#
#  @ref tui_3dsketcher_page "Example"
class Sketcher3D:
    """
    3D sketcher interface.

    Example of usage:
        sk = geompy.Sketcher3D()
        sk.addPointsAbsolute(0,0,0, 70,0,0)
        sk.addPointsRelative(0, 0, 130)
        sk.addPointAnglesLength("OXY", 50, 0, 100)
        sk.addPointAnglesLength("OXZ", 30, 80, 130)
        sk.close()
        a3D_Sketcher_1 = sk.wire()
    """

    def __init__(self, geompyD):
        self.geompyD = geompyD
        self.myCommand = "3DSketcher"
        pass

    ## Add one or more points, sequentially connected with straight segments.
    #  Coordinates are considered as absolute.
    #  If the first point of sketcher is not yet defined, the first point
    #  from the listCoords will become the first sketcher point.
    #  @param X1, Y1, Z1, X2, Y2, Z2, ... Coordinates of points
    def addPointsAbsolute (self, *listCoords):
        """
        Add one or more points, sequentially connected with straight segments.
        Coordinates are considered as absolute.
        If the first point of sketcher is not yet defined, the first point
        from the listCoords will become the first sketcher point.

        Parameters:
            X1, Y1, Z1, X2, Y2, Z2, ... Coordinates of points

        Example of usage:
            sk = geompy.Sketcher3D()
            sk.addPointsAbsolute(0,0,0, 70,0,0)
            a3D_Sketcher_1 = sk.wire()
        """
        ii = 1
        for cc in listCoords:
            if ii == 1:
                self.myCommand = self.myCommand + ":TT"
            self.myCommand = self.myCommand + " %s"%printVar(cc)
            if ii == 3:
                ii = 1
            else:
                ii = ii + 1
        pass

    ## Add one or more points, sequentially connected with straight segments.
    #  Coordinates are considered relative to the previous point.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0, 0) will become the first sketcher point.
    #  @param X1, Y1, Z1, X2, Y2, Z2, ... Coordinates of points
    def addPointsRelative (self, *listCoords):
        """
        Add one or more points, sequentially connected with straight segments.
        Coordinates are considered relative to the previous point.
        If the first point of sketcher is not yet defined, the
        origin (0, 0, 0) will become the first sketcher point.

        Parameters:
            X1, Y1, Z1, X2, Y2, Z2, ... Relative coordinates of points

        Example of usage:
            sk = geompy.Sketcher3D()
            sk.addPointsRelative(0,0,130, 70,0,-130)
            a3D_Sketcher_1 = sk.wire()
        """
        ii = 1
        for cc in listCoords:
            if ii == 1:
                self.myCommand = self.myCommand + ":T"
            self.myCommand = self.myCommand + " %s"%printVar(cc)
            if ii == 3:
                ii = 1
            else:
                ii = ii + 1
        pass

    ## Add one straight segment, defined by two angles and length.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0, 0) will become the first sketcher point.
    #  @param axes can be: "OXY", "OYZ" or "OXZ"
    #  @param angle1 angle in a plane, defined by the \a axes
    #  @param angle2 angle from the plane, defined by the \a axes
    #  @param length length of the segment
    def addPointAnglesLength (self, axes, angle1, angle2, length):
        """
        Add one straight segment, defined by two angles and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0, 0) will become the first sketcher point.

        Parameters:
            axes can be: "OXY", "OYZ" or "OXZ"
            angle1 angle in a plane, defined by the \a axes
            angle2 angle from the plane, defined by the \a axes
            length length of the segment

        Example of usage:
            sk = geompy.Sketcher3D()
            sk.addPointAnglesLength("OXY", 50, 0, 100)
            a3D_Sketcher_1 = sk.wire()
        """
        self.myCommand = self.myCommand + ":%s %s %s %s" % (axes, printVar(angle1), printVar(angle2), printVar(length))
        pass

    ## Set to close the wire
    def close (self):
        """
        Set to close the wire

        Example of usage:
            sk = geompy.Sketcher3D()
            sk.addPointsRelative(0,0,130, 70,0,-130)
            sk.close()
            a3D_Sketcher_1 = sk.wire()
        """
        self.myCommand = self.myCommand + ":WW"
        pass

    ## Obtain the sketcher result.
    #  @return New GEOM_Object, containing the created wire
    def wire (self):
        """
        Obtain the sketcher result.

        Returns:
            New GEOM_Object, containing the created wire.

        Example of usage:
            sk = geompy.Sketcher3D()
            sk.addPointsRelative(0,0,130, 70,0,-130)
            a3D_Sketcher_1 = sk.wire()
        """
        from geompyDC import ParseSketcherCommand, RaiseIfFailed
        Command,Parameters = ParseSketcherCommand(self.myCommand)
        wire = self.geompyD.CurvesOp.Make3DSketcherCommand(Command)
        self.myCommand = "3DSketcher"
        RaiseIfFailed("Sketcher3D", self.geompyD.CurvesOp)
        wire.SetParameters(Parameters)
        return wire
