#==============================================================================
#  Info.
#  Bug (from script, bug)   : tube_geom_corrected.py, PAL5243
#  Modified                 : 25/11/2004
#  Author                   : Kovaltchuk Alexey
#  Project                  : PAL/SALOME
#==============================================================================
#
# Geometrie du cas test
#
import salome
from geompy import *

#
# base de l'assemblage
#
L_base = 300.
l_base = 150.
h_base = 75.
e_base = 6.

base_bot = MakeBox(0.,0.,0.,L_base,l_base,e_base)
base_top = MakeTranslation(base_bot,0.,0.,h_base-e_base)
base_front = MakeBox(0,0,0,L_base,e_base,h_base)
base_rear = MakeTranslation(base_front,0.,l_base,0.)

tmp = MakeBoolean(base_top,base_rear,3)
tmp = MakeBoolean(tmp,base_bot,3)
base = MakeBoolean(tmp,base_front,3)

addToStudy(base,"base")
#
# traverse
#
L_trav = 60.
h_trav = 300.
e_trav = 7.
marge = 15.

flange_left = MakeBox(0.,0.,0,e_trav,L_trav,h_trav)
flange_right = MakeTranslation(flange_left,L_trav-e_trav,0.,0.)
flange_front = MakeBox(0.,0.,0,L_trav,e_trav,h_trav)
flange_rear = MakeTranslation(flange_front,0.,L_trav-e_trav,0.)

tmp = MakeBoolean(flange_left,flange_rear,3)
tmp = MakeBoolean(tmp,flange_right,3)
flange = MakeBoolean(tmp,flange_front,3)
#
# left chamfer
#
import math
length = e_trav/math.sin(math.pi/4.)
p1 = MakeVertex(0,0,0)
p2 = MakeVertex(0,1,0)
yaxis = MakeVector(p1, p2)
cut_left = MakeBox(0.,-marge,0.,-length,L_trav+marge,length)
cut_left = MakeRotation(cut_left,yaxis,-math.pi/4.)
cut_left = MakeTranslation(cut_left,e_trav,0.,0.)
#
# right chamfer
#
symPlaneYZ = MakePlane(
     MakeVertex(L_trav/2.,0.,0.),
     MakeVector(p1,MakeVertex(1.,0.,0.)), 10000. )
cut_right = MakeMirrorByPlane(cut_left,symPlaneYZ)
#
# front chamfer
#
xaxis = MakeVector(p1,MakeVertex(1.,0.,0.))
cut_front = MakeBox(-marge,0.,0.,L_trav+marge,length,length)
cut_front = MakeRotation(cut_front,xaxis,3.*math.pi/4.)
cut_front = MakeTranslation(cut_front,0.,e_trav,0.)
#
# rear chamfer
#
symPlaneXZ = MakePlane(
     MakeVertex(0.,L_trav/2.,0.),
     MakeVector(p1,MakeVertex(0.,1.,0.)), 10000. )
cut_rear = MakeMirrorByPlane(cut_front,symPlaneXZ)
#
# chamfer
#
trav = MakeBoolean(flange,cut_left,2)
trav = MakeBoolean(trav,cut_right,2)
trav = MakeBoolean(trav,cut_front,2)
trav = MakeBoolean(trav,cut_rear,2)
trav = MakeTranslation(trav,
                       L_base/2.-L_trav/2.,
                       l_base/2.-L_trav/2.,
                       h_base)
addToStudy(trav,"trav")
#
# Welding
#
ground = MakeBox(-1000.,-1000.,0.,1000.,1000.,-1000.)
weld_left = MakeBoolean(cut_left,ground,2)
weld_right = MakeBoolean(cut_right,ground,2)
weld_front = MakeBoolean(cut_front,ground,2)
weld_rear = MakeBoolean(cut_rear,ground,2)
#
# Assembly
#
coarse_weld = MakeBoolean(weld_left,weld_rear,3)
coarse_weld = MakeBoolean(coarse_weld,weld_right,3)
coarse_weld = MakeBoolean(coarse_weld,weld_front,3)
#
# Cleaners
#
left_cleaner = MakeBox(0.,-10*marge,0.,-10.*marge,10.*marge,10.*marge)
left_cleaner = MakeRotation(left_cleaner,yaxis,math.pi/4.)
left_cleaner = MakeTranslation(left_cleaner,-e_trav-2.*marge,0.,-2.*marge)

right_cleaner = MakeMirrorByPlane(left_cleaner,symPlaneYZ)

front_cleaner = MakeBox(-10*marge,0.,0.,10.*marge,10.*marge,10.*marge)
front_cleaner = MakeRotation(front_cleaner,xaxis,3.*math.pi/4.)
front_cleaner = MakeTranslation(front_cleaner,0.,2.*marge-e_trav,2.*marge)

rear_cleaner = MakeMirrorByPlane(front_cleaner,symPlaneXZ)
#
# Welding
#
weld = MakeBoolean(coarse_weld,left_cleaner,2)
weld = MakeBoolean(weld,rear_cleaner,2)
weld = MakeBoolean(weld,right_cleaner,2)
weld = MakeBoolean(weld,front_cleaner,2)
weld = MakeTranslation(weld,
                       L_base/2.-L_trav/2.,
                       l_base/2.-L_trav/2.,
                       h_base)
weldId = addToStudy(weld,"weld")
#
# Assembly
#
assemblage = Partition([base ,weld ,trav])
assemblageId = addToStudy(assemblage, "assemblage")
salome.sg.updateObjBrowser(1)
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


