# Curvature of a Face along given direction

import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()
import math
import numpy as np

def test_acceptance():
  """
  Acceptance test [tuleap29472]
  """
  Vector = [0,100,100]
  O = geompy.MakeVertex(0, 0, 0)
  OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
  OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
  OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
  Cylinder_1 = geompy.MakeCylinderRH(100, 300)
  Translation_1 = geompy.MakeTranslation(Cylinder_1, 0, 0, -150)
  Vertex_1 = geompy.MakeVertex(100, 0, 0)
  Vertex_2 = geompy.MakeVertex(100, -Vector[2], Vector[1])
  Line_1 = geompy.MakeLineTwoPnt(Vertex_1, Vertex_2)
  Plane_1 = geompy.MakePlane(Vertex_1, Line_1, 2000)
  Rotation_1 = geompy.MakeRotation(Translation_1, OZ, 90*math.pi/180.0)# avoid to have degenerated edge across Vertex_1

  [Face_1,Face_2,Face_3] = geompy.ExtractShapes(Rotation_1, geompy.ShapeType["FACE"], True)

  curvature_29472 = np.array( geompy.VectorCoordinates( geompy.CurvatureOnFace(Face_2, Vertex_1, geompy.MakeVectorDXDYDZ(*Vector))) ).reshape(1,3)
  expected_curvature = np.array( [-200.0,0.0,0.0] ).reshape(1,3)
  assert( np.isclose( 0.0, np.linalg.norm( curvature_29472 - expected_curvature ) ,rtol=0,atol=1e-5 ) )

  Intersection_1 = geompy.MakeSection(Face_2, Plane_1, True)
  geompy.addToStudy( O, 'O' )
  geompy.addToStudy( OX, 'OX' )
  geompy.addToStudy( OY, 'OY' )
  geompy.addToStudy( OZ, 'OZ' )
  geompy.addToStudy( Vertex_1, 'Vertex_1' )
  geompy.addToStudy( Cylinder_1, 'Cylinder_1' )
  geompy.addToStudy( Translation_1, 'Translation_1' )
  geompy.addToStudy( Vertex_2, 'Vertex_2' )
  geompy.addToStudy( Line_1, 'Line_1' )
  geompy.addToStudy( Plane_1, 'Plane_1' )
  geompy.addToStudy( Rotation_1, 'Rotation_1' )
  geompy.addToStudyInFather( Rotation_1, Face_1, 'Face_1' )
  geompy.addToStudyInFather( Rotation_1, Face_2, 'Face_2' )
  geompy.addToStudyInFather( Rotation_1, Face_3, 'Face_3' )
  geompy.addToStudy( Intersection_1, 'Intersection_1' )
  angle = math.asin(Vector[2]/math.sqrt(Vector[1]*Vector[1]+Vector[2]*Vector[2]))
  tmp = geompy.MakeTranslation(Intersection_1,*[-elt for elt in geompy.PointCoordinates(Vertex_1)])
  tmp = geompy.MakeRotation(tmp,OX,-angle)
  Intersection_1_OXY = geompy.MakeTranslation(tmp,*geompy.PointCoordinates(Vertex_1))
  geompy.addToStudy( Intersection_1_OXY, 'Intersection_1_OXY' )

  eps = 0.01
  offset = 0.75
  p0 = np.array( geompy.PointCoordinates( geompy.MakeVertexOnCurve(Intersection_1_OXY,offset-eps) ) ).reshape(1,3)
  p1 = np.array( geompy.PointCoordinates( geompy.MakeVertexOnCurve(Intersection_1_OXY,offset) ) ).reshape(1,3)
  p2 = np.array( geompy.PointCoordinates( geompy.MakeVertexOnCurve(Intersection_1_OXY,offset+eps) ) ).reshape(1,3)
  assert( np.isclose(0.0,np.linalg.norm(p1- np.array(geompy.PointCoordinates(Vertex_1)).reshape(1,3)  ),rtol=0,atol=1e-8) )
  p01=(p0+p1)/2
  p12=(p1+p2)/2
  v0 = (p1-p0)/np.linalg.norm(p1-p0)
  v1 = (p2-p1)/np.linalg.norm(p2-p1)
  computedRadius =  1/np.linalg.norm((v1-v0)/np.linalg.norm(p12-p01))
  # manual detection of radius : https://fr.wikipedia.org/wiki/Courbure_d%27un_arc
  circle = geompy.MakeCircle(O,OZ,computedRadius)
  circle = geompy.MakeTranslation(circle,100-computedRadius,0,0)
  geompy.addToStudy(circle, "expectedCircle")
  print("Radius expected is {}".format(computedRadius))
  print("Radius obtain by CurvatureOnFace is {}".format(np.linalg.norm(curvature_29472)))

O = geompy.MakeVertex(0, 0, 0, 'O')
OX = geompy.MakeVectorDXDYDZ(1, 0, 0, 'OX')
OY = geompy.MakeVectorDXDYDZ(0, 1, 0, 'OY')
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1, 'OZ')

pXYZ = geompy.MakeVertex(105, 105, 105, 'pXYZ')
pY = geompy.MakeVertex(0, 105, 0, 'pY')
pZ = geompy.MakeVertex(0, 0, 105, 'pZ')

vZ_XY = geompy.MakeVectorDXDYDZ(-1, -1, 1, 'vZ-XY')
vZ_XY2 = geompy.MakeVectorDXDYDZ(-1, -1, 10, 'vZ-XY')
vZ_XY3 = geompy.MakeVectorDXDYDZ(-1, -1, 100, 'vZ-XY')

R = 100.0

# I. Curvature of a Sphere
Sphere_1 = geompy.MakeSphereR(R, 'Sphere_1')
[Sph] = geompy.ExtractShapes(Sphere_1, geompy.ShapeType["FACE"], True, "Sph")

curvature_1 = geompy.CurvatureOnFace(Sph, pXYZ, OX,    'curvature_sph_pXYZ_OX')
curvature_2 = geompy.CurvatureOnFace(Sph, pXYZ, vZ_XY, 'curvature_sph_pXYZ_vt')
curvature_3 = geompy.CurvatureOnFace(Sph, pY,   OX,    'curvature_sph_pY_OX')

# All sphere curvature radiuces = R
assert(abs(geompy.BasicProperties(curvature_1)[0] - R) < 1e-07)
assert(abs(geompy.BasicProperties(curvature_2)[0] - R) < 1e-07)
assert(abs(geompy.BasicProperties(curvature_3)[0] - R) < 1e-07)

# Pole
isExcept = False
try:
  geompy.CurvatureOnFace(Sph, pZ, OX)
except:
  isExcept = True
assert(isExcept)

# Normal direction
isExcept = False
try:
  geompy.CurvatureOnFace(Sph, pY,   OY)
except:
  isExcept = True
assert(isExcept)

# II. Curvature of a Cylinder
Cylinder_1 = geompy.MakeCylinderRH(R, 300, 'Cylinder_1')
[Face_1,Face_2,Face_3] = geompy.ExtractShapes(Cylinder_1, geompy.ShapeType["FACE"], True, "Face")

# Curvature radius of a cylinder along any direction, orthogonal to its Z axis, equal to R
curvature_4 = geompy.CurvatureOnFace(Face_2, pY, OX, 'curvature_cyl_pY_OX')
assert(abs(geompy.BasicProperties(curvature_4)[0] - R) < 1e-07)

# Curvature radius of a cylinder along its Z direction is infinite
curvature_zero = geompy.CurvatureOnFace(Face_2, pY, OZ)
assert(geompy.MeasuOp.GetErrorCode() == "ZERO_CURVATURE")
assert(not curvature_zero)

# Curvature radius of a cylinder along some direction, different from two above
curvature_5 = geompy.CurvatureOnFace(Face_2, pY, vZ_XY,  'curvature_cyl_pY_vZ_XY')
curvature_6 = geompy.CurvatureOnFace(Face_2, pY, vZ_XY2, 'curvature_cyl_pY_vZ_XY2')
curvature_7 = geompy.CurvatureOnFace(Face_2, pY, vZ_XY3, 'curvature_cyl_pY_vZ_XY3')

# R < r5 < r6 < r7
# r5 = 100.01, r6 = 101.0, r7 = 200
r5 = geompy.BasicProperties(curvature_5)[0]
r6 = geompy.BasicProperties(curvature_6)[0]
r7 = geompy.BasicProperties(curvature_7)[0]

assert(R  + 1e-07 < r5)
assert(r5 + 1e-07 < r6)
assert(r6 + 1e-07 < r7)

# Projection aborted. Point is out of the face boundaries.
isExcept = False
try:
  pXY_Z = geompy.MakeVertex(105, 105, -105, 'pXY_Z')
  geompy.CurvatureOnFace(Face_2, pXY_Z, OX, 'curvature_cyl_pXY_Z')
except:
  isExcept = True
assert(isExcept)

# Projection aborted (point on axis). Equal distances to many points.
isExcept = False
try:
  geompy.CurvatureOnFace(Face_2, O, vZ_XY, 'curvature_cyl_O')
except:
  isExcept = True
assert(isExcept)

# Curvature radius of a planar face is infinite
curvature_zero_2 = geompy.CurvatureOnFace(Face_1, pZ, OX)
assert(geompy.MeasuOp.GetErrorCode() == "ZERO_CURVATURE")
assert(not curvature_zero_2)

# III. Curvature of a "Horse saddle"
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
Vertex_2 = geompy.MakeVertex(100, 0, 0, 'Vertex_2')

curvature_Y = geompy.CurvatureOnFace(Filling_1, Vertex_2, OY, 'curvature_Y')
curvature_Z = geompy.CurvatureOnFace(Filling_1, Vertex_2, OZ, 'curvature_Z')

cury = np.array( geompy.VectorCoordinates(curvature_Y) ).reshape(1,3)
curz = np.array( geompy.VectorCoordinates(curvature_Z) ).reshape(1,3)
cury_expected = np.array( [50,0,0] ).reshape(1,3)
curz_expected = np.array( [-100,0,0] ).reshape(1,3)
assert( np.isclose( 0.0, np.linalg.norm( cury - cury_expected ) ,rtol=0,atol=1e-5 ) )
assert( np.isclose( 0.0, np.linalg.norm( curz - curz_expected ) ,rtol=0,atol=1e-5 ) )

# Normal direction
norm_1 = geompy.GetNormal(Filling_1, Vertex_2, "Normal_1")
isExcept = False
try:
  geompy.CurvatureOnFace(Filling_1, Vertex_2, norm_1)
except:
  isExcept = True
assert(isExcept)

# acceptance case
test_acceptance()