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

// File:        CurveCreator_Diff.hxx
// Author:      Sergey KHROMOV

#ifndef _CurveCreator_Diff_HeaderFile
#define _CurveCreator_Diff_HeaderFile

#include "CurveCreator_Operation.hxx"

class CurveCreator_Curve;


/**
 * This is the support class for store/retrieve differences of undo/redo
 * operations. To fill the difference it is necessary to create it with
 * an appropriate type and to call the method initialize with required
 * parameters.
 */
class CurveCreator_Diff
{

private:

public:

  /**
   * Constructor.
   */
  CurveCreator_Diff();

  /**
   * Destructor.
   */
  ~CurveCreator_Diff();

  /**
   * This method initializes the difference with an operation without
   * parameters. It is applicable to the following operations:
   * <UL>
   *   <LI>Clear</LI>
   * </UL>
   */
  bool init(const CurveCreator_Curve *theCurve);

  /**
   * This method initializes the difference with an operation with one integer
   * parameter. It is applicable to the following operations:
   * <UL>
   *   <LI>RemoveSection</LI>
   * </UL>
   */
  bool init(const CurveCreator_Curve *theCurve,
            const CurveCreator_Operation::Type theType,
            const int theIntParam);

  /**
   * This method initializes the difference with an operation with two integer
   * parameters. It is applicable to the following operations:
   * <UL>
   *   <LI>SetType</LI>
   *   <LI>SetClosed</LI>
   *   <LI>MoveSection</LI>
   * </UL>
   */
  bool init(const CurveCreator_Curve *theCurve,
            const CurveCreator_Operation::Type theType,
            const int theIntParam1,
            const int theIntParam2);

    /**
   * This method initializes the difference with an operation with two integer
   * parameters. It is applicable to the following operations:
   * <UL>
   *   <LI>Join (with a list of int arguments)</LI>
   * </UL>
   */
  bool init(const CurveCreator_Curve *theCurve,
            const CurveCreator_Operation::Type theType,
            const std::list<int>& theParams);

  /**
   * This method initializes the difference with an operation with one
   * Name, one CurveCreator::Coordinates parameter and two integer parameters.
   * It is applicable to the following operations:
   * <UL>
   *   <LI>AddSection</LI>
   * </UL>
   */
  bool init(const CurveCreator_Curve *theCurve,
            const CurveCreator_Operation::Type theType,
            const std::string& theName,
            const CurveCreator::Coordinates &theCoords,
            const int theIntParam1,
            const int theIntParam2);

  /**
   * This method initializes the difference with an operation with one
   * string and one integer parameters.
   * It is applicable to the following operations:
   * <UL>
   *   <LI>RenameSection</LI>
   * </UL>
   */
  bool init(const CurveCreator_Curve *theCurve,
            const CurveCreator_Operation::Type theType,
            const std::string &theName,
            const int theIntParam1 );

  /**
   * This method initializes the difference with an operation with 
   * list of pairs of integer parameters.
   * It is applicable to the following operations:
   * <UL>
   *   <LI>RemovePoints</LI>
   * </UL>
   */
  bool init(const CurveCreator_Curve *theCurve,
            const CurveCreator_Operation::Type theType,
            const CurveCreator_ICurve::SectionToPointList &theParamList);

  /**
   * This method initializes the difference with an operation with 
   * list of pairs of integer parameters with point coordinates.
   * It is applicable to the following operations:
   * <UL>
   *   <LI>RemovePoints</LI>
   * </UL>
   */
  bool init(const CurveCreator_Curve *theCurve,
            const CurveCreator_Operation::Type theType,
            const CurveCreator_ICurve::SectionToPointCoordsList &theParamList);

  /**
   * This method initializes the difference with an operation with 
   * list of pairs of integer parameters with point coordinates.
   * \param theCurve the modified curve
   * \param theOldParamList the old parameters (to be saved for undo)
   */
  bool init(const CurveCreator_Curve *theCurve,
            const CurveCreator_ICurve::SectionToPointCoordsList &theOldParamList);

  /**
   * This method applies undo operation to theCurve.
   */
  void applyUndo(CurveCreator_Curve *theCurve);

  /**
   * This method applies redo operation to theCurve.
   */
  void applyRedo(CurveCreator_Curve *theCurve);

private:

  /**
   * This method clears initialized data pointers.
   */
  void clear();

  /**
   * This method sets the number of undos and allocates the required
   * space for myPUndo.
   */
  void setNbUndos(const int theNbUndos);

  /**
   * This method returns the section index. It returns theIndex if it is
   * a real index and the last section's index if theIndex is equal to -1.
   */
  int getSectionIndex(const CurveCreator_Curve *theCurve,
                      const int theIndex) const;

  /**
   * Convert theIndex'th section of theCurve into AddSection command
   * and store it in theOperation. Returns true in case of success and
   * false otherwise.
   */
  bool addSectionToUndo(const CurveCreator_Curve *theCurve,
                        const int theIndex,
                        CurveCreator_Operation &theOperation) const;

  /**
   * Construct undos for SetType and SetClosed operations. Note: the
   * algorithm is optimized taking into account that there are only 2 types
   * and 2 values of isClosed flag. If the number of types is increased,
   * this algorithm should be re-implemented.
   */
  bool setTypeOrClosedToUndo(const CurveCreator_Curve *theCurve,
                             const CurveCreator_Operation::Type theType,
                             const int theIntParam1,
                             const int theIntParam2);

private:

  int                     myNbUndos;
  CurveCreator_Operation *myPUndo;
  CurveCreator_Operation *myPRedo;

};

#endif
