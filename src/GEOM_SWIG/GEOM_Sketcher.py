#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
# 
#  This library is free software; you can redistribute it and/or 
#  modify it under the terms of the GNU Lesser General Public 
#  License as published by the Free Software Foundation; either 
#  version 2.1 of the License. 
# 
#  This library is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
#  Lesser General Public License for more details. 
# 
#  You should have received a copy of the GNU Lesser General Public 
#  License along with this library; if not, write to the Free Software 
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
# 
#  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
#
#
#
#  File   : GEOM_Sketcher.py
#  Author : Damien COQUERET, Open CASCADE
#  Module : GEOM
#  $Header$

import geompy

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

#To finish
#"WW" : Close Wire

#Create Sketcher
Cmd = "Sketch:F 0 0:TT 0 100:C 100 180:WW"
Sketcher = geompy.MakeSketcher(Cmd) #(string)->GEOM_Shape_ptr

#Add In Study
id_Sketcher = geompy.addToStudy(Sketcher, "Sketcher")
