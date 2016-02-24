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

// GEOM OBJECT : interactive object for Geometry entities visualization
// File   : GEOM_AISVector.hxx
// Author : Julia DOROVSKIKH
// Module : GEOM
//
#ifndef GEOM_AISVector_HeaderFile
#define GEOM_AISVector_HeaderFile

#include <GEOM_AISShape.hxx>
#include <Standard_DefineHandle.hxx>

/*!
 * \class GEOM_AISVector
 * \brief Interactive object, representing a vector with arrow on its end
 */

class GEOM_AISVector : public GEOM_AISShape
{
public:
  /*!
   * Constructor
   * \param theShape A linear edge to be represented as a vector
   * \param theName A name to be passed in constructor of \a GEOM_AISShape
   */
  Standard_EXPORT GEOM_AISVector (const TopoDS_Shape& theShape, const Standard_CString theName);

protected:
  /*!
   * Redefined from GEOM_AISShape
   */
  virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                        const Handle(Prs3d_Presentation)& thePresentation,
                        const Standard_Integer theMode = 0);

  virtual bool isShowVectors() {return true;}

public:
  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_AISVector,GEOM_AISShape)
};

DEFINE_STANDARD_HANDLE(GEOM_AISVector, GEOM_AISShape)

#endif
