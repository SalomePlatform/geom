# Adding an object to the group

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create two vertices
p0 = geompy.MakeVertex(0.  , 0.  , 0.  )
p200 = geompy.MakeVertex(200., 200., 200.)

# create a box from two points
Box = geompy.MakeBoxTwoPnt(p0, p200)

# create a group from the faces of the box
group = geompy.CreateGroup(Box, geompy.ShapeType["FACE"])

# add objects to the group
SubFaceList = geompy.SubShapeAllSortedCentres(Box, geompy.ShapeType["FACE"])
for i in [0, 3, 5] :
    FaceID = geompy.GetSubShapeID(Box, SubFaceList[i])
    geompy.AddObject(group, FaceID)
id_group1 = geompy.addToStudy(group, "Group1")

# display the contents of the group
gg.createAndDisplayGO(id_group1)
salome.sg.updateObjBrowser(True) 
