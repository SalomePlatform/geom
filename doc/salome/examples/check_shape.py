# Check Shape

import geompy

# create a box
box = geompy.MakeBoxDXDYDZ(100,30,100)
IsValid = geompy.CheckShape(box)
if IsValid == 0:
    raise RuntimeError, "Invalid box created"
else:
    print "\nBox is valid"
