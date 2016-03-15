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

#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#  File   : GEOM_example5.py
#  Author : Damien COQUERET, Open CASCADE
#  Module : GEOM
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

#Create Points
BasicOp = geompy.GetIBasicOperations(salome.myStudyId)
Vertex1 = BasicOp.MakePointXYZ(0., 0., 0.)
Vertex2 = BasicOp.MakePointXYZ(100., 20., 30.)
Vertex3 = BasicOp.MakePointXYZ(150., 100., -25.)
Vertex4 = BasicOp.MakePointXYZ(40., -80., 79.)

#GEOM_Objects List for Splines
GOList = []
GOList.append(Vertex1)
GOList.append(Vertex2)
GOList.append(Vertex3)
GOList.append(Vertex4)

#Create Splines
Bezier   = geompy.MakeBezier(GOList)
Interpol = geompy.MakeInterpol(GOList)

#Add In Study
id_Bezier   = geompy.addToStudy(Bezier,   "Bezier")
id_Interpol = geompy.addToStudy(Interpol, "Interpol")
