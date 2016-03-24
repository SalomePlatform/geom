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

#ifndef _AdvancedEngine_IDividedDisk_HXX_
#define _AdvancedEngine_IDividedDisk_HXX_

#include "GEOM_Function.hxx"

#define DIVIDEDDISK_ARG_R      1
#define DIVIDEDDISK_ARG_RATIO  2
#define DIVIDEDDISK_ARG_ORIENT 3

#define DIVIDEDDISK_ARG_CENTER 4
#define DIVIDEDDISK_ARG_VECTOR 5

#define DIVIDEDDISK_ARG_TYPE 6

class AdvancedEngine_IDividedDisk
{
public:
  AdvancedEngine_IDividedDisk(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetR(double theR) { _func->SetReal(DIVIDEDDISK_ARG_R, theR); }
  double GetR() { return _func->GetReal(DIVIDEDDISK_ARG_R); }

  void SetRatio(double theRatio) { _func->SetReal(DIVIDEDDISK_ARG_RATIO, theRatio); }
  double GetRatio() { return _func->GetReal(DIVIDEDDISK_ARG_RATIO); }
  
  void SetOrientation(int theOrientation) { _func->SetInteger(DIVIDEDDISK_ARG_ORIENT, theOrientation); }
  int GetOrientation() { return _func->GetInteger(DIVIDEDDISK_ARG_ORIENT); }
  
  void SetType(int theType) { _func->SetInteger(DIVIDEDDISK_ARG_TYPE, theType); }
  int GetType() { return _func->GetInteger(DIVIDEDDISK_ARG_TYPE); }
  
  void SetCenter(Handle(GEOM_Function) theP) { _func->SetReference(DIVIDEDDISK_ARG_CENTER, theP); }
  void SetVector(Handle(GEOM_Function) theV) { _func->SetReference(DIVIDEDDISK_ARG_VECTOR, theV); }
  
  Handle(GEOM_Function) GetCenter() { return _func->GetReference(DIVIDEDDISK_ARG_CENTER); }
  Handle(GEOM_Function) GetVector() { return _func->GetReference(DIVIDEDDISK_ARG_VECTOR); }

private:
  Handle(GEOM_Function) _func;
};

#endif // _AdvancedEngine_IDividedDisk_HXX_
