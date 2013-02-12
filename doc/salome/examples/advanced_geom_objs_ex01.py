# Creation of PipeTShape

import geompy
import salome
gg = salome.ImportComponentGUI("GEOM")

# create PipeTShape object
pipetshape = geompy.MakePipeTShape(80.0, 20.0, 200.0, 50.0, 20.0, 200.0)

# add object in the study
id_pipetshape = geompy.addToStudy(pipetshape[0],"PipeTShape")
# add groups in the study
for g in pipetshape[1:]:
    geompy.addToStudyInFather(pipetshape[0], g, g.GetName())
    
# Create junction vertices
P1 = geompy.MakeVertex(0.0, 0.0, 0.0)
P2 = geompy.MakeVertex(400.0, 0.0, 0.0)
P3 = geompy.MakeVertex(200.0, 0.0, 200.0)

# create PipeTShape object with position
pipetshape_position = geompy.MakePipeTShape(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, True, P1, P2, P3)

# add object in the study
id_pipetshape_position = geompy.addToStudy(pipetshape_position[0],"PipeTShape_position")
# add groups in the study
for g in pipetshape_position[1:]:
    geompy.addToStudyInFather(pipetshape_position[0], g, g.GetName())

# create PipeTShape with chamfer object
pipetshapechamfer = geompy.MakePipeTShapeChamfer(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 20.0, 20.0)

# add object in the study
id_pipetshapechamfer = geompy.addToStudy(pipetshapechamfer[0],"PipeTShapeChamfer")
# add groups in the study
for g in pipetshapechamfer[1:]:
    geompy.addToStudyInFather(pipetshapechamfer[0], g, g.GetName())

# create PipeTShape with chamfer object with position
pipetshapechamfer_position = geompy.MakePipeTShapeChamfer(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 20.0, 20.0, True, P1, P2, P3)

# add object in the study
id_pipetshapechamfer_position = geompy.addToStudy(pipetshapechamfer_position[0],"PipeTShapeChamfer_position")
# add groups in the study
for g in pipetshapechamfer_position[1:]:
    geompy.addToStudyInFather(pipetshapechamfer_position[0], g, g.GetName())

# create PipeTShape with fillet object
pipetshapefillet = geompy.MakePipeTShapeFillet(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 5.0)

# add object in the study
id_pipetshapefillet = geompy.addToStudy(pipetshapefillet[0],"PipeTShapeFillet")
# add groups in the study
for g in pipetshapefillet[1:]:
    geompy.addToStudyInFather(pipetshapefillet[0], g, g.GetName())

# create PipeTShape with fillet object with position
pipetshapefillet_position = geompy.MakePipeTShapeFillet(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 5.0, True, P1, P2, P3)

# add object in the study
id_pipetshapefillet_position = geompy.addToStudy(pipetshapefillet_position[0],"PipeTShapeFillet_position")
# add groups in the study
for g in pipetshapefillet_position[1:]:
    geompy.addToStudyInFather(pipetshapefillet_position[0], g, g.GetName())
    

# display pipetshapes
gg.createAndDisplayGO(id_pipetshape)
gg.createAndDisplayGO(id_pipetshape_position)
gg.createAndDisplayGO(id_pipetshapechamfer)
gg.createAndDisplayGO(id_pipetshapechamfer_position)
gg.createAndDisplayGO(id_pipetshapefillet)
gg.createAndDisplayGO(id_pipetshapefillet_position)
