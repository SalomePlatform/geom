#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

#####################################################################
#Created                :17/02/2005
#Auhtor                 :KOVALTCHUK Alexey 
#GUI test scenario      :PAL-MESH-028 (geometry part)
#####################################################################
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

import os

#Initialization

#Points construction (2.1)
Vertices = [geompy.MakeVertex(0, 0, 0), geompy.MakeVertex(200, 0, 0), geompy.MakeVertex(200, 200, 0), geompy.MakeVertex(0, 200, 0), geompy.MakeVertex(50, 50, 200), geompy.MakeVertex(150, 50, 200), geompy.MakeVertex(150, 150, 200), geompy.MakeVertex(50, 150, 200)]
for i in range(0, len(Vertices)):
    name = "Vertex_" + str(i+1)
    geompy.addToStudy(Vertices[i], name)

#Edges construction (3.1)
Edges = [geompy.MakeEdge(Vertices[0], Vertices[1]), geompy.MakeEdge(Vertices[1], Vertices[5]), geompy.MakeEdge(Vertices[5], Vertices[4]), geompy.MakeEdge(Vertices[4], Vertices[0]), geompy.MakeEdge(Vertices[1], Vertices[2]), geompy.MakeEdge(Vertices[2], Vertices[6]), geompy.MakeEdge(Vertices[6], Vertices[5]), geompy.MakeEdge(Vertices[2], Vertices[3]), geompy.MakeEdge(Vertices[3], Vertices[7]), geompy.MakeEdge(Vertices[7], Vertices[6]), geompy.MakeEdge(Vertices[3], Vertices[0]), geompy.MakeEdge(Vertices[4], Vertices[7])]
for i in range(0, len(Edges)):
    name = "Edge_" + str(i+1)
    geompy.addToStudy(Edges[i], name)

#Wires construction (4.1)
Wires = [geompy.MakeWire([Edges[0], Edges[1], Edges[2], Edges[3]]), geompy.MakeWire([Edges[1], Edges[4], Edges[5], Edges[6]]), geompy.MakeWire([Edges[5], Edges[7], Edges[8], Edges[9]]), geompy.MakeWire([Edges[3], Edges[10], Edges[8], Edges[11]]), geompy.MakeWire([Edges[2], Edges[6], Edges[9], Edges[11]]), geompy.MakeWire([Edges[0], Edges[4], Edges[7], Edges[10]])]
for i in range(0, len(Wires)):
    name = "Wire_" + str(i+1)
    geompy.addToStudy(Wires[i], name)

#Faces construction (5.1)
Faces = [geompy.MakeFace(Wires[0], 1), geompy.MakeFace(Wires[1], 1), geompy.MakeFace(Wires[2], 1), geompy.MakeFace(Wires[3], 1), geompy.MakeFace(Wires[4], 1), geompy.MakeFace(Wires[5], 1)]
for i in range(0, len(Faces)):
    name = "Face_" + str(i+1)
    geompy.addToStudy(Faces[i], name)

#Shell construction (6.1)
Shell_1 = geompy.MakeShell(Faces)
geompy.addToStudy(Shell_1, "Shell_1")
Shells = [Shell_1]

#Solid construction (7.1)
Solid_1 = geompy.MakeSolid(Shells)
geompy.addToStudy(Solid_1, "Solid_1")

#Prism construction (8.1)
Prism_1 = geompy.MakePrismVecH(Faces[4], Edges[1], 100)
geompy.addToStudy(Prism_1, "Prism_1")

#Save Study (9.1)
theHomePath = os.getenv("HOME")
theFilenameToSave = theHomePath + "/Pyramid.hdf"
if os.access(theFilenameToSave, os.F_OK):
    if os.access(theFilenameToSave, os.W_OK):
        os.remove(theFilenameToSave)
        salome.myStudyManager.SaveAs(theFilenameToSave, salome.myStudy, 0)
    else:
        print "You have no enough permissions to overwrite HDF file: ",theFilenameToSave
else:
    salome.myStudyManager.SaveAs(theFilenameToSave, salome.myStudy, 0)
    

salome.sg.updateObjBrowser(True)
