# Creation of a Wire

import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()
gg = salome.ImportComponentGUI("GEOM")

# create vertices
px   = geompy.MakeVertex(100., 0.  , 0.  )
py   = geompy.MakeVertex(0.  , 100., 0.  )
pz   = geompy.MakeVertex(0.  , 0.  , 100.)

# create a vector from two points
vxy = geompy.MakeVector(px, py)

# create an arc from three points
arc = geompy.MakeArc(py, pz, px)

# create a wire
wire = geompy.MakeWire([vxy, arc])

# add an object in the study
id_wire = geompy.addToStudy(wire,"Wire")

# display the wire
gg.createAndDisplayGO(id_wire) 

# Create a wire from edges that are not connected to each other.
# Given tolerance should cover the gaps to build connected wire.
# Three different modes to do that are possible.
gap = 1
tolerance = gap
px_gap  = geompy.MakeVertex(100. + gap, 0.  , 0.  )
py_gap  = geompy.MakeVertex(0.  , 100. + gap, 0.  )
vxy_gap = geompy.MakeVector(px_gap, py_gap)

# 1. Join edges by fixing tolerance
wire_ft = geompy.MakeWire([vxy_gap, arc], tolerance, "wire_fixed_tolerance")
# final wire tolerance will be near the half of the gap (0.5)
print(geompy.Tolerance(wire_ft)[5])
assert(abs(geompy.Tolerance(wire_ft)[5] - 0.5) < 1e-04)

# 2. Join edges by changing their underlying curves (but keeping curve types)
#    This functionality is available in TUI only.
wire_cc = geompy.MakeWireConstCurveType([vxy_gap, arc], tolerance, "wire_const_curve_type")
# final wire tolerance will stay small
print(geompy.Tolerance(wire_cc)[5])
assert(geompy.Tolerance(wire_cc)[5] < 1e-04)

# 3. Join edges by changing their underlying curves (approximating with b-splines)
#    This functionality is available in TUI only.
wire_ap = geompy.MakeWireWithMode([vxy_gap, arc], tolerance, GEOM.WBM_Approximation, "wire_approximated_curves")
# final wire tolerance will stay small
print(geompy.Tolerance(wire_ap)[5])
assert(geompy.Tolerance(wire_ap)[5] < 1e-04)
