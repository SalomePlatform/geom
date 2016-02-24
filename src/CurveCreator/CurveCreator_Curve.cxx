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

// File:        CurveCreator_Curve.cxx
// Author:      Sergey KHROMOV

#include "CurveCreator_Curve.hxx"

#include "CurveCreator.hxx"
#include "CurveCreator_PosPoint.hxx"
#include "CurveCreator_Section.hxx"
#include "CurveCreator_Displayer.hxx"
#include "CurveCreator_Utils.hxx"

#include <AIS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Geom_CartesianPoint.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

#include <Prs3d_PointAspect.hxx>
#include <iostream>
#define DEBTRACE(msg) {std::cerr<<std::flush<<__FILE__<<" ["<<__LINE__<<"] : "<<msg<<std::endl<<std::flush;}

#include <stdio.h>

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
CurveCreator_Curve::CurveCreator_Curve( const CurveCreator::Dimension theDimension )
: myIsLocked  (false),
  myDimension (theDimension),
  myDisplayer (NULL),
  myAISShape  (NULL),
  myNbUndos   (0),
  myNbRedos   (0),
  myUndoDepth (-1),
  myOpLevel(0),
  mySkipSorting(false)
{
}

//=======================================================================
// function: Destructor
// purpose:
//=======================================================================
CurveCreator_Curve::~CurveCreator_Curve()
{
  // Delete all allocated data.
  clear();
}

//=======================================================================
// function: getDimension
// purpose:
//=======================================================================
CurveCreator::Dimension CurveCreator_Curve::getDimension() const
{
  return myDimension;
}

//=======================================================================
// function: getUniqSectionName
// purpose: return unique section name
//=======================================================================
std::string CurveCreator_Curve::getUniqSectionName() const
{
    CurveCreator_Section* aSection;
    for( int i = 0 ; i < 1000000 ; i++ ){
        char aBuffer[255];
        sprintf( aBuffer, "Section_%d", i+1 );
        std::string aName(aBuffer);
        int j;
        for( j = 0 ; j < mySections.size() ; j++ ){
            aSection = (CurveCreator_Section*)getSection( j );
            if ( aSection && aSection->myName == aName )
              break;
        }
        if( j == mySections.size() )
            return aName;
    }
    return "";
}

//=======================================================================
// function: setDisplayer
// purpose: set curve changes Displayer
//=======================================================================
void CurveCreator_Curve::setDisplayer( CurveCreator_Displayer* theDisplayer )
{
  myDisplayer = theDisplayer;
}

//=======================================================================
// function: getDisplayer
// purpose: get curve changes Displayer
//=======================================================================
CurveCreator_Displayer* CurveCreator_Curve::getDisplayer()
{
  return myDisplayer;
}

//=======================================================================
// function: removeDisplayer
// purpose: remove the attached Displayer
//=======================================================================
void CurveCreator_Curve::removeDisplayer()
{
  myDisplayer = NULL;
}

//=======================================================================
// function: addDiff
// purpose:
//=======================================================================
bool CurveCreator_Curve::addEmptyDiff()
{
  bool isEnabled = false;

  if (myUndoDepth != 0) {
    // Forget all Redos after the current one.
    if (myNbRedos > 0) {
      myNbRedos = 0;
      myListDiffs.erase(myCurrenPos, myListDiffs.end());
    }

    if (myUndoDepth == -1 || myNbUndos < myUndoDepth) {
      // Increase the number of undos.
      myNbUndos++;
    } else {
      // If there are too many differences, remove the first one.
      myListDiffs.pop_front();
    }

    // Add new difference.
    myListDiffs.push_back(CurveCreator_Diff());
    myCurrenPos = myListDiffs.end();
    isEnabled = true;
  }

  return isEnabled;
}

void CurveCreator_Curve::startOperation()
{
    myOpLevel++;
}

void CurveCreator_Curve::finishOperation()
{
   myOpLevel--;
}

//=======================================================================
// function: toICoord
// purpose:
//=======================================================================
int CurveCreator_Curve::toICoord(const int theIPnt) const
{
  return theIPnt * myDimension;
}

//=======================================================================
// function: setUndoDepth
// purpose:
//=======================================================================
void CurveCreator_Curve::setUndoDepth(const int theDepth)
{
  if (theDepth == 0) {
    // Reset all undo/redo data.
    myNbUndos = 0;
    myNbRedos = 0;
    myListDiffs.clear();
    myCurrenPos = myListDiffs.end();
    myUndoDepth = 0;
  } else if (theDepth == -1) {
    // There is nothing to do as the depth become unlimited.
    myUndoDepth = -1;
  } else if (theDepth > 0) {
    // The new "real" depth is set.
    if (theDepth < myNbRedos) {
      // The new depth is less then number of redos. Remove the latest redos.
      int aShift = (myNbRedos - theDepth);
      ListDiff::iterator aFromPos = myListDiffs.end();

      while (aShift--) {
        aFromPos--;
      }

      myListDiffs.erase(aFromPos, myListDiffs.end());
      myNbRedos = theDepth;
    }

    if (theDepth < myNbUndos + myNbRedos) {
      // The new depth is less then the total number of differences.
      // Remove the first undos.
      int aShift = (myNbUndos + myNbRedos - theDepth);
      ListDiff::iterator aToPos = myListDiffs.begin();

      while (aShift--) {
        aToPos++;
      }

      myListDiffs.erase(myListDiffs.begin(), aToPos);
      myNbUndos = theDepth - myNbRedos;
    }

    myUndoDepth = theDepth;
  }
}

//=======================================================================
// function: getUndoDepth
// purpose:
//=======================================================================
int CurveCreator_Curve::getUndoDepth() const
{
  return myUndoDepth;
}

void CurveCreator_Curve::getCoordinates( int theISection, int theIPoint, double& theX, double& theY, double& theZ ) const
{
  CurveCreator::Coordinates aCoords = getPoint( theISection, theIPoint );
  theX = aCoords[0];
  theY = aCoords[1];
  theZ = 0.;
  if( getDimension() == CurveCreator::Dim3d ){
    theZ = aCoords[2];
  }
}

void CurveCreator_Curve::redisplayCurve()
{
  //DEBTRACE("redisplayCurve");
  if( myDisplayer ) {
    myDisplayer->eraseAll( false );
    myAISShape = NULL;

    myDisplayer->display( getAISObject( true ), true );
  }
}

//! For internal use only! Undo/Redo are not used here.
bool CurveCreator_Curve::moveSectionInternal(const int theISection,
                                             const int theNewIndex)
{
  bool res = false;
  int aMovedSectionId = theISection >= 0 ? theISection : mySections.size()-1;

  if (aMovedSectionId != theNewIndex) {
    CurveCreator_Section* aSection = (CurveCreator_Section*)getSection( aMovedSectionId );

    // Remove section
    CurveCreator::Sections::iterator anIter = mySections.begin() + aMovedSectionId;

    mySections.erase(anIter);

    // Insert section.
    anIter = mySections.begin() + theNewIndex;
    mySections.insert(anIter, aSection);
    res = true;
  }
  return res;
}

//=======================================================================
// function: moveSection
// purpose:
//=======================================================================
bool CurveCreator_Curve::moveSection(const int theISection,
                                     const int theNewIndex)
{
  bool res = false;
  // Set the difference.
  startOperation();
  if (addEmptyDiff()) {
    myListDiffs.back().init(this, CurveCreator_Operation::MoveSection,
                            theISection, theNewIndex);
  }

  // Update the curve.
  res = moveSectionInternal(theISection, theNewIndex);
  finishOperation();
  return res;
}

/************   Implementation of INTERFACE methods   ************/

/***********************************************/
/***          Undo/Redo methods              ***/
/***********************************************/

//! Get number of available undo operations
int CurveCreator_Curve::getNbUndo() const
{
  return myNbUndos;
}

//! Undo previous operation
bool CurveCreator_Curve::undo()
{
  bool res = false;
  if (myNbUndos > 0) {
    myNbUndos--;
    myNbRedos++;
    myCurrenPos--;
    myCurrenPos->applyUndo(this);
    res = true;
  }
  return res;
}

//! Get number of available redo operations
int CurveCreator_Curve::getNbRedo() const
{
  return myNbRedos;
}

//! Redo last previously "undone" operation
bool CurveCreator_Curve::redo()
{
  bool res = false;
  if (myNbRedos > 0) {
    myCurrenPos->applyRedo(this);
    myCurrenPos++;
    myNbRedos--;
    myNbUndos++;
    res = true;
  }
  return res;
}

/***********************************************/
/***           Section methods               ***/
/***********************************************/
//! For internal use only! Undo/Redo are not used here.
bool CurveCreator_Curve::clearInternal()
{
  // erase curve from the viewer
  if( myDisplayer ) {
    myDisplayer->eraseAll( true );
    myAISShape = NULL;
  }
  // Delete all allocated data.
  int i = 0;
  const int aNbSections = getNbSections();

  CurveCreator_Section* aSection;
  for (; i < aNbSections; i++) {
    aSection = (CurveCreator_Section*)getSection( i );
    if ( aSection )
      delete aSection;
  }

  mySections.clear();
  
  return true;
}

//=======================================================================
// function: clear
// purpose:
//=======================================================================
bool CurveCreator_Curve::clear()
{
  bool res = false;
  startOperation();
  // Set the difference.
  if (addEmptyDiff()) {
    myListDiffs.back().init(this);
  }
  res = clearInternal();
  finishOperation();
  return res;
}

//! For internal use only! Undo/Redo are not used here.
bool CurveCreator_Curve::joinInternal( const std::list<int>& theSections )
{
  bool res = false;
  if ( theSections.empty() )
    return res;

  int anISectionMain = theSections.front();
  CurveCreator_Section* aSectionMain =
    (CurveCreator_Section*)getSection( anISectionMain );

  std::list <int> aSectionsToJoin = theSections;
  aSectionsToJoin.erase( aSectionsToJoin.begin() ); // skip the main section
  // it is important to sort and reverse the section ids in order to correctly remove them
  aSectionsToJoin.sort();
  aSectionsToJoin.reverse();

  std::list<int>::const_iterator anIt = aSectionsToJoin.begin(), aLast = aSectionsToJoin.end();
  CurveCreator_Section* aSection;
  for (; anIt != aLast; anIt++) {
    aSection = (CurveCreator_Section*)getSection( *anIt );
    aSectionMain->myPoints.insert(aSectionMain->myPoints.end(), aSection->myPoints.begin(),
                                  aSection->myPoints.end());
    res = removeSectionInternal(*anIt);
    if ( !res )
      break;
  }

  redisplayCurve();
  return res;
}

bool CurveCreator_Curve::join( const std::list<int>& theSections )
{
  bool res = false;

  if ( !theSections.empty() )
  {
    startOperation();
    if (addEmptyDiff())
      myListDiffs.back().init(this, CurveCreator_Operation::Join, theSections);

    res = joinInternal( theSections );

    finishOperation();
  }
  return res;
}

//! Get number of sections
int CurveCreator_Curve::getNbSections() const
{
  return mySections.size();
}

//! For internal use only! Undo/Redo are not used here.
int CurveCreator_Curve::addSectionInternal
        (const std::string& theName, const CurveCreator::SectionType theType,
         const bool theIsClosed, const CurveCreator::Coordinates &thePoints)
{
  CurveCreator_Section *aSection = new CurveCreator_Section;

  std::string aName = theName;
  if( aName.empty() ){
      aName = getUniqSectionName();
  }
  aSection->myName     = aName;
  aSection->myType     = theType;
  aSection->myIsClosed = theIsClosed;
  aSection->myPoints   = thePoints;
  mySections.push_back(aSection);
  redisplayCurve();
  return mySections.size()-1;
}

//=======================================================================
// function: addSection
// purpose: adds an empty section
//=======================================================================
int CurveCreator_Curve::addSection
        (const std::string& theName, const CurveCreator::SectionType theType,
         const bool theIsClosed)
{
  int resISection = -1;
  // Set the difference.
  startOperation();
  CurveCreator::Coordinates aCoords; //empty list
  if (addEmptyDiff()) {
    myListDiffs.back().init(this, CurveCreator_Operation::AddSection,
                            theName, aCoords, theType, theIsClosed);
  }

  resISection = addSectionInternal(theName, theType, theIsClosed, aCoords);

  finishOperation();
  return resISection;
}
//=======================================================================
// function: addSection
// purpose: adds a section with the given points
//=======================================================================
int CurveCreator_Curve::addSection
        (const std::string& theName, const CurveCreator::SectionType theType,
         const bool theIsClosed, const CurveCreator::Coordinates &thePoints)
{
  int resISection = -1;
  // Set the difference.
  startOperation();
  if (addEmptyDiff()) {
    myListDiffs.back().init(this, CurveCreator_Operation::AddSection,
                            theName, thePoints, theType, theIsClosed);
  }

  resISection = addSectionInternal(theName, theType, theIsClosed, thePoints);

  finishOperation();
  return resISection;
}

//! For internal use only! Undo/Redo are not used here.
bool CurveCreator_Curve::removeSectionInternal( const int theISection )
{
  if (theISection == -1) {
    delete mySections.back();
    mySections.pop_back();
  } else {
    CurveCreator::Sections::iterator anIterRm = mySections.begin() + theISection;

    delete *anIterRm;
    mySections.erase(anIterRm);
  }
  redisplayCurve();
  return true;
}
  
//! Removes the given sections.
bool CurveCreator_Curve::removeSection( const int theISection )
{
  bool res = false;
  // Set the difference.
  startOperation();
  if (addEmptyDiff())
    myListDiffs.back().init(this, CurveCreator_Operation::RemoveSection, theISection);

  res = removeSectionInternal( theISection );

  finishOperation();
  return res;
}

/**
 *  Get number of points in specified section or (the total number of points
 *  in Curve if theISection is equal to -1).
 */
int CurveCreator_Curve::getNbPoints( const int theISection ) const
{
  int aNbCoords = 0;

  CurveCreator_Section* aSection;
  if (theISection == -1) {
    int i = 0;
    const int aNbSections = getNbSections();

    for (; i < aNbSections; i++) {
      aSection = (CurveCreator_Section*)getSection( i );
      if ( aSection )
        aNbCoords += aSection->myPoints.size();
    }
  } else {
    aSection = (CurveCreator_Section*)getSection( theISection );
    if ( aSection )
      aNbCoords = aSection->myPoints.size();
  }

  return aNbCoords/myDimension;
}

void CurveCreator_Curve::setSkipSorting( const bool theIsToSkip )
{
  mySkipSorting = theIsToSkip;
}

bool CurveCreator_Curve::canPointsBeSorted()
{
  return false;
}

/**
 * Saves points coordinates difference.
 * \param theOldCoords the old points coordinates
 */
void CurveCreator_Curve::saveCoordDiff( const SectionToPointCoordsList &theOldCoords )
{
  // Set the difference.
  startOperation();
  if (addEmptyDiff()) {
    myListDiffs.back().init(this, theOldCoords);
  }
  finishOperation();
}

//! Get "closed" flag of the specified section
bool CurveCreator_Curve::isClosed( const int theISection ) const
{
  const CurveCreator_Section* aSection =
    (CurveCreator_Section*)getSection( theISection );
  return aSection ? aSection->myIsClosed : false;
}

//! For internal use only! Undo/Redo are not used here.
bool CurveCreator_Curve::setClosedInternal( const int theISection, 
                                            const bool theIsClosed )
{
  CurveCreator_Section* aSection = 0;
  if (theISection == -1) {
    int aSize = mySections.size();
    int i;

    for (i = 0; i < aSize; i++) {
      aSection = (CurveCreator_Section*)getSection( i );
      if( aSection ) {
        aSection->myIsClosed = theIsClosed;
        redisplayCurve();
      }
    }
  } else {
    aSection = (CurveCreator_Section*)getSection( theISection );
    if ( aSection ) {
      aSection->myIsClosed = theIsClosed;
      redisplayCurve();
    }
  }
  return true;
}

/**
 *  Set "closed" flag of the specified section (all sections if
 *  \a theISection is -1).
 */
bool CurveCreator_Curve::setClosed( const int theISection, 
                                    const bool theIsClosed )
{
  bool res = false;
  // Set the difference.
  startOperation();
  if (addEmptyDiff()) {
    myListDiffs.back().init(this, CurveCreator_Operation::SetClosed,
                            theIsClosed, theISection);
  }
  res = setClosedInternal( theISection, theIsClosed );
  finishOperation();
  return res;
}

//! Returns specified section name
std::string CurveCreator_Curve::getSectionName( const int theISection ) const
{
  CurveCreator_Section* aSection = (CurveCreator_Section*)getSection( theISection );
  return aSection ? aSection->myName : "";
}

//! For internal use only! Undo/Redo are not used here.
bool CurveCreator_Curve::setSectionNameInternal( const int theISection, 
                                                 const std::string& theName )
{
  bool res = false;
  CurveCreator_Section* aSection = (CurveCreator_Section*)getSection( theISection );
  if( aSection ) {
    aSection->myName = theName;
    res = true;
  }
  return res;
}

/** Set name of the specified section */
bool CurveCreator_Curve::setSectionName( const int theISection, 
                                         const std::string& theName )
{
  bool res = false;
  // Set the difference.
  startOperation();
  if (addEmptyDiff()) {
    myListDiffs.back().init(this, CurveCreator_Operation::RenameSection,
                            theName, theISection);
  }
  res = setSectionNameInternal( theISection, theName );
  finishOperation();
  return res;
}

//! Get type of the specified section
CurveCreator::SectionType CurveCreator_Curve::getSectionType
  ( const int theISection ) const
{
  CurveCreator_Section* aSection = (CurveCreator_Section*)getSection( theISection );
  return aSection ? aSection->myType : CurveCreator::Polyline;
}

//! For internal use only! Undo/Redo are not used here.
bool CurveCreator_Curve::setSectionTypeInternal( const int theISection, 
                                                 const CurveCreator::SectionType theType )
{
  CurveCreator_Section* aSection;
  if (theISection == -1) {
    int i = 0;
    const int aNbSections = getNbSections();

    for (; i < aNbSections; i++) {
      aSection = (CurveCreator_Section*)getSection( i );
      if ( aSection )
        aSection->myType = theType;
    }
    redisplayCurve();
  } else {
    aSection = (CurveCreator_Section*)getSection( theISection );
    if ( aSection && aSection->myType != theType ){
      aSection->myType = theType;
      redisplayCurve();
    }
  }
  return true;
}

/**
 *  Set type of the specified section (or all sections
 *  if \a theISection is -1).
 */
bool CurveCreator_Curve::setSectionType( const int theISection, 
                                         const CurveCreator::SectionType theType )
{
  bool res = false;
  startOperation();
  // Set the difference.
  if (addEmptyDiff()) {
    myListDiffs.back().init(this, CurveCreator_Operation::SetType,
                            theType, theISection);
  }

  res = setSectionTypeInternal( theISection, theType );

  finishOperation();
  return res;
}


/***********************************************/
/***           Point methods                 ***/
/***********************************************/

//! For internal use only! Undo/Redo are not used here.
bool CurveCreator_Curve::addPointsInternal( const CurveCreator::SectionsMap &theSectionsMap )
{
  bool res = false;
  CurveCreator::SectionsMap::const_iterator anIt = theSectionsMap.begin();
  CurveCreator_Section *aSection = 0;
  for ( ; anIt != theSectionsMap.end(); anIt++ ) {
    int anISection = anIt->first;
    aSection = (CurveCreator_Section*)getSection( anISection );
    if( aSection ) {
      CurveCreator::PosPointsList aSectionPoints = anIt->second;
      CurveCreator::PosPointsList::const_iterator aPntIt = aSectionPoints.begin();
      for( ; aPntIt != aSectionPoints.end(); aPntIt++ ){
        int anIPnt = (*aPntIt)->myID;
        CurveCreator::Coordinates aCoords = (*aPntIt)->myCoords;
        CurveCreator::Coordinates::iterator anIterPosition;
        if(anIPnt == -1)
          anIterPosition = aSection->myPoints.end();
        else
          anIterPosition = aSection->myPoints.begin() + toICoord(anIPnt);
        CurveCreator::Coordinates::const_iterator aFirstPosition = 
          aCoords.begin();
        aSection->myPoints.insert(anIterPosition,
                                  aCoords.begin(), aCoords.end());
      }
      res = true;
    }
  }
  if(res)
    redisplayCurve();
  return res;
}

/**
 *  Add one point to the specified section starting from the given theIPnt index
 *  (or at the end of points if \a theIPnt is -1).
 */
bool CurveCreator_Curve::addPoints( const CurveCreator::Coordinates& theCoords,
                                    const int theISection,
                                    const int theIPnt )
{
  //DEBTRACE("addPoints");
  bool res = false;
  CurveCreator::Coordinates aCoords = theCoords;
  // Set the difference.
  startOperation();
  if (addEmptyDiff()) {
    CurveCreator_ICurve::SectionToPointCoordsList aList;
    aList.push_back(std::make_pair(std::make_pair(theISection, theIPnt), theCoords));
    myListDiffs.back().init(this, CurveCreator_Operation::InsertPoints,
                            aList);
  }
  CurveCreator::SectionsMap aSectionsMap;
  CurveCreator::PosPointsList aPoints;
  CurveCreator_PosPoint* aPosPoint = new CurveCreator_PosPoint( theIPnt, theCoords );
  aPoints.push_back( aPosPoint );
  aSectionsMap[theISection] = aPoints;

  res = addPointsInternal( aSectionsMap );

  finishOperation();
  return res;
}

//! For internal use only! Undo/Redo are not used here.
bool CurveCreator_Curve::setPointInternal( const CurveCreator::SectionsMap &theSectionsMap )
{
  bool res = false;
  // Update the curve.
  CurveCreator::SectionsMap::const_iterator anIt = theSectionsMap.begin();
  CurveCreator_Section *aSection = 0;
  for ( ; anIt != theSectionsMap.end(); anIt++ ) {
    int anISection = anIt->first;
    aSection = (CurveCreator_Section*)getSection( anISection );
    if( aSection ) { 
      CurveCreator::PosPointsList aSectionPoints = anIt->second;
      CurveCreator::PosPointsList::const_iterator aPntIt = aSectionPoints.begin();
      for( ; aPntIt != aSectionPoints.end(); aPntIt++ ){
        int anIPnt = (*aPntIt)->myID;
        CurveCreator::Coordinates aCoords = (*aPntIt)->myCoords;
        for ( int i = 0; i < myDimension; i++)
          aSection->myPoints.at(toICoord(anIPnt) + i) = aCoords[i];
      }
      res = true;
    }
  }
  if(res)
    redisplayCurve();
  
  return res;
}

//! Set coordinates of specified point
bool CurveCreator_Curve::setPoint( const int theISection,
                                   const int theIPnt,
                                   const CurveCreator::Coordinates& theNewCoords )
{
  //DEBTRACE("setPoint");
  bool res = false;
  // Set the difference.
  startOperation();
  if (addEmptyDiff()) {
    CurveCreator_ICurve::SectionToPointCoordsList aList;
    aList.push_back(std::make_pair(std::make_pair(theISection, theIPnt), theNewCoords));
    myListDiffs.back().init(this, CurveCreator_Operation::SetCoordinates,
                            aList);
  }
  CurveCreator::SectionsMap aSectionsMap;
  CurveCreator::PosPointsList aPoints;
  CurveCreator_PosPoint* aPosPoint = new CurveCreator_PosPoint( theIPnt, theNewCoords );
  aPoints.push_back( aPosPoint );
  aSectionsMap[theISection] = aPoints;

  int aSize1 = getNbPoints( theISection );
  res = setPointInternal( aSectionsMap );
  int aSize2 = getNbPoints( theISection );

  finishOperation();

  return res; 
}

//! Set coordinates of specified points from different sections
bool CurveCreator_Curve::setSeveralPoints( const SectionToPointCoordsList &theSectionToPntCoords,
                                           const bool theIsToSaveDiff )
{
  //DEBTRACE("setSeveralPoints");
  bool res = false;
  // Set the difference.
  startOperation();
  if (theIsToSaveDiff && addEmptyDiff()) {
    myListDiffs.back().init(this, CurveCreator_Operation::SetCoordinates,
                            theSectionToPntCoords);
  }
  CurveCreator::SectionsMap aSectionsMap;
  CurveCreator::PosPointsList aPosPoints;
  CurveCreator_ICurve::SectionToPointCoordsList::const_iterator anIt = 
    theSectionToPntCoords.begin(), aLast = theSectionToPntCoords.end();
  int aSectionId, aPointId;
  for ( ; anIt != aLast; anIt++ ) {
    aPosPoints.clear();
    aSectionId = anIt->first.first;
    aPointId = anIt->first.second;
    CurveCreator::Coordinates aNewCoords = anIt->second;
    CurveCreator_PosPoint* aPosPoint = 
      new CurveCreator_PosPoint( aPointId, aNewCoords );
    if( aSectionsMap.find(aSectionId) != aSectionsMap.end() )
      aPosPoints = aSectionsMap[aSectionId];
    aPosPoints.push_back( aPosPoint );
    aSectionsMap[aSectionId] = aPosPoints;
    
  }
  res = setPointInternal( aSectionsMap );
  finishOperation();

  return res; 
}

//! For internal use only! Undo/Redo are not used here.
bool CurveCreator_Curve::removePointsInternal( const SectionToPointList &thePoints )
{
  bool aRes = false;
  std::map<int, std::list<int> > aConvPoints;
  convert( thePoints, aConvPoints );
  std::map<int, std::list<int> >::const_iterator anIt = aConvPoints.begin(),
                                          aLast = aConvPoints.end();
  for ( ; anIt != aLast; anIt++ ) {
    int aSectionId = anIt->first;
    aRes = removeSectionPoints(aSectionId, anIt->second);
  }
  if( aRes)
    redisplayCurve();

  return aRes;
}

//! Remove point with given id
bool CurveCreator_Curve::removePoint( const int theISection, const int theIPnt )
{
  bool res = false;
  // Set the difference.
  startOperation();
  SectionToPointList aListOfSectionsToPoints;
  aListOfSectionsToPoints.push_back(std::make_pair(theISection, theIPnt));
  if (addEmptyDiff()) {
    myListDiffs.back().init(this, CurveCreator_Operation::RemovePoints,
                            aListOfSectionsToPoints);
  }
  res = removePointsInternal( aListOfSectionsToPoints );
  finishOperation();
  return res;
}

//! Remove several points from different sections with given ids
bool CurveCreator_Curve::removeSeveralPoints( const SectionToPointList &theSectionToPntIDs)
{
  bool res = false;
  // Set the difference.
  startOperation();
  if (addEmptyDiff()) {
    myListDiffs.back().init(this, CurveCreator_Operation::RemovePoints,
                            theSectionToPntIDs);
  }
  res = removePointsInternal( theSectionToPntIDs );
  finishOperation();
  return res;
}

  //=======================================================================
// function: getCoordinates
// purpose:
//=======================================================================
CurveCreator::Coordinates CurveCreator_Curve::getPoint( const int theISection,
                                                        const int theIPnt) const
{
  //DEBTRACE("getPoint");
  CurveCreator_Section* aSection = (CurveCreator_Section*)getSection( theISection );
  CurveCreator::Coordinates::const_iterator
    anIter = aSection->myPoints.begin() + toICoord(theIPnt);
  CurveCreator::Coordinates aResult(anIter, anIter + myDimension);

  return aResult;
}

//=======================================================================
// function: getPoints
// purpose:
//=======================================================================
CurveCreator::Coordinates CurveCreator_Curve::getPoints( const int theISection ) const
{
  //DEBTRACE("getPoints");
  CurveCreator_Section* aSection = (CurveCreator_Section*)getSection( theISection );
  return aSection ? aSection->myPoints : CurveCreator::Coordinates();
}

void CurveCreator_Curve::constructAISObject()
{
  //DEBTRACE("constructAISObject");
  TopoDS_Shape aShape;
  CurveCreator_Utils::constructShape( this, aShape );

  myAISShape = new AIS_Shape( aShape );
  Handle(Prs3d_PointAspect) anAspect = myAISShape->Attributes()->PointAspect();
  anAspect->SetScale( 3.0 );
  anAspect->SetTypeOfMarker(Aspect_TOM_O_POINT);
  anAspect->SetColor(Quantity_NOC_ROYALBLUE4);
  myAISShape->Attributes()->SetPointAspect( anAspect );

}

Handle(AIS_InteractiveObject) CurveCreator_Curve::getAISObject( const bool theNeedToBuild ) const
{
  //DEBTRACE("getAISObject");
  if ( !myAISShape && theNeedToBuild ) {
    CurveCreator_Curve* aCurve = (CurveCreator_Curve*)this;
    aCurve->constructAISObject();
  }
  return myAISShape;
}

bool CurveCreator_Curve::removeSectionPoints( const int theSectionId,
                                              const std::list<int>& thePointIds )
{
  bool aRes = false;

  CurveCreator_Section* aSection = (CurveCreator_Section*)getSection( theSectionId );
  if ( !aSection )
    return aRes;

  std::list<int> aSectionPoints = thePointIds;
  aSectionPoints.sort();
  std::list<int>::const_reverse_iterator aPntIt = aSectionPoints.rbegin();
  for ( ; aPntIt != aSectionPoints.rend(); aPntIt++ ) {
    int aPntIndx = *aPntIt;
    CurveCreator::Coordinates::iterator aFirstPosition;
    if ( aPntIndx == -1 )
      aFirstPosition = aSection->myPoints.end() - getDimension();
    else
      aFirstPosition = aSection->myPoints.begin() + toICoord( aPntIndx );
    aSection->myPoints.erase( aFirstPosition, aFirstPosition + getDimension() );
    aRes = true;
  }
  return aRes;
}

void CurveCreator_Curve::convert( const SectionToPointList& thePoints,
                                  std::map< int, std::list<int> >& theConvPoints )
{
  theConvPoints.clear();

  SectionToPointList::const_iterator anIt = thePoints.begin(), aLast = thePoints.end();
  std::list<int> aPoints;
  int aSectionId, aPointId;
  for ( ; anIt != aLast; anIt++ ) {
    aSectionId = anIt->first;
    aPointId = anIt->second;
    aPoints.clear();
    if ( theConvPoints.find( aSectionId ) != theConvPoints.end() )
      aPoints = theConvPoints[aSectionId];
    aPoints.push_back( aPointId );
    theConvPoints[aSectionId] = aPoints;
  }
}
