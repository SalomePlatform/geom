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

// File:        CurveCreator_Curve.hxx
// Author:      Sergey KHROMOV

#ifndef _CurveCreator_Curve_HeaderFile
#define _CurveCreator_Curve_HeaderFile

#include "CurveCreator_ICurve.hxx"

#include "CurveCreator_Macro.hxx"
#include "CurveCreator.hxx"
#include "CurveCreator_Diff.hxx"

#include <list>
#include <map>

struct CurveCreator_Section;
class CurveCreator_Displayer;
class AIS_Shape;
class AIS_InteractiveObject;

/**
 *  The CurveCreator_Curve object is represented as one or more sets of
 *  connected points; thus CurveCreator_Curve object can contain several
 *  not connected curves (polylines or b-splines), each such curve has two
 *  only ends "start and end points" in other words non-manifold curves
 *  are not supported.
 */
class CURVECREATOR_EXPORT CurveCreator_Curve : public CurveCreator_ICurve
{
protected:
  typedef std::list<CurveCreator_Diff> ListDiff;

public:
  //! Constructor of the curve.
  /** The dimension is explicitly specified in the constructor
   *  and cannot be changed later.
   */
  CurveCreator_Curve(const CurveCreator::Dimension theDimension);

  //! Destructor.
  virtual ~CurveCreator_Curve();

  //! Get the dimension.
  virtual CurveCreator::Dimension getDimension() const;

  //! Return unique section name
  virtual std::string getUniqSectionName() const;

  //! Set curve creator Displayer object
  virtual void setDisplayer( CurveCreator_Displayer* theDisplayer );

  //! Return curve creator Displayer object
  CurveCreator_Displayer* getDisplayer();

  //! Remove curve creator Displayer object
  virtual void removeDisplayer();

  /** Set depth of undo operations (unlimited if \a theDepth is -1
   *  or disabled if \a theDepth is 0)
   */
  virtual void setUndoDepth(const int theDepth = -1);

  //! Get depth of undo operations.
  virtual int getUndoDepth() const;

  virtual void startOperation();
  virtual void finishOperation();

  /**
   * This method converts the point index to the index in
   * an array of coordinates.
   */
  virtual int toICoord(const int theIPnt) const;

  //! For internal use only! Undo/Redo are not used here.
  virtual bool moveSectionInternal(const int theISection,
                           const int theNewIndex);
  //! Move section to new position in list
  virtual bool moveSection(const int theISection,
                   const int theNewIndex);

protected:
  /** This method updates all undo/redo information required to be updated
   *  after curve modification operation. It returns false if undo/redo
   *  is disabled and true otherwise.
   */
  virtual bool addEmptyDiff();

public: // TODO: remove public
  void getCoordinates( int theISection, int theIPoint, double& theX, double& theY, double& theZ ) const;
protected:  // TODO: remove public
  void redisplayCurve();

public:
  /************   Implementation of INTERFACE methods   ************/

  /***********************************************/
  /***          Undo/Redo methods              ***/
  /***********************************************/

  //! Get number of available undo operations
  virtual int getNbUndo() const;

  //! Undo previous operation
  virtual bool undo();

  //! Get number of available redo operations
  virtual int getNbRedo() const;

  //! Redo last previously "undone" operation
  virtual bool redo();


  /***********************************************/
  /***           Section methods               ***/
  /***********************************************/

  //! For internal use only! Undo/Redo are not used here.
  virtual bool clearInternal();
  //! Clear the polyline (remove all sections)
  virtual bool clear();

  //! For internal use only! Undo/Redo are not used here.
  virtual bool joinInternal( const std::list<int>& theSections );

  //! Join list of sections to one section (join all if the list is empty)
  // The first section in the list is a leader, another sections are joined to it
  virtual bool join( const std::list<int>& theSections );

  //! Get number of sections
  virtual int getNbSections() const;

  //! For internal use only! Undo/Redo are not used here.
  virtual int addSectionInternal( const std::string &theName, 
                                  const CurveCreator::SectionType theType,
                                  const bool theIsClosed,
                                  const CurveCreator::Coordinates &thePoints);
  //! Add a new section.
  virtual int addSection( const std::string &theName, 
                           const CurveCreator::SectionType theType,
                           const bool theIsClosed );
  //! Add a new section.
  virtual int addSection( const std::string &theName, 
                           const CurveCreator::SectionType theType,
                           const bool theIsClosed,
                           const CurveCreator::Coordinates &thePoints);
  
  //! For internal use only! Undo/Redo are not used here.
  virtual bool removeSectionInternal( const int theISection );
  //! Removes the given sections.
  virtual bool removeSection( const int theISection );

  //! Get "closed" flag of the specified section
  virtual bool isClosed( const int theISection ) const;

  //! For internal use only! Undo/Redo are not used here.
  virtual bool setClosedInternal( const int theISection, 
                                  const bool theIsClosed );
  /**
   *  Set "closed" flag of the specified section (all sections if
   *  \a theISection is -1).
   */
  virtual bool setClosed( const int theISection, 
                          const bool theIsClosed );

  //! Returns specifyed section name
  virtual std::string getSectionName( const int theISection ) const;

  //! For internal use only! Undo/Redo are not used here.
  virtual bool setSectionNameInternal( const int theISection, 
                                       const std::string& theName );
  /** Set name of the specified section */
  virtual bool setSectionName( const int theISection, 
                               const std::string& theName );

  //! Get type of the specified section
  virtual CurveCreator::SectionType getSectionType( const int theISection ) const;

  //! For internal use only! Undo/Redo are not used here.
  virtual bool setSectionTypeInternal( const int theISection, 
                                       const CurveCreator::SectionType theType );
  /**
   *  Set type of the specified section (or all sections
   *  if \a theISection is -1).
   */
  virtual bool setSectionType( const int theISection, 
                               const CurveCreator::SectionType theType );

  //! A virtual method.
  const CurveCreator_ISection* getSection(const int theSectionIndex) const
  {
    if (theSectionIndex >= 0 && theSectionIndex < mySections.size())
    {
      return (CurveCreator_ISection*)mySections[theSectionIndex];
    }
    return NULL;
  }

  //! A virtual method.
  CurveCreator_ISection* getSection(const int theSectionIndex)
  {
    return (CurveCreator_ISection*)mySections[theSectionIndex];
  }

  /***********************************************/
  /***           Point methods                 ***/
  /***********************************************/

  //! For internal use only! Undo/Redo are not used here.
  virtual bool addPointsInternal( const CurveCreator::SectionsMap &theSectionsMap );
  /**
   *  Add one point to the specified section starting from the given theIPnt index
   *  (or at the end of points if \a theIPnt is -1).
   */
  virtual bool addPoints( const CurveCreator::Coordinates &theCoords,
                          const int theISection,
                          const int theIPnt = -1 );

  //! For internal use only! Undo/Redo are not used here.
  virtual bool setPointInternal( const CurveCreator::SectionsMap &theSectionsMap );
   //! Set coordinates of specified point
  virtual bool setPoint( const int theISection,
                         const int theIPnt,
                         const CurveCreator::Coordinates& theNewCoords );

  //! Set coordinates of specified points from different sections
  virtual bool setSeveralPoints( const SectionToPointCoordsList &theSectionToPntCoords,
                                 const bool theIsToSaveDiff = true );

  //! For internal use only! Undo/Redo are not used here.
  virtual bool removePointsInternal( const SectionToPointList &thePoints );
  /** Remove point with given id */
  virtual bool removePoint( const int theISection, const int theIPnt = -1 );

  //! Remove several points from different sections with given ids
  virtual bool removeSeveralPoints( const SectionToPointList &theSectionToPntIDs);

  //! Get coordinates of specified point
  virtual CurveCreator::Coordinates getPoint( const int theISection, 
                                              const int theIPnt ) const;

  /**
   * Get points of a section (the total points in Curve if theISection is equal to -1)..
   */
  virtual CurveCreator::Coordinates getPoints( const int theISection = -1 ) const;


  /**
   *  Get number of points in specified section or (the total number of points
   *  in Curve if theISection is equal to -1).
   */
  virtual int getNbPoints( const int theISection ) const;

   /**
   * Set skip sorting flag. If the flag is true - points sorting will be skipped.
   */
  virtual void setSkipSorting( const bool theIsToSkip );

  /**
   * Indicates whether the points can be sorted.
   */
  virtual bool canPointsBeSorted();

  /**
   * Saves points coordinates difference.
   * \param theOldCoords the old points coordinates
   */
  virtual void saveCoordDiff( const SectionToPointCoordsList &theOldCoords );

  /***********************************************/
  /***       Presentation methods              ***/
  /***********************************************/
  /**
   *  Get the curve AIS object
   */
  virtual Handle(AIS_InteractiveObject) getAISObject( const bool theNeedToBuild = false ) const;

protected:
  /**
   *  Removes the points from the section. It sortes the points and remove them
   * in the decreasing order
   * \param theSectionId a section index
   * \param thePointIds a list of section points
   */
  bool removeSectionPoints( const int theSectionId,
                            const std::list<int>& thePointIds );
  /**
   * Converts the list of pairs of section to point into map of a section to list of points
   * \param thePoints an source list
   * \param theConvPoints a converted map
   */
  void convert( const SectionToPointList &thePoints,
                std::map<int, std::list<int> > &theConvPoints );

protected:
  virtual void constructAISObject();

protected:
  bool                            mySkipSorting;

public:
  bool                            myIsLocked;
  CurveCreator::Sections          mySections;   //!< curve data
  CurveCreator::Dimension         myDimension;  //!< curve dimension
  CurveCreator_Displayer*         myDisplayer;  //!< curve displayer

private:

  int                             myNbUndos;
  int                             myNbRedos;
  ListDiff::iterator              myCurrenPos;
  ListDiff                        myListDiffs;
  int                             myUndoDepth;
  int                             myOpLevel;
  AIS_Shape*                      myAISShape;   //!< AIS shape
};

#endif
