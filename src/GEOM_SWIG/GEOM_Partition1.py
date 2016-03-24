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
#  File   : GEOM_Partition1.py
#  Module : GEOM
# appel:
# import alveole_3D_01_GEOM
# reload(alveole_3D_01_GEOM)
# -- Rayon de la bariere
#
barier_height = 7.0
barier_radius = 5.6 / 2 # Rayon de la bariere
colis_radius = 1.0 / 2  # Rayon du colis
colis_step = 2.0        # Distance s‰parant deux colis
cc_width = 0.11         # Epaisseur du complement de colisage

# --

cc_radius = colis_radius + cc_width
from math import sqrt
colis_center = sqrt(2.0)*colis_step/2

# --

import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

boolean_common  = 1
boolean_cut     = 2
boolean_fuse    = 3
boolean_section = 4

# --

pnt0 = geompy.MakeVertex(0.,0.,0.)
vecz = geompy.MakeVectorDXDYDZ(0.,0.,1.)

barier = geompy.MakeCylinder(
    pnt0,
    vecz,
    barier_radius,
    barier_height)

# --

colis = geompy.MakeCylinder(pnt0, vecz, colis_radius, barier_height)

cc = geompy.MakeCylinder(pnt0, vecz, cc_radius, barier_height)

colis_cc = geompy.MakeCompound([colis, cc])

colis_cc = geompy.MakeTranslation(colis_cc, colis_center, 0.0, 0.0)

colis_cc_multi = geompy.MultiRotate1D(colis_cc, vecz, 4)

# --

Compound1 = geompy.MakeCompound([colis_cc_multi, barier])
SubShape_theShape = geompy.SubShapeAll(Compound1,geompy.ShapeType["SOLID"])
alveole = geompy.MakePartition(SubShape_theShape)
#alveole = geompy.MakePartition([colis_cc_multi, barier])

geompy.addToStudy(alveole, "alveole before explode")

subshapes = geompy.SubShapeAll(alveole, geompy.ShapeType["SHAPE"])

## there are 9 sub-shapes

comp1 = geompy.MakeCompound([subshapes[0], subshapes[1]]);
comp2 = geompy.MakeCompound([subshapes[2], subshapes[3]]);
comp3 = geompy.MakeCompound([subshapes[4], subshapes[5]]);
comp4 = geompy.MakeCompound([subshapes[6], subshapes[7]]);

compGOs = []
compGOs.append(comp1);
compGOs.append(comp2);
compGOs.append(comp3);
compGOs.append(comp4);
comp = geompy.MakeCompound(compGOs);

alveole = geompy.MakeCompound([comp, subshapes[8]]);

geompy.addToStudy(alveole, "alveole")
