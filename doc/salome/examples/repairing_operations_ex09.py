# Limit Tolerance

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# import initial topology with bad tolerances (more than 1e-07)
shape1 = geompy.ImportBREP("my_shape_1.brep")
shape2 = geompy.ImportBREP("my_shape_2.brep")

geompy.addToStudy(shape1, "Shape 1")
geompy.addToStudy(shape2, "Shape 2")

# perform partition
try:
    part = geompy.MakePartition([shape1, shape2])
except:
    # limit tolerance
    tolerance = 1e-07
    shape1_lt = geompy.LimitTolerance(shape1, tolerance)
    shape2_lt = geompy.LimitTolerance(shape2, tolerance)

    # process shape
    good_shape1 = geompy.ProcessShape(shape1_lt, ["FixShape"], ["FixShape.Tolerance3d"], ["1e-7"])
    good_shape2 = geompy.ProcessShape(shape2_lt, ["FixShape"], ["FixShape.Tolerance3d"], ["1e-7"])

    geompy.addToStudy(good_shape1, "Shape 1 corrected")
    geompy.addToStudy(good_shape2, "Shape 2 corrected")

    # perform partition on corrected shapes
    part = geompy.MakePartition([good_shape1, good_shape2])
    pass

geompy.addToStudy(part, "Partition")
