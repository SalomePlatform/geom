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

//NOTE: This is an interface to a function for the Polyline2D creation.


#ifndef _GEOMImpl_IPolyline2D_HXX_
#define _GEOMImpl_IPolyline2D_HXX_


#include <GEOM_GEOMImpl.hxx>
#include <GEOM_Function.hxx>

#include <list>

#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TColStd_HArray1OfReal.hxx>


#define POLY_ARG_NAMES         1
#define POLY_ARG_TYPES         2
#define POLY_ARG_CLOSEDS       3
#define POLY_ARG_COORDS        4
#define POLY_ARG_START_INDICES 5
#define POLY_ARG_WPLANE_DBLS   6
#define POLY_ARG_WPLANE_OBJ    7


class GEOMIMPL_EXPORT GEOMImpl_IPolyline2D
{
 public:

  GEOMImpl_IPolyline2D(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetNames(const Handle(TColStd_HArray1OfExtendedString) &theValue)
              { _func->SetStringArray(POLY_ARG_NAMES, theValue); }

  Handle(TColStd_HArray1OfExtendedString) GetNames()
              { return _func->GetStringArray(POLY_ARG_NAMES); }

  void SetTypes(const Handle(TColStd_HArray1OfByte) &theValue)
              { _func->SetByteArray(POLY_ARG_TYPES, theValue); }

  Handle(TColStd_HArray1OfByte) GetTypes()
              { return _func->GetByteArray(POLY_ARG_TYPES); }

  void SetClosedFlags(const Handle(TColStd_HArray1OfByte) &theValue)
              { _func->SetBooleanArray(POLY_ARG_CLOSEDS, theValue); }

  Handle(TColStd_HArray1OfByte) GetClosedFlags()
              { return _func->GetBooleanArray(POLY_ARG_CLOSEDS); }

  void SetWorkingPlaneDbls(const Handle(TColStd_HArray1OfReal) &thePlane)
              { _func->SetRealArray(POLY_ARG_WPLANE_DBLS, thePlane); }

  Handle(TColStd_HArray1OfReal) GetWorkingPlaneDbls()
              { return _func->GetRealArray(POLY_ARG_WPLANE_DBLS); }

  void SetWorkingPlane(const Handle(GEOM_Function) &thePlane)
              { _func->SetReference(POLY_ARG_WPLANE_OBJ, thePlane); }

  Handle(GEOM_Function) GetWorkingPlane()
              { return _func->GetReference(POLY_ARG_WPLANE_OBJ); }

  void SetCoords(const std::list <std::list <double> > &theValue);

  void GetCoords(std::list <std::list <double> > &theValue);

 private:

  Handle(GEOM_Function) _func;
};

#endif
