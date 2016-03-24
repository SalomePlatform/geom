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
#  File   : GEOM_example2.py
#  Author : Paul RASCLE, EDF
#  Module : GEOM
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math

geom = salome.lcc.FindOrLoadComponent("FactoryServer", "GEOM")
myBuilder = salome.myStudy.NewBuilder()

BasicOp  = geom.GetIBasicOperations(salome.myStudyId)
PrimOp   = geom.GetI3DPrimOperations(salome.myStudyId)
InsertOp = geom.GetIInsertOperations(salome.myStudyId)
TrsfOp   = geom.GetITransformOperations(salome.myStudyId)

point0  = BasicOp.MakePointXYZ(0.,0.,0.)
pointz1 = BasicOp.MakePointXYZ(0.,0.,1.)
dirz = BasicOp.MakeVectorTwoPnt(point0,pointz1)

torus1 = PrimOp.MakeTorusPntVecRR(point0,dirz,150.,25.)
id_torus1 = geompy.addToStudy(torus1,"torus1")

torus2 = InsertOp.MakeCopy(torus1)

vec1 = BasicOp.MakeVectorDXDYDZ(0.,0.,100.)
torus2 = TrsfOp.TranslateVectorCopy(torus2,vec1)
id_torus2 = geompy.addToStudy(torus2,"torus2")

cylz1 = PrimOp.MakeCylinderPntVecRH(point0,dirz,25.,100.)

ind = 0
cyllist = []
while ind < 6:
    acyl = InsertOp.MakeCopy(cylz1)
    x = 150. * math.cos(ind * math.pi/3.)
    y = 150. * math.sin(ind * math.pi/3.)
    z = 0.
    vec_i = BasicOp.MakeVectorDXDYDZ(x,y,z)
    name = "cyl%d"%(ind)
    acyl = TrsfOp.TranslateVectorCopy(acyl,vec_i)
    id_acyl = geompy.addToStudy(acyl,name)
    cyllist.append(acyl)
    ind = ind + 1
