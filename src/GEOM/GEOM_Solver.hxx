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

#ifndef _GEOM_Solver_HXX_
#define _GEOM_Solver_HXX_

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include "GEOM_Function.hxx"

#include <TDF_LabelSequence.hxx>

class GEOM_Solver
{
 public:
  Standard_EXPORT GEOM_Solver(GEOM_Engine* theEngine) :_engine(theEngine) {}
  Standard_EXPORT ~GEOM_Solver() {}

  //Update the values of all GEOM_Object in the document, theSeq will contain a list of touched labels
  //Note: not Implemented
  Standard_EXPORT bool Update(TDF_LabelSequence& theSeq);
 
  //Updates a value of theObject, theSeq will contain a list of touched labels
  //Note: not implemented
  Standard_EXPORT bool UpdateObject(Handle(GEOM_Object) theObject, TDF_LabelSequence& theSeq);

  //Recomputes a function
  Standard_EXPORT bool ComputeFunction(Handle(GEOM_Function) theFunction);

 private:

  GEOM_Engine* _engine;    

};

#endif
