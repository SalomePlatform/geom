# -*- coding: utf-8 -*-
# Copyright (C) 2014-2022  EDF R&D, OPEN CASCADE
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
# Author : Alexey SOZINOV, Open CASCADE S.A.S.


## @defgroup check_conformity CheckConformity - Wrapper to find imperfections in the shape
#  @{ 
#  @details
#  This tool provides the user with a simple python API 
#  to analyze, available shape for Boolean Operations or not.
#  Also tool provide advanced output to indicate imperfections in the input shape.
#  @n Example:
#  @code
#  import GEOM
#  from salome.geom import geomBuilder
#  from salome.geom.CheckConformity import CheckConformity
#
#  geompy = geomBuilder.New()
#
#  O = geompy.MakeVertex(0, 0, 0)
#  OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
#  OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
#  OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
#  Vertex_1 = geompy.MakeVertex(-30, -70, 0)
#  Vertex_2 = geompy.MakeVertex(-30, 50, 0)
#  Line_1 = geompy.MakeLineTwoPnt(Vertex_2, Vertex_1)
#  Vertex_3 = geompy.MakeVertex(0, -50, 0)
#  Vertex_4 = geompy.MakeVertex(-40, -10, 0)
#  Vertex_5 = geompy.MakeVertex(0, 40, 0)
#  Arc_1 = geompy.MakeArc(Vertex_5, Vertex_4, Vertex_3)
#  Vertex_6 = geompy.MakeVertex(10, -50, 4)
#  Vertex_7 = geompy.MakeVertex(10, -50, 10)
#  Vertex_8 = geompy.MakeVertex(10, 40, 10)
#  Arc_1_vertex_3 = geompy.GetSubShape(Arc_1, [3])
#  Line_2 = geompy.MakeLineTwoPnt(Arc_1_vertex_3, Vertex_6)
#  Line_3 = geompy.MakeLineTwoPnt(Vertex_6, Vertex_7)
#  Line_4 = geompy.MakeLineTwoPnt(Vertex_7, Vertex_8)
#  Vertex_9 = geompy.MakeVertex(15, 40, 10)
#  Vertex_10 = geompy.MakeVertex(17, 0, 6)
#  Vertex_11 = geompy.MakeVertex(17, 0, 3)
#  Line_5 = geompy.MakeLineTwoPnt(Vertex_8, Vertex_9)
#  Line_6 = geompy.MakeLineTwoPnt(Vertex_9, Vertex_10)
#  Line_7 = geompy.MakeLineTwoPnt(Vertex_10, Vertex_11)
#  Arc_1_vertex_2 = geompy.GetSubShape(Arc_1, [2])
#  Line_8 = geompy.MakeLineTwoPnt(Vertex_11, Arc_1_vertex_2)
#  Wire_1 = geompy.MakeWire([Arc_1, Line_2, Line_3, Line_4, Line_5, Line_6, Line_7, Line_8], 1e-07)
#  Wire_2 = geompy.MakeWire([Line_1], 1e-07)
#  Compound_1 = geompy.MakeCompound([Wire_1, Wire_2])
#
#  # Create class CheckConformity for check shape
#  cc = CheckConformity(Compound_1, geompy)
#  valid = cc.isValid()
#  dist = cc.distantShapes()
#  small = cc.smallEdges()
#  interfer = cc.interferingSubshapes()
#  intersect = cc.selfIntersected2D()
#
#  geompy.addToStudy( O, 'O' )
#  geompy.addToStudy( OX, 'OX' )
#  geompy.addToStudy( OY, 'OY' )
#  geompy.addToStudy( OZ, 'OZ' )
#  geompy.addToStudy( Vertex_1, 'Vertex_1' )
#  geompy.addToStudy( Vertex_2, 'Vertex_2' )
#  geompy.addToStudy( Line_1, 'Line_1' )
#  geompy.addToStudy( Vertex_3, 'Vertex_3' )
#  geompy.addToStudy( Vertex_4, 'Vertex_4' )
#  geompy.addToStudy( Vertex_5, 'Vertex_5' )
#  geompy.addToStudy( Arc_1, 'Arc_1' )
#  geompy.addToStudy( Vertex_6, 'Vertex_6' )
#  geompy.addToStudy( Vertex_7, 'Vertex_7' )
#  geompy.addToStudy( Vertex_8, 'Vertex_8' )
#  geompy.addToStudyInFather( Arc_1, Arc_1_vertex_3, 'Arc_1:vertex_3' )
#  geompy.addToStudy( Line_2, 'Line_2' )
#  geompy.addToStudy( Line_3, 'Line_3' )
#  geompy.addToStudy( Line_4, 'Line_4' )
#  geompy.addToStudy( Vertex_9, 'Vertex_9' )
#  geompy.addToStudy( Vertex_10, 'Vertex_10' )
#  geompy.addToStudy( Vertex_11, 'Vertex_11' )
#  geompy.addToStudy( Line_5, 'Line_5' )
#  geompy.addToStudy( Line_6, 'Line_6' )
#  geompy.addToStudy( Line_7, 'Line_7' )
#  geompy.addToStudyInFather( Arc_1, Arc_1_vertex_2, 'Arc_1:vertex_2' )
#  geompy.addToStudy( Line_8, 'Line_8' )
#  geompy.addToStudy( Wire_1, 'Wire_1' )
#  geompy.addToStudy( Wire_2, 'Wire_2' )
#  geompy.addToStudy( Compound_1, 'Compound_1' )
#  
#  salome.sg.updateObjBrowser()
#
#  @endcode
#  @n Additional examples can be found as unit tests in the source code.
#  @}


## An interface to find imperfections in the shape.
#  Use geompy.CheckConformity(shape) method to obtain an instance of this class
#
#  @ref tui_check_conformity_page "Example"
#  @ingroup check_conformity
class CheckConformity:
    AUTO = -1

    """
    Check Conformity interface

    Example of usage:
        cc = CheckConformity(Lot4_twistedFace_brep_1, geompy)
        valid = cc.isValid()
        dist = cc.distantShapes(geompy.ShapeType["EDGE"], geompy.ShapeType["VERTEX"])
        small = cc.smallEdges()
        interfer = cc.interferingSubshapes()
        interferEV = cc.interferingSubshapes(geompy.ShapeType["EDGE"], geompy.ShapeType["VERTEX"])
        interferVV = cc.interferingSubshapes(geompy.ShapeType["VERTEX], geompy.ShapeType["VERTEX"])
        intersect = cc.selfIntersected2D()
    """

    def __init__(self, shape, geompyD):
        self.geompyD = geompyD
        self.myShape = shape
        self.myIsChecked = False;
        self.myResults = []
      
    ##  Perform analyse of shape.
    #
    #  @return New List, which contains pair: type of check and single of pair failed sub-shapes.
    def __checkShape(self): 
        """
        Perform analyse of shape.

        Returns:
            New List, which contains pair: type of check and single of pair failed sub-shapes.
        """
        anOp = self.geompyD.GetIMeasureOperations()
        self.myResults = anOp.CheckConformityShape(self.myShape)
        self.myIsChecked = True

    ## Check whether the shape is applicable for Boolean Operations.
    #
    #  @return Boolean value True if shape is applicable for Boolean Operations, otherwise - False
    def isValid(self):
        """
        check whether the shape is applicable for Boolean Operations.

        Returns:
            Boolean value True if shape is applicable for Boolean Operations, otherwise - False.

        Example of usage:
            Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
            cc = CheckConformity(Box_1, geompy)
            isValid = cc.isValid()
        """
        if not self.myIsChecked:
            self.__checkShape()
        return len(self.myResults) == 0
       
    ## Find all self-intersected 2D curves.
    #
    #  @return New List, of pair sub-shape, which has self-intersected in 2D
    def selfIntersected2D(self):
        """
        Find all self-intersected 2D curves.

        Returns:
            New List, of pair sub-shape, which has self-intersected in 2D/

        Example of usage:
            Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
            cc = CheckConformity(Box_1, geompy)
            selfIntersected2D = cc.selfIntersected2D()
        """
        if not self.myIsChecked:
            self.__checkShape()

        anOp = self.geompyD.GetIMeasureOperations()
        return anOp.SelfIntersected2D(self.myResults)

    ## Find pairs of interfering sub-shapes:
    #  - vertices touched by tolerance;
    #  - vertex touching an edge in the inner point;
    #  - vertex lying on the inner point of a face;
    #  - edges intersecting by inner points;
    #  - edge touching/intersecting face in the inner point;
    #  - faces intersection by inner point
    #
    # Types of interfering shapes could be specified,
    # by default all pairs of interfering shapes are returned.
    #
    #  @param shapeType1 first type of shape
    #  @param shapeType2 second type of shape
    #
    #  @return New List, of pairs of interfering shapes with given types (if they was specified)
    def interferingSubshapes(self, shapeType1 = AUTO, shapeType2 = AUTO):
        """
        Find pairs of interfering sub-shapes:
         - vertices touched by tolerance
         - vertex touching an edge in the inner point
         - vertex lying on the inner point of a face
         - edges intersecting by inner points
         - edge touching/intersecting face in the inner point
         - faces intersection by inner point
        
         Types of interfering shapes could be specified, by default all pairs of 
         interfering shapes are returned.

        Parameters:
            shapeType1 first type of shape
            shapeType2 second type of shape

        Returns:
            New List, of pairs of interfering shapes with given types (if they was specified)

        Example of usage:
            Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
            cc = CheckConformity(Box_1, geompy)
            interferingSubshapes = cc.interferingSubshapes()
            interferingSubshapesVV = cc.interferingSubshapes(geompy.ShapeType["VERTEX"], geompy.ShapeType["VERTEX"])
        """
        if not self.myIsChecked:
            self.__checkShape()

        anOp = self.geompyD.GetIMeasureOperations()
        return anOp.InterferingSubshapes(self.myResults, shapeType1, shapeType2)
       
    ## Find edges, which are fully covered by tolerances of vertices.
    #
    #  @return New List of edges, which are fully covered by tolerances of vertices
    def smallEdges(self):
        """
        Find edges, which are fully covered by tolerances of vertices.

        Returns:
            New List of edges, which are fully covered by tolerances of vertices.

        Example of usage:
            Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
            cc = CheckConformity(Box_1, geompy)
            smallEdges = cc.smallEdges()
        """
        if not self.myIsChecked:
            self.__checkShape()

        anOp = self.geompyD.GetIMeasureOperations()
        return anOp.SmallEdges(self.myResults)
       
    ## Find remote objects (sub-shape on a shape):
    #  - vertex far from edge;
    #  - vertex far from face;
    #  - edge far from face
    #
    #  Sub-shape which are lying far from its parent shape more than the given tolerance.
    #
    #  @param shapeType type of shape
    #  @param subShapeType type of sub-shape
    #  @param tolerance available tolerance, by default used tolerance of sub-shape.
    #
    #  @return New List, of pair of sub-shape with given types,
    #          that lying far from its parent shape more than the given tolerance.
    def distantShapes(self, shapeType = AUTO, subShapeType = AUTO, tolerance = -1.0):
        """
        Find remote objects (sub-shape on a shape):
         - vertex far from edge;
         - vertex far from face;
         - edge far from face

        Sub-shape which are lying far from its parent shape more than the given tolerance.

        Parameters:
            shapeType type of shape
            subShapeType type of sub-shape
            tolerance available tolerance, by default used tolerance of sub-shape.

        Returns:
            New List, of pair of sub-shape with given types,
                that lying far from its parent shape more than the given tolerance

        Example of usage:
            Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
            cc = CheckConformity(Box_1, geompy)
            distantShapes = cc.distantShapes(geompy.ShapeType["EDGE"], geompy.ShapeType["VERTEX"])
        """
        if not self.myIsChecked:
            self.__checkShape()

        anOp = self.geompyD.GetIMeasureOperations()
        return anOp.DistantShapes(self.myResults, shapeType, subShapeType, tolerance)
        
    ## Compute possible tolerance for the shape,
    #   minimize tolerance of shape as well as tolerance of sub-shapes as much as possible  .
    #
    #  @return New value of tolerance.
    def updateTolerance(self):
        """
        Compute possible tolerance for the shape,
        minimize tolerance of shape as well as tolerance of sub-shapes as much as possible.

        Returns:
            New value of tolerance.

        Example of usage:
            Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
            cc = CheckConformity(Box_1, geompy)
            toler = cc.updateTolerance()
        """
        anOp = self.geompyD.GetIMeasureOperations()
        return anOp.UpdateTolerance(self.myShape)
