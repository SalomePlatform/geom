# Union Groups

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a box and some groups of faces on it
Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
Group_1 = geompy.CreateGroup(Box_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Group_1, [13, 23])
Group_2 = geompy.CreateGroup(Box_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Group_2, [3, 27])
Group_3 = geompy.CreateGroup(Box_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Group_3, [33, 23])
Group_4 = geompy.CreateGroup(Box_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Group_4, [31, 27])

# union groups
Group_U_1_2 = geompy.UnionGroups(Group_1, Group_2)
Group_UL_3_4 = geompy.UnionListOfGroups([Group_3, Group_4])

# publish shapes
geompy.addToStudy(Box_1, 'Box_1')
geompy.addToStudyInFather(Box_1, Group_1, 'Group_1')
geompy.addToStudyInFather(Box_1, Group_2, 'Group_2')
geompy.addToStudyInFather(Box_1, Group_3, 'Group_3')
geompy.addToStudyInFather(Box_1, Group_4, 'Group_4')
geompy.addToStudyInFather(Box_1, Group_U_1_2, 'Group_U_1_2')
geompy.addToStudyInFather(Box_1, Group_UL_3_4, 'Group_UL_3_4')
salome.sg.updateObjBrowser(True) 
