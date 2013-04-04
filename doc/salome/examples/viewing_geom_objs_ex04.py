# Set Point Marker

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

texture = geompy.LoadTexture(os.getenv("DATA_DIR")+"/Textures/texture1.dat")

v1 = geompy.MakeVertex(0, 0, 0)
v2 = geompy.MakeVertex(100, 0, 0)
v3 = geompy.MakeVertex(0, 100, 0)

v1.SetMarkerStd(GEOM.MT_O_PLUS, GEOM.MS_25)
v2.SetMarkerStd(GEOM.MT_BALL,   GEOM.MS_40)
v3.SetMarkerTexture(texture)
