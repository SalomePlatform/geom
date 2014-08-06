# Creation of a Rectangle

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create vertices
px = geompy.MakeVertex(100., 0.  , 0.  )
py = geompy.MakeVertex(0.  , 100., 0.  )

# create a vector on two points
vxy  = geompy.MakeVector(px, py)

# create a rectangle in OXY plane
face1 = geompy.MakeFaceHW(100, 100, 1)

# create a rectangle using normal vector
face2 = geompy.MakeFaceObjHW(vxy, 50, 150)

# create a rectangle from other face
face3 = geompy.MakeFaceObjHW(face2, 150, 50)

# add objects in the study
id_face1  = geompy.addToStudy(face1,"Face1")
id_face2  = geompy.addToStudy(face2,"Face2")
id_face3  = geompy.addToStudy(face3,"Face3")

# display rectangles
gg.createAndDisplayGO(id_face1)
gg.createAndDisplayGO(id_face2)
gg.createAndDisplayGO(id_face3)
