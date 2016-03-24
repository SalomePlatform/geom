# Copyright (C) 2014-2016  EDF R&D
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

IF(SALOME_BUILD_GUI)
  INCLUDE(UsePyQt)
ENDIF()

# --- scripts ---

# base scripts
SET(base_SCRIPTS
  __init__.py
  t_shape_builder.py
  )

IF(SALOME_BUILD_GUI)
   # gui scripts 
   SET(gui_SCRIPTS
     t_shape_dialog.py
     t_shape_dialog.ui
     t_shape_progress.py
     )

  # uic files / to be processed by pyuic
  SET(_pyuic_files
    t_shape_dialog.ui
    )

  # scripts / pyuic wrappings
  PYQT_WRAP_UIC(_pyuic_SCRIPTS ${_pyuic_files})
ENDIF()

# --- rules ---

SALOME_INSTALL_SCRIPTS("${base_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/geom/t_shape)

IF(SALOME_BUILD_GUI)
  SALOME_INSTALL_SCRIPTS("${gui_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/geom/t_shape)
  SALOME_INSTALL_SCRIPTS("${_pyuic_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/geom/t_shape)
ENDIF()