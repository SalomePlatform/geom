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

// File:        CurveCreator_Operation.cxx
// Author:      Sergey KHROMOV

#include "CurveCreator_Operation.hxx"
#include "CurveCreator_Curve.hxx"

#include <string>
#include <stdlib.h>

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
CurveCreator_Operation::CurveCreator_Operation()
: myType  (CurveCreator_Operation::Unknown),
  myPData (NULL)
{
}

//=======================================================================
// function: Destructor
// purpose:
//=======================================================================
CurveCreator_Operation::~CurveCreator_Operation()
{
  clear();
}

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
bool CurveCreator_Operation::init(const CurveCreator_Operation::Type theType)
{
  bool isOK = false;

  if (theType == CurveCreator_Operation::Clear ||
      theType == CurveCreator_Operation::Join) {
    clear();
    myType = theType;
    isOK   = true;
  }

  return isOK;
}

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
bool CurveCreator_Operation::init(const CurveCreator_Operation::Type theType,
                                  const int theIntParam)
{
  bool isOK = false;

  if (theType == CurveCreator_Operation::RemoveSection) {
    int *pData = (int *)allocate(sizeof(int));

    pData[0] = theIntParam;
    myType   = theType;
    isOK     = true;
  }

  return isOK;
}

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
bool CurveCreator_Operation::init(const CurveCreator_Operation::Type theType,
                                  const int theIntParam1,
                                  const int theIntParam2)
{
  bool isOK = false;

  if (theType == CurveCreator_Operation::SetType     ||
      theType == CurveCreator_Operation::SetClosed   ||
      theType == CurveCreator_Operation::MoveSection ||
      theType == CurveCreator_Operation::Join) {
    int *pData = (int *)allocate(2*sizeof(int));

    pData[0] = theIntParam1;
    pData[1] = theIntParam2;
    myType   = theType;
    isOK     = true;
  }

  return isOK;
}

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
bool CurveCreator_Operation::init(const CurveCreator_Operation::Type theType,
                                  const int theIntParam1,
                                  const int theIntParam2,
                                  const int theIntParam3)
{
  bool isOK = false;

  if (theType == CurveCreator_Operation::RemovePoints) {
    int *pData = (int *)allocate(3*sizeof(int));

    pData[0] = theIntParam1;
    pData[1] = theIntParam2;
    pData[2] = theIntParam3;
    myType   = theType;
    isOK     = true;
  }

  return isOK;
}

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
bool CurveCreator_Operation::init(const CurveCreator_Operation::Type theType,
                                  const CurveCreator::Coordinates &theCoords,
                                  const int theIntParam)
{
  bool isOK = false;

  if (theType == CurveCreator_Operation::AddPoints) {
    const int aNbCoords = theCoords.size();
    const size_t aSize =
      2*sizeof(theIntParam) + aNbCoords*sizeof(CurveCreator::TypeCoord);
    int *pIntData = (int *)allocate(aSize);

    *pIntData++ = theIntParam;
    *pIntData++ = aNbCoords;

    CurveCreator::TypeCoord *pRealData = (CurveCreator::TypeCoord *)pIntData;
    int i = 0;

    for (; i < aNbCoords; i++) {
      *pRealData++ = theCoords[i];
    }

    myType = theType;
    isOK   = true;
  }

  return isOK;
}

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
bool CurveCreator_Operation::init(const CurveCreator_Operation::Type theType,
                                  const CurveCreator::Coordinates &theCoords,
                                  const int theIntParam1,
                                  const int theIntParam2)
{
  bool isOK = false;

  if (theType == CurveCreator_Operation::AddSection   ||
      theType == CurveCreator_Operation::InsertPoints ||
      theType == CurveCreator_Operation::SetCoordinates) {
    const int aNbCoords = theCoords.size();
    const size_t aSize =
      3*sizeof(theIntParam1) + aNbCoords*sizeof(CurveCreator::TypeCoord);
    int *pIntData = (int *)allocate(aSize);

    *pIntData++ = theIntParam1;
    *pIntData++ = theIntParam2;
    *pIntData++ = aNbCoords;

    CurveCreator::TypeCoord *pRealData = (CurveCreator::TypeCoord *)pIntData;
    int i = 0;

    for (; i < aNbCoords; i++) {
      *pRealData++ = theCoords[i];
    }

    myType = theType;
    isOK   = true;
  }

  return isOK;
}

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
bool CurveCreator_Operation::init(const CurveCreator_Operation::Type theType,
                                  const std::string& theName,
                                  const CurveCreator::Coordinates &theCoords,
                                  const int theIntParam1,
                                  const int theIntParam2)
{
  bool isOK = false;
  if (theType == CurveCreator_Operation::AddSection ) {
    const int aNbCoords = theCoords.size();
    const size_t aSize =
      3*sizeof(theIntParam1) + aNbCoords*sizeof(CurveCreator::TypeCoord) + theName.length() + 1;
    int *pIntData = (int *)allocate(aSize);

    *pIntData++ = theIntParam1;
    *pIntData++ = theIntParam2;
    char* aStrPtr = (char*)pIntData;
    if( !theName.empty() ){
        strcpy( aStrPtr, theName.c_str() );
        aStrPtr += theName.length();
    }
    else{
        *aStrPtr = 0;
    }
    aStrPtr++;
    pIntData = (int*)aStrPtr;
    *pIntData++ = aNbCoords;

    CurveCreator::TypeCoord *pRealData = (CurveCreator::TypeCoord *)aStrPtr;
    int i = 0;

    for (; i < aNbCoords; i++) {
      *pRealData++ = theCoords[i];
    }

    myType = theType;
    isOK   = true;
  }

  return isOK;
}

bool CurveCreator_Operation::init(const CurveCreator_Operation::Type theType,
                                  const std::string &theName,
                                  const int theIntParam1 )
{
    if (theType == CurveCreator_Operation::RenameSection ) {
        size_t aSize = sizeof(theIntParam1) + theName.length() + 1;
        int *pIntData = (int *)allocate(aSize);
        *pIntData = theIntParam1;
        pIntData++;
        if( !theName.empty() ){
            strcpy( (char*)pIntData, theName.c_str() );
        }
        else{
            *((char*)pIntData) = 0;
        }
        myType = theType;
        return true;
    }
    return false;
}

//=======================================================================
// function: apply
// purpose:
//=======================================================================
void CurveCreator_Operation::apply(CurveCreator_Curve *theCurve)
{
  if (theCurve != NULL) {
    int *pInt = (int *)myPData;

    switch (myType) {
      case CurveCreator_Operation::AddPoints:
        {
          CurveCreator::Coordinates aCoords;

          getCoords(&pInt[1], aCoords);
          theCurve->addPoints(aCoords, pInt[0]);
        }
        break;
      case CurveCreator_Operation::RemovePoints:
        theCurve->removePoints(pInt[0], pInt[1], pInt[2]);
        break;
      case CurveCreator_Operation::InsertPoints:
        {
          CurveCreator::Coordinates aCoords;

          getCoords(&pInt[2], aCoords);
          theCurve->insertPoints(aCoords, pInt[0], pInt[1]);
        }
        break;
      case CurveCreator_Operation::SetType:
        {
          const CurveCreator::Type aType = (CurveCreator::Type) pInt[0];

          theCurve->setType(aType, pInt[1]);
        }
        break;
      case CurveCreator_Operation::Clear:
        theCurve->clear();
        break;
      case CurveCreator_Operation::SetCoordinates:
        {
          CurveCreator::Coordinates aCoords;

          getCoords(&pInt[2], aCoords);
          theCurve->setCoordinates(aCoords, pInt[0], pInt[1]);
        }
        break;
      case CurveCreator_Operation::SetClosed:
        theCurve->setClosed((pInt[0] != 0), pInt[1]);
        break;
      case CurveCreator_Operation::MoveSection:
        theCurve->moveSection(pInt[0], pInt[1]);
        break;
      case CurveCreator_Operation::Join:
        if (myPData == NULL) {
          theCurve->join();
        } else {
          theCurve->join(pInt[0], pInt[1]);
        }
        break;
      case CurveCreator_Operation::AddSection:
        {
          const CurveCreator::Type aType = (CurveCreator::Type) pInt[0];

          std::string aName = std::string((char*)&pInt[2]);

          CurveCreator::Coordinates aCoords;

          char* aPtr =  ((char*)&pInt[2]);
          aPtr += (aName.length()) + 1;
          getCoords((int*)aPtr, aCoords);
          theCurve->addSection(aName, aType, (pInt[1] != 0), aCoords);
        }
        break;
      case CurveCreator_Operation::RemoveSection:
        theCurve->removeSection(pInt[0]);
        break;
      case CurveCreator_Operation::RenameSection:
        {
            std::string aName = std::string((char*)&pInt[1]);
            theCurve->setName(aName, pInt[0]);
        }
        break;
      default:
        break;
    }
  }
}

//=======================================================================
// function: allocate
// purpose:
//=======================================================================
void *CurveCreator_Operation::allocate(const size_t theSize)
{
  if (myPData != NULL) {
    clear();
  }

  myPData = malloc(theSize);

  return myPData;
}

//=======================================================================
// function: clear
// purpose:
//=======================================================================
void CurveCreator_Operation::clear()
{
  myType = CurveCreator_Operation::Unknown;

  if (myPData != NULL) {
    free(myPData);
    myPData = NULL;
  }
}

//=======================================================================
// function: getCoords
// purpose:
//=======================================================================
void CurveCreator_Operation::getCoords
          (int *thePInt, CurveCreator::Coordinates &theCoords) const
{
  const int aNbPnts = *thePInt;
  CurveCreator::TypeCoord *pCoord = (CurveCreator::TypeCoord *)&thePInt[1];

  for (int i = 0; i < aNbPnts; i++) {
    theCoords.push_back(pCoord[i]);
  }
}
