// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  File   : GEOMImpl_PolylineDumper.cxx
//  Author : Sergey KHROMOV
//  Module : GEOM


#include "GEOMImpl_PolylineDumper.hxx"
#include "GEOMImpl_ICurvesOperations.hxx"

#include <GEOM_PythonDump.hxx>


//=======================================================================
// function : Constructor
// purpose  : 
//=======================================================================
GEOMImpl_PolylineDumper::GEOMImpl_PolylineDumper
            (const std::list <std::list <double> >         &theCoords,
             const Handle(TColStd_HArray1OfExtendedString) &theNames,
             const Handle(TColStd_HArray1OfByte)           &theTypes,
             const Handle(TColStd_HArray1OfByte)           &theCloseds,
             const Handle(TColStd_HArray1OfReal)           &thePlnCoords)
             : myCoords    (theCoords),
               myNames     (theNames),
               myTypes     (theTypes),
               myCloseds   (theCloseds),
               myPlnCoords (thePlnCoords),
               myIsDone    (Standard_False)
{
  init();
}


//=======================================================================
// function : Constructor
// purpose  : 
//=======================================================================
GEOMImpl_PolylineDumper::GEOMImpl_PolylineDumper
            (const std::list <std::list <double> >         &theCoords,
             const Handle(TColStd_HArray1OfExtendedString) &theNames,
             const Handle(TColStd_HArray1OfByte)           &theTypes,
             const Handle(TColStd_HArray1OfByte)           &theCloseds,
             const Handle(GEOM_Object)                     &theWorkingPlane)
             : myCoords       (theCoords),
               myNames        (theNames),
               myTypes        (theTypes),
               myCloseds      (theCloseds),
               myWorkingPlane (theWorkingPlane),
               myIsDone       (Standard_False)
{
  init();
}

//=======================================================================
// function : Dump
// purpose  : 
//=======================================================================
Standard_Boolean GEOMImpl_PolylineDumper::Dump
        (const Handle(GEOM_Object) &theObject)
{
  if (theObject.IsNull()) {
    return Standard_False;
  }

  if (myIsDone) {
    Handle(GEOM_Function) aFunction = theObject->GetLastFunction();
    GEOM::TPythonDump aPD(aFunction);

    aPD << myDescr;
    aPD << theObject << " = pl.result(";

    if (myWorkingPlane.IsNull()) {
      // Add coodinates of working plane.
      Standard_Integer i;

      aPD << "[";
      for (i = 0; i < 9; ++i) {
        aPD << myPlnCoords->Value(myPlnCoords->Lower() + i);

        if (i < 8) {
          aPD << ", ";
        }
      }
      aPD << "]";
    } else {
      aPD << myWorkingPlane;
    }

    aPD << ")";
  }

  return myIsDone;
}

//=======================================================================
// function : init
// purpose  : 
//=======================================================================
void GEOMImpl_PolylineDumper::init()
{
  // Check input parameters.
  if (myCoords.empty() || myNames.IsNull() ||
      myTypes.IsNull() || myCloseds.IsNull()) {
    // One or more input parameters are null or empty()
    return;
  }

  const Standard_Integer aNbSec = myCoords.size();

  if (aNbSec != myNames->Length() || aNbSec != myTypes->Length() ||
      aNbSec != myCloseds->Length()) {
    // Inconsistent data.
    return;
  }

  // Check the reference plane
  if (myPlnCoords.IsNull()) {
    if (myWorkingPlane.IsNull()) {
      // Null working plane
      return;
    }
  } else {
    if (myWorkingPlane.IsNull() == Standard_False) {
      // Ambiguous working plane
      return;
    }

    if (myPlnCoords->Length() != 9) {
      // Invalid number of plane coordinates.
      return;
    }
  }

  const char *aSeparator = "\n\t";
  Standard_Integer i;
  std::list <std::list <double> >::const_iterator anIt = myCoords.begin();

  myDescr += "pl = geompy.Polyline2D()";

  // Add sections.
  for (i = 0; i < aNbSec && anIt != myCoords.end(); ++i, ++anIt) {
    myDescr += aSeparator;
    myDescr += "pl.addSection(";
    // Add name
    myDescr += "\"";
    myDescr += myNames->Value(myNames->Lower() + i) + "\", ";
    // Add type
    const Standard_Integer aType = myTypes->Value(myTypes->Lower() + i);

    switch (aType) {
      case GEOMImpl_ICurvesOperations::Polyline:
        myDescr += "GEOM.Polyline, ";
        break;
      case GEOMImpl_ICurvesOperations::Interpolation:
        myDescr += "GEOM.Interpolation, ";
        break;
      default:
        myDescr.Clear();
        return;
        break; // NEVERREACHED
    }

    // Add Closed flag.
    if (myCloseds->Value(myCloseds->Lower() + i)) {
      myDescr += "True";
    } else {
      myDescr += "False";
    }

    // Add points.
    const Standard_Integer aNbCoords = anIt->size();

    if (aNbCoords > 0) {
      if (aNbCoords % 2) {
        // Odd number of coordinates.
        myDescr.Clear();
        return;
      }

      if (aNbCoords <= 4) {
        // Add 2 points to the same command addSection.
        myDescr += ", [";

        std::list <double>::const_iterator aCIt = anIt->begin();

        while (aCIt != anIt->end()) {
          myDescr += *aCIt;

          if (++aCIt != anIt->end()) {
            myDescr += ", ";
          }
        }
      } else {
        // Add points to a separate command addPoints.
        // Add maximum 4 points in a command.
        std::list <double>::const_iterator aCIt        = anIt->begin();
        Standard_Integer                   aMaxNbCoord = 8;
        Standard_Integer                   j           = 1;

        myDescr += ")";
        myDescr += aSeparator;
        myDescr += "pl.addPoints([";

        while (aCIt != anIt->end()) {
          myDescr += *aCIt;

          if (++aCIt != anIt->end()) {
            if (j == aMaxNbCoord) {
              // 4 points are added. Add a new command.
              myDescr += "])";
              myDescr += aSeparator;
              myDescr += "pl.addPoints([";
              j        = 1;
            } else {
              myDescr += ", ";
              j++;
            }
          }
        }
      }

      myDescr += "]";
    }

    myDescr += ")";
  }

  myDescr  += aSeparator;
  myIsDone  = Standard_True;
}
