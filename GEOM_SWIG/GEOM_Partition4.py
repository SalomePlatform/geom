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

backA = geompy.MakeBox(0.0,0.0,0.0,baDx,baDy,baDz)
back = geompy.MakeTranslation(backA,-baDx/2,-baDy/2,-baDz/2)

# -- Construction de alveolus

import math

alveA = geompy.MakeBox(0.0,0.0,0.0,alDx,alDy,alDz)
alveB = geompy.MakeTranslation(alveA,-alDx/2,baDy/2,-alDz/2)
axis  = geompy.geom.MakeAxisStruct(0.0,0.0,0.0,1.0,0.0,0.0)
alve1 = geompy.MakeRotation(alveB,axis,math.pi)
alve2 = geompy.MakeTranslation(alveB,+alSepx/2,0.0,0.0)
alve3 = geompy.MakeTranslation(alveB,-alSepx/2,0.0,0.0)
IORlist = []
IORlist.append(alve1._get_Name())
IORlist.append(alve2._get_Name())
IORlist.append(alve3._get_Name())
alve = geompy.MakeCompound(IORlist)

# -- Construction de geological medium

geolA = geompy.MakeBox(0.0,0.0,0.0,gmDx,gmDy,gmDz)
geol = geompy.MakeTranslation(geolA,-gmDx/2,-gmDy/2,-gmDz/2)

geol = geompy.Partition(
    [alve._get_Name(), geol._get_Name(), back._get_Name()])

subshapes = geompy.SubShapeAll( geol, geompy.ShapeType["SHAPE"] )

IORlist = []
IORlist.append(subshapes[0]._get_Name())
IORlist.append(subshapes[1]._get_Name())
IORlist.append(subshapes[2]._get_Name())
alve = geompy.MakeCompound(IORlist)

geol = subshapes[3]
back = subshapes[4]

# --

geol = geompy.MakeCompound(
    [geol._get_Name(), back._get_Name(), alve._get_Name()])

geompy.addToStudy(geol,"couplex2 2")


