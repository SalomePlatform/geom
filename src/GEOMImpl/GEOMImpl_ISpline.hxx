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

//  NOTE: This is an interface to a function for the Spline creation.

#include <GEOM_Function.hxx>

#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_HArray1OfReal.hxx>

class GEOMImpl_ISpline
{
 public:

  enum {
    ARG_POINTS  = 1,
    ARG_CLOSED  = 2,
    ARG_REORDER = 3,
    CONSTRUCTOR = 4,
    ARG_ARRAY   = 5,
    ARG_VEC_1   = 6,
    ARG_VEC_2   = 7
  };

  GEOMImpl_ISpline(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  // Set

  void SetPoints (const Handle(TColStd_HSequenceOfTransient)& thePoints)
  { _func->SetReferenceList(ARG_POINTS, thePoints); }

  void SetIsClosed (bool theIsClosed) { _func->SetInteger(ARG_CLOSED, (int)theIsClosed); }
  void SetDoReordering (bool theDoReordering) { _func->SetInteger(ARG_REORDER, (int)theDoReordering); }

  void SetConstructorType (int theConstructor) { _func->SetInteger(CONSTRUCTOR, theConstructor); }

  void SetCoordinates (const Handle(TColStd_HArray1OfReal)& theValue)
  { _func->SetRealArray(ARG_ARRAY, theValue); }

  void SetFirstVector (Handle(GEOM_Function) theVec) { _func->SetReference(ARG_VEC_1, theVec); }
  void SetLastVector  (Handle(GEOM_Function) theVec) { _func->SetReference(ARG_VEC_2, theVec); }

  // Get

  Handle(TColStd_HSequenceOfTransient) GetPoints() { return _func->GetReferenceList(ARG_POINTS); }

  bool GetIsClosed() { return (bool)_func->GetInteger(ARG_CLOSED); }
  bool GetDoReordering() { return (bool)_func->GetInteger(ARG_REORDER); }

  int GetConstructorType() { return _func->GetInteger(CONSTRUCTOR); }

  Handle(TColStd_HArray1OfReal) GetCoordinates() { return _func->GetRealArray(ARG_ARRAY); }

  Handle(GEOM_Function) GetFirstVector () { return _func->GetReference(ARG_VEC_1); }
  Handle(GEOM_Function) GetLastVector  () { return _func->GetReference(ARG_VEC_2); }

  /* Old implementation (Salome 6.6.0 and earlier)
#define SPL_ARG_LENG 1
#define SPL_ARG_CLOS 2
#define SPL_ARG_REOR 3
#define SPL_ARG_LAST 2

#define SPL_CONSTRUCTOR 4
#define SPL_ARG_ARRAY 5

  void SetLength(int theLen) { _func->SetInteger(SPL_ARG_LENG, theLen); }
  void SetIsClosed(bool theIsClosed) { _func->SetInteger(SPL_ARG_CLOS, (int)theIsClosed); }
  void SetDoReordering(bool theDoReordering) { _func->SetInteger(SPL_ARG_REOR, (int)theDoReordering); }
  void SetConstructorType(int theConstructor) {_func->SetInteger(SPL_CONSTRUCTOR,theConstructor); }
  void SetPoint(int theId, Handle(GEOM_Function) theP) { _func->SetReference(SPL_ARG_LAST + theId, theP); }
  void SetCoordinates(const Handle(TColStd_HArray1OfReal)& theValue)
              { _func->SetRealArray(SPL_ARG_ARRAY, theValue); }

  int GetLength() { return _func->GetInteger(SPL_ARG_LENG); }
  bool GetIsClosed() { return (bool)_func->GetInteger(SPL_ARG_CLOS); }
  bool GetDoReordering() { return (bool)_func->GetInteger(SPL_ARG_REOR); }
  int GetConstructorType() { return _func->GetInteger(SPL_CONSTRUCTOR); }
  Handle(GEOM_Function) GetPoint(int theId) { return _func->GetReference(SPL_ARG_LAST + theId); }
  Handle(TColStd_HArray1OfReal) GetCoordinates() { return _func->GetRealArray(SPL_ARG_ARRAY); }
  */

 private:

  Handle(GEOM_Function) _func;
};
