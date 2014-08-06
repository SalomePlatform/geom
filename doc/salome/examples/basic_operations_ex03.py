# Restore presentation parameters and sub-shapes

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import SALOMEDS

# create a box and a cylinder
box = geompy.MakeBoxDXDYDZ(200, 200, 200)
cyl = geompy.MakeCylinderRH(100, 300)

# create translated box
vec = geompy.MakeVectorDXDYDZ(100, 50, 0)
tra = geompy.MakeTranslationVector(box, vec)

# create partition objects
partition1 = geompy.MakePartition([box, cyl])
partition2 = geompy.MakePartition([box], [cyl])
partition3 = geompy.MakePartition([box], [tra])

# set colours
box.SetColor(SALOMEDS.Color(1,0,0))
cyl.SetColor(SALOMEDS.Color(0,1,0))

# add objects in the study
geompy.addToStudy(box, "Box")
geompy.addToStudy(cyl, "Cylinder")
geompy.addToStudy(vec, "Vector")
geompy.addToStudy(tra, "Translation")
geompy.addToStudy(partition1, "Partition_1")
geompy.addToStudy(partition2, "Partition_2")
geompy.addToStudy(partition3, "Partition_3")

# Restore presentation parameters and sub-shapes
# different methods can be used to find the sub-shapes in the result:
# GetInPlace, GetSame, GetInPlaceByHistory, GetShapesOnShape.
# By default, GetInPlace method is used (GEOM.FSM_GetInPlace)
geompy.RestoreSubShapes(partition1)

geompy.RestoreSubShapes(partition2, [], GEOM.FSM_GetInPlace)

# The list of arguments can be used to avoid restoring all arguments,
# but restore only the passed.
geompy.RestoreSubShapes(partition3, [tra], GEOM.FSM_GetInPlaceByHistory)

# To find sub-shapes in a transformed shape only one method could be
# used: pass GEOM.FSM_Transformed for that.
# True passed for the last argument, means that the transformed shape
# will inherit colour and sub-shapes from its first argument (see above
# MakeTranslation).
geompy.RestoreSubShapes(tra, [], GEOM.FSM_Transformed, True)

# Also we could do this directly with method addToStudy:
partition4 = geompy.MakePartition([box, tra])
geompy.addToStudy(partition4, "Partition_4", True, [],
                  GEOM.FSM_GetInPlaceByHistory, False)
