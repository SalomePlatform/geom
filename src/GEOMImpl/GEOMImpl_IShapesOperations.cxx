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

//  File      : GEOMImpl_IShapesOperations.cxx
//  Created   :
//  Author    : modified by Lioka RAZAFINDRAZAKA (CEA) 22/06/2007
//  Project   : SALOME

//#include <Standard_Stream.hxx>

#include "GEOMImpl_IShapesOperations.hxx"

#include "GEOMImpl_Types.hxx"

#include "GEOMImpl_VectorDriver.hxx"
#include "GEOMImpl_ShapeDriver.hxx"
#include "GEOMImpl_GlueDriver.hxx"
#include "GEOMImpl_FillingDriver.hxx"

#include "GEOMImpl_IExtract.hxx"
#include "GEOMImpl_IVector.hxx"
#include "GEOMImpl_IShapes.hxx"
#include "GEOMImpl_IShapeExtend.hxx"
#include "GEOMImpl_IGlue.hxx"
#include "GEOMImpl_IFilling.hxx"

#include "GEOMImpl_Block6Explorer.hxx"
#include "GEOMImpl_IHealingOperations.hxx"

#include "GEOMImpl_Gen.hxx"

#include "GEOM_Function.hxx"
#include "GEOM_ISubShape.hxx"
#include "GEOM_PythonDump.hxx"

#include "GEOMUtils.hxx"

#include "GEOMAlgo_ClsfBox.hxx"
#include "GEOMAlgo_ClsfQuad.hxx"
#include "GEOMAlgo_ClsfSolid.hxx"
#include "GEOMAlgo_ClsfSurf.hxx"
#include "GEOMAlgo_FinderShapeOn2.hxx"
#include "GEOMAlgo_GetInPlace.hxx"
#include "GEOMAlgo_GetInPlaceAPI.hxx"
#include "GEOMAlgo_GlueDetector.hxx"

#include <utilities.h>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <GeomLib_Tool.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Precision.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_TShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>

#include <vector>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

namespace {

  void AddFlatSubShapes(const TopoDS_Shape& S, TopTools_ListOfShape& L, TopTools_MapOfShape& M)
  {
    if (S.ShapeType() != TopAbs_COMPOUND) {
      L.Append(S);
    }
    else {
      TopoDS_Iterator It(S, Standard_True, Standard_True);
      for (; It.More(); It.Next()) {
	TopoDS_Shape SS = It.Value();
	if (M.Add(SS))
	  AddFlatSubShapes(SS, L, M);
      }
    }
  }
}

namespace
{
  const double MAX_TOLERANCE = 1.e-7;

  /**
   * \brief Returns the vertex from theWhere shape that is coincident with
   * theVertex.
   *
   * \param theWhere the shape where the coinsident vertex is searched.
   * \param theVertex the vertex to be searched.
   * \return the coincident vertex if it is found. Otherwise null object.
   */
  static TopoDS_Vertex getSameVertex(const TopoDS_Shape  &theWhere,
                                     const TopoDS_Vertex &theVertex)
  {
    TopoDS_Vertex       aResult;
    gp_Pnt              aPoint = BRep_Tool::Pnt(theVertex);
    TopExp_Explorer     anExp(theWhere, TopAbs_VERTEX);
    TopTools_MapOfShape aMap;
    
    for(; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &aLocalShape = anExp.Current();

      if(!aMap.Add(aLocalShape)) {
        continue;
      }

      TopoDS_Vertex aVertex = TopoDS::Vertex(aLocalShape);
      gp_Pnt        aPoint2 = BRep_Tool::Pnt(aVertex);

      if(aPoint.Distance(aPoint2) <= MAX_TOLERANCE) {
        aResult = aVertex;
        break;
      }
    }

    return aResult;
  }
} // end of namespace

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOMImpl_IShapesOperations::GEOMImpl_IShapesOperations (GEOM_Engine* theEngine, int theDocID)
: GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("GEOMImpl_IShapesOperations::GEOMImpl_IShapesOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_IShapesOperations::~GEOMImpl_IShapesOperations()
{
  MESSAGE("GEOMImpl_IShapesOperations::~GEOMImpl_IShapesOperations");
}

//=============================================================================
/*!
 *  MakeEdge
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeEdge
                     (Handle(GEOM_Object) thePnt1, Handle(GEOM_Object) thePnt2)
{
  SetErrorCode(KO);

  if (thePnt1.IsNull() || thePnt2.IsNull()) return NULL;

  //Add a new Edge object
  Handle(GEOM_Object) anEdge = GetEngine()->AddObject(GetDocID(), GEOM_EDGE);

  //Add a new Vector function
  Handle(GEOM_Function) aFunction =
    anEdge->AddFunction(GEOMImpl_VectorDriver::GetID(), VECTOR_TWO_PNT);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_VectorDriver::GetID()) return NULL;

  GEOMImpl_IVector aPI (aFunction);

  Handle(GEOM_Function) aRef1 = thePnt1->GetLastFunction();
  Handle(GEOM_Function) aRef2 = thePnt2->GetLastFunction();
  if (aRef1.IsNull() || aRef2.IsNull()) return NULL;

  aPI.SetPoint1(aRef1);
  aPI.SetPoint2(aRef2);

  //Compute the Edge value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Vector driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << anEdge << " = geompy.MakeEdge("
                               << thePnt1 << ", " << thePnt2 << ")";

  SetErrorCode(OK);
  return anEdge;
}

//=============================================================================
/*!
 *  MakeEdgeOnCurveByLength
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeEdgeOnCurveByLength
                     (Handle(GEOM_Object) theRefCurve,
                      const Standard_Real theLength,
                      Handle(GEOM_Object) theStartPoint)
{
  SetErrorCode(KO);

  if (theRefCurve.IsNull()) return NULL;

  //Add a new Edge object
  Handle(GEOM_Object) anEdge = GetEngine()->AddObject(GetDocID(), GEOM_EDGE);

  //Add a new Vector function
  Handle(GEOM_Function) aFunction =
    anEdge->AddFunction(GEOMImpl_ShapeDriver::GetID(), EDGE_CURVE_LENGTH);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IVector aPI (aFunction);

  Handle(GEOM_Function) aRef1 = theRefCurve->GetLastFunction();
  if (aRef1.IsNull()) return NULL;
  aPI.SetPoint1(aRef1);

  if (!theStartPoint.IsNull()) {
    Handle(GEOM_Function) aRef2 = theStartPoint->GetLastFunction();
    aPI.SetPoint2(aRef2);
  }

  aPI.SetParameter(theLength);

  //Compute the Edge value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Vector driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << anEdge << " = geompy.MakeEdgeOnCurveByLength("
                               << theRefCurve << ", " << theLength << ", " << theStartPoint << ")";

  SetErrorCode(OK);
  return anEdge;
}

//=============================================================================
/*!
 *  MakeEdgeWire
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeEdgeWire
                    (Handle(GEOM_Object) theWire,
                     const Standard_Real theLinearTolerance,
                     const Standard_Real theAngularTolerance)
{
  SetErrorCode(KO);

  if (theWire.IsNull()) return NULL;

  //Add a new Edge object
  Handle(GEOM_Object) anEdge = GetEngine()->AddObject(GetDocID(), GEOM_EDGE);

  //Add a new Vector function
  Handle(GEOM_Function) aFunction =
    anEdge->AddFunction(GEOMImpl_ShapeDriver::GetID(), EDGE_WIRE);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aCI (aFunction);

  Handle(GEOM_Function) aWire = theWire->GetLastFunction();

  if (aWire.IsNull()) return NULL;

  aCI.SetBase(aWire);
  aCI.SetTolerance(theLinearTolerance);
  aCI.SetAngularTolerance(theAngularTolerance);

  //Compute the Edge value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  const double DEF_LIN_TOL = Precision::Confusion();
  const double DEF_ANG_TOL = Precision::Angular();
  //Make a Python command
  if ( theAngularTolerance == DEF_ANG_TOL ) {
    if ( theLinearTolerance == DEF_LIN_TOL )
      GEOM::TPythonDump(aFunction) << anEdge  << " = geompy.MakeEdgeWire("
                                   << theWire << ")";
    else
      GEOM::TPythonDump(aFunction) << anEdge  << " = geompy.MakeEdgeWire("
                                   << theWire << ", " << theLinearTolerance << ")";
  }
  else {
    GEOM::TPythonDump(aFunction) << anEdge  << " = geompy.MakeEdgeWire("
                                 << theWire << ", " << theLinearTolerance << ", "
                                 << theAngularTolerance << ")";
  }

  SetErrorCode(OK);
  return anEdge;
}

//=============================================================================
/*!
 *  MakeWire
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeWire
                             (std::list<Handle(GEOM_Object)> theShapes,
                              const Standard_Real            theTolerance)
{
  SetErrorCode(KO);

  //Add a new object
  Handle(GEOM_Object) aWire = GetEngine()->AddObject(GetDocID(), GEOM_WIRE);

  //Add a new function
  Handle(GEOM_Function) aFunction =
    aWire->AddFunction(GEOMImpl_ShapeDriver::GetID(), WIRE_EDGES);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aCI (aFunction);
  aCI.SetTolerance(theTolerance);

  Handle(TColStd_HSequenceOfTransient) aShapesSeq = new TColStd_HSequenceOfTransient;

  // Shapes
  std::list<Handle(GEOM_Object)>::iterator it = theShapes.begin();
  for (; it != theShapes.end(); it++) {
    Handle(GEOM_Function) aRefSh = (*it)->GetLastFunction();
    if (aRefSh.IsNull()) {
      SetErrorCode("NULL argument shape for the shape construction");
      return NULL;
    }
    aShapesSeq->Append(aRefSh);
  }
  aCI.SetShapes(aShapesSeq);

  //Compute the shape
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aWire << " = geompy.MakeWire([";

  // Shapes
  it = theShapes.begin();
  if (it != theShapes.end()) {
    pd << (*it++);
    while (it != theShapes.end()) {
      pd << ", " << (*it++);
    }
  }
  pd << "], " << theTolerance << ")";

  SetErrorCode(OK);
  return aWire;
}

//=============================================================================
/*!
 *  MakeFace
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeFace (Handle(GEOM_Object) theWire,
                                                          const bool isPlanarWanted)
{
  SetErrorCode(KO);

  if (theWire.IsNull()) return NULL;

  //Add a new Face object
  Handle(GEOM_Object) aFace = GetEngine()->AddObject(GetDocID(), GEOM_FACE);

  //Add a new Shape function for creation of a face from a wire
  Handle(GEOM_Function) aFunction =
    aFace->AddFunction(GEOMImpl_ShapeDriver::GetID(), FACE_WIRE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aCI (aFunction);

  Handle(GEOM_Function) aRefWire = theWire->GetLastFunction();

  if (aRefWire.IsNull()) return NULL;

  aCI.SetBase(aRefWire);
  aCI.SetIsPlanar(isPlanarWanted);

  //Compute the Face value
  Standard_Boolean isWarning = Standard_False;
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed to compute a face");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    // to provide warning
    if (!aFunction->GetValue().IsNull()) {
      isWarning = Standard_True;
    } else {
      return NULL;
    }
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aFace << " = geompy.MakeFace("
    << theWire << ", " << (int)isPlanarWanted << ")";

  // to provide warning
  if (!isWarning) SetErrorCode(OK);
  return aFace;
}

//=============================================================================
/*!
 *  MakeFaceWires
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeFaceWires
                             (std::list<Handle(GEOM_Object)> theShapes,
                              const bool isPlanarWanted)
{
  SetErrorCode(KO);

  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_FACE);

  //Add a new function
  Handle(GEOM_Function) aFunction =
    aShape->AddFunction(GEOMImpl_ShapeDriver::GetID(), FACE_WIRES);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aCI (aFunction);

  Handle(TColStd_HSequenceOfTransient) aShapesSeq = new TColStd_HSequenceOfTransient;

  // Shapes
  std::list<Handle(GEOM_Object)>::iterator it = theShapes.begin();
  for (; it != theShapes.end(); it++) {
    Handle(GEOM_Function) aRefSh = (*it)->GetLastFunction();
    if (aRefSh.IsNull()) {
      SetErrorCode("NULL argument shape for the face construction");
      return NULL;
    }
    aShapesSeq->Append(aRefSh);
  }
  aCI.SetShapes(aShapesSeq);

  aCI.SetIsPlanar(isPlanarWanted);

  //Compute the shape
  Standard_Boolean isWarning = Standard_False;
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    // to provide warning
    if (!aFunction->GetValue().IsNull()) {
      isWarning = Standard_True;
    } else {
      return NULL;
    }
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aShape << " = geompy.MakeFaceWires([";

  // Shapes
  it = theShapes.begin();
  if (it != theShapes.end()) {
    pd << (*it++);
    while (it != theShapes.end()) {
      pd << ", " << (*it++);
    }
  }
  pd << "], " << (int)isPlanarWanted << ")";

  // to provide warning
  if (!isWarning) SetErrorCode(OK);
  return aShape;
}

//=============================================================================
/*!
 *  MakeFaceFromSurface
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeFaceFromSurface
                                              (Handle(GEOM_Object) theFace,
                                               Handle(GEOM_Object) theWire)
{
  SetErrorCode(KO);

  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_FACE);

  //Add a new function
  Handle(GEOM_Function) aFunction =
    aShape->AddFunction(GEOMImpl_ShapeDriver::GetID(), FACE_FROM_SURFACE);

  if (aFunction.IsNull()) {
    return NULL;
  }

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) {
    return NULL;
  }

  GEOMImpl_IShapes aCI (aFunction);
  Handle(TColStd_HSequenceOfTransient) aShapesSeq =
    new TColStd_HSequenceOfTransient;
  Handle(GEOM_Function) aRefFace = theFace->GetLastFunction();
  Handle(GEOM_Function) aRefWire = theWire->GetLastFunction();

  if (aRefFace.IsNull()) {
    SetErrorCode("NULL argument face for the face construction");
    return NULL;
  }

  if (aRefWire.IsNull()) {
    SetErrorCode("NULL argument wire for the face construction");
    return NULL;
  }

  aShapesSeq->Append(aRefFace);
  aShapesSeq->Append(aRefWire);

  aCI.SetShapes(aShapesSeq);

  //Compute the face
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump (aFunction) << aShape
    << " = geompy.MakeFaceFromSurface(" << theFace << ", " << theWire << ")";

  SetErrorCode(OK);

  return aShape;
}

//=============================================================================
/*!
 *  MakeFaceWithConstraints
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeFaceWithConstraints
                             (std::list<Handle(GEOM_Object)> theConstraints)
{
  SetErrorCode(KO);

  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_FILLING);

  //Add a new function
  Handle(GEOM_Function) aFunction =
    aShape->AddFunction(GEOMImpl_FillingDriver::GetID(), FILLING_ON_CONSTRAINTS);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_FillingDriver::GetID()) return NULL;

  GEOMImpl_IFilling aCI (aFunction);
  Handle(TColStd_HSequenceOfTransient) aConstraints = new TColStd_HSequenceOfTransient;

  // Shapes
  std::list<Handle(GEOM_Object)>::iterator it = theConstraints.begin();
  while (it != theConstraints.end()) {
    Handle(GEOM_Object) anObject = (*it);
    if ( anObject.IsNull() || anObject->GetValue().ShapeType() != TopAbs_EDGE ) {
      SetErrorCode("NULL argument edge for the face construction");
      return NULL;
    }
    Handle(GEOM_Function) aRefSh = anObject->GetLastFunction();
    aConstraints->Append(aRefSh);
    it++;
    if ( it != theConstraints.end() ) {
      Handle(GEOM_Object) aFace = (*it);
      if ( aFace.IsNull() ) {
        // null constraint face - it is a valid case
        it++;
        continue;
      }
      if ( aFace->GetValue().ShapeType() != TopAbs_FACE )
        // constraint face can be omitted - it is a valid case
        continue;
      // Keep the old error code as IsSubShapeBelongsTo changes it.
      TCollection_AsciiString anOldCode = GetErrorCode();

      if ( IsSubShapeBelongsTo( anObject, 0, aFace, 0 ) ) {
        // valid constraint
        SetErrorCode(anOldCode);
        aRefSh = aFace->GetLastFunction();
        aConstraints->Append(aRefSh);
        it++;
      }
      else {
        // bad constraint
        SetErrorCode("Face is NULL or not connected to the Edge");
        return NULL;
      }
    }
  }
  aCI.SetShapes( aConstraints );

  //Compute the shape
  Standard_Boolean isWarning = Standard_False;
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    // to provide warning
    if (!aFunction->GetValue().IsNull()) {
      isWarning = Standard_True;
    } else {
      return NULL;
    }
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aShape << " = geompy.MakeFaceWithConstraints([";

  // Constraints
  it = theConstraints.begin();
  if (it != theConstraints.end() ) {
    pd << (*it++);
    while (it != theConstraints.end()) {
      Handle(GEOM_Object) anObject = (*it++);
      if( !anObject.IsNull() )
        pd << ", " << anObject;
    }
  }
  pd << "])";

  // to provide warning
  if (!isWarning) SetErrorCode(OK);
  return aShape;
}

//=============================================================================
/*!
 *  MakeShell
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeShell
                             (std::list<Handle(GEOM_Object)> theShapes)
{
  return MakeShape(theShapes, GEOM_SHELL, SHELL_FACES, "MakeShell");
}

//=============================================================================
/*!
 *  MakeSolidShells
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeSolidShells
                             (std::list<Handle(GEOM_Object)> theShapes)
{
  return MakeShape(theShapes, GEOM_SOLID, SOLID_SHELLS, "MakeSolid");
}

//=============================================================================
/*!
 *  MakeCompound
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeCompound
                             (std::list<Handle(GEOM_Object)> theShapes)
{
  return MakeShape(theShapes, GEOM_COMPOUND, COMPOUND_SHAPES, "MakeCompound");
}

//=============================================================================
/*!
 *  MakeShape
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeShape
                             (std::list<Handle(GEOM_Object)> theShapes,
                              const Standard_Integer         theObjectType,
                              const Standard_Integer         theFunctionType,
                              const TCollection_AsciiString& theMethodName)
{
  SetErrorCode(KO);

  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), theObjectType);

  //Add a new function
  Handle(GEOM_Function) aFunction =
    aShape->AddFunction(GEOMImpl_ShapeDriver::GetID(), theFunctionType);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aCI (aFunction);

  Handle(TColStd_HSequenceOfTransient) aShapesSeq = new TColStd_HSequenceOfTransient;

  // Shapes
  std::list<Handle(GEOM_Object)>::iterator it = theShapes.begin();
  for (; it != theShapes.end(); it++) {
    Handle(GEOM_Function) aRefSh = (*it)->GetLastFunction();
    if (aRefSh.IsNull()) {
      SetErrorCode("NULL argument shape for the shape construction");
      return NULL;
    }
    aShapesSeq->Append(aRefSh);
  }
  aCI.SetShapes(aShapesSeq);

  //Compute the shape
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aShape << " = geompy." << theMethodName.ToCString() << "([";

  // Shapes
  it = theShapes.begin();
  if (it != theShapes.end()) {
    pd << (*it++);
    while (it != theShapes.end()) {
      pd << ", " << (*it++);
    }
  }
  pd << "])";

  SetErrorCode(OK);
  return aShape;
}

//=============================================================================
/*!
 *  MakeSolidFromConnectedFaces
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeSolidFromConnectedFaces
                             (std::list<Handle(GEOM_Object)> theFacesOrShells,
                              const Standard_Boolean isIntersect)
{
  SetErrorCode(KO);

  //Add a new object
  Handle(GEOM_Object) aSolid = GetEngine()->AddObject(GetDocID(), GEOM_SOLID);

  //Add a new function
  Handle(GEOM_Function) aFunction =
    aSolid->AddFunction(GEOMImpl_ShapeDriver::GetID(), SOLID_FACES);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aCI (aFunction);

  Handle(TColStd_HSequenceOfTransient) aShapesSeq = new TColStd_HSequenceOfTransient;

  // Shapes
  std::list<Handle(GEOM_Object)>::iterator it = theFacesOrShells.begin();
  for (; it != theFacesOrShells.end(); it++) {
    Handle(GEOM_Function) aRefSh = (*it)->GetLastFunction();
    if (aRefSh.IsNull()) {
      SetErrorCode("NULL argument shape for the shape construction");
      return NULL;
    }
    aShapesSeq->Append(aRefSh);
  }
  aCI.SetShapes(aShapesSeq);
  aCI.SetIsIntersect(isIntersect);

  //Compute the shape
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aSolid << " = geompy.MakeSolidFromConnectedFaces([";

  // Shapes
  it = theFacesOrShells.begin();
  if (it != theFacesOrShells.end()) {
    pd << (*it++);
    while (it != theFacesOrShells.end()) {
      pd << ", " << (*it++);
    }
  }
  pd << "]," << (isIntersect ? "True" : "False") << ")";

  SetErrorCode(OK);
  return aSolid;
}

//=============================================================================
/*!
 *  MakeGlueFaces
 */
//=============================================================================
Handle(GEOM_Object)
GEOMImpl_IShapesOperations::MakeGlueFaces (std::list< Handle(GEOM_Object) >& theShapes,
                                           const Standard_Real               theTolerance,
                                           const Standard_Boolean            doKeepNonSolids)
{
  SetErrorCode(KO);

  Handle(TColStd_HSequenceOfTransient) objects = GEOM_Object::GetLastFunctions( theShapes );
  if ( objects.IsNull() || objects->IsEmpty() ) {
    SetErrorCode("NULL argument shape");
    return NULL;
  }

  //Add a new Glued object
  Handle(GEOM_Object) aGlued = GetEngine()->AddObject(GetDocID(), GEOM_GLUED);

  //Add a new Glue function
  Handle(GEOM_Function) aFunction;
  aFunction = aGlued->AddFunction(GEOMImpl_GlueDriver::GetID(), GLUE_FACES);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_GlueDriver::GetID()) return NULL;

  GEOMImpl_IGlue aCI (aFunction);

  aCI.SetBase( objects );
  aCI.SetTolerance(theTolerance);
  aCI.SetKeepNonSolids(doKeepNonSolids);

  //Compute the sub-shape value
  Standard_Boolean isWarning = Standard_False;
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed to glue faces");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    // to provide warning
    if (!aFunction->GetValue().IsNull()) {
      isWarning = Standard_True;
    } else {
      return NULL;
    }
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aGlued << " = geompy.MakeGlueFaces("
    << theShapes << ", " << theTolerance << ")";

  // to provide warning
  if (!isWarning) SetErrorCode(OK);
  return aGlued;
}

//=============================================================================
/*!
 *  GetGlueFaces
 */
//=============================================================================
/*
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetGlueFaces
                                                (Handle(GEOM_Object) theShape,
                                                 const Standard_Real theTolerance)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) return NULL;

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;

  Standard_Integer iErr;
  TopoDS_Shape aS;
  GEOMAlgo_Gluer1 aGluer;
  GEOMAlgo_ListIteratorOfListOfCoupleOfShapes aItCS;
  GEOMAlgo_CoupleOfShapes aCS;
  GEOMAlgo_ListOfCoupleOfShapes aLCS;

  //aGluer = new GEOMAlgo_Gluer1;
  aGluer.SetShape(aShape);
  aGluer.SetTolerance(theTolerance);
  aGluer.Perform();
  iErr = aGluer.ErrorStatus();
  if (iErr) return NULL;

  TopTools_ListOfShape listShape;
  const GEOMAlgo_ListOfCoupleOfShapes& aLCSG = aGluer.GluedFaces();
  // Access to faces
  aItCS.Initialize(aLCSG);
  for (; aItCS.More(); aItCS.Next()) {
    const GEOMAlgo_CoupleOfShapes& aCSG = aItCS.Value();
    listShape.Append(aCSG.Shape1());
  }

  TopTools_ListIteratorOfListOfShape itSub (listShape);
  TCollection_AsciiString anAsciiList, anEntry;
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);
  Handle(TColStd_HArray1OfInteger) anArray;
  Handle(GEOM_Object) anObj;
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    TopoDS_Shape aValue = itSub.Value();
    anArray = new TColStd_HArray1OfInteger(1,1);
    anArray->SetValue(1, anIndices.FindIndex(aValue));
    anObj = GetEngine()->AddSubShape(theShape, anArray);
    if (!anObj.IsNull()) {
      aSeq->Append(anObj);

      // for python command
      TDF_Tool::Entry(anObj->GetEntry(), anEntry);
      anAsciiList += anEntry;
      anAsciiList += ",";
    }
  }

  //Make a Python command
  if( anAsciiList.Length() > 0 ) {
    anAsciiList.Trunc(anAsciiList.Length() - 1);
    Handle(GEOM_Function) aFunction = theShape->GetLastFunction();
    GEOM::TPythonDump pd (aFunction, true);
    pd << "[" << anAsciiList.ToCString();
    pd << "] = geompy.GetGlueFaces(" << theShape << ", " << theTolerance << ")";
  }

  SetErrorCode(OK);

  return aSeq;
}
*/

//=============================================================================
/*!
 *  MakeGlueFacesByList
 */
//=============================================================================
Handle(GEOM_Object)
GEOMImpl_IShapesOperations::MakeGlueFacesByList(std::list< Handle(GEOM_Object) >& theShapes,
                                                const Standard_Real               theTolerance,
                                                std::list<Handle(GEOM_Object)> &  theFaces,
                                                const Standard_Boolean            doKeepNonSolids,
                                                const Standard_Boolean            doGlueAllEdges)
{
  SetErrorCode(KO);

  Handle(TColStd_HSequenceOfTransient) objects = GEOM_Object::GetLastFunctions( theShapes );
  if ( objects.IsNull() || objects->IsEmpty() ) {
    SetErrorCode("NULL argument shape");
    return NULL;
  }
  Handle(TColStd_HSequenceOfTransient) aFaces = GEOM_Object::GetLastFunctions( theFaces );
  if ( aFaces.IsNull() ) {
    SetErrorCode("NULL argument shape for the shape construction");
    return NULL;
  }

  //Add a new Glued object
  Handle(GEOM_Object) aGlued = GetEngine()->AddObject(GetDocID(), GEOM_GLUED);

  //Add a new Glue function
  Handle(GEOM_Function) aFunction;
  aFunction = aGlued->AddFunction(GEOMImpl_GlueDriver::GetID(), GLUE_FACES_BY_LIST);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_GlueDriver::GetID()) return NULL;

  GEOMImpl_IGlue aCI (aFunction);

  aCI.SetBase( objects );
  aCI.SetTolerance(theTolerance);
  aCI.SetKeepNonSolids(doKeepNonSolids);
  aCI.SetGlueAllEdges(doGlueAllEdges);
  aCI.SetFaces(aFaces);

  //Compute the sub-shape value
  Standard_Boolean isWarning = Standard_False;
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed to glue faces");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    // to provide warning
    if (!aFunction->GetValue().IsNull()) {
      isWarning = Standard_True;
    } else {
      return NULL;
    }
  }

  //Make a Python command

  GEOM::TPythonDump pd(aFunction);
  pd << aGlued << " = geompy.MakeGlueFacesByList("
     << theShapes << ", " << theTolerance << ", " << theFaces << ", "
     << (bool)doKeepNonSolids << ", " << (bool)doGlueAllEdges << ")";

  // to provide warning
  if (!isWarning) SetErrorCode(OK);
  return aGlued;
}

//=============================================================================
/*!
 *  MakeGlueEdges
 */
//=============================================================================
Handle(GEOM_Object)
GEOMImpl_IShapesOperations::MakeGlueEdges (std::list< Handle(GEOM_Object) >& theShapes,
                                           const Standard_Real               theTolerance)
{
  SetErrorCode(KO);

  Handle(TColStd_HSequenceOfTransient) objects = GEOM_Object::GetLastFunctions( theShapes );
  if ( objects.IsNull() || objects->IsEmpty() ) {
    SetErrorCode("NULL argument shape");
    return NULL;
  }

  //Add a new Glued object
  Handle(GEOM_Object) aGlued = GetEngine()->AddObject(GetDocID(), GEOM_GLUED);

  //Add a new Glue function
  Handle(GEOM_Function) aFunction;
  aFunction = aGlued->AddFunction(GEOMImpl_GlueDriver::GetID(), GLUE_EDGES);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_GlueDriver::GetID()) return NULL;

  GEOMImpl_IGlue aCI (aFunction);

  aCI.SetBase( objects );
  aCI.SetTolerance(theTolerance);
  aCI.SetKeepNonSolids(true);

  //Compute the sub-shape value
  Standard_Boolean isWarning = Standard_False;
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed to glue edges");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    // to provide warning
    if (!aFunction->GetValue().IsNull()) {
      isWarning = Standard_True;
    } else {
      return NULL;
    }
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aGlued << " = geompy.MakeGlueEdges("
    << theShapes << ", " << theTolerance << ")";

  // to provide warning
  if (!isWarning) SetErrorCode(OK);
  return aGlued;
}

//=============================================================================
/*!
 *  GetGlueShapes
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
GEOMImpl_IShapesOperations::GetGlueShapes (std::list< Handle(GEOM_Object) >& theShapes,
                                           const Standard_Real               theTolerance,
                                           const TopAbs_ShapeEnum            theType)
{
  SetErrorCode(KO);

  TopoDS_Shape aShape;
  TopTools_SequenceOfShape shapes;
  std::list< Handle(GEOM_Object) >::iterator s = theShapes.begin();
  Handle(GEOM_Object) lastCreatedGO;
  for ( ; s != theShapes.end(); ++s )
  {
    Handle(GEOM_Object) go = *s;
    if ( go.IsNull() ) return NULL;
    aShape = go->GetValue();
    if ( aShape.IsNull() ) return NULL;
    shapes.Append( aShape );
    lastCreatedGO = GEOM::GetCreatedLast( lastCreatedGO, go );
  }
  if ( shapes.Length() > 1 )
  {
    TopoDS_Compound compound;
    BRep_Builder builder;
    builder.MakeCompound( compound );
    for ( int i = 1; i <= shapes.Length(); ++i )
      builder.Add( compound, shapes( i ) );

    aShape = compound;
  }

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;

  GEOMAlgo_GlueDetector aGluer;
  aGluer.SetArgument(aShape);
  aGluer.SetTolerance(theTolerance);
  aGluer.Perform();
  Standard_Integer iErr = aGluer.ErrorStatus();
  if (iErr) return NULL;

  std::vector< TopTools_IndexedMapOfShape* > anIndices( shapes.Length(), NULL );
  Handle(TColStd_HArray1OfInteger) anArray;
  Handle(GEOM_Object) anObj;

  TopTools_ListOfShape listOnePerSet;

  const TopTools_DataMapOfShapeListOfShape& aImages = aGluer.Images();
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItDMSLS (aImages);
  for (int index = 1; aItDMSLS.More(); aItDMSLS.Next(), ++index) {
    // some key shape
    //const TopoDS_Shape& aSkey = aItDMSLS.Key();

    // list of shapes of the argument that can be glued
    const TopTools_ListOfShape& aLSD = aItDMSLS.Value();

    //listShape.Append(aLSD.First());
    TopoDS_Shape aValue = aLSD.First();

    if (aValue.ShapeType() == theType) {
      listOnePerSet.Append(aValue);
    }
  }

  // for stable order of returned entities
  GEOMUtils::SortShapes(listOnePerSet, Standard_False);

  TopTools_ListIteratorOfListOfShape aListIt (listOnePerSet);
  for (; aListIt.More(); aListIt.Next())
  {
    TopoDS_Shape aValue = aListIt.Value();
    // find a shape to add aValue as a sub-shape
    anObj.Nullify();
    s = theShapes.begin();
    for ( int i = 0; i < shapes.Length(); ++i, ++s )
    {
      Handle(GEOM_Object) object = *s;
      if ( !anIndices[i] ) {
        anIndices[i] = new TopTools_IndexedMapOfShape;
        TopExp::MapShapes( object->GetValue(), *anIndices[i]);
      }
      if (int index = anIndices[i]->FindIndex( aValue )) {
        anArray = new TColStd_HArray1OfInteger(1,1);
        anArray->SetValue(1, index);
        anObj = GetEngine()->AddSubShape( object, anArray);
        break;
      }
    }
    if (!anObj.IsNull())
      aSeq->Append(anObj);
  }
  for ( size_t i = 0 ; i < anIndices.size(); ++i )
    delete anIndices[i];

  // Make a Python command
  if ( aSeq->Length() > 0)
  {
    Handle(GEOM_Function) aFunction = lastCreatedGO->GetLastFunction();
    GEOM::TPythonDump pd (aFunction, /*append=*/true);
    pd << aSeq
       << " = geompy." << (theType == TopAbs_FACE ? "GetGlueFaces" : "GetGlueEdges" )
       << "( " << theShapes << ", " << theTolerance << ")";
  }

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  MakeGlueEdgesByList
 */
//=============================================================================
Handle(GEOM_Object)
GEOMImpl_IShapesOperations::MakeGlueEdgesByList (std::list< Handle(GEOM_Object) >& theShapes,
                                                 const Standard_Real               theTolerance,
                                                 std::list<Handle(GEOM_Object)>&   theEdges)
{
  SetErrorCode(KO);

  Handle(TColStd_HSequenceOfTransient) objects = GEOM_Object::GetLastFunctions( theShapes );
  if ( objects.IsNull() || objects->IsEmpty() ) {
    SetErrorCode("NULL argument shape");
    return NULL;
  }
  Handle(TColStd_HSequenceOfTransient) anEdges = GEOM_Object::GetLastFunctions( theEdges );
  if ( anEdges.IsNull() ) {
    SetErrorCode("NULL argument shape for the shape construction");
    return NULL;
  }
  //Add a new Glued object
  Handle(GEOM_Object) aGlued = GetEngine()->AddObject(GetDocID(), GEOM_GLUED);

  //Add a new Glue function
  Handle(GEOM_Function) aFunction;
  aFunction = aGlued->AddFunction(GEOMImpl_GlueDriver::GetID(), GLUE_EDGES_BY_LIST);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_GlueDriver::GetID()) return NULL;

  GEOMImpl_IGlue aCI (aFunction);

  aCI.SetBase( objects );
  aCI.SetTolerance(theTolerance);
  aCI.SetKeepNonSolids(true);
  aCI.SetFaces(anEdges);

  //Compute the sub-shape value
  Standard_Boolean isWarning = Standard_False;
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed to glue edges");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    // to provide warning
    if (!aFunction->GetValue().IsNull()) {
      isWarning = Standard_True;
    } else {
      return NULL;
    }
  }

  //Make a Python command

  GEOM::TPythonDump pd (aFunction);
  pd << aGlued << " = geompy.MakeGlueEdgesByList("
     << theShapes << ", " << theTolerance << ", " << theEdges << " )";

  // to provide warning
  if (!isWarning) SetErrorCode(OK);
  return aGlued;
}

//=============================================================================
/*!
 *  GetExistingSubObjects
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
GEOMImpl_IShapesOperations::GetExistingSubObjects(Handle(GEOM_Object)    theShape,
                                                  const Standard_Boolean theGroupsOnly)
{
  // note: this method does not return fields

  Standard_Integer types = theGroupsOnly ? Groups : Groups|SubShapes;
  Handle(TColStd_HSequenceOfTransient) results = GetExistingSubObjects(theShape, types);

  if (results->Length() > 0) {
    //Make a Python command
    TCollection_AsciiString anAsciiList;
    for (int i = 1; i <= results->Length(); i++)
    {
      Handle(GEOM_BaseObject) obj = Handle(GEOM_BaseObject)::DownCast( results->Value(i));
      obj->GetEntryString();
      if ( i < results->Length() )
        anAsciiList += ",";
    }
    
    GEOM::TPythonDump pd (theShape->GetLastFunction(), /*append=*/true);
    pd << "[" << anAsciiList.ToCString();
    pd << "] = geompy.GetExistingSubObjects(";
    pd << theShape << ", " << (bool)theGroupsOnly << ")";
  }

  return results;
}

Handle(TColStd_HSequenceOfTransient)
GEOMImpl_IShapesOperations::GetExistingSubObjects(Handle(GEOM_Object)    theShape,
                                                  const Standard_Integer theTypes)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;

  Handle(GEOM_Function) aMainShape = theShape->GetLastFunction();
  if (aMainShape.IsNull()) return NULL;

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
  SetErrorCode(NOT_FOUND_ANY);

  if (!aMainShape->HasSubShapeReferences()) return aSeq;
  const TDataStd_ListOfExtendedString& aListEntries = aMainShape->GetSubShapeReferences();
  if (aListEntries.IsEmpty()) return aSeq;

  SetErrorCode(KO);

  TDataStd_ListIteratorOfListOfExtendedString anIt (aListEntries);
  for (; anIt.More(); anIt.Next()) {
    TCollection_ExtendedString anEntry = anIt.Value();
    Standard_Integer aStrLen = anEntry.LengthOfCString();
    char* anEntryStr = new char[aStrLen+1];
    anEntry.ToUTF8CString(anEntryStr);
    Handle(GEOM_BaseObject) anObj = GetEngine()->GetObject(GetDocID(), anEntryStr, false);
    if (!anObj.IsNull() ) {
      bool isGroup    = anObj->IsKind(STANDARD_TYPE(GEOM_Object)) && anObj->GetType() == GEOM_GROUP;
      bool isSubShape = anObj->IsKind(STANDARD_TYPE(GEOM_Object)) && anObj->GetType() != GEOM_GROUP;
      bool isField    = anObj->IsKind(STANDARD_TYPE(GEOM_Field));
      if (theTypes & Groups    && isGroup ||
          theTypes & SubShapes && isSubShape ||
          theTypes & Fields    && isField) {
        aSeq->Append(anObj);
      }
    }
    delete [] anEntryStr;
  }

  if (aSeq->Length() == 0) {
    SetErrorCode(NOT_FOUND_ANY);
    return aSeq;
  }

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  MakeExplode
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::MakeExplode
                                          (Handle(GEOM_Object)    theShape,
                                           const Standard_Integer theShapeType,
                                           const Standard_Boolean isSorted,
                                           const ExplodeType      theExplodeType)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) return NULL;

  Handle(GEOM_Function) aMainShape = theShape->GetLastFunction();

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
  Handle(GEOM_Object) anObj;
  TopTools_MapOfShape mapShape;
  TopTools_ListOfShape listShape;

  if (aShape.ShapeType() == TopAbs_COMPOUND &&
      (theShapeType == TopAbs_SHAPE || theShapeType == TopAbs_FLAT || theShapeType == TopAbs_COMPOUND))
  {
    TopoDS_Iterator It (aShape, Standard_True, Standard_True);
    for (; It.More(); It.Next()) {
      TopoDS_Shape SS = It.Value();
      if (mapShape.Add(SS)) {
	if (theShapeType == TopAbs_FLAT) {
          AddFlatSubShapes(SS, listShape, mapShape);
	}
        else if (theShapeType == TopAbs_SHAPE || theShapeType == SS.ShapeType()) {
	  listShape.Append(SS);
        }
        // VSR: for EXPLODE_NEW_INCLUDE_MAIN and EXPLODE_OLD_INCLUDE_MAIN:
        // it seems it is necessary to add top-level shape if theShapeType == TopAbs_COMPOUND
      }
    }
  }
  else if (theExplodeType != EXPLODE_NEW_EXCLUDE_MAIN || aShape.ShapeType() != theShapeType) // issue 0021079
  {
    TopExp_Explorer exp (aShape, TopAbs_ShapeEnum(theShapeType));
    for (; exp.More(); exp.Next())
      if (mapShape.Add(exp.Current()))
        listShape.Append(exp.Current());
  }

  if (listShape.IsEmpty()){
    //SetErrorCode("The given shape has no sub-shapes of the requested type");
    SetErrorCode(NOT_FOUND_ANY); // NPAL18017
    return aSeq;
  }

  if (isSorted) {
    bool isOldSorting = false;
    if (theExplodeType == EXPLODE_OLD_INCLUDE_MAIN)
      isOldSorting = true;
    GEOMUtils::SortShapes(listShape, isOldSorting);
  }

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);
  Handle(TColStd_HArray1OfInteger) anArray;

  TopTools_ListIteratorOfListOfShape itSub (listShape);
  TCollection_AsciiString anAsciiList, anEntry;
  for (int index = 1; itSub.More(); itSub.Next(), ++index)
  {
    TopoDS_Shape aValue = itSub.Value();
    anArray = new TColStd_HArray1OfInteger(1,1);
    anArray->SetValue(1, anIndices.FindIndex(aValue));

    //anObj = GetEngine()->AddSubShape(theShape, anArray);
    {
      anObj = GetEngine()->AddObject(GetDocID(), GEOM_SUBSHAPE);
      Handle(GEOM_Function) aFunction = anObj->AddFunction(GEOM_Object::GetSubShapeID(), 1);
      if (aFunction.IsNull()) return aSeq;

      GEOM_ISubShape aSSI (aFunction);
      aSSI.SetMainShape(aMainShape);
      aSSI.SetIndices(anArray);

      // Set function value directly, as we know it.
      // Usage of Solver here would lead to significant loss of time,
      // because GEOM_SubShapeDriver will build TopTools_IndexedMapOfShape
      // on the main shape for each being calculated sub-shape separately.
      aFunction->SetValue(aValue);

      // Put this subshape in the list of sub-shapes of theMainShape
      aMainShape->AddSubShapeReference(aFunction);
    }
    if (!anObj.IsNull()) {
          aSeq->Append(anObj);

          // for python command
          TDF_Tool::Entry(anObj->GetEntry(), anEntry);
          anAsciiList += anEntry;
          anAsciiList += ",";
        }
      }

  //Make a Python command
  anAsciiList.Trunc(anAsciiList.Length() - 1);

  GEOM::TPythonDump pd (aMainShape, /*append=*/true);
  pd << "[" << anAsciiList.ToCString() << "] = geompy.";
  switch (theExplodeType) {
  case EXPLODE_NEW_EXCLUDE_MAIN:
    pd << "ExtractShapes(" << theShape << ", "
       << TopAbs_ShapeEnum(theShapeType) << ", " << (isSorted ? "True" : "False") << ")";
    break;
  case EXPLODE_NEW_INCLUDE_MAIN:
    pd << "SubShapeAll" << (isSorted ? "SortedCentres(" : "(")
       << theShape << ", " << TopAbs_ShapeEnum(theShapeType) << ")";
    break;
  case EXPLODE_OLD_INCLUDE_MAIN:
    pd << "SubShapeAll" << (isSorted ? "Sorted(" : "(")
       << theShape << ", " << TopAbs_ShapeEnum(theShapeType) << ")";
    break;
  default: ;
  }
  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  SubShapeAllIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::SubShapeAllIDs
                                          (Handle(GEOM_Object)    theShape,
                                           const Standard_Integer theShapeType,
                                           const Standard_Boolean isSorted,
                                           const ExplodeType      theExplodeType)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) return NULL;

  Handle(TColStd_HSequenceOfInteger) aSeq = new TColStd_HSequenceOfInteger;
  TopTools_MapOfShape mapShape;
  TopTools_ListOfShape listShape;

  if (aShape.ShapeType() == TopAbs_COMPOUND &&
      (theShapeType == TopAbs_SHAPE || theShapeType == TopAbs_FLAT || theShapeType == TopAbs_COMPOUND))
  {
    TopoDS_Iterator It (aShape, Standard_True, Standard_True);
    for (; It.More(); It.Next()) {
      TopoDS_Shape SS = It.Value();
      if (mapShape.Add(SS)) {
	if (theShapeType == TopAbs_FLAT) {
          AddFlatSubShapes(SS, listShape, mapShape);
	}
        else if (theShapeType == TopAbs_SHAPE || theShapeType == SS.ShapeType()) {
          listShape.Append(SS);
        }
      }
    }
  }
  else if (theExplodeType != EXPLODE_NEW_EXCLUDE_MAIN || aShape.ShapeType() != theShapeType) // issue 0021079
  {
    TopExp_Explorer exp (aShape, TopAbs_ShapeEnum(theShapeType));
    for (; exp.More(); exp.Next())
      if (mapShape.Add(exp.Current()))
        listShape.Append(exp.Current());
  }

  if (listShape.IsEmpty()) {
    //SetErrorCode("The given shape has no sub-shapes of the requested type");
    SetErrorCode(NOT_FOUND_ANY); // NPAL18017
    return aSeq;
  }

  if (isSorted) {
    bool isOldSorting = false;
    if (theExplodeType == EXPLODE_OLD_INCLUDE_MAIN)
      isOldSorting = true;
    GEOMUtils::SortShapes(listShape, isOldSorting);
  }

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);
  Handle(TColStd_HArray1OfInteger) anArray;

  TopTools_ListIteratorOfListOfShape itSub (listShape);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    TopoDS_Shape aValue = itSub.Value();
    aSeq->Append(anIndices.FindIndex(aValue));
  }

  Handle(GEOM_Function) aFunction = theShape->GetLastFunction();

  //Make a Python command
  GEOM::TPythonDump pd (aFunction, /*append=*/true);
  pd << "listSubShapeIDs = geompy.SubShapeAll";
  switch (theExplodeType) {
  case EXPLODE_NEW_EXCLUDE_MAIN:
    break;
  case EXPLODE_NEW_INCLUDE_MAIN:
    pd << (isSorted ? "SortedCentresIDs(" : "IDs(")
       << theShape << ", " << TopAbs_ShapeEnum(theShapeType) << ")";
    break;
  case EXPLODE_OLD_INCLUDE_MAIN:
    pd << (isSorted ? "SortedIDs(" : "IDs(")
       << theShape << ", " << TopAbs_ShapeEnum(theShapeType) << ")";
    break;
  default: ;
  }

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetSubShape
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::GetSubShape
                                          (Handle(GEOM_Object)    theMainShape,
                                           const Standard_Integer theID)
{
  SetErrorCode(KO);

  if (theMainShape.IsNull()) return NULL;

  Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger(1,1);
  anArray->SetValue(1, theID);
  Handle(GEOM_Object) anObj = GetEngine()->AddSubShape(theMainShape, anArray,true);
  if (anObj.IsNull()) {
    SetErrorCode("Can not get a sub-shape with the given ID");
    return NULL;
  }

  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  //Make a Python command
  GEOM::TPythonDump(aFunction) << anObj << " = geompy.GetSubShape("
                               << theMainShape << ", [" << theID << "])";

  SetErrorCode(OK);
  return anObj;
}

//=============================================================================
/*!
 *  MakeSubShapes
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::MakeSubShapes
                                (Handle(GEOM_Object)              theMainShape,
                                 Handle(TColStd_HArray1OfInteger) theIndices)
{
  SetErrorCode(KO);

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;

  if (!theIndices->Length()) {
    SetErrorCode(NOT_FOUND_ANY);
    return aSeq;
  }

  if (theMainShape.IsNull()) return NULL;
  TopoDS_Shape aShape = theMainShape->GetValue();
  if (aShape.IsNull()) return NULL;

  Handle(GEOM_Function) aMainShape = theMainShape->GetLastFunction();

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);

  Handle(TColStd_HArray1OfInteger) anArray;
  Handle(GEOM_Object) anObj;

  TCollection_AsciiString anAsciiList, anEntry;
  Standard_Integer i, low = theIndices->Lower(), up = theIndices->Upper();
  for (i = low; i <= up; i++) {
    int id = theIndices->Value(i);
    if (1 <= id && id <= anIndices.Extent()) {
      TopoDS_Shape aValue = anIndices.FindKey(id);
      anArray = new TColStd_HArray1OfInteger(1,1);
      anArray->SetValue(1, id);

      anObj = GetEngine()->AddObject(GetDocID(), GEOM_SUBSHAPE);
      if (!anObj.IsNull()) {
        Handle(GEOM_Function) aFunction = anObj->AddFunction(GEOM_Object::GetSubShapeID(), 1);
        if (aFunction.IsNull()) return aSeq;

        GEOM_ISubShape aSSI (aFunction);
        aSSI.SetMainShape(aMainShape);
        aSSI.SetIndices(anArray);

        // Set function value directly, as we know it.
        // Usage of Solver here would lead to significant loss of time,
        // because GEOM_SubShapeDriver will build TopTools_IndexedMapOfShape
        // on the main shape for each being calculated sub-shape separately.
        aFunction->SetValue(aValue);

        // Put this sub-shape in the list of sub-shapes of theMainShape
        aMainShape->AddSubShapeReference(aFunction);

        aSeq->Append(anObj);

        // for python command
        TDF_Tool::Entry(anObj->GetEntry(), anEntry);
        anAsciiList += anEntry;
        anAsciiList += ",";
      }
    }
  }

  //Make a Python command
  anAsciiList.Trunc(anAsciiList.Length() - 1);

  GEOM::TPythonDump pd (aMainShape, /*append=*/true);
  pd << "[" << anAsciiList.ToCString() << "] = geompy.SubShapes("
     << theMainShape << ", [" ;
  for (i = low; i <= up - 1; i++) {
    pd << theIndices->Value(i) << ", ";
  }
  pd << theIndices->Value(up) << "])";

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  GetSubShapeIndex
 */
//=============================================================================
Standard_Integer GEOMImpl_IShapesOperations::GetSubShapeIndex (Handle(GEOM_Object) theMainShape,
                                                               Handle(GEOM_Object) theSubShape)
{
  SetErrorCode(KO);

  TopoDS_Shape aMainShape = theMainShape->GetValue();
  TopoDS_Shape aSubShape = theSubShape->GetValue();

  if (aMainShape.IsNull() || aSubShape.IsNull()) return -1;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aMainShape, anIndices);
//   if (anIndices.Contains(aSubShape)) {
//     SetErrorCode(OK);
//     return anIndices.FindIndex(aSubShape);
//   }
  int id = anIndices.FindIndex(aSubShape);
  if (id > 0)
  {
    SetErrorCode(OK);
    return id;
  }
  return -1;
}



//=============================================================================
/*!
 *  GetSubShapeIndices
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetSubShapesIndices (Handle(GEOM_Object) theMainShape,
                                                                                    std::list<Handle(GEOM_Object)> theSubShapes)
{
  MESSAGE("GEOMImpl_IShapesOperations::GetSubShapesIndices")
  SetErrorCode(KO);
  
  Handle(TColStd_HSequenceOfInteger) aSeq = new TColStd_HSequenceOfInteger;
  
  TopoDS_Shape aMainShape = theMainShape->GetValue();
  if (aMainShape.IsNull())
  {
    MESSAGE("NULL main shape")
    return NULL;
  }
  
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aMainShape, anIndices);
  
  std::list<Handle(GEOM_Object)>::iterator it;
  for (it=theSubShapes.begin(); it != theSubShapes.end(); ++it)
  {
    TopoDS_Shape aSubShape = (*it)->GetValue(); 
    if (aSubShape.IsNull())
    {
      MESSAGE("NULL subshape")
      return NULL;
    }
    int id = anIndices.FindIndex(aSubShape);
    aSeq->Append(id);
  }  
  
  SetErrorCode(OK);
  return aSeq;
}


//=============================================================================
/*!
 *  GetTopologyIndex
 */
//=============================================================================
Standard_Integer GEOMImpl_IShapesOperations::GetTopologyIndex (Handle(GEOM_Object) theMainShape,
                                                               Handle(GEOM_Object) theSubShape)
{
  SetErrorCode(OK);

  TopoDS_Shape aMainShape = theMainShape->GetValue();
  TopoDS_Shape aSubShape = theSubShape->GetValue();

  if (aMainShape.IsNull() || aSubShape.IsNull()) {
    SetErrorCode("Null argument shape given");
    return -1;
  }

  int index = 1;
  if (aSubShape.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator it;
    TopTools_ListOfShape CL;
    CL.Append(aMainShape);
    TopTools_ListIteratorOfListOfShape itC;
    for (itC.Initialize(CL); itC.More(); itC.Next()) {
      for (it.Initialize(itC.Value()); it.More(); it.Next()) {
        if (it.Value().ShapeType() == TopAbs_COMPOUND) {
          if (it.Value().IsSame(aSubShape))
            return index;
          else
            index++;
          CL.Append(it.Value());
        }
      }
    }
  } else {
    TopExp_Explorer anExp (aMainShape, aSubShape.ShapeType());
    TopTools_MapOfShape M;
    for (; anExp.More(); anExp.Next()) {
      if (M.Add(anExp.Current())) {
        if (anExp.Current().IsSame(aSubShape))
          return index;
        index++;
      }
    }
  }

  SetErrorCode("The sub-shape does not belong to the main shape");
  return -1;
}

//=============================================================================
/*!
 *  GetShapeTypeString
 */
//=============================================================================
TCollection_AsciiString GEOMImpl_IShapesOperations::GetShapeTypeString (Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);

  TCollection_AsciiString aTypeName ("Null Shape");

  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull())
    return aTypeName;

  switch (aShape.ShapeType() )
  {
  case TopAbs_COMPOUND:
    aTypeName = "Compound";
    break;
  case  TopAbs_COMPSOLID:
    aTypeName = "Compound Solid";
    break;
  case TopAbs_SOLID:
    aTypeName = "Solid";
    break;
  case TopAbs_SHELL:
    aTypeName = "Shell";
    break;
  case TopAbs_FACE:
    {
      BRepAdaptor_Surface surf (TopoDS::Face(aShape));
      if (surf.GetType() == GeomAbs_Plane)
        aTypeName = "Plane";
      else if (surf.GetType() == GeomAbs_Cylinder)
        aTypeName = "Cylindrical Face";
      else if (surf.GetType() == GeomAbs_Sphere)
        aTypeName = "Spherical Face";
      else if (surf.GetType() == GeomAbs_Torus)
        aTypeName = "Toroidal Face";
      else if (surf.GetType() == GeomAbs_Cone)
        aTypeName = "Conical Face";
      else
        aTypeName = "GEOM::FACE";
    }
    break;
  case TopAbs_WIRE:
    aTypeName = "Wire";
    break;
  case TopAbs_EDGE:
    {
      BRepAdaptor_Curve curv (TopoDS::Edge(aShape));
      if (curv.GetType() == GeomAbs_Line) {
        if ((Abs(curv.FirstParameter()) >= 1E6) ||
            (Abs(curv.LastParameter()) >= 1E6))
          aTypeName = "Line";
        else
          aTypeName = "Edge";
      } else if (curv.GetType() == GeomAbs_Circle) {
        if (curv.IsClosed())
          aTypeName = "Circle";
        else
          aTypeName = "Arc";
      } else {
        aTypeName = "Edge";
      }
    }
    break;
  case TopAbs_VERTEX:
    aTypeName = "Vertex";
    break;
  case TopAbs_SHAPE:
    aTypeName = "Shape";
    break;
  default:
    aTypeName = "Shape of unknown type";
  }

  return aTypeName;
}

//=============================================================================
/*!
 *  IsSubShapeBelongsTo
 */
//=============================================================================
Standard_Boolean GEOMImpl_IShapesOperations::IsSubShapeBelongsTo( Handle(GEOM_Object) theSubObject,
                                                                  const Standard_Integer theSubObjectIndex,
                                                                  Handle(GEOM_Object) theObject,
                                                                  const Standard_Integer theObjectIndex)
{
  SetErrorCode(KO);

  if ( theObject.IsNull() || theSubObject.IsNull() )
    return false;

  TopoDS_Shape shape    = theObject->GetValue();
  TopoDS_Shape subShape = theSubObject->GetValue();

  if ( shape.IsNull() || subShape.IsNull() )
    return false;

  TopTools_IndexedMapOfShape anIndices;
  if ( theObjectIndex > 0 ) {
    TopExp::MapShapes( shape, anIndices );
    shape = anIndices.FindKey(theObjectIndex);
  }
  if ( theSubObjectIndex > 0 ) {
    TopExp::MapShapes( subShape, anIndices );
    subShape = anIndices.FindKey(theSubObjectIndex);
  }

  TopExp::MapShapes( shape, anIndices );

  const Standard_Boolean isBelongTo = anIndices.Contains(subShape);

  SetErrorCode(OK);

  return isBelongTo;
}

//=============================================================================
/*!
 *  NumberOfSubShapes
 */
//=============================================================================
Standard_Integer GEOMImpl_IShapesOperations::NumberOfSubShapes
                                          (Handle(GEOM_Object)    theShape,
                                           const Standard_Integer theShapeType)
{
  SetErrorCode(KO);
  Standard_Integer nbShapes = 0;

  if (theShape.IsNull()) return -1;
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) return -1;

  /*
  TopTools_MapOfShape mapShape;

  if (aShape.ShapeType() == TopAbs_COMPOUND &&
      (TopAbs_ShapeEnum(theShapeType) == TopAbs_SHAPE ||
       TopAbs_ShapeEnum(theShapeType) == TopAbs_COMPSOLID ||
       TopAbs_ShapeEnum(theShapeType) == TopAbs_COMPOUND)) {
    TopoDS_Iterator It (aShape, Standard_True, Standard_True);
    for (; It.More(); It.Next()) {
      if (mapShape.Add(It.Value())) {
        if (TopAbs_ShapeEnum(theShapeType) == TopAbs_SHAPE ||
            TopAbs_ShapeEnum(theShapeType) == It.Value().ShapeType()) {
          nbShapes++;
        }
      }
    }
  } else {
    TopExp_Explorer exp (aShape, TopAbs_ShapeEnum(theShapeType));
    for (; exp.More(); exp.Next())
      if (mapShape.Add(exp.Current()))
        nbShapes++;
  }
  */

  try {
    if (theShapeType == TopAbs_FLAT) {
      TopTools_MapOfShape aMapOfShape;
      TopTools_ListOfShape aListOfShape;
      AddFlatSubShapes(aShape, aListOfShape, aMapOfShape);
      nbShapes = aListOfShape.Extent();
    }
    else {
      OCC_CATCH_SIGNALS;
      int iType, nbTypes [TopAbs_SHAPE];
      for (iType = 0; iType < TopAbs_SHAPE; ++iType)
        nbTypes[iType] = 0;
      nbTypes[aShape.ShapeType()]++;
      
      TopTools_MapOfShape aMapOfShape;
      aMapOfShape.Add(aShape);
      TopTools_ListOfShape aListOfShape;
      aListOfShape.Append(aShape);
      
      TopTools_ListIteratorOfListOfShape itL (aListOfShape);
      for (; itL.More(); itL.Next()) {
        TopoDS_Iterator it (itL.Value());
        for (; it.More(); it.Next()) {
          TopoDS_Shape s = it.Value();
          if (aMapOfShape.Add(s)) {
            aListOfShape.Append(s);
            nbTypes[s.ShapeType()]++;
          }
        }
      }
      
      if (TopAbs_ShapeEnum(theShapeType) == TopAbs_SHAPE)
        nbShapes = aMapOfShape.Extent();
      else
        nbShapes = nbTypes[theShapeType];
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return -1;
  }

  SetErrorCode(OK);
  return nbShapes;
}

//=============================================================================
/*!
 *  ReverseShape
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::ReverseShape(Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;

  /*
  //Add a new reversed object
  Handle(GEOM_Object) aReversed = GetEngine()->AddObject(GetDocID(), theShape->GetType());

  //Add a new Revese function
  Handle(GEOM_Function) aFunction;
  aFunction = aReversed->AddFunction(GEOMImpl_ShapeDriver::GetID(), REVERSE_ORIENTATION);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aSI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aSI.SetBase(aRefShape);

  //Compute the sub-shape value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed to reverse shape");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aReversed
    << " = geompy.ChangeOrientation(" << theShape << ")";

  SetErrorCode(OK);
  */

  Handle(GEOM_Object) aReversed;

  GEOM_Engine* anEngine = GetEngine();
  //GEOMImpl_Gen* aGen = dynamic_cast<GEOMImpl_Gen*>(anEngine);
  GEOMImpl_Gen* aGen = (GEOMImpl_Gen*)anEngine;

  if (aGen) {
    GEOMImpl_IHealingOperations* anIHealingOperations =
      aGen->GetIHealingOperations(GetDocID());
    aReversed = anIHealingOperations->ChangeOrientationCopy(theShape);
    SetErrorCode(anIHealingOperations->GetErrorCode());
  }

  return aReversed;
}

//=============================================================================
/*!
 *  GetFreeFacesIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetFreeFacesIDs
                                                 (Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) return NULL;

  Handle(TColStd_HSequenceOfInteger) aSeq = new TColStd_HSequenceOfInteger;

  TopTools_IndexedDataMapOfShapeListOfShape mapFaceBlocks;
  GEOMImpl_Block6Explorer::MapShapesAndAncestors
    (aShape, TopAbs_FACE, TopAbs_SOLID, mapFaceBlocks);

  Standard_Integer ind = 1, nbFaces = mapFaceBlocks.Extent();

  if (nbFaces == 0) {
    SetErrorCode("The given shape has no faces");
    return aSeq;
  }

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);

  Standard_Integer id;
  for (; ind <= nbFaces; ind++) {
    if (mapFaceBlocks.FindFromIndex(ind).Extent() != 2) {
      id = anIndices.FindIndex(mapFaceBlocks.FindKey(ind));
      aSeq->Append(id);
    }
  }

  //The explode doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = theShape->GetLastFunction();

  //Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listFreeFacesIDs = geompy.GetFreeFacesIDs(" << theShape << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=======================================================================
//function : GetSharedShapes
//purpose  :
//=======================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetSharedShapes
                                                (Handle(GEOM_Object)    theShape1,
                                                 Handle(GEOM_Object)    theShape2,
                                                 const Standard_Integer theShapeType)
{
  SetErrorCode(KO);

  if (theShape1.IsNull() || theShape2.IsNull()) return NULL;

  TopoDS_Shape aShape1 = theShape1->GetValue();
  TopoDS_Shape aShape2 = theShape2->GetValue();

  if (aShape1.IsNull() || aShape2.IsNull()) return NULL;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape1, anIndices);
  Handle(TColStd_HArray1OfInteger) anArray;

  TopTools_IndexedMapOfShape mapShape1;
  TopExp::MapShapes(aShape1, TopAbs_ShapeEnum(theShapeType), mapShape1);

  Handle(GEOM_Object) anObj;
  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
  TCollection_AsciiString anAsciiList, anEntry;

  TopTools_MapOfShape mapShape2;
  TopExp_Explorer exp (aShape2, TopAbs_ShapeEnum(theShapeType));
  for (; exp.More(); exp.Next()) {
    TopoDS_Shape aSS = exp.Current();
    if (mapShape2.Add(aSS) && mapShape1.Contains(aSS)) {
      anArray = new TColStd_HArray1OfInteger(1,1);
      anArray->SetValue(1, anIndices.FindIndex(aSS));
      anObj = GetEngine()->AddSubShape(theShape1, anArray);
      aSeq->Append(anObj);

      // for python command
      TDF_Tool::Entry(anObj->GetEntry(), anEntry);
      anAsciiList += anEntry;
      anAsciiList += ",";
    }
  }

  if (aSeq->IsEmpty()) {
    SetErrorCode(NOT_FOUND_ANY);
    return aSeq;
  }

  //Make a Python command
  anAsciiList.Trunc(anAsciiList.Length() - 1);

  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction) << "[" << anAsciiList.ToCString()
    << "] = geompy.GetSharedShapes(" << theShape1 << ", "
      << theShape2 << ", " << TopAbs_ShapeEnum(theShapeType) << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=======================================================================
//function : GetSharedShapes
//purpose  :
//
// NOTE on the implementation
// 
// 1) Resulting sub-shapes are published as a children of the 1st input shape
//    from theShapes list. Due to this reason only direct sub-shapes of the 1st
//    shape can be contained in the result of the operation (i.e. shares between
//    2nd/3rd, etc couples cannot be retrieved.
// 2) An exception from above case is when a single compound is specified as an
//    input. In this case we search shares between its top-level content, so we
//    are able to search shares between all possible couples of shapes.
// 3) Parameter theMultiShare controls what types of shares to search:
//    - True: get sub-shapes that are shared between ALL input shapes;
//    - False: get shares between couples of input sub-shapes (see points 1 and 2).
//
// Thus, we have the following cases:
// [1] theShapes = N shapes (N>1), theMultiShare = True
//     Result: sub-shapes that are shared by all theShapes
// [2] theShapes = N shapes (N>1), theMultiShare = False
//     Result: sub-shapes of 1st shape from theShapes that are shared with any shape
//     from theShapes
// [3] theShapes = 1 shape, theMultiShare = True
//     Result: sub-shapes that are shared by all top-level sub-objects of theShapes[0]
// [4] theShapes = 1 shape, theMultiShare = False
//     Result: sub-shapes of all possible couples of all top-level sub-objects of
//     theShapes[0].
//=======================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetSharedShapes
                                     (std::list<Handle(GEOM_Object)> & theShapes,
                                      const Standard_Integer           theShapeType,
                                      const bool                       theMultiShare)
{
  SetErrorCode(KO);

  int aLen = theShapes.size();
  if (aLen < 1) return NULL;

  std::list<Handle(GEOM_Object)>::iterator it = theShapes.begin();

  // main object is always first in the input list
  // it is the object from which sub-shapes indices are taken
  // and where results are published
  Handle(GEOM_Object) aMainObj = *it;
  Handle(GEOM_Function) aMainShape = aMainObj->GetLastFunction();

  // collect all shapes from the input list (including first one) for processing
  TopTools_SequenceOfShape shapeSeq;
  for (; it != theShapes.end(); it++) {
    Handle(GEOM_Function) aRefShape = (*it)->GetLastFunction();
    if (aRefShape.IsNull()) {
      SetErrorCode("NULL shape for GetSharedShapes");
      return NULL;
    }
    TopoDS_Shape aShape = aRefShape->GetValue();
    if (aShape.IsNull()) {
      SetErrorCode("NULL shape for GetSharedShapes");
      return NULL;
    }
    shapeSeq.Append( aShape );
  }

  // if only single shape is specified as input
  // collect all ites top-level sub-shapes for processing
  if ( shapeSeq.Length() == 1 )
  {
    TopoDS_Shape aShape = shapeSeq.First();
    shapeSeq.Clear();
    for ( TopoDS_Iterator it( aShape ); it.More(); it.Next() )
      shapeSeq.Append( it.Value() );
  }

  // map all sub-shapes in a main shape to their indices
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aMainShape->GetValue(), anIndices);
  TopTools_MapOfShape mapShape;

  // find shared shapes

  // here we will collect all shares
  TopTools_ListOfShape aShared;

  // number of iterations
  int nbIters  =  theMultiShare || theShapes.size() > 1 ? 1 : shapeSeq.Length()-1;
  // numShares factor to search (i.e. by what nb of shapes each found sub-shape should be shared)
  int nbShares =  theMultiShare ? shapeSeq.Length()-1 : 1;
    
  for ( int iter = 1; iter <= nbIters; iter++) {
    for ( int ind = iter+1; ind <= shapeSeq.Length(); ind++) {
      if ( ind-1+nbShares > shapeSeq.Length() ) break;
      TopoDS_Compound aCurrSelection;
      TopoDS_Shape aShape1 = shapeSeq.Value( iter );
      TopTools_IndexedMapOfShape mapSelected;
      TopExp::MapShapes(aShape1, TopAbs_ShapeEnum(theShapeType), mapSelected);
      for ( int s = 0; s < nbShares; s++ ) {
        BRep_Builder B;
        TopoDS_Compound aCompound;
        B.MakeCompound(aCompound);
        const TopoDS_Shape& aShape2 = shapeSeq.Value( ind+s );
        TopTools_MapOfShape mapShape2;
        TopExp_Explorer exp (aShape2, TopAbs_ShapeEnum(theShapeType));
        for (; exp.More(); exp.Next()) {
          const TopoDS_Shape& aSS = exp.Current();
          if (mapShape2.Add(aSS) && mapSelected.Contains(aSS)) {
            B.Add(aCompound, aSS);
          }
        }
        mapSelected.Clear();
        aCurrSelection = aCompound;
        TopExp::MapShapes(aCurrSelection, TopAbs_ShapeEnum(theShapeType), mapSelected);
      }
      TopoDS_Iterator itSel(aCurrSelection, Standard_True, Standard_True);
      for (; itSel.More(); itSel.Next()) {
        const TopoDS_Shape& aSS = itSel.Value();
        if (mapShape.Add(aSS) )
	  aShared.Append(aSS);
      }
    }
  }

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;

  if (aShared.IsEmpty()){
    SetErrorCode(NOT_FOUND_ANY);
    return aSeq;
  }

  // create GEOM_Object for each found shared shape (collected in aShared)
  TCollection_AsciiString anAsciiList;
  Handle(GEOM_Object) anObj;
  TopTools_ListIteratorOfListOfShape itSub (aShared);
  for (; itSub.More(); itSub.Next()) {
    TopoDS_Shape aValue = itSub.Value();
    Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger(1,1);
    anArray->SetValue(1, anIndices.FindIndex(aValue));
    anObj = GetEngine()->AddSubShape(aMainObj, anArray);
    aSeq->Append(anObj);

    // for python command
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(anObj->GetEntry(), anEntry);
    anAsciiList += anEntry;
    anAsciiList += ",";
  }

  // make a Python command
  anAsciiList.Trunc(anAsciiList.Length() - 1);

  GEOM::TPythonDump pd (anObj->GetLastFunction());
  pd << "[" << anAsciiList.ToCString()
     << "] = geompy.GetSharedShapesMulti(";

  if ( aLen > 1 )
    pd << "[";

  it = theShapes.begin();
  pd << (*it++);
  while (it != theShapes.end()) {
    pd << ", " << (*it++);
  }
  if ( aLen > 1 )
    pd << "]";

  pd << ", " << TopAbs_ShapeEnum(theShapeType) << ", " << theMultiShare << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *
 */
//=============================================================================
static GEOM::TPythonDump& operator<< (GEOM::TPythonDump&   theDump,
                                      const GEOMAlgo_State theState)
{
  switch (theState) {
  case GEOMAlgo_ST_IN:
    theDump << "GEOM.ST_IN";
    break;
  case GEOMAlgo_ST_OUT:
    theDump << "GEOM.ST_OUT";
    break;
  case GEOMAlgo_ST_ON:
    theDump << "GEOM.ST_ON";
    break;
  case GEOMAlgo_ST_ONIN:
    theDump << "GEOM.ST_ONIN";
    break;
  case GEOMAlgo_ST_ONOUT:
    theDump << "GEOM.ST_ONOUT";
    break;
  default:
    theDump << "GEOM.ST_UNKNOWN";
    break;
  }
  return theDump;
}

//=======================================================================
//function : checkTypeShapesOn
/*!
 * \brief Checks if theShapeType parameter of GetShapesOnXXX() is OK
 * \param theShapeType - the shape type to check
 * \retval bool  - result of the check
 */
//=======================================================================
bool GEOMImpl_IShapesOperations::checkTypeShapesOn(const Standard_Integer theShapeType)
{
  if (theShapeType != TopAbs_VERTEX &&
      theShapeType != TopAbs_EDGE &&
      theShapeType != TopAbs_FACE &&
      theShapeType != TopAbs_SOLID) {
    SetErrorCode("Only solids, vertices, edges or faces can be found by this method");
    return false;
  }
  return true;
}

//=======================================================================
//function : makePlane
  /*!
   * \brief Creates Geom_Plane
    * \param theAx1 - shape object defining plane parameters
    * \retval Handle(Geom_Surface) - resulting surface
   */
//=======================================================================
Handle(Geom_Surface) GEOMImpl_IShapesOperations::makePlane(const TopoDS_Shape& anAx1)
{
  if (anAx1.ShapeType() != TopAbs_EDGE) return NULL;
  TopoDS_Edge anEdge = TopoDS::Edge(anAx1);
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(anEdge, V1, V2, Standard_True);
  if (V1.IsNull() || V2.IsNull()) {
    SetErrorCode("Bad edge given for the plane normal vector");
    return NULL;
  }
  gp_Pnt aLoc = BRep_Tool::Pnt(V1);
  gp_Vec aVec (aLoc, BRep_Tool::Pnt(V2));
  if (aVec.Magnitude() < Precision::Confusion()) {
    SetErrorCode("Vector with null magnitude given");
    return NULL;
  }
  return new Geom_Plane(aLoc, aVec);
}

//=======================================================================
//function : makeCylinder
  /*!
   * \brief Creates Geom_CylindricalSurface
    * \param theAx1 - edge defining cylinder axis
    * \param theRadius - cylinder radius
    * \retval Handle(Geom_Surface) - resulting surface
   */
//=======================================================================
Handle(Geom_Surface) GEOMImpl_IShapesOperations::makeCylinder(const TopoDS_Shape& anAxis,
                                                              const Standard_Real theRadius)
{
  //Axis of the cylinder
  if (anAxis.ShapeType() != TopAbs_EDGE) {
    SetErrorCode("Not an edge given for the axis");
    return NULL;
  }
  TopoDS_Edge anEdge = TopoDS::Edge(anAxis);
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(anEdge, V1, V2, Standard_True);
  if (V1.IsNull() || V2.IsNull()) {
    SetErrorCode("Bad edge given for the axis");
    return NULL;
  }
  gp_Pnt aLoc = BRep_Tool::Pnt(V1);
  gp_Vec aVec (aLoc, BRep_Tool::Pnt(V2));
  if (aVec.Magnitude() < Precision::Confusion()) {
    SetErrorCode("Vector with null magnitude given");
    return NULL;
  }

  gp_Ax3 anAx3 (aLoc, aVec);
  return new Geom_CylindricalSurface(anAx3, theRadius);
}

//=======================================================================
//function : getShapesOnBoxIDs
  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
    * \param theBox - the box to check state of sub-shapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
//=======================================================================
Handle(TColStd_HSequenceOfInteger)
  GEOMImpl_IShapesOperations::getShapesOnBoxIDs(const Handle(GEOM_Object)& theBox,
                                                const Handle(GEOM_Object)& theShape,
                                                const Standard_Integer theShapeType,
                                                GEOMAlgo_State theState)
{
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs;

  TopoDS_Shape aBox = theBox->GetValue();
  TopoDS_Shape aShape = theShape->GetValue();

  // Check presence of triangulation, build if need
  if (theShapeType != TopAbs_VERTEX && !GEOMUtils::CheckTriangulation(aShape)) {
    SetErrorCode("Cannot build triangulation on the shape");
    return aSeqOfIDs;
  }

  // Call algo
  GEOMAlgo_FinderShapeOn2 aFinder;
  Standard_Real aTol = 0.0001; // default value

  Handle(GEOMAlgo_ClsfBox) aClsfBox = new GEOMAlgo_ClsfBox;
  aClsfBox->SetBox(aBox);

  aFinder.SetShape(aShape);
  aFinder.SetTolerance(aTol);
  aFinder.SetClsf(aClsfBox);
  aFinder.SetShapeType( (TopAbs_ShapeEnum)theShapeType );
  aFinder.SetState(theState);
  aFinder.Perform();

  // Interprete results
  Standard_Integer iErr = aFinder.ErrorStatus();
  // the detailed description of error codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iErr) {
    MESSAGE(" iErr : " << iErr);
    TCollection_AsciiString aMsg (" iErr : ");
    aMsg += TCollection_AsciiString(iErr);
    SetErrorCode(aMsg);
    return aSeqOfIDs;
  }
  Standard_Integer iWrn = aFinder.WarningStatus();
  // the detailed description of warning codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iWrn) {
    MESSAGE(" *** iWrn : " << iWrn);
  }

  const TopTools_ListOfShape& listSS = aFinder.Shapes(); // the result

  if (listSS.Extent() < 1) {
    //SetErrorCode("Not a single sub-shape of the requested type found on the given surface");
    SetErrorCode(NOT_FOUND_ANY); // NPAL18017
    return aSeqOfIDs;
  }

  // Fill sequence of object IDs
  aSeqOfIDs = new TColStd_HSequenceOfInteger;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);

  TopTools_ListIteratorOfListOfShape itSub (listSS);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    int id = anIndices.FindIndex(itSub.Value());
    aSeqOfIDs->Append(id);
  }

  return aSeqOfIDs;
}

//=======================================================================
//function : GetShapesOnBoxIDs
/*!
   * \brief Find sub-shapes complying with given status about surface
    * \param theBox - the box to check state of sub-shapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
 */
//=======================================================================
Handle(TColStd_HSequenceOfInteger)
    GEOMImpl_IShapesOperations::GetShapesOnBoxIDs(const Handle(GEOM_Object)& theBox,
                                                  const Handle(GEOM_Object)& theShape,
                                                  const Standard_Integer theShapeType,
                                                  GEOMAlgo_State theState)
{
  // Find sub-shapes ids
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs =
    getShapesOnBoxIDs (theBox, theShape, theShapeType, theState);
  if ( aSeqOfIDs.IsNull()  || aSeqOfIDs->Length() == 0 )
    return NULL;

  // The GetShapesOnBox() doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = GEOM::GetCreatedLast(theShape,theBox)->GetLastFunction();

  // Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnBoxIDs = geompy.GetShapesOnBoxIDs("
    << theBox << ", "
    << theShape << ", "
    << TopAbs_ShapeEnum(theShapeType) << ", "
    << theState << ")";

  SetErrorCode(OK);
  return aSeqOfIDs;
}

//=======================================================================
//function : GetShapesOnBox
/*!
   * \brief Find sub-shapes complying with given status about surface
    * \param theBox - the box to check state of sub-shapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfTransient) - found sub-shapes
 */
//=======================================================================
Handle(TColStd_HSequenceOfTransient)
    GEOMImpl_IShapesOperations::GetShapesOnBox(const Handle(GEOM_Object)& theBox,
                                               const Handle(GEOM_Object)&  theShape,
                                               const Standard_Integer theShapeType,
                                               GEOMAlgo_State theState)
{
  // Find sub-shapes ids
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs =
    getShapesOnBoxIDs (theBox, theShape, theShapeType, theState);
  if ( aSeqOfIDs.IsNull()  || aSeqOfIDs->Length() == 0 )
    return NULL;

  // Find objects by indices
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getObjectsShapesOn( theShape, aSeqOfIDs, anAsciiList );
  if ( aSeq.IsNull() || aSeq->IsEmpty() )
    return NULL;

  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction)
    << "[" << anAsciiList.ToCString() << "] = geompy.GetShapesOnBox("
    << theBox << ", "
    << theShape << ", "
    << TopAbs_ShapeEnum(theShapeType) << ", "
    << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=======================================================================
//function : getShapesOnShapeIDs
/*!
 * \brief Find IDs of sub-shapes complying with given status about surface
 * \param theCheckShape - the shape to check state of sub-shapes against
 * \param theShape - the shape to explore
 * \param theShapeType - type of sub-shape of theShape
 * \param theState - required state
 * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
 */
//=======================================================================
Handle(TColStd_HSequenceOfInteger)
  GEOMImpl_IShapesOperations::getShapesOnShapeIDs
                                 (const Handle(GEOM_Object)& theCheckShape,
                                  const Handle(GEOM_Object)& theShape,
                                  const Standard_Integer theShapeType,
                                  GEOMAlgo_State theState)
{
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs;

  TopoDS_Shape aCheckShape = theCheckShape->GetValue();
  TopoDS_Shape aShape = theShape->GetValue();
  TopTools_ListOfShape res;

  // Check presence of triangulation, build if need
  if (theShapeType != TopAbs_VERTEX && !GEOMUtils::CheckTriangulation(aShape)) {
    SetErrorCode("Cannot build triangulation on the shape");
    return aSeqOfIDs;
  }

  // Compute classification tolerance.
  TopTools_IndexedMapOfShape aMapVtx;
  Standard_Real              aTol = Precision::Confusion();

  TopExp::MapShapes(aShape, TopAbs_VERTEX, aMapVtx);

  Standard_Integer i;
  Standard_Integer aNbVtx = aMapVtx.Extent();

  for (i = 1; i <= aNbVtx; ++i) {
    const TopoDS_Vertex aVtx    = TopoDS::Vertex(aMapVtx.FindKey(i));
    const Standard_Real aVtxTol = BRep_Tool::Tolerance(aVtx);

    if (aTol < aVtxTol) {
      aTol = aVtxTol;
    }
  }

  // Bound the tolerance value.
  if (aTol > 0.0001) {
    aTol = 0.0001;
  }

  // Call algo
  GEOMAlgo_FinderShapeOn2 aFinder;

  Handle(GEOMAlgo_ClsfSolid) aClsfSolid = new GEOMAlgo_ClsfSolid;
  aClsfSolid->SetShape(aCheckShape);

  aFinder.SetShape(aShape);
  aFinder.SetTolerance(aTol);
  aFinder.SetClsf(aClsfSolid);
  aFinder.SetShapeType( (TopAbs_ShapeEnum)theShapeType );
  aFinder.SetState(theState);
  aFinder.Perform();

  // Interprete results
  Standard_Integer iErr = aFinder.ErrorStatus();
  // the detailed description of error codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iErr) {
    if (iErr == 41) {
      SetErrorCode("theCheckShape must be a solid");
    }
    else {
      MESSAGE(" iErr : " << iErr);
      TCollection_AsciiString aMsg (" iErr : ");
      aMsg += TCollection_AsciiString(iErr);
      SetErrorCode(aMsg);
    }
    return aSeqOfIDs;
  }
  Standard_Integer iWrn = aFinder.WarningStatus();
  // the detailed description of warning codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iWrn) {
    MESSAGE(" *** iWrn : " << iWrn);
  }

  const TopTools_ListOfShape& listSS = aFinder.Shapes(); // the result

  if (listSS.Extent() < 1) {
    //SetErrorCode("Not a single sub-shape of the requested type found on the given surface");
    SetErrorCode(NOT_FOUND_ANY); // NPAL18017
  }

  // Fill sequence of object IDs
  aSeqOfIDs = new TColStd_HSequenceOfInteger;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);

  TopTools_ListIteratorOfListOfShape itSub (listSS);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    int id = anIndices.FindIndex(itSub.Value());
    aSeqOfIDs->Append(id);
  }

  return aSeqOfIDs;
}

//=======================================================================
//function : GetShapesOnShapeIDs
/*!
 * \brief Find sub-shapes complying with given status about surface
 * \param theCheckShape - the shape to check state of sub-shapes against
 * \param theShape - the shape to explore
 * \param theShapeType - type of sub-shape of theShape
 * \param theState - required state
 * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
 */
//=======================================================================
Handle(TColStd_HSequenceOfInteger)
    GEOMImpl_IShapesOperations::GetShapesOnShapeIDs
                            (const Handle(GEOM_Object)& theCheckShape,
                             const Handle(GEOM_Object)& theShape,
                             const Standard_Integer theShapeType,
                             GEOMAlgo_State theState)
{
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs =
    getShapesOnShapeIDs (theCheckShape, theShape, theShapeType, theState);

  if ( aSeqOfIDs.IsNull()  || aSeqOfIDs->Length() == 0 )
    return NULL;

  // The GetShapesOnShape() doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction =
    GEOM::GetCreatedLast(theShape,theCheckShape)->GetLastFunction();

  // Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnBoxIDs = geompy.GetShapesOnShapeIDs("
    << theCheckShape << ", "
    << theShape << ", "
    << TopAbs_ShapeEnum(theShapeType) << ", "
    << theState << ")";

  SetErrorCode(OK);
  return aSeqOfIDs;
}

//=======================================================================
//function : GetShapesOnShape
/*!
 * \brief Find sub-shapes complying with given status about surface
 * \param theCheckShape - the shape to check state of sub-shapes against
 * \param theShape - the shape to explore
 * \param theShapeType - type of sub-shape of theShape
 * \param theState - required state
 * \retval Handle(TColStd_HSequenceOfTransient) - found sub-shapes
 */
//=======================================================================
Handle(TColStd_HSequenceOfTransient)
  GEOMImpl_IShapesOperations::GetShapesOnShape
                             (const Handle(GEOM_Object)& theCheckShape,
                              const Handle(GEOM_Object)&  theShape,
                              const Standard_Integer theShapeType,
                              GEOMAlgo_State theState)
{
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs =
    getShapesOnShapeIDs (theCheckShape, theShape, theShapeType, theState);
  if ( aSeqOfIDs.IsNull()  || aSeqOfIDs->Length() == 0 )
    return NULL;

  // Find objects by indices
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getObjectsShapesOn( theShape, aSeqOfIDs, anAsciiList );

  if ( aSeq.IsNull() || aSeq->IsEmpty() )
    return NULL;

  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction)
    << "[" << anAsciiList.ToCString() << "] = geompy.GetShapesOnShape("
    << theCheckShape << ", "
    << theShape << ", "
    << TopAbs_ShapeEnum(theShapeType) << ", "
    << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=======================================================================
//function : GetShapesOnShapeAsCompound
//=======================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::GetShapesOnShapeAsCompound
                             (const Handle(GEOM_Object)& theCheckShape,
                              const Handle(GEOM_Object)&  theShape,
                              const Standard_Integer theShapeType,
                              GEOMAlgo_State theState)
{
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs =
    getShapesOnShapeIDs (theCheckShape, theShape, theShapeType, theState);

  if ( aSeqOfIDs.IsNull()  || aSeqOfIDs->Length() == 0 )
    return NULL;

  // Find objects by indices
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getObjectsShapesOn( theShape, aSeqOfIDs, anAsciiList );

  if ( aSeq.IsNull() || aSeq->IsEmpty() )
    return NULL;

  TopoDS_Compound aCompound;
  BRep_Builder B;
  B.MakeCompound(aCompound);
  int i = 1;
  for(; i<=aSeq->Length(); i++) {
    Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast(aSeq->Value(i));
    TopoDS_Shape aShape_i = anObj->GetValue();
    B.Add(aCompound,aShape_i);
  }

  //Add a new result object
  Handle(GEOM_Object) aRes = GetEngine()->AddObject(GetDocID(), GEOM_SHAPES_ON_SHAPE);
  Handle(GEOM_Function) aFunction =
    aRes->AddFunction(GEOMImpl_ShapeDriver::GetID(), SHAPES_ON_SHAPE);
  aFunction->SetValue(aCompound);

  aSeq->Clear();
  aSeq->Append( theCheckShape->GetLastFunction() );
  aSeq->Append( theShape->GetLastFunction() );

  GEOMImpl_IShapes aCI( aFunction );
  aCI.SetShapes( aSeq );
  aCI.SetSubShapeType( theShapeType );
  aCI.SetTolerance( theState );

  GEOM::TPythonDump(aFunction)
    << aRes << " = geompy.GetShapesOnShapeAsCompound("
    << theCheckShape << ", "
    << theShape << ", "
    << TopAbs_ShapeEnum(theShapeType) << ", "
    << theState << ")";

  SetErrorCode(OK);

  return aRes;
}

//=============================================================================
/*!
 *  GetSubShapeEdgeSorted
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
    GEOMImpl_IShapesOperations::GetSubShapeEdgeSorted
                          (const Handle(GEOM_Object) &theShape,
                           const Handle(GEOM_Object) &theStartPoint)
{
  // Get the sorted edges indices.
  Handle(TColStd_HSequenceOfInteger) aSortedIDs =
    getSubShapeEdgeSortedIDs(theShape, theStartPoint);

  // Get object by indices.
  TCollection_AsciiString              anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq =
    getObjectsShapesOn(theShape, aSortedIDs, anAsciiList);

  if (aSeq.IsNull() || aSeq->IsEmpty()) {
    SetErrorCode("Empty sequence of edges");
    return NULL;
  }

  // Make a Python command
  Handle(GEOM_Object)   anObj     =
    Handle(GEOM_Object)::DownCast(aSeq->Value(1));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction)
    << "[" << anAsciiList.ToCString() << "] = geompy.GetSubShapeEdgeSorted("
    << theShape << ", " << theStartPoint << ")";

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  GetSubShapesWithTolerance
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
    GEOMImpl_IShapesOperations::GetSubShapesWithTolerance
                     (const Handle(GEOM_Object)            &theShape,
                      const Standard_Integer                theShapeType,
                      const GEOMUtils::ComparisonCondition  theCondition,
                      const Standard_Real                   theTolerance)
{
  if (theShape.IsNull()) {
    SetErrorCode("NULL GEOM object");
    return NULL;
  }

  TopoDS_Shape aShape = theShape->GetValue();

  if (aShape.IsNull()) {
    SetErrorCode("NULL Shape");
    return NULL;
  }

  if (theShapeType != TopAbs_FACE && theShapeType != TopAbs_EDGE &&
      theShapeType != TopAbs_VERTEX && aShape.ShapeType() >= theShapeType) {
    SetErrorCode("Invalid shape type");
    return NULL;
  }

  TopTools_IndexedMapOfShape         anIndices;
  TopTools_MapOfShape                aMapFence;
  TopExp_Explorer                    anExp(aShape,
                                           (TopAbs_ShapeEnum) theShapeType);
  Handle(TColStd_HSequenceOfInteger) anIDs = new TColStd_HSequenceOfInteger;

  TopExp::MapShapes(aShape, anIndices);

  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &aSubShape = anExp.Current();

    if (aMapFence.Add(aSubShape)) {
      // Compute tolerance
      Standard_Real aTolerance = -1.;

      switch (aSubShape.ShapeType()) {
        case TopAbs_FACE:
          aTolerance = BRep_Tool::Tolerance(TopoDS::Face(aSubShape));
          break;
        case TopAbs_EDGE:
          aTolerance = BRep_Tool::Tolerance(TopoDS::Edge(aSubShape));
          break;
        case TopAbs_VERTEX:
          aTolerance = BRep_Tool::Tolerance(TopoDS::Vertex(aSubShape));
          break;
        default:
          break;
      }

      if (aTolerance < 0.) {
        continue;
      }

      // Compare the tolerance with reference value.
      if (GEOMUtils::IsFitCondition (theCondition, aTolerance, theTolerance)) {
        anIDs->Append(anIndices.FindIndex(aSubShape));
      }
    }
  }

  if (anIDs->IsEmpty()) {
    SetErrorCode("Empty sequence of sub-shapes");
    return NULL;
  }

  // Get objects by indices.
  TCollection_AsciiString              anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq =
    getObjectsShapesOn(theShape, anIDs, anAsciiList);

  if (aSeq.IsNull() || aSeq->IsEmpty()) {
    SetErrorCode("Empty sequence of edges");
    return NULL;
  }

  // Make a Python command
  Handle(GEOM_Object)   anObj     =
    Handle(GEOM_Object)::DownCast(aSeq->Value(1));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction)
    << "[" << anAsciiList.ToCString() << "] = geompy.GetSubShapesWithTolerance("
    << theShape << ", " << theShapeType << ", " << theCondition << ", "
    << theTolerance << ")";

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  MakeExtraction
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeExtraction
                     (const Handle(GEOM_Object)              &theShape,
                      const Handle(TColStd_HArray1OfInteger) &theSubShapeIDs,
                      std::list<ExtractionStat>              &theStats)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) {
    return NULL;
  }

  //Add a new Result object
  Handle(GEOM_Object) aResult =
              GetEngine()->AddObject(GetDocID(), GEOM_EXTRACTION);

  //Add a new Extraction function
  Handle(GEOM_Function) aFunction =
    aResult->AddFunction(GEOMImpl_ShapeDriver::GetID(), EXTRACTION);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) {
    return NULL;
  }

  Handle(GEOM_Function) aShape = theShape->GetLastFunction();

  if (aShape.IsNull()) {
    return NULL;
  }

  GEOMImpl_IExtract aCI (aFunction);

  aCI.SetShape(aShape);
  aCI.SetSubShapeIDs(theSubShapeIDs);

  //Compute the Edge value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");

      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());

    return NULL;
  }

  // Fill in statistics.
  theStats.clear();

  Handle(TColStd_HArray1OfInteger) aStatIDsArray[3] = 
    { aCI.GetRemovedIDs(), aCI.GetModifiedIDs(), aCI.GetAddedIDs() };
  int                              i;
  int                              j;

  for (j = 0; j < 3; ++j) {
    if (!aStatIDsArray[j].IsNull()) {
      const int      anUpperID = aStatIDsArray[j]->Upper();
      ExtractionStat aStat;

      for (i = aStatIDsArray[j]->Lower(); i <= anUpperID; ++i) {
        aStat.indices.push_back(aStatIDsArray[j]->Value(i));
      }

      aStat.type = (ExtractionStatType) j;
      theStats.push_back(aStat);
    }
  }

  //Make a Python command
  GEOM::TPythonDump pd(aFunction);

  pd << aResult  << " = geompy.MakeExtraction(" << theShape << ", [";

  if (!theSubShapeIDs.IsNull()) {
    const int aNbIDs = theSubShapeIDs->Upper();

    for (i = theSubShapeIDs->Lower(); i < aNbIDs; ++i) {
      pd << theSubShapeIDs->Value(i) << ", ";
    }

    // Dump the last value without a comma.
    pd << theSubShapeIDs->Value(i);
  }

  pd << "])";

  SetErrorCode(OK);

  return aResult;
}

//=======================================================================
//function : getShapesOnSurfaceIDs
  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
    * \param theSurface - the surface to check state of sub-shapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
//=======================================================================
Handle(TColStd_HSequenceOfInteger)
  GEOMImpl_IShapesOperations::getShapesOnSurfaceIDs(const Handle(Geom_Surface)& theSurface,
                                                    const TopoDS_Shape&         theShape,
                                                    TopAbs_ShapeEnum            theShapeType,
                                                    GEOMAlgo_State              theState)
{
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs;

  // Check presence of triangulation, build if need
  if (theShapeType != TopAbs_VERTEX &&
      !GEOMUtils::CheckTriangulation(theShape)) {
    SetErrorCode("Cannot build triangulation on the shape");
    return aSeqOfIDs;
  }

  // BEGIN: Mantis issue 0020961: Error on a pipe T-Shape
  // Compute tolerance
  Standard_Real T, VertMax = -RealLast();
  try {
    OCC_CATCH_SIGNALS;
    for (TopExp_Explorer ExV (theShape, TopAbs_VERTEX); ExV.More(); ExV.Next()) {
      TopoDS_Vertex Vertex = TopoDS::Vertex(ExV.Current());
      T = BRep_Tool::Tolerance(Vertex);
      if (T > VertMax)
        VertMax = T;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return aSeqOfIDs;
  }
  // END: Mantis issue 0020961

  // Call algo
  GEOMAlgo_FinderShapeOn2   aFinder;
  Handle(GEOMAlgo_ClsfSurf) aClsfSurf = new GEOMAlgo_ClsfSurf;
  Standard_Real             aTol      = VertMax; // Mantis issue 0020961

  aClsfSurf->SetSurface(theSurface);
  aFinder.SetShape(theShape);
  aFinder.SetTolerance(aTol);
  aFinder.SetClsf(aClsfSurf);
  aFinder.SetShapeType(theShapeType);
  aFinder.SetState(theState);

  // Sets the minimal number of inner points for the faces that do not have own
  // inner points at all (for e.g. rectangular planar faces have just 2 triangles).
  // Default value=3
  aFinder.SetNbPntsMin(3);
  // Sets the maximal number of inner points for edges or faces.
  // It is usefull for the cases when this number is very big (e.g =2000) to improve
  // the performance. If this value =0, all inner points will be taken into account.
  // Default value=0
  aFinder.SetNbPntsMax(100);

  aFinder.Perform();

  // Interprete results
  Standard_Integer iErr = aFinder.ErrorStatus();
  // the detailed description of error codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iErr) {
    MESSAGE(" iErr : " << iErr);
    TCollection_AsciiString aMsg (" iErr : ");
    aMsg += TCollection_AsciiString(iErr);
    SetErrorCode(aMsg);
    return aSeqOfIDs;
  }
  Standard_Integer iWrn = aFinder.WarningStatus();
  // the detailed description of warning codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iWrn) {
    MESSAGE(" *** iWrn : " << iWrn);
  }

  const TopTools_ListOfShape& listSS = aFinder.Shapes(); // the result

  if (listSS.Extent() < 1) {
    //SetErrorCode("Not a single sub-shape of the requested type found on the given surface");
    SetErrorCode(NOT_FOUND_ANY); // NPAL18017
    return aSeqOfIDs;
  }

  // Fill sequence of object IDs
  aSeqOfIDs = new TColStd_HSequenceOfInteger;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(theShape, anIndices);

  TopTools_ListIteratorOfListOfShape itSub (listSS);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    int id = anIndices.FindIndex(itSub.Value());
    aSeqOfIDs->Append(id);
  }

  return aSeqOfIDs;
}

//=======================================================================
//function : getObjectsShapesOn
/*!
 * \brief Find shape objects and their entries by their ids
 * \param theShapeIDs - incoming shape ids
 * \param theShapeEntries - outgoing entries like "entry1, entry2, ..."
 * \retval Handle(TColStd_HSequenceOfTransient) - found shape objects
 */
//=======================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::
 getObjectsShapesOn(const Handle(GEOM_Object)&                theShape,
                    const Handle(TColStd_HSequenceOfInteger)& theShapeIDs,
                    TCollection_AsciiString &                 theShapeEntries)
{
  Handle(TColStd_HSequenceOfTransient) aSeq;

  if ( !theShapeIDs.IsNull() && theShapeIDs->Length() > 0 )
  {
    aSeq = new TColStd_HSequenceOfTransient;
    Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger(1,1);
    TCollection_AsciiString anEntry;
    for ( int i = 1; i <= theShapeIDs->Length(); ++i )
    {
      anArray->SetValue(1, theShapeIDs->Value( i ));
      Handle(GEOM_Object) anObj = GetEngine()->AddSubShape(theShape, anArray);
      aSeq->Append( anObj );

      TDF_Tool::Entry(anObj->GetEntry(), anEntry);
      if ( i != 1 ) theShapeEntries += ",";
      theShapeEntries += anEntry;
    }
  }
  return aSeq;
}

//=============================================================================
/*!
 *  getSubShapeEdgeSortedIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger)
    GEOMImpl_IShapesOperations::getSubShapeEdgeSortedIDs
                               (const Handle(GEOM_Object) &theShape,
                                const Handle(GEOM_Object) &theStartPoint)
{
  Handle(TColStd_HSequenceOfInteger) aResult;

  if (theShape.IsNull() || theStartPoint.IsNull()) {
    SetErrorCode("NULL GEOM object");
    return aResult;
  }

  const TopoDS_Shape aShape      = theShape->GetValue();
  const TopoDS_Shape aStartPoint = theStartPoint->GetValue();

  if (aShape.IsNull() || aStartPoint.IsNull()) {
    SetErrorCode("NULL Shape");
    return aResult;
  }

  if (aStartPoint.ShapeType() != TopAbs_VERTEX) {
    SetErrorCode("Starting point is not a vertex");
    return aResult;
  }

  TopExp_Explorer      anExp(aShape, TopAbs_EDGE);
  TopTools_MapOfShape  aMapFence;
  TopTools_ListOfShape anEdges;

  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &anEdge = anExp.Current();

    if (aMapFence.Add(anEdge)) {
      anEdges.Append(anEdge);
    }
  }

  if (anEdges.IsEmpty()) {
    SetErrorCode("Shape doesn't contain edges");
    return aResult;
  }

  // Step 1: Sort edges
  GEOMUtils::SortShapes(anEdges, Standard_False);

  TopTools_ListIteratorOfListOfShape anIter(anEdges);
  TopoDS_Vertex                      aV[2];
  TopTools_DataMapOfShapeListOfShape aMapVE;

  // Step 2: Fill the map vertex - list of edges.
  for (; anIter.More(); anIter.Next()) {
    TopoDS_Edge anEdge = TopoDS::Edge(anIter.Value());

    TopExp::Vertices(anEdge, aV[0], aV[1]);

    const Standard_Integer aNbV = aV[0].IsSame(aV[1]) ? 1 : 2;
    Standard_Integer       i;

    for (i = 0; i < aNbV; ++i) {
      if (aV[i].IsNull() == Standard_False) {
        if (!aMapVE.IsBound(aV[i])) {
          // There is no this vertex in the map.
          aMapVE.Bind(aV[i], TopTools_ListOfShape());
        }

        // Add the edge to the list bound with the vertex aV[i].
        TopTools_ListOfShape &aLEdges = aMapVE.ChangeFind(aV[i]);

        aLEdges.Append(anEdge);
      }
    }
  }

  // Step 3: Find starting point in aMapVE.
  TopoDS_Vertex aStartVtx = TopoDS::Vertex(aStartPoint);

  if (!aMapVE.IsBound(aStartVtx)) {
    aStartVtx = getSameVertex(aShape, aStartVtx);

    if (aStartVtx.IsNull()) {
      SetErrorCode("Invalid Starting point");
      return aResult;
    }
  }

  TopTools_IndexedMapOfShape anIndices;
  TopTools_MapOfShape        aMapVFence;
  TopoDS_Shape               aCurVtx  = aStartVtx;
  TopoDS_Edge                aCurEdge =
    TopoDS::Edge(aMapVE.Find(aCurVtx).First());

  aResult = new TColStd_HSequenceOfInteger;
  TopExp::MapShapes(aShape, anIndices);

  // Step 4: Fill the list of sorted edges.
  while (aMapVFence.Add(aCurVtx)) {
    // Append the ID of the current edge to the list of sorted.
    aResult->Append(anIndices.FindIndex(aCurEdge));
    TopExp::Vertices(aCurEdge, aV[0], aV[1]);

    // Get the next vertex.
    if (aCurVtx.IsSame(aV[0])) {
      if (aCurVtx.IsSame(aV[1])) {
        // There is no next vertex.
        break;
      } else {
        aCurVtx = aV[1];
      }
    } else {
      aCurVtx = aV[0];
    }

    if (aCurVtx.IsNull()) {
      // There is no next vertex.
      break;
    }

    // Get the next edge.
    const TopTools_ListOfShape         &aLEdges = aMapVE.Find(aCurVtx);
    TopTools_ListIteratorOfListOfShape  anEIter(aLEdges);

    for (; anEIter.More(); anEIter.Next()) {
      const TopoDS_Shape &aLocalEdge = anEIter.Value();

      if (aLocalEdge.IsNull() == Standard_False) {
        if (!aCurEdge.IsSame(aLocalEdge)) {
          aCurEdge = TopoDS::Edge(aLocalEdge);
          break;
        }
      }
    }

    if (!anEIter.More()) {
      // There is no next edge.
      break;
    }
  }

  return aResult;
}

//=======================================================================
//function : getShapesOnSurface
/*!
   * \brief Find sub-shapes complying with given status about surface
    * \param theSurface - the surface to check state of sub-shapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theState - required state
    * \param theShapeEntries - outgoing entries like "entry1, entry2, ..."
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
 */
//=======================================================================
Handle(TColStd_HSequenceOfTransient)
    GEOMImpl_IShapesOperations::getShapesOnSurface(const Handle(Geom_Surface)& theSurface,
                                                   const Handle(GEOM_Object)&  theShape,
                                                   TopAbs_ShapeEnum            theShapeType,
                                                   GEOMAlgo_State              theState,
                                                   TCollection_AsciiString &   theShapeEntries)
{
  // Find sub-shapes ids
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs =
    getShapesOnSurfaceIDs (theSurface, theShape->GetValue(), theShapeType, theState);
  if ( aSeqOfIDs.IsNull()  || aSeqOfIDs->Length() == 0 )
    return NULL;

  return getObjectsShapesOn( theShape, aSeqOfIDs, theShapeEntries );
}

//=============================================================================
/*!
 *  GetShapesOnPlane
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetShapesOnPlane
                                        (const Handle(GEOM_Object)& theShape,
                                         const Standard_Integer     theShapeType,
                                         const Handle(GEOM_Object)& theAx1,
                                         const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAx1.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAx1  = theAx1->GetValue();

  if (aShape.IsNull() || anAx1.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( theShapeType ))
    return NULL;

  // Create plane
  Handle(Geom_Surface) aPlane = makePlane( anAx1 );
  if ( aPlane.IsNull() )
    return NULL;

  // Find objects
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getShapesOnSurface( aPlane, theShape, aShapeType, theState, anAsciiList );
  if ( aSeq.IsNull() || aSeq->Length() == 0 )
    return NULL;

  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction) << "[" << anAsciiList.ToCString()
    << "] = geompy.GetShapesOnPlane(" << theShape << ", "
      << aShapeType << ", " << theAx1 << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnPlaneWithLocation
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetShapesOnPlaneWithLocation
                                        (const Handle(GEOM_Object)& theShape,
                                         const Standard_Integer     theShapeType,
                                         const Handle(GEOM_Object)& theAx1,
                                         const Handle(GEOM_Object)& thePnt,
                                         const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAx1.IsNull() || thePnt.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAx1  = theAx1->GetValue();
  TopoDS_Shape anPnt = thePnt->GetValue();

  if (aShape.IsNull() || anAx1.IsNull() || anPnt.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( theShapeType ))
    return NULL;

  // Create plane
  if ( anAx1.ShapeType() != TopAbs_EDGE || anPnt.ShapeType() != TopAbs_VERTEX ) return NULL;
  TopoDS_Vertex V1, V2, V3;
  TopoDS_Edge anEdge = TopoDS::Edge(anAx1);
  TopExp::Vertices(anEdge, V1, V2, Standard_True);

  if (V1.IsNull() || V2.IsNull()) {
    SetErrorCode("Bad edge given for the plane normal vector");
    return NULL;
  }
  V3 = TopoDS::Vertex(anPnt);

  if(V3.IsNull()) {
    SetErrorCode("Bad vertex given for the plane location");
      return NULL;
  }
  gp_Pnt aLoc = BRep_Tool::Pnt(V3);
  gp_Vec aVec(BRep_Tool::Pnt(V1),BRep_Tool::Pnt(V2));

  if (aVec.Magnitude() < Precision::Confusion()) {
    SetErrorCode("Vector with null magnitude given");
    return NULL;
  }
  Handle(Geom_Surface) aPlane = new Geom_Plane(aLoc, aVec);

  if ( aPlane.IsNull() )
    return NULL;

  // Find objects
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getShapesOnSurface( aPlane, theShape, aShapeType, theState, anAsciiList );
  if ( aSeq.IsNull() || aSeq->Length() == 0 )
    return NULL;

  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction) << "[" << anAsciiList.ToCString()
    << "] = geompy.GetShapesOnPlaneWithLocation(" << theShape << ", "
    << aShapeType << ", " << theAx1 << ", "<< thePnt <<", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnCylinder
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetShapesOnCylinder
                                          (const Handle(GEOM_Object)& theShape,
                                           const Standard_Integer     theShapeType,
                                           const Handle(GEOM_Object)& theAxis,
                                           const Standard_Real        theRadius,
                                           const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAxis.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAxis = theAxis->GetValue();

  if (aShape.IsNull() || anAxis.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Create a cylinder surface
  Handle(Geom_Surface) aCylinder = makeCylinder( anAxis, theRadius );
  if ( aCylinder.IsNull() )
    return NULL;

  // Find objects
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getShapesOnSurface( aCylinder, theShape, aShapeType, theState, anAsciiList );
  if ( aSeq.IsNull() || aSeq->Length() == 0 )
    return NULL;

  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction) << "[" << anAsciiList.ToCString()
    << "] = geompy.GetShapesOnCylinder(" << theShape << ", " << aShapeType
      << ", " << theAxis << ", " << theRadius << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnCylinderWithLocation
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetShapesOnCylinderWithLocation
                                          (const Handle(GEOM_Object)& theShape,
                                           const Standard_Integer     theShapeType,
                                           const Handle(GEOM_Object)& theAxis,
                                           const Handle(GEOM_Object)& thePnt,
                                           const Standard_Real        theRadius,
                                           const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAxis.IsNull() || thePnt.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAxis = theAxis->GetValue();
  TopoDS_Shape aPnt   = thePnt->GetValue();

  if (aShape.IsNull() || anAxis.IsNull() || aPnt.IsNull()) return NULL;

  if (aPnt.ShapeType() != TopAbs_VERTEX )
  {
    SetErrorCode("Bottom location point must be vertex");
    return NULL;
  }

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Create a cylinder surface
  Handle(Geom_Surface) aCylinder = makeCylinder( anAxis, theRadius );
  if ( aCylinder.IsNull() )
    return NULL;

  // translate the surface
  Handle(Geom_CylindricalSurface) aCylSurface =
    Handle(Geom_CylindricalSurface)::DownCast( aCylinder );
  if ( aCylSurface.IsNull() )
  {
    SetErrorCode("Unexpected surface type instead of Geom_CylindricalSurface");
    return NULL;
  }
  gp_Pnt fromLoc = aCylSurface->Cylinder().Location();
  gp_Pnt toLoc   = BRep_Tool::Pnt( TopoDS::Vertex( aPnt ));
  aCylinder->Translate( fromLoc, toLoc );

  // Find objects
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getShapesOnSurface( aCylinder, theShape, aShapeType, theState, anAsciiList );
  if ( aSeq.IsNull() || aSeq->Length() == 0 )
    return NULL;

  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction)
    << "[" << anAsciiList.ToCString()
    << "] = geompy.GetShapesOnCylinderWithLocation(" << theShape << ", " << aShapeType << ", "
    << theAxis << ", " << thePnt << ", " << theRadius << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnSphere
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetShapesOnSphere
                                          (const Handle(GEOM_Object)& theShape,
                                           const Standard_Integer     theShapeType,
                                           const Handle(GEOM_Object)& theCenter,
                                           const Standard_Real        theRadius,
                                           const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theCenter.IsNull()) return NULL;

  TopoDS_Shape aShape  = theShape->GetValue();
  TopoDS_Shape aCenter = theCenter->GetValue();

  if (aShape.IsNull() || aCenter.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Center of the sphere
  if (aCenter.ShapeType() != TopAbs_VERTEX) return NULL;
  gp_Pnt aLoc = BRep_Tool::Pnt(TopoDS::Vertex(aCenter));

  gp_Ax3 anAx3 (aLoc, gp::DZ());
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(anAx3, theRadius);

  // Find objects
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getShapesOnSurface( aSphere, theShape, aShapeType, theState, anAsciiList );
  if ( aSeq.IsNull() || aSeq->Length() == 0 )
    return NULL;

  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction) << "[" << anAsciiList.ToCString()
    << "] = geompy.GetShapesOnSphere(" << theShape << ", " << aShapeType
      << ", " << theCenter << ", " << theRadius << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnPlaneIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetShapesOnPlaneIDs
                                        (const Handle(GEOM_Object)& theShape,
                                         const Standard_Integer     theShapeType,
                                         const Handle(GEOM_Object)& theAx1,
                                         const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAx1.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAx1  = theAx1->GetValue();

  if (aShape.IsNull() || anAx1.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Create plane
  Handle(Geom_Surface) aPlane = makePlane( anAx1 );
  if ( aPlane.IsNull() )
    return NULL;

  // Find object IDs
  Handle(TColStd_HSequenceOfInteger) aSeq;
  aSeq = getShapesOnSurfaceIDs( aPlane, aShape, aShapeType, theState );

  // The GetShapesOnPlaneIDs() doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = GEOM::GetCreatedLast(theShape,theAx1)->GetLastFunction();

  // Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnPlane = geompy.GetShapesOnPlaneIDs"
    << "(" << theShape << "," << aShapeType << "," << theAx1 << "," << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnPlaneWithLocationIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetShapesOnPlaneWithLocationIDs
                                        (const Handle(GEOM_Object)& theShape,
                                         const Standard_Integer     theShapeType,
                                         const Handle(GEOM_Object)& theAx1,
                                         const Handle(GEOM_Object)& thePnt,
                                         const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAx1.IsNull() || thePnt.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAx1  = theAx1->GetValue();
  TopoDS_Shape anPnt  = thePnt->GetValue();

  if (aShape.IsNull() || anAx1.IsNull() || anPnt.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Create plane
  if (anAx1.ShapeType() != TopAbs_EDGE || anPnt.ShapeType() != TopAbs_VERTEX) return NULL;
  TopoDS_Edge anEdge = TopoDS::Edge(anAx1);
  TopoDS_Vertex V1, V2, V3;
  TopExp::Vertices(anEdge, V1, V2, Standard_True);
  if (V1.IsNull() || V2.IsNull()) {
    SetErrorCode("Bad edge given for the plane normal vector");
    return NULL;
  }
  V3 = TopoDS::Vertex(anPnt);
  if(V3.IsNull()) {
    SetErrorCode("Bad vertex given for the plane location");
      return NULL;
  }
  gp_Pnt aLoc = BRep_Tool::Pnt(V3);
  gp_Vec aVec(BRep_Tool::Pnt(V1),BRep_Tool::Pnt(V2));
  if (aVec.Magnitude() < Precision::Confusion()) {
    SetErrorCode("Vector with null magnitude given");
    return NULL;
  }

  Handle(Geom_Surface) aPlane = new Geom_Plane(aLoc, aVec);
  if ( aPlane.IsNull() )
    return NULL;

  // Find object IDs
  Handle(TColStd_HSequenceOfInteger) aSeq;
  aSeq = getShapesOnSurfaceIDs( aPlane, aShape, aShapeType, theState );

  // The GetShapesOnPlaneIDs() doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = GEOM::GetCreatedLast(theShape,theAx1)->GetLastFunction();

  // Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnPlane = geompy.GetShapesOnPlaneWithLocationIDs"
    << "(" << theShape << ", " << aShapeType << ", " << theAx1 << ", "<< thePnt << ", "  << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnCylinderIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetShapesOnCylinderIDs
                                          (const Handle(GEOM_Object)& theShape,
                                           const Standard_Integer     theShapeType,
                                           const Handle(GEOM_Object)& theAxis,
                                           const Standard_Real        theRadius,
                                           const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAxis.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAxis = theAxis->GetValue();

  if (aShape.IsNull() || anAxis.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Create a cylinder surface
  Handle(Geom_Surface) aCylinder = makeCylinder( anAxis, theRadius );
  if ( aCylinder.IsNull() )
    return NULL;

  // Find object IDs
  Handle(TColStd_HSequenceOfInteger) aSeq;
  aSeq = getShapesOnSurfaceIDs( aCylinder, aShape, aShapeType, theState );

  // The GetShapesOnCylinder() doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = GEOM::GetCreatedLast(theShape,theAxis)->GetLastFunction();

  // Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnCylinder = geompy.GetShapesOnCylinderIDs"
    << "(" << theShape << ", " << aShapeType << ", " << theAxis << ", "
    << theRadius << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnCylinderWithLocationIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetShapesOnCylinderWithLocationIDs
                                          (const Handle(GEOM_Object)& theShape,
                                           const Standard_Integer     theShapeType,
                                           const Handle(GEOM_Object)& theAxis,
                                           const Handle(GEOM_Object)& thePnt,
                                           const Standard_Real        theRadius,
                                           const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAxis.IsNull() || thePnt.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAxis = theAxis->GetValue();
  TopoDS_Shape aPnt   = thePnt->GetValue();

  if (aShape.IsNull() || anAxis.IsNull() || aPnt.IsNull()) return NULL;

  if (aPnt.ShapeType() != TopAbs_VERTEX )
  {
    SetErrorCode("Bottom location point must be vertex");
    return NULL;
  }

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Create a cylinder surface
  Handle(Geom_Surface) aCylinder = makeCylinder( anAxis, theRadius );
  if ( aCylinder.IsNull() )
    return NULL;

  // translate the surface
  Handle(Geom_CylindricalSurface) aCylSurface =
    Handle(Geom_CylindricalSurface)::DownCast( aCylinder );
  if ( aCylSurface.IsNull() )
  {
    SetErrorCode("Unexpected surface type instead of Geom_CylindricalSurface");
    return NULL;
  }
  gp_Pnt fromLoc = aCylSurface->Cylinder().Location();
  gp_Pnt toLoc   = BRep_Tool::Pnt( TopoDS::Vertex( aPnt ));
  aCylinder->Translate( fromLoc, toLoc );

  // Find object IDs
  Handle(TColStd_HSequenceOfInteger) aSeq;
  aSeq = getShapesOnSurfaceIDs( aCylinder, aShape, aShapeType, theState );

  // The GetShapesOnCylinder() doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction =
    GEOM::GetCreatedLast(theShape, GEOM::GetCreatedLast(thePnt,theAxis))->GetLastFunction();

  // Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnCylinder = geompy.GetShapesOnCylinderWithLocationIDs"
    << "(" << theShape << ", " << aShapeType << ", " << theAxis << ", "
    << thePnt << ", " << theRadius << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnSphereIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetShapesOnSphereIDs
                                          (const Handle(GEOM_Object)& theShape,
                                           const Standard_Integer     theShapeType,
                                           const Handle(GEOM_Object)& theCenter,
                                           const Standard_Real        theRadius,
                                           const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theCenter.IsNull()) return NULL;

  TopoDS_Shape aShape  = theShape->GetValue();
  TopoDS_Shape aCenter = theCenter->GetValue();

  if (aShape.IsNull() || aCenter.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Center of the sphere
  if (aCenter.ShapeType() != TopAbs_VERTEX) return NULL;
  gp_Pnt aLoc = BRep_Tool::Pnt(TopoDS::Vertex(aCenter));

  gp_Ax3 anAx3 (aLoc, gp::DZ());
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(anAx3, theRadius);

  // Find object IDs
  Handle(TColStd_HSequenceOfInteger) aSeq;
  aSeq = getShapesOnSurfaceIDs( aSphere, aShape, aShapeType, theState );

  // The GetShapesOnSphere() doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = GEOM::GetCreatedLast(theShape,theCenter)->GetLastFunction();

  // Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnCylinder = geompy.GetShapesOnSphereIDs"
    << "(" << theShape << ", " << aShapeType << ", " << theCenter << ", "
    << theRadius << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=======================================================================
//function : getShapesOnQuadrangleIDs
  /*!
   * \brief Find IDs of sub-shapes complying with given status about quadrangle
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theTopLeftPoint - top left quadrangle corner
    * \param theTopRigthPoint - top right quadrangle corner
    * \param theBottomLeftPoint - bottom left quadrangle corner
    * \param theBottomRigthPoint - bottom right quadrangle corner
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
//=======================================================================
Handle(TColStd_HSequenceOfInteger)
  GEOMImpl_IShapesOperations::getShapesOnQuadrangleIDs (const Handle(GEOM_Object)& theShape,
                                                        const Standard_Integer     theShapeType,
                                                        const Handle(GEOM_Object)& theTopLeftPoint,
                                                        const Handle(GEOM_Object)& theTopRigthPoint,
                                                        const Handle(GEOM_Object)& theBottomLeftPoint,
                                                        const Handle(GEOM_Object)& theBottomRigthPoint,
                                                        const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if ( theShape.IsNull() ||
       theTopLeftPoint.IsNull() ||
       theTopRigthPoint.IsNull() ||
       theBottomLeftPoint.IsNull() ||
       theBottomRigthPoint.IsNull() )
    return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape aTL = theTopLeftPoint->GetValue();
  TopoDS_Shape aTR = theTopRigthPoint->GetValue();
  TopoDS_Shape aBL = theBottomLeftPoint->GetValue();
  TopoDS_Shape aBR = theBottomRigthPoint->GetValue();

  if (aShape.IsNull() ||
      aTL.IsNull() ||
      aTR.IsNull() ||
      aBL.IsNull() ||
      aBR.IsNull() ||
      aTL.ShapeType() != TopAbs_VERTEX ||
      aTR.ShapeType() != TopAbs_VERTEX ||
      aBL.ShapeType() != TopAbs_VERTEX ||
      aBR.ShapeType() != TopAbs_VERTEX )
    return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs;

  // Check presence of triangulation, build if need
  if (theShapeType != TopAbs_VERTEX && !GEOMUtils::CheckTriangulation(aShape)) {
    SetErrorCode("Cannot build triangulation on the shape");
    return aSeqOfIDs;
  }

  // Call algo
  gp_Pnt aPntTL = BRep_Tool::Pnt(TopoDS::Vertex(aTL));
  gp_Pnt aPntTR = BRep_Tool::Pnt(TopoDS::Vertex(aTR));
  gp_Pnt aPntBL = BRep_Tool::Pnt(TopoDS::Vertex(aBL));
  gp_Pnt aPntBR = BRep_Tool::Pnt(TopoDS::Vertex(aBR));

  GEOMAlgo_FinderShapeOn2  aFinder;
  Handle(GEOMAlgo_ClsfQuad) aClsfQuad = new GEOMAlgo_ClsfQuad;

  Standard_Real aTol = 0.0001; // default value

  aClsfQuad->SetCorners(aPntTL, aPntTR, aPntBL, aPntBR);
  aFinder.SetShape(aShape);
  aFinder.SetTolerance(aTol);
  aFinder.SetClsf(aClsfQuad);
  aFinder.SetShapeType(aShapeType);
  aFinder.SetState(theState);

  // Sets the minimal number of inner points for the faces that do not have own
  // inner points at all (for e.g. rectangular planar faces have just 2 triangles).
  // Default value=3
  aFinder.SetNbPntsMin(3);
  // Sets the maximal number of inner points for edges or faces.
  // It is usefull for the cases when this number is very big (e.g =2000) to improve
  // the performance. If this value =0, all inner points will be taken into account.
  // Default value=0
  aFinder.SetNbPntsMax(100);

  aFinder.Perform();

  // Interprete results
  Standard_Integer iErr = aFinder.ErrorStatus();
  // the detailed description of error codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iErr) {
    MESSAGE(" iErr : " << iErr);
    TCollection_AsciiString aMsg (" iErr : ");
    aMsg += TCollection_AsciiString(iErr);
    SetErrorCode(aMsg);
    return aSeqOfIDs;
  }
  Standard_Integer iWrn = aFinder.WarningStatus();
  // the detailed description of warning codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iWrn) {
    MESSAGE(" *** iWrn : " << iWrn);
  }

  const TopTools_ListOfShape& listSS = aFinder.Shapes(); // the result

  if (listSS.Extent() < 1) {
    //SetErrorCode("Not a single sub-shape of the requested type found on the given surface");
    SetErrorCode(NOT_FOUND_ANY); // NPAL18017
    return aSeqOfIDs;
  }

  // Fill sequence of object IDs
  aSeqOfIDs = new TColStd_HSequenceOfInteger;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);

  TopTools_ListIteratorOfListOfShape itSub (listSS);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    int id = anIndices.FindIndex(itSub.Value());
    aSeqOfIDs->Append(id);
  }
  return aSeqOfIDs;
}

//=======================================================================
//function : GetShapesOnQuadrangle
  /*!
   * \brief Find sub-shapes complying with given status about quadrangle
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theTopLeftPoint - top left quadrangle corner
    * \param theTopRigthPoint - top right quadrangle corner
    * \param theBottomLeftPoint - bottom left quadrangle corner
    * \param theBottomRigthPoint - bottom right quadrangle corner
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
//=======================================================================
Handle(TColStd_HSequenceOfTransient)
    GEOMImpl_IShapesOperations::GetShapesOnQuadrangle (const Handle(GEOM_Object)& theShape,
                                                       const Standard_Integer     theShapeType,
                                                       const Handle(GEOM_Object)& theTopLeftPoint,
                                                       const Handle(GEOM_Object)& theTopRigthPoint,
                                                       const Handle(GEOM_Object)& theBottomLeftPoint,
                                                       const Handle(GEOM_Object)& theBottomRigthPoint,
                                                       const GEOMAlgo_State       theState)
{
  // Find indices
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs =
    getShapesOnQuadrangleIDs( theShape,
                              theShapeType,
                              theTopLeftPoint,
                              theTopRigthPoint,
                              theBottomLeftPoint,
                              theBottomRigthPoint,
                              theState);
  if ( aSeqOfIDs.IsNull() || aSeqOfIDs->IsEmpty() )
    return NULL;

  // Find objects by indices
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getObjectsShapesOn( theShape, aSeqOfIDs, anAsciiList );
  if ( aSeq.IsNull() || aSeq->IsEmpty() )
    return NULL;

  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction)
    << "[" << anAsciiList.ToCString() << "] = geompy.GetShapesOnQuadrangle("
    << theShape << ", "
    << TopAbs_ShapeEnum(theShapeType) << ", "
    << theTopLeftPoint << ", "
    << theTopRigthPoint << ", "
    << theBottomLeftPoint << ", "
    << theBottomRigthPoint << ", "
    << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=======================================================================
//function : GetShapesOnQuadrangleIDs
  /*!
   * \brief Find IDs of sub-shapes complying with given status about quadrangle
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theTopLeftPoint - top left quadrangle corner
    * \param theTopRigthPoint - top right quadrangle corner
    * \param theBottomLeftPoint - bottom left quadrangle corner
    * \param theBottomRigthPoint - bottom right quadrangle corner
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
//=======================================================================
Handle(TColStd_HSequenceOfInteger)
  GEOMImpl_IShapesOperations::GetShapesOnQuadrangleIDs (const Handle(GEOM_Object)& theShape,
                                                        const Standard_Integer     theShapeType,
                                                        const Handle(GEOM_Object)& theTopLeftPoint,
                                                        const Handle(GEOM_Object)& theTopRigthPoint,
                                                        const Handle(GEOM_Object)& theBottomLeftPoint,
                                                        const Handle(GEOM_Object)& theBottomRigthPoint,
                                                        const GEOMAlgo_State       theState)
{
  // Find indices
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs =
    getShapesOnQuadrangleIDs( theShape,
                              theShapeType,
                              theTopLeftPoint,
                              theTopRigthPoint,
                              theBottomLeftPoint,
                              theBottomRigthPoint,
                              theState);
  if ( aSeqOfIDs.IsNull() || aSeqOfIDs->IsEmpty() )
    return NULL;

  // Make a Python command

  // The GetShapesOnCylinder() doesn't change object so no new function is required.
  Handle(GEOM_Object) lastObj = GEOM::GetCreatedLast(theShape,theTopLeftPoint);
  lastObj = GEOM::GetCreatedLast(lastObj,theTopRigthPoint);
  lastObj = GEOM::GetCreatedLast(lastObj,theBottomRigthPoint);
  lastObj = GEOM::GetCreatedLast(lastObj,theBottomLeftPoint);
  Handle(GEOM_Function) aFunction = lastObj->GetLastFunction();

  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnQuadrangle = geompy.GetShapesOnQuadrangleIDs("
    << theShape << ", "
    << TopAbs_ShapeEnum(theShapeType) << ", "
    << theTopLeftPoint << ", "
    << theTopRigthPoint << ", "
    << theBottomLeftPoint << ", "
    << theBottomRigthPoint << ", "
    << theState << ")";

  SetErrorCode(OK);
  return aSeqOfIDs;
}

//=============================================================================
/*!
 *  case GetInPlace:
 *  default:
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::GetInPlace (Handle(GEOM_Object) theShapeWhere,
                                                            Handle(GEOM_Object) theShapeWhat)
{
  SetErrorCode(KO);

  if (theShapeWhere.IsNull() || theShapeWhat.IsNull()) return NULL;

  TopoDS_Shape aWhere = theShapeWhere->GetValue();
  TopoDS_Shape aWhat  = theShapeWhat->GetValue();

  if (aWhere.IsNull() || aWhat.IsNull()) {
    SetErrorCode("Error: aWhere and aWhat TopoDS_Shape are Null.");
    return NULL;
  }

  // Searching for the sub-shapes inside the ShapeWhere shape
  GEOMAlgo_GetInPlace aGIP;

  if (!GEOMAlgo_GetInPlaceAPI::GetInPlace(aWhere, aWhat, aGIP)) {
    SetErrorCode("Error in GEOMAlgo_GetInPlace");
    return NULL;
  }

  // Add direct result.
  TopTools_ListOfShape        aLSA;
  const TopoDS_Shape         &aShapeResult = aGIP.Result();
  TopTools_MapOfShape         aMFence;
  TopTools_IndexedMapOfShape  aWhereIndices;
  Standard_Integer            aShapeType = -1;

  TopExp::MapShapes(aWhere, aWhereIndices);

  if (aShapeResult.IsNull() == Standard_False) {
    TopoDS_Iterator  anIt(aShapeResult);
    Standard_Boolean isFirst = Standard_True;

    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape &aPart = anIt.Value();

      if(aWhereIndices.Contains(aPart) && aMFence.Add(aPart)) {
        const TopAbs_ShapeEnum aType = aPart.ShapeType();

        if (aShapeType == -1) {
          // Initialization.
          aShapeType = aType;
        } else if (aShapeType != TopAbs_SHAPE && aShapeType != aType) {
          // Different types.
          aShapeType = TopAbs_SHAPE;
        }

        aLSA.Append(aPart);
      }
    }
  }

  if (aLSA.Extent() == 0) {
    SetErrorCode(NOT_FOUND_ANY); // Not found any Results
    return NULL;
  }

  Handle(TColStd_HArray1OfInteger) aModifiedArray = new TColStd_HArray1OfInteger (1, aLSA.Extent());
  TopTools_ListIteratorOfListOfShape anIterModif (aLSA);
  for (Standard_Integer imod = 1; anIterModif.More(); anIterModif.Next(), imod++) {
    aModifiedArray->SetValue(imod, aWhereIndices.FindIndex(anIterModif.Value()));
  }

  //Add a new object
  Handle(GEOM_Object) aResult = GetEngine()->AddSubShape(theShapeWhere, aModifiedArray);
  if (aResult.IsNull()) {
    SetErrorCode("Error in algorithm: result found, but cannot be returned.");
    return NULL;
  }

  const Standard_Boolean isSameType = (aShapeType != TopAbs_SHAPE);

  if ((aModifiedArray->Length() > 1 && isSameType) ||
      theShapeWhat->GetType() == GEOM_GROUP) {
    //Set a GROUP type
    aResult->SetType(GEOM_GROUP);

    //Set a sub-shape type
    TopoDS_Shape aFirstFound = aLSA.First();
    TopAbs_ShapeEnum aShapeType = aFirstFound.ShapeType();

    TDF_Label aFreeLabel = aResult->GetFreeLabel();
    TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)aShapeType);
  }

  //Make a Python command
  Handle(GEOM_Function) aFunction = aResult->GetFunction(1);

  GEOM::TPythonDump(aFunction) << aResult << " = geompy.GetInPlace("
    << theShapeWhere << ", " << theShapeWhat << ", True)";

  SetErrorCode(OK);
  return aResult;
}

//=============================================================================
/*!
 *  case GetInPlaceOld:
 *  default:
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::GetInPlaceOld
                            (Handle(GEOM_Object) theShapeWhere,
                             Handle(GEOM_Object) theShapeWhat)
{
  SetErrorCode(KO);

  if (theShapeWhere.IsNull() || theShapeWhat.IsNull()) return NULL;

  TopoDS_Shape           aWhere = theShapeWhere->GetValue();
  TopoDS_Shape           aWhat  = theShapeWhat->GetValue();
  TopTools_ListOfShape   aModifiedList;
  const Standard_Integer iErr   =
    GEOMAlgo_GetInPlaceAPI::GetInPlaceOld(aWhere, aWhat, aModifiedList);

  if (iErr) {
    switch (iErr) {
      case 1:
        SetErrorCode("Error: aWhere and aWhat TopoDS_Shape are Null.");
        break;
      case 2:
        SetErrorCode
          ("Error: An attempt to extract a shape of not supported type.");
        break;
      case 3:
        SetErrorCode(NOT_FOUND_ANY);
        break;
      default:
        SetErrorCode("Shape driver failed");
        break;
    }

    return NULL;
  }

  TopTools_IndexedMapOfShape aWhereIndices;
  TopExp::MapShapes(aWhere, aWhereIndices);

  Handle(TColStd_HArray1OfInteger)   aModifiedArray =
    new TColStd_HArray1OfInteger (1, aModifiedList.Extent());
  TopTools_ListIteratorOfListOfShape anIterModif (aModifiedList);
  Standard_Integer                   imod;
  Standard_Integer                   aShapeType = -1;

  for (imod = 1; anIterModif.More(); anIterModif.Next(), imod++) {
    const Standard_Integer anIndex =
      aWhereIndices.FindIndex(anIterModif.Value());
    const TopAbs_ShapeEnum aType   = anIterModif.Value().ShapeType();

    if (aShapeType == -1) {
      // Initialization.
      aShapeType = aType;
    } else if (aShapeType != TopAbs_SHAPE && aShapeType != aType) {
      // Different types.
      aShapeType = TopAbs_SHAPE;
    }

    aModifiedArray->SetValue(imod, anIndex);
  }

  //Add a new object
  Handle(GEOM_Object) aResult =
    GetEngine()->AddSubShape(theShapeWhere, aModifiedArray);

  if (aResult.IsNull()) {
    SetErrorCode("Error in algorithm: result found, but cannot be returned.");
    return NULL;
  }

  const Standard_Boolean isSameType = (aShapeType != TopAbs_SHAPE);

  if ((aModifiedArray->Length() > 1 && isSameType) ||
      theShapeWhat->GetType() == GEOM_GROUP) {
    //Set a GROUP type
    aResult->SetType(GEOM_GROUP);

    //Set a sub-shape type
    TopoDS_Shape aFirstFound = aWhereIndices.FindKey(aModifiedArray->Value(1));
    TopAbs_ShapeEnum aShapeType = aFirstFound.ShapeType();

    TDF_Label aFreeLabel = aResult->GetFreeLabel();
    TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)aShapeType);
  }

  //Make a Python command
  Handle(GEOM_Function) aFunction = aResult->GetFunction(1);

  GEOM::TPythonDump(aFunction) << aResult << " = geompy.GetInPlace("
    << theShapeWhere << ", " << theShapeWhat << ", False)";

  SetErrorCode(OK);

  return aResult;
}

//=======================================================================
//function : GetInPlaceByHistory
//purpose  :
//=======================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::GetInPlaceByHistory
                                          (Handle(GEOM_Object) theShapeWhere,
                                           Handle(GEOM_Object) theShapeWhat)
{
  SetErrorCode(KO);

  if (theShapeWhere.IsNull() || theShapeWhat.IsNull()) return NULL;

  TopoDS_Shape aWhere = theShapeWhere->GetValue();
  TopoDS_Shape aWhat  = theShapeWhat->GetValue();

  if (aWhere.IsNull() || aWhat.IsNull()) return NULL;

  Handle(GEOM_Function) aWhereFunction = theShapeWhere->GetLastFunction();
  if (aWhereFunction.IsNull()) return NULL;

  //Fill array of indices
  TopTools_IndexedMapOfShape aWhereIndices;

  TopExp::MapShapes(aWhere, aWhereIndices);

  // process shape
  TopTools_ListOfShape aModifiedList;
  bool isFound = GEOMAlgo_GetInPlaceAPI::GetInPlaceByHistory
    (aWhereFunction, aWhereIndices, aWhat, aModifiedList);

  if (!isFound || aModifiedList.Extent() < 1) {
    SetErrorCode("Error: No history found for the sought shape or its sub-shapes.");
    return NULL;
  }

  Handle(TColStd_HArray1OfInteger)   aModifiedArray =
    new TColStd_HArray1OfInteger (1, aModifiedList.Extent());
  TopTools_ListIteratorOfListOfShape anIterModif (aModifiedList);
  Standard_Integer                   imod;
  Standard_Integer                   aShapeType = -1;

  for (imod = 1; anIterModif.More(); anIterModif.Next(), imod++) {
    const Standard_Integer anIndex =
      aWhereIndices.FindIndex(anIterModif.Value());
    const TopAbs_ShapeEnum aType   = anIterModif.Value().ShapeType();

    if (aShapeType == -1) {
      // Initialization.
      aShapeType = aType;
    } else if (aShapeType != TopAbs_SHAPE && aShapeType != aType) {
      // Different types.
      aShapeType = TopAbs_SHAPE;
    }

    aModifiedArray->SetValue(imod, anIndex);
  }

  //Add a new object
  Handle(GEOM_Object) aResult = GetEngine()->AddSubShape(theShapeWhere, aModifiedArray);
  if (aResult.IsNull()) {
    SetErrorCode("Error in algorithm: result found, but cannot be returned.");
    return NULL;
  }

  const Standard_Boolean isSameType = (aShapeType != TopAbs_SHAPE);

  if ((aModifiedArray->Length() > 1 && isSameType) ||
      theShapeWhat->GetType() == GEOM_GROUP) {
    //Set a GROUP type
    aResult->SetType(GEOM_GROUP);

    //Set a sub-shape type
    TopoDS_Shape aFirstFound = aWhereIndices.FindKey(aModifiedArray->Value(1));
    TopAbs_ShapeEnum aShapeType = aFirstFound.ShapeType();

    TDF_Label aFreeLabel = aResult->GetFreeLabel();
    TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)aShapeType);
  }

  //Make a Python command
  Handle(GEOM_Function) aFunction = aResult->GetFunction(1);

  GEOM::TPythonDump(aFunction) << aResult << " = geompy.GetInPlaceByHistory("
                               << theShapeWhere << ", " << theShapeWhat << ")";

  SetErrorCode(OK);
  return aResult;
}

//=======================================================================
//function : isSameEdge
//purpose  : Returns True if two edges coincide
//=======================================================================
static bool isSameEdge(const TopoDS_Edge& theEdge1, const TopoDS_Edge& theEdge2)
{
  TopoDS_Vertex V11, V12, V21, V22;
  TopExp::Vertices(theEdge1, V11, V12);
  TopExp::Vertices(theEdge2, V21, V22);
  gp_Pnt P11 = BRep_Tool::Pnt(V11);
  gp_Pnt P12 = BRep_Tool::Pnt(V12);
  gp_Pnt P21 = BRep_Tool::Pnt(V21);
  gp_Pnt P22 = BRep_Tool::Pnt(V22);
  bool coincide = false;

  //Check that ends of edges coincide
  if(P11.Distance(P21) <= MAX_TOLERANCE) {
    if(P12.Distance(P22) <= MAX_TOLERANCE) coincide =  true;
  }
  else if(P11.Distance(P22) <= MAX_TOLERANCE) {
    if(P12.Distance(P21) <= MAX_TOLERANCE) coincide = true;
  }

  if(!coincide) return false;

  if (BRep_Tool::Degenerated(theEdge1))
    if (BRep_Tool::Degenerated(theEdge2)) return true;
    else return false;
  else
    if (BRep_Tool::Degenerated(theEdge2)) return false;

  double U11, U12, U21, U22;
  Handle(Geom_Curve) C1 = BRep_Tool::Curve(theEdge1, U11, U12);
  Handle(Geom_Curve) C2 = BRep_Tool::Curve(theEdge2, U21, U22);

  //Check that both edges has the same geometry
  double range = U12-U11;
  double U = U11+ range/3.0;
  gp_Pnt P1 = C1->Value(U);     //Compute a point on one third of the edge's length
  U = U11+range*2.0/3.0;
  gp_Pnt P2 = C1->Value(U);     //Compute a point on two thirds of the edge's length

  C2 = new Geom_TrimmedCurve(C2, U21, U22);

  if(!GeomLib_Tool::Parameter(C2, P1, MAX_TOLERANCE, U) ||  U < U21 || U > U22)
    return false;

  if(P1.Distance(C2->Value(U)) > MAX_TOLERANCE) return false;

  if(!GeomLib_Tool::Parameter(C2, P2, MAX_TOLERANCE, U) || U < U21 || U > U22)
    return false;

  if(P2.Distance(C2->Value(U)) > MAX_TOLERANCE) return false;

  return true;
}

//=======================================================================
//function : isSameFace
//purpose  : Returns True if two faces coincide
//=======================================================================
static bool isSameFace(const TopoDS_Face& theFace1, const TopoDS_Face& theFace2)
{
  TopExp_Explorer E(theFace1, TopAbs_EDGE);
  TopTools_ListOfShape LS1, LS2;
  for(; E.More(); E.Next()) LS1.Append(E.Current());

  E.Init(theFace2, TopAbs_EDGE);
  for(; E.More(); E.Next()) LS2.Append(E.Current());

  //Compare the number of edges in the faces
  if(LS1.Extent() != LS2.Extent()) return false;

  double aMin = RealFirst(), aMax = RealLast();
  double xminB1=aMax, yminB1=aMax, zminB1=aMax, xminB2=aMax, yminB2=aMax, zminB2=aMax;
  double xmaxB1=aMin, ymaxB1=aMin, zmaxB1=aMin, xmaxB2=aMin, ymaxB2=aMin, zmaxB2=aMin;

  for(E.Init(theFace1, TopAbs_VERTEX); E.More(); E.Next()) {
    gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(E.Current()));
    if(P.X() < xminB1) xminB1 = P.X();
    if(P.Y() < yminB1) yminB1 = P.Y();
    if(P.Z() < zminB1) zminB1 = P.Z();
    if(P.X() > xmaxB1) xmaxB1 = P.X();
    if(P.Y() > ymaxB1) ymaxB1 = P.Y();
    if(P.Z() > zmaxB1) zmaxB1 = P.Z();
  }

  for(E.Init(theFace2, TopAbs_VERTEX); E.More(); E.Next()) {
    gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(E.Current()));
    if(P.X() < xminB2) xminB2 = P.X();
    if(P.Y() < yminB2) yminB2 = P.Y();
    if(P.Z() < zminB2) zminB2 = P.Z();
    if(P.X() > xmaxB2) xmaxB2 = P.X();
    if(P.Y() > ymaxB2) ymaxB2 = P.Y();
    if(P.Z() > zmaxB2) zmaxB2 = P.Z();
  }

  //Compare the bounding boxes of both faces
  if(gp_Pnt(xminB1, yminB1, zminB1).Distance(gp_Pnt(xminB2, yminB2, zminB2)) > MAX_TOLERANCE)
    return false;

  if(gp_Pnt(xmaxB1, ymaxB1, zmaxB1).Distance(gp_Pnt(xmaxB2, ymaxB2, zmaxB2)) > MAX_TOLERANCE)
    return false;

  Handle(Geom_Surface) S1 = BRep_Tool::Surface(theFace1);
  Handle(Geom_Surface) S2 = BRep_Tool::Surface(theFace2);

  //Check if there a coincidence of two surfaces at least in two points
  double U11, U12, V11, V12, U21, U22, V21, V22;
  BRepTools::UVBounds(theFace1, U11, U12, V11, V12);
  BRepTools::UVBounds(theFace2, U21, U22, V21, V22);

  double rangeU = U12-U11;
  double rangeV = V12-V11;
  double U = U11 + rangeU/3.0;
  double V = V11 + rangeV/3.0;
  gp_Pnt P1 = S1->Value(U, V);
  U = U11+rangeU*2.0/3.0;
  V = V11+rangeV*2.0/3.0;
  gp_Pnt P2 = S1->Value(U, V);

  if (!GeomLib_Tool::Parameters(S2, P1, MAX_TOLERANCE, U, V) || U < U21 || U > U22 || V < V21 || V > V22)
    return false;

  if (P1.Distance(S2->Value(U,V)) > MAX_TOLERANCE) return false;

  if (!GeomLib_Tool::Parameters(S2, P2, MAX_TOLERANCE, U, V) || U < U21 || U > U22 || V < V21 || V > V22)
    return false;

  if (P2.Distance(S2->Value(U, V)) > MAX_TOLERANCE) return false;

  //Check that each edge of the Face1 has a counterpart in the Face2
  TopTools_MapOfOrientedShape aMap;
  TopTools_ListIteratorOfListOfShape LSI1(LS1);
  for(; LSI1.More(); LSI1.Next()) {
    TopoDS_Edge E = TopoDS::Edge(LSI1.Value());
    bool isFound = false;
    TopTools_ListIteratorOfListOfShape LSI2(LS2);
    for(; LSI2.More(); LSI2.Next()) {
      TopoDS_Shape aValue = LSI2.Value();
      if(aMap.Contains(aValue)) continue; //To avoid checking already found edge several times
      if(isSameEdge(E, TopoDS::Edge(aValue))) {
        aMap.Add(aValue);
        isFound = true;
        break;
      }
    }
    if(!isFound) return false;
  }

  return true;
}

//=======================================================================
//function : isSameSolid
//purpose  : Returns True if two solids coincide
//=======================================================================
bool isSameSolid(const TopoDS_Solid& theSolid1, const TopoDS_Solid& theSolid2)
{
  TopExp_Explorer E(theSolid1, TopAbs_FACE);
  TopTools_ListOfShape LS1, LS2;
  for(; E.More(); E.Next()) LS1.Append(E.Current());
  E.Init(theSolid2, TopAbs_FACE);
  for(; E.More(); E.Next()) LS2.Append(E.Current());

  if(LS1.Extent() != LS2.Extent()) return false;

  double aMin = RealFirst(), aMax = RealLast();
  double xminB1=aMax, yminB1=aMax, zminB1=aMax, xminB2=aMax, yminB2=aMax, zminB2=aMax;
  double xmaxB1=aMin, ymaxB1=aMin, zmaxB1=aMin, xmaxB2=aMin, ymaxB2=aMin, zmaxB2=aMin;

  for(E.Init(theSolid1, TopAbs_VERTEX); E.More(); E.Next()) {
    gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(E.Current()));
    if(P.X() < xminB1) xminB1 = P.X();
    if(P.Y() < yminB1) yminB1 = P.Y();
    if(P.Z() < zminB1) zminB1 = P.Z();
    if(P.X() > xmaxB1) xmaxB1 = P.X();
    if(P.Y() > ymaxB1) ymaxB1 = P.Y();
    if(P.Z() > zmaxB1) zmaxB1 = P.Z();
  }

  for(E.Init(theSolid2, TopAbs_VERTEX); E.More(); E.Next()) {
    gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(E.Current()));
    if(P.X() < xminB2) xminB2 = P.X();
    if(P.Y() < yminB2) yminB2 = P.Y();
    if(P.Z() < zminB2) zminB2 = P.Z();
    if(P.X() > xmaxB2) xmaxB2 = P.X();
    if(P.Y() > ymaxB2) ymaxB2 = P.Y();
    if(P.Z() > zmaxB2) zmaxB2 = P.Z();
  }

  //Compare the bounding boxes of both solids
  if(gp_Pnt(xminB1, yminB1, zminB1).Distance(gp_Pnt(xminB2, yminB2, zminB2)) > MAX_TOLERANCE)
    return false;

  if(gp_Pnt(xmaxB1, ymaxB1, zmaxB1).Distance(gp_Pnt(xmaxB2, ymaxB2, zmaxB2)) > MAX_TOLERANCE)
    return false;

  //Check that each face of the Solid1 has a counterpart in the Solid2
  TopTools_MapOfOrientedShape aMap;
  TopTools_ListIteratorOfListOfShape LSI1(LS1);
  for(; LSI1.More(); LSI1.Next()) {
    TopoDS_Face F = TopoDS::Face(LSI1.Value());
    bool isFound = false;
    TopTools_ListIteratorOfListOfShape LSI2(LS2);
    for(; LSI2.More(); LSI2.Next()) {
      if(aMap.Contains(LSI2.Value())) continue; //To avoid checking already found faces several times
      if(isSameFace(F, TopoDS::Face(LSI2.Value()))) {
        aMap.Add(LSI2.Value());
        isFound = true;
        break;
      }
    }
    if(!isFound) return false;
  }

  return true;
}

//=======================================================================
//function : GetSame
//purpose  :
//=======================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::GetSame(const Handle(GEOM_Object)& theShapeWhere,
                                                        const Handle(GEOM_Object)& theShapeWhat)
{
  SetErrorCode(KO);
  if (theShapeWhere.IsNull() || theShapeWhat.IsNull()) return NULL;

  TopoDS_Shape aWhere = theShapeWhere->GetValue();
  TopoDS_Shape aWhat  = theShapeWhat->GetValue();

  if (aWhere.IsNull() || aWhat.IsNull()) return NULL;

  int anIndex = -1;
  bool isFound = false;
  TopoDS_Shape aSubShape;
  TopTools_MapOfShape aMap;

  if (aWhat.ShapeType() == TopAbs_COMPOUND || aWhat.ShapeType() == TopAbs_COMPSOLID) {
    TopoDS_Iterator It (aWhat, Standard_True, Standard_True);
    if (It.More()) aWhat = It.Value();
    It.Next();
    if (It.More()) {
      SetErrorCode("Compounds of two or more shapes are not allowed for aWhat argument");
      return NULL;
    }
  }

  switch (aWhat.ShapeType()) {
    case TopAbs_VERTEX: {
      aSubShape = getSameVertex(aWhere, TopoDS::Vertex(aWhat));
      isFound   = !aSubShape.IsNull();
      break;
                        }
    case TopAbs_EDGE: {
      TopoDS_Edge anEdge = TopoDS::Edge(aWhat);
      TopExp_Explorer E(aWhere, TopAbs_EDGE);
      for(; E.More(); E.Next()) {
        if(!aMap.Add(E.Current())) continue;
        if(isSameEdge(anEdge, TopoDS::Edge(E.Current()))) {
          aSubShape = E.Current();
          isFound = true;
          break;
        }
      }
      break;
                      }
    case TopAbs_FACE: {
      TopoDS_Face aFace = TopoDS::Face(aWhat);
      TopExp_Explorer E(aWhere, TopAbs_FACE);
      for(; E.More(); E.Next()) {
        if(!aMap.Add(E.Current())) continue;
        if(isSameFace(aFace, TopoDS::Face(E.Current()))) {
          aSubShape = E.Current();
          isFound = true;
          break;
        }
      }
      break;
                      }
    case TopAbs_SOLID: {
      TopoDS_Solid aSolid = TopoDS::Solid(aWhat);
      TopExp_Explorer E(aWhere, TopAbs_SOLID);
      for(; E.More(); E.Next()) {
        if(!aMap.Add(E.Current())) continue;
        if(isSameSolid(aSolid, TopoDS::Solid(E.Current()))) {
          aSubShape = E.Current();
          isFound = true;
          break;
        }
      }
      break;
                       }
    default:
      return NULL;
  }

  if (isFound) {
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(aWhere, anIndices);
    if (anIndices.Contains(aSubShape))
      anIndex = anIndices.FindIndex(aSubShape);
  }

  if (anIndex < 0) return NULL;

  Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger(1,1);

  anArray->SetValue(1, anIndex);

  Handle(GEOM_Object) aResult = GetEngine()->AddSubShape(theShapeWhere, anArray);
  Handle(GEOM_Function) aFunction = aResult->GetLastFunction();

  GEOM::TPythonDump(aFunction) << aResult << " = geompy.GetSame("
    << theShapeWhere << ", " << theShapeWhat << ")";

  SetErrorCode(OK);

  return aResult;
}


//=======================================================================
//function : GetSameIDs
//purpose  :
//=======================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetSameIDs
                                                       (const Handle(GEOM_Object)& theShapeWhere,
                                                        const Handle(GEOM_Object)& theShapeWhat)
{
  SetErrorCode(KO);
  if (theShapeWhere.IsNull() || theShapeWhat.IsNull()) return NULL;

  TopoDS_Shape aWhere = theShapeWhere->GetValue();
  TopoDS_Shape aWhat  = theShapeWhat->GetValue();

  if (aWhere.IsNull() || aWhat.IsNull()) return NULL;

  TopTools_ListOfShape listShape;
  TopTools_MapOfShape aMap;

  if (aWhat.ShapeType() == TopAbs_COMPOUND || aWhat.ShapeType() == TopAbs_COMPSOLID) {
    TopoDS_Iterator It (aWhat, Standard_True, Standard_True);
    if (It.More()) aWhat = It.Value();
    It.Next();
    if (It.More()) {
      SetErrorCode("Compounds of two or more shapes are not allowed for aWhat argument");
      return NULL;
    }
  }

  switch (aWhat.ShapeType()) {
    case TopAbs_VERTEX: {
      gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(aWhat));
      TopExp_Explorer E(aWhere, TopAbs_VERTEX);
      for(; E.More(); E.Next()) {
        if(!aMap.Add(E.Current())) continue;
        gp_Pnt P2 = BRep_Tool::Pnt(TopoDS::Vertex(E.Current()));
        if(P.Distance(P2) <= MAX_TOLERANCE) {
          listShape.Append(E.Current());
        }
      }
      break;
                        }
    case TopAbs_EDGE: {
      TopoDS_Edge anEdge = TopoDS::Edge(aWhat);
      TopExp_Explorer E(aWhere, TopAbs_EDGE);
      for(; E.More(); E.Next()) {
        if(!aMap.Add(E.Current())) continue;
        if(isSameEdge(anEdge, TopoDS::Edge(E.Current()))) {
          listShape.Append(E.Current());
        }
      }
      break;
                      }
    case TopAbs_FACE: {
      TopoDS_Face aFace = TopoDS::Face(aWhat);
      TopExp_Explorer E(aWhere, TopAbs_FACE);
      for(; E.More(); E.Next()) {
        if(!aMap.Add(E.Current())) continue;
        if(isSameFace(aFace, TopoDS::Face(E.Current()))) {
          listShape.Append(E.Current());
        }
      }
      break;
                      }
    case TopAbs_SOLID: {
      TopoDS_Solid aSolid = TopoDS::Solid(aWhat);
      TopExp_Explorer E(aWhere, TopAbs_SOLID);
      for(; E.More(); E.Next()) {
        if(!aMap.Add(E.Current())) continue;
        if(isSameSolid(aSolid, TopoDS::Solid(E.Current()))) {
          listShape.Append(E.Current());
        }
      }
      break;
                       }
    default:
      return NULL;
  }

  if ( !listShape.IsEmpty() ) {
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(aWhere, anIndices);
    TopTools_ListIteratorOfListOfShape itSub (listShape);
    Handle(TColStd_HSequenceOfInteger) aSeq = new TColStd_HSequenceOfInteger;
    for (; itSub.More(); itSub.Next()) {
      if (anIndices.Contains(itSub.Value()))
        aSeq->Append(anIndices.FindIndex(itSub.Value()));
    }
    SetErrorCode(OK);
    // The GetSameIDs() doesn't change object so no new function is required.
    Handle(GEOM_Function) aFunction = GEOM::GetCreatedLast(theShapeWhere,theShapeWhat)->GetLastFunction();

  // Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listSameIDs = geompy.GetSameIDs("
    << theShapeWhere << ", "
    << theShapeWhat << ")";
    return aSeq;
  } else {
    SetErrorCode(NOT_FOUND_ANY);
    return NULL;
  }
}

//=======================================================================
//function : ExtendEdge
//purpose  :
//=======================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::ExtendEdge
                                      (const Handle(GEOM_Object) &theEdge,
                                       const Standard_Real        theMin,
                                       const Standard_Real        theMax)
{
  SetErrorCode(KO);

  if (theEdge.IsNull()) {
    return NULL;
  }

  //Add a new Edge object
  Handle(GEOM_Object) aResEdge = GetEngine()->AddObject(GetDocID(), GEOM_EDGE);

  //Add a new Vector function
  Handle(GEOM_Function) aFunction =
    aResEdge->AddFunction(GEOMImpl_ShapeDriver::GetID(), EDGE_UV);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) {
    return NULL;
  }

  GEOMImpl_IShapeExtend aCI (aFunction);

  Handle(GEOM_Function) anEdge = theEdge->GetLastFunction();

  if (anEdge.IsNull()) {
    return NULL;
  }

  aCI.SetShape(anEdge);
  aCI.SetUMin(theMin);
  aCI.SetUMax(theMax);

  //Compute the Edge value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");

      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());

    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction)
             << aResEdge  << " = geompy.ExtendEdge("
             << theEdge << ", " << theMin << ", " << theMax << ")";

  SetErrorCode(OK);

  return aResEdge;
}

//=======================================================================
//function : ExtendFace
//purpose  :
//=======================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::ExtendFace
                                      (const Handle(GEOM_Object) &theFace,
                                       const Standard_Real        theUMin,
                                       const Standard_Real        theUMax,
                                       const Standard_Real        theVMin,
                                       const Standard_Real        theVMax)
{
  SetErrorCode(KO);

  if (theFace.IsNull()) {
    return NULL;
  }

  //Add a new Face object
  Handle(GEOM_Object) aResFace = GetEngine()->AddObject(GetDocID(), GEOM_FACE);

  //Add a new Vector function
  Handle(GEOM_Function) aFunction =
    aResFace->AddFunction(GEOMImpl_ShapeDriver::GetID(), FACE_UV);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) {
    return NULL;
  }

  GEOMImpl_IShapeExtend aCI (aFunction);

  Handle(GEOM_Function) aFace = theFace->GetLastFunction();

  if (aFace.IsNull()) {
    return NULL;
  }

  aCI.SetShape(aFace);
  aCI.SetUMin(theUMin);
  aCI.SetUMax(theUMax);
  aCI.SetVMin(theVMin);
  aCI.SetVMax(theVMax);

  //Compute the Face value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");

      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());

    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction)
             << aResFace  << " = geompy.ExtendFace("
             << theFace << ", " << theUMin << ", " << theUMax << ", "
             << theVMin << ", " << theVMax << ")";

  SetErrorCode(OK);

  return aResFace;
}

//=======================================================================
//function : MakeSurfaceFromFace
//purpose  :
//=======================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeSurfaceFromFace
                                      (const Handle(GEOM_Object) &theFace)
{
  SetErrorCode(KO);

  if (theFace.IsNull()) {
    return NULL;
  }

  //Add a new Face object
  Handle(GEOM_Object) aResFace = GetEngine()->AddObject(GetDocID(), GEOM_FACE);

  //Add a new Vector function
  Handle(GEOM_Function) aFunction =
    aResFace->AddFunction(GEOMImpl_ShapeDriver::GetID(), SURFACE_FROM_FACE);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) {
    return NULL;
  }

  GEOMImpl_IShapeExtend aCI (aFunction);

  Handle(GEOM_Function) aFace = theFace->GetLastFunction();

  if (aFace.IsNull()) {
    return NULL;
  }

  aCI.SetShape(aFace);

  //Compute the Face value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");

      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());

    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction)
             << aResFace  << " = geompy.MakeSurfaceFromFace("
             << theFace << ")";

  SetErrorCode(OK);

  return aResFace;
}
