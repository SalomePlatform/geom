# Copyright (C) 2012-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

# --- scripts ---

# scripts / samples,data
SET(_other_SCRIPTS
  geompy.py
  GEOM_Spanner.py
  GEOM_blocks.py
  GEOM_example.py
  GEOM_example2.py
  GEOM_example3.py
  GEOM_example4.py
  GEOM_example5.py
  GEOM_example6.py
  GEOM_example7.py
  GEOM_moteur.py
  GEOM_TestAll.py
  GEOM_TestField.py
  GEOM_TestOthers.py
  GEOM_TestHealing.py
  GEOM_TestMeasures.py
  GEOM_usinggeom.py
  GEOM_Partition1.py
  GEOM_Partition2.py
  GEOM_Partition3.py
  GEOM_Partition4.py
  GEOM_Partition5.py
  GEOM_cyl2complementary.py
  GEOM_shellSolid.py
  GEOM_tube_geom.py
  GEOM_tube_geom_gg2.py
  GEOM_Nut.py
  GEOM_Sketcher.py
  GEOM_ObjectInfo.py
  PAL_MESH_019_020_geometry.py
  PAL_MESH_028_geometry.py
  PAL_MESH_030_geometry.py
  PAL_MESH_033_geometry.py
  PAL_MESH_035_geometry.py
  )

# scripts / python

SET(_python_SCRIPTS
  geomBuilder.py
  gsketcher.py
  )

# Advanced scripts

SET(_python_advanced_SCRIPTS
  AdvancedGEOMBuilder.py
  __init__.py
)

# STL plugin scripts

SET(_python_STL_SCRIPTS
  STLPluginBuilder.py
  __init__.py
)

# BREP plugin scripts

SET(_python_BREP_SCRIPTS
  BREPPluginBuilder.py
  __init__.py
)

# STEP plugin scripts

SET(_python_STEP_SCRIPTS
  STEPPluginBuilder.py
  __init__.py
)

# IGES plugin scripts

SET(_python_IGES_SCRIPTS
  IGESPluginBuilder.py
  __init__.py
)

# XAO plugin scripts

SET(_python_XAO_SCRIPTS
  XAOPluginBuilder.py
  __init__.py
)

# VTK plugin scripts

SET(_python_VTK_SCRIPTS
  VTKPluginBuilder.py
  __init__.py
)

# scripts / shared

SET(_shared_SCRIPTS
  GEOM_shared_modules.py
  )

# --- rules ---

SALOME_INSTALL_SCRIPTS("${_other_SCRIPTS}" ${SALOME_INSTALL_SCRIPT_DATA} DEF_PERMS)
SALOME_INSTALL_SCRIPTS("${_shared_SCRIPTS}" ${SALOME_INSTALL_PYTHON_SHARED} DEF_PERMS)
SALOME_INSTALL_SCRIPTS("${_python_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/geom DEF_PERMS)

# Install plugin scripts
SALOME_INSTALL_SCRIPTS("${_python_advanced_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/AdvancedGEOM DEF_PERMS)
SALOME_INSTALL_SCRIPTS("${_python_STL_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/STLPlugin DEF_PERMS)
SALOME_INSTALL_SCRIPTS("${_python_BREP_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/BREPPlugin DEF_PERMS)
SALOME_INSTALL_SCRIPTS("${_python_STEP_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/STEPPlugin DEF_PERMS)
SALOME_INSTALL_SCRIPTS("${_python_IGES_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/IGESPlugin DEF_PERMS)
SALOME_INSTALL_SCRIPTS("${_python_XAO_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/XAOPlugin DEF_PERMS)
IF(SALOME_GEOM_USE_VTK)
  SALOME_INSTALL_SCRIPTS("${_python_VTK_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/VTKPlugin DEF_PERMS)
ENDIF(SALOME_GEOM_USE_VTK)
