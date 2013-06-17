#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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
    \brief 2D and 3D Sketcher interfaces
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
        sk.addPointRadiusAnglesRelative(50, 0, 100, 'OXY')
        sk.addPointRadiusAnglesRelative(30, 80, 130, 'OXZ')
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
    #  @param listCoords X1, Y1, Z1, X2, Y2, Z2, ... Coordinates of points
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
    #  @param listCoords X1, Y1, Z1, X2, Y2, Z2, ... Coordinates of points
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
    #  The radius and angles coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param length length of the segment
    #  @param angle1 angle in a plane, defined by the \a axes
    #  @param angle2 angle from the plane, defined by the \a axes
    #  @param axes can be: "OXY", "OYZ" or "OXZ"
    def addPointRadiusAnglesRelative (self, length, angle1, angle2, axes="OXY"):
        """
        Add one straight segment, defined by two angles and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0, 0) will become the first sketcher point.

        Parameters:
            length length of the segment
            angle1 angle in a plane, defined by the \a axes
            angle2 angle from the plane, defined by the \a axes
            axes can be: "OXY", "OYZ" or "OXZ"

        Example of usage:
            sk = geompy.Sketcher3D()
            sk.addPointRadiusAnglesRelative(100, 50, 0, "OXY")
            a3D_Sketcher_1 = sk.wire()
        """
        self.myCommand = self.myCommand + ":%s"%axes+"SR"+" %s %s %s" % (printVar(angle1), printVar(angle2), printVar(length))
        pass
    
    ## Add one straight segment, defined by two angles and radius.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0, 0) will become the first sketcher point.
    #  The radius and angles coordinates are defined 
    #  in a coordinate system which origin is the global coordinate system origin
    #  
    #  @param radius distance to the coordinate system origin
    #  @param angle1 angle in a plane, defined by the \a axes
    #  @param angle2 angle from the plane, defined by the \a axes
    #  @param axes can be: "OXY", "OYZ" or "OXZ"
    def addPointRadiusAnglesAbsolute (self, radius, angle1, angle2, axes="OXY"):
        """
        Add one straight segment, defined by two angles and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0, 0) will become the first sketcher point.

        Parameters:
            radius distance to the coordinate system origin
            angle1 angle in a plane, defined by the \a axes
            angle2 angle from the plane, defined by the \a axes
            axes can be: "OXY", "OYZ" or "OXZ"

        Example of usage:
            sk = geompy.Sketcher3D()
            sk.addPointRadiusAnglesAbsolute(100, 50, 0, "OXY")
            a3D_Sketcher_1 = sk.wire()
        """
        self.myCommand = self.myCommand + ":%s"%axes+"SA"+" %s %s %s" % (printVar(angle1), printVar(angle2), printVar(radius))
        pass
    
    ## Add one straight segment, defined by an angle, a height and a radius.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0, 0) will become the first sketcher point.
    #  The radius height and angle coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #  
    #  @param axes can be: "OXY", "OYZ" or "OXZ"
    #  @param angle angle in a plane, defined by the \a axes
    #  @param height height from the plane, defined by the \a axes
    #  @param length distance to the coordinate system origin
    def addPointRadiusAngleHRelative (self, length, angle, height, axes="OXY"):
        """
        Add one straight segment, defined by two angles and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0, 0) will become the first sketcher point.

        Parameters:
            radius distance to the coordinate system origin
            angle  angle in a plane, defined by the \a axes
            height height from the plane, defined by the \a axes
            axes can be: "OXY", "OYZ" or "OXZ"

        Example of usage:
            sk = geompy.Sketcher3D()
            sk.addPointRadiusAngleHRelative(100, 50, 40, "OXY")
            a3D_Sketcher_1 = sk.wire()
        """
        self.myCommand = self.myCommand + ":%s"%axes+"CR"+" %s %s %s" % (printVar(angle), printVar(height), printVar(length))
        pass
    
    ## Add one straight segment, defined by an angle, a height and a radius.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0, 0) will become the first sketcher point.
    #  The radius height and angle coordinates are defined 
    #  in a coordinate system which origin is the global coordinate system origin
    # 
    #  @param radius distance to the coordinate system origin
    #  @param angle angle in a plane, defined by the \a axes
    #  @param height height from the plane, defined by the \a axes
    #  @param axes can be: "OXY", "OYZ" or "OXZ"
    def addPointRadiusAngleHAbsolute (self, radius, angle, height, axes="OXY"):
        """
        Add one straight segment, defined by two angles and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0, 0) will become the first sketcher point.

        Parameters:
            axes can be: "OXY", "OYZ" or "OXZ"
            angle1 angle in a plane, defined by the \a axes
            height height from the plane, defined by the \a axes
            radius distance to the coordinate system origin

        Example of usage:
            sk = geompy.Sketcher3D()
            sk.addPointRadiusAngleHAbsolute( 100, 50, 40, "OXY")
            a3D_Sketcher_1 = sk.wire()
        """
        self.myCommand = self.myCommand + ":%s"%axes+"CA"+" %s %s %s" % (printVar(angle), printVar(height), printVar(radius))
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
    #  @param theName Object name; when specified, this parameter is used
    #         for result publication in the study. Otherwise, if automatic
    #         publication is switched on, default value is used for result name.
    #
    #  @return New GEOM_Object, containing the created wire
    def wire (self, theName=None):
        """
        Obtain the sketcher result.

        Parameters:
            theName Object name; when specified, this parameter is used
                    for result publication in the study. Otherwise, if automatic
                    publication is switched on, default value is used for result name.

        Returns:
            New GEOM_Object, containing the created wire.

        Example of usage:
            sk = geompy.Sketcher3D()
            sk.addPointsRelative(0,0,130, 70,0,-130)
            a3D_Sketcher_1 = sk.wire()
        """
        from salome.geom.geomBuilder import ParseSketcherCommand, RaiseIfFailed
        Command,Parameters = ParseSketcherCommand(self.myCommand)
        wire = self.geompyD.CurvesOp.Make3DSketcherCommand(Command)
        self.myCommand = "3DSketcher"
        RaiseIfFailed("Sketcher3D", self.geompyD.CurvesOp)
        wire.SetParameters(Parameters)
        self.geompyD._autoPublish(wire, theName, "wire")
        return wire

#  An interface to build a 2D Sketcher step-by-step.
#  Use geompy.Sketcher2D() method to obtain an instance of this class.
  
class Sketcher2D:
    """
    2D sketcher interface.

    Example of usage:
        sk = geompy.Sketcher2D()
        sk.addPoint(20, 20)
        sk.addSegmentRelative(15, 70)
        sk.addSegmentPerpY(50)
        sk.addArcRadiusRelative(25, 15, 14.5, 0)
        sk.addArcCenterAbsolute(1, 1, 50, 50, 0, 0)
        sk.addArcDirectionRadiusLength(20, 20, 101, 162.13)
        sk.close()
        Sketch_1 = sk.wire(geomObj_1)
    """
    def __init__(self, geompyD):
        self.geompyD = geompyD
        self.myCommand = "Sketcher"
        self.closed = False
        pass
    
    ## Add one point.
    #  Used to set the starting point
    #
    #  @param x, y - Coordinates of point
    def addPoint (self, x, y): 
        """
        Add one point.
        Used to set the starting point

        Parameters:
            x, y - Coordinates of point

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addPoint(20, 20)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":F %s %s" % (printVar(x), printVar(y))
        pass
    
    ## Add angle.
    #  Used to set the angle for further building
    #
    #  @param angle - angle in a plane
    def addAngle (self, angle): 
        """
        Add angle.
        Used to set the angle for further building

        Parameters:
            angle - angle in a plane

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addAngle(70)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":RR %s" % (printVar(angle))
        pass

    #  Commands for creation of segment
    
    ## Add segment, which sequentially connects the previous point
    #  with a point lying on a line parallel to the axis Ox and the value x = param x.
    #  The current point will be the previous value to the coordinate y and 
    #  new value to the coordinate x.
    #  Coordinate is considered relative to the previous point.
    #
    #  @param x - Coordinate of point on axis Ox 
    def addSegmentParalX(self, x):
        """
        Add segment, which sequentially connects the previous point
        with a point lying on a line parallel to the axis Ox and the value x = param x.
        The current point will be the previous value to the coordinate y and 
        new value to the coordinate x.
        Coordinate is considered relative to the previous point.

        Parameters:
            x - Coordinate of point on axis Ox 

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentParalX(40)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":X %s" % (printVar(x))
        pass
    
    ## Add segment, which sequentially connects the previous point
    #  with a point lying on a line parallel to the axis Ox and the value x = 0
    #  The current point will be the previous value to the coordinate y.
    def addSegmentParalXToZero(self):
        """
        Add segment, which sequentially connects the previous point
        with a point lying on a line parallel to the axis Ox and the value x = 0
        The current point will be the previous value to the coordinate y.

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentParalXToZero()
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":XX"
        pass    
    
    ## Add segment, which sequentially connects the previous point
    #  with a point lying on a line parallel to the axis Oy and the value y = param y.
    #  The current point will be the previous value to the coordinate x and 
    #  new value to the coordinate y.
    #  Coordinate is considered relative to the previous point.
    #
    #  @param y - Coordinate of point on axis Oy 
    def addSegmentParalY(self, y):
        """
        Add segment, which sequentially connects the previous point
        with a point lying on a line parallel to the axis Oy and the value y = param y.
        The current point will be the previous value to the coordinate x and 
        new value to the coordinate y.
        Coordinate is considered relative to the previous point.

        Parameters:
            y - Coordinate of point on axis Oy 

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentParalY(80)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":Y %s" % (printVar(y))
        pass
    
    ## Add segment, which sequentially connects the previous point
    #  with a point lying on a line parallel to the axis Ox and the value x = 0
    #  The current point will be the previous value to the coordinate y.
    def addSegmentParalYToZero(self):
        """
        Add segment, which sequentially connects the previous point
        with a point lying on a line parallel to the axis Oy and the value y = 0
        The current point will be the previous value to the coordinate x.

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentParalYToZero()
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":YY"
        pass 
           
    ## Add segment, which sequentially connects the given point with previous point
    #
    #  Coordinates are considered as absolute.
    #  @param x, y - Coordinates of point
    def addSegmentAbsolute (self, x, y):
        """
        Add segment, which sequentially connects the starting point with the given point
        Coordinates are considered as absolute.

        Parameters:
            x, y - Coordinates of point

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentAbsolute(30, 50)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":TT %s %s" % (printVar(x), printVar(y))
        pass

    ## Add segment, which sequentially connects the given point with previous point
    #  Coordinates are considered relative to the previous point.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #
    #  @param dx, dy - Coordinates of point relative a previous point
    def addSegmentRelative (self, dx, dy):
        """
        Add segment, which sequentially connects the given point with previous point
        Coordinates are considered relative to the previous point.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.

        Parameters:
            dx, dy - Coordinates of point relative a previous point

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentRelative(60, 10)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":T %s %s" % (printVar(dx), printVar(dy))
        pass
    
    ## Add one straight segment, defined by angle and length.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The angle and length coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param angle - angle in a plane
    #  @param length - length of the segment
    def addSegmentAngleLength (self, angle, length ):
        """
        Add one straight segment, defined by angle and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The radius and angles coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            angle - angle in a plane
            length - length of the segment

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentAngleLength(10, 30)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":R %s:L %s" % (printVar(angle), printVar(length))
        pass
    
    ## Add one straight segment, defined by angle and intersect straight x= param x.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The angle and point's coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param angle - angle in a plane
    #  @param x - value on the axis Ox
    def addSegmentAngleX (self, angle, x ):
        """
        Add one straight segment, defined by angle and intersect straight x= param x.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The angle and point's coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            angle - angle in a plane
            x - value on the axis Ox

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentAngleX(25, 90)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":R %s:IX %s" % (printVar(angle), printVar(x))
        pass
    
    ## Add one straight segment, defined by angle and intersect straight y= param y.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The angle and point's coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param angle - angle in a plane
    #  @param y - value on the axis Oy
    def addSegmentAngleY (self, angle, y ):
        """
        Add one straight segment, defined by angle and intersect straight y= param y.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The angle and point's coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            angle - angle in a plane
            y - value on the axis Oy

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentAngleY(40, 0)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":R %s:IY %s" % (printVar(angle), printVar(y))
        pass
    
    
    ## Add one straight segment, defined by perpendicular(angle=90) and length.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The length coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param length - length of the segment
    def addSegmentPerpLength (self, length ):
        """
        Add one straight segment, defined by perpendicular and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The length coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            length - length of the segment

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentPerpLength(100)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":R 90:L %s" % (printVar(length))
        pass
    
    ## Add one straight segment, defined by perpendicular(angle=90) and intersect straight x= param x.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The point's coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param x - value on the axis Ox
    def addSegmentPerpX (self, x ):
        """
        Add one straight segment, defined by perpendicular(angle=90) and intersect straight x= param x.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The point's coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            x - value on the axis Ox

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentPerpX(30)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":R 90:IX %s" % (printVar(x))
        pass

    ## Add one straight segment, defined by perpendicular(angle=90) and intersect straight y= param y.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The point's coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param y - value on the axis Oy    
    def addSegmentPerpY (self, y ):
        """
        Add one straight segment, defined by perpendicular(angle=90) and intersect straight y= param y.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The point's coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            y - value on the axis Oy

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentPerpY(10)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":R 90:IY %s" % (printVar(y))
        pass
    
    
    ## Add one straight segment, defined by previous direction and length.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The length coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param length - length of the segment
    def addSegmentLength (self, length ):
        """
        Add one straight segment, defined by previous direction and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The length coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            length - length of the segment

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentLength(100)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":L %s" % (printVar(length))
        pass
 
    ## Add one straight segment, defined by previous direction and intersect straight x= param x.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The point's coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param x - value on the axis Ox   
    def addSegmentX (self, x ):
        """
        Add one straight segment, defined by previous direction and intersect straight x= param x.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The point's coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            x - value on the axis Ox

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentX(30)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":IX %s" % (printVar(x))
        pass
    
    ## Add one straight segment, defined by previous direction and intersect straight y= param y.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The point's coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param y - value on the axis Oy   
    def addSegmentY (self, y ):
        """
        Add one straight segment, defined by previous direction and intersect straight y= param y.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The point's coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            y - value on the axis Oy

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentY(10)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":IY %s" % (printVar(y))
        pass
    

    ## Add one straight segment, defined by direction and length.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The direction and length coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param dx, dy - direction of the segment    
    #  @param length - length of the segment   
    def addSegmentDirectionLength (self, dx, dy, length ):
        """
        Add one straight segment, defined by direction and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The direction and length coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            dx, dy - direction of the segment
            length - length of the segment

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentDirectionLength(20, 40, 30)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":D %s %s:L %s" % (printVar(dx), printVar(dy), printVar(length))
        pass

    ## Add one straight segment, defined by direction and intersect straight x= param x.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The direction and point's coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param dx, dy - direction of the segment
    #  @param x - value on the axis Ox    
    def addSegmentDirectionX (self, dx, dy, x ):
        """
        Add one straight segment, defined by direction and intersect straight x= param x.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The direction and point's coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            dx, dy - direction of the segment
            x - value on the axis Ox

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentDirectionX(10, -40, 20)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":D %s %s:IX %s" % (printVar(dx), printVar(dy), printVar(x))
        pass
    
    ## Add one straight segment, defined by direction and intersect straight y= param y.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #  The direction and point's coordinates are defined 
    #  in a local coordinate system which origin is the last point of the sketch
    #
    #  @param dx, dy - direction of the segment
    #  @param y - value on the axis Oy  
    def addSegmentDirectionY (self, dx, dy, y ):
        """
        Add one straight segment, defined by direction and intersect straight y= param y.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.
        The direction and point's coordinates are defined 
        in a local coordinate system which origin is the last point of the sketch

        Parameters:
            dx, dy - direction of the segment
            y - value on the axis Oy 

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addSegmentDirectionY(-10, -50, 20)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":D %s %s:IY %s" % (printVar(dx), printVar(dy), printVar(y))
        pass
    
    #  Commands for creation of arc
    
    ## Add arc, which connects the given point with previous point
    #  Coordinates are considered as absolute.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #
    #  @param x, y - Coordinates of second point of arc    
    def addArcAbsolute (self, x, y ):
        """
        Add arc, which connects the given point with previous point
        Coordinates are considered as absolute.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point. 

        Parameters:
            param x, y - Coordinates of second point of arc

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addArcAbsolute(50, 10)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":AA %s %s" % (printVar(x), printVar(y))
        pass
 
    ## Add arc, which connects the given point with previous point
    #  Coordinates are considered relative to the previous point.
    #
    #  @param dx, dy - Coordinates of second point of arc relative to the previous point   
    def addArcRelative (self, dx, dy ):
        """
        Add arc, which connects the given point with previous point
        Coordinates are considered relative to the previous point. 

        Parameters:
            param dx, dy - Coordinates of second point of arc

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addArcRelative(50, 10)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":A %s %s" % (printVar(dx), printVar(dy))
        pass
 
    ## Add arc, defined by radius and coordinates of next point.
    #  Coordinates are considered as absolute.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #
    #  @param x, y - Coordinates of second point of arc
    #  @param radius - radius of arc 
    #  @param flag - is 0 or 2
    #                if 0 the drawn arc is the one of lower angle (<Pi)
    #                if 2 the drawn arc is the one of greater angle (>Pi)
    def addArcRadiusAbsolute (self, x, y, radius, flag ):
        """
        Add arc, defined by radius and coordinates of next point.
        Coordinates are considered as absolute.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.

        Parameters:
            param x, y - Coordinates of second point of arc
            param radius - radius of arc 
            param flag - is 0 or 2
                         if 0 the drawn arc is the one of lower angle (<Pi)
                         if 2 the drawn arc is the one of greater angle (>Pi)

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addArcRadiusAbsolute(50, 10, 20, 0)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":UU %s %s %s %s" % (printVar(x), printVar(y), printVar(radius), printVar(flag))
        pass
 
    ## Add arc, defined by radius and coordinates of next point.
    #  Coordinates are considered relative to the previous point.
    #
    #  @param dx, dy - Coordinates of second point of arc
    #  @param radius - radius of arc 
    #  @param flag - is 0 or 2
    #                if 0 the drawn arc is the one of lower angle (<Pi)
    #                if 2 the drawn arc is the one of greater angle (>Pi)   
    def addArcRadiusRelative (self, dx, dy, radius, flag ):
        """
        Add arc, defined by radius and coordinates of next point.
        Coordinates are considered relative to the previous point.

        Parameters:
            param dx, dy - Coordinates of second point of arc
            param radius - radius of arc 
            param flag - is 0 or 2
                         if 0 the drawn arc is the one of lower angle (<Pi)
                         if 2 the drawn arc is the one of greater angle (>Pi)

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addArcRadiusRelative(-30, -15, 20, 2)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":U %s %s %s %s" % (printVar(dx), printVar(dy), printVar(radius), printVar(flag))
        pass
 
    ## Add arc, defined by coordinates of next point and coordinates of center.
    #  Coordinates are considered as absolute.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #
    #  @param x, y - Coordinates of second point of arc
    #  @param xc, yc - Coordinates of center
    #  @param flag1 - (reverse) is 0 or 2
    #                if 0 the drawn arc is the one of lower angle (<Pi)
    #                if 2 the drawn arc is the one of greater angle (>Pi)  
    #  @param flag2 - (control tolerance) is 0 or 1
    #                if 0 the specified end point can be at a distance of the arc   
    def addArcCenterAbsolute (self, x, y, xc, yc, flag1, flag2 ):
        """
        Add arc, defined by coordinates of next point and coordinates of center.
        Coordinates are considered as absolute.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.

        Parameters:
            param x, y - Coordinates of second point of arc
            param xc, yc - Coordinates of center
            param flag1 - is 0 or 2
                         if 0 the drawn arc is the one of lower angle (<Pi)
                         if 2 the drawn arc is the one of greater angle (>Pi)
            param flag2 - (control tolerance) is 0 or 1
                         if 0 the specified end point can be at a distance of the arc 

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addArcCenterAbsolute(-30, -15, 20, 10, 0, 0)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":EE %s %s %s %s %s %s" % (printVar(xc), printVar(yc), printVar(x), printVar(y),  
                                                                     printVar(flag1), printVar(flag2))
        pass

    ## Add arc, defined by coordinates of next point and coordinates of center.
    #  Coordinates are considered relative to the previous point.
    #
    #  @param dx, dy - Coordinates of second point of arc
    #  @param xc, yc - Coordinates of center
    #  @param flag1 - (reverse) is 0 or 2
    #                if 0 the drawn arc is the one of lower angle (<Pi)
    #                if 2 the drawn arc is the one of greater angle (>Pi)  
    #  @param flag2 - (control tolerance) is 0 or 1
    #                if 0 the specified end point can be at a distance of the arc     
    def addArcCenterRelative (self, dx, dy, xc, yc, flag1, flag2 ):
        """
        Add arc, defined by coordinates of next point and coordinates of center.
        Coordinates are considered relative to the previous point.

        Parameters:
            param dx, dy - Coordinates of second point of arc
            param xc, yc - Coordinates of center
            param flag1 - is 0 or 2
                         if 0 the drawn arc is the one of lower angle (<Pi)
                         if 2 the drawn arc is the one of greater angle (>Pi)
            param flag2 - (control tolerance) is 0 or 1
                         if 0 the specified end point can be at a distance of the arc 

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addArcCenterRelative(-30, -15, 20, 10, 2, 1)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":E %s %s %s %s %s %s" % (printVar(xc), printVar(yc), printVar(dx), printVar(dy), 
                                                                    printVar(flag1), printVar(flag2))
        pass
 
    ## Add arc, defined by angle, radius and length.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #
    #  @param angle - angle in a plane
    #  @param radius - radius of the arc 
    #  @param length - length of the arc     
    def addArcAngleRadiusLength (self, angle, radius, length ):
        """
        Add arc, defined by angle, radius and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.

        Parameters:
            param angle - angle in a plane
            param radius - radius of the arc 
            param length - length of the arc     

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addArcAngleRadiusLength(30, 15, 40)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":R %s:C %s %s" % (printVar(angle), printVar(radius), printVar(length))
        pass

    ## Add arc, defined by perpendicular(angle=90), radius and length.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #
    #  @param radius - radius of the arc 
    #  @param length - length of the arc    
    def addArcPerpRadiusLength (self, radius, length ):
        """
        Add arc, defined by perpendicular(angle=90), radius and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.

        Parameters:
            param radius - radius of the arc 
            param length - length of the arc     

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addArcPerpRadiusLength(15, 40)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":R 90:C %s %s" % (printVar(radius), printVar(length))
        pass

    ## Add arc, defined by previous direction, radius and length.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #
    #  @param radius - radius of the arc 
    #  @param length - length of the arc        
    def addArcRadiusLength (self, radius, length ):
        """
        Add arc, defined by previous direction, radius and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.

        Parameters:
            param radius - radius of the arc 
            param length - length of the arc     

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addArcRadiusLength(15, 40)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":C %s %s" % (printVar(radius), printVar(length))
        pass
 
    ## Add arc, defined by direction, radius and length.
    #  If the first point of sketcher is not yet defined, the
    #  origin (0, 0) will become the first sketcher point.
    #
    #  @param dx, dy - direction of the arc
    #  @param radius - radius of the arc 
    #  @param length - length of the arc     
    def addArcDirectionRadiusLength (self, dx, dy, radius, length ):
        """
        Add arc, defined by direction, radius and length.
        If the first point of sketcher is not yet defined, the
        origin (0, 0) will become the first sketcher point.

        Parameters:
            param dx, dy - direction of the arc
            param radius - radius of the arc 
            param length - length of the arc     

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addArcDirectionRadiusLength(-50, 40, 20, 3.5)
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.myCommand = self.myCommand + ":D %s %s:C %s %s" % (printVar(dx), printVar(dy), printVar(radius), printVar(length))
        pass

    ## Set to close the wire    
    def close (self):
        """
        Set to close the wire

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addPoint(15, 85.6)
            sk.close()
            Sketcher_1 = sk.wire(geomObj_1)
        """
        self.closed = True
        pass

    ## Obtain the sketcher result as a wire.
    #
    #  @param WorkingPlane - current Working Plane used for this 2DSketcher
    #  @param theName Object name; when specified, this parameter is used
    #         for result publication in the study. Otherwise, if automatic
    #         publication is switched on, default value is used for result name.
    #
    #  @return New GEOM_Object, containing the created wire    
    def wire (self, WorkingPlane=[0, 0, 0, 0, 0, 1, 1, 0, 0], theName=None):
        """
        Obtain the sketcher result as a wire.

        Parameters:
            theName Object name; when specified, this parameter is used
                    for result publication in the study. Otherwise, if automatic
                    publication is switched on, default value is used for result name
            param WorkingPlane - current Working Plane used for this 2DSketcher

        Returns:
            New GEOM_Object, containing the created wire.

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addPoint(30, 70)
            a3D_Sketcher_1 = sk.wire(geomObj_1)
        """

        if self.closed:
            self.myCommand = self.myCommand + ":WW"

        import GEOM
        if isinstance(WorkingPlane, list): wire = self.geompyD.CurvesOp.MakeSketcher(self.myCommand, WorkingPlane)
        if isinstance(WorkingPlane, GEOM._objref_GEOM_Object): wire = self.geompyD.CurvesOp.MakeSketcherOnPlane(self.myCommand, WorkingPlane)

        self.myCommand = "Sketcher"
        self.geompyD._autoPublish(wire, theName, "wire")
        return wire
        
    ## Obtain the sketcher result as a face.
    #
    #  @param WorkingPlane - current Working Plane used for this 2DSketcher
    #  @param theName Object name; when specified, this parameter is used
    #         for result publication in the study. Otherwise, if automatic
    #         publication is switched on, default value is used for result name.
    #
    #  @return New GEOM_Object, containing the created face    
    def face (self, WorkingPlane=[0, 0, 0, 0, 0, 1, 1, 0, 0], theName=None):
        """
        Obtain the sketcher result as a face.

        Parameters:
            theName Object name; when specified, this parameter is used
                    for result publication in the study. Otherwise, if automatic
                    publication is switched on, default value is used for result name
            param WorkingPlane - current Working Plane used for this 2DSketcher

        Returns:
            New GEOM_Object, containing the created face.

        Example of usage:
            sk = geompy.Sketcher2D()
            sk.addPoint(0, 0)
            sk.addSegment(100, 0)
            sk.addSegment(100, 100)
            sk.addSegment(0, 100)
            sk.close()
            a3D_Sketcher_1 = sk.face(geomObj_1)
        """
    
        if self.closed:
            self.myCommand = self.myCommand + ":WF"
        else:
            raise RuntimeError, "Sketcher2D.close() : can't build face on unclosed wire"

        import GEOM
        if isinstance(WorkingPlane, list): face = self.geompyD.CurvesOp.MakeSketcher(self.myCommand, WorkingPlane)
        if isinstance(WorkingPlane, GEOM._objref_GEOM_Object): face = self.geompyD.CurvesOp.MakeSketcherOnPlane(self.myCommand, WorkingPlane)

        self.myCommand = "Sketcher"
        self.geompyD._autoPublish(face, theName, "face")
        return face
