# -*- coding: iso-8859-1 -*-

import sys
import salome

import GEOM
from salome.geom import geomBuilder
import math
import SALOMEDS


def demidisk(study, r1, a1, roty=0):
  geompy = geomBuilder.New(study)
  
  O = geompy.MakeVertex(0, 0, 0)
  OX = geompy.MakeVectorDXDYDZ(1, 0, 0) 
  OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
  OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
  
  v=range(8)
  l=range(8)
  v0 = geompy.MakeVertex(0, 0, 0)
  v[0] = geompy.MakeVertex(0, r1/2.0, 0)
  v[1] = geompy.MakeVertex(0, r1, 0)
  l[1] = geompy.MakeLineTwoPnt(v[0], v[1])
  l[2] = geompy.MakeRotation(l[1], OX, a1*math.pi/180.0)
  v[4] = geompy.MakeRotation(v[0], OX, a1*math.pi/180.0)
  v[6] = geompy.MakeRotation(v[1], OX, a1*math.pi/180.0)

  v[2] = geompy.MakeVertex(0, -r1/2.0, 0)
  v[3] = geompy.MakeVertex(0, -r1, 0)
  l[3] = geompy.MakeLineTwoPnt(v[2], v[3])
  l[4] = geompy.MakeRotation(l[3], OX, -a1*math.pi/180.0)
  v[5] = geompy.MakeRotation(v[2], OX, -a1*math.pi/180.0)
  v[7] = geompy.MakeRotation(v[3], OX, -a1*math.pi/180.0)

  l[5] = geompy.MakeLineTwoPnt(v[4], v[5])
  l[6] = geompy.MakeLineTwoPnt(v[0], v[4])
  l[7] = geompy.MakeLineTwoPnt(v[2], v[5])

  v7 = geompy.MakeVertex(0, 0, r1)
  arc1 = geompy.MakeArc(v[1], v7, v[3])
  l[0] = geompy.MakeLineTwoPnt(v[1], v[3])
  face1 = geompy.MakeFaceWires([arc1, l[0]], 1)
  part1 = geompy.MakePartition([face1], [l[2], l[4], l[5], l[6], l[7]], [], [], geompy.ShapeType["FACE"], 0, [], 0, True)

  if roty != 0:
    vrot = [ geompy.MakeRotation(vert, OY, roty*math.pi/180.0) for vert in v ]
    lrot = [ geompy.MakeRotation(lin, OY, roty*math.pi/180.0) for lin in l ]
    arc = geompy.MakeRotation(arc1, OY, roty*math.pi/180.0)
    part = geompy.MakeRotation(part1, OY, roty*math.pi/180.0)
    return vrot, lrot, arc, part
  else:
    return v, l, arc1, part1

def pointsProjetes(study, vref, face):
  geompy = geomBuilder.New(study)
  vface = geompy.ExtractShapes(face, geompy.ShapeType["VERTEX"], True)
  vord = range(len(vref))
  plan = geompy.MakePlaneThreePnt(vref[0], vref[1], vref[-1], 10000)
  vproj = [ geompy.MakeProjection(vert, plan) for vert in vface ]
  for i,v in enumerate(vproj):
    dist = [ (geompy.MinDistance(v, vr), j) for j,vr in enumerate(vref) ]
    dist.sort()
    #print dist
    if dist[0][0] < 1.e-3:
      vord[dist[0][1]] = vface[i]
  return vord

def arcsProjetes(study, vf, face):
  geompy = geomBuilder.New(study)
  lface = geompy.ExtractShapes(face, geompy.ShapeType["EDGE"], True)
  lord = range(3)
  ends = [vf[1], vf[6], vf[7], vf[3]]
  for i in range(3):
    for lf in lface:
      pts = geompy.ExtractShapes(lf, geompy.ShapeType["VERTEX"], True)
      if (((geompy.MinDistance(pts[0], ends[i]) < 0.001) and (geompy.MinDistance(pts[1], ends[i+1]) < 0.001)) or
          ((geompy.MinDistance(pts[1], ends[i]) < 0.001) and (geompy.MinDistance(pts[0], ends[i+1]) < 0.001))):
        lord[i] = lf
        print "arc_%d OK"%i
        break
    pass
  return lord
 
def build_shape(study, r1, r2, h1, h2):
  geompy = geomBuilder.New(study)
  
  O = geompy.MakeVertex(0, 0, 0)
  OX = geompy.MakeVectorDXDYDZ(1, 0, 0) 
  OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
  OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
  
  a1 = 45.0
  seuilmax = 0.1
  ratio = float(r2)/float(r1)
  if ratio > (1.0 -seuilmax):
    a1 = 45.0*(1.0 -ratio)/seuilmax
    
    
  # --- sections droites des deux demi cylindres avec le partionnement

  v1, l1, arc1, part1 = demidisk(study, r1, a1)
  v2, l2, arc2, part2 = demidisk(study, r2, a1, 90.0)

  geompy.addToStudy(part1, 'part1')
  geompy.addToStudy(part2, 'part2')

  # --- extrusion des sections --> demi cylindres de travail, pour en extraire les sections utilisées au niveau du Té
  #     et enveloppe cylindrique du cylindre principal

  demicyl1 = geompy.MakePrismVecH(part1, OX, h1)
  demicyl2 = geompy.MakePrismVecH(part2, OZ, h2)
  arcextru = geompy.MakePrismVecH(arc1, OX, h1)

  geompy.addToStudy(demicyl1, 'demicyl1')
  geompy.addToStudy(demicyl2, 'demicyl2')
  geompy.addToStudy(arcextru, 'arcextru')

  # --- plan de coupe à 45° sur le cylindre principal,
  #     section à 45° du cylndre principal,
  #     section du cylindre secondaire par l'enveloppe cylindique du cylindre principal

  plan1 = geompy.MakePlane(O, OX, 4*r1)
  planr = geompy.MakeRotation(plan1, OY, 45*math.pi/180.0)
  geompy.addToStudy(planr, 'planr')

  sect45 = geompy.MakeCommonList([demicyl1, planr], True)
  geompy.addToStudy(sect45, 'sect45')

  sect90 = geompy.MakeCommonList([demicyl2, arcextru], True)
  geompy.addToStudy(sect90, 'sect90')

  # --- liste ordonnée des points projetés sur les deux sections

  vord45 = pointsProjetes(study, v1, sect45)
  vord90 = pointsProjetes(study, v2, sect90)
  for i,v in enumerate(vord45):
    geompy.addToStudyInFather(sect45, v, 'v%d'%i)
  for i,v in enumerate(vord90):
    geompy.addToStudyInFather(sect90, v, 'v%d'%i)

  # --- identification des projections des trois arcs de cercle, sur les deux sections.

  lord45 = arcsProjetes(study, vord45, sect45)
  lord90 = arcsProjetes(study, vord90, sect90)
  for i,l in enumerate(lord45):
    geompy.addToStudyInFather(sect45, l, 'l%d'%i)
  for i,l in enumerate(lord90):
    geompy.addToStudyInFather(sect90, l, 'l%d'%i)

  # --- abaissement des quatre points centraux de la section du cylindre secondaire

  dz = -r2/2.0
  for i in (0, 2, 4, 5):
    vord90[i] = geompy.TranslateDXDYDZ(vord90[i], 0, 0, dz, True)
    geompy.addToStudyInFather(sect90, vord90[i], 'vm%d'%i)
    
  # --- création des deux arêtes curvilignes sur l'enveloppe cylindrique du cylindre principal, à la jonction

  curv = [None for i in range(4)] # liaisons entre les points 1, 3, 6 et 7 des 2 sections

  curv[0] = geompy.MakeArcCenter(O, vord90[1] , vord45[1], False)
  curv[1] = geompy.MakeArcCenter(O, vord90[3] , vord45[3], False)

  lipts = ((6, 6, 4), (7, 7, 5))
  for i, ipts in enumerate(lipts):
    print i, ipts
    p0 = vord90[ipts[0]]
    p1 = vord45[ipts[1]]
    p2 = vord45[ipts[2]]
    plan = geompy.MakePlaneThreePnt(p0, p1, p2, 10000)
    #geompy.addToStudy(plan, "plan%d"%i)
    section = geompy.MakeSection(plan, arcextru, True)
    secpart = geompy.MakePartition([section], [sect45, sect90], [], [], geompy.ShapeType["EDGE"], 0, [], 0, True)
    #geompy.addToStudy(secpart, "secpart%d"%i)
    lsec = geompy.ExtractShapes(secpart, geompy.ShapeType["EDGE"], True)
    #print "len(lsec)", len(lsec)
    for l in lsec:
      pts = geompy.ExtractShapes(l, geompy.ShapeType["VERTEX"], True)
      if (((geompy.MinDistance(pts[0], p0) < 0.001) and (geompy.MinDistance(pts[1], p1) < 0.001)) or
          ((geompy.MinDistance(pts[1], p0) < 0.001) and (geompy.MinDistance(pts[0], p1) < 0.001))):
        curv[i+2] =l
        #print "curv_%d OK"%i
        break
  for i,l in enumerate(curv):
    geompy.addToStudyInFather(arcextru, l, "curv%d"%i)
    
  # --- creation des arêtes droites manquantes, des faces et volumes pour les quatre volumes de la jonction

  edges = [None for i in range(8)]
  edges[0] = geompy.MakeLineTwoPnt(vord45[0], vord90[0])
  edges[1] = curv[0]
  edges[2] = geompy.MakeLineTwoPnt(vord45[2], vord90[2])
  edges[3] = curv[1]
  edges[4] = geompy.MakeLineTwoPnt(vord45[4], vord90[4])
  edges[5] = geompy.MakeLineTwoPnt(vord45[5], vord90[5])
  edges[6] = curv[2]
  edges[7] = curv[3]
  for i,l in enumerate(edges):
    geompy.addToStudy( l, "edge%d"%i)

  ed45 = [None for i in range(8)]
  ed45[0] = geompy.MakeLineTwoPnt(vord45[0], vord45[2])
  ed45[1] = geompy.MakeLineTwoPnt(vord45[0], vord45[1])
  ed45[2] = geompy.MakeLineTwoPnt(vord45[4], vord45[6])
  ed45[3] = geompy.MakeLineTwoPnt(vord45[2], vord45[3])
  ed45[4] = geompy.MakeLineTwoPnt(vord45[5], vord45[7])
  ed45[5] = geompy.MakeLineTwoPnt(vord45[4], vord45[5])
  ed45[6] = geompy.MakeLineTwoPnt(vord45[0], vord45[4])
  ed45[7] = geompy.MakeLineTwoPnt(vord45[2], vord45[5])
  for i,l in enumerate(ed45):
    geompy.addToStudyInFather(sect45, l, "ed45_%d"%i)

  ed90 = [None for i in range(8)]
  ed90[0] = geompy.MakeLineTwoPnt(vord90[0], vord90[2])
  ed90[1] = geompy.MakeLineTwoPnt(vord90[0], vord90[1])
  ed90[2] = geompy.MakeLineTwoPnt(vord90[4], vord90[6])
  ed90[3] = geompy.MakeLineTwoPnt(vord90[2], vord90[3])
  ed90[4] = geompy.MakeLineTwoPnt(vord90[5], vord90[7])
  ed90[5] = geompy.MakeLineTwoPnt(vord90[4], vord90[5])
  ed90[6] = geompy.MakeLineTwoPnt(vord90[0], vord90[4])
  ed90[7] = geompy.MakeLineTwoPnt(vord90[2], vord90[5])
  for i,l in enumerate(ed90):
    geompy.addToStudyInFather(sect90, l, "ed90_%d"%i)

  faci = []
  faci.append(geompy.MakeFaceWires([ed45[6], edges[0], ed90[6], edges[4]], 0))
  faci.append(geompy.MakeFaceWires([ed45[7], edges[2], ed90[7], edges[5]], 0))
  faci.append(geompy.MakeFaceWires([ed45[2], edges[4], ed90[2], edges[6]], 0))
  faci.append(geompy.MakeFaceWires([ed45[5], edges[4], ed90[5], edges[5]], 0))
  faci.append(geompy.MakeFaceWires([ed45[4], edges[5], ed90[4], edges[7]], 0))
  faci.append(geompy.MakeFaceWires([ed90[0], ed90[6], ed90[5], ed90[7]], 0))
  faci.append(geompy.MakeFaceWires([ed90[1], ed90[6], ed90[2], lord90[0]], 0))
  faci.append(geompy.MakeFaceWires([ed90[2], ed90[5], ed90[4], lord90[1]], 0))
  faci.append(geompy.MakeFaceWires([ed90[3], ed90[7], ed90[4], lord90[2]], 0))
  for i,f in enumerate(faci):
    geompy.addToStudy(f, "faci_%d"%i)

  # --- extrusion droite des faces de jonction, pour reconstituer les demi cylindres

  extru1 = geompy.MakePrismVecH(sect45, OX, h1+10)
  geompy.addToStudy(extru1, "extru1")

  base2 = geompy.MakePartition(faci[5:], [], [], [], geompy.ShapeType["FACE"], 0, [], 0, True)
  extru2 = geompy.MakePrismVecH(base2, OZ, h2)
  geompy.addToStudy(extru2, "extru2")

  # --- partition et coupe

  demiDisque = geompy.MakeFaceWires([arc1, l1[0]], 1)
  demiCylindre = geompy.MakePrismVecH(demiDisque, OX, h1)
  #geompy.addToStudy(demiCylindre, "demiCylindre")
  box = geompy.MakeBox(0, -2*(r1+h1), -2*(r1+h1), 2*(r1+h1), 2*(r1+h1), 2*(r1+h1))
  rot = geompy.MakeRotation(box, OY, 45*math.pi/180.0)
  #geompy.addToStudy(rot, "rot")
  garder = geompy.MakeCutList(demiCylindre, [extru2, rot], True)
  geompy.addToStudy(garder, "garder")
  raccord = geompy.MakePartition([garder], faci, [], [], geompy.ShapeType["SOLID"], 0, [], 0, True)
  assemblage = geompy.MakePartition([raccord, extru1, extru2], [], [], [], geompy.ShapeType["SOLID"], 0, [], 0, True)
  geompy.addToStudy(assemblage, "assemblage")

  box = geompy.MakeBox(-1, -(r1+r2), -1, h1, r1+r2, h2)
  geompy.addToStudy(box, "box")
  final = geompy.MakeCommonList([box, assemblage], True)
  
  # --- Partie infiérieure
  v3, l3, arc3, part3 = demidisk(study, r1, a1, 180.0)
  geompy.addToStudy(part3,"part3")
  extru3 = geompy.MakePrismVecH(part3, OX, h1)
  geompy.addToStudy(extru3,"extru3")

  # --- Symétrie

  compound = geompy.MakeCompound([final, extru3])
  plane = geompy.MakePlane(O,OX,2000)
  compound_mirrored = geompy.MakeMirrorByPlane(compound, plane)
  final = geompy.MakeCompound([compound, compound_mirrored])
  
  return final