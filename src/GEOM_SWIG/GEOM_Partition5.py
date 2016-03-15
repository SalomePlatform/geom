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
#  File   : GEOM_Partition5.py
#  Module : GEOM
# import gallery_01_GEOM
# reload(gallery_01_GEOM)
# -- Import geompy pour piloter GEOM par script
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# -- Dimensions de la boite entiˆre

LX, LY, LZ = 300.0, 150.0, 150.0
p0 = geompy.MakeVertex(0.,0.,0.)
vy = geompy.MakeVectorDXDYDZ(0.,1.,0.)

# -- D‰finition du plan de sym‰trie (O,Ox,Oz)

symPlane = geompy.MakePlane(p0, vy, 10.0)

# -- bo

bo = geompy.MakeBoxDXDYDZ(LX, LY, LZ)
bo = geompy.MakeTranslation(bo, 0.0, -LY/2, -LZ/2)

# -- Galerie principale

gal_diam = 80.0
gal_lony = LY
gal_x = LX-20.0-gal_diam/2

gal = geompy.MakeCylinder(p0, vy, gal_diam/2, gal_lony)
gal = geompy.MakeTranslation(gal, gal_x, -gal_lony/2, 0.0)

# -- Galerie perpendiculaire
# -- Dimensions de la galerie perpendiculaire
# -- La longueur est compt‰e € partir du centre
# -- de la galerie principale

gpe_long, gpe_diam = 200.0, 60.0 
gpe_x = gal_x
gpe = geompy.MakeCylinder(
    p0,
    geompy.MakeVectorDXDYDZ(-1.,0.,0.),
    gpe_diam/2,gpe_long)
gpe = geompy.MakeTranslation(gpe, gpe_x, 0, 0)

# -- Dimensions d'une alveole
# -- Construction d'une alv‰ole

alv_long, alv_diam = 60.0, 18.0
alv01 = geompy.MakeCylinder(p0, vy, alv_diam/2, alv_long)
alv02 = geompy.MakeMirrorByPlane(alv01, symPlane)
alv0 = geompy.MakeCompound([alv01, alv02])

# -- Construction des alv‰oles

alv_del, alv_sep = 40.0, 35.0
alv1 = geompy.MakeTranslation(alv0, gal_x-gal_diam/2-alv_sep, 0.0, 0.0)
alv2 = geompy.MakeTranslation(alv1, -alv_del, 0.0, 0.0 )
alv3 = geompy.MakeTranslation(alv2, -alv_del, 0.0, 0.0 )
alv = geompy.MakeCompound([alv1, alv2, alv3])

# -- Remplissage de la BO

gallery = geompy.MakePartition([alv, bo], [], [], [gal, gpe])

subshapes = geompy.SubShapeAll(gallery, geompy.ShapeType["SHAPE"])

alvGOs = []
alvGOs.append(subshapes[0])
alvGOs.append(subshapes[1])
alvGOs.append(subshapes[2])
alvGOs.append(subshapes[3])
alvGOs.append(subshapes[4])
alvGOs.append(subshapes[5])
alvcomp = geompy.MakeCompound(alvGOs)

boGO  = subshapes[6]
galGO = subshapes[7]
gpeGO = subshapes[8]

gallery = geompy.MakeCompound([boGO, alvcomp, gpeGO, galGO])

geompy.addToStudy(gallery,"Gallery 2")
