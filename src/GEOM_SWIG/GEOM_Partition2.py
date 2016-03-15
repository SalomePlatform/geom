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
#  File   : GEOM_Partition2.py
#  Module : GEOM
# import callovo_01_GEOM
# reload(callovo_01_GEOM)
# --------------------------------------------
# Geometrie avec une galerie perpendiculaire
# --------------------------------------------
#

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

vecx = geompy.MakeVectorDXDYDZ(1.,0.,0.)
vecy = geompy.MakeVectorDXDYDZ(0.,1.,0.)

# -- colis

colis_xmin = 05.76
colis_xmax = 19.83
colis_radius = 0.3

colis = geompy.MakeCylinder(
    geompy.MakeVertex(colis_xmin,0.,0.),
    vecx,
    colis_radius,
    colis_xmax-colis_xmin)

# -- bo

bo_xmin = 04.83
bo_xmax = colis_xmax
bo_radius = 1.23

bo = geompy.MakeCylinder(
    geompy.MakeVertex(bo_xmin,0.,0.),
    vecx,
    bo_radius,
    bo_xmax-bo_xmin)


# -- rupture alveole

alvRup_xmin = 04.46
alvRup_xmax = colis_xmax
alvRup_radius = 1.6

alvRup = geompy.MakeCylinder(
    geompy.MakeVertex(alvRup_xmin,0.,0.),
    vecx,
    alvRup_radius,
    alvRup_xmax-alvRup_xmin)


# bouchon

bouchon_xmin = colis_xmax
bouchon_xmax = 22.83
bouchon_radius = alvRup_radius

bouchon = geompy.MakeCylinder(
    geompy.MakeVertex(bouchon_xmin,0.,0.),
    vecx,
    bouchon_radius,
    bouchon_xmax-bouchon_xmin)

# galerie

galerie_xmax = 25.0
galerie_radius = 3.0
box_y = 30.0

pnt_galerie_xmax = geompy.MakeVertex(galerie_xmax,0.,0.)

galerie = geompy.MakeCylinder(
    pnt_galerie_xmax,
    vecy,
    galerie_radius,
    box_y)
galerie = geompy.MakeTranslation(galerie, 0, -box_y/2, 0)

# -- box

box = geompy.MakeBox(0., 0., 0., galerie_xmax, box_y, box_y)
box = geompy.MakeTranslation(box, 0.0, -box_y/2, -box_y/2)


# -- rupture galerie

galRup_xmax = galerie_xmax
galRup_radius = 3.9

galRup = geompy.MakeCylinder(
    pnt_galerie_xmax,
    vecy,
    galRup_radius,
    box_y)
galRup = geompy.MakeTranslation(galRup, 0, -box_y/2, 0)

# -- endommagement galerie

galEnd_xmax = galerie_xmax
galEnd_radius = 4.6

galEnd = geompy.MakeCylinder(pnt_galerie_xmax, vecy, galEnd_radius, box_y)
galEnd = geompy.MakeTranslation(galEnd, 0, -box_y/2, 0)

# -- endommagement alveole

alvEnd_xmin = 03.6
alvEnd_xmax = galerie_xmax
alvEnd_radius = 2.46

alvEnd = geompy.MakeCylinder(
    geompy.MakeVertex(alvEnd_xmin,0.,0.),
    vecx,
    alvEnd_radius,
    alvEnd_xmax-alvEnd_xmin)

# remove internal parts of galEnd intersecting alvEnd

galEnd_alvEnd = geompy.MakePartition([alvEnd], [], [], [galEnd])

GOList =  [colis]
GOList += [bo]
GOList += [alvRup]
#GOList += [galerie ]
GOList += [galRup]
GOList += [galEnd_alvEnd]

# --

# remove section parts outside bo and inside bouchon

callovo = geompy.MakePartition(GOList, [], [box], [galerie, bouchon])

geompy.addToStudy(callovo, "callovo perp. 2")
