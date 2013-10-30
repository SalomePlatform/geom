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

// File:        CurveCreator_CurveEditor.cxx
// Author:      Sergey KHROMOV

#include "CurveCreator_CurveEditor.hxx"

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
CurveCreator_CurveEditor::CurveCreator_CurveEditor
                                (CurveCreator_Curve* thePCurve)
 : myNbUndos   (0),
   myNbRedos   (0),
   myPCurve    (thePCurve),
   myUndoDepth (-1),
   myOpLevel(0)
{
  if (myPCurve != NULL) {
    if (myPCurve->isLocked()) {
      // This curve is locked by another editor. Invalid case.
      myPCurve = NULL;
    } else {
      // Lock the curve.
      myPCurve->myIsLocked = true;
      myCurrenPos = myListDiffs.end();
    }
  }
}

//=======================================================================
// function: Destructor
// purpose:
//=======================================================================
CurveCreator_CurveEditor::~CurveCreator_CurveEditor()
{
  if (myPCurve != NULL) {
    // Unlock the curve.
    myPCurve->myIsLocked = false;
  }
}

//=======================================================================
// function: getCurve
// purpose:
//=======================================================================
CurveCreator_Curve *CurveCreator_CurveEditor::getCurve() const
{
  return myPCurve;
}

//=======================================================================
// function: isAttached
// purpose:
//=======================================================================
bool CurveCreator_CurveEditor::isAttached() const
{
  return (myPCurve != NULL);
}

//=======================================================================
// function: undo
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::undo()
{
  if (myNbUndos > 0) {
    myNbUndos--;
    myNbRedos++;
    myCurrenPos--;
    myCurrenPos->applyUndo(myPCurve);
  }
}

//=======================================================================
// function: redo
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::redo()
{
  if (myNbRedos > 0) {
    myCurrenPos->applyRedo(myPCurve);
    myCurrenPos++;
    myNbRedos--;
    myNbUndos++;
  }
}

//=======================================================================
// function: getNbUndo
// purpose:
//=======================================================================
int CurveCreator_CurveEditor::getNbUndo() const
{
  return myNbUndos;
}

//=======================================================================
// function: getNbRedo
// purpose:
//=======================================================================
int CurveCreator_CurveEditor::getNbRedo() const
{
  return myNbRedos;
}

//=======================================================================
// function: setUndoDepth
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::setUndoDepth(const int theDepth)
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
int CurveCreator_CurveEditor::getUndoDepth() const
{
  return myUndoDepth;
}

//=======================================================================
// function: setType
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::setType(const CurveCreator::Type theType,
                                       const int theISection)
{
  if (myPCurve != NULL) {
    startOperation();
    // Set the difference.
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::SetType,
                              theType, theISection);
    }

    // Update the curve.
    myPCurve->setType(theType, theISection);
    finishOperation();
  }
}

//=======================================================================
// function: addPoints
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::addPoints
                      (const CurveCreator::Coordinates &thePoints,
                       const int theISection)
{
  if (myPCurve != NULL) {
    // Set the difference.
    startOperation();
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::AddPoints,
                              thePoints, theISection);
    }

    // Update the curve.
    myPCurve->addPoints(thePoints, theISection);
    finishOperation();
  }
}

//=======================================================================
// function: addSection
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::addSection
        (const std::string& theName, const CurveCreator::Type theType,
         const bool theIsClosed,
         const CurveCreator::Coordinates &thePoints)
{
  if (myPCurve != NULL) {
    // Set the difference.
    startOperation();
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::AddSection,
                              theName, thePoints, theType, theIsClosed);
    }

    // Update the curve.
    myPCurve->addSection(theName, theType, theIsClosed, thePoints);
    finishOperation();
  }
}

//=======================================================================
// function: removeSection
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::removeSection(const int theISection)
{
  if (myPCurve != NULL) {
    // Set the difference.
    startOperation();
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::RemoveSection,
                              theISection);
    }

    // Update the curve.
    myPCurve->removeSection(theISection);
    finishOperation();
  }
}

//=======================================================================
// function: insertPoints
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::insertPoints
        (const CurveCreator::Coordinates &thePoints,
         const int theISection,
         const int theIPnt)
{
  if (myPCurve != NULL) {
    // Set the difference.
    startOperation();
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::InsertPoints,
                              thePoints, theISection, theIPnt);
    }

    // Update the curve.
    myPCurve->insertPoints(thePoints, theISection, theIPnt);
    finishOperation();
  }
}

//=======================================================================
// function: movePoints
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::movePoint(const int theISection,
                const int theOrigIPnt,
                const int theNewIPnt )
{
    startOperation();
    myPCurve->movePoint(theISection, theOrigIPnt, theNewIPnt);
    finishOperation();
}

//=======================================================================
// function: removePoints
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::removePoints
              (const int theISection,
               const int theIPnt,
               const int theNbPoints)
{
  if (myPCurve != NULL) {
    // Set the difference.
    startOperation();
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::RemovePoints,
                              theISection, theIPnt, theNbPoints);
    }

    // Update the curve.
    myPCurve->removePoints(theISection, theIPnt, theNbPoints);
    finishOperation();
  }
}

//=======================================================================
// function: clear
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::clear()
{
  if (myPCurve != NULL) {
    startOperation();
    // Set the difference.
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::Clear);
    }

    // Update the curve.
    myPCurve->clear();
    finishOperation();
  }
}

//=======================================================================
// function: setCoordinates
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::setCoordinates
                     (const CurveCreator::Coordinates &theCoords,
                      const int theISection,
                      const int theIPnt)
{
  if (myPCurve != NULL) {
    // Set the difference.
    startOperation();
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::SetCoordinates,
                              theCoords, theISection, theIPnt);
    }

    // Update the curve.
    myPCurve->setCoordinates(theCoords, theISection, theIPnt);
    finishOperation();
  }
}

//=======================================================================
// function: setClosed
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::setClosed(const bool theIsClosed,
                                         const int theISection)
{
  if (myPCurve != NULL) {
    // Set the difference.
    startOperation();
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::SetClosed,
                              theIsClosed, theISection);
    }

    // Update the curve.
    myPCurve->setClosed(theIsClosed, theISection);
    finishOperation();
  }
}

//=======================================================================
// function: setName
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::setName(const std::string& theName,
                                         const int theISection)
{
    if (myPCurve != NULL) {
      // Set the difference.
      startOperation();
      if (addEmptyDiff()) {
        myListDiffs.back().init(myPCurve, CurveCreator_Operation::RenameSection,
                                theName, theISection);
      }
      myPCurve->setName( theName, theISection );
      finishOperation();
    }
}

//=======================================================================
// function: moveSection
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::moveSection(const int theISection,
                                           const int theNewIndex)
{
  if (myPCurve != NULL) {
    // Set the difference.
    startOperation();
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::MoveSection,
                              theISection, theNewIndex);
    }

    // Update the curve.
    myPCurve->moveSection(theISection, theNewIndex);
    finishOperation();
  }
}

//=======================================================================
// function: join
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::join(const int theISectionTo,
                                    const int theISectionFrom)
{
  if (myPCurve != NULL) {
    // Set the difference.
    startOperation();
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::Join,
                              theISectionTo, theISectionFrom);
    }

    // Update the curve.
    myPCurve->join(theISectionTo, theISectionFrom);
    finishOperation();
  }
}

//=======================================================================
// function: join
// purpose:
//=======================================================================
void CurveCreator_CurveEditor::join()
{
  if (myPCurve != NULL) {
    // Set the difference.
    startOperation();
    if (addEmptyDiff()) {
      myListDiffs.back().init(myPCurve, CurveCreator_Operation::Join);
    }

    // Update the curve.
    myPCurve->join();
    finishOperation();
  }
}

//=======================================================================
// function: addDiff
// purpose:
//=======================================================================
bool CurveCreator_CurveEditor::addEmptyDiff()
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

void CurveCreator_CurveEditor::startOperation()
{
    myOpLevel++;
}

void CurveCreator_CurveEditor::finishOperation()
{
   myOpLevel--;
}
