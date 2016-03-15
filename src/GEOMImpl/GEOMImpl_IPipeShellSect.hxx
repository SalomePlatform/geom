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

//NOTE: This is an intreface to a function for the Pipe creation.

#ifndef _GEOMImpl_IPIPESHELLSECT_HXX_
#define _GEOMImpl_IPIPESHELLSECT_HXX_


#include "GEOMImpl_IPipeDiffSect.hxx"
#include "GEOM_Function.hxx"
#include <TColStd_HSequenceOfTransient.hxx>

// Position definitions are declared in the base class GEOMImpl_IPipe.

class GEOMImpl_IPipeShellSect : public GEOMImpl_IPipeDiffSect
{
 public:

  GEOMImpl_IPipeShellSect(const Handle(GEOM_Function)& theFunction):
    GEOMImpl_IPipeDiffSect(theFunction) {}
  
  void SetSubBases (const Handle(TColStd_HSequenceOfTransient)& theSubBases) 
  {
    _func->SetReferenceList(PIPEDS_LIST_SUBBASES,theSubBases);
  }

  Handle(TColStd_HSequenceOfTransient) GetSubBases ()
  { 
    Handle(TColStd_HSequenceOfTransient) aSubBases =
      _func->GetReferenceList(PIPEDS_LIST_SUBBASES);
    return aSubBases; 
  }

};

#endif
