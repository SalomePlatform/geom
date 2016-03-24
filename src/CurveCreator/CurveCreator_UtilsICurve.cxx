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

#include "CurveCreator_UtilsICurve.hxx"

#include "CurveCreator.hxx"
#include <gp_Pnt.hxx>

const double LOCAL_SELECTION_TOLERANCE = 0.0001;

int CurveCreator_UtilsICurve::findLocalPointIndex( const CurveCreator_ICurve* theCurve,
                                             int theSectionId, double theX, double theY )
{
  int aPntIndex = -1;
  if ( !theCurve )
    return aPntIndex;

  CurveCreator::Coordinates aCoords;
  for ( int i = 0, aNb = theCurve->getNbPoints( theSectionId ); i < aNb && aPntIndex < 0; i++ ) {
    aCoords = theCurve->getPoint( theSectionId, i );
    if ( aCoords.size() < 2 )
      continue;
    if ( fabs( aCoords[0] - theX ) < LOCAL_SELECTION_TOLERANCE &&
         fabs( aCoords[1] - theY ) < LOCAL_SELECTION_TOLERANCE )
      aPntIndex = i;
  }

  return aPntIndex;
}

void CurveCreator_UtilsICurve::findSectionsToPoints( const CurveCreator_ICurve* theCurve,
                                 const double theX, const double theY,
                                 CurveCreator_ICurve::SectionToPointList& thePoints )
{
  thePoints.clear();

  int aPointId = -1;
  for ( int i = 0, aNb = theCurve->getNbSections(); i < aNb; i++ ) {
    aPointId = CurveCreator_UtilsICurve::findLocalPointIndex( theCurve, i, theX, theY );
    if ( aPointId < 0 )
      continue;
    CurveCreator_ICurve::SectionToPoint aPoint = std::make_pair( i, aPointId );
    if ( !CurveCreator_UtilsICurve::contains( thePoints, aPoint ) )
      thePoints.push_back( aPoint );
  }
}

void CurveCreator_UtilsICurve::convert( const CurveCreator_ICurve::SectionToPointList& thePoints,
                                  QMap<int, QList<int> >& theConvPoints )
{
  theConvPoints.clear();

  CurveCreator_ICurve::SectionToPointList::const_iterator anIt = thePoints.begin(),
                                                          aLast = thePoints.end();
  QList<int> aPoints;
  int aSectionId, aPointId;
  for ( ; anIt != aLast; anIt++ ) {
    aSectionId = anIt->first;
    aPointId = anIt->second;
    aPoints.clear();
    if ( theConvPoints.contains( aSectionId ) )
      aPoints = theConvPoints[aSectionId];
    if ( aPoints.contains( aPointId ) )
      continue;
    aPoints.append( aPointId );
    theConvPoints[aSectionId] = aPoints;
  }
}

#include "CurveCreator_Curve.hxx" // TODO
void CurveCreator_UtilsICurve::getPoint( const CurveCreator_ICurve* theCurve, const int theISection,
                                         const int theIPoint, gp_Pnt& thePoint )
{
  double anX, anY, aZ;
  // TODO
  const CurveCreator_Curve* aCurve = dynamic_cast<const CurveCreator_Curve*>( theCurve );
  if ( aCurve )
    aCurve->getCoordinates( theISection, theIPoint, anX, anY, aZ );
  thePoint = gp_Pnt( anX, anY, aZ);
}

std::string CurveCreator_UtilsICurve::getUniqSectionName( CurveCreator_ICurve* theCurve )
{
  for( int i = 0 ; i < 1000000 ; i++ ){
    char aBuffer[255];
    sprintf( aBuffer, "Section_%d", i+1 );
    std::string aName(aBuffer);
    int j;
    for( j = 0 ; j < theCurve->getNbSections() ; j++ ){
      if( theCurve->getSectionName(j) == aName )
        break;
    }
    if( j == theCurve->getNbSections() )
      return aName;
  }
  return "";
}

bool CurveCreator_UtilsICurve::contains( const CurveCreator_ICurve::SectionToPointList& theList,
                                         const CurveCreator_ICurve::SectionToPoint& theValue )
{
  bool isFound = false;

  CurveCreator_ICurve::SectionToPointList::const_iterator anIt = theList.begin(),
                                                          aLast = theList.end();
  for ( ; anIt != aLast && !isFound; anIt++ )
    isFound = anIt->first == theValue.first && anIt->second == theValue.second;

  return isFound;
}
