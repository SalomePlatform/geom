#==============================================================================
#  File      : GEOM_moteur.py
#  Created   : mon feb 18 11:55:00 CET 2002
#  Author    : Damien COQUERET, Open CASCADE
#  Project   : SALOME
#  Copyright : Open CASCADE, 2002
#  $Header$
#==============================================================================

import salome
import geompy
import math

geom = salome.lcc.FindOrLoadComponent("FactoryServer", "GEOM")
myBuilder = salome.myStudy.NewBuilder()

#Variables modifiables
PosX = 0          #Position du cylindre dans l'espace
PosY = 0          #Il est oriente suivant Z
PosZ = 0
NbBranches = 7    #>2
HauteurT = 70     #Hauteur total du stator

#Varaibles
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
P0 = geom.MakePointStruct(0, 0, 1)
P1 = geom.MakePointStruct(PosX, PosY, PosZ)
P2 = geom.MakePointStruct(PosX, PosY, PosZ + Ep)
P3 = geom.MakePointStruct(PosX, PosY, PosCour)
P4 = geom.MakePointStruct(PosX, PosY, PosCour + Ep)
P5 = geom.MakePointStruct(PosX, PosY, PosRot)
P6 = geom.MakePointStruct(Pos1C, Pos1S, PosZ)
P7 = geom.MakePointStruct(PosX + DExtExt, Pos1S, PosZ)
P8 = geom.MakePointStruct(Pos1C, Pos1S, PosZ + HauteurT)

#Vecteurs
V1 = geom.MakeDirection(P0)
V2 = geom.MakeAxisStruct(PosX, PosY, PosZ, 0, 0, 1)
V3 = geom.MakeAxisStruct(Pos1C, Pos1S, PosZ, 0, 0, 1)

#Cylindre central
C0 = geompy.MakeCylinder(P1, V1, DIntInt, PosCour + Ep - PosZ)
C01 = geompy.MakeCylinder(P1, V1, DIntExt, PosCour + Ep - PosZ)
Cylindre = geompy.MakeBoolean(C01, C0, 2)
#Id_Cyl = geompy.addToStudy(Cylindre, "Cylindre")

#Camemberts de coupe
B1 = geompy.MakeVector(P6, P7)
B2 = geompy.MakePrism(B1, P6, P8)
S0 = geompy.MakeRevolution(B2, V3, Angle1)
CoupeList = []
CoupeList.append(S0)
Ind = 1
while Ind < NbBranches :
      S = geompy.MakeRotation(S0, V2, Ind * Angle1)
      CoupeList.append(S)
      Ind = Ind + 1
IorCoupeList = []
for CoupeShape in CoupeList :
    IorCoupeList.append(CoupeShape._get_Name())
Coupe1 = geompy.MakeCompound(IorCoupeList)
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
IorRotor1List = []
for Rotor1Shape in Rotor1List :
    IorRotor1List.append(Rotor1Shape._get_Name())
Rotor1 = geompy.MakeCompound(IorRotor1List)
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
IorRotor2List = []
for Rotor2Shape in Rotor2List :
    IorRotor2List.append(Rotor2Shape._get_Name())
Rotor2 = geompy.MakeCompound(IorRotor2List)
Id_Rotor2 = geompy.addToStudy(Rotor2, "Rotor2")

#Rotor3
Rotor3 = geompy.MakeRotation(Rotor2, V2, Angle2)
Id_Rotor3 = geompy.addToStudy(Rotor3, "Rotor3")

#Stator
StatorList = []
StatorList.append(Cylindre)
StatorList.append(Couronne1)
StatorList.append(Couronne2)
IorStatorList = []
for StatorShape in StatorList :
    IorStatorList.append(StatorShape._get_Name())
Stator = geompy.MakeCompound(IorStatorList)
Id_Stator = geompy.addToStudy(Stator, "Stator")
