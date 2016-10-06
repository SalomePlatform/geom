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

#==============================================================================
#  Created              : 17/02/2005
#  Author               : Alexander KOVALEV
#  GUI test scenario    : PAL-MESH-030_geometry
#============================================================================== 
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# Stage 2: "Creation of geometry"

# (2.1) Creating a box
Box_1 = geompy.MakeBoxDXDYDZ(200, 400, 300)
# (2.8) Exploding Box_1 to vertices and faces
Box_1_vertices = geompy.SubShapeAll(Box_1, geompy.ShapeType["VERTEX"])
Box_1_faces = geompy.SubShapeAll(Box_1, geompy.ShapeType["FACE"])
# (2.17) Creating a new points
Vertex_9 = geompy.MakeVertex(200, 200, 50)
Vertex_10 = geompy.MakeVertex(150, 300, 80)
# (2.26) Translation of the faces 
Face_translated = []
Face_translated.append(geompy.MakeTranslationTwoPoints(Box_1_faces[2], Box_1_vertices[5], Vertex_9))
Face_translated.append(geompy.MakeTranslationTwoPoints(Box_1_faces[5], Vertex_9, Vertex_10))
Face_translated.append(geompy.MakeTranslationTwoPoints(Box_1_faces[2], Vertex_10, Vertex_9))
Face_translated.append(geompy.MakeTranslationTwoPoints(Box_1_faces[3], Box_1_vertices[5], Vertex_9))
Face_translated.append(geompy.MakeTranslationTwoPoints(Face_translated[1], Vertex_9, Vertex_10))
# (2.49) Building a Compound_1
ShapeListCompound_1 = []
i = 0
while i < 5 :
    ShapeListCompound_1.append(Face_translated[i])
    i = i + 1
Compound_1 = geompy.MakeCompound(ShapeListCompound_1)
# (2.54) Exploding of Compound_1 to faces
Compound_1_faces = geompy.SubShapeAll(Compound_1, geompy.ShapeType["FACE"])
# (2.54) Exploding of Face_translated_2 to vertices
Face_translated_2_vertices = geompy.SubShapeAll(Face_translated[1], geompy.ShapeType["VERTEX"])
# (2.62) Creating two boxes
Box_1 = geompy.MakeBoxTwoPnt(Vertex_9, Face_translated_2_vertices[1])
Box_2 = geompy.MakeBoxTwoPnt(Vertex_10, Face_translated_2_vertices[0])
# (2.70) Building Compound_2
ShapeListCompound_2 = []
ShapeListCompound_2.append(Box_1)
ShapeListCompound_2.append(Box_2)
Compound_2 = geompy.MakeCompound(ShapeListCompound_2)
# (2.75) Exploding of Compound_2 to solids
Compound_2_solids = geompy.SubShapeAll(Compound_2, geompy.ShapeType["SOLID"])

# Adding to study of geometrical objects
id_Compound_1 = geompy.addToStudy(Compound_1, "Compound_1")
id_Face_1 = geompy.addToStudyInFather(Compound_1,Compound_1_faces[0], "Face_1")
id_Face_5 = geompy.addToStudyInFather(Compound_1,Compound_1_faces[4], "Face_5")
id_Face_4 = geompy.addToStudyInFather(Compound_1,Compound_1_faces[3], "Face_4")
id_Compound_2 = geompy.addToStudy(Compound_2, "Compound_2")
id_Solid_2 = geompy.addToStudyInFather(Compound_2,Compound_2_solids[1], "Solid_2")

salome.sg.updateObjBrowser(True)
