import salome, GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create a could of points
points = [
    geompy.MakeVertex( 0,0,0 ),
    geompy.MakeVertex( 9,0,0 ),
    geompy.MakeVertex( 0,9,0 ),
    geompy.MakeVertex( 9,9,0 ),
    geompy.MakeVertex( 3,3,1 ),
    geompy.MakeVertex( 6,6,2 )]

# create SmoothingSurface object
smoothingsurface = geompy.MakeSmoothingSurface( points )

# add object in the study
id_smoothingsurface = geompy.addToStudy(smoothingsurface,"SmoothingSurface")

# display smoothingsurface
gg = salome.ImportComponentGUI("GEOM")
gg.createAndDisplayGO(id_smoothingsurface) 


