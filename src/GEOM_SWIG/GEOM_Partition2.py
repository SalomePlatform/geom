#%Make geometry (like CEA script (A2)) using Partition algorithm%
# import callovo_01_GEOM
# reload(callovo_01_GEOM)


# --------------------------------------------
# Geometrie avec une galerie perpendiculaire
# --------------------------------------------

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

# -- bo

bo_xmin = 04.83
bo_xmax = colis_xmax
bo_radius = 1.23

bo = geompy.MakeCylinder(
    geom.MakePointStruct(bo_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    bo_radius,
    bo_xmax-bo_xmin)


# -- rupture alveole

alvRup_xmin = 04.46
alvRup_xmax = colis_xmax
alvRup_radius = 1.6

alvRup = geompy.MakeCylinder(
    geom.MakePointStruct(alvRup_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    alvRup_radius,
    alvRup_xmax-alvRup_xmin)


# bouchon

bouchon_xmin = colis_xmax
bouchon_xmax = 22.83
bouchon_radius = alvRup_radius

bouchon = geompy.MakeCylinder(
    geom.MakePointStruct(bouchon_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    bouchon_radius,
    bouchon_xmax-bouchon_xmin)

# galerie

galerie_xmax = 25.0
galerie_radius = 3.0
box_y = 30.0

galerie = geompy.MakeCylinder(
    geom.MakePointStruct(galerie_xmax,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(0.,1.,0.)),
    galerie_radius,
    box_y)
galerie = geompy.MakeTranslation(galerie, 0, -box_y/2, 0)

# -- box

box = geompy.MakeBox(0., 0., 0., galerie_xmax, box_y, box_y)
box = geompy.MakeTranslation(box, 0.0, -box_y/2, -box_y/2)


# -- rupture galerie

galRup_xmax = galerie_xmax
galRup_radius = 3.9

galRup = geompy.MakeCylinder(
    geom.MakePointStruct(galerie_xmax,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(0.,1.,0.)),
    galRup_radius,
    box_y)
galRup = geompy.MakeTranslation(galRup, 0, -box_y/2, 0)

# -- endommagement galerie

galEnd_xmax = galerie_xmax
galEnd_radius = 4.6

galEnd = geompy.MakeCylinder(
    geom.MakePointStruct(galerie_xmax,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(0.,1.,0.)),
    galEnd_radius,
    box_y)
galEnd = geompy.MakeTranslation(galEnd, 0, -box_y/2, 0)

# -- endommagement alveole

alvEnd_xmin = 03.6
alvEnd_xmax = galerie_xmax
alvEnd_radius = 2.46

alvEnd = geompy.MakeCylinder(
    geom.MakePointStruct(alvEnd_xmin,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(1.,0.,0.)),
    alvEnd_radius,
    alvEnd_xmax-alvEnd_xmin)

# remove internal parts of galEnd intersecting alvEnd

galEnd_alvEnd = geompy.Partition(
   [ alvEnd._get_Name() ] , [], [], [ galEnd._get_Name() ] )

iorL =  [ colis._get_Name()   ]
iorL += [ bo._get_Name()      ]
iorL += [ alvRup._get_Name()  ]
#iorL += [ galerie._get_Name() ]
iorL += [ galRup._get_Name()  ]
iorL += [ galEnd_alvEnd._get_Name() ]

# --

# remove section parts outside bo and inside bouchon

callovo = geompy.Partition( 
    iorL, [], [ box._get_Name() ], [galerie._get_Name(), bouchon._get_Name()] )



geompy.addToStudy(callovo, "callovo perp. 2")
