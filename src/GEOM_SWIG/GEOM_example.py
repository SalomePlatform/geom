#==============================================================================
#  File      : GEOM_example.py
#  Created   : mer oct 17 16:38:32 CEST 2001
#  Author    : Paul RASCLE, EDF
#  Project   : SALOME
#  Copyright : EDF 2001
#  $Header$
#==============================================================================

import salome
import geompy

ind = 0
boxlist = []
while ind < 5:
    x1 = 10. * ind
    y1 = 0.
    z1 = 0.
    x2 = 10. * (ind+1)
    y2 = 20. * (ind+1)
    z2 = 30. * (ind+1)
    print x1, y1, z1, x2, y2, z2
    name = "box%d"%(ind)
    box = geompy.MakeBox(x1, y1, z1, x2, y2, z2)
    id_box = geompy.addToStudy(box, name)
    boxlist.append(box)
    ind = ind +1

