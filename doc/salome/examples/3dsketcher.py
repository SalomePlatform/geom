# 3D Sketcher

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# Create a 3D sketcher (wire) on the given points coordinates
sketcher1 = geompy.Make3DSketcher([ 0,0,0, 50,50,50, 0,50,0, 50,0,25, 10,20,100, 0,0,0 ])

# add object in the study
id_sketcher1 = geompy.addToStudy(sketcher1, "Sketcher1")

# display the sketcher
gg.createAndDisplayGO(id_sketcher1)

# Create a 3D sketcher (wire) with Sketcher3D interface

# get the interface instance
sk = geompy.Sketcher3D()

# add three points with absolute coordinates
# the first point will be the start point of sketcher
# two segments will be added by this command
sk.addPointsAbsolute(1,2,3, 7,0,0, 10,-3.5,-11)

# add one segment, defined by two angles in "OXY" coordinate system and length
sk.addPointRadiusAnglesRelative(45, 0, 100, "OXY")

# add three points with relative coordinates
# three segments will be added by this command
sk.addPointsRelative(20,0,0, 20,0,100, -40,0,-50)

# set to close the sketcher
sk.close()

# obtain the sketcher result
sketcher2 = sk.wire()

# add object in the study
id_sketcher2 = geompy.addToStudy(sketcher2, "Sketcher2")

# display the sketcher
gg.createAndDisplayGO(id_sketcher2)
