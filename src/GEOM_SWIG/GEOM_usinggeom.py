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

#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#  File   : GEOM_usinggeom.py
#  Author : Damien COQUERET, Open CASCADE
#  Module : GEOM
#
import salome
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

import math

import GEOM_TestAll
import GEOM_TestOthers
import GEOM_TestHealing
import GEOM_TestMeasures
import GEOM_TestField

GEOM_TestField.TestField(geompy, math) # it goes 1st as it checks Python Dump
GEOM_TestMeasures.TestMeasureOperations(geompy, math)
GEOM_TestHealing.TestHealingOperations(geompy, math)
GEOM_TestOthers.TestOtherOperations(geompy, math)
GEOM_TestAll.TestAll(geompy, math)
