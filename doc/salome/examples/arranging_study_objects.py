# Using SALOME NoteBook

import salome
salome.salome_init()

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

Circle_1 = geompy.MakeCircle(None, None, 100)
Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
Cylinder_1 = geompy.MakeCylinderRH(100, 300)
geompy.addToStudy( Circle_1, 'Circle_1' )
geompy.addToStudy( Box_1, 'Box_1' )
geompy.addToStudy( Cylinder_1, 'Cylinder_1' )

### Folders and it's content
Basic = geompy.NewFolder('Basic')
geompy.PutToFolder(Circle_1, Basic)
Primitives = geompy.NewFolder('Primitives')
geompy.PutListToFolder([Box_1, Cylinder_1], Primitives)


if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(True)
