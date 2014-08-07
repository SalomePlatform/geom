# Hexagonal Solid

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
p0  = geompy.MakeVertex(  0.,   0.,   0.)
p1  = geompy.MakeVertex(  0.,   0.,  40.)
p2  = geompy.MakeVertex( 70., -15.,   0.)
p3  = geompy.MakeVertex( 70., -15.,  70.)

p4  = geompy.MakeVertex(  0.,  70.,   0.)
p5  = geompy.MakeVertex(  0.,  70.,  40.)
p6  = geompy.MakeVertex( 70.,  70.,   0.)
p7  = geompy.MakeVertex( 70.,  70.,  70.)

p8  = geompy.MakeVertex(  0., -50.,   0.)
p9  = geompy.MakeVertex(  0., -50.,  40.)
p10 = geompy.MakeVertex( 70., -35.,   0.)
p11 = geompy.MakeVertex( 70., -35.,  70.)

# create faces
qface1 = geompy.MakeQuad4Vertices(p0, p1, p2, p3)
qface2 = geompy.MakeQuad4Vertices(p4, p5, p6, p7)
qface3 = geompy.MakeQuad4Vertices(p0, p1, p4, p5)
qface4 = geompy.MakeQuad4Vertices(p2, p3, p6, p7)
qface5 = geompy.MakeQuad4Vertices(p0, p2, p4, p6)
qface6 = geompy.MakeQuad4Vertices(p1, p3, p5, p7)
qface7 = geompy.MakeQuad4Vertices(p8, p9, p10, p11)

# create a hexahedral solid between two given faces
solid1 = geompy.MakeHexa2Faces(qface1, qface7)

# create a hexahedral solid, bounded by six given faces
solid2 = geompy.MakeHexa(qface1, qface2, qface3, qface4, qface5, qface6)

# add objects in the study
geompy.addToStudy(qface1,"qface1")
geompy.addToStudy(qface2,"qface2")
geompy.addToStudy(qface3,"qface3")
geompy.addToStudy(qface4,"qface4")
geompy.addToStudy(qface5,"qface5")
geompy.addToStudy(qface6,"qface6")
geompy.addToStudy(qface7,"qface7")

id_solid1 = geompy.addToStudy(solid1,"Solid1")
id_solid2 = geompy.addToStudy(solid2,"Solid2")

# display solids
gg.createAndDisplayGO(id_solid1)
gg.setDisplayMode(id_solid1, 1)
gg.createAndDisplayGO(id_solid2)
gg.setDisplayMode(id_solid2, 1) 
