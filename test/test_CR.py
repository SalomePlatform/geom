#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

def GetShapeType(theShape):
    CR = geompy.CanonicalRecognition()
    if CR.isLine(theShape, 0.1)[0]:
        return "Line"
    if CR.isPlane(theShape, 0.1)[0]:
        if CR.isCircle(theShape, 0.1)[0]:
            return ("Plane","Circle")
        if CR.isEllipse(theShape, 0.1)[0]:
            return ("Plane","Ellipse")
        return "Plane"
    if CR.isSphere(theShape, 0.1)[0]:
        return "Sphere"
    if CR.isCone(theShape, 0.1)[0]:
        return "Cone"
    if CR.isCylinder(theShape, 0.1)[0]:
        return "Cylinder"
    return "Not defined"

class GEOMTestCR(unittest.TestCase):
    def testBasic(self):
        """
        Acceptance test for tuleap29469
        """
        O = geompy.MakeVertex(0, 0, 0)
        OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
        OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
        OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
        Cylinder_1 = geompy.MakeCylinderRH(100, 300)
        [Face_1,Face_2,Face_3] = geompy.ExtractShapes(Cylinder_1, geompy.ShapeType["FACE"], True)
        [Edge_1,Edge_2,Edge_3] = geompy.ExtractShapes(Cylinder_1, geompy.ShapeType["EDGE"], True)
        Sphere_1 = geompy.MakeSphereR(100)
        [Shell_1] = geompy.ExtractShapes(Sphere_1, geompy.ShapeType["SHELL"], True)
        Cone_1 = geompy.MakeConeR1R2H(100, 0, 300)
        [Shell_2] = geompy.ExtractShapes(Cone_1, geompy.ShapeType["SHELL"], True)
        [Face_4,Face_5] = geompy.ExtractShapes(Shell_2, geompy.ShapeType["FACE"], True)

        ## Create ellips
        Cylinder_2 = geompy.MakeCylinderRH(100, 300)
        Plane_1 = geompy.MakePlaneLCS(None, 350, 1)
        Translation_1 = geompy.MakeTranslation(Plane_1, 0, 0, 150)
        Rotation_1 = geompy.MakeRotation(Translation_1, OX, 20*math.pi/180.0)
        Partition_1 = geompy.MakePartition([Cylinder_2], [Rotation_1], [], [], geompy.ShapeType["SOLID"], 0, [], 0)
        [Edge_4,Edge_5,Edge_6,Edge_7,Edge_8] = geompy.ExtractShapes(Partition_1, geompy.ShapeType["EDGE"], True)

        assert GetShapeType(Face_1) == "Plane"
        assert GetShapeType(Face_2) == "Cylinder"
        assert GetShapeType(Edge_3) == "Line"
        assert GetShapeType(Edge_2)[1] == "Circle"
        assert GetShapeType(Shell_1) == "Sphere"
        assert GetShapeType(Shell_2) == "Cone"
        assert GetShapeType(Edge_5)[1] == "Ellipse"
        
    def testOnSetOfPtsOnCyl(self):
        import numpy as np
        tab = [[49.9999999999997, 86.6025403784442, 0.0], [1.43524931876432e-12, 100.0, 0.0], [6.12323399573677e-15, 100.0, 300.0], [50.0, 86.6025403784439, 300.0], [34.20201433256674, 93.96926207859089, 300.0], [17.364817766692976, 98.48077530122082, 300.0], [0.0, 100.0, 19.999999999999854], [0.0, 100.0, 39.99999999999963], [0.0, 100.0, 59.99999999999965], [0.0, 100.0, 79.99999999999966], [0.0, 100.0, 99.9999999999997], [0.0, 100.0, 119.99999999999973], [0.0, 100.0, 139.99999999999974], [0.0, 100.0, 159.99999999999977], [0.0, 100.0, 179.9999999999998], [0.0, 100.0, 199.99999999999983], [0.0, 100.0, 219.9999999999999], [0.0, 100.0, 239.99999999999994], [0.0, 100.0, 259.9999999999999], [0.0, 100.0, 279.99999999999994], [34.20201433256736, 93.96926207859066, 0.0], [17.364817766694355, 98.48077530122058, 0.0], [50.0, 86.60254037844386, 19.999999999999854], [50.0, 86.60254037844386, 39.99999999999963], [50.0, 86.60254037844386, 59.99999999999965], [50.0, 86.60254037844386, 79.99999999999966], [50.0, 86.60254037844386, 99.9999999999997], [50.0, 86.60254037844386, 119.99999999999973], [50.0, 86.60254037844386, 139.99999999999974], [50.0, 86.60254037844386, 159.99999999999977], [50.0, 86.60254037844386, 179.9999999999998], [50.0, 86.60254037844386, 199.99999999999983], [50.0, 86.60254037844386, 219.9999999999999], [50.0, 86.60254037844386, 239.99999999999994], [50.0, 86.60254037844386, 259.9999999999999], [50.0, 86.60254037844386, 279.99999999999994], [29.61877316560949, 95.5129743865417, 277.8870679979398], [17.205660214608802, 98.50870650140234, 30.630206624576903], [16.710324551353793, 98.59394024679419, 72.64745394595525], [17.100883188710984, 98.52694958317781, 107.80372339391727], [17.635764230289126, 98.4326156312716, 152.57612984579256], [13.644784679867689, 99.06472556384563, 189.91477232835643], [17.211909165765633, 98.50761484712454, 226.50789445170153], [16.947190833941324, 98.55350182940215, 264.31714685564623], [33.70205726794775, 94.14972828377132, 23.514274138894333], [33.9866992950552, 94.04735121749837, 45.6467424308921], [33.80199583830124, 94.11389417799832, 66.95510896864648], [31.934324162661458, 94.76391159230425, 89.88408369489096], [34.465261348238855, 93.8730299936973, 112.129816095113], [37.584878577685345, 92.66810078069287, 129.81302524607864], [34.76442821196357, 93.76264997905739, 147.223574108196], [33.921830158958535, 94.07076824745704, 167.36545102566686], [31.81206857237576, 94.80502251013108, 189.95083125203968], [33.807919537982364, 94.11176640842159, 212.57124554127284], [34.08001408422917, 94.01357689195078, 233.1435048617797], [33.7912654017101, 94.11774743666143, 253.72564508639465], [17.227853600937443, 98.50482759897946, 52.4555873421884], [13.783376774516714, 99.04553763139313, 90.1191962222661], [19.72881682971234, 98.03455404345786, 129.98468730013374], [16.735452147700688, 98.58967816872119, 172.6689841537882], [16.664342186017773, 98.60172259908703, 207.05379129229303], [17.201463406704146, 98.50943942926392, 246.03943188659505]]

        vtx = [ geompy.MakeVertex(x,y,z) for x,y,z in tab]

        theShape = geompy.MakeSmoothingSurface( vtx, 2, 8, 0)
        CR = geompy.CanonicalRecognition()
        recog,axis,pt,radius = CR.isCylinder(theShape, 0.1)
        self.assertTrue( recog )
        self.assertTrue( np.isclose(100,radius,rtol=1e-2, atol = 0) )
        axisA = np.array(axis).reshape(1,3)
        self.assertTrue( np.isclose( np.linalg.norm( axisA ) , 1.0 , rtol=1e-10, atol = 0 ) )
        self.assertTrue( np.isclose( np.abs( np.dot( axisA.reshape(3), np.array([0,0,1]) ) ), 1.0 , rtol=1e-7, atol = 0 ) )
        relDistOfPtToIdealAxis = np.linalg.norm( np.cross( axisA.reshape(3), np.array(pt) )/np.linalg.norm( np.array(pt) ) )
        self.assertTrue( np.isclose( relDistOfPtToIdealAxis, 0.0, rtol = 0, atol = 1e-3 ) )

if __name__ == '__main__':
    unittest.main()
