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

#==============================================================================
#  Info.
#  Bug (from script, bug)   : cyl2complementary_modified.py, PAL6700
#  Modified                 : 25/11/2004
#  Author                   : Kovaltchuk Alexey
#  Project                  : PAL/SALOME
#==============================================================================
# Import
# ------
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

geomgui = salome.ImportComponentGUI("GEOM") 
import salome_ComponentGUI
def addToStudy(shape, name):
    i = geompy.addToStudy(shape, name)
    salome.sg.updateObjBrowser(False)
    if not isinstance(geomgui, type(salome_ComponentGUI)):
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

p_element = geompy.MakePartition([t_boite], p_tools, [], [], 4, 0, [], 0)

# Compound
# --------

# Ajouter la piece dans l'etude
# -----------------------------

idpiece = addToStudy(p_element, "BoxHoled2Cylinders")
