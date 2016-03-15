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

// File:        CurveCreator_Operation.cxx
// Author:      Sergey KHROMOV

#include "CurveCreator_Operation.hxx"
#include "CurveCreator_Curve.hxx"
#include "CurveCreator.hxx"

#include <string>
#include <stdlib.h>
#include <string.h>

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

bool compId(CurveCreator_PosPoint* p1, CurveCreator_PosPoint* p2)
{
  return p1->myID < p2->myID;
}

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
bool CurveCreator_Operation::init(const CurveCreator_Operation::Type theType)
{
  bool isOK = false;

  if (theType == CurveCreator_Operation::Clear) {
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
bool CurveCreator_Operation::init(const Type theType, const std::list<int> theParamList)
{
  bool isOK = false;

  if (theType == CurveCreator_Operation::Join)
  {
    const int aNbPoints = theParamList.size();

    const size_t aSize =
      sizeof(aNbPoints) +
      aNbPoints * (sizeof(int));

    int *pIntData = (int *)allocate(aSize);

    *pIntData++ = aNbPoints;
    std::list<int>::const_iterator anIt = theParamList.begin(), aLast = theParamList.end();
    for ( ; anIt != aLast; anIt++ )
      *pIntData++ = *anIt;

    myType   = theType;
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

bool CurveCreator_Operation::init(const CurveCreator_Operation::Type theType,
                                  const CurveCreator_ICurve::SectionToPointCoordsList &theParamList1)
{
  bool isOK = false;

  if (theType == CurveCreator_Operation::InsertPoints ||
      theType == CurveCreator_Operation::SetCoordinates ) {

    const int aNbPoints = theParamList1.size();

    CurveCreator_ICurve::SectionToPointCoordsList::const_iterator anIt = 
      theParamList1.begin();
    const int aNbCoords = anIt->second.size();

    const size_t aSize =
      sizeof(aNbPoints) + sizeof(aNbCoords) + 
      aNbPoints * (3*sizeof(int) + aNbCoords*sizeof(CurveCreator::TypeCoord));
    int *pIntData = (int *)allocate(aSize);

    *pIntData++ = aNbPoints;
    *pIntData++ = aNbCoords;
    int aSectionId, aPointId;
    for ( ; anIt != theParamList1.end(); anIt++ ) {
      aSectionId = anIt->first.first;
      aPointId = anIt->first.second;

      *pIntData++ = aSectionId;
      *pIntData++ = aPointId;
      *pIntData++ = aNbCoords;

      const CurveCreator::Coordinates &aCoords = anIt->second;
      CurveCreator::TypeCoord *pRealData = (CurveCreator::TypeCoord *)pIntData;
      for (int i = 0; i < aNbCoords; i++) {
        *pRealData++ = aCoords[i];
      }
      pIntData = (int *)pRealData;
    }

    myType = theType;
    isOK   = true;
  }

  return isOK;
}

bool CurveCreator_Operation::init(const CurveCreator_Operation::Type theType,
                                  const CurveCreator_ICurve::SectionToPointList &theParamList1)
{
  bool isOK = false;

  if (theType == CurveCreator_Operation::RemovePoints) {
    const int aNbPoints = theParamList1.size();

    CurveCreator_ICurve::SectionToPointList::const_iterator anIt = 
      theParamList1.begin();

    const size_t aSize =
      sizeof(aNbPoints) +
      aNbPoints * (2*sizeof(int));
    int *pIntData = (int *)allocate(aSize);

    *pIntData++ = aNbPoints;
    int aSectionId, aPointId;
    for ( ; anIt != theParamList1.end(); anIt++ ) {
      aSectionId = anIt->first;
      aPointId = anIt->second;

      *pIntData++ = aSectionId;
      *pIntData++ = aPointId;
    }

    myType = theType;
    isOK   = true;
  }

  return isOK;
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
      case CurveCreator_Operation::InsertPoints:
      case CurveCreator_Operation::SetCoordinates:
        {
          int aSectionId, aPointId;
          CurveCreator::SectionsMap aSectionsMap;
          CurveCreator::PosPointsList aPoints;
          CurveCreator::Coordinates aCoords;

          int nbPoints = pInt[0];
          int nbCoords = pInt[1];
          int nbParams = 3+nbCoords*sizeof(double)/sizeof(int);
          for (int i = 0; i < nbPoints*nbParams; i=i+nbParams) {
            aCoords.clear();
            aPoints.clear();
            getCoords(&pInt[4+i], aCoords);
            aSectionId = pInt[2+i];
            aPointId = pInt[3+i];
            if ( aSectionsMap.find( aSectionId ) != aSectionsMap.end() )
              aPoints = aSectionsMap[aSectionId];
            CurveCreator_PosPoint* aPosPoint = new CurveCreator_PosPoint( aPointId, aCoords );
            aPoints.push_back( aPosPoint );
            aPoints.sort(compId);
            aSectionsMap[aSectionId] = aPoints;
          }
          switch (myType) {
            case CurveCreator_Operation::AddPoints:
            case CurveCreator_Operation::InsertPoints:
              theCurve->addPointsInternal( aSectionsMap );
              break;
            case CurveCreator_Operation::SetCoordinates:
              theCurve->setPointInternal( aSectionsMap );
              break;
          }
        }
        break;
      case CurveCreator_Operation::RemovePoints:
        {
          CurveCreator_ICurve::SectionToPointList aListOfSectionsToPoints;
          int nbPoints = pInt[0];
          for (int i = 1; i < nbPoints*2; i=i+2) {
            aListOfSectionsToPoints.push_back(std::make_pair(pInt[i], pInt[i+1]));
          }
          theCurve->removePointsInternal(aListOfSectionsToPoints);
        }
        break;
      case CurveCreator_Operation::SetType:
        {
          const CurveCreator::SectionType aType = (CurveCreator::SectionType) pInt[0];

          theCurve->setSectionTypeInternal( pInt[1], aType );
        }
        break;
      case CurveCreator_Operation::Clear:
        theCurve->clearInternal();
        break;
      case CurveCreator_Operation::SetClosed:
        theCurve->setClosedInternal(pInt[1], (pInt[0] != 0));
        break;
      case CurveCreator_Operation::MoveSection:
        theCurve->moveSectionInternal(pInt[0], pInt[1]);
        break;
      case CurveCreator_Operation::Join:
        if (myPData != NULL)
        {
          std::list<int> aListOfSections;
          int nbSections = pInt[0];
          for (int i = 1; i < nbSections+1; i++) {
            aListOfSections.push_back(pInt[i]);
          }
          theCurve->joinInternal(aListOfSections);
        }
        break;
      case CurveCreator_Operation::AddSection:
        {
          const CurveCreator::SectionType aType = (CurveCreator::SectionType) pInt[0];

          std::string aName = std::string((char*)&pInt[2]);

          CurveCreator::Coordinates aCoords;

          char* aPtr =  ((char*)&pInt[2]);
          aPtr += (aName.length()) + 1;
          getCoords((int*)aPtr, aCoords);
          theCurve->addSectionInternal(aName, aType, (pInt[1] != 0), aCoords);
        }
        break;
      case CurveCreator_Operation::RemoveSection:
        theCurve->removeSectionInternal(pInt[0]);
        break;
      case CurveCreator_Operation::RenameSection:
        {
            std::string aName = std::string((char*)&pInt[1]);
            theCurve->setSectionNameInternal(pInt[0], aName);
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
