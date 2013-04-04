# Minimal Distance

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# Create two curves with three closest points
Vertex_1 = geompy.MakeVertex(0, 0, 0)
Vertex_2 = geompy.MakeVertex(0, 70, 0)
Vertex_3 = geompy.MakeVertex(30, 100, 0)
Vertex_4 = geompy.MakeVertex(60, 70, 0)
Vertex_5 = geompy.MakeVertex(90, 100, 0)
Vertex_6 = geompy.MakeVertex(120, 70, 0)
Vertex_7 = geompy.MakeVertex(120, 0, 0)
Vertex_8 = geompy.MakeVertex(90, -30, 0)
Vertex_9 = geompy.MakeVertex(60, 0, 0)
Vertex_10 = geompy.MakeVertex(30, -30, 0)

geompy.addToStudy( Vertex_1, 'Vertex_1' )
geompy.addToStudy( Vertex_2, 'Vertex_2' )
geompy.addToStudy( Vertex_3, 'Vertex_3' )
geompy.addToStudy( Vertex_4, 'Vertex_4' )
geompy.addToStudy( Vertex_5, 'Vertex_5' )
geompy.addToStudy( Vertex_6, 'Vertex_6' )
geompy.addToStudy( Vertex_7, 'Vertex_7' )
geompy.addToStudy( Vertex_8, 'Vertex_8' )
geompy.addToStudy( Vertex_9, 'Vertex_9' )
geompy.addToStudy( Vertex_10, 'Vertex_10' )

Curve_a = geompy.MakeInterpol([Vertex_2, Vertex_3, Vertex_4, Vertex_5, Vertex_6], False, True)
Curve_b = geompy.MakeInterpol([Vertex_1, Vertex_7, Vertex_8, Vertex_9, Vertex_10], False, True)

geompy.addToStudy( Curve_a, 'Curve_a' )
geompy.addToStudy( Curve_b, 'Curve_b' )

# Get all closest points
[nbSols, listCoords] = geompy.ClosestPoints(Curve_a, Curve_b)

for i in range(nbSols):
  v1 = geompy.MakeVertex(listCoords[i*6 + 0], listCoords[i*6 + 1], listCoords[i*6 + 2])
  v2 = geompy.MakeVertex(listCoords[i*6 + 3], listCoords[i*6 + 4], listCoords[i*6 + 5])

  geompy.addToStudy(v1, 'MinDist_%d_Curve_a'%(i+1))
  geompy.addToStudy(v2, 'MinDist_%d_Curve_b'%(i+1))

# Get minimum distance
print "Minimal distance between Curve_a and Curve_b is", geompy.MinDistance(Curve_a, Curve_b)

# Get minimum distance with components along axes
[aDist, DX, DY, DZ] = geompy.MinDistanceComponents(Curve_a, Curve_b)
print "Minimal distance between Curve_a and Curve_b is (", DX, ",", DY, ",", DZ, ")"
