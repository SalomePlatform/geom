# Get shared sub-shapes

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import SALOMEDS

# create a box and partigion it by two planes
box = geompy.MakeBoxDXDYDZ(200, 200, 200)
p = geompy.MakeVertex(100, 100, 100)
v1 = geompy.MakeVectorDXDYDZ(1, 1, 0)
v2 = geompy.MakeVectorDXDYDZ(1, -1, 0)
pln1 = geompy.MakePlane(p, v1, 2000)
pln2 = geompy.MakePlane(p, v2, 2000)
partition = geompy.MakePartition([box], [pln1, pln2])

# extract solids from result of partition
solids = geompy.SubShapeAllSorted(partition, geompy.ShapeType['SOLID'])

# get shared shapes from the partition (compound of 4 solids)
# a) faces that are shared by all 4 solids (0 found)
pF_T = geompy.GetSharedShapesMulti(partition, geompy.ShapeType['FACE'])
# b) faces that are shared by any couple of solids (4 found)
pF_F = geompy.GetSharedShapesMulti(partition, geompy.ShapeType['FACE'], False)
# c) edges that are shared by all 4 solids (1 found)
pE_T = geompy.GetSharedShapesMulti(partition, geompy.ShapeType['EDGE'])
# d) edges that are shared by any couple of solids (13 found)
pE_F = geompy.GetSharedShapesMulti(partition, geompy.ShapeType['EDGE'], False)

# get shared shapes from the list of solids
# a) faces that are shared by all 4 solids (0 found)
sF_T = geompy.GetSharedShapesMulti(solids, geompy.ShapeType['FACE'])
# b) faces that are shared by 1st/2nd, 1st/3rd and 1st/4th solids (2 found)
sF_F = geompy.GetSharedShapesMulti(solids, geompy.ShapeType['FACE'], False)
# c) edges that are shared by all 4 solids (1 found)
sE_T = geompy.GetSharedShapesMulti(solids, geompy.ShapeType['EDGE'])
# d) edges that are shared by 1st/2nd, 1st/3rd and 1st/4th solids (7 found)
sE_F = geompy.GetSharedShapesMulti(solids, geompy.ShapeType['EDGE'], False)
