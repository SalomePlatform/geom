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
#  File   : GEOM_Partition4.py
#  Module : GEOM
# import couplex2_01_GEOM
# reload(couplex2_01_GEOM)
# Dimensions de alveolus
# Parall‰pipˆde rectangle de taille alDx, alDy, alDz
#
alDx   = 2.5
alDy   = 20.0
alDz   = 2.5
alSepx = 18.0

# Dimensions de backfill
# Parall‰pipˆde rectangle de taille baDx, baDy, baDz

baDx = 27.0
baDy = 4.8
baDz = 4.8

# Dimensions de geological medium

gmDx = baDx
gmDy = 49.6
gmDz = 100.0

# --

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# -- Construction de backfill

backA = geompy.MakeBox(0.0, 0.0, 0.0, baDx, baDy, baDz)
back = geompy.MakeTranslation(backA, -baDx/2, -baDy/2, -baDz/2)

# -- Construction de alveolus

import math

alveA = geompy.MakeBox(0.0, 0.0, 0.0, alDx, alDy, alDz)
alveB = geompy.MakeTranslation(alveA, -alDx/2, baDy/2, -alDz/2)
axis  = geompy.MakeVectorDXDYDZ(1.0, 0.0, 0.0)
alve1 = geompy.MakeRotation(alveB, axis, math.pi)
alve2 = geompy.MakeTranslation(alveB, +alSepx/2, 0.0, 0.0)
alve3 = geompy.MakeTranslation(alveB, -alSepx/2, 0.0, 0.0)
GOlist = []
GOlist.append(alve1)
GOlist.append(alve2)
GOlist.append(alve3)
alve = geompy.MakeCompound(GOlist)

# -- Construction de geological medium

geolA = geompy.MakeBox(0.0, 0.0, 0.0, gmDx, gmDy, gmDz)
geol = geompy.MakeTranslation(geolA, -gmDx/2, -gmDy/2, -gmDz/2)

geol = geompy.MakePartition([alve, geol, back])

subshapes = geompy.SubShapeAll(geol, geompy.ShapeType["SHAPE"])

GOlist = []
GOlist.append(subshapes[0])
GOlist.append(subshapes[1])
GOlist.append(subshapes[2])
alve = geompy.MakeCompound(GOlist)

geol = subshapes[3]
back = subshapes[4]

# --

geol = geompy.MakeCompound([geol, back, alve])

geompy.addToStudy(geol, "couplex2 2")
