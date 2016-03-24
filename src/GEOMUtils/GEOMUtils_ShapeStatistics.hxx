// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : GEOMUtils_ShapeStatisticsDlg.hxx
// Author : Alexander KOVALEV, OPEN CASCADE S.A.S.

#ifndef _GEOMUtils_ShapeStatistics_HXX_
#define _GEOMUtils_ShapeStatistics_HXX_

#include <list>
#include <map>
#include <vector>

#include <TopoDS_Shape.hxx>
 
namespace GEOMUtils
{
  // struct to store range data
  typedef struct { double min; double max; long count; std::list<long> indices; } Range;
  // distribution is a set of ranges
  typedef std::vector<Range> Distribution;

  // function to get measures of entities and compute range for list of shapes
  Standard_EXPORT std::map<int,double> ComputeMeasures(
    std::list<TopoDS_Shape> shapes, 
    TopAbs_ShapeEnum entity, 
    Range &range );

  // function to get distribution data for single shape
  Standard_EXPORT Distribution ComputeDistribution(
    TopoDS_Shape shape, 
    TopAbs_ShapeEnum entity, 
    int intervals, 
    Range range );

  // function to get distribution data for list of shapes
  Standard_EXPORT Distribution ComputeDistribution(
    std::list<TopoDS_Shape> shapes, 
    TopAbs_ShapeEnum entity, 
    int intervals, 
    Range range );

}

#endif // _GEOMUtils_ShapeStatistics_HXX_
