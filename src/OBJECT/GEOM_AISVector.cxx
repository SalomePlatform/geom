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

// GEOM OBJECT : interactive object for Geometry entities visualization
// File   : GEOM_AISVector.cxx
// Author : Julia DOROVSKIKH
//
#include <GEOM_AISVector.hxx>

// OCCT Includes
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_PresentationManager3d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GEOM_AISVector, GEOM_AISShape)

//=======================================================================
//function : GEOM_AISVector
//purpose  : Constructor
//=======================================================================
GEOM_AISVector::GEOM_AISVector (const TopoDS_Shape& theShape, const Standard_CString theName)
  : GEOM_AISShape(theShape, theName)
{
}

//=======================================================================
//function : Compute
//purpose  : Compute a presentation
//=======================================================================
void GEOM_AISVector::Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                              const Handle(Prs3d_Presentation)& thePrs,
                              const Standard_Integer theMode)
{
  GEOM_AISShape::Compute(thePresentationManager, thePrs, theMode);
}
