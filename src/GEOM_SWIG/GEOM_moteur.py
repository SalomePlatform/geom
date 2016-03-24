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
#  File   : GEOM_moteur.py
#  Author : Damien COQUERET, Open CASCADE
#  Module : GEOM
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
import math

#Variables modifiables
PosX = 0          #Position du cylindre dans l'espace
PosY = 0          #Il est oriente suivant Z
PosZ = 0
NbBranches = 7    #>2
HauteurT = 70     #Hauteur total du stator

#Variables
Angle1 = 2 * math.pi / NbBranches
Angle2 = Angle1 / 2
HauteurR = HauteurT / 3
Ep = HauteurT / 10
DExtExt = HauteurT * 6 / 7
DExtInt = DExtExt - Ep
DIntExt = DExtExt / 3
DIntInt = DExtExt / 4
EpRot = DIntExt * math.sin(Angle2)
Pos1C = PosX + DIntExt * math.cos(Angle2)
Pos1S = PosY + DIntExt * math.sin(Angle2)
PosCour = PosZ + HauteurT * 4 / 7
PosRot = PosZ + 0.9 * HauteurT

#Points
BasicOp = geompy.GetIBasicOperations(salome.myStudyId)
OO = BasicOp.MakePointXYZ(0, 0, 0)
P0 = BasicOp.MakePointXYZ(0, 0, 1)
P1 = BasicOp.MakePointXYZ(PosX, PosY, PosZ)
P2 = BasicOp.MakePointXYZ(PosX, PosY, PosZ + Ep)
P3 = BasicOp.MakePointXYZ(PosX, PosY, PosCour)
P4 = BasicOp.MakePointXYZ(PosX, PosY, PosCour + Ep)
P5 = BasicOp.MakePointXYZ(PosX, PosY, PosRot)
P6 = BasicOp.MakePointXYZ(Pos1C, Pos1S, PosZ)
P7 = BasicOp.MakePointXYZ(PosX + DExtExt, Pos1S, PosZ)
P8 = BasicOp.MakePointXYZ(Pos1C, Pos1S, PosZ + HauteurT)

#Vecteurs
V1 = BasicOp.MakeVectorTwoPnt(OO,P0)
V2 = BasicOp.MakeVectorTwoPnt(P1,P2)
V3 = BasicOp.MakeVectorTwoPnt(P6,P8)

#Cylindre central
C0  = geompy.MakeCylinder(P1, V1, DIntInt, PosCour + Ep - PosZ)
C01 = geompy.MakeCylinder(P1, V1, DIntExt, PosCour + Ep - PosZ)
Cylindre = geompy.MakeBoolean(C01, C0, 2)
#Id_Cyl = geompy.addToStudy(Cylindre, "Cylindre")

#Camemberts de coupe
B1 = BasicOp.MakeVectorTwoPnt(P6, P7)
B2 = geompy.MakePrismVecH(B1, V1, HauteurT)
S0 = geompy.MakeRevolution(B2, V3, Angle1)
CoupeList = []
CoupeList.append(S0)
Ind = 1
while Ind < NbBranches :
      S = geompy.MakeRotation(S0, V2, Ind * Angle1)
      CoupeList.append(S)
      Ind = Ind + 1
Coupe1 = geompy.MakeCompound(CoupeList)
#Id_Coupe1 = geompy.addToStudy(Coupe1, "Coupe1")

#Couronne1
C1 = geompy.MakeCylinder(P1, V1, DExtExt, HauteurT)
C2 = geompy.MakeCylinder(P2, V1, DExtInt, HauteurT)
C3 = geompy.MakeBoolean(C1, C2, 2)
C31 = geompy.MakeBoolean(C3, C0, 2)
Couronne1 = geompy.MakeBoolean(C31, Coupe1, 2)
#Id_Couronne1 = geompy.addToStudy(Couronne1, "Couronne1")

#Couronne2
C4 = geompy.MakeCylinder(P3, V1, DExtExt, PosZ + HauteurT - PosCour)
C5 = geompy.MakeCylinder(P4, V1, DExtInt, HauteurT)
Coupe2 = geompy.MakeRotation(Coupe1, V2, Angle2)
C6 = geompy.MakeBoolean(C4, C5, 2)
C61 = geompy.MakeBoolean(C6, C0, 2)
Couronne2 = geompy.MakeBoolean(C61, Coupe2, 2)
#Id_Couronne2 = geompy.addToStudy(Couronne2, "Couronne2")

#Rotor1
C9 = geompy.MakeCylinder(P5, V1, DIntExt, HauteurR)
C10 = geompy.MakeCylinder(P5, V1, DIntExt / 4, 2 * HauteurR)
Rotor1List = []
Rotor1List.append(C9)
Rotor1List.append(C10)
Rotor1 = geompy.MakeCompound(Rotor1List)
Id_Rotor1 = geompy.addToStudy(Rotor1, "Rotor1")

#Rotor2
D0 = geompy.MakeBox(2 * Pos1C - PosX, -Pos1S + 2 * PosY, PosRot, 2 * Pos1C + EpRot - PosX, Pos1S, PosRot + EpRot / 2)
Rotor2List = []
Rotor2List.append(D0)
Ind = 1
while Ind < NbBranches :
      R = geompy.MakeRotation(D0, V2, Ind * Angle1)
      Rotor2List.append(R)
      Ind = Ind + 1
Rotor2 = geompy.MakeCompound(Rotor2List)
Id_Rotor2 = geompy.addToStudy(Rotor2, "Rotor2")

#Rotor3
Rotor3 = geompy.MakeRotation(Rotor2, V2, Angle2)
Id_Rotor3 = geompy.addToStudy(Rotor3, "Rotor3")

#Stator
StatorList = []
StatorList.append(Cylindre)
StatorList.append(Couronne1)
StatorList.append(Couronne2)
Stator = geompy.MakeCompound(StatorList)
Id_Stator = geompy.addToStudy(Stator, "Stator")
