#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#
#  Copyright (C) 2003  CEA/DEN, EDF R&D
#
#
#
#  File   : GEOM_Partition5.py
#  Module : GEOM

#%Make geometry (like CEA script (A4)) using Partition algorithm%
# import gallery_01_GEOM
# reload(gallery_01_GEOM)

# -- Import geompy pour piloter GEOM par script

import geompy
geom = geompy.geom

# -- Dimensions de la boite entiˆre

LX, LY, LZ = 300.0, 150.0, 150.0

# -- D‰finition du plan de sym‰trie (O,Ox,Oz)

symPlane = geompy.MakePlane (
    geom.MakePointStruct(0.,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(0.,1.,0.)),
    10.0 )

# -- bo

bo = geompy.MakeBox(0.0,0.0,0.0,LX,LY,LZ)
bo = geompy.MakeTranslation(bo,0.0,-LY/2,-LZ/2)

# -- Galerie principale

gal_diam = 80.0
gal_lony = LY
gal_x = LX-20.0-gal_diam/2
gal = geompy.MakeCylinder(
    geom.MakePointStruct(0.,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(0.,1.,0.)),
    gal_diam/2,gal_lony)
gal = geompy.MakeTranslation(gal,gal_x,-gal_lony/2,0.0)

# -- Galerie perpendiculaire
# -- Dimensions de la galerie perpendiculaire
# -- La longueur est compt‰e € partir du centre
# -- de la galerie principale

gpe_long, gpe_diam = 200.0, 60.0 
gpe_x = gal_x
gpe = geompy.MakeCylinder(
    geom.MakePointStruct(0.,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(-1.,0.,0.)),
    gpe_diam/2,gpe_long)
gpe = geompy.MakeTranslation(gpe,gpe_x,0,0)

# -- Dimensions d'une alveole
# -- Construction d'une alv‰ole

alv_long, alv_diam = 60.0, 18.0
alv01 = geompy.MakeCylinder(
    geom.MakePointStruct(0.,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(0.,1.,0.)),
    alv_diam/2,alv_long)
alv02 = geompy.MakeMirrorByPlane(alv01,symPlane)
alv0 = geompy.MakeCompound( [ alv01._get_Name(), alv02._get_Name() ] )

# -- Construction des alv‰oles

alv_del, alv_sep = 40.0, 35.0
alv1 = geompy.MakeTranslation (
    alv0,gal_x-gal_diam/2-alv_sep,0.0,0.0 )
alv2 = geompy.MakeTranslation (
    alv1,-alv_del,0.0,0.0 )
alv3 = geompy.MakeTranslation (
    alv2,-alv_del,0.0,0.0 )
alv = geompy.MakeCompound( [ alv1._get_Name(), alv2._get_Name(), alv3._get_Name() ] )

# -- Remplissage de la BO

gallery = geompy.Partition(
    [ alv._get_Name(), bo._get_Name() ], [], [], [ gal._get_Name(), gpe._get_Name() ])

subshapes = geompy.SubShapeAll( gallery, geompy.ShapeType["SHAPE"] )

alvIORs = []
alvIORs.append( subshapes[0]._get_Name() )
alvIORs.append( subshapes[1]._get_Name() )
alvIORs.append( subshapes[2]._get_Name() )
alvIORs.append( subshapes[3]._get_Name() )
alvIORs.append( subshapes[4]._get_Name() )
alvIORs.append( subshapes[5]._get_Name() )
alvcomp = geompy.MakeCompound( alvIORs )

boIOR  = subshapes[6]._get_Name()
galIOR = subshapes[7]._get_Name()
gpeIOR = subshapes[8]._get_Name()

gallery = geompy.MakeCompound( [ boIOR, alvcomp._get_Name(), gpeIOR, galIOR ] )

geompy.addToStudy(gallery,"Gallery 2")
