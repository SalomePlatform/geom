# Cut

import geompy
import salome
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(25, 55, 0)
p2 = geompy.MakeVertex( 0,  0, 0)
v = geompy.MakeVector(p1, p2)

# create a cylinder
height = 35
radius1 = 20
cylinder = geompy.MakeCylinder(p1, v, radius1, height)

# create a sphere
sphere = geompy.MakeSphereR(40)

#cut
cut = geompy.MakeCut(cylinder, sphere)

# add objects in the study
id_cut = geompy.addToStudy(cut, "Cut")

# display the results
gg.createAndDisplayGO(id_cut)
gg.setDisplayMode(id_cut,1) 
