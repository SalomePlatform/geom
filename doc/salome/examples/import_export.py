# Import/Export

import geompy
import salome

# Example of import from IGES using various formats

# get a path to SAMPLES_SRC
import os
thePath = os.getenv("DATA_DIR")
# create filenames
theFileName1 = thePath + "/Shapes/Iges/boite-3Dipsos_m.igs"
theFileName2 = thePath + "/Shapes/Iges/boite-3Dipsos_mm.igs"
#print "thePath = ", thePath

# get units from files
UnitName1 = geompy.GetIGESUnit(theFileName1)
UnitName2 = geompy.GetIGESUnit(theFileName2)
print "UnitName1 = ", UnitName1
print "UnitName2 = ", UnitName2

# import shapes
Shape1 = geompy.ImportIGES(theFileName1)
Shape2 = geompy.ImportIGES(theFileName2)
Shape3 = geompy.ImportFile(theFileName2,"IGES_SCALE")
[Xmin1,Xmax1, Ymin1,Ymax1, Zmin1,Zmax1] = geompy.BoundingBox(Shape1)
[Xmin2,Xmax2, Ymin2,Ymax2, Zmin2,Zmax2] = geompy.BoundingBox(Shape2)
[Xmin3,Xmax3, Ymin3,Ymax3, Zmin3,Zmax3] = geompy.BoundingBox(Shape3)
geompy.addToStudy(Shape1, "3Dipsos_m")
geompy.addToStudy(Shape2, "3Dipsos_mm")
geompy.addToStudy(Shape3, "3Dipsos_mm_scaled")
d1 = (Xmax1-Xmin1)*(Xmax1-Xmin1) + (Ymax1-Ymin1)*(Ymax1-Ymin1) + (Zmax1-Zmin1)*(Zmax1-Zmin1)
d2 = (Xmax2-Xmin2)*(Xmax2-Xmin2) + (Ymax2-Ymin2)*(Ymax2-Ymin2) + (Zmax2-Zmin2)*(Zmax2-Zmin2)
d3 = (Xmax3-Xmin3)*(Xmax3-Xmin3) + (Ymax3-Ymin3)*(Ymax3-Ymin3) + (Zmax3-Zmin3)*(Zmax3-Zmin3)
import math
dd32 = math.sqrt(d3/d2)
dd12 = math.sqrt(d1/d2)
dd31 = math.sqrt(d3/d1)
# values dd31, dd12 and dd31 can be using for checking
print "dd32 = ",dd32,"  dd12 = ",dd12,"  dd31 = ",dd31
