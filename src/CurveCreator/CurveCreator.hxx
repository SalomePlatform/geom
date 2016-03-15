// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

// File:        CurveCreator.hxx
// Author:      Sergey KHROMOV

#ifndef _CurveCreator_HeaderFile
#define _CurveCreator_HeaderFile

#include <deque>
#include <map>
#include <list>

struct CurveCreator_Section;
struct CurveCreator_PosPoint;

namespace CurveCreator
{
  //! Points coordinates
  typedef double TypeCoord;

  /** List of coordinates in format depends on section dimension:
   *  2D: [x1, y1,     x2, y2,     x3, y3,     ..]
   *  3D: [x1, y1, z1, x2, y2, z2, x3, y3, z3, ..]
   */
  typedef std::deque<TypeCoord> Coordinates;

  //! List of sections
  typedef std::deque<CurveCreator_Section *> Sections;

  // List of positioned points (points with coordinates)
  typedef std::list<CurveCreator_PosPoint*> PosPointsList;
  //! Map of sections with positioned points
  typedef std::map<int,PosPointsList> SectionsMap;
};

#endif
