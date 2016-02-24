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

// File:        CurveCreator_ICurve.hxx
// Author:      Alexander KOVALEV and Alexander SOLOVYOV

#ifndef _CurveCreator_ICurve_HeaderFile
#define _CurveCreator_ICurve_HeaderFile

#include "CurveCreator.hxx"
#include "CurveCreator_Macro.hxx"

#include <TColgp_HArray1OfPnt.hxx>
#include <AIS_InteractiveObject.hxx>

#include <deque>
#include <vector>
#include <string>
#include <list>

namespace CurveCreator
{
  //! Type of the section
  enum SectionType
  {
    Polyline,
    Spline,
  };

  //! Dimension of the curve
  enum Dimension
  {
    Dim2d = 2,
    Dim3d = 3
  };

};

//! The type represents the interface to the curve section.
struct CURVECREATOR_EXPORT CurveCreator_ISection
{
  //! The destructor.
  virtual ~CurveCreator_ISection() {}

  //! Calculates the different points of the section.
  virtual void GetDifferentPoints(
    const int theDimension, Handle(TColgp_HArray1OfPnt)& thePoints) const = 0;
};

/**
 *  The CurveCreator_ICurve object is represented as one or more sets of
 *  connected points; thus CurveCreator_ICurve object can contain several
 *  not connected curves (polylines or b-splines), each such curve has two
 *  only ends "start and end points" in other words non-manifold curves
 *  are not supported.
 */
class CURVECREATOR_EXPORT CurveCreator_ICurve
{
public:
  typedef std::pair<int,int> SectionToPoint;
  typedef std::deque<SectionToPoint> SectionToPointList;

  typedef std::deque< std::pair< SectionToPoint, CurveCreator::Coordinates > > SectionToPointCoordsList;

public:
  /***********************************************/
  /***          Undo/Redo methods              ***/
  /***********************************************/

  //! The destructor.
  virtual ~CurveCreator_ICurve() {}

  //! Get number of available undo operations
  virtual int getNbUndo() const = 0;

  //! Undo previous operation
  virtual bool undo() = 0;

  //! Get number of available redo operations
  virtual int getNbRedo() const = 0;

  //! Redo last previously "undone" operation
  virtual bool redo() = 0;


  /***********************************************/
  /***           Section methods               ***/
  /***********************************************/

  //! Clear the polyline (remove all sections)
  virtual bool clear() = 0;

  //! Join list of sections to one section (join all if the list is empty)
  // The first section in the list is a leader, another sections are joined to it
  virtual bool join( const std::list<int>& theSections ) = 0;

  //! Get number of sections
  virtual int getNbSections() const = 0;

  //! Add a new section.
  virtual int addSection( const std::string& theName, 
                           const CurveCreator::SectionType theType,
                           const bool theIsClosed ) = 0;

  //! Removes the given sections.
  virtual bool removeSection( const int theISection ) = 0;

  //! Get "closed" flag of the specified section
  virtual bool isClosed( const int theISection ) const = 0;

  /**
   *  Set "closed" flag of the specified section (all sections if
   *  \a theISection is -1).
   */
  virtual bool setClosed( const int theISection, 
                          const bool theIsClosed ) = 0;

  //! Returns specifyed section name
  virtual std::string getSectionName( const int theISection ) const = 0;

  /** Set name of the specified section */
  virtual bool setSectionName( const int theISection, 
                               const std::string& theName ) = 0;

  //! Get type of the specified section
  virtual CurveCreator::SectionType getSectionType( const int theISection ) const = 0;

  /**
   *  Set type of the specified section (or all sections
   *  if \a theISection is -1).
   */
  virtual bool setSectionType( const int theISection, 
                               const CurveCreator::SectionType theType ) = 0;

  //! Returns the curve section with the index.
  virtual const CurveCreator_ISection* getSection(
    const int theSectionIndex) const = 0;

  //! Returns the curve section with the index.
  virtual CurveCreator_ISection* getSection(const int theSectionIndex) = 0;


  /***********************************************/
  /***           Point methods                 ***/
  /***********************************************/

  //! Get the dimension.
  virtual CurveCreator::Dimension getDimension() const = 0;

  /**
   *  Insert one or several points to the specified section starting from the given theIPnt index
   *  (or add these at the end of section points if \a theIPnt is -1).
   */
  virtual bool addPoints( const CurveCreator::Coordinates& theCoords,
                          const int theISection,
                          const int theIPnt = -1 ) = 0;

  //! Set coordinates of specified point
  virtual bool setPoint( const int theISection,
                         const int theIPnt,
                         const CurveCreator::Coordinates& theNewCoords ) = 0;
  
  //! Set coordinates of specified points from different sections
  virtual bool setSeveralPoints( const SectionToPointCoordsList &theSectionToPntCoords,
                                 const bool theIsToSaveDiff = true ) = 0;

  //! Remove point with given id
  virtual bool removePoint( const int theISection, const int theIPnt = -1 ) = 0;
  //! Remove several points from different sections
  virtual bool removeSeveralPoints( const SectionToPointList &theSectionToPntIDs) = 0;

  //! Get coordinates of specified point
  virtual CurveCreator::Coordinates getPoint( const int theISection, 
                                      const int theIPnt ) const = 0;

  /**
   * Get points of a section (the total points in Curve if theISection is equal to -1)..
   */
  virtual CurveCreator::Coordinates getPoints( const int theISection = -1 ) const = 0;

  /**
   *  Get number of points in specified section or (the total number of points
   *  in Curve if theISection is equal to -1).
   */
  virtual int getNbPoints( const int theISection ) const = 0;

  /**
   * Set skip sorting flag. If the flag is true - points sorting will be skipped.
   */
  virtual void setSkipSorting( const bool ) = 0;

  /**
   * Indicates whether the points can be sorted.
   */
  virtual bool canPointsBeSorted() = 0;

  /**
   * Saves points coordinates difference.
   * \param theOldCoords the old points coordinates
   */
  virtual void saveCoordDiff( const SectionToPointCoordsList &theOldCoords ) = 0;

  /***********************************************/
  /***       Presentation methods              ***/
  /***********************************************/

  virtual Handle(AIS_InteractiveObject) getAISObject( const bool theNeedToBuild = false ) const = 0;

protected:
  virtual void constructAISObject() = 0;

};

#endif
