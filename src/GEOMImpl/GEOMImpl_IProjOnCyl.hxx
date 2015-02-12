// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

//NOTE: This is an interface to a function for the Projection on cylinder creation.

#include "GEOM_Function.hxx"

#define PROJECTION_ARG_SHAPE            1
#define PROJECTION_ARG_RADIUS           2
#define PROJECTION_ARG_START_ANGLE      3
#define PROJECTION_ARG_ANGLE_LENGTH     4

class GEOMImpl_IProjOnCyl
{

public:

  GEOMImpl_IProjOnCyl(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape       (const Handle(GEOM_Function) &theShape)
          { _func->SetReference(PROJECTION_ARG_SHAPE,   theShape); }
  void SetRadius      (const Standard_Real          theRadius)
          { _func->SetReal(PROJECTION_ARG_RADIUS,       theRadius); }
  void SetStartAngle  (const Standard_Real          theStartAngle)
          { _func->SetReal(PROJECTION_ARG_START_ANGLE,  theStartAngle); }
  void SetAngleLength (const Standard_Real          theAngleLength)
          { _func->SetReal(PROJECTION_ARG_ANGLE_LENGTH, theAngleLength); }

  Handle(GEOM_Function) GetShape()
          { return _func->GetReference(PROJECTION_ARG_SHAPE); }
  Standard_Real         GetRadius()
          { return _func->GetReal(PROJECTION_ARG_RADIUS ); }
  Standard_Real         GetStartAngle()
          { return _func->GetReal(PROJECTION_ARG_START_ANGLE ); }
  Standard_Real         GetAngleLength()
          { return _func->GetReal(PROJECTION_ARG_ANGLE_LENGTH ); }

 private:

  Handle(GEOM_Function) _func;

};
