# Check shape applicability for Boolean Operations

import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

from inspect import getfile
from os.path import abspath, dirname, join

data_dir = abspath(join(dirname(getfile(lambda: None)), 'data'))
shape = geompy.ImportBREP(join(data_dir, 'test_twisted_face.brep'))

tol = geompy.Tolerance(shape)

# perform all checks
check = geompy.CheckConformity(shape)
assert(not check.isValid())
assert(len(check.smallEdges()) == 4)
assert(len(check.selfIntersected2D()) == 2)
assert(len(check.interferingSubshapes()) == 6)
for p in check.interferingSubshapes():
    assert(geomBuilder.EnumToLong(p.first.GetShapeType()) == geompy.ShapeType["VERTEX"] and
           geomBuilder.EnumToLong(p.second.GetShapeType()) == geompy.ShapeType["VERTEX"])
assert(len(check.distantShapes()) == 2)
for p in check.distantShapes():
    assert(geomBuilder.EnumToLong(p.first.GetShapeType()) == geompy.ShapeType["EDGE"] and
           geomBuilder.EnumToLong(p.second.GetShapeType()) == geompy.ShapeType["FACE"])
assert(len(check.distantShapes(tolerance = 300.0)) == 1)
assert(len(check.distantShapes(tolerance = 400.0)) == 0)

newTol = check.updateTolerance()
assert(newTol < max(tol))
