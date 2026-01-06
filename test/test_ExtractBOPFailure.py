# Copyright (C) 2024-2026  CEA, EDF
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# Test ExtractBOPFailure method

from inspect import getfile
from os.path import abspath, dirname, join
from salome.kernel import salome
from salome.geom import geomBuilder
geompy = geomBuilder.New()

data_dir = abspath(join(dirname(getfile(lambda: None)), 'data'))

### Case 1: Invalid face with (3) intersecting wires
Shapes_1 = geompy.ImportBREP(join(data_dir, "ExtractBOPFailure_compound1.brep"))
geompy.addToStudy( Shapes_1, 'CompoundPostIdealisation' )

try :
  Partition_1 = geompy.MakePartition([Shapes_1],[])
except :
  print("EXCEPTION: Partition problem")

print("Case 1: Invalid face with (3) intersecting wires")
(IsValid, GFRes, ShapeErrors) = geompy.ExtractBOPFailure([Shapes_1], theTimer=True, theVerbose=True)

assert GFRes is not None, "Resulting shape expected in Case #1"
geompy.addToStudy( GFRes, 'GFRes_Case_1' )
assert not IsValid, "IsValid == False expected in Case #1"

assert len(ShapeErrors) == 1, "One error expected"
list_ids = ShapeErrors[0].incriminated
assert len(list_ids) == 1, "One incriminated shape expected"

[Face_1] = geompy.SubShapes(GFRes, list_ids)
geompy.addToStudyInFather( GFRes, Face_1, 'Bad face' )

list_wires = geompy.ExtractShapes(Face_1, geompy.ShapeType["WIRE"], True, "Wire")

assert len(list_wires) == 3, "Three wires expected"

### Case 2: Empty list of input shapes
print("Case 2: Empty list of input shapes")
(IsValid, GFRes, ShapeErrors) = geompy.ExtractBOPFailure([], theVerbose=True)
assert GFRes is None, "General Fuse result is NOT expected on empty list of shapes"
assert not IsValid, "IsValid == False expected on empty list of shapes"
assert len(ShapeErrors) == 0, "ExtractBOPFailure should report no errors on empty list of shapes"

### Case 3: One shape in list, and it is not a COMPOUND
print("Case 3: One shape in list, and it is not a COMPOUND")
(IsValid, GFRes, ShapeErrors) = geompy.ExtractBOPFailure([geompy.MakeBoxDXDYDZ(10,10,10)], theVerbose=True)
assert GFRes is None, "General Fuse result is NOT expected"
assert not IsValid, "IsValid == False expected"
assert len(ShapeErrors) == 0, "ExtractBOPFailure should report no errors here"

### Case 4: Unusual type Case (two orthogonal faces, Partition OK)
print("Case 4: Unusual type Case (two orthogonal faces, Partition OK)")
origin = geompy.MakeVertex(0,0,0)
oX = geompy.MakeVectorDXDYDZ(1,0,0)
YOZ = geompy.MakePlane(origin, oX, 5)
oY = geompy.MakeVectorDXDYDZ(0,1,0)
XOZ = geompy.MakePlane(origin, oY, 5)
(IsValid, GFRes, ShapeErrors) = geompy.ExtractBOPFailure([YOZ, XOZ], theVerbose=True)
assert GFRes is not None, "General Fuse result is expected"
geompy.addToStudy( GFRes, 'GFRes_Case_4' )
assert IsValid, "Valid result is expected"
assert len(ShapeErrors) == 0, "ExtractBOPFailure should report no errors here"

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
