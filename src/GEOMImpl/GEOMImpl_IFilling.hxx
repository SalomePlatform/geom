// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

//NOTE: This is an interface to a function for the Filling operation.
//
#include "GEOM_Function.hxx"
#include <TColStd_HSequenceOfTransient.hxx>

enum GEOMImpl_IFilling_Arg {
  FILL_ARG_MINDEG = 1,
  FILL_ARG_MAXDEG = 2,
  FILL_ARG_TOL2D  = 3,
  FILL_ARG_TOL3D  = 4,
  FILL_ARG_SHAPE  = 5,
  FILL_ARG_NBITER = 6,
  FILL_ARG_APPROX = 7,
  FILL_ARG_METHOD = 8
};

class GEOMImpl_IFilling
{
public:

  GEOMImpl_IFilling(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetTol2D(double theTol2D) { _func->SetReal(FILL_ARG_TOL2D, theTol2D); }
  void SetTol3D(double theTol3D) { _func->SetReal(FILL_ARG_TOL3D, theTol3D); }
  double GetTol2D() { return _func->GetReal(FILL_ARG_TOL2D); }
  double GetTol3D() { return _func->GetReal(FILL_ARG_TOL3D); }

  void SetMinDeg(int theMinDeg) { _func->SetInteger(FILL_ARG_MINDEG, theMinDeg); }
  void SetMaxDeg(int theMaxDeg) { _func->SetInteger(FILL_ARG_MAXDEG, theMaxDeg); }
  int GetMinDeg() { return _func->GetInteger(FILL_ARG_MINDEG); }
  int GetMaxDeg() { return _func->GetInteger(FILL_ARG_MAXDEG); }
  void SetNbIter(int theNbIter) { _func->SetInteger(FILL_ARG_NBITER, theNbIter); }
  int GetNbIter() { return _func->GetInteger(FILL_ARG_NBITER); }

  void SetApprox(bool theApprox) { _func->SetInteger(FILL_ARG_APPROX, theApprox); }
  bool GetApprox() { return _func->GetInteger(FILL_ARG_APPROX); }

  void SetMethod(int theMethod) { _func->SetInteger(FILL_ARG_METHOD, theMethod); }
  int GetMethod() { return _func->GetInteger(FILL_ARG_METHOD); }

  void SetShapes(const Handle(TColStd_HSequenceOfTransient)& theShapes)
  { _func->SetReferenceList(FILL_ARG_SHAPE, theShapes); }
  Handle(TColStd_HSequenceOfTransient) GetShapes()
  {
    Handle(TColStd_HSequenceOfTransient) aSeq = _func->GetReferenceList(FILL_ARG_SHAPE);
    if ( aSeq.IsNull() ) {
      Handle(GEOM_Function) fun = GetShape();
      if ( !fun.IsNull() ) {
        aSeq = new TColStd_HSequenceOfTransient;
        aSeq->Append( fun );
      }
    }
    return aSeq;
  }
  void SetShape(Handle(GEOM_Function) theShape) { _func->SetReference(FILL_ARG_SHAPE, theShape); }
  Handle(GEOM_Function) GetShape() { return _func->GetReference(FILL_ARG_SHAPE); }

private:

  Handle(GEOM_Function) _func;
};
