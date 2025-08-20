from salome.kernel import salome
salome.salome_init()
from salome.kernel import GEOM
from salome.geom import geomBuilder

geompy = geomBuilder.New()
aCircle = geompy.MakeCircleR(150.)
aCylinder = geompy.MakeCylinderRH(100.,200.)

aCR = geompy.CanonicalRecognition()
aCR.isCylinder(aCylinder, 0.1)
