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

//NOTE: This is an intreface to a function for the Transfer Data functionality.
//


#ifndef _GEOMImpl_ITransferData_HXX_
#define _GEOMImpl_ITransferData_HXX_


#include "GEOM_GEOMImpl.hxx"
#include "GEOM_Function.hxx"

#include <TColStd_HArray1OfInteger.hxx>


#define TD_ARG_REF1            1
#define TD_ARG_REF2            2
#define TD_ARG_METHOD          3
#define TD_ARG_DATUM_NAME      4
#define TD_ARG_DATUM_MAX_VALUE 5
#define TD_ARG_DATUM_VALUE     6

class GEOMImpl_ITransferData
{
 public:

  GEOMImpl_ITransferData(const Handle(GEOM_Function) &theFunction)
    : _func(theFunction) {}

  void SetRef1(const Handle(GEOM_Function) &theRefPoint1)
  { _func->SetReference(TD_ARG_REF1, theRefPoint1); }

  Handle(GEOM_Function) GetRef1() { return _func->GetReference(TD_ARG_REF1); }

  void SetRef2(const Handle(GEOM_Function) &theRefPoint2)
  { _func->SetReference(TD_ARG_REF2, theRefPoint2); }

  Handle(GEOM_Function) GetRef2() { return _func->GetReference(TD_ARG_REF2); }

  void SetFindMethod(const int theFindMethod)
  { _func->SetInteger(TD_ARG_METHOD, theFindMethod); }

  int GetFindMethod() { return _func->GetInteger(TD_ARG_METHOD); }

  void SetDatumName(const Handle(TColStd_HArray1OfExtendedString) &theDatumName)
  { _func->SetStringArray(TD_ARG_DATUM_NAME, theDatumName); }

  Handle(TColStd_HArray1OfExtendedString) GetDatumName()
  { return _func->GetStringArray(TD_ARG_DATUM_NAME); }

  void SetDatumMaxVal(const Handle(TColStd_HArray1OfInteger) &theDatumMaxVal)
  { _func->SetIntegerArray(TD_ARG_DATUM_MAX_VALUE, theDatumMaxVal); }

  Handle(TColStd_HArray1OfInteger) GetDatumMaxVal()
  { return _func->GetIntegerArray(TD_ARG_DATUM_MAX_VALUE); }

  void SetDatumVal(const Handle(TColStd_HArray1OfInteger) &theDatumVal)
  { _func->SetIntegerArray(TD_ARG_DATUM_VALUE, theDatumVal); }

  Handle(TColStd_HArray1OfInteger) GetDatumVal()
  { return _func->GetIntegerArray(TD_ARG_DATUM_VALUE); }

  GEOMIMPL_EXPORT void SetName(const TopoDS_Shape            &theSubShape,
                                const TCollection_AsciiString &theName)
  { SetStringData(theSubShape, theName, Standard_True); }

  GEOMIMPL_EXPORT TCollection_AsciiString GetName
                               (const TopoDS_Shape &theSubShape)
  { return GetStringData(theSubShape, Standard_True); }

  GEOMIMPL_EXPORT void SetMaterial(const TopoDS_Shape         &theSubShape,
                                   const TCollection_AsciiString &theName)
  { SetStringData(theSubShape, theName, Standard_False); }

  GEOMIMPL_EXPORT TCollection_AsciiString GetMaterial
                               (const TopoDS_Shape &theSubShape)
  { return GetStringData(theSubShape, Standard_False); }

 private:

  TCollection_AsciiString GetStringData(const TopoDS_Shape     &theSubShape,
                                        const Standard_Boolean  IsName);

  void SetStringData(const TopoDS_Shape            &theSubShape,
                     const TCollection_AsciiString &theData,
                     const Standard_Boolean         IsName);

 private:

  Handle(GEOM_Function) _func;
};

#endif
