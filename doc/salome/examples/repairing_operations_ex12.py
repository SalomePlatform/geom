# Remove internal(shared) faces from a compound

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# create solids with some coincident faces
Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
Translation_1 = geompy.MakeTranslation(Box_1, 200, 0, 0)
Translation_2 = geompy.MakeTranslation(Box_1, 200, 200, 0)

geompy.addToStudy(Box_1, 'Box_1')
geompy.addToStudy(Translation_1, 'Translation_1')
geompy.addToStudy(Translation_2, 'Translation_2')

# partition three solids to obtain shared faces
Partition_1 = geompy.MakePartition([Box_1, Translation_1, Translation_2])
geompy.addToStudy(Partition_1, 'Partition_1')

# remove shared faces to obtain one solid instead of three
Joined_1 = geompy.RemoveInternalFaces(Partition_1)
geompy.addToStudy(Joined_1, 'Joined_1')

salome.sg.updateObjBrowser(True) 
