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

#ifndef _GEOMImpl_IFieldOperations_HXX_
#define _GEOMImpl_IFieldOperations_HXX_

#include "GEOM_IOperations.hxx"

#include <GEOM_Field.hxx>

#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>

class GEOM_Object;

class GEOMImpl_IFieldOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_IFieldOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_IFieldOperations();

  Standard_EXPORT Handle(GEOM_Field)
    CreateField ( const Handle(GEOM_Object)&                     theShape,
                  const char*                                    theName,
                  const int                                      theType,
                  const int                                      theDimension,
                  const Handle(TColStd_HArray1OfExtendedString)& theComponentNames);
  /*!
   *  \brief Returns number of fields on a shape
   */
  Standard_EXPORT int CountFields( const Handle(GEOM_Object)& shape);

  /*!
   *  \brief Returns all fields on a shape
   */
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
    GetFields( const Handle(GEOM_Object)& shape );

  /*!
   *  \brief Returns a field on a shape by its name
   */
  Standard_EXPORT Handle(GEOM_Field) GetField( const Handle(GEOM_Object)& shape,
                                               const char*                name);
};

#endif
