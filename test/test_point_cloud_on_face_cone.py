# Point Cloud on Face Cone

from salome.geom import geomBuilder
geompy = geomBuilder.New()

# Create one closed cone face
O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
Cone_1 = geompy.MakeConeR1R2H(50, 100, 50)
[Face_1] = geompy.SubShapes(Cone_1, [3])
geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( Cone_1, 'Cone_1' )
geompy.addToStudyInFather( Cone_1, Face_1, 'Face_1' )

# Make a cloud of given number of points in range 1-30 on the cone face
try:
    for i in range(1, 31):
        PointCloudOnFilling = geompy.MakeVertexInsideFace(Face_1, i, "PointCloudOnFilling")
except Exception as err:
    print(err)
    # this test should not fail in case of "Improper OCCT version"
    assert("Improper OCCT version" in str(err))
