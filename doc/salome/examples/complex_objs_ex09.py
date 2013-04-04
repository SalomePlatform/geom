# Creation of a Middle Path

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# Create a box
Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)

# Get two opposite faces
[Face_1,Face_2] = geompy.SubShapes(Box_1, [31, 33])

# Get edges
Box_1_edge_12 = geompy.GetSubShape(Box_1, [12])
Box_1_edge_22 = geompy.GetSubShape(Box_1, [22])
Box_1_edge_25 = geompy.GetSubShape(Box_1, [25])
Box_1_edge_29 = geompy.GetSubShape(Box_1, [29])
Box_1_edge_8 = geompy.GetSubShape(Box_1, [8])
Box_1_edge_18 = geompy.GetSubShape(Box_1, [18])
Box_1_edge_26 = geompy.GetSubShape(Box_1, [26])
Box_1_edge_30 = geompy.GetSubShape(Box_1, [30])

# These three calls to RestorePath return the same result
Path_1 = geompy.RestorePath(Box_1, Face_1, Face_2)
Path_2 = geompy.RestorePathEdges(Box_1, [Face_1], [Face_2])
Path_3 = geompy.RestorePathEdges(Box_1,
                                 [Box_1_edge_12, Box_1_edge_22, Box_1_edge_25, Box_1_edge_29],
                                 [Box_1_edge_8, Box_1_edge_18, Box_1_edge_26, Box_1_edge_30])

# Publish created objects
geompy.addToStudy( Box_1, 'Box_1' )
geompy.addToStudyInFather( Box_1, Face_1, 'Face_1' )
geompy.addToStudyInFather( Box_1, Face_2, 'Face_2' )
geompy.addToStudyInFather( Box_1, Box_1_edge_25, 'Box_1:edge_25' )
geompy.addToStudyInFather( Box_1, Box_1_edge_22, 'Box_1:edge_22' )
geompy.addToStudyInFather( Box_1, Box_1_edge_12, 'Box_1:edge_12' )
geompy.addToStudyInFather( Box_1, Box_1_edge_29, 'Box_1:edge_29' )
geompy.addToStudyInFather( Box_1, Box_1_edge_18, 'Box_1:edge_18' )
geompy.addToStudyInFather( Box_1, Box_1_edge_26, 'Box_1:edge_26' )
geompy.addToStudyInFather( Box_1, Box_1_edge_8, 'Box_1:edge_8' )
geompy.addToStudyInFather( Box_1, Box_1_edge_30, 'Box_1:edge_30' )
geompy.addToStudy( Path_1, 'Path_1' )
geompy.addToStudy( Path_2, 'Path_2' )
geompy.addToStudy( Path_3, 'Path_3' )
