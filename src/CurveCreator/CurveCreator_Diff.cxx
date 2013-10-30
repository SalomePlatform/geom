// Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
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
bool CurveCreator_Diff::init(const CurveCreator_Curve *theCurve,
                             const CurveCreator_Operation::Type theType)
{
  bool isOK = false;

  if (theCurve != NULL) {
    clear();

    // Set redo.
    myPRedo = new CurveCreator_Operation;

    if (myPRedo->init(theType)) {
      isOK = true;

      const int aNbSections = theCurve->getNbSections();

      if (theType == CurveCreator_Operation::Clear) {
        // Construct undo for Clear command.
        if (aNbSections > 0) {
          setNbUndos(aNbSections);

          for (int i = 0; i < aNbSections && isOK; i++) {
            // Add AddSection command.
            isOK = addSectionToUndo(theCurve, i, myPUndo[i]);
          }
        }
      } else { // theType == CurveCreator_Operation::Join
        // Construct undo for Join command.
        if (aNbSections > 1) {
          // Add the RemovePoints command to remove points of
          // the second section fron the first one.
          const int aNbPoints = theCurve->getNbPoints(0);

          setNbUndos(aNbSections);
          isOK = myPUndo[0].init(CurveCreator_Operation::RemovePoints,
                                 0, aNbPoints, -1);

          for (int i = 1; i < aNbSections && isOK; i++) {
            // Add AddSection command.
            isOK = addSectionToUndo(theCurve, i, myPUndo[i]);
          }
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
        case CurveCreator_Operation::Join:
          {
            // If the last section is removed, one AddSection command is
            // enough. If not last section is removed, two commands are
            // requred: AddSection and MoveSection.
            const int aLastIndex = theCurve->getNbSections() - 1;
            const int aNbPoints  = theCurve->getNbPoints(theIntParam1);

            if (theIntParam2 == aLastIndex) {
              setNbUndos(2);
            } else {
              setNbUndos(3);
            }

            isOK = myPUndo[0].init(CurveCreator_Operation::RemovePoints,
                                   theIntParam1, aNbPoints, -1);

            if (isOK) {
              isOK = addSectionToUndo(theCurve, theIntParam2, myPUndo[1]);

              if (isOK && theIntParam2 != aLastIndex) {
                isOK = myPUndo[2].init(CurveCreator_Operation::MoveSection,
                                       aLastIndex, theIntParam2);
              }
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
                             const int theIntParam1,
                             const int theIntParam2,
                             const int theIntParam3)
{
  bool isOK = false;

  if (theCurve != NULL) {
    clear();

    // Set redo.
    myPRedo = new CurveCreator_Operation;

    if (myPRedo->init(theType, theIntParam1, theIntParam2, theIntParam3)) {
      // Construct undo for RemovePoints command.
      const CurveCreator::Dimension aDim = theCurve->getDimension();
      const CurveCreator::Coordinates &aPoints =
              theCurve->getPoints(theIntParam1);
      CurveCreator::Coordinates::const_iterator anIterBegin =
          aPoints.begin() + (aDim*theIntParam2);
      CurveCreator::Coordinates::const_iterator anIterEnd;

      if (theIntParam3 == -1) {
        anIterEnd = aPoints.end();
      } else {
        anIterEnd = anIterBegin + (aDim*theIntParam3);
      }

      CurveCreator::Coordinates aPointsToAdd;

      setNbUndos(1);
      aPointsToAdd.insert(aPointsToAdd.end(), anIterBegin, anIterEnd);
      isOK = myPUndo[0].init(CurveCreator_Operation::InsertPoints,
                             aPointsToAdd, theIntParam1, theIntParam2);
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
                             const CurveCreator::Coordinates &theCoords,
                             const int theIntParam)
{
  bool isOK = false;

  if (theCurve != NULL) {
    clear();

    // Set redo.
    myPRedo = new CurveCreator_Operation;

    if (myPRedo->init(theType, theCoords, theIntParam)) {
      // Construct undo for AddPoints command.
      const int aSectionInd = getSectionIndex(theCurve, theIntParam);
      const CurveCreator::Dimension aDim = theCurve->getDimension();
      const CurveCreator::Coordinates &aPoints =
              theCurve->getPoints(aSectionInd);
      const int aNbPoints = (aPoints.size()/aDim);

      setNbUndos(1);
      isOK = myPUndo[0].init(CurveCreator_Operation::RemovePoints,
                             aSectionInd, aNbPoints, -1);
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

//=======================================================================
// function: init
// purpose:
//=======================================================================
bool CurveCreator_Diff::init(const CurveCreator_Curve *theCurve,
                             const CurveCreator_Operation::Type theType,
                             const CurveCreator::Coordinates &theCoords,
                             const int theIntParam1,
                             const int theIntParam2)
{
  bool isOK = false;

  if (theCurve != NULL) {
    clear();

    // Set redo.
    myPRedo = new CurveCreator_Operation;

    if (myPRedo->init(theType, theCoords, theIntParam1, theIntParam2)) {
      // Construct undo for different commands.
      switch (theType) {
        case CurveCreator_Operation::InsertPoints:
          {
            const CurveCreator::Dimension aDim = theCurve->getDimension();
            const int aNbPoints = (theCoords.size()/aDim);
            const int aSectionInd = getSectionIndex(theCurve, theIntParam1);
            int aPointInd;

            if (theIntParam2 == -1) {
              aPointInd = theCurve->getNbPoints(aSectionInd);
            } else {
              aPointInd = theIntParam2;
            }

            setNbUndos(1);
            isOK = myPUndo[0].init(CurveCreator_Operation::RemovePoints,
                                   aSectionInd, aPointInd, aNbPoints);
          }
          break;
        case CurveCreator_Operation::SetCoordinates:
          {
            const CurveCreator::Coordinates anOldCoords =
              theCurve->getCoordinates(theIntParam1, theIntParam2);

            setNbUndos(1);
            isOK = myPUndo[0].init(CurveCreator_Operation::SetCoordinates,
                                   anOldCoords, theIntParam1, theIntParam2);
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
  const CurveCreator::Coordinates &aPnts = theCurve->getPoints(theIndex);
  const CurveCreator::Type aType = theCurve->getType(theIndex);
  const bool isClosed = theCurve->isClosed(theIndex);

  bool isOK = theOperation.init(CurveCreator_Operation::AddSection,
                                aPnts, aType, isClosed);

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
          aValue = theCurve->getType(i);
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
      aValue = theCurve->getType(theIntParam2);
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
      aValue = theCurve->getType(*anIter);
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
