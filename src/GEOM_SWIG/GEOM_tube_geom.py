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

#==============================================================================
#  Info.
#  Bug (from script, bug)   : tube_geom_corrected.py, PAL5243
#  Modified                 : 25/11/2004
#  Author                   : Kovaltchuk Alexey
#  Project                  : PAL/SALOME
#==============================================================================
# Geometrie du cas test
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

#
# base de l'assemblage
#
L_base = 300.
l_base = 150.
h_base = 75.
e_base = 6.

base_bot = geompy.MakeBox(0.,0.,0.,L_base,l_base,e_base)
base_top = geompy.MakeTranslation(base_bot,0.,0.,h_base-e_base)
base_front = geompy.MakeBox(0,0,0,L_base,e_base,h_base)
base_rear = geompy.MakeTranslation(base_front,0.,l_base,0.)

tmp = geompy.MakeBoolean(base_top,base_rear,3)
tmp = geompy.MakeBoolean(tmp,base_bot,3)
base = geompy.MakeBoolean(tmp,base_front,3)

geompy.addToStudy(base,"base")
#
# traverse
#
L_trav = 60.
h_trav = 300.
e_trav = 7.
marge = 15.

flange_left = geompy.MakeBox(0.,0.,0,e_trav,L_trav,h_trav)
flange_right = geompy.MakeTranslation(flange_left,L_trav-e_trav,0.,0.)
flange_front = geompy.MakeBox(0.,0.,0,L_trav,e_trav,h_trav)
flange_rear = geompy.MakeTranslation(flange_front,0.,L_trav-e_trav,0.)

tmp = geompy.MakeBoolean(flange_left,flange_rear,3)
tmp = geompy.MakeBoolean(tmp,flange_right,3)
flange = geompy.MakeBoolean(tmp,flange_front,3)
#
# left chamfer
#
import math
length = e_trav/math.sin(math.pi/4.)
p1 = geompy.MakeVertex(0,0,0)
p2 = geompy.MakeVertex(0,1,0)
yaxis = geompy.MakeVector(p1, p2)
cut_left = geompy.MakeBox(0.,-marge,0.,-length,L_trav+marge,length)
cut_left = geompy.MakeRotation(cut_left,yaxis,-math.pi/4.)
cut_left = geompy.MakeTranslation(cut_left,e_trav,0.,0.)
#
# right chamfer
#
symPlaneYZ = geompy.MakePlane(
     geompy.MakeVertex(L_trav/2.,0.,0.),
     geompy.MakeVector(p1,geompy.MakeVertex(1.,0.,0.)), 10000. )
cut_right = geompy.MakeMirrorByPlane(cut_left,symPlaneYZ)
#
# front chamfer
#
xaxis = geompy.MakeVector(p1,geompy.MakeVertex(1.,0.,0.))
cut_front = geompy.MakeBox(-marge,0.,0.,L_trav+marge,length,length)
cut_front = geompy.MakeRotation(cut_front,xaxis,3.*math.pi/4.)
cut_front = geompy.MakeTranslation(cut_front,0.,e_trav,0.)
#
# rear chamfer
#
symPlaneXZ = geompy.MakePlane(
     geompy.MakeVertex(0.,L_trav/2.,0.),
     geompy.MakeVector(p1,geompy.MakeVertex(0.,1.,0.)), 10000. )
cut_rear = geompy.MakeMirrorByPlane(cut_front,symPlaneXZ)
#
# chamfer
#
trav = geompy.MakeBoolean(flange,cut_left,2)
trav = geompy.MakeBoolean(trav,cut_right,2)
trav = geompy.MakeBoolean(trav,cut_front,2)
trav = geompy.MakeBoolean(trav,cut_rear,2)
trav = geompy.MakeTranslation(trav,
                       L_base/2.-L_trav/2.,
                       l_base/2.-L_trav/2.,
                       h_base)
geompy.addToStudy(trav,"trav")
#
# Welding
#
ground = geompy.MakeBox(-1000.,-1000.,0.,1000.,1000.,-1000.)
weld_left = geompy.MakeBoolean(cut_left,ground,2)
weld_right = geompy.MakeBoolean(cut_right,ground,2)
weld_front = geompy.MakeBoolean(cut_front,ground,2)
weld_rear = geompy.MakeBoolean(cut_rear,ground,2)
#
# Assembly
#
coarse_weld = geompy.MakeBoolean(weld_left,weld_rear,3)
coarse_weld = geompy.MakeBoolean(coarse_weld,weld_right,3)
coarse_weld = geompy.MakeBoolean(coarse_weld,weld_front,3)
#
# Cleaners
#
left_cleaner = geompy.MakeBox(0.,-10*marge,0.,-10.*marge,10.*marge,10.*marge)
left_cleaner = geompy.MakeRotation(left_cleaner,yaxis,math.pi/4.)
left_cleaner = geompy.MakeTranslation(left_cleaner,-e_trav-2.*marge,0.,-2.*marge)

right_cleaner = geompy.MakeMirrorByPlane(left_cleaner,symPlaneYZ)

front_cleaner = geompy.MakeBox(-10*marge,0.,0.,10.*marge,10.*marge,10.*marge)
front_cleaner = geompy.MakeRotation(front_cleaner,xaxis,3.*math.pi/4.)
front_cleaner = geompy.MakeTranslation(front_cleaner,0.,2.*marge-e_trav,2.*marge)

rear_cleaner = geompy.MakeMirrorByPlane(front_cleaner,symPlaneXZ)
#
# Welding
#
weld = geompy.MakeBoolean(coarse_weld,left_cleaner,2)
weld = geompy.MakeBoolean(weld,rear_cleaner,2)
weld = geompy.MakeBoolean(weld,right_cleaner,2)
weld = geompy.MakeBoolean(weld,front_cleaner,2)
weld = geompy.MakeTranslation(weld,
                       L_base/2.-L_trav/2.,
                       l_base/2.-L_trav/2.,
                       h_base)
weldId = geompy.addToStudy(weld,"weld")
#
# Assembly
#
assemblage = geompy.Partition([base ,weld ,trav])
assemblageId = geompy.addToStudy(assemblage, "assemblage")
salome.sg.updateObjBrowser(True)
#
# Display
#
## gg.createAndDisplayGO(assemblageId)
## gg.createAndDisplayGO(baseId)
## gg.setTransparency(baseId,0.5)
## gg.setDisplayMode(baseId,1)
## gg.setColor(baseId,255,0,0)
## gg.createAndDisplayGO(travId)
## gg.setTransparency(travId,0.5)
## gg.setDisplayMode(travId,1)
## gg.setColor(travId,0,255,0)


