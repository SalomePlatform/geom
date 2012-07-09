//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

#ifndef _GEOMImpl_IDividedDisk_HXX_
#define _GEOMImpl_IDividedDisk_HXX_

#include "GEOM_Function.hxx"

#define DIVIDEDDISK_ARG_R      1
#define DIVIDEDDISK_ARG_RATIO  2
#define DIVIDEDDISK_ARG_ORIENT 3

class GEOMImpl_IDividedDisk
{
public:
  GEOMImpl_IDividedDisk(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetR(double theR) { _func->SetReal(DIVIDEDDISK_ARG_R, theR); }
  double GetR() { return _func->GetReal(DIVIDEDDISK_ARG_R); }

  void SetRatio(double theRatio) { _func->SetReal(DIVIDEDDISK_ARG_RATIO, theRatio); }
  double GetRatio() { return _func->GetReal(DIVIDEDDISK_ARG_RATIO); }
  
  void SetOrientation(int theOrientation) { _func->SetInteger(DIVIDEDDISK_ARG_ORIENT, theOrientation); }
  double GetOrientation() { return _func->GetInteger(DIVIDEDDISK_ARG_ORIENT); }

private:
  Handle(GEOM_Function) _func;
};

#endif // _GEOMImpl_IDividedDisk_HXX_
