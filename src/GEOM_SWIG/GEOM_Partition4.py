#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#
#  Copyright (C) 2003  CEA/DEN, EDF R&D
#
#
#
#  File   : GEOM_Partition4.py
#  Module : GEOM

#%Make geometry (like CEA script (A3)) using Partition algorithm%
# import couplex2_01_GEOM
# reload(couplex2_01_GEOM)

# Dimensions de alveolus
# Parall‰pipˆde rectangle de taille alDx, alDy, alDz

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

import geompy
geom = geompy.geom

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
