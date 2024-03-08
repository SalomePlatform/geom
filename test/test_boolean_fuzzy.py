#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

import salome
###
salome.salome_init()

import GEOM
from salome.geom import geomBuilder
import math
import SALOMEDS

geompy = geomBuilder.New()

import unittest

def HasSameSubShapes( shape, expected_info ):
    """
    Compare topology information about shape with expected data
    param shape - shape to be checked
    param expected_info - expected WhatIsInformation for the shape
    return True, if number of shapes are equal - False, otherwise
    """
    name = shape.GetName()
    if name:
        name = '"%s"' % name
    what_is = geompy.WhatIs( shape )
    what_is_list = what_is.strip().split( "\n" )
    # Remove flat content
    if what_is_list.count( " Flat content : " ):
        wIndex = what_is_list.index( " Flat content : " )
        what_is_list = what_is_list[:( wIndex - len( what_is_list ) - 1 )]
    got_info_dict = {}
    for item in what_is_list:
        pair = item.split( ":" )
        if len(pair)==2:
            type = item.split( ":" )[0].strip()
            value = item.split( ":" )[1].strip()
            if type.find( "Number of sub-shapes" ) == -1:
                got_info_dict[type] = value
    if len( expected_info ) > len( got_info_dict ):
        print( "ERROR!!! Got topology information about shape %s isn't complete..." % name )
        return False
    for key in expected_info:
        if key not in got_info_dict:
            print( "ERROR!!! There is no information about number of " + key + "(s) in %s shape!!!" % name )
            return False
        elif str( expected_info[key] ).find( str( got_info_dict[key] )) == -1 or len( str( expected_info[key] )) != len( str( got_info_dict[key] )):
            print( "ERROR!!! The number of " + key + "(s) is incorrect in %s shape!!! ( " % name + str( got_info_dict[key] ) + " instead of " + str( expected_info[key] ) + " )" )
            return False
    return True


class GEOMTestBooleanFuzzy(unittest.TestCase):
    def testFuse(self):
        Vertex_1 = geompy.MakeVertex(10, 0, 0)
        Vertex_2 = geompy.MakeVertex(20, 10.0001, 10.0001)
        Box_1 = geompy.MakeBoxDXDYDZ(10, 10, 10)
        Box_2 = geompy.MakeBoxTwoPnt(Vertex_1, Vertex_2)
        # Fuse without fuzzy parameter
        Fuse_1 = geompy.MakeFuse(Box_1, Box_2, True, True)
        WHAT_IS_1={"VERTEX":14, "EDGE":21, "WIRE":9, "FACE":9, "SHELL":1, "SOLID":1, "COMPSOLID":0, "COMPOUND":0, "SHAPE":55}
        assert(HasSameSubShapes(Fuse_1, WHAT_IS_1))
        # Fuse with fuzzy parameter
        Fuse_2 = geompy.MakeFuse(Box_1, Box_2, True, True, None, 1.e-4)
        WHAT_IS_2={"VERTEX":11, "EDGE":17, "WIRE":8, "FACE":8, "SHELL":1, "SOLID":1, "COMPSOLID":0, "COMPOUND":0, "SHAPE":46}
        assert(HasSameSubShapes(Fuse_2, WHAT_IS_2))

    def testCommon(self):
        Vertex_1 = geompy.MakeVertex(0, 9.999995, 0)
        Sphere_1 = geompy.MakeSphereR(5)
        Sphere_2 = geompy.MakeSpherePntR(Vertex_1, 5)
        # Common without fuzzy parameter
        Common_1 = geompy.MakeCommon(Sphere_1, Sphere_2, True)
        WHAT_IS_1={"VERTEX":1, "EDGE":1, "WIRE":2, "FACE":2, "SHELL":1, "SOLID":1, "COMPSOLID":0, "COMPOUND":0, "SHAPE":8}
        assert(HasSameSubShapes(Common_1, WHAT_IS_1))
        # Common with fuzzy parameter
        Common_2 = geompy.MakeCommon(Sphere_1, Sphere_2, True, None, 1.e-5)
        WHAT_IS_2={"VERTEX":0, "EDGE":0, "WIRE":0, "FACE":0, "SHELL":0, "SOLID":0, "COMPSOLID":0, "COMPOUND":1, "SHAPE":1}
        assert(HasSameSubShapes(Common_2, WHAT_IS_2))

    def testCut(self):
        OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
        Vertex_1 = geompy.MakeVertex(5.e-5, 5, 5)
        Box_1 = geompy.MakeBoxDXDYDZ(10, 10, 10)
        Cylinder_1 = geompy.MakeCylinder(Vertex_1, OX, 6, 13)
        # Cut with low fuzzy parameter
        Cut_1 = geompy.MakeCut(Box_1, Cylinder_1, True, None, 1.e-5)
        WHAT_IS_1={"VERTEX":24, "EDGE":36, "WIRE":14, "FACE":14, "SHELL":1, "SOLID":1, "COMPSOLID":0, "COMPOUND":0, "SHAPE":90}
        assert(HasSameSubShapes(Cut_1, WHAT_IS_1))
        # Cut with high fuzzy parameter
        Cut_2 = geompy.MakeCut(Box_1, Cylinder_1, True, None, 5.e-5)
        WHAT_IS_2={"VERTEX":24, "EDGE":36, "WIRE":20, "FACE":20, "SHELL":4, "SOLID":4, "COMPSOLID":0, "COMPOUND":1, "SHAPE":109}
        assert(HasSameSubShapes(Cut_2, WHAT_IS_2))

    def testBoolean(self):
        Vertex_1 = geompy.MakeVertex(0, 9.999995, 0)
        Sphere_1 = geompy.MakeSphereR(5)
        Sphere_2 = geompy.MakeSpherePntR(Vertex_1, 5)
        # Common without fuzzy parameter
        Common_1 = geompy.MakeBoolean(Sphere_1, Sphere_2, 1, True)
        WHAT_IS_1={"VERTEX":1, "EDGE":1, "WIRE":2, "FACE":2, "SHELL":1, "SOLID":1, "COMPSOLID":0, "COMPOUND":0, "SHAPE":8}
        assert(HasSameSubShapes(Common_1, WHAT_IS_1))
        # Common with fuzzy parameter
        Common_2 = geompy.MakeBoolean(Sphere_1, Sphere_2, 1, True, None, 1.e-5)
        WHAT_IS_2={"VERTEX":0, "EDGE":0, "WIRE":0, "FACE":0, "SHELL":0, "SOLID":0, "COMPSOLID":0, "COMPOUND":1, "SHAPE":1}
        assert(HasSameSubShapes(Common_2, WHAT_IS_2))

    def testPartition(self):
        Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
        Cylinder_1 = geompy.MakeCylinderRH(100, 300)
        Box_2 = geompy.MakeBoxDXDYDZ(200, 200, 200)
        Box_translated_slightly_inside = geompy.MakeTranslation(Box_2, 0, -199.9999, 0)
        Box_translated_slightly_outside = geompy.MakeTranslation(Box_2, 0, -200.0001, 0)
        Partitition_small_intersection = geompy.MakePartition([Box_1, Cylinder_1, Box_translated_slightly_inside])
        WHAT_IS_1={"VERTEX":25, "EDGE":49, "WIRE":32, "FACE":32, "SHELL":7, "SOLID":7, "COMPSOLID":0, "COMPOUND":1, "SHAPE":153}
        assert(HasSameSubShapes(Partitition_small_intersection, WHAT_IS_1))
        Partitition_small_intersection_fixed_by_fuzzy = geompy.MakePartition([Box_1, Cylinder_1, Box_translated_slightly_inside], theFuzzyParam=0.0001)
        WHAT_IS_2={"VERTEX":19, "EDGE":36, "WIRE":23, "FACE":23, "SHELL":5, "SOLID":5, "COMPSOLID":0, "COMPOUND":1, "SHAPE":112}
        assert(HasSameSubShapes(Partitition_small_intersection_fixed_by_fuzzy, WHAT_IS_2))
        Partitition_small_gap = geompy.MakePartition([Box_1, Cylinder_1, Box_translated_slightly_outside])
        WHAT_IS_3={"VERTEX":25, "EDGE":44, "WIRE":25, "FACE":25, "SHELL":5, "SOLID":5, "COMPSOLID":0, "COMPOUND":1, "SHAPE":130}
        assert(HasSameSubShapes(Partitition_small_gap, WHAT_IS_3))
        Partitition_small_gap_fixed_by_fuzzy = geompy.MakePartition([Box_1, Cylinder_1, Box_translated_slightly_inside], theFuzzyParam=0.0001)
        WHAT_IS_4={"VERTEX":19, "EDGE":36, "WIRE":23, "FACE":23, "SHELL":5, "SOLID":5, "COMPSOLID":0, "COMPOUND":1, "SHAPE":112}
        assert(HasSameSubShapes(Partitition_small_gap_fixed_by_fuzzy, WHAT_IS_4))

if __name__ == '__main__':
    unittest.main()
