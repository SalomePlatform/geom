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

//NOTE: This is an intreface to a function for the Pipe creation.
//
#ifndef _GEOMImpl_IPIPE_HXX_
#define _GEOMImpl_IPIPE_HXX_

#include "GEOM_Function.hxx"

#include <TColStd_HArray1OfInteger.hxx>

// ---- GEOMImpl_IPipe
#define PIPE_ARG_BASE          1
#define PIPE_ARG_PATH          2

// ---- GEOMImpl_IPipeBiNormal
#define PIPE_ARG_VEC           3

// ---- GEOMImpl_IPipeDiffSect
#define PIPEDS_LIST_BASES      1
//#define PIPEDS_ARG_PATH      2
#define PIPEDS_LIST_LOCATIONS  3
#define PIPEDS_ARG_WITHCONTACT 4
#define PIPEDS_ARG_WITHCORRECT 5
#define PIPEDS_ARG_BY_STEPS    6

// ---- GEOMImpl_IPipeShellSect
#define PIPEDS_LIST_SUBBASES   6

// ---- Generate groups block (common)
#define PIPE_GENERATE_GROUPS   7
#define PIPE_GROUP_DOWN        8
#define PIPE_GROUP_UP          9
#define PIPE_GROUP_SIDE1       10
#define PIPE_GROUP_SIDE2       11
#define PIPE_GROUP_OTHER       12


class GEOMImpl_IPipe
{
 public:

  GEOMImpl_IPipe(Handle(GEOM_Function) theFunction): _func(theFunction) {}
  virtual ~GEOMImpl_IPipe() {}

  void SetBase (Handle(GEOM_Function) theBase) { _func->SetReference(PIPE_ARG_BASE, theBase); }
  void SetPath (Handle(GEOM_Function) thePath) { _func->SetReference(PIPE_ARG_PATH, thePath); }
  void SetGenerateGroups (int theGenerateGroups)
  { _func->SetInteger(PIPE_GENERATE_GROUPS, theGenerateGroups); }
  void SetGroupDown (const Handle(TColStd_HArray1OfInteger) &theGroup)
  { _func->SetIntegerArray(PIPE_GROUP_DOWN, theGroup); }
  void SetGroupUp (const Handle(TColStd_HArray1OfInteger) &theGroup)
  { _func->SetIntegerArray(PIPE_GROUP_UP, theGroup); }
  void SetGroupSide1 (const Handle(TColStd_HArray1OfInteger) &theGroup)
  { _func->SetIntegerArray(PIPE_GROUP_SIDE1, theGroup); }
  void SetGroupSide2 (const Handle(TColStd_HArray1OfInteger) &theGroup)
  { _func->SetIntegerArray(PIPE_GROUP_SIDE2, theGroup); }
  void SetGroupOther (const Handle(TColStd_HArray1OfInteger) &theGroup)
  { _func->SetIntegerArray(PIPE_GROUP_OTHER, theGroup); }

  Handle(GEOM_Function) GetBase() { return _func->GetReference(PIPE_ARG_BASE); }
  Handle(GEOM_Function) GetPath() { return _func->GetReference(PIPE_ARG_PATH); }
  int GetGenerateGroups () { return _func->GetInteger(PIPE_GENERATE_GROUPS); }
  Handle(TColStd_HArray1OfInteger) GetGroupDown ()
  { return _func->GetIntegerArray(PIPE_GROUP_DOWN); }
  Handle(TColStd_HArray1OfInteger) GetGroupUp ()
  { return _func->GetIntegerArray(PIPE_GROUP_UP); }
  Handle(TColStd_HArray1OfInteger) GetGroupSide1 ()
  { return _func->GetIntegerArray(PIPE_GROUP_SIDE1); }
  Handle(TColStd_HArray1OfInteger) GetGroupSide2 ()
  { return _func->GetIntegerArray(PIPE_GROUP_SIDE2); }
  Handle(TColStd_HArray1OfInteger) GetGroupOther ()
  { return _func->GetIntegerArray(PIPE_GROUP_OTHER); }

 protected:

  Handle(GEOM_Function) _func;
};

#endif
