# Creation of PipeTShape

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
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

# create PipeTShape object with left thickness reduction
pipetshape_thr = geompy.MakePipeTShape(80.0, 20.0, 200.0, 50.0, 20.0, 200.0,
                                       theRL=60, theWL=20, theLtransL=40, theLthinL=20)

# add object in the study
id_pipetshape_thr = geompy.addToStudy(pipetshape_thr[0],"PipeTShape_left_thickness_reduction")
# add groups in the study
for g in pipetshape_thr[1:]:
    geompy.addToStudyInFather(pipetshape_thr[0], g, g.GetName())

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

# create PipeTShape with chamfer object with left and right thickness reductions
pipetshapechamfer_thr = geompy.MakePipeTShapeChamfer(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 20.0, 20.0,
                                                     theRL=60, theWL=20, theLtransL=40, theLthinL=20,
                                                     theRR=40, theWR=10, theLtransR=60, theLthinR=30)

# add object in the study
id_pipetshapechamfer_thr = geompy.addToStudy(pipetshapechamfer_thr[0],"PipeTShapeChamfer_two_thickness_reductions")
# add groups in the study
for g in pipetshapechamfer_thr[1:]:
    geompy.addToStudyInFather(pipetshapechamfer_thr[0], g, g.GetName())

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

# create PipeTShape with fillet object with three thickness reductions
pipetshapefillet_thr = geompy.MakePipeTShapeFillet(80.0, 20.0, 200.0, 50.0, 20.0, 200.0,
                                                   5.0, True, None, None, None,
                                                   60, 20, 40, 20,
                                                   40, 10, 60, 30,
                                                   20, 10, 30, 30)

# add object in the study
id_pipetshapefillet_thr = geompy.addToStudy(pipetshapefillet_thr[0],"PipeTShapeFillet_three_thickness_reductions")
# add groups in the study
for g in pipetshapefillet_thr[1:]:
    geompy.addToStudyInFather(pipetshapefillet_thr[0], g, g.GetName())


# display pipetshapes
gg.createAndDisplayGO(id_pipetshape)
gg.createAndDisplayGO(id_pipetshape_position)
gg.createAndDisplayGO(id_pipetshape_thr)
gg.createAndDisplayGO(id_pipetshapechamfer)
gg.createAndDisplayGO(id_pipetshapechamfer_position)
gg.createAndDisplayGO(id_pipetshapechamfer_thr)
gg.createAndDisplayGO(id_pipetshapefillet)
gg.createAndDisplayGO(id_pipetshapefillet_position)
gg.createAndDisplayGO(id_pipetshapefillet_thr)
