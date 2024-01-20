#  -*- coding: utf-8 -*-
# Copyright (C) 2022-2024  CEA, EDF
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

import unittest

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

class BasicGeomObjsEx11(unittest.TestCase):

    def testNoRaiseOnMakeFaceWires(self):
        """
        Work in pair with testRaiseOnMakeFaceWires
        """
        pts = [(0,0,0),(1,0,0),(1,1,1e-6),(0,1,0)] # diff with testRaiseOnMakeFaceWires is 1e-6 instead of 1e-5
        vertices = [ geompy.MakeVertex(*list(elt)) for elt in pts]
        polyline0 =  geompy.MakePolyline([ vertices[nodeidx] for nodeidx in range(len(pts)) ], True)
        wire_0 =  geompy.MakeFaceWires( [ polyline0 ] , isPlanarWanted = True, theName=None, raiseException=True) # isPlanarWanted and raiseException are expected to be True here !
        self.assertTrue(wire_0) # wire_0 is expected to be not None because wire has been created and detected to be planar
        wire_1 =  geompy.MakeFace( polyline0 , isPlanarWanted = True, theName=None, raiseException=True) # isPlanarWanted and raiseException are expected to be True here !
        self.assertTrue(wire_1)

    def testRaiseOnMakeFaceWires(self):
        """
        Work in pair with testNoRaiseOnMakeFaceWires
        """
        pts = [(0,0,0),(1,0,0),(1,1,1e-5),(0,1,0)] # diff with testRaiseOnMakeFaceWires is 1e-5 instead of 1e-6
        vertices = [ geompy.MakeVertex(*list(elt)) for elt in pts]
        polyline0 =  geompy.MakePolyline([ vertices[nodeidx] for nodeidx in range(len(pts)) ], True)
        # MakeFaceWires is expected to fail here because third point is too far from Oxy plane
        self.assertRaises( RuntimeError, geompy.MakeFaceWires, [ polyline0 ] , True, None, True )# isPlanarWanted and raiseException are expected to be True here !
        wire_0 =  geompy.MakeFaceWires( [ polyline0 ] , isPlanarWanted = True, theName=None, raiseException=False) # returns something bug wire_0 is incorrect
        self.assertRaises( RuntimeError, geompy.MakeFace, polyline0 , True, None, True )# isPlanarWanted and raiseException are expected to be True here !
        wire_1 =  geompy.MakeFace( polyline0 , isPlanarWanted = True, theName=None, raiseException=False) # returns something bug wire_1 is incorrect

if __name__ == '__main__':
    unittest.main()
