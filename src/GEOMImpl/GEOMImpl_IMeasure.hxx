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

// NOTE: This is an interface to a functions for the Measurement
//
#include "GEOM_Function.hxx"

class GEOMImpl_IMeasure
{
  enum {
    MEASURE_ARG_BASE  = 1,
    MEASURE_ARG_POINT = 2,
    MEASURE_INDEX = 3,
    MEASURE_USE_ORI = 4,
    MEASURE_ARG_DIR = 5
  };
 public:

  GEOMImpl_IMeasure(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetBase(Handle(GEOM_Function) theRefBase)
  { _func->SetReference(MEASURE_ARG_BASE, theRefBase); }

  Handle(GEOM_Function) GetBase() { return _func->GetReference(MEASURE_ARG_BASE); }

  void SetPoint(Handle(GEOM_Function) thePnt)
  { _func->SetReference(MEASURE_ARG_POINT, thePnt); }

  Handle(GEOM_Function) GetPoint() { return _func->GetReference(MEASURE_ARG_POINT); }
  
  void SetIndex(int theIndex) { _func->SetInteger(MEASURE_INDEX, theIndex); }
    
  int GetIndex() { return _func->GetInteger(MEASURE_INDEX); }

  void SetUseOri(int theIndex) { _func->SetInteger(MEASURE_USE_ORI, theIndex); }
    
  bool GetUseOri() { return ( _func->GetInteger(MEASURE_USE_ORI) ||
                              !_func->IsDone() ); // old behavior was to useOri
  }

  void SetDirection(Handle(GEOM_Function) theDir)
  { _func->SetReference(MEASURE_ARG_DIR, theDir); }

  Handle(GEOM_Function) GetDirection() { return _func->GetReference(MEASURE_ARG_DIR); }

 private:

  Handle(GEOM_Function) _func;
};
