# Creation of a PipeWithShellSections

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math
gg = salome.ImportComponentGUI("GEOM")

# create path
WirePath = geompy.MakeSketcher("Sketcher:F 0 0:TT 100 0:R 0:C 100 90:T 0 200", [0, 0, 0, 0, 0, 1, 1, 0, -0])

# get sub-shapes
edges = geompy.SubShapeAll(WirePath, geompy.ShapeType["EDGE"])
vertices = geompy.SubShapeAll(WirePath, geompy.ShapeType["VERTEX"])


#=======================================================
#                 Create shell sections
#=======================================================
ps = [vertices[0],vertices[1],vertices[2],vertices[3]]
theLocations = [vertices[0],vertices[1],vertices[2],vertices[3]]
VC = geompy.MakeCompound(theLocations)
geompy.addToStudy(VC,"VC")
vs = [edges[0],edges[0],edges[2],edges[2]]
hs = [20,40,30,20]
shells = []
subbases = []

# 1 section
c0 = geompy.PointCoordinates(ps[0])
c1 = geompy.PointCoordinates(ps[1])
nx = c1[0] - c0[0]
ny = c1[1] - c0[1]
nz = c1[2] - c0[2]

faces = []
f1 = geompy.MakeSketcher("Sketcher:F 0 0:TT 20 0:TT 20 20:TT 0 20:WF",
                         [c0[0], c0[1], c0[2], nx, ny, nz, 0, 0, 1])
f2 = geompy.MakeSketcher("Sketcher:F 0 0:TT 0 20:TT -20 20:TT -20 0:WF",
                         [c0[0], c0[1], c0[2], nx, ny, nz, 0, 0, 1])
f3 = geompy.MakeSketcher("Sketcher:F 0 0:TT -20 0:TT -20 -20:TT 0 -20:WF",
                         [c0[0], c0[1], c0[2], nx, ny, nz, 0, 0, 1])
f4 = geompy.MakeSketcher("Sketcher:F 0 0:TT 0 -20:TT 20 -20:TT 20 0:WF",
                         [c0[0], c0[1], c0[2], nx, ny, nz, 0, 0, 1])
faces.append(f1)
faces.append(f2)
faces.append(f3)
faces.append(f4)
shell = geompy.MakeSewing(faces,1.e-6)
shells.append(shell)
faces = geompy.SubShapeAllSortedCentres(shell, geompy.ShapeType["FACE"])
subbases.append(faces[0])

# 2 section
faces = []

w = geompy.MakeSketcher("Sketcher:F 20 20:TT 0 20:TT 0 0:TT 20 0",
                         [c1[0], c1[1], c1[2], nx, ny, nz, 0, 0, 1])
contour = geompy.CloseContour(w, [], isCommonVertex=False)
[e1,e2,e3,e4] = geompy.SubShapeAll(contour, geompy.ShapeType["EDGE"])
w = geompy.MakeWire([e1,e2,e3,e4])
f1 = geompy.MakeFace(w,1)

w = geompy.MakeSketcher("Sketcher:F -20 0:TT 0 0:TT 0 20:TT -20 20",
                         [c1[0], c1[1], c1[2], nx, ny, nz, 0, 0, 1])
contour = geompy.CloseContour(w, [], isCommonVertex=False)
[e1,e2,e3,e4] = geompy.SubShapeAll(contour, geompy.ShapeType["EDGE"])
w = geompy.MakeWire([e1,e2,e3,e4])
f2 = geompy.MakeFace(w,1)

w = geompy.MakeSketcher("Sketcher:F 20 0:TT 0 0:TT 0 -20:TT 20 -20",
                         [c1[0], c1[1], c1[2], nx, ny, nz, 0, 0, 1])
contour = geompy.CloseContour(w, [], isCommonVertex=False)
[e1,e2,e3,e4] = geompy.SubShapeAll(contour, geompy.ShapeType["EDGE"])
w = geompy.MakeWire([e1,e2,e3,e4])
f3 = geompy.MakeFace(w,1)

w = geompy.MakeSketcher("Sketcher:F -20 -20:TT 0 -20:TT 0 0:TT -20 0",
                         [c1[0], c1[1], c1[2], nx, ny, nz, 0, 0, 1])
contour = geompy.CloseContour(w, [], isCommonVertex=False)
[e1,e2,e3,e4] = geompy.SubShapeAll(contour, geompy.ShapeType["EDGE"])
w = geompy.MakeWire([e1,e2,e3,e4])
f4 = geompy.MakeFace(w,1)

faces.append(f1)
faces.append(f2)
faces.append(f3)
faces.append(f4)
shell = geompy.MakeSewing(faces,1.e-6)
shells.append(shell)
faces = geompy.SubShapeAllSortedCentres(shell, geompy.ShapeType["FACE"])
subbases.append(faces[0])

# 3 section
faces = []
c2 = geompy.PointCoordinates(ps[2])
c3 = geompy.PointCoordinates(ps[3])
nx = c3[0] - c2[0]
ny = c3[1] - c2[1]
nz = c3[2] - c2[2]

w = geompy.MakeSketcher("Sketcher:F 20 20:TT 0 20:TT 0 0:TT 20 0",
                         [c2[0], c2[1], c2[2], nx, ny, nz, 0, 0, 1])
contour = geompy.CloseContour(w, [], isCommonVertex=False)
[e1,e2,e3,e4] = geompy.SubShapeAll(contour, geompy.ShapeType["EDGE"])
w = geompy.MakeWire([e1,e2,e3,e4])
f1 = geompy.MakeFace(w,1)

w = geompy.MakeSketcher("Sketcher:F -20 0:TT 0 0:TT 0 20:TT -20 20",
                         [c2[0], c2[1], c2[2], nx, ny, nz, 0, 0, 1])
contour = geompy.CloseContour(w, [], isCommonVertex=False)
[e1,e2,e3,e4] = geompy.SubShapeAll(contour, geompy.ShapeType["EDGE"])
w = geompy.MakeWire([e1,e2,e3,e4])
f2 = geompy.MakeFace(w,1)

w = geompy.MakeSketcher("Sketcher:F 20 0:TT 0 0:TT 0 -20:TT 20 -20",
                         [c2[0], c2[1], c2[2], nx, ny, nz, 0, 0, 1])
contour = geompy.CloseContour(w, [], isCommonVertex=False)
[e1,e2,e3,e4] = geompy.SubShapeAll(contour, geompy.ShapeType["EDGE"])
w = geompy.MakeWire([e1,e2,e3,e4])
f3 = geompy.MakeFace(w,1)

w = geompy.MakeSketcher("Sketcher:F -20 -20:TT 0 -20:TT 0 0:TT -20 0",
                         [c2[0], c2[1], c2[2], nx, ny, nz, 0, 0, 1])
contour = geompy.CloseContour(w, [], isCommonVertex=False)
[e1,e2,e3,e4] = geompy.SubShapeAll(contour, geompy.ShapeType["EDGE"])
w = geompy.MakeWire([e1,e2,e3,e4])
f4 = geompy.MakeFace(w,1)

faces.append(f1)
faces.append(f2)
faces.append(f3)
faces.append(f4)
shell = geompy.MakeSewing(faces,1.e-6)
shells.append(shell)
faces = geompy.SubShapeAllSortedCentres(shell, geompy.ShapeType["FACE"])
subbases.append(faces[2])

# 4 section
faces = []

kk = 4
dx = c3[0] - nx/kk
dy = c3[1] - ny/kk
dz = c3[2] - nz/kk
rad = math.sqrt(nx*nx+ny*ny+nz*nz)
vc = geompy.MakeVertex(dx,dy,dz)
sph = geompy.MakeSpherePntR(vc,rad/kk)
shellsph = geompy.SubShapeAll(sph, geompy.ShapeType["SHELL"])

fs = []
vec = geompy.MakeVectorDXDYDZ(0,0,1)
ff = geompy.MakePlane(ps[3],vec,40)
fs.append(ff)
vp = geompy.MakeVertex(c3[0],c3[1],c3[2]+20)
ff = geompy.MakePlane(vp,vec,40)
fs.append(ff)
vp = geompy.MakeVertex(c3[0],c3[1],c3[2]-20)
ff = geompy.MakePlane(vp,vec,40)
fs.append(ff)
vec = geompy.MakeVectorDXDYDZ(1,0,0)
ff = geompy.MakePlane(ps[3],vec,40)
fs.append(ff)
vp = geompy.MakeVertex(c3[0]+20,c3[1],c3[2])
ff = geompy.MakePlane(vp,vec,40)
fs.append(ff)
vp = geompy.MakeVertex(c3[0]-20,c3[1],c3[2])
ff = geompy.MakePlane(vp,vec,40)
fs.append(ff)
aPartition = geompy.MakePartition(shellsph,fs)
fs = geompy.SubShapeAllSortedCentres(aPartition, geompy.ShapeType["FACE"])

for face in fs:
    if ( 4 == geompy.NbShapes(face, geompy.ShapeType["VERTEX"]) ):
        faces.append(face)
        pass
    pass
shell = geompy.MakeSewing(faces,1.e-6)
shells.append(shell)
faces = geompy.SubShapeAllSortedCentres(shell, geompy.ShapeType["FACE"])
subbases.append(faces[2])

#===========================================================
#                   Create Pipe
#===========================================================
Pipe = geompy.MakePipeWithShellSections(shells, subbases, theLocations, WirePath,
                                        theWithContact=0, theWithCorrection=0)

# add objects in the study
resc = geompy.MakeCompound(shells)
id_sec = geompy.addToStudy(resc,"sections")
id_wire = geompy.addToStudy(WirePath,"WirePath")
id_pipe = geompy.addToStudy(Pipe, "Pipe")

# display the wire(path), sections and the pipe
gg.createAndDisplayGO(id_wire)
gg.createAndDisplayGO(id_sec)
gg.createAndDisplayGO(id_pipe)
gg.setDisplayMode(id_pipe,1) 
