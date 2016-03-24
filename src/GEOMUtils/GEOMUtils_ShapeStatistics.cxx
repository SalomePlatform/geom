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

// File   : GEOMUtils_ShapeStatisticsDlg.cxx
// Author : Alexander KOVALEV, OPEN CASCADE S.A.S.

#include "GEOMUtils_ShapeStatistics.hxx"

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

namespace GEOMUtils
{
//=================================================================================
// function : ComputeMeasures()
// purpose  : gets measures of the given type for list of shapes in the range
//=================================================================================
  std::map<int,double> ComputeMeasures( std::list<TopoDS_Shape> shapes, 
			      TopAbs_ShapeEnum entity, 
			      Range &range)
{
  bool hasRange = (range.min != -1.0); // -1.0 means that range must not be used
  if ( !hasRange )
    range.min = 1e+32, range.max = 0.0;
  // list of measures of entities
  std::map<int, double> measures;
    
  std::list<TopoDS_Shape>::const_iterator it;
  int shift = 0;
  for ( it = shapes.begin(); it != shapes.end(); ++it ) {
    double aMeasure;
    TopTools_IndexedMapOfShape aSubShapesMap;
    TopExp::MapShapes(*it, aSubShapesMap); // map of all global indices
    TopTools_IndexedMapOfShape aMx;
    TopExp::MapShapes( *it, entity, aMx ); // map of current type sub-shape indices 
    int aNbS = aMx.Extent();
    int index = -1;
    for ( int i = 1; i <= aNbS; ++i ) {
      aMeasure = 0.0;
      const TopoDS_Shape& aSubShape = aMx( i );
      //Get the measure: length, area or volume
      GProp_GProps LProps, SProps, VProps;
      if ( entity == TopAbs_EDGE ) {
	BRepGProp::LinearProperties( aSubShape, LProps );
	aMeasure = LProps.Mass();
      } else if ( entity == TopAbs_FACE ) {
	BRepGProp::SurfaceProperties( aSubShape, SProps );
	aMeasure = SProps.Mass();
      } else if ( entity == TopAbs_SOLID ) {
	BRepGProp::VolumeProperties( aSubShape, VProps );
	aMeasure = VProps.Mass();
      }
      // Don't pass sub-shapes with out of range measure, if range is used
      if ( hasRange ) {
	if ( aMeasure < range.min || aMeasure > range.max )
	  continue;
      } else {
	// get range min and max
	if ( aMeasure < range.min ) range.min = aMeasure;
	if ( aMeasure > range.max ) range.max = aMeasure;
      }
      // get global index of sub-shape
      index = aSubShapesMap.FindIndex( aSubShape );
      // keep measures to distribute it
      measures[shift+index] = aMeasure;
    }
    shift += aSubShapesMap.Extent();
  }
  return measures;
}

//=================================================================================
// function : ComputeDistribution()
// purpose  : gets distribution data for single shape
//=================================================================================
Distribution ComputeDistribution( TopoDS_Shape shape, 
				  TopAbs_ShapeEnum entity, 
				  int intervals, 
				  Range range)
{
  std::list<TopoDS_Shape> aShapes;
  aShapes.push_back( shape );
  return ComputeDistribution( aShapes, entity, intervals, range );
}

//=================================================================================
// function : ComputeDistribution()
// purpose  : gets distribution data for list of shapes
//=================================================================================
Distribution ComputeDistribution( std::list<TopoDS_Shape> shapes, 
				  TopAbs_ShapeEnum entity, 
				  int nbIntervals, 
				  Range range)
{
  // get list of measures and compute range (if it was not specified)
  std::map<int,double> measures = ComputeMeasures( shapes, entity, range );

  // compute a step
  double aStep = (range.max - range.min) / nbIntervals;

  // compute distribution in intervals
  Distribution aDistr;
  std::map<int,double>::iterator dit;
  for ( int i = 0; i < nbIntervals; i++ ) {
    Range localRange; // range of current interval
    localRange.min = range.min + ( i * aStep );
    localRange.max = range.min + ( (i+1) * aStep );
    localRange.count = 0;

    std::vector<int> indicesToErase;
    for ( dit = measures.begin(); dit != measures.end(); dit++ ) {
      if ( ( dit->second >= localRange.min && dit->second < localRange.max ) || 
	   ( i == nbIntervals-1 && dit->second == localRange.max ) ) {
	localRange.count++;
	localRange.indices.push_back( dit->first );
	// measure is in interval, so remove it from map of search
	indicesToErase.push_back( dit->first );
      }
    }
    aDistr.push_back( localRange );
    for( int j=0; j < indicesToErase.size(); j++ )
      measures.erase( indicesToErase[j] );
  }

  return aDistr;
}

} //namespace GEOMUtils
