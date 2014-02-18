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

// File:        CurveCreator_CurveEditor.hxx
// Author:      Sergey KHROMOV

#ifndef _CurveCreator_CurveEditor_HeaderFile
#define _CurveCreator_CurveEditor_HeaderFile

#include "CurveCreator_Diff.hxx"
#include "CurveCreator_Curve.hxx"

#include <list>

/**
 *  The CurveCreator_CurveEditor is designed to manage of
 *  editing operations of CurveCreator_Curve class.
 */
class CURVECREATOR_EXPORT CurveCreator_CurveEditor
{

private:

  typedef std::list<CurveCreator_Diff> ListDiff;

public:

  //! Constuctor, initialized by the curve object
  CurveCreator_CurveEditor(CurveCreator_Curve* thePCurve);

  //! Destructor, detaches from the Curve
  ~CurveCreator_CurveEditor();

  //! Returns the curve.
  CurveCreator_Curve *getCurve() const;

  //! This method returns true if this editor is attached to a valid curve.
  bool isAttached() const;

  //! Undo previous operation
  void undo();

  //! Redo last previously �undoed� operation
  void redo();

  //! Get number of available undo operations
  int getNbUndo() const;

  //! Get number of available redo operations
  int getNbRedo() const;

  //! Set depth of undo operations (unlimited if \a theDepth is -1
  //  or disabled if \a theDepth is 0)
  void setUndoDepth(const int theDepth = -1);

  //! Get depth of undo operations.
  int getUndoDepth() const;

  /** Set type of the specified section (or all sections
   *  if \a theISection is -1).
   */
  void setType(const CurveCreator::Type theType, const int theISection = -1);

  /** Set section closed (or all sections
   *  if \a theISection is -1).
   */
  void setClosed(const bool theIsClosed, const int theISection);

  /** Set section name (if theISection is invalid it is ignored).
   */
  void setName(const std::string& theName, const int theISection);

  /** Add points to the specified section (or last section
   *  if \a theISection is -1).
   */
  void addPoints(const CurveCreator::Coordinates &thePoints,
                 const int theISection = -1);

  //! Add a new section.
  void addSection(const std::string &theName, const CurveCreator::Type theType,
                  const bool theIsClosed,
                  const CurveCreator::Coordinates &thePoints);

  //! Removes the section. If theISection equals -1, removes the last section.
  void removeSection(const int theISection = -1);

  /** Insert points in the given position (add to the end of list
   *  if \a theIPnt parameter is -1) of the specified section
   *  (or last section if \a theISection parameter is -1).
   */
  void insertPoints(const CurveCreator::Coordinates &thePoints,
                    const int theISection = -1,
                    const int theIPnt = -1);

  /** Remove \a nbPoints points from given \a theISection,
   *  starting from given \a theIPnt (of all points up to the end of
   *  section if \a theNbPoints is -1).
   */
  void removePoints(const int theISection,
                    const int theIPnt,
                    const int theNbPoints = -1);

  /** Mobe point in \a theISection from given position \a theOrigIPnt
   *  to new position \a theNewIPnt.
   */
  void movePoint(const int theISection,
                  const int theOrigIPnt,
                  const int theNewIPnt );

  //! Remove all sections.
  void clear();

  //! Set coordinates of specified point
  void setCoordinates(const CurveCreator::Coordinates &theCoords,
                      const int theISection,
                      const int theIPnt);

  /** Move specified \a theISection to the specified position
   *  in the sections list.
   */
  void moveSection(const int theISection, const int theNewIndex);

  //! Join two sections to one section
  void join(const int theISectionTo, const int theISectionFrom);

  //! Join all sections to the single curve
  void join();

  void startOperation();
  void finishOperation();
private:

  /** This method updates all undo/redo information required to be updated
   *  after curve modification operation. It returns false if undo/redo
   *  is disabled and true otherwise.
   */
  bool addEmptyDiff();

private:

  int myNbUndos;
  int myNbRedos;
  ListDiff::iterator myCurrenPos;
  ListDiff myListDiffs;
  CurveCreator_Curve* myPCurve;
  int myUndoDepth;
  int myOpLevel;
};

#endif
