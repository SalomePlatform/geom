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

// File:        CurveCreator_Operation.hxx
// Author:      Sergey KHROMOV

#ifndef _CurveCreator_Operation_HeaderFile
#define _CurveCreator_Operation_HeaderFile

#include "CurveCreator.hxx"
#include "CurveCreator_ICurve.hxx"
#include "CurveCreator_PosPoint.hxx"

#include <string>
#include <vector>

class CurveCreator_Curve;


/**
 * This is the support class that describes a modification operation that
 * can be applied to CurveCreator_Curve.
 */
class CurveCreator_Operation
{

public:

  /**
   * This is a type of CurveCreator_Curve modification operation.
   */
  enum Type
  {
    Unknown = 0,    //!< Unknown method.
    AddPoints,      //!< Method CurveCreator_Curve::addPoints
    RemovePoints,   //!< Method CurveCreator_Curve::removePoints
    InsertPoints,   //!< Method CurveCreator_Curve::insertPoints
    SetType,        //!< Method CurveCreator_Curve::setType
    Clear,          //!< Method CurveCreator_Curve::clear
    SetCoordinates, //!< Method CurveCreator_Curve::setCoordinates
    SetClosed,      //!< Method CurveCreator_Curve::setClosed
    MoveSection,    //!< Method CurveCreator_Curve::moveSection
    Join,           //!< Method CurveCreator_Curve::join
    AddSection,     //!< Method CurveCreator_Curve::addSection
    RemoveSection,   //!< Method CurveCreator_Curve::removeSection
    RenameSection   //!< Method CurveCreator_Curve::renameSection
  };

  /**
   * Empty constructor.
   */
  CurveCreator_Operation();

  /**
   * Destructor.
   */
  ~CurveCreator_Operation();

  /**
   * This method initializes the object with an operation without parameters.
   * It is applicable to the following operations:
   * <UL>
   *   <LI>Clear</LI>
   * </UL>
   * @return true in case of success; false otherwise.
   */
  bool init(const Type theType);

  /**
   * This method initializes the object with an operation with one integer
   * parameter. It is applicable to the following operations:
   * <UL>
   *   <LI>RemoveSection</LI>
   * </UL>
   * @return true in case of success; false otherwise.
   */
  bool init(const Type theType, const int theIntParam);

  /**
   * This method initializes the object with an operation with two integer
   * parameters. It is applicable to the following operations:
   * <UL>
   *   <LI>SetType</LI>
   *   <LI>SetClosed</LI>
   *   <LI>MoveSection</LI>
   *   <LI>Join (with 2 int arguments)</LI>
   * </UL>
   * @return true in case of success; false otherwise.
   */
  bool init(const Type theType, const int theIntParam1,
            const int theIntParam2);

  /**
   * This method initializes the object with an operation with two integer
   * parameters. It is applicable to the following operations:
   * <UL>
   *   <LI>Join (with a list of int arguments)</LI>
   * </UL>
   * @return true in case of success; false otherwise.
   */
  bool init(const Type theType, const std::list<int> theParamList);

  /**
   * This method initializes the object with an operation with 
   * list of pairs of integer parameters. 
   * It is applicable to the following operations:
   * <UL>
   *   <LI>RemovePoints</LI>
   * </UL>
   * @return true in case of success; false otherwise.
   */
  bool init(const Type theType, 
    const CurveCreator_ICurve::SectionToPointList &theParamList1);

  /**
   * This method initializes the object with an operation with one
   * CurveCreator::Coordinates parameter and one integer parameter.
   * It is applicable to the following operations:
   * <UL>
   *   <LI>AddPoints</LI>
   * </UL>
   * @return true in case of success; false otherwise.
   */
  bool init(const Type theType, const CurveCreator::Coordinates &theCoords,
            const int theIntParam);

  /**
   * This method initializes the object with an operation with
   * list of pairs of integer parameters and CurveCreator::Coordinates parameters.
   * It is applicable to the following operations:
   * <UL>
   *   <LI>InsertPoints</LI>
   * </UL>
   * @return true in case of success; false otherwise.
   */
  bool init(const Type theType, 
            const CurveCreator_ICurve::SectionToPointCoordsList &theParamList1);

  /**
   * This method initializes the object with an operation with one
   * string, one CurveCreator::Coordinates parameter and two integer parameters.
   * It is applicable to the following operations:
   * <UL>
   *   <LI>AddSection</LI>
   *   <LI>InsertPoints</LI>
   *   <LI>SetCoordinates</LI>
   * </UL>
   * @return true in case of success; false otherwise.
   */
  bool init(const CurveCreator_Operation::Type theType,
            const std::string& theName,
            const CurveCreator::Coordinates &theCoords,
            const int theIntParam1,
            const int theIntParam2);


  /**
   * This method initializes the object with an operation with one
   * string parameter and one integer.
   * It is applicable to the following operations:
   * <UL>
   *   <LI>RenameSection</LI>
   * </UL>
   * @return true in case of success; false otherwise.
   */
  bool init(const CurveCreator_Operation::Type theType,
                                    const std::string &theName,
                                    const int theIntParam1 );

  /**
   * This method applies the current operation to theCurve.
   */
  void apply(CurveCreator_Curve *theCurve);

private:

  /**
   * This method allocates required memory for the operation data.
   * Returns myPData for convenience purpose.
   */
  void *allocate(const size_t theSize);

  /**
   * This method clears initialized data pointers.
   */
  void clear();

  /**
   * This method returns the coordinates read from thePInt.
   */
  void getCoords(int *thePInt, CurveCreator::Coordinates &theCoords) const;

private:

  Type  myType;
  void *myPData;

};

#endif
