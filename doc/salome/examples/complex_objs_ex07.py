# Creation of a PipeShellsWithoutPath

import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()
import salome
gg = salome.ImportComponentGUI("GEOM")

# Complex section
# result - 16 quads from lines
# pnt - point from path
# vec - direction from path
def MakeComplexSect(pnt, vec, rmax, rmin, nb, geom_builder):
    dang = 1.0/nb/2
    cmax = geom_builder.MakeCircle(pnt,vec,rmax)
    cmin = geom_builder.MakeCircle(pnt,vec,rmin)
    faces = []
    for i in range(0,2*nb,2):
        p1 = geom_builder.MakeVertexOnCurve(cmin,dang*i)
        p2 = geom_builder.MakeVertexOnCurve(cmax,dang*(i+1))
        p3 = geom_builder.MakeVertexOnCurve(cmin,dang*(i+2))
        f = geom_builder.MakeQuad4Vertices(pnt,p1,p2,p3)
        faces.append(f)
        pass
    shell = geom_builder.MakeSewing(faces,1.e-6)
    return shell


#=======================================================
#       Create simple path and receive points
#              for section creation
#=======================================================
WirePath = geompy.MakeSketcher("Sketcher:F 0 0:T 60 0:T 40 0:R 0:C 100 90:",
                               [0, 0, 0, 0, 0, 1, 1, 0, 0])
vs = geompy.SubShapeAll(WirePath, geompy.ShapeType["VERTEX"])

#=======================================================
#                 Create shell sections
#=======================================================
shells = []
subbases = []
locs = []

# 1 section
shell = MakeComplexSect(vs[0], geompy.MakeVectorDXDYDZ(1,0,0), 60, 40, 16, geom_builder=geompy)
shells.append(shell)
vs1 = geompy.SubShapeAllSortedCentres(shell,geompy.ShapeType["VERTEX"])
locs.append(vs1[17])

# 2 section
shell = MakeComplexSect(vs[1], geompy.MakeVectorDXDYDZ(1,0,0), 80, 30, 16, geom_builder=geompy)
shells.append(shell)
vs2 = geompy.SubShapeAllSortedCentres(shell,geompy.ShapeType["VERTEX"])
locs.append(vs2[17])

# 3 section
shell = MakeComplexSect(vs[2], geompy.MakeVectorDXDYDZ(1,0,0), 60, 40, 16, geom_builder=geompy)
shells.append(shell)
vs3 = geompy.SubShapeAllSortedCentres(shell,geompy.ShapeType["VERTEX"])
locs.append(vs3[17])

# 4 section
shell = MakeComplexSect(vs[3], geompy.MakeVectorDXDYDZ(0,1,0), 40, 35, 16, geom_builder=geompy)
shells.append(shell)
vs4 = geompy.SubShapeAllSortedCentres(shell,geompy.ShapeType["VERTEX"])
locs.append(vs4[17])


#===========================================================
#                   Create Pipe
#===========================================================

Pipe = geompy.MakePipeShellsWithoutPath(shells,locs)

# add objects in the study
resc = geompy.MakeCompound(shells)
id_sec = geompy.addToStudy(resc,"sections")
resl = geompy.MakeCompound(locs)
id_loc = geompy.addToStudy(resl,"locations")
id_pipe = geompy.addToStudy(Pipe, "Pipe")

# display the sections, locations and pipe
gg.createAndDisplayGO(id_sec)
gg.createAndDisplayGO(id_loc)
gg.createAndDisplayGO(id_pipe)
gg.setDisplayMode(id_pipe,1) 
