# Bounding Box

import geompy

# create a box
box = geompy.MakeBoxDXDYDZ(100,30,100)

bb = geompy.BoundingBox(box)
print "\nBounding Box of box 100x30x100:"
print " Xmin = ", bb[0], ", Xmax = ", bb[1]
print " Ymin = ", bb[2], ", Ymax = ", bb[3]
print " Zmin = ", bb[4], ", Zmax = ", bb[5]

aBB = geompy.MakeBoundingBox(box)

geompy.addToStudy(box, "box 100x30x100")
geompy.addToStudy(aBB, "Bounding box of box 100x30x100")
