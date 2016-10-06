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

#####################################################################################
#Created                :17/02/2005
#Auhtor                 :KOVALTCHUK Alexey 
#GUI test scenario      :PAL-MESH-019 (geometry part), PAL-MESH-020 (geometry part)
#####################################################################################
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

#Points creation (2.2)
Vertex_1 = geompy.MakeVertex(0, 0, 0)
geompy.addToStudy(Vertex_1, "Vertex_1")

Vertex_2 = geompy.MakeVertex(20, 0, 0)
geompy.addToStudy(Vertex_2, "Vertex_2")

Vertex_3 = geompy.MakeVertex(10, 10, 0)
geompy.addToStudy(Vertex_3, "Vertex_3")

Vertex_4 = geompy.MakeVertex(0, 100, 0)
geompy.addToStudy(Vertex_4, "Vertex_4")

Vertex_5 = geompy.MakeVertex(0, 80, 20)
geompy.addToStudy(Vertex_5, "Vertex_5")

#Curve creation (2.21)
Curve_1 = geompy.MakeInterpol([Vertex_1, Vertex_3, Vertex_2])
geompy.addToStudy(Curve_1, "Curve_1")

#Line creation (2.29)
Line_1 = geompy.MakeLineTwoPnt(Vertex_4, Vertex_5)
geompy.addToStudy(Line_1, "Line_1")

salome.sg.updateObjBrowser(True)
