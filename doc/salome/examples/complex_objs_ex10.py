# Creation of Tangent Plane On Face

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# Create Vertexes for curve
Vertex_1 = geompy.MakeVertex(0, 0, 0)
Vertex_2 = geompy.MakeVertex(0, 90, 30)
Vertex_3 = geompy.MakeVertex(100, 90, 0)
Vertex_4 = geompy.MakeVertex(-100, 90, 0)
# Create curve
Curve_1 = geompy.MakeInterpol([Vertex_4, Vertex_2, Vertex_3, Vertex_1])
# Create Face by Extrusion of the Curve
Extrusion_1 = geompy.MakePrismDXDYDZ(Curve_1, 0, 30, -60)
# Make Tangent on this Extrusion (Face)
Tangent_1 = geompy.MakeTangentPlaneOnFace(Extrusion_1, 0.7, 0.5, 150)
# Publish in the study
geompy.addToStudy( Vertex_1, "Vertex_1" )
geompy.addToStudy( Vertex_2, "Vertex_2" )
geompy.addToStudy( Vertex_3, "Vertex_3" )
geompy.addToStudy( Vertex_4, "Vertex_4" )
geompy.addToStudy( Curve_1, "Curve_1" )
geompy.addToStudy( Extrusion_1, "Extrusion_1" )
geompy.addToStudy( Tangent_1, "Tangent_1" )
