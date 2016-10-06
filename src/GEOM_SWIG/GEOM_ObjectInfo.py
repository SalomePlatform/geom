#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
#  File   : GEOM_ObjectInfo.py
#  Author : Michael ZORIN
#  Module : GEOM
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# Create several objects
obj1 = geompy.MakeVertex(0.,0.,0.)
obj2 = geompy.MakeCircleR(150.)
obj3 = geompy.MakeBoxDXDYDZ(300.,400.,500.)
obj4 = geompy.MakeCylinderRH(100.,200.)
ShapeListCompound = []
ShapeListCompound.append(obj3)
ShapeListCompound.append(obj4)
obj5 = geompy.MakeCompound(ShapeListCompound)

obj1_entry = geompy.addToStudy(obj1, "Object1")
obj2_entry = geompy.addToStudy(obj2, "Object2")
obj3_entry = geompy.addToStudy(obj3, "Object3")
obj4_entry = geompy.addToStudy(obj4, "Object4")
obj5_entry = geompy.addToStudy(obj5, "Object5")

# Get information about objects
hasInfo = geompy.hasObjectInfo()
print "Check if GEOM module provides information about its objects: ", hasInfo
if hasInfo == True:
    print "Information about first  object: ", geompy.getObjectInfo(salome.myStudyId, obj1_entry)
    print "Information about second object: ", geompy.getObjectInfo(salome.myStudyId, obj2_entry)
    print "Information about third  object: ", geompy.getObjectInfo(salome.myStudyId, obj3_entry)
    print "Information about fourth object: ", geompy.getObjectInfo(salome.myStudyId, obj4_entry)
    print "Information about fifth  object: ", geompy.getObjectInfo(salome.myStudyId, obj5_entry)

salome.sg.updateObjBrowser(True)
