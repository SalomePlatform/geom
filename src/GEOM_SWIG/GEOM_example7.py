# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

# ---
#  File   : GEOM_example7.py
#  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
# ---
#
import os
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

data_dir = os.getenv('DATA_DIR')
if data_dir:
    texture_1 = geompy.LoadTexture(os.path.join(data_dir, "Textures", "texture1.dat"))
    texture_2 = geompy.LoadTexture(os.path.join(data_dir, "Textures", "texture2.dat"))
    texture_3 = geompy.LoadTexture(os.path.join(data_dir, "Textures", "texture3.dat"))
    
    Vertex_1 = geompy.MakeVertex(0, 0, 0)
    Vertex_2 = geompy.MakeVertex(100, 0, 0)
    Vertex_3 = geompy.MakeVertex(0, 100, 0)
    Vertex_4 = geompy.MakeVertex(0, 0, 100)
    Vertex_5 = geompy.MakeVertex(100, 0, 100)
    Vertex_1.SetMarkerTexture(texture_1);
    Vertex_2.SetMarkerTexture(texture_2);
    Vertex_3.SetMarkerTexture(texture_3);
    Vertex_4.SetMarkerStd(GEOM.MT_O_PLUS, GEOM.MS_25);
    Vertex_5.SetMarkerStd(GEOM.MT_BALL,   GEOM.MS_40);
    geompy.addToStudy( Vertex_1, "Vertex_1" )
    geompy.addToStudy( Vertex_2, "Vertex_2" )
    geompy.addToStudy( Vertex_3, "Vertex_3" )
    geompy.addToStudy( Vertex_4, "Vertex_4" )
    geompy.addToStudy( Vertex_5, "Vertex_5" )
    pass
