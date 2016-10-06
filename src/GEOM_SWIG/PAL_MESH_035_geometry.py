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
#Created                :15/02/2005
#Auhtor                 :KOVALTCHUK Alexey 
#GUI test scenario      :PAL-MESH-035 (geometry part)
#####################################################################
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

#Box creation (2.2)
Box_1 = geompy.MakeBoxDXDYDZ(200, 400, 300)
geompy.addToStudy(Box_1, "Box_1")

#Cylinder creation (2.8)
Cylinder_1 = geompy.MakeCylinderRH(100, 300)
geompy.addToStudy(Cylinder_1, "Cylinder_1")

#Cone creation (2.13)
Cone_1 = geompy.MakeConeR1R2H(100, 50, 200)
geompy.addToStudy(Cone_1, "Cone_1")

#Explode box, cone and cylinder on faces and vertices(2.18)
ListOfFaces_Box_1 = geompy.SubShapeAll(Box_1, geompy.ShapeType["FACE"])
NbExplodedFaces_Box_1 = len(ListOfFaces_Box_1)
for i in range (0, NbExplodedFaces_Box_1):
   name = "Face_" + str(i+1)
   geompy.addToStudyInFather(Box_1, ListOfFaces_Box_1[i], name)

ListOfVertices_Box_1 = geompy.SubShapeAll(Box_1, geompy.ShapeType["VERTEX"])
NbExplodedVertices_Box_1 = len(ListOfVertices_Box_1)
for i in range (0, NbExplodedVertices_Box_1):
   name = "Vertex_" + str(i+1)
   geompy.addToStudyInFather(Box_1, ListOfVertices_Box_1[i], name)
   
ListOfFaces_Cylinder_1 = geompy.SubShapeAll(Cylinder_1, geompy.ShapeType["FACE"])
NbExplodedFaces_Cylinder_1 = len(ListOfFaces_Cylinder_1)
for i in range (0, NbExplodedFaces_Cylinder_1):
   name = "Face_" + str(NbExplodedFaces_Box_1+i+1)
   geompy.addToStudyInFather(Cylinder_1, ListOfFaces_Cylinder_1[i], name)
   
ListOfFaces_Cone_1 = geompy.SubShapeAll(Cone_1, geompy.ShapeType["FACE"])
NbExplodedFaces_Cone_1 = len(ListOfFaces_Cone_1)
for i in range (0, NbExplodedFaces_Cone_1):
   name = "Face_" + str(NbExplodedFaces_Box_1+NbExplodedFaces_Cylinder_1+i+1)
   geompy.addToStudyInFather(Cone_1, ListOfFaces_Cone_1[i], name)
   
#Plane creation (2.32)
Plane_1 = geompy.MakePlaneThreePnt(ListOfVertices_Box_1[0], ListOfVertices_Box_1[1], ListOfVertices_Box_1[3], 600)
geompy.addToStudy(Plane_1, "Plane_1")

#Partition (2.32)
compound = geompy.MakeCompound([ListOfFaces_Cylinder_1[0], ListOfFaces_Cone_1[0]])
Partition_1 = geompy.MakeHalfPartition(compound, Plane_1)
geompy.addToStudy(Partition_1, "Partition_1")

#Explode partition on faces and vertices(2.38)
ListOfFaces_Partition_1 = geompy.SubShapeAll(Partition_1, geompy.ShapeType["FACE"])
NbExplodedFaces_Partition_1 = len(ListOfFaces_Partition_1)
for i in range (0, NbExplodedFaces_Partition_1):
   name = "Face_" + str(NbExplodedFaces_Box_1+NbExplodedFaces_Cylinder_1+NbExplodedFaces_Cone_1+i+1)
   geompy.addToStudyInFather(Partition_1, ListOfFaces_Partition_1[i], name)

#Explode faces on vertices(2.43)
ListOfVertices_Face_7 = geompy.SubShapeAll(ListOfFaces_Cylinder_1[0], geompy.ShapeType["VERTEX"])
NbExplodedVertices_Face_7 = len(ListOfVertices_Face_7)
for i in range (0, NbExplodedVertices_Face_7):
   name = "Vertex_" + str(NbExplodedVertices_Box_1+i+1)
   geompy.addToStudyInFather(ListOfFaces_Cylinder_1[0], ListOfVertices_Face_7[i], name)

ListOfVertices_Face_10 = geompy.SubShapeAll(ListOfFaces_Cone_1[0], geompy.ShapeType["VERTEX"])
NbExplodedVertices_Face_10 = len(ListOfVertices_Face_10)
for i in range (0, NbExplodedVertices_Face_10):
   name = "Vertex_" + str(NbExplodedVertices_Box_1+NbExplodedVertices_Face_7+i+1)
   geompy.addToStudyInFather(ListOfFaces_Cone_1[0], ListOfVertices_Face_10[i], name)

ListOfVertices_Face_15 = geompy.SubShapeAll(ListOfFaces_Partition_1[2], geompy.ShapeType["VERTEX"])
NbExplodedVertices_Face_15 = len(ListOfVertices_Face_15)
for i in range (0, NbExplodedVertices_Face_15):
   name = "Vertex_" + str(NbExplodedVertices_Box_1+NbExplodedVertices_Face_7+NbExplodedVertices_Face_10+i+1)
   geompy.addToStudyInFather(ListOfFaces_Partition_1[2], ListOfVertices_Face_15[i], name)

ListOfVertices_Face_18 = geompy.SubShapeAll(ListOfFaces_Partition_1[NbExplodedFaces_Partition_1-1], geompy.ShapeType["VERTEX"])
NbExplodedVertices_Face_18 = len(ListOfVertices_Face_18)
for i in range (0, NbExplodedVertices_Face_18):
   name = "Vertex_" + str(NbExplodedVertices_Box_1+NbExplodedVertices_Face_7+NbExplodedVertices_Face_10+NbExplodedVertices_Face_15+i+1)
   geompy.addToStudyInFather(ListOfFaces_Partition_1[NbExplodedFaces_Partition_1-1], ListOfVertices_Face_18[i], name)

salome.sg.updateObjBrowser(True)
