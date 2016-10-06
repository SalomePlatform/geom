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
#GUI test scenario      :PAL-MESH-033 (geometry part)
#####################################################################
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

#Points creation (2.2)
Vertex_1 = geompy.MakeVertex(0, 0, 0)
geompy.addToStudy(Vertex_1, "Vertex_1")

Vertex_2 = geompy.MakeVertex(0, 100, 0)
geompy.addToStudy(Vertex_2, "Vertex_2")

Vertex_3 = geompy.MakeVertex(100, 100, 0)
geompy.addToStudy(Vertex_3, "Vertex_3")

Vertex_4 = geompy.MakeVertex(100, 0, 0)
geompy.addToStudy(Vertex_4, "Vertex_4")

#Edges creation (2.18)
Edge_1 = geompy.MakeEdge(Vertex_1, Vertex_2)
geompy.addToStudy(Edge_1, "Edge_1")

Edge_2 = geompy.MakeEdge(Vertex_2, Vertex_3)
geompy.addToStudy(Edge_2, "Edge_2")

Edge_3 = geompy.MakeEdge(Vertex_3, Vertex_4)
geompy.addToStudy(Edge_3, "Edge_3")

Edge_4 = geompy.MakeEdge(Vertex_4, Vertex_1)
geompy.addToStudy(Edge_4, "Edge_4")

#Wire creation (2.33)
Wire_1 = geompy.MakeWire([Edge_1, Edge_2, Edge_3, Edge_4])
geompy.addToStudy(Wire_1, "Wire_1")

salome.sg.updateObjBrowser(True)
