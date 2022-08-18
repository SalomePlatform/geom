# Point Cloud on Face

import math
import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

# create spherical face
Sphere = geompy.MakeSphereR(10, "Sphere")
[Face] = geompy.ExtractShapes(Sphere, geompy.ShapeType["FACE"], True, "Sphere_face")

# make a cloud of 100 points on the spherical face
try:
    CompoundOfVertices = geompy.MakeVertexInsideFace(Face, 100, "CompoundOfVertices")
except Exception as err:
    print(err)
    # this test should not fail in case of "Improper OCCT version"
    assert("Improper OCCT version" in str(err))
else:
    # check result
    assert(geompy.NumberOfSubShapes(CompoundOfVertices, geompy.ShapeType["VERTEX"]) == 100)

# test point cloud on a "Horse saddle"
OX = geompy.MakeVectorDXDYDZ(1, 0, 0, 'OX')
OY = geompy.MakeVectorDXDYDZ(0, 1, 0, 'OY')
[Edge_1,Edge_2,Edge_3] = geompy.ExtractShapes(Sphere, geompy.ShapeType["EDGE"], True, "Edge")
Rotation_1 = geompy.MakeRotation(Edge_3, OX, 90*math.pi/180.0, 'Rotation_1')
Rotation_2 = geompy.MakeRotation(Rotation_1, OY, 180*math.pi/180.0, 'Rotation_2')
Translation_1 = geompy.MakeTranslation(Rotation_2, 200, 0, 0, 'Translation_1')
Translation_2 = geompy.MakeTranslation(Edge_3, 100, 100, 0, 'Translation_2')
Translation_3 = geompy.MakeTranslation(Translation_2, 0, -200, 0, 'Translation_3')
Filling_1 = geompy.MakeFilling([Translation_2, Edge_3, Translation_3])
geompy.addToStudy(Filling_1, 'Filling_1')

try:
    PointCloudOnFilling = geompy.MakeVertexInsideFace(Filling_1, 30, "PointCloudOnFilling")
except Exception as err:
    print(err)
    # this test should not fail in case of "Improper OCCT version"
    assert("Improper OCCT version" in str(err))
else:
    assert(geompy.NumberOfSubShapes(PointCloudOnFilling, geompy.ShapeType["VERTEX"]) == 30)
