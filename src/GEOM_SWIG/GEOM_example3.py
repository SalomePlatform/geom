#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
# 
#  This library is free software; you can redistribute it and/or 
#  modify it under the terms of the GNU Lesser General Public 
#  License as published by the Free Software Foundation; either 
#  version 2.1 of the License. 
# 
#  This library is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
#  Lesser General Public License for more details. 
# 
#  You should have received a copy of the GNU Lesser General Public 
#  License along with this library; if not, write to the Free Software 
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
# 
#  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
#
#
#
#  File   : GEOM_example3.py
#  Author : Paul RASCLE, EDF
#  Module : GEOM
#  $Header$

import salome
import geompy
import math
from time import sleep

geom = salome.lcc.FindOrLoadComponent("FactoryServer", "Geometry")
myBuilder = salome.myStudy.NewBuilder()

gg = salome.ImportComponentGUI("Geometry")

point0  = geom.MakePointStruct(0.,0.,0.)
pointz1 = geom.MakePointStruct(0.,0.,1.)
dirz = geom.MakeDirection(pointz1)

torus1 = geompy.MakeTorus(point0,dirz,150.,25.)
id_torus1 = geompy.addToStudy(torus1,"torus1")
gg.createAndDisplayGO(id_torus1)

torus2 = geom.MakeCopy(torus1)
torus2 = geom.MakeTranslation(torus2,0.,0.,100.)
id_torus2 = geompy.addToStudy(torus2,"torus2")
gg.createAndDisplayGO(id_torus2)

cylz1 = geompy.MakeCylinder(point0,dirz,25.,100.)

ind = 0
shapeList = []
idList = []
while ind < 6:
    acyl = geom.MakeCopy(cylz1)
    x = 150. * math.cos(ind * math.pi/3.)
    y = 150. * math.sin(ind * math.pi/3.)
    z = 0.
    name = "cyl%d"%(ind)
    acyl = geompy.MakeTranslation(acyl,x,y,z)
    id_acyl = geompy.addToStudy(acyl,name)
    gg.createAndDisplayGO(id_acyl)
    shapeList.append(acyl)
    idList.append(id_acyl)
    ind = ind +1

shapeList.append(torus1)
shapeList.append(torus2)
idList.append(id_torus1)
idList.append(id_torus2)

iorStringList = []
for shape in shapeList:
    iorStringList.append(shape._get_Name())

cage = geompy.MakeCompound(iorStringList)
id_cage = geompy.addToStudy(cage,"cage")
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

gg.setTransparency(id_torus1,0.5)

gg.setDisplayMode(id_torus1,1)
gg.setDisplayMode(id_torus2,1)
gg.setDisplayMode(id_acyl,1)
#gg.setDisplayMode(id_cage,1)

gg.setColor(id_torus1,0,0,255)
gg.setColor(id_torus2,255,0,0)
gg.setColor(id_acyl,0,255,0)
#gg.setColor(id_cage,255,255,0)