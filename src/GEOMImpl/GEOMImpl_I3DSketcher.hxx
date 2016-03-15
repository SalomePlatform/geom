// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//NOTE: This is an interface to a function for the Sketcher creation.

#include "GEOM_Function.hxx"

#include <TColStd_HArray1OfReal.hxx>

class GEOMImpl_I3DSketcher
{
public:

  GEOMImpl_I3DSketcher (Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetCoordinates (const Handle(TColStd_HArray1OfReal)& theValue)
  { _func->SetRealArray(SKETCH_ARG_COORDS, theValue); }

  Handle(TColStd_HArray1OfReal) GetCoordinates() { return _func->GetRealArray(SKETCH_ARG_COORDS); }

  void SetCommand (const TCollection_AsciiString& theCommand)
  { _func->SetString(SKETCH_ARG_COMMAND, theCommand); }

  TCollection_AsciiString GetCommand() { return _func->GetString(SKETCH_ARG_COMMAND); }

private:

  enum {
    SKETCH_ARG_COORDS = 1,
    SKETCH_ARG_COMMAND = 2
  };

  Handle(GEOM_Function) _func;
};
