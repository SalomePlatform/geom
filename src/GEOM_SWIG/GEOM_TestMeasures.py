def TestMeasureOperations (geompy, math):

  p0   = geompy.MakeVertex(0 ,  0,  0)
  p137 = geompy.MakeVertex(10, 30, 70)

  box = geompy.MakeBoxTwoPnt(p0, p137)

  p678 = geompy.MakeVertex(60, 70, 80)
  p789 = geompy.MakeVertex(70, 80, 90)

  cube = geompy.MakeBoxTwoPnt(p678, p789)

  ####### PointCoordinates #######

  Coords = geompy.PointCoordinates(p137)
  if Coords[0] != 10 or Coords[1] != 30 or Coords[2] != 70:
    print "Coordinates of p137 must be (10, 30, 70), but returned (", Coords[0], ", ", Coords[1], ", ", Coords[2], ")"

  ####### CheckShape #######

  IsValid = geompy.CheckShape(box)
  if IsValid == 0:
    raise RuntimeError, "Invalid box created"
  else:
    print "\nBox is valid"

  ####### WhatIs #######

  Descr = geompy.WhatIs(box)
  print "\nBox 10x30x70 description:"
  print Descr

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

  ####### MinDistance #######

  MinDist = geompy.MinDistance(box, cube)

  #print "\nMinimal distance between Box and Cube = ", MinDist[0]
  #print "It is reached at points:"
  #print " On Box  (", MinDist[1], ", ", MinDist[2], ", ", MinDist[3], ")"
  #print " On Cube (", MinDist[4], ", ", MinDist[5], ", ", MinDist[6], ")"

  print "\nMinimal distance between Box and Cube = ", MinDist
