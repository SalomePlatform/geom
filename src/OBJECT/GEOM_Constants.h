// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  File   : GEOM_Constants.h
//  Author : Margarita KARPUNINA
//  Module : GEOM
//
#ifndef GEOM_CONSTANTS_H
#define GEOM_CONSTANTS_H

// minimum allowed value for deflection coefficient
#define DEFLECTION_MIN 1e-06

//Define separators
#define NAME_SEPARATOR  '_' // character used to separate parameter names
#define DIGIT_SEPARATOR ':' // character used to separate numeric parameter values (color = r:g:b)

#define VISIBILITY_PROP       "Visibility"      //Object visibility property
#define OPACITY_PROP          "Opacity"         //Object opacity property
#define TRANSPARENCY_PROP     "Transparency"    //Object transparency property
#define DISPLAY_MODE_PROP     "DisplayMode"     //Object display mode property
#define ISOS_PROP             "Isos"            //Number of the Isos property of the object
#define COLOR_PROP            "Color"           //Color of the object 
#define VECTOR_MODE_PROP      "VectorMode"      //Vector mode property
#define DEFLECTION_COEFF_PROP "DeflectionCoeff" //Deflection coeff property
#define MARKER_TYPE_PROP      "MarkerType"      //Marker type property
#define FRONT_MATERIAL_PROP   "FrontMaterial"   //Object front material property
#define BACK_MATERIAL_PROP    "BackMaterial"    //Object back material property

#endif //GEOM_CONSTANTS_H
