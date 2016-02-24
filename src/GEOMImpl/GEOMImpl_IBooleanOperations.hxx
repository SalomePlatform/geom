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

#ifndef _GEOMImpl_IBooleanOperations_HXX_
#define _GEOMImpl_IBooleanOperations_HXX_

#include "GEOM_IOperations.hxx"

#include <TColStd_HArray1OfInteger.hxx>
#include "list"

class GEOM_Engine;
class GEOM_Object;

class GEOMImpl_IBooleanOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_IBooleanOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_IBooleanOperations();

  Standard_EXPORT Handle(GEOM_Object) MakeBoolean
                                (Handle(GEOM_Object)    theShape1,
                                 Handle(GEOM_Object)    theShape2,
                                 const Standard_Integer theOp,
                                 const Standard_Boolean IsCheckSelfInte);

  Standard_EXPORT Handle(GEOM_Object) MakeFuse
                                (Handle(GEOM_Object) theShape1,
                                 Handle(GEOM_Object) theShape2,
                                 const bool          IsCheckSelfInte,
                                 const bool          IsRmExtraEdges);

  Standard_EXPORT Handle(GEOM_Object) MakeFuseList
                  (const Handle(TColStd_HSequenceOfTransient)& theShapes,
                   const bool                                  IsCheckSelfInte,
                   const bool                                  IsRmExtraEdges);

  Standard_EXPORT Handle(GEOM_Object) MakeCommonList
                  (const Handle(TColStd_HSequenceOfTransient)& theShapes,
                   const Standard_Boolean IsCheckSelfInte);

  Standard_EXPORT Handle(GEOM_Object) MakeCutList
                  (Handle(GEOM_Object) theMainShape,
                   const Handle(TColStd_HSequenceOfTransient)& theShapes,
                   const Standard_Boolean IsCheckSelfInte);

  Standard_EXPORT Handle(GEOM_Object) MakePartition
                  (const Handle(TColStd_HSequenceOfTransient)& theShapes,
                   const Handle(TColStd_HSequenceOfTransient)& theTools,
                   const Handle(TColStd_HSequenceOfTransient)& theKeepInside,
                   const Handle(TColStd_HSequenceOfTransient)& theRemoveInside,
                   const Standard_Integer                      theLimit,
                   const Standard_Boolean                      theRemoveWebs,
                   const Handle(TColStd_HArray1OfInteger)&     theMaterials,
                   const Standard_Integer theKeepNonlimitShapes,
                   const Standard_Boolean thePerformSelfIntersections,
                   const Standard_Boolean IsCheckSelfInte);

  Standard_EXPORT Handle(GEOM_Object) MakeHalfPartition (Handle(GEOM_Object)    theShape,
                                                         Handle(GEOM_Object)    thePlane);

private:

  Handle(TColStd_HSequenceOfTransient) getShapeFunctions
                  (const Handle(TColStd_HSequenceOfTransient)& theObjects,
                         TCollection_AsciiString &theDescription);

};

#endif
