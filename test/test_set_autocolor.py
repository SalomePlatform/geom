#!/usr/bin/env python3
# Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
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

# Testing of setting and getting an automatically set color

from salome.kernel import salome
salome.salome_init()

from salome.geom import geomBuilder
geompy = geomBuilder.New()

# Create a box and extract its faces
Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
faces = geompy.ExtractShapes(Box_1, geompy.ShapeType["FACE"], True)
Box_1.SetAutoColor(1)
geompy.addToStudy(Box_1, 'Box_1')

# Add each face to the study with a numeric name
for i, face in enumerate(faces, start=1):
    geompy.addToStudyInFather(Box_1, face, f'Face_{i}')

# Check color of each face
for face in faces:
    color = face.GetColor()
    print(f'{face.GetName()}: {color}')
    assert color.R != -1 and color.G != -1 and color.B != -1, 'Auto color must be different than (-1, -1, -1)'