#%Make geometry (like CEA script (A2)) using Partition algorithm%
# import callovo_01_GEOM
# reload(callovo_01_GEOM)


import geompy
geom = geompy.geom

# -- colis

colis_xmin = 05.76
colis_xmax = 19.83
colis_radius = 0.3

colis = geompy.MakeCylinder(
    geom.MakePointStruct(colis_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    colis_radius,
    colis_xmax-colis_xmin)

iorL = [colis._get_Name()]

# -- bo

bo_xmin = 04.83
bo_xmax = colis_xmax
bo_radius = 1.23

bo = geompy.MakeCylinder(
    geom.MakePointStruct(bo_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    bo_radius,
    bo_xmax-bo_xmin)

iorL.append(bo._get_Name())

# -- rupture alveole

alvRup_xmin = 04.46
alvRup_xmax = colis_xmax
alvRup_radius = 1.6

alvRup = geompy.MakeCylinder(
    geom.MakePointStruct(alvRup_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    alvRup_radius,
    alvRup_xmax-alvRup_xmin)

iorL.append(alvRup._get_Name())

# bouchon

bouchon_xmin = colis_xmax
bouchon_xmax = 22.83
bouchon_radius = alvRup_radius

bouchon = geompy.MakeCylinder(
    geom.MakePointStruct(bouchon_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    bouchon_radius,
    bouchon_xmax-bouchon_xmin)

#iorL.append(bouchon._get_Name())

# -- endommagement alveole

alvEnd_xmin = 03.6
alvEnd_xmax = 20.66
alvEnd_radius = 2.46

alvEnd = geompy.MakeCylinder(
    geom.MakePointStruct(alvEnd_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    alvEnd_radius,
    alvEnd_xmax-alvEnd_xmin)

iorL.append(alvEnd._get_Name())

# galerie

galerie_xmin = bouchon_xmax
galerie_xmax = 25.0
galerie_radius = 2.17

galerie = geompy.MakeCylinder(
    geom.MakePointStruct(galerie_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    galerie_radius,
    galerie_xmax-galerie_xmin)

iorL.append(galerie._get_Name())

# -- rupture galerie

galRup_xmin = 22.18
galRup_xmax = galerie_xmax
galRup_radius = 2.82

galRup = geompy.MakeCylinder(
    geom.MakePointStruct(galRup_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    galRup_radius,
    galRup_xmax-galRup_xmin)

iorL.append(galRup._get_Name())

# -- endom. galerie

galEnd_xmin = alvEnd_xmax
galEnd_xmax = galerie_xmax
galEnd_radius = 4.34

galEnd = geompy.MakeCylinder(
    geom.MakePointStruct(galEnd_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    galEnd_radius,
    galEnd_xmax-galEnd_xmin)

iorL.append(galEnd._get_Name())

# --

box_y = 30.0
box = geompy.MakeBox(0., 0., 0., galerie_xmax, box_y, box_y)
box = geompy.MakeTranslation(box, 0.0, -box_y/2, -box_y/2)

iorL.append(box._get_Name())

# --

callovo = geompy.Partition(iorL,[],[], [bouchon._get_Name()])
geompy.addToStudy(callovo, "callovo 2")
