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

import salome_version

def TestMeasureOperations (geompy, math):

  p0   = geompy.MakeVertex(0 ,  0,  0)
  p137 = geompy.MakeVertex(10, 30, 70)

  box = geompy.MakeBoxTwoPnt(p0, p137)

  p678 = geompy.MakeVertex(60, 70, 80)
  p789 = geompy.MakeVertex(70, 80, 90)

  vz = geompy.MakeVectorDXDYDZ(0, 0, 1)

  cube = geompy.MakeBoxTwoPnt(p678, p789)

  cylinder = geompy.MakeCylinder(p0, vz, 5, 70)

  ####### PointCoordinates #######

  Coords = geompy.PointCoordinates(p137)
  if Coords[0] != 10 or Coords[1] != 30 or Coords[2] != 70:
    print "Coordinates of p137 must be (10, 30, 70), but returned (", Coords[0], ", ", Coords[1], ", ", Coords[2], ")"

  ####### CheckShape #######

  (IsValid, err) = geompy.CheckShape(box, 0, 2)
  if IsValid == 0:
    geompy.PrintShapeError(box, err)
    raise RuntimeError, "Invalid box created"
  else:
    print "\nBox is valid"

  ####### Detect Self-intersections #######

  selfIntersected = geompy.MakeCompound([box, cylinder])
  if geompy.CheckSelfIntersections(selfIntersected):
    raise RuntimeError, "Existing self-intersection is not detected"

  ####### Detect Self-intersections fast #######

  if salome_version.getXVersion() > "0x70600":
    if geompy.CheckSelfIntersectionsFast(selfIntersected):
      raise RuntimeError, "Existing self-intersection is not detected"

  ####### Fast intersection #######

  if not geompy.FastIntersect(box, cylinder)[0]:
    raise RuntimeError, "Existing intersection is not detected"

  ####### WhatIs #######

  Descr = geompy.WhatIs(box)
  print "\nBox 10x30x70 description:"
  print Descr

  ####### NbShapes #######

  NbSolids = geompy.NbShapes(box, geompy.ShapeType["SOLID"])
  print "\nBox 10x30x70 quantity of solids:", NbSolids

  ####### ShapeInfo #######

  BoxInfo = geompy.ShapeInfo(box)
  print "\nBox 10x30x70 shapes:"
  print BoxInfo

  ####### BasicProperties #######

  Props = geompy.BasicProperties(box)
  print "\nBox 10x30x70 Basic Properties:"
  print " Wires length: ", Props[0]
  print " Surface area: ", Props[1]
  print " Volume      : ", Props[2]

  dl = math.sqrt((Props[0] -   880)*(Props[0] -   880))
  da = math.sqrt((Props[1] -  6200)*(Props[1] -  6200))
  dv = math.sqrt((Props[2] - 21000)*(Props[2] - 21000))
  #print "|Props[0] - 880| = ", dl

  if dl > 1e-7 or da > 1e-7 or dv > 1e-7:
    print "While must be:"
    print " Wires length: ", 880
    print " Surface area: ", 6200
    print " Volume      : ", 21000

  ####### BoundingBox #######

  BB = geompy.BoundingBox(box)
  print "\nBounding Box of box 10x30x70:"
  print " Xmin = ", BB[0], ", Xmax = ", BB[1]
  print " Ymin = ", BB[2], ", Ymax = ", BB[3]
  print " Zmin = ", BB[4], ", Zmax = ", BB[5]
  BB = geompy.MakeBoundingBox(box)
  geompy.addToStudy(BB, "BoundingBox")

  ####### Inertia #######

  In = geompy.Inertia(box)
  print "\nInertia matrix of box 10x30x70:"
  print " (", In[0], ", ", In[1], ", ", In[2], ")"
  print " (", In[3], ", ", In[4], ", ", In[5], ")"
  print " (", In[6], ", ", In[7], ", ", In[8], ")"
  print "Main moments of inertia of box 10x30x70:"
  print " Ix = ", In[9], ", Iy = ", In[10], ", Iz = ", In[11]

  ####### Tolerance #######

  Toler = geompy.Tolerance(box)
  print "\nBox 10x30x70 tolerance:"
  print " Face min. tolerance: ", Toler[0]
  print " Face max. tolerance: ", Toler[1]
  print " Edge min. tolerance: ", Toler[2]
  print " Edge max. tolerance: ", Toler[3]
  print " Vertex min. tolerance: ", Toler[4]
  print " Vertex max. tolerance: ", Toler[5]

  ####### MakeCDG #######

  pcdg = geompy.MakeCDG(box)
  if pcdg is None:
    raise RuntimeError, "MakeCDG(box) failed"
  else:
    print "\nCentre of gravity of box has been successfully obtained:"
    Coords = geompy.PointCoordinates(pcdg)
    print "(", Coords[0], ", ", Coords[1], ", ", Coords[2], ")"
    if Coords[0] != 5 or Coords[1] != 15 or Coords[2] != 35:
      print "But must be (5, 15, 35)"

  ####### GetNormal #######

  faces = geompy.SubShapeAllSortedCentres(box, geompy.ShapeType["FACE"])
  face0 = faces[0]
  vnorm = geompy.GetNormal(face0)
  if vnorm is None:
    raise RuntimeError, "GetNormal(face0) failed"
  else:
    geompy.addToStudy(face0, "Face0")
    geompy.addToStudy(vnorm, "Normale to Face0")
    print "\nNormale of face has been successfully obtained:"
    #Coords = geompy.PointCoordinates(pcdg)
    #print "(", Coords[0], ", ", Coords[1], ", ", Coords[2], ")"
    #if Coords[0] != 5 or Coords[1] != 15 or Coords[2] != 35:
    #  print "But must be (5, 15, 35)"

  ####### MinDistance #######

  MinDist = geompy.MinDistance(box, cube)

  #print "\nMinimal distance between Box and Cube = ", MinDist[0]
  #print "It is reached at points:"
  #print " On Box  (", MinDist[1], ", ", MinDist[2], ", ", MinDist[3], ")"
  #print " On Cube (", MinDist[4], ", ", MinDist[5], ", ", MinDist[6], ")"

  print "\nMinimal distance between Box and Cube = ", MinDist

  MinDistComps = geompy.MinDistanceComponents(box, cube)
  print "\nMinimal distance between Box and Cube = ", MinDistComps[0]
  print "Its components are  (", MinDistComps[1], ", ", MinDistComps[2], ", ", MinDistComps[3], ")"

  # Get all closest points
  [nbSols, listCoords] = geompy.ClosestPoints(box, cube)
  for i in range(nbSols):
    v1 = geompy.MakeVertex(listCoords[i*6 + 0], listCoords[i*6 + 1], listCoords[i*6 + 2])
    v2 = geompy.MakeVertex(listCoords[i*6 + 3], listCoords[i*6 + 4], listCoords[i*6 + 5])

    geompy.addToStudy(v1, 'MinDist_%d_on_Box'%(i+1))
    geompy.addToStudy(v2, 'MinDist_%d_on_Cube'%(i+1))
    pass

  ####### Angle #######

  OX  = geompy.MakeVectorDXDYDZ(10, 0,0)
  OXY = geompy.MakeVectorDXDYDZ(10,10,0)

  # in one plane
  Angle = geompy.GetAngle(OX, OXY)

  print "\nAngle between OX and OXY = ", Angle
  if math.fabs(Angle - 45.0) > 1e-05:
    print "  Error: returned angle is", Angle, "while must be 45.0"

  Angle = geompy.GetAngleRadians(OX, OXY)

  print "\nAngle between OX and OXY in radians = ", Angle
  if math.fabs(Angle - math.pi/4) > 1e-05:
    print "  Error: returned angle is", Angle, "while must be pi/4"
    pass

  # not in one plane
  OXY_shift = geompy.MakeTranslation(OXY,10,-10,20)
  Angle = geompy.GetAngle(OX, OXY_shift)

  print "Angle between OX and OXY_shift = ", Angle
  if math.fabs(Angle - 45.0) > 1e-05:
    print "  Error: returned angle is", Angle, "while must be 45.0"

  ####### Position (LCS) #######

  Pos = geompy.GetPosition(box)
  print "\nPosition(LCS) of box 10x30x70:"
  print "Origin: (", Pos[0], ", ", Pos[1], ", ", Pos[2], ")"
  print "Z axis: (", Pos[3], ", ", Pos[4], ", ", Pos[5], ")"
  print "X axis: (", Pos[6], ", ", Pos[7], ", ", Pos[8], ")"

  ####### KindOfShape #######

  Kind = geompy.KindOfShape(box)
  print "\nKindOfShape(box 10x30x70):", Kind
  #if Kind[0] != geompy.kind.BOX:
  #  print "Error: returned type is", Kind[0], "while must be", geompy.kind.BOX

  Kind = geompy.KindOfShape(p137)
  print "\nKindOfShape(p137):", Kind
  if Kind[0] != geompy.kind.VERTEX:
    print "  Error: returned type is", Kind[0], "while must be", geompy.kind.VERTEX
  else:
    dx = math.fabs(Kind[1] - 10)
    dy = math.fabs(Kind[2] - 30)
    dz = math.fabs(Kind[3] - 70)
    if (dx + dy + dz) > 1e-5:
      print "  Error: coordinates are (", Kind[1], ",", Kind[2], ",", Kind[3], ") while must be (10, 20, 30)"

  pass
