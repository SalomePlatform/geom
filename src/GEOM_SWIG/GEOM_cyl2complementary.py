#==============================================================================
#  Info.
#  Bug (from script, bug)   : cyl2complementary_modified.py, PAL6700
#  Modified                 : 25/11/2004
#  Author                   : Kovaltchuk Alexey
#  Project                  : PAL/SALOME
#==============================================================================

# Import
# ------

import salome
import geompy
geomgui = salome.ImportComponentGUI("GEOM") 

def addToStudy(shape, name):
    i = geompy.addToStudy(shape, name)
    salome.sg.updateObjBrowser(0)
    geomgui.createAndDisplayGO(i)
    return i

# Piece
# -----

# Creer la geometrie en bloc hexahedrique d'un parallelepipede troue d'une piece en forme de T composee de 2 cylindres de diametre different dont les axes se coupent orthogonalement,
# puis mailler en hexahedrique.

gx = 0
gy = 0
gz = 0

g_dx = 250
g_dy = 200
g_dz = 150

g_rayonGrand = 70
g_rayonPetit = 50

# Geometrie
# =========

# Le parallelepipede
# ------------------

p_boite = geompy.MakeBox(gx-g_dx, gy-g_dy, gz-g_dz,  gx+g_dx, gy+g_dy, gz+g_dz)

# Le grand cylindre
# -----------------

g_base = geompy.MakeVertex(gx-g_dx, gy, gz)
g_dir  = geompy.MakeVectorDXDYDZ(1, 0, 0)
g_cyl  = geompy.MakeCylinder(g_base, g_dir, g_rayonGrand, g_dx*2)

# Le petit cylindre
# -----------------

c_base = geompy.MakeVertex(gx, gy, gz)
c_dir  = geompy.MakeVectorDXDYDZ(0, 0, 1)
c_cyl  = geompy.MakeCylinder(c_base, c_dir, g_rayonPetit, g_dz)

# Le parallelepipede troue
# ------------------------

t_boite1 = geompy.MakeBoolean(p_boite , g_cyl, 2)
t_boite  = geompy.MakeBoolean(t_boite1, c_cyl, 2)

# Partitionner
# ------------

p_tools = []

p_tools.append(geompy.MakePlane(c_base, geompy.MakeVectorDXDYDZ(0,  g_dz, g_dy), 10))
p_tools.append(geompy.MakePlane(c_base, geompy.MakeVectorDXDYDZ(0, -g_dz, g_dy), 10))

p_tools.append(geompy.MakePlane(geompy.MakeVertex(gx-g_rayonPetit, 0, 0), geompy.MakeVectorDXDYDZ(1, 0, 0), 10))
p_tools.append(geompy.MakePlane(geompy.MakeVertex(gx+g_rayonPetit, 0, 0), geompy.MakeVectorDXDYDZ(1, 0, 0), 10))

addToStudy(t_boite, "t_boite")

p_element = geompy.MakePartition([t_boite], p_tools, [], [], 4, 0, [])

# Compound
# --------

# Ajouter la piece dans l'etude
# -----------------------------

idpiece = addToStudy(p_element, "BoxHoled2Cylinders")
