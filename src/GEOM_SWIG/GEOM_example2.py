#==============================================================================
#  File      : GEOM_example2.py
#  Created   : sam nov 10 00:53:24 CET 2001
#  Author    : Paul RASCLE, EDF
#  Project   : SALOME
#  Copyright : EDF 2001
#  $Header$
#==============================================================================

import salome
import geompy
import math

geom = salome.lcc.FindOrLoadComponent("FactoryServer", "GEOM")
myBuilder = salome.myStudy.NewBuilder()

point0  = geom.MakePointStruct(0.,0.,0.)
pointz1 = geom.MakePointStruct(0.,0.,1.)
dirz = geom.MakeDirection(pointz1)

torus1 = geompy.MakeTorus(point0,dirz,150.,25.)
id_torus1 = geompy.addToStudy(torus1,"torus1")

torus2 = geom.MakeCopy(torus1)
torus2 = geom.MakeTranslation(torus2,0.,0.,100.)
id_torus2 = geompy.addToStudy(torus2,"torus2")

cylz1 = geompy.MakeCylinder(point0,dirz,25.,100.)

ind = 0
cyllist = []
while ind < 6:
    acyl = geom.MakeCopy(cylz1)
    x = 150. * math.cos(ind * math.pi/3.)
    y = 150. * math.sin(ind * math.pi/3.)
    z = 0.
    name = "cyl%d"%(ind)
    acyl = geompy.MakeTranslation(acyl,x,y,z)
    id_acyl = geompy.addToStudy(acyl,name)
    cyllist.append(acyl)
    ind = ind +1



                                  
