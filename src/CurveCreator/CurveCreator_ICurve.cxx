// Copyright (C) 2013-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:        CurveCreator_ICurve.cxx
// Author:      Sergey KHROMOV

#include "CurveCreator_ICurve.hxx"
#include "CurveCreator_Section.hxx"
#include "CurveCreator_Listener.hxx"

#include <stdio.h>

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
CurveCreator_ICurve::CurveCreator_ICurve
                (const CurveCreator::Dimension theDimension)
: myIsLocked  (false),
  myDimension (theDimension),
  myListener(NULL)
{
}

//=======================================================================
// function: Destructor
// purpose:
//=======================================================================
CurveCreator_ICurve::~CurveCreator_ICurve()
{
  // Delete all allocated data.
  clear();
}

//=======================================================================
// function: isLocked
// purpose:
//=======================================================================
bool CurveCreator_ICurve::isLocked() const
{
  return myIsLocked;
}

//=======================================================================
// function: getDimension
// purpose:
//=======================================================================
CurveCreator::Dimension CurveCreator_ICurve::getDimension() const
{
  return myDimension;
}

//=======================================================================
// function: getNbPoints
// purpose:
//=======================================================================
int CurveCreator_ICurve::getNbPoints(const int theISection) const
{
  int aNbCoords = 0;

  if (theISection == -1) {
    int i = 0;
    const int aNbSections = getNbSections();

    for (; i < aNbSections; i++) {
      aNbCoords += mySections[i]->myPoints.size();
    }
  } else {
    aNbCoords = mySections.at(theISection)->myPoints.size();
  }

  return aNbCoords/myDimension;
}

//=======================================================================
// function: getNbSections
// purpose:
//=======================================================================
int CurveCreator_ICurve::getNbSections() const
{
  return mySections.size();
}

//=======================================================================
// function: getCoordinates
// purpose:
//=======================================================================
CurveCreator::Coordinates CurveCreator_ICurve::getCoordinates
                            (const int theISection, const int theIPnt) const
{
  CurveCreator_Section *aSection = mySections.at(theISection);
  CurveCreator::Coordinates::const_iterator
    anIter = aSection->myPoints.begin() + toICoord(theIPnt);
  CurveCreator::Coordinates aResult(anIter, anIter + myDimension);

  return aResult;
}

//=======================================================================
// function: getType
// purpose:
//=======================================================================
CurveCreator::Type CurveCreator_ICurve::getType(const int theISection) const
{
  return mySections.at(theISection)->myType;
}

//=======================================================================
// function: getPoints
// purpose:
//=======================================================================
const CurveCreator::Coordinates &CurveCreator_ICurve::getPoints
                  (const int theISection) const
{
  return mySections.at(theISection)->myPoints;
}

//=======================================================================
// function: isClosed
// purpose:
//=======================================================================
bool CurveCreator_ICurve::isClosed(const int theISection) const
{
  return mySections.at(theISection)->myIsClosed;
}

std::string CurveCreator_ICurve::getSectionName(const int theISection) const
{
    return mySections.at(theISection)->myName;
}

//=======================================================================
// function: setType
// purpose:
//=======================================================================
void CurveCreator_ICurve::setType
    (const CurveCreator::Type theType, const int theISection)
{
  if (theISection == -1) {
    int i = 0;
    const int aNbSections = getNbSections();

    for (; i < aNbSections; i++) {
      mySections[i]->myType = theType;
    }
    if( myListener )
      myListener->curveChanged();
  } else {
    if( mySections.at(theISection)->myType != theType ){
      mySections.at(theISection)->myType = theType;
      if( myListener )
        myListener->sectionTypeChanged(theISection);
    }
  }
}

//=======================================================================
// function: addSection
// purpose:
//=======================================================================
void CurveCreator_ICurve::addSection
                  (const std::string& theName,
                   const CurveCreator::Type theType,
                   const bool theIsClosed,
                   const CurveCreator::Coordinates &thePoints)
{
  CurveCreator_Section *aSection = new CurveCreator_Section;

  std::string aName = theName;
  if( aName.empty() ){
      aName = getUnicSectionName();
  }
  aSection->myName     = aName;
  aSection->myType     = theType;
  aSection->myIsClosed = theIsClosed;
  aSection->myPoints   = thePoints;
  mySections.push_back(aSection);
  if( myListener )
    myListener->sectionAdded( -1 );
}

//=======================================================================
// function: removeSection
// purpose:
//=======================================================================
void CurveCreator_ICurve::removeSection(const int theISection)
{
  if (theISection == -1) {
    delete mySections.back();
    mySections.pop_back();
  } else {
    Sections::iterator anIterRm = mySections.begin() + theISection;

    delete *anIterRm;
    mySections.erase(anIterRm);
  }
  if( myListener )
    myListener->sectionRemoved(theISection);
}

//=======================================================================
// function: insertPoints
// purpose:
//=======================================================================
void CurveCreator_ICurve::insertPoints
                   (const CurveCreator::Coordinates &thePoints,
                    const int theISection,
                    const int theIPnt)
{
  if (theIPnt == -1) {
    // Add points to the end of section points.
    addPoints(thePoints, theISection);
  } else {
    CurveCreator_Section *aSection =
      (theISection == -1 ? mySections.back() : mySections.at(theISection));

    aSection->myPoints.insert(aSection->myPoints.begin() + toICoord(theIPnt),
                             thePoints.begin(), thePoints.end());
    if( myListener )
      myListener->pointInserted( theISection, theIPnt );
  }
}

void CurveCreator_ICurve::movePoint(const int theISection, const int theIPointFrom, const int theNewIndex)
{
    CurveCreator::Coordinates aCoords = getCoordinates(theISection, theIPointFrom );
    insertPoints(aCoords, theISection, theNewIndex+1);
    int aRemPntIndx = theIPointFrom;
    if( theNewIndex < theIPointFrom )
        aRemPntIndx++;
    removePoints(theISection, aRemPntIndx, 1 );
}

//=======================================================================
// function: removePoints
// purpose:
//=======================================================================
void CurveCreator_ICurve::removePoints(const int theISection,
                                      const int theIPnt,
                                      const int theNbPoints)
{
  CurveCreator_Section *aSection = mySections.at(theISection);
  CurveCreator::Coordinates::iterator anIterBegin =
    aSection->myPoints.begin() + toICoord(theIPnt);
  CurveCreator::Coordinates::iterator anIterEnd =
    (theNbPoints == -1 ?
    aSection->myPoints.end() : anIterBegin + toICoord(theNbPoints));

  aSection->myPoints.erase(anIterBegin, anIterEnd);
  if( myListener )
    myListener->pointRemoved(theISection, theIPnt, theNbPoints );
}

//=======================================================================
// function: clear
// purpose:
//=======================================================================
void CurveCreator_ICurve::clear()
{
  // Delete all allocated data.
  int i = 0;
  const int aNbSections = getNbSections();

  for (; i < aNbSections; i++) {
    delete mySections[i];
  }

  mySections.clear();
  if( myListener )
    myListener->curveChanged();
}

//=======================================================================
// function: setCoordinates
// purpose:
//=======================================================================
void CurveCreator_ICurve::setCoordinates
                     (const CurveCreator::Coordinates &theCoords,
                      const int theISection,
                      const int theIPnt)
{
  if (theCoords.size() == myDimension) {
    CurveCreator_Section *aSection = mySections.at(theISection);
    int i;

    for (i = 0; i < myDimension; i++) {
      aSection->myPoints.at(toICoord(theIPnt) + i) = theCoords[i];
    }

    if( myListener )
      myListener->pointChanged( theISection, theIPnt );
  }
}

//=======================================================================
// function: setClosed
// purpose:
//=======================================================================
void CurveCreator_ICurve::setClosed(const bool theIsClosed,
                                   const int theISection)
{
  if (theISection == -1) {
    int aSize = mySections.size();
    int i;

    for (i = 0; i < aSize; i++) {
      mySections[i]->myIsClosed = theIsClosed;
      if( myListener ){
        myListener->sectionClosed( theISection, theIsClosed );
      }
    }
  } else {
    mySections.at(theISection)->myIsClosed = theIsClosed;
    if( myListener ){
      myListener->sectionClosed( theISection, theIsClosed );
    }
  }
}

/** Set name of the specified section.
 */
void CurveCreator_ICurve::setName( const std::string& theName, const int theISection )
{
    if( ( theISection >= 0 ) && ( theISection < mySections.size() )){
        mySections.at(theISection)->myName = theName;
    }
}

//=======================================================================
// function: moveSection
// purpose:
//=======================================================================
void CurveCreator_ICurve::moveSection(const int theISection,
                                     const int theNewIndex)
{
  if (theISection != theNewIndex) {
    CurveCreator_Section *aSection = mySections.at(theISection);

    // Remove section
    Sections::iterator anIter = mySections.begin() + theISection;

    mySections.erase(anIter);

    // Insert section.
    anIter = mySections.begin() + theNewIndex;
    mySections.insert(anIter, aSection);
  }
}

//=======================================================================
// function: join
// purpose:
//=======================================================================
void CurveCreator_ICurve::join(const int theISectionTo,
                              const int theISectionFrom)
{
  if (theISectionTo != theISectionFrom) {
    CurveCreator_Section *aSection1 = mySections.at(theISectionTo);
    CurveCreator_Section *aSection2 = mySections.at(theISectionFrom);

    aSection1->myPoints.insert(aSection1->myPoints.end(),
    aSection2->myPoints.begin(), aSection2->myPoints.end());

    removeSection(theISectionFrom);
    if( myListener )
      myListener->curveChanged();
  }
}

//=======================================================================
// function: join
// purpose:
//=======================================================================
void CurveCreator_ICurve::join()
{
  const int aSize = mySections.size();

  if (aSize > 1) {
    CurveCreator_Section *aSection1 = mySections[0];
    int i;

    for (i = 1; i < aSize; i++) {
      CurveCreator_Section *aSection2 = mySections[i];

      aSection1->myPoints.insert(aSection1->myPoints.end(),
        aSection2->myPoints.begin(), aSection2->myPoints.end());
      delete aSection2;
    }

    // Just erace section pointers as they were deleted before.
    mySections.erase(mySections.begin() + 1, mySections.end());
    if( myListener )
      myListener->curveChanged();
  }
}

//=======================================================================
// function: toICoord
// purpose:
//=======================================================================
int CurveCreator_ICurve::toICoord(const int theIPnt) const
{
  return theIPnt*myDimension;
}

//=======================================================================
// function: getUnicSectionName
// purpose: return unic section name
//=======================================================================
std::string CurveCreator_ICurve::getUnicSectionName()
{
    for( int i = 0 ; i < 1000000 ; i++ ){
        char aBuffer[255];
        sprintf( aBuffer, "Section_%d", i+1 );
        std::string aName(aBuffer);
        int j;
        for( j = 0 ; j < mySections.size() ; j++ ){
            if( mySections[j]->myName == aName )
              break;
        }
        if( j == mySections.size() )
            return aName;
    }
    return "";
}

//=======================================================================
// function: setListener
// purpose: set curve changes listener
//=======================================================================
void CurveCreator_ICurve::setListener( CurveCreator_Listener*   theListener )
{
  myListener = theListener;
}

//=======================================================================
// function: setListener
// purpose: set curve changes listener
//=======================================================================
void CurveCreator_ICurve::removeListener()
{
  myListener = NULL;
}
