# Intersect Groups

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

# intersect groups
Group_I_1_3 = geompy.IntersectGroups(Group_1, Group_3)
Group_IL_1_3 = geompy.IntersectListOfGroups([Group_1, Group_3])

# publish shapes
geompy.addToStudy(Box_1, 'Box_1')
geompy.addToStudyInFather(Box_1, Group_1, 'Group_1')
geompy.addToStudyInFather(Box_1, Group_2, 'Group_2')
geompy.addToStudyInFather(Box_1, Group_3, 'Group_3')
geompy.addToStudyInFather(Box_1, Group_4, 'Group_4')
geompy.addToStudyInFather(Box_1, Group_I_1_3, 'Group_I_1_3')
geompy.addToStudyInFather(Box_1, Group_IL_1_3, 'Group_IL_1_3')
salome.sg.updateObjBrowser(True) 
