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

// File:        CurveCreator_Diff.cxx
// Author:      Sergey KHROMOV

#include "CurveCreator_Diff.hxx"
#include "CurveCreator_Curve.hxx"

#include <list>

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
CurveCreator_Diff::CurveCreator_Diff()
: myNbUndos (0),
  myPUndo   (NULL),
  myPRedo   (NULL)
{
}

//=======================================================================
// function: Destructor
// purpose:
//=======================================================================
CurveCreator_Diff::~CurveCreator_Diff()
{
  clear();
}

//=======================================================================
// function: init
// purpose:
//=======================================================================
bool CurveCreator_Diff::init(const CurveCreator_Curve *theCurve)
{
  CurveCreator_Operation::Type aType = CurveCreator_Operation::Clear;
  bool isOK = false;

  if (theCurve != NULL) {
    clear();

    // Set redo.
    myPRedo = new CurveCreator_Operation;

    if (myPRedo->init(aType)) {
      isOK = true;

      const int aNbSections = theCurve->getNbSections();

      // Construct undo for Clear command.
      if (aNbSections > 0) {
        setNbUndos(aNbSections);

        for (int i = 0; i < aNbSections && isOK; i++) {
          // Add AddSection command.
          isOK = addSectionToUndo(theCurve, i, myPUndo[i]);
        }
      }
    }

    if (!isOK) {
      clear();
    }
  }

  return isOK;
}

//=======================================================================
// function: init
// purpose:
//=======================================================================
bool CurveCreator_Diff::init(const CurveCreator_Curve *theCurve,
                             const CurveCreator_Operation::Type theType,
                             const int theIntParam)
{
  bool isOK = false;

  if (theCurve != NULL) {
    clear();

    // Set redo.
    myPRedo = new CurveCreator_Operation;

    if (myPRedo->init(theType, theIntParam)) {
      // Construct undo for RemoveSection command.
      // If the last section is removed, one AddSection command is enough.
      // If not last section is removed, two commands are requred: AddSection
      // and MoveSection.
      const int aLastIndex = theCurve->getNbSections() - 1;

      if (theIntParam == aLastIndex) {
        setNbUndos(1);
      } else {
        setNbUndos(2);
      }

      isOK = addSectionToUndo(theCurve, theIntParam, myPUndo[0]);

      if (isOK && theIntParam != aLastIndex) {
        isOK = myPUndo[1].init(CurveCreator_Operation::MoveSection,
                               aLastIndex, theIntParam);
      }
    }

    if (!isOK) {
      clear();
    }
  }

  return isOK;
}

//=======================================================================
// function: init
// purpose:
//=======================================================================
bool CurveCreator_Diff::init(const CurveCreator_Curve *theCurve,
                             const CurveCreator_Operation::Type theType,
                             const int theIntParam1,
                             const int theIntParam2)
{
  bool isOK = false;

  if (theCurve != NULL) {
    clear();

    // Set redo.
    myPRedo = new CurveCreator_Operation;

    if (myPRedo->init(theType, theIntParam1, theIntParam2)) {
      // Construct undo for different commands.
      switch (theType) {
        case CurveCreator_Operation::SetType:
        case CurveCreator_Operation::SetClosed:
            isOK = setTypeOrClosedToUndo
              (theCurve, theType, theIntParam1, theIntParam2);
          break;
        case CurveCreator_Operation::MoveSection:
          setNbUndos(1);
          isOK = myPUndo[0].init(theType, theIntParam2, theIntParam1);
          break;
        default:
          break;
      }
    }

    if (!isOK) {
      clear();
    }
  }

  return isOK;
}

//=======================================================================
// function: init
// purpose:
//=======================================================================
bool CurveCreator_Diff::init(const CurveCreator_Curve *theCurve,
                             const CurveCreator_Operation::Type theType,
                             const std::list<int>& theParams)
{
  bool isOK = false;

  if (theCurve != NULL || theParams.empty()) {
    clear();

    // Set redo.
    myPRedo = new CurveCreator_Operation;

    if (myPRedo->init(theType, theParams)) {
      // Construct undo for different commands.
      switch (theType) {
        case CurveCreator_Operation::Join:
          {
            int aSectionMain = theParams.front();
            const int aNbPointsMain  = theCurve->getNbPoints(aSectionMain);

            std::list<int> aSectionsToJoin = theParams;
            aSectionsToJoin.erase( aSectionsToJoin.begin() );
            // it is important to sort the section indices in order to correct perform undo
            // for the move sections to the previous positions
            aSectionsToJoin.sort();
            // 1rst undo for remove points from the main and n-1 undoes to contain joined sections
            int aSectionsToJoinNb = aSectionsToJoin.size();
            int aNbUndos = 2*aSectionsToJoinNb + 1;
            setNbUndos( aNbUndos );

            // Add joined sections to undo
            std::list<int>::const_iterator anIt = aSectionsToJoin.begin(),
                                           aLast = aSectionsToJoin.end();
            anIt = aSectionsToJoin.begin();
            int aLastSectionId = -1;
            for (int i = 0; anIt != aLast && i < aSectionsToJoinNb; anIt++, i++) {
              int anISection = *anIt;
              isOK = addSectionToUndo( theCurve, anISection, myPUndo[i*2] );
              if (isOK) {
                isOK = myPUndo[i*2+1].init(CurveCreator_Operation::MoveSection,
                                           aLastSectionId, anISection);
                if (!isOK)
                  break;
              }
            }
            // Construct undo for RemovePoints command.
            if (isOK) {
              int aNbPointsInJoined = 0;
              anIt = aSectionsToJoin.begin();
              for ( ; anIt != aLast; anIt++ )
                aNbPointsInJoined += theCurve->getNbPoints( *anIt );

              int aJoinedSize = aNbPointsMain + aNbPointsInJoined;
              CurveCreator_ICurve::SectionToPointList aSectionToPointList;
              for (int anIPoint = aNbPointsMain; anIPoint < aJoinedSize; anIPoint++)
                aSectionToPointList.push_back(std::make_pair(aSectionMain, anIPoint));

              isOK = myPUndo[aNbUndos-1].init(CurveCreator_Operation::RemovePoints, aSectionToPointList);
            }
          }
          break;
        default:
          break;
      }
    }

    if (!isOK) {
      clear();
    }
  }

  return isOK;
}

//=======================================================================
// function: init
// purpose:
//=======================================================================
bool CurveCreator_Diff::init(const CurveCreator_Curve *theCurve,
                             const CurveCreator_Operation::Type theType,
                             const std::string& theName,
                             const CurveCreator::Coordinates &theCoords,
                             const int theIntParam1,
                             const int theIntParam2)
{
    bool isOK = false;

    if (theCurve != NULL) {
      clear();

      // Set redo.
      myPRedo = new CurveCreator_Operation;

      if (myPRedo->init(theType, theName, theCoords, theIntParam1, theIntParam2)) {
        // Construct undo for different commands.
        switch (theType) {
          case CurveCreator_Operation::AddSection:
            setNbUndos(1);
            isOK = myPUndo[0].init(CurveCreator_Operation::RemoveSection, -1);
            break;
        }
      }
    }
    if( !isOK )
        clear();
    return isOK;
}

bool CurveCreator_Diff::init(const CurveCreator_Curve *theCurve,
                             const CurveCreator_Operation::Type theType,
                             const std::string &theName,
                             const int theIntParam1 )
{
  bool isOK = false;
  myPRedo = new CurveCreator_Operation;

  if (myPRedo->init(theType, theName, theIntParam1 )) {
    // Construct undo for different commands.
    switch (theType) {
      case CurveCreator_Operation::RenameSection:
        setNbUndos(1);
        isOK = myPUndo[0].init(CurveCreator_Operation::RenameSection,
                               theCurve->getSectionName(theIntParam1), theIntParam1);
        break;
    }
  }
  if( !isOK ){
    clear();
  }
  return isOK;
}

bool CurveCreator_Diff::init(const CurveCreator_Curve *theCurve,
                             const CurveCreator_Operation::Type theType,
                             const CurveCreator_ICurve::SectionToPointList &theParamList1)
{
  bool isOK = false;

  if (theCurve != NULL) {
    clear();

    // Set redo.
    myPRedo = new CurveCreator_Operation;

    if (myPRedo->init(theType, theParamList1)) {
      // Construct undo for different commands.
      switch (theType) {
        case CurveCreator_Operation::RemovePoints:
          {
            // Construct undo for RemovePoints command.
            CurveCreator_ICurve::SectionToPointCoordsList aSectionToPointCoords;
            CurveCreator::Coordinates aPointsToAdd;
            const CurveCreator::Dimension aDim = theCurve->getDimension();
            CurveCreator_ICurve::SectionToPointList::const_iterator anIt = theParamList1.begin(), aLast = theParamList1.end();
            std::list<int> aPoints;
            int aSectionId, aPointId;
            for ( ; anIt != aLast; anIt++ ) {
              aPointsToAdd.clear();
              aSectionId = anIt->first;
              aPointId = anIt->second;
              const CurveCreator::Coordinates &aPoints =
                      theCurve->getPoints(aSectionId);
              CurveCreator::Coordinates::const_iterator anIterBegin =
                  aPoints.begin() + (aDim*aPointId);
              CurveCreator::Coordinates::const_iterator anIterEnd = 
                anIterBegin + aDim;
              aPointsToAdd.insert(aPointsToAdd.end(), anIterBegin, anIterEnd);
              aSectionToPointCoords.push_back(std::make_pair(*anIt, aPointsToAdd));
            }
            setNbUndos(1);
            isOK = myPUndo[0].init(CurveCreator_Operation::InsertPoints,
                                   aSectionToPointCoords);
          }
          break;
        default:
          break;
      }
    }

    if (!isOK) {
      clear();
    }
  }

  return isOK;
}

bool CurveCreator_Diff::init(const CurveCreator_Curve *theCurve,
                             const CurveCreator_Operation::Type theType,
                             const CurveCreator_ICurve::SectionToPointCoordsList &theParamList1)
{
  bool isOK = false;

  if (theCurve != NULL) {
    clear();

    // Set redo.
    myPRedo = new CurveCreator_Operation;

    if (myPRedo->init(theType, theParamList1)) {
      // Construct undo for different commands.
      switch (theType) {
        case CurveCreator_Operation::InsertPoints:
          {
            // Construct undo for RemovePoints command.
            CurveCreator_ICurve::SectionToPointList aSectionToPointList;
            CurveCreator_ICurve::SectionToPointCoordsList::const_iterator anIt = theParamList1.begin(), aLast = theParamList1.end();
            for ( ; anIt != aLast; anIt++ ) {
              aSectionToPointList.push_back(anIt->first);
            }
            setNbUndos(1);
            isOK = myPUndo[0].init(CurveCreator_Operation::RemovePoints,
                                   aSectionToPointList);
          }
          break;
        case CurveCreator_Operation::SetCoordinates:
          {
            // Construct undo for SetCoordinates command.
            CurveCreator_ICurve::SectionToPointCoordsList aSectionToPointOldCoords;
            CurveCreator_ICurve::SectionToPointCoordsList::const_iterator anIt = theParamList1.begin(), aLast = theParamList1.end();
            for ( ; anIt != aLast; anIt++ ) {
              CurveCreator::Coordinates anOldCoords = theCurve->getPoint(anIt->first.first, anIt->first.second);
              aSectionToPointOldCoords.push_back(std::make_pair(anIt->first, anOldCoords));
            }

            setNbUndos(1);
            isOK = myPUndo[0].init(CurveCreator_Operation::SetCoordinates,
                                   aSectionToPointOldCoords);
          }
          break;
        default:
          break;
      }
    }

    if (!isOK) {
      clear();
    }
  }

  return isOK;
}

bool CurveCreator_Diff::init(const CurveCreator_Curve *theCurve,
                             const CurveCreator_ICurve::SectionToPointCoordsList &theOldParamList)
{
  bool isOK = false;

  if (theCurve != NULL && theOldParamList.size() > 0) {
    clear();

    // Set redo.
    myPRedo = new CurveCreator_Operation;

    // Construct redo for SetCoordinates command.
    CurveCreator_ICurve::SectionToPointCoordsList aSectionToPointActualCoords;
    CurveCreator_ICurve::SectionToPointCoordsList::const_iterator anIt = 
      theOldParamList.begin(), aLast = theOldParamList.end();
    for ( ; anIt != aLast; anIt++ ) {
      CurveCreator::Coordinates anActualCoords = theCurve->getPoint(anIt->first.first, anIt->first.second);
      aSectionToPointActualCoords.push_back(std::make_pair(anIt->first, anActualCoords));
    }

    if (myPRedo->init(CurveCreator_Operation::SetCoordinates, aSectionToPointActualCoords)) {
      // Undo for SetCoordinates command.
      setNbUndos(1);
      isOK = myPUndo[0].init(CurveCreator_Operation::SetCoordinates,
                             theOldParamList);
    }

    if (!isOK) {
      clear();
    }
  }

  return isOK;
}

//=======================================================================
// function: applyUndo
// purpose:
//=======================================================================
void CurveCreator_Diff::applyUndo(CurveCreator_Curve *theCurve)
{
  if (myNbUndos > 0 && myPUndo != NULL) {
    for (int i = 0; i < myNbUndos; i++) {
      myPUndo[i].apply(theCurve);
    }
  }
}

//=======================================================================
// function: applyRedo
// purpose:
//=======================================================================
void CurveCreator_Diff::applyRedo(CurveCreator_Curve *theCurve)
{
  if (myPRedo != NULL) {
    myPRedo->apply(theCurve);
  }
}

//=======================================================================
// function: clear
// purpose:
//=======================================================================
void CurveCreator_Diff::clear()
{
  if (myPUndo != NULL) {
    delete [] myPUndo;
    myPUndo = NULL;
  }

  myNbUndos = 0;

  if (myPRedo != NULL) {
    delete myPRedo;
    myPRedo = NULL;
  }
}

//=======================================================================
// function: setNbUndos
// purpose:
//=======================================================================
void CurveCreator_Diff::setNbUndos(const int theNbUndos)
{
  myNbUndos = theNbUndos;
  myPUndo = new CurveCreator_Operation[myNbUndos];
}

//=======================================================================
// function: getSectionIndex
// purpose:
//=======================================================================
int CurveCreator_Diff::getSectionIndex(const CurveCreator_Curve *theCurve,
                                       const int theIndex) const
{
  return (theIndex == -1 ? theCurve->getNbSections() - 1 : theIndex);
}

//=======================================================================
// function: addSectionToUndo
// purpose:
//=======================================================================
bool CurveCreator_Diff::addSectionToUndo
                      (const CurveCreator_Curve *theCurve,
                       const int theIndex,
                       CurveCreator_Operation &theOperation) const
{
  const std::string aName = theCurve->getSectionName(theIndex);
  const CurveCreator::Coordinates &aPnts = theCurve->getPoints(theIndex);
  const CurveCreator::SectionType aType = theCurve->getSectionType(theIndex);
  const bool isClosed = theCurve->isClosed(theIndex);

  bool isOK = theOperation.init(CurveCreator_Operation::AddSection,
                                aName, aPnts, aType, isClosed);

  return isOK;
}

//=======================================================================
// function: setTypeOrClosedToUndo
// purpose:
//=======================================================================
bool CurveCreator_Diff::setTypeOrClosedToUndo
                            (const CurveCreator_Curve *theCurve,
                             const CurveCreator_Operation::Type theType,
                             const int theIntParam1,
                             const int theIntParam2)
{
  bool isOK = true;

  // Compute number of modified sections.
  const bool isSetType = (theType == CurveCreator_Operation::SetType);
  int aNbModif = 0;
  std::list<int> aListOfInd;
  int aValue;
  int i;

  if (theIntParam2 == -1) {
    // The operation is applied to all sections. We need to collect
    // really modified sections for undo.
    const int aNbSections = theCurve->getNbSections();

    if (aNbSections > 0) {
      // Get sections to be modified.
      for (i = 0; i < aNbSections; i++) {
        if (isSetType) {
          aValue = theCurve->getSectionType(i);
        } else {
          aValue = theCurve->isClosed(i);
        }

        if (theIntParam1 != aValue) {
          aNbModif++;
          aListOfInd.push_back(i);
        }
      }

      if (aNbSections == aNbModif) {
        // All sections are modified. We can use one single command
        // with -1 section index.
        aNbModif = 1;
        aListOfInd.clear();
        aListOfInd.push_back(-1);
      }
    }
  } else {
    // There is only particular section modified.
    // Check if there is a real modification required.
    if (isSetType) {
      aValue = theCurve->getSectionType(theIntParam2);
    } else {
      aValue = theCurve->isClosed(theIntParam2);
    }

    if (theIntParam1 != aValue) {
      aNbModif = 1;
      aListOfInd.push_back(theIntParam2);
    }
  }

  if (aNbModif > 0) {
    // Store the undos
    std::list<int>::iterator anIter = aListOfInd.begin();

    if (isSetType) {
      aValue = theCurve->getSectionType(*anIter);
    } else {
      aValue = theCurve->isClosed(*anIter);
    }

    setNbUndos(aNbModif);

    for (i = 0; anIter != aListOfInd.end() && isOK; i++, anIter++) {
      isOK = myPUndo[i].init(theType, aValue, *anIter);
    }
  }

  return isOK;
}
