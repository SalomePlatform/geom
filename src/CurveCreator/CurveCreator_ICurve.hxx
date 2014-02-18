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

// File:        CurveCreator_Curve.hxx
// Author:      Sergey KHROMOV

#ifndef _CurveCreator_ICurve_HeaderFile
#define _CurveCreator_ICurve_HeaderFile

#include "CurveCreator.hxx"
#include "CurveCreator_Macro.hxx"
#include "CurveCreator_Operation.hxx"

class CurveCreator_Section;
class CurveCreator_Listener;

/**
 *  The CurveCreator_ICurve object is represented as one or more sets of
 *  connected points; thus CurveCreator_ICurve object can contain several
 *  not connected curves (polylines or b-splines), each such curve has two
 *  only ends � start and end points � in other words non-manifold curves
 *  are not supported.
 */
class CURVECREATOR_EXPORT CurveCreator_ICurve
{

  //! List of curves
  typedef std::deque<CurveCreator_Section *> Sections;

public:
  //! Constructor of the curve.
  /** The dimension is explicitly specified in the constructor
   *  and cannot be changed later.
   */
  CurveCreator_ICurve(const CurveCreator::Dimension theDimension);

  //! Destructor.
  virtual ~CurveCreator_ICurve();

  //! Returns true if this curve is locked by a curve editor.
  virtual bool isLocked() const;

  //! Get the dimension.
  virtual CurveCreator::Dimension getDimension() const;

  //! Get number of sections.
  virtual int getNbSections() const;

  /** Get number of points in specified section or (the total number of points
   *  in Curve if theISection is equal to -1).
   */
  virtual int getNbPoints(const int theISection = -1) const;

  //! Get coordinates of specified point
  virtual CurveCreator::Coordinates getCoordinates
                  (const int theISection, const int theIPnt) const;

  //! Get points of a section.
  virtual const CurveCreator::Coordinates &getPoints(const int theISection) const;

  //! Get type of the specified section
  virtual CurveCreator::Type getType(const int theISection) const;

  //! Get �closed� flag of the specified section
  virtual bool isClosed(const int theISection) const;

  //! Returns specifyed section name
  virtual std::string   getSectionName(const int theISection) const;

  /**
   * Return unic section name
   */
  virtual std::string getUnicSectionName();

  /**
   * Set curve creator listener object
   */
  virtual void setListener( CurveCreator_Listener*   myWatcher );

  /**
   * Remove curve creator listener object
   */
  virtual void removeListener();

protected:

  /** Set type of the specified section (or all sections
   *  if \a theISection is -1).
   */
  virtual void setType(const CurveCreator::Type theType, const int theISection = -1);

  /** Add points to the specified section (or last section
   *  if \a theISection is -1).
   */
  virtual void addPoints
    (const CurveCreator::Coordinates &thePoints, const int theISection = -1) = 0;

  //! Add a new section.
  virtual void addSection (const std::string &theName, const CurveCreator::Type theType,
                   const bool theIsClosed,
                   const CurveCreator::Coordinates &thePoints);

  //! Removes the section. If theISection equals -1, removes the last section.
  virtual void removeSection(const int theISection = -1);

  /** Insert points in the given position (add to the end of list
   *  if \a theIPnt parameter is -1) of the specified section
   *  (or last section if \a theISection parameter is -1).
   */
  virtual void insertPoints(const CurveCreator::Coordinates &thePoints,
                    const int theISection = -1,
                    const int theIPnt = -1);

  /** Remove \a nbPoints points from given \a theISection,
   *  starting from given \a theIPnt (of all points up to the end of
   *  section if \a theNbPoints is -1).
   */
  virtual void removePoints(const int theISection,
                    const int theIPnt,
                    const int theNbPoints = -1);

  /** Move specified  point within section to new position
   */
  virtual void movePoint(const int theISection,
                 const int theIPointFrom,
                 const int theNewIndex);

  //! Remove all sections.
  virtual void clear();

  //! Set coordinates of specified point
  virtual void setCoordinates(const CurveCreator::Coordinates &theCoords,
                      const int theISection,
                      const int theIPnt);

  /** Set �closed� flag of the specified section (all sections if
   *  \a theISection is -1).
   */
  virtual void setClosed(const bool theIsClosed, const int theISection = -1);

  /** Set name of the specified section.
   */
  virtual void setName( const std::string& theName, const int theISection );

  /** Move specified \a theISection to the specified position
   *  in the sections list.
   */
  virtual void moveSection(const int theISection, const int theNewIndex);

  //! Join two sections to one section
  virtual void join(const int theISectionTo, const int theISectionFrom);

  //! Join all sections to the single curve
  virtual void join();

  /**
   * This method converts the point index to the index in
   * an array of coordinates.
   */
  virtual int toICoord(const int theIPnt) const;

public:

  bool                    myIsLocked;
  Sections                mySections;   //!< curve data
  CurveCreator::Dimension myDimension;  //!< curve dimension
  CurveCreator_Listener*  myListener;   //!< listener

};

#endif
