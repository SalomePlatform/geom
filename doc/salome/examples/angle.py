# Angle

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math

OX  = geompy.MakeVectorDXDYDZ(10, 0,0)
OXY = geompy.MakeVectorDXDYDZ(10,10,0)

# in one plane
Angle = geompy.GetAngle(OX, OXY)

print "\nAngle between OX and OXY = ", Angle
if math.fabs(Angle - 45.0) > 1e-05:
    print "  Error: returned angle is", Angle, "while must be 45.0"
    pass

Angle = geompy.GetAngleRadians(OX, OXY)

print "\nAngle between OX and OXY in radians = ", Angle
if math.fabs(Angle - math.pi/4) > 1e-05:
    print "  Error: returned angle is", Angle, "while must be pi/4"
    pass

Angle = geompy.GetAngleVectors(OX, OXY, True)

print "\nAngle between vectors OX and OXY = ", Angle
if math.fabs(Angle - 45.0) > 1e-05:
    print "  Error: returned angle is", Angle, "while must be 45.0"
    pass

Angle = geompy.GetAngleRadiansVectors(OX, OXY, False)

print "\nBig angle between vectors OX and OXY in radians = ", Angle
if math.fabs(Angle - math.pi*7./4.) > 1e-05:
    print "  Error: returned angle is", Angle, "while must be 7*pi/4"
    pass

# not in one plane
OXY_shift = geompy.MakeTranslation(OXY,10,-10,20)
Angle = geompy.GetAngle(OX, OXY_shift)

print "\nAngle between OX and OXY_shift = ", Angle
if math.fabs(Angle - 45.0) > 1e-05:
    print "  Error: returned angle is", Angle, "while must be 45.0"
    pass

# not linear
pnt1 = geompy.MakeVertex(0, 0, 0)
pnt2 = geompy.MakeVertex(10, 0, 0)
pnt3 = geompy.MakeVertex(20, 10, 0)
arc  = geompy.MakeArc(pnt1, pnt2, pnt3)
try:
    Angle = geompy.GetAngle(OX, arc)
    pass
except:
    # It is a correct case, because GetAngle() must not be computed on curvilinear edges.
    pass

assert (not geompy.MeasuOp.IsDone() ), 'Angle must not be computed on curvilinear edges.'
