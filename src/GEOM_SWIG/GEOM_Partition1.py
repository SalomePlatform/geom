#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#
#  Copyright (C) 2003  CEA/DEN, EDF R&D
#
#
#
#  File   : GEOM_Partition1.py
#  Module : GEOM

#%Make geometry (like CEA script (A1)) using Partition algorithm%
# appel: 
# import alveole_3D_01_GEOM
# reload(alveole_3D_01_GEOM)

# -- Rayon de la bariere

barier_height = 7.0
barier_radius = 5.6 / 2 # Rayon de la bariere
colis_radius = 1.0 / 2  # Rayon du colis
colis_step = 2.0        # Distance s‰parant deux colis
cc_width = 0.11         # Epaisseur du complement de colisage

# --

cc_radius = colis_radius + cc_width
from math import sqrt
colis_center = sqrt(2.0)*colis_step/2

# --

import geompy
geom = geompy.geom

boolean_common  = 1
boolean_cut     = 2
boolean_fuse    = 3
boolean_section = 4

# --

barier = geompy.MakeCylinder(
    geom.MakePointStruct(0.,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(0.,0.,1.)),
    barier_radius,
    barier_height)

# --

colis = geompy.MakeCylinder(
    geom.MakePointStruct(0.,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(0.,0.,1.)),
    colis_radius,
    barier_height)

cc = geompy.MakeCylinder(
    geom.MakePointStruct(0.,0.,0.),
    geom.MakeDirection(geom.MakePointStruct(0.,0.,1.)),
    cc_radius,
    barier_height)

colis_cc = geompy.MakeCompound(
    [colis._get_Name(), cc._get_Name()])

colis_cc = geompy.MakeTranslation(
    colis_cc, colis_center, 0.0, 0.0)

colis_cc_multi = geompy.MakeMultiRotation1D(
    colis_cc,
    geom.MakeDirection(geom.MakePointStruct(0.,0.,1.)),
    geom.MakePointStruct(0.,0.,0.),
    4)

# --

alveole = geompy.Partition(
    [colis_cc_multi._get_Name(), barier._get_Name()])

subshapes = geompy.SubShapeAll( alveole, geompy.ShapeType["SHAPE"] )

## there are 9 subshapes

comp1 = geompy.MakeCompound( [ subshapes[0]._get_Name(), subshapes[1]._get_Name() ] );
comp2 = geompy.MakeCompound( [ subshapes[2]._get_Name(), subshapes[3]._get_Name() ] );
comp3 = geompy.MakeCompound( [ subshapes[4]._get_Name(), subshapes[5]._get_Name() ] );
comp4 = geompy.MakeCompound( [ subshapes[6]._get_Name(), subshapes[7]._get_Name() ] );

compIORs = []
compIORs.append( comp1._get_Name() );
compIORs.append( comp2._get_Name() );
compIORs.append( comp3._get_Name() );
compIORs.append( comp4._get_Name() );
comp = geompy.MakeCompound( compIORs );

alveole = geompy.MakeCompound( [ comp._get_Name(), subshapes[8]._get_Name() ]);
	
geompy.addToStudy(alveole, "alveole")


