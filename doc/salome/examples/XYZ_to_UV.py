# Test XYZtoUV and UVtoXYZ methods

import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()
import math
import numpy as np

toler = 1e-04

# Create "Horse saddle"
OX = geompy.MakeVectorDXDYDZ(1, 0, 0, 'OX')
OY = geompy.MakeVectorDXDYDZ(0, 1, 0, 'OY')
Sphere_1 = geompy.MakeSphereR(100, 'Sphere_1')
[Edge_1,Edge_2,Edge_3] = geompy.ExtractShapes(Sphere_1, geompy.ShapeType["EDGE"], True)
geompy.addToStudyInFather( Sphere_1, Edge_1, 'Edge_1' )
geompy.addToStudyInFather( Sphere_1, Edge_2, 'Edge_2' )
geompy.addToStudyInFather( Sphere_1, Edge_3, 'Edge_3' )

Rotation_1 = geompy.MakeRotation(Edge_3, OX, 90*math.pi/180.0, 'Rotation_1')
Rotation_2 = geompy.MakeRotation(Rotation_1, OY, 180*math.pi/180.0, 'Rotation_2')
Translation_1 = geompy.MakeTranslation(Rotation_2, 200, 0, 0, 'Translation_1')
Translation_2 = geompy.MakeTranslation(Edge_3, 100, 100, 0, 'Translation_2')
Translation_3 = geompy.MakeTranslation(Translation_2, 0, -200, 0, 'Translation_3')

Filling_1 = geompy.MakeFilling([Translation_2, Edge_3, Translation_3])
geompy.addToStudy(Filling_1, 'Filling_1')

HorseSaddle = geompy.LimitTolerance(Filling_1, toler, 'HorseSaddle')

# Get 100 equidistant points on the "Horse saddle"
CompoundOfVertices = geompy.MakeVertexInsideFace(HorseSaddle, 100, "CompoundOfVertices")
assert(geompy.NumberOfSubShapes(CompoundOfVertices, geompy.ShapeType["VERTEX"]) == 100)

# Extract the vertices
listOfVertices = geompy.ExtractShapes(CompoundOfVertices, geompy.ShapeType["VERTEX"], True)

# Get list of coordinates of all 100 vertices
listOfCoords = []
for aV in listOfVertices:
    listOfCoords += geompy.PointCoordinates(aV)
    pass

# Test 1: with normalization of parameters

# Convert XYZ to UV
listOfParams_norm = geompy.XYZtoUV(HorseSaddle, listOfCoords, True)
assert(len(listOfParams_norm) == 200)

# Convert UV to XYZ
listOfCoords_new1 = geompy.UVtoXYZ(HorseSaddle, listOfParams_norm, True)
assert(len(listOfCoords_new1) == 300)

# Compare listOfCoords_new with listOfCoords
for (c1, c2) in zip(listOfCoords, listOfCoords_new1):
    assert(abs(c1 - c2) < toler)
    pass

# Test 2: without normalization of parameters

# Convert XYZ to UV
listOfParams = geompy.XYZtoUV(HorseSaddle, listOfCoords, False)
assert(len(listOfParams) == 200)

# Convert UV to XYZ
listOfCoords_new2 = geompy.UVtoXYZ(HorseSaddle, listOfParams, False)
assert(len(listOfCoords_new2) == 300)

# Compare listOfCoords_new with listOfCoords
for (c1, c2) in zip(listOfCoords, listOfCoords_new2):
    assert(abs(c1 - c2) < toler)
    pass

# Test 3: Check exceptions throwing if point (XYZ or UV) is out of face
listXYZ_3 = listOfCoords[:3]
listXYZ_3[2] = listXYZ_3[2] + 1.0 # move along OZ
try:
    geompy.XYZtoUV(HorseSaddle, listXYZ_3, True)
except RuntimeError:
    print(geompy.MeasuOp.GetErrorCode(), ", it's OK")
    pass
except Exception:
    assert False, 'Unexpected exception raised'
else:
    assert False, 'XYZtoUV should raise an exception if input point is out of face'

listUV_2 = [2, 2] # each parameter value should be in [0,1] range (for normalized case)
try:
    geompy.UVtoXYZ(HorseSaddle, listUV_2, True)
except RuntimeError:
    print(geompy.MeasuOp.GetErrorCode(), ", it's OK")
    pass
except Exception:
    assert False, 'Unexpected exception raised'
else:
    assert False, 'UVtoXYZ should raise an exception if input parameters are out of face'

# parameter U should be in [4.71239, 7.85398] range (on this face, for not normalized case)
# parameter V should be in [0, 1] range (on this face, for not normalized case)
listUV_2 = [10, 10]
try:
    geompy.UVtoXYZ(HorseSaddle, listUV_2, True)
except RuntimeError:
    print(geompy.MeasuOp.GetErrorCode(), ", it's OK")
    pass
except Exception:
    assert False, 'Unexpected exception raised'
else:
    assert False, 'UVtoXYZ should raise an exception if input parameters are out of face'

# Test 4: Check exceptions in case of invalid data type (wrong length of array or type of elements)

#   1. Length of input array is not divisible by 3 (for XYZtoUV) or by 2 (for UVtoXYZ)
listXYZ_4 = listOfCoords[:4]
assert(len(listXYZ_4) == 4)
try:
    geompy.XYZtoUV(HorseSaddle, listXYZ_4, True)
except RuntimeError:
    print(geompy.MeasuOp.GetErrorCode(), ", it's OK")
    pass
except Exception:
    assert False, 'Unexpected exception raised'
else:
    assert False, 'XYZtoUV should raise an exception if input list length is not divisible by 3'

listUV_3 = listOfParams[:3]
assert(len(listUV_3) == 3)
try:
    geompy.UVtoXYZ(HorseSaddle, listUV_3, True)
except RuntimeError:
    print(geompy.MeasuOp.GetErrorCode(), ", it's OK")
    pass
except Exception:
    assert False, 'Unexpected exception raised'
else:
    assert False, 'UVtoXYZ should raise an exception if input list length is not divisible by 2'

#   2. Input array contains data of wrong type
listXYZ_w = ['a', 'b', 'c']
try:
    geompy.XYZtoUV(HorseSaddle, listXYZ_w, True)
except Exception:
    pass
else:
    assert False, 'XYZtoUV should raise TypeError if input list contains not numerical data'

listXYZ_w = [10.0, '10.0', 10.0]
try:
    geompy.XYZtoUV(HorseSaddle, listXYZ_w, True)
except Exception:
    pass
else:
    assert False, 'XYZtoUV should raise TypeError if input list contains not numerical data'

listUV_w = ['a', 'b']
try:
    geompy.UVtoXYZ(HorseSaddle, listUV_w, True)
except Exception:
    pass
else:
    assert False, 'UVtoXYZ should raise TypeError if input list contains not numerical data'

listUV_w = [10.0, '10.0']
try:
    geompy.UVtoXYZ(HorseSaddle, listUV_w, True)
except Exception:
    pass
else:
    assert False, 'UVtoXYZ should raise TypeError if input list contains not numerical data'

# Test 5: a. Translate each of the 100 points by toler*2.0 along the face normal
#            and check that the XYZtoUV method fails by raising an exception.
#         b. Translate each of the 100 points by toler*0.7 along the face normal
#            and check that we obtain a result.
for ii in range(100):
    # cc - coordinates of point #ii
    cc = listOfCoords[ii*3 : ii*3 + 3]

    pnt = geompy.MakeVertex(cc[0], cc[1], cc[2])
    normal = geompy.GetNormal(HorseSaddle, pnt)
    vv = geompy.VectorCoordinates(normal)
    norm = np.linalg.norm(vv)
    if norm > toler: 
        vec = vv / norm
        # a. Move cc by toler*2.0 (XYZtoUV should fail)
        pp_2tol = [cc[0] + vec[0]*toler*2.0,
                   cc[1] + vec[1]*toler*2.0,
                   cc[2] + vec[2]*toler*2.0]
        try:
            geompy.XYZtoUV(HorseSaddle, pp_2tol)
        except RuntimeError:
            pass
        except Exception:
            assert False, 'Unexpected exception raised'
        else:
            assert False, 'XYZtoUV should raise an exception if input point is out of face'
            pass

        # b. Move cc by toler*0.7 (XYZtoUV should not fail)
        pp_07tol = [cc[0] + vec[0]*toler*0.7,
                    cc[1] + vec[1]*toler*0.7,
                    cc[2] + vec[2]*toler*0.7]
        UV_pp = geompy.XYZtoUV(HorseSaddle, pp_07tol, False)
        # compare with value from listOfParams (computed above)
        UV_cc = listOfParams[ii*2 : ii*2 + 2]
        for (c1, c2) in zip(UV_pp, UV_cc):
            assert(abs(c1 - c2) < toler)
            pass
        pass
    pass
