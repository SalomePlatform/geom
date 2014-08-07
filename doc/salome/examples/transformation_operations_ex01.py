# Translation

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# create a vertex and a vector
p1 = geompy.MakeVertex(10, 40, 0)
p2 = geompy.MakeVertex( 0,  0, 50)
p3 = geompy.MakeVertex(50, 80, 0)
v = geompy.MakeVector(p1, p2)
vt = geompy.MakeVector(p1, p3)

# create a cylinder
height = 35
radius1 = 20
cylinder = geompy.MakeCylinder(p1, v, radius1, height)

# translate the given object along the vector, specified by its end points
# (all three functions produce the same result)
translation1 = geompy.MakeTranslationTwoPoints(cylinder, p1, p3)
translation2 = geompy.MakeTranslation(cylinder, 40, 40, 0)
translation3 = geompy.MakeTranslationVector(cylinder, vt)
translation4 = geompy.MakeTranslationVectorDistance(cylinder, vt, 200)

# add objects in the study
id_cylinder = geompy.addToStudy(cylinder, "Cylinder")
id_translation1 = geompy.addToStudy(translation1, "Translation1")
id_translation2 = geompy.addToStudy(translation2, "Translation2")
id_translation3 = geompy.addToStudy(translation3, "Translation3")
id_translation4 = geompy.addToStudy(translation4, "Translation4")

# display the results
gg.createAndDisplayGO(id_cylinder)
gg.setDisplayMode(id_cylinder,1)
gg.createAndDisplayGO(id_translation1)
gg.setDisplayMode(id_translation1,1)
gg.createAndDisplayGO(id_translation2)
gg.setDisplayMode(id_translation2,1)
gg.createAndDisplayGO(id_translation3)
gg.setDisplayMode(id_translation3,1) 
gg.createAndDisplayGO(id_translation4)
gg.setDisplayMode(id_translation4,1) 
