// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
//NOTE: This is an intreface to a function for the Face creation.


#include "GEOM_Function.hxx"

#define FACE_ARG_REF1  1
#define FACE_ARG_REF2  2
#define FACE_ARG_REF3  3

#define FACE_ARG_PLANE 4
#define FACE_ARG_H     5
#define FACE_ARG_W     6

class GEOMImpl_IFace
{
 public:

  GEOMImpl_IFace(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetRef1(Handle(GEOM_Function) theRefPoint1) { _func->SetReference(FACE_ARG_REF1, theRefPoint1); }
  Handle(GEOM_Function) GetRef1() { return _func->GetReference(FACE_ARG_REF1); }

  void SetRef2(Handle(GEOM_Function) theRefPoint2) { _func->SetReference(FACE_ARG_REF2, theRefPoint2); }
  Handle(GEOM_Function) GetRef2() { return _func->GetReference(FACE_ARG_REF2); }

  void SetRef3(Handle(GEOM_Function) theRefVec) { _func->SetReference(FACE_ARG_REF3, theRefVec); }
  Handle(GEOM_Function) GetRef3() { return _func->GetReference(FACE_ARG_REF3); }

  void SetH(double theH) { _func->SetReal(FACE_ARG_H, theH); }
  void SetW(double theW) { _func->SetReal(FACE_ARG_W, theW); }

  double GetH() { return _func->GetReal(FACE_ARG_H); }
  double GetW() { return _func->GetReal(FACE_ARG_W); }

 private:

  Handle(GEOM_Function) _func;
};
