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
#  File   : GEOM_Sketcher.py
#  Author : Damien COQUERET, Open CASCADE
#  Module : GEOM
#
import salome
salome.salome_init()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

#SKETCHER INFOS
#Init Sketcher
#Create a string beginning by :"Sketcher:"
#Each command must be separated by ":"
#"F x y" : Create first point at X & Y

#To Make Segment
#"R angle" : Set the direction by angle
#"D dx dy" : Set the direction by DX & DY

#"TT x y" : Create by point at X & Y
#"T dx dy" : Create by point with DX & DY
#"L length" : Create by direction & Length
#"IX x" : Create by direction & Intersect. X
#"IY y" : Create by direction & Intersect. Y

#To Make Arc
#"C radius length" : Create by direction, radius and length(in degree)
#"AA x y": Create by point at X & Y
#"A dx dy" : Create by point with DX & DY
#"UU x y radius flag1": Create by point at X & Y with given radiUs 
#"U dx dy radius flag1" : Create by point with DX & DY with given radiUs
#"EE x y xc yc flag1 flag2": Create by point at X & Y with given cEnter 
#"E dx dy dxc dyc radius flag1 flag2" : Create by point with DX & DY with given cEnter

#Flag1 is 0 or 2 ... 
# if 0 the drawn arc is the one of lower angle (< Pi)
# if 2 the drawn arc ius the one of greater angle (> Pi)

#Flag2 is 0 or 1 ...
# if 0 the specified end point can be at a distance of the arc greater than the tolerance (10^-7)
# if 1 the wire is built only if the end point is on the arc 
#   with a tolerance of 10^-7 on the distance else the creation fails

#To finish
#"WW" : Close Wire

#Create Sketcher
Cmd = "Sketch:F 0 0:TT 0 100:C 100 180:AA -100 100:WW"
Sketcher = geompy.MakeSketcher(Cmd) #(string)->GEOM_Shape_ptr

#Add In Study
id_Sketcher = geompy.addToStudy(Sketcher, "Sketcher")
