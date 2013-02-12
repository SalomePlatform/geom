# Set Point Marker

import salome
import geompy

texture = geompy.LoadTexture("/users/user/mytexture.dat")

v1 = geompy.MakeVertex(0, 0, 0)
v2 = geompy.MakeVertex(100, 0, 0)
v3 = geompy.MakeVertex(0, 100, 0)

v1.SetMarkerStd(GEOM.MT_O_PLUS, GEOM.MS_25)
v2.SetMarkerStd(GEOM.MT_BALL,   GEOM.MS_40)
v3.SetMarkerTexture(texture)
