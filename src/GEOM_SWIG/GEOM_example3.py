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
#  File   : GEOM_example3.py
#  Author : Paul RASCLE, EDF
#  Module : GEOM
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math
from time import sleep
import salome_ComponentGUI

gg = salome.ImportComponentGUI("GEOM")

BasicOp  = geompy.GetIBasicOperations(salome.myStudyId)
PrimOp   = geompy.GetI3DPrimOperations(salome.myStudyId)
InsertOp = geompy.GetIInsertOperations(salome.myStudyId)
TrsfOp   = geompy.GetITransformOperations(salome.myStudyId)

point0  = BasicOp.MakePointXYZ(0.,0.,0.)
pointz1 = BasicOp.MakePointXYZ(0.,0.,1.)
dirz = BasicOp.MakeVectorTwoPnt(point0,pointz1)

torus1 = PrimOp.MakeTorusPntVecRR(point0,dirz,150.,25.)
id_torus1 = geompy.addToStudy(torus1,"torus1")
if not isinstance(gg, type(salome_ComponentGUI)):
    gg.createAndDisplayGO(id_torus1)

torus2 = InsertOp.MakeCopy(torus1)

vec1 = BasicOp.MakeVectorDXDYDZ(0.,0.,100.)
torus2 = TrsfOp.TranslateVectorCopy(torus2,vec1)
id_torus2 = geompy.addToStudy(torus2,"torus2")
if not isinstance(gg, type(salome_ComponentGUI)):
    gg.createAndDisplayGO(id_torus2)

cylz1 = PrimOp.MakeCylinderPntVecRH(point0,dirz,25.,100.)

ind = 0
shapeList = []
idList = []
while ind < 6:
    acyl = InsertOp.MakeCopy(cylz1)
    x = 150. * math.cos(ind * math.pi/3.)
    y = 150. * math.sin(ind * math.pi/3.)
    z = 0.
    vec_i = BasicOp.MakeVectorDXDYDZ(x,y,z)
    name = "cyl%d"%(ind)
    acyl = TrsfOp.TranslateVectorCopy(acyl,vec_i)
    id_acyl = geompy.addToStudy(acyl,name)
    if not isinstance(gg, type(salome_ComponentGUI)):
        gg.createAndDisplayGO(id_acyl)
    shapeList.append(acyl)
    idList.append(id_acyl)
    ind = ind +1

shapeList.append(torus1)
shapeList.append(torus2)
idList.append(id_torus1)
idList.append(id_torus2)

cage = geompy.MakeCompound(shapeList)
id_cage = geompy.addToStudy(cage,"cage")
if not isinstance(gg, type(salome_ComponentGUI)):
    gg.createAndDisplayGO(id_cage)

from salome import sg
sleep(1)
sg.EraseAll()
for id in idList:
    sg.DisplayOnly(id)
    sleep(1)
sg.EraseAll()
for id in idList:
    sg.Display(id)
    sleep(1)
for id in idList:
    sg.Erase(id)
    sleep(1)

#sg.Display(id_cage)
sg.Display(id_torus1)
sg.Display(id_torus2)
sg.Display(id_acyl)
if not isinstance(gg, type(salome_ComponentGUI)):
    gg.setTransparency(id_torus1,0.5)
    
    gg.setDisplayMode(id_torus1,1)
    gg.setDisplayMode(id_torus2,1)
    gg.setVectorsMode(id_acyl,True)
    gg.setVerticesMode(id_acyl,True)
    #gg.setDisplayMode(id_cage,1)
    
    gg.setColor(id_torus1,0,0,255)
    gg.setColor(id_torus2,255,0,0)
    gg.setColor(id_acyl,0,255,0)
    #gg.setColor(id_cage,255,255,0)
