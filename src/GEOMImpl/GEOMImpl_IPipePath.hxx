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

// NOTE: This is an interface to the function RestorePath.

#ifndef _GEOMImpl_IPIPEPATH_HXX_
#define _GEOMImpl_IPIPEPATH_HXX_

#include "GEOM_Function.hxx"

#include <TColStd_HSequenceOfTransient.hxx>

class GEOMImpl_IPipePath
{
 public:

  enum {
    PIPE_PATH_SHAPE = 1,
    PIPE_PATH_BASE1 = 2,
    PIPE_PATH_BASE2 = 3,
    PIPE_PATH_SEQ1  = 4,
    PIPE_PATH_SEQ2  = 5
  };

  GEOMImpl_IPipePath (Handle(GEOM_Function)& theFunction): _func(theFunction) {}
  
  void SetShape (Handle(GEOM_Function) theShape) { _func->SetReference(PIPE_PATH_SHAPE, theShape); }
  void SetBase1 (Handle(GEOM_Function) theBase1) { _func->SetReference(PIPE_PATH_BASE1, theBase1); }
  void SetBase2 (Handle(GEOM_Function) theBase2) { _func->SetReference(PIPE_PATH_BASE2, theBase2); }
  void SetBaseSeq1 (const Handle(TColStd_HSequenceOfTransient)& theBase1)
                                              { _func->SetReferenceList(PIPE_PATH_SEQ1, theBase1); }
  void SetBaseSeq2 (const Handle(TColStd_HSequenceOfTransient)& theBase2)
                                              { _func->SetReferenceList(PIPE_PATH_SEQ2, theBase2); }

  Handle(GEOM_Function) GetShape() { return _func->GetReference(PIPE_PATH_SHAPE); }
  Handle(GEOM_Function) GetBase1() { return _func->GetReference(PIPE_PATH_BASE1); }
  Handle(GEOM_Function) GetBase2() { return _func->GetReference(PIPE_PATH_BASE2); }
  Handle(TColStd_HSequenceOfTransient) GetBaseSeq1 ()
                                   { return _func->GetReferenceList(PIPE_PATH_SEQ1); }
  Handle(TColStd_HSequenceOfTransient) GetBaseSeq2 ()
                                   { return _func->GetReferenceList(PIPE_PATH_SEQ2); }

 protected:

  Handle(GEOM_Function) _func;
};

#endif
