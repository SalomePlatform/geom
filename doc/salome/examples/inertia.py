# Inertia

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a box
box = geompy.MakeBoxDXDYDZ(100,30,100)
In = geompy.Inertia(box)
print "\nInertia matrix of box 100x30x100:"
print " (", In[0], ", ", In[1], ", ", In[2], ")"
print " (", In[3], ", ", In[4], ", ", In[5], ")"
print " (", In[6], ", ", In[7], ", ", In[8], ")"
print "Main moments of inertia of box 100x30x100:"
print " Ix = ", In[9], ", Iy = ", In[10], ", Iz = ", In[11]
