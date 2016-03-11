# Set Point Marker

import os
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

texture_file = os.path.join(os.getenv("GEOM_ROOT_DIR"), "share", "salome", "resources", "geom", "textures", "texture1.dat")
texture = geompy.LoadTexture(texture_file)

v1 = geompy.MakeVertex(0, 0, 0)
v2 = geompy.MakeVertex(100, 0, 0)
v3 = geompy.MakeVertex(0, 100, 0)

v1.SetMarkerStd(GEOM.MT_O_PLUS, GEOM.MS_25)
v2.SetMarkerStd(GEOM.MT_BALL,   GEOM.MS_40)
v3.SetMarkerTexture(texture)
