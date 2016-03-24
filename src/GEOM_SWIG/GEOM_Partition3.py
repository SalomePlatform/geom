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
#  File   : GEOM_Partition3.py
#  Module : GEOM
# import callovo_01_GEOM
# reload(callovo_01_GEOM)
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

vecx = geompy.MakeVectorDXDYDZ(1.,0.,0.)

# -- colis

colis_xmin = 05.76
colis_xmax = 19.83
colis_radius = 0.3

colis = geompy.MakeCylinder(
    geompy.MakeVertex(colis_xmin,0.,0.),
    vecx,
    colis_radius,
    colis_xmax-colis_xmin)

GOList = [colis]

# -- bo

bo_xmin = 04.83
bo_xmax = colis_xmax
bo_radius = 1.23

bo = geompy.MakeCylinder(
    geompy.MakeVertex(bo_xmin,0.,0.),
    vecx,
    bo_radius,
    bo_xmax-bo_xmin)

GOList.append(bo)

# -- rupture alveole

alvRup_xmin = 04.46
alvRup_xmax = colis_xmax
alvRup_radius = 1.6

alvRup = geompy.MakeCylinder(
    geompy.MakeVertex(alvRup_xmin,0.,0.),
    vecx,
    alvRup_radius,
    alvRup_xmax-alvRup_xmin)

GOList.append(alvRup)

# bouchon

bouchon_xmin = colis_xmax
bouchon_xmax = 22.83
bouchon_radius = alvRup_radius

bouchon = geompy.MakeCylinder(
    geompy.MakeVertex(bouchon_xmin,0.,0.),
    vecx,
    bouchon_radius,
    bouchon_xmax-bouchon_xmin)

#GOList.append(bouchon)

# -- endommagement alveole

alvEnd_xmin = 03.6
alvEnd_xmax = 20.66
alvEnd_radius = 2.46

alvEnd = geompy.MakeCylinder(
    geompy.MakeVertex(alvEnd_xmin,0.,0.),
    vecx,
    alvEnd_radius,
    alvEnd_xmax-alvEnd_xmin)

GOList.append(alvEnd)

# galerie

galerie_xmin = bouchon_xmax
galerie_xmax = 25.0
galerie_radius = 2.17

galerie = geompy.MakeCylinder(
    geompy.MakeVertex(galerie_xmin,0.,0.),
    vecx,
    galerie_radius,
    galerie_xmax-galerie_xmin)

GOList.append(galerie)

# -- rupture galerie

galRup_xmin = 22.18
galRup_xmax = galerie_xmax
galRup_radius = 2.82

galRup = geompy.MakeCylinder(
    geompy.MakeVertex(galRup_xmin,0.,0.),
    vecx,
    galRup_radius,
    galRup_xmax-galRup_xmin)

GOList.append(galRup)

# -- endom. galerie

galEnd_xmin = alvEnd_xmax
galEnd_xmax = galerie_xmax
galEnd_radius = 4.34

galEnd = geompy.MakeCylinder(
    geompy.MakeVertex(galEnd_xmin,0.,0.),
    vecx,
    galEnd_radius,
    galEnd_xmax-galEnd_xmin)

GOList.append(galEnd)

# --

box_y = 30.0
box = geompy.MakeBox(0., 0., 0., galerie_xmax, box_y, box_y)
box = geompy.MakeTranslation(box, 0.0, -box_y/2, -box_y/2)

GOList.append(box)

# --

callovo = geompy.MakePartition(GOList, [], [], [bouchon])
geompy.addToStudy(callovo, "callovo 2")
