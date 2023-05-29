# Test KindOfShape method for Edges

import math
import salome
from inspect import getfile
from os.path import abspath, dirname, join
salome.salome_init_without_session()
import GEOM

from salome.geom import geomBuilder
geompy = geomBuilder.New()


def isEqual(v1, v2, tol=1.e-5):
  return abs(v1 - v2) < tol

def isEqualPoint(p1, p2, tol=1.e-5):
  return isEqual(p1[0], p2[0], tol) and isEqual(p1[1], p2[1], tol) and isEqual(p1[2], p2[2], tol)

def assertEqualType(props, t):
  assert (props[0]==t), f"Expected type {t}, but was {props[0]}"

def checkSegment(props, p1, p2):
  assertEqualType(props, geompy.kind.SEGMENT)
  p = props[1:4]
  assert isEqualPoint(p, p1), f"Expected SEGMENT start point ({p1[0]:.4f}, {p1[1]:.4f}, {p1[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[4:7]
  assert isEqualPoint(p, p2), f"Expected SEGMENT end point ({p2[0]:.4f}, {p2[1]:.4f}, {p2[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"

def checkCircle(props, c, d, r):
  assertEqualType(props, geompy.kind.CIRCLE)
  p = props[1:4]
  assert isEqualPoint(p, c), f"Expected CIRCLE center point ({c[0]:.4f}, {c[1]:.4f}, {c[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[4:7]
  assert isEqualPoint(p, d), f"Expected CIRCLE direction ({d[0]:.4f}, {d[1]:.4f}, {d[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  assert isEqual(props[7], r), f"Expected CIRCLE radius {r:.4f}, but was {props[7]:.4f}"

def checkArcCircle(props, c, d, r, p1, p2):
  assertEqualType(props, geompy.kind.ARC_CIRCLE)
  p = props[1:4]
  assert isEqualPoint(p, c), f"Expected ARC_CIRCLE center point ({c[0]:.4f}, {c[1]:.4f}, {c[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[4:7]
  assert isEqualPoint(p, d), f"Expected ARC_CIRCLE direction ({d[0]:.4f}, {d[1]:.4f}, {d[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  assert isEqual(props[7], r), f"Expected ARC_CIRCLE radius {r:.4f}, but was {props[7]:.4f}"
  p = props[8:11]
  assert isEqualPoint(p, p1), f"Expected ARC_CIRCLE start point ({p1[0]:.4f}, {p1[1]:.4f}, {p1[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[11:14]
  assert isEqualPoint(p, p2), f"Expected ARC_CIRCLE end point ({p2[0]:.4f}, {p2[1]:.4f}, {p2[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"

def checkEllipse(props, c, d, r1, r2, vx, vy):
  assertEqualType(props, geompy.kind.ELLIPSE)
  p = props[1:4]
  assert isEqualPoint(p, c), f"Expected ELLIPSE center point ({c[0]:.4f}, {c[1]:.4f}, {c[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[4:7]
  assert isEqualPoint(p, d), f"Expected ELLIPSE direction ({d[0]:.4f}, {d[1]:.4f}, {d[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  assert isEqual(props[7], r1), f"Expected ELLIPSE major radius {r1:.4f}, but was {props[7]:.4f}"
  assert isEqual(props[8], r2), f"Expected ELLIPSE minor radius {r2:.4f}, but was {props[8]:.4f}"
  p = props[9:12]
  assert isEqualPoint(p, vx), f"Expected ELLIPSE x-direction ({vx[0]:.4f}, {vx[1]:.4f}, {vx[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[12:15]
  assert isEqualPoint(p, vy), f"Expected ELLIPSE y-direction ({vy[0]:.4f}, {vy[1]:.4f}, {vy[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"

def checkArcEllipse(props, c, d, r1, r2, p1, p2, vx, vy):
  assertEqualType(props, geompy.kind.ARC_ELLIPSE)
  p = props[1:4]
  assert isEqualPoint(p, c), f"Expected ARC_ELLIPSE center point ({c[0]:.4f}, {c[1]:.4f}, {c[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[4:7]
  assert isEqualPoint(props[4:7], d), f"Expected ARC_ELLIPSE direction ({d[0]:.4f}, {d[1]:.4f}, {d[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  assert isEqual(props[7], r1), f"Expected ARC_ELLIPSE major radius {r1:.4f}, but was {props[7]:.4f}"
  assert isEqual(props[8], r2), f"Expected ARC_ELLIPSE minor radius {r2:.4f}, but was {props[8]:.4f}"
  p = props[9:12]
  assert isEqualPoint(props[9:12], p1), f"Expected ARC_ELLIPSE start point ({p1[0]:.4f}, {p1[1]:.4f}, {p1[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[12:15]
  assert isEqualPoint(props[12:15], p2), f"Expected ARC_ELLIPSE end point ({p2[0]:.4f}, {p2[1]:.4f}, {p2[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[15:18]
  assert isEqualPoint(props[15:18], vx), f"Expected ARC_ELLIPSE x-direction ({vx[0]:.4f}, {vx[1]:.4f}, {vx[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[18:21]
  assert isEqualPoint(props[18:21], vy), f"Expected ARC_ELLIPSE y-direction ({vy[0]:.4f}, {vy[1]:.4f}, {vy[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"

def checkHyperbola(props, c, d, r1, r2, vx, vy):
  assertEqualType(props, geompy.kind.HYPERBOLA)
  p = props[1:4]
  assert isEqualPoint(p, c), f"Expected HYPERBOLA center point ({c[0]:.4f}, {c[1]:.4f}, {c[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[4:7]
  assert isEqualPoint(p, d), f"Expected HYPERBOLA direction ({d[0]:.4f}, {d[1]:.4f}, {d[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  assert isEqual(props[7], r1), f"Expected HYPERBOLA major radius {r1:.4f}, but was {props[7]:.4f}"
  assert isEqual(props[8], r2), f"Expected HYPERBOLA minor radius {r2:.4f}, but was {props[8]:.4f}"
  p = props[9:12]
  assert isEqualPoint(p, vx), f"Expected HYPERBOLA x-direction ({vx[0]:.4f}, {vx[1]:.4f}, {vx[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[12:15]
  assert isEqualPoint(p, vy), f"Expected HYPERBOLA y-direction ({vy[0]:.4f}, {vy[1]:.4f}, {vy[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"

def checkParabola(props, c, d, f, vx, vy):
  assertEqualType(props, geompy.kind.PARABOLA)
  p = props[1:4]
  assert isEqualPoint(p, c), f"Expected PARABOLA center point ({c[0]:.4f}, {c[1]:.4f}, {c[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[4:7]
  assert isEqualPoint(p, d), f"Expected PARABOLA direction ({d[0]:.4f}, {d[1]:.4f}, {d[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  assert isEqual(props[7], f), f"Expected PARABOLA focal length {f:.4f}, but was {props[7]:.4f}"
  p = props[8:11]
  assert isEqualPoint(p, vx), f"Expected PARABOLA x-direction ({vx[0]:.4f}, {vx[1]:.4f}, {vx[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"
  p = props[11:14]
  assert isEqualPoint(p, vy), f"Expected PARABOLA y-direction ({vy[0]:.4f}, {vy[1]:.4f}, {vy[2]:.4f}), but was ({p[0]:.4f}, {p[1]:.4f}, {p[2]:.4f})"

def assertPoles(props, np, pfirst, poles):
  n = len(poles)
  assert (n<=np), f"Too much poles given ({n}), but should not be more than {np}"
  for i in range(0,n-1):
    p1 = poles[i]
    p2 = props[pfirst+i*3:pfirst+(i+1)*3]
    assert isEqualPoint(p1, p2), f"Expected pole #{i+1} ({p1[0]:.4f}, {p1[1]:.4f}, {p1[2]:.4f}), but was ({p2[0]:.4f}, {p2[1]:.4f}, {p2[2]:.4f})"

def assertKnots(props, nk, kfirst, knots):
  n = len(knots)
  assert (n<=nk), f"Too much knots given ({n}), but should not be more than {nk}"
  try:
    for i in range(0,n-1):
      assert isEqual(knots[i], props[kfirst+i]), f"Expected knot #{i+1} {knots[i]:.4f}, but was {props[kfirst+i]:.4f}"
  except:
    assert("Number of knots does not match expected number")

def assertWeights(props, nw, wfirst, weights):
  n = len(weights)
  assert (n<=nw), f"Too much weights given ({n}), but should not be more than {nw}"
  try:
    for i in range(0,n-1):
      assert isEqual(weights[i], props[wfirst+i]), f"Expected weight #{i+1} {weights[i]:.4f}, but was {props[wfirst+i]:.4f}"
  except:
    assert("Number of weights does not match expected number")

def assertMultiplicities(props, nm, mfirst, multis):
  n = len(multis)
  assert (n<=nm), f"Too much multiplicities given ({n}), but should not be more than {nm}"
  try:
    for i in range(0,n-1):
      assert isEqual(multis[i], props[mfirst+i]), f"Expected multiplicity #{i+1} {multis[i]:.4f}, but was {props[mfirst+i]:.4f}"
  except:
    assert("Number of multiplicities does not match expected number")

def checkBSpline(props, period, deg, np, nk, nw, nm, poles=[], knots=[], weights=[], multis=[]):
  assertEqualType(props, geompy.kind.CRV_BSPLINE)
  assert (period==props[1]), f"Expected CRV_BSPLINE periodicity ({period}), but was ({props[1]})"
  assert (deg==props[2]), f"Expected CRV_BSPLINE degree ({deg}), but was ({props[2]})"
  assert (np==props[3]), f"Expected CRV_BSPLINE number of poles ({np}), but was ({props[3]})"
  assert (nk==props[4]), f"Expected CRV_BSPLINE number of knots ({nk}), but was ({props[4]})"
  assert (nw==props[5]), f"Expected CRV_BSPLINE number of weights ({nw}), but was ({props[5]})"
  assert (nm==props[6]), f"Expected CRV_BSPLINE number of multiplicities ({nm}), but was ({props[6]})"
  if poles and len(poles):
    assertPoles(props, np, 7, poles)
  if knots and len(knots):
    assertKnots(props, nk, 7+3*np, knots)
  if weights and len(weights):
    assertWeights(props, nw, 7+3*np+nk, weights)

def checkBezier(props, np, nw, poles=[]):
  assertEqualType(props, geompy.kind.CRV_BEZIER)
  assert (np==props[1]), f"Expected CRV_BEZIER number of poles ({np}), but was ({props[1]})"
  assert (nw==props[2]), f"Expected CRV_BEZIER number of weights ({nw}), but was ({props[2]})"
  if poles and len(poles):
    assertPoles(props, np, 3, poles)

    
data_dir = abspath(join(dirname(getfile(lambda: None)), 'data'))

O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
Vertex_1 = geompy.MakeVertex(0, 0, 0)
Vertex_2 = geompy.MakeVertex(50, 100, 0)
Vertex_3 = geompy.MakeVertex(-10, 60, 0)
Vertex_4 = geompy.MakeVertex(0, 100, 0)
Vertex_5 = geompy.MakeVertex(-100, 100, 0)
Vertex_6 = geompy.MakeVertex(-100, 0, 0)
Vertex_7 = geompy.MakeVertex(-200, 0, 0)
Vertex_8 = geompy.MakeVertex(-200, 100, 0)

# create or import some curves
Line_1 = geompy.MakeLineTwoPnt(Vertex_1, Vertex_2)
Circle_1 = geompy.MakeCircle(Vertex_2, OZ, 50)
Ellipse_1 = geompy.MakeEllipse(Vertex_1, OZ, 200, 100, Line_1)
Arc_1 = geompy.MakeArc(Vertex_2, Vertex_3, Vertex_1)
Curve_1 = geompy.MakeCurveParametric("t", "50*sin(t)", "0", 0, 360, 30, GEOM.Interpolation, True)
Curve_2 = geompy.MakeCurveParametric("-t", "50*cos(t)", "t", 0, 360, 14, GEOM.Bezier, True)
Curve_3 = geompy.ImportBREP(join(data_dir, "KindOfShape_Curve3.brep"))
Curve_4 = geompy.ImportBREP(join(data_dir, "KindOfShape_Curve4.brep"))
Curve_5 = geompy.MakeInterpol([Vertex_1, Vertex_4, Vertex_5, Vertex_6, Vertex_7, Vertex_8], False, False)
Curve_6 = geompy.ImportBREP(join(data_dir, "KindOfShape_Curve6.brep"))
Curve_7 = geompy.MakeBezier([Vertex_5, Vertex_6, Vertex_7, Vertex_8], True)
Curve_8 = geompy.MakeBezier([Vertex_5, Vertex_6, Vertex_1, Vertex_4], False)
Curve_9 = geompy.ImportBREP(join(data_dir, "KindOfShape_Curve9.brep"))
Curve_10 = geompy.ImportBREP(join(data_dir, "KindOfShape_Curve10.brep"))

# check all curves
props = geompy.KindOfShape(Line_1)
# [SEGMENT, 0.,0.,0., 50.,100.,0.]
checkSegment(props, [0,0,0], [50,100,0])

props = geompy.KindOfShape(Circle_1)
# [CIRCLE, 50.,100.,0., 0.,0.,1., 50.]
checkCircle(props, [50,100,0], [0,0,1], 50)

props = geompy.KindOfShape(Ellipse_1)
# [ELLIPSE, 0.,0.,0., 0.,0.,1., 200., 100., 0.44721,0.89443,0., 0.44721,0.89443,0.]
checkEllipse(props, [0,0,0], [0,0,1], 200, 100, [0.4472136,0.8944272,0], [-0.8944272,0.4472136,0])

props = geompy.KindOfShape(Arc_1)
# [ARC_CIRCLE, 47.5,38.75,0., 0.,0.,1., 61.301, 50.,100.,0., 0.,0.,0.]
checkArcCircle(props, [47.5,38.75,0], [0,0,1], 61.301, [50,100,0], [0,0,0])

props = geompy.KindOfShape(Curve_1)
# [CRV_BSPLINE, 0, 3, 33, 31, 0, 31, 0.,0.,0.,..., 4,1,...,1,4]
checkBSpline(props, 0, 3, 33, 31, 0, 31, [[0,0,0],[5.246092,-6.374961,0],[10.613646,-25.338811,0],[19.662636,-44.299221,0]], [0,29.39007,51.399444,64.149986], [], [4,1,1,1,1])

props = geompy.KindOfShape(Curve_2)
# [CRV_BEZIER, 15, 0, 0.,50.,0.,...,-360.,-14.18455,360.]
checkBezier(props, 15, 0, [[0,50,0],[-25.714286,41.780762,25.714286],[-51.428571,19.825283,51.428571],[-77.142857,-8.648146,77.142857]])

props = geompy.KindOfShape(Curve_3)
# [CRV_BSPLINE, 1, 3, 8, 9, 0, 0, -100.,0.,0.,..., 0.,1.,2.,3.,4.,5.,6.,7.,8.]
checkBSpline(props, 1, 3, 8, 9, 0, 0, [[-100,0,0],[-200,200,0],[0,100,0],[200,200,0],[100,0,0],[200,-200,0],[0,-100,0],[-200,-200,0]], [0,1,2,3,4,5,6,7,8])

props = geompy.KindOfShape(Curve_4)
# [CRV_BSPLINE, 1, 2, 3, 4, 3, 0, 0.,0.,0., 100.,200.,0., 200.,0.,0., 0,1,2,3, 200.,100.,100.]
checkBSpline(props, 1, 2, 3, 4, 3, 0, [[0,0,0],[100,200,0],[200,0,0]], [0,1,2,3], [200,100,100])

props = geompy.KindOfShape(Curve_5)
# [CRV_BSPLINE, 0, 3, 8, 6, 0, 6, 0.,0.,0.,..., 100.,0.,0.,100.,200.,300.,400.,500., 4,1,1,1,1,4]
checkBSpline(props, 0, 3, 8, 6, 0, 6, [[0,0,0],[38.888889,50,0],[23.684211,113.157895,0]], [0,100,200,300,400,500], [], [4,1,1,1,1,4])

props = geompy.KindOfShape(Curve_6)
# [ARC_ELLIPSE, -18.46154,0.,64.61538, 0.57668,0.,-0.81697, 96.03993, 76.25867, 50.,-37.25439,112.94118, 50.,37.25439,112.94118, -0.81697,0.,-0.57668, 0.,1.,0.]
checkArcEllipse(props, [-18.46154,0,64.61538], [0.57668,0,-0.81697], 96.03993, 76.25867, [50,-37.25439,112.94118], [50,37.25439,112.94118], [-0.81697,0,-0.57668], [0,1,0])

props = geompy.KindOfShape(Curve_7)
# [CRV_BEZIER, 5, 0, -100.,100.,0., -100.,0.,0., -200.,0.,0., -200.,100.,0., -100.,100.,0.]
checkBezier(props, 5, 0, [[-100,100,0],[-100,0,0],[-200,0,0],[-200,100,0],[-100,100,0]])

props = geompy.KindOfShape(Curve_8)
# [CRV_BEZIER, 4, 0, -100.,100.,0., -100.,0.,0., 0.,0.,0., 0.,100.,0.]
checkBezier(props, 4, 0, [[-100,100,0],[-100,0,0],[0,0,0],[0,100,0]])

props = geompy.KindOfShape(Curve_9)
# [HYPERBOLA, -50., 0., 300., 1., 0., 0., 75., 50., 0., 0., -1., 0., 1., 0.]
checkHyperbola(props, [-50,0,300], [1,0,0], 75, 50, [0,0,-1], [0,1,0])

props = geompy.KindOfShape(Curve_10)
# [PARABOLA, -25.,0.,262.5, -0.83205,0.,-0.5547, 13.8675, 0.5547,0.,-0.83205, 0.,-1.,0.]
checkParabola(props, [-25,0,262.5], [-0.83205,0,-0.5547], 13.867505, [0.5547,0,-0.83205], [0,-1,0])
