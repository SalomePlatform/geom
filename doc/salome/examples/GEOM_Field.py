# Geom Field

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
gg = salome.ImportComponentGUI("GEOM")

# a box
shape = geompy.MakeBoxDXDYDZ( 1, 1, 1, theName="box")

# Create fields holding sub-shape IDs as strings,
# which can be useful to see sub-shape IDs in the Viewer

componentNames = ['ID']
fieldDataType = GEOM.FDT_String
stepID = 0
stamp = 0

dim = 0 # == vertices
values = [str(i) for i in geompy.SubShapeAllIDs( shape, geompy.ShapeType["VERTEX"] )]
f0 = geompy.CreateField( shape, "vertices", fieldDataType, dim, componentNames )
s0 = f0.addStep( stepID, stamp, values )
geompy.addToStudyInFather( shape, f0, f0.GetName() )
s0id = geompy.addToStudyInFather( f0, s0, s0.GetName() )

dim = 1 # == edges
values = [str(i) for i in geompy.SubShapeAllIDs( shape, geompy.ShapeType["EDGE"] )]
f1 = geompy.CreateField( shape, "edges", fieldDataType, dim, componentNames )
s1 = f1.addStep( stepID, stamp, values )
geompy.addToStudyInFather( shape, f1, f1.GetName() )
geompy.addToStudyInFather( f1, s1, s1.GetName() )

dim = 2 # == faces
values = [str(i) for i in geompy.SubShapeAllIDs( shape, geompy.ShapeType["FACE"] )]
f2 = geompy.CreateField( shape, "faces", fieldDataType, dim, componentNames )
s2 = f2.addStep( stepID, stamp, values )
geompy.addToStudyInFather( shape, f2, f2.GetName() )
geompy.addToStudyInFather( f2, s2, s2.GetName() )

dim = 3 # == solids
values = [str(i) for i in geompy.SubShapeAllIDs( shape, geompy.ShapeType["SOLID"] )]
f3 = geompy.CreateField( shape, "solids", fieldDataType, dim, componentNames )
s3 = f3.addStep( stepID, stamp, values )
geompy.addToStudyInFather( shape, f3, f3.GetName() )
geompy.addToStudyInFather( f3, s3, s3.GetName() )

dim = -1 # == whole shape
f_1 = geompy.CreateField( shape, "whole shape", fieldDataType, dim, componentNames )
s_1 = f_1.addStep(stepID, stamp, ["1"])
geompy.addToStudyInFather( shape, f_1, f_1.GetName() )
geompy.addToStudyInFather( f_1, s_1, s_1.GetName() )

# display
gg.createAndDisplayGO( s0id )
