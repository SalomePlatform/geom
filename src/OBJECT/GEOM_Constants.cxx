// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  File   : GEOM_Constants.cxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "GEOM_Constants.h"

namespace GEOM
{

  /*!
    \brief Get minimal allowed value for deflection coefficient
  */
  double minDeflection()
  {
    return 1e-06; // DEFLECTION_MIN
  }

  /*!
    \brief Get character used to separate parameter names
  */
  QString sectionSeparator()
  {
    return "_"; // NAME_SEPARATOR
  }

  /*!
    \brief Get character used to separate numeric parameter values (color = r:g:b)
  */
  QString subSectionSeparator()
  {
    return ":"; // DIGIT_SEPARATOR
  }
  
  QString propertyName( Property type )
  {
    static const char* names[] = {
      // visibility 
      "Visibility",      // VISIBILITY_PROP
      // transparency
      "Transparency",    // TRANSPARENCY_PROP
      // display mode
      "DisplayMode",     // DISPLAY_MODE_PROP
      // number of the iso lines (along u and v directions)
      "Isos",            // ISOS_PROP
      // main / shading color
      "Color",           // COLOR_PROP
      // "show edges direction" flag
      "VectorMode",      // VECTOR_MODE_PROP
      // deflection coefficient
      "DeflectionCoeff", // DEFLECTION_COEFF_PROP
      // point marker data
      "MarkerType",      // MARKER_TYPE_PROP
      // material
      "Material",        // MATERIAL_PROP
      // general line width
      "EdgeWidth",       // EDGE_WIDTH_PROP
      // isos width
      "IsosWidth",       // ISOS_WIDTH_PROP
      // top-level flag
      "TopLevelFlag",    // TOP_LEVEL_PROP
      // opacity
      "Opacity",         // OPACITY_PROP
      // shading color
      "ShadingColor",    // -
      // wireframe color
      "WireframeColor",  // -
      // standalone edges color
      "LineColor",       // -
      // free boundaries color
      "FreeBndColor",    // -
      // points color
      "PointColor",      // -
      // isos color
      "IsosColor",       // -
      // outlines color
      "OutlineColor",    // -
    };
    return ( type >= GEOM::Visibility && type <= GEOM::LastProperty ) ? names[type] : QString();
  }

} // namespace GEOM
