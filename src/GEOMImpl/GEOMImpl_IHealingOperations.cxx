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

#ifdef WIN32
#pragma warning( disable:4786 )
#endif

#include <Standard_Version.hxx>

#include <GEOMImpl_IHealingOperations.hxx>
#include <GEOM_PythonDump.hxx>
#include <GEOMImpl_HealingDriver.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_IHealing.hxx>
#include <GEOMImpl_IVector.hxx>
#include <GEOMImpl_VectorDriver.hxx>
#include <GEOMImpl_CopyDriver.hxx>
#include <ShHealOper_ModifStats.hxx>
#include <ShHealOper_ShapeProcess.hxx>

#include <utilities.h>
#include <Utils_ExceptHandlers.hxx>

#include <BRep_Builder.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopoDS_Compound.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOMImpl_IHealingOperations::GEOMImpl_IHealingOperations (GEOM_Engine* theEngine)
: GEOM_IOperations(theEngine)
{
  myModifStats = new ShHealOper_ModifStats;
  MESSAGE("GEOMImpl_IHealingOperations::GEOMImpl_IHealingOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_IHealingOperations::~GEOMImpl_IHealingOperations()
{
  delete myModifStats;
  MESSAGE("GEOMImpl_IHealingOperations::~GEOMImpl_IHealingOperations");
}


//=============================================================================
/*!
 *  ShapeProcess
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IHealingOperations::ShapeProcess (Handle(GEOM_Object) theObject,
                                  const Handle(TColStd_HArray1OfExtendedString)& theOperators,
                                  const Handle(TColStd_HArray1OfExtendedString)& theParams,
                                  const Handle(TColStd_HArray1OfExtendedString)& theValues)
{
  // set error code, check parameters
  SetErrorCode(KO);

  if (theObject.IsNull())
    return NULL;

  if (theOperators.IsNull() || theOperators->Length() <= 0) {
    SetErrorCode("No operators requested");
    return NULL;
  }

  Standard_Integer nbParams = 0, nbValues = 0;
  if (!theParams.IsNull()) {
    nbParams = theParams->Length();
  }
  if (!theValues.IsNull()) {
    nbValues = theValues->Length();
  }

  if (nbParams != nbValues) {
    SetErrorCode("Number of parameter values must be equal to the number of parameters");
    return NULL;
  }

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be processed

  // Add a new object
  Handle(GEOM_Object) aNewObject = GetEngine()->AddObject( GEOM_COPY );

  //Add the function
  aFunction = aNewObject->AddFunction(GEOMImpl_HealingDriver::GetID(), SHAPE_PROCESS);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

  // prepare "data container" class IHealing
  GEOMImpl_IHealing HI(aFunction);
  HI.SetOriginal(aLastFunction);
  HI.SetOperators( theOperators );
  if (nbParams > 0) {
    HI.SetParameters( theParams );
    HI.SetValues( theValues );
  }
  HI.SetStatistics( myModifStats );

  //Compute the translation
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction))
    {
      SetErrorCode("Shape Healing algorithm failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aNewObject << " = geompy.ProcessShape(" << theObject << ", [";

  // list of operators
  int i = theOperators->Lower(), nb = theOperators->Upper();
  for ( ; i <= nb; i++) {
    pd << "\"" << TCollection_AsciiString(theOperators->Value( i )).ToCString()
      << (( i < nb ) ? "\", " : "\"");
  }
  pd << "], [";
  // list of parameters
  i = theParams->Lower(); nb = theParams->Upper();
  for ( ; i <= nb; i++) {
    pd << "\"" << TCollection_AsciiString(theParams->Value( i )).ToCString()
      << (( i < nb ) ? "\", " : "\"");
  }
  pd << "], [";
  // list of values
  i = theValues->Lower(); nb = theValues->Upper();
  for ( ; i <= nb; i++) {
    pd << "\"" << TCollection_AsciiString(theValues->Value( i )).ToCString()
      << (( i < nb ) ? "\", " : "\"");
  }
  pd << "])";

  SetErrorCode(OK);
  return aNewObject;
}

//=============================================================================
/*!
 *  ShapeProcess
 */
//=============================================================================
void GEOMImpl_IHealingOperations::GetShapeProcessParameters (std::list<std::string>& theOperations,
                                                             std::list<std::string>& theParams,
                                                             std::list<std::string>& theValues)
{
  ShHealOper_ShapeProcess aHealer;
  TColStd_SequenceOfAsciiString anOperators;
  int nbOperatorErrors( 0 );
  if ( aHealer.GetOperators( anOperators ) )
  {
    for ( Standard_Integer i = 1; i <= anOperators.Length(); i++ )
    {
      std::string anOperation = anOperators.Value( i ).ToCString();
      if ( GetOperatorParameters( anOperation, theParams, theValues ) )
        theOperations.push_back( anOperation );
      else
        nbOperatorErrors++;
    }
  }
  else
  {
    SetErrorCode("ERROR retrieving operators (GEOMImpl_IHealingOperations)");
  }

  if ( nbOperatorErrors ) {
    TCollection_AsciiString aMsg ("ERRORS retrieving ShapeProcess parameters (GEOMImpl_IHealingOperations): nbOperatorErrors = ");
    aMsg += TCollection_AsciiString( nbOperatorErrors );
    MESSAGE(aMsg.ToCString());
  }
}

//=============================================================================
/*!
 *  GetOperatorParameters
 */
//=============================================================================
bool GEOMImpl_IHealingOperations::GetOperatorParameters( const std::string &     theOperation,
                                                         std::list<std::string>& theParams,
                                                         std::list<std::string>& theValues )
{
  ShHealOper_ShapeProcess aHealer;
  int nbParamValueErrors( 0 );
  std::list<std::string> aParams;
  if ( GetParameters( theOperation, aParams ) ) {
    for ( std::list<std::string>::iterator it = aParams.begin(); it != aParams.end(); ++it ) {
      TCollection_AsciiString aParam( (Standard_CString)(*it).c_str() );
      TCollection_AsciiString aValue;
      if ( aHealer.GetParameter( aParam, aValue ) ) {
        theParams.push_back( aParam.ToCString() );
        theValues.push_back( aValue.ToCString() );
      }
      else
        nbParamValueErrors++;
    }
  }
  else
    return false;

  if ( nbParamValueErrors ) {
    TCollection_AsciiString aMsg ("ERRORS retrieving ShapeProcess parameter values (GEOMImpl_IHealingOperations): nbParamValueErrors = ");
    aMsg += TCollection_AsciiString( nbParamValueErrors );
    MESSAGE(aMsg.ToCString());
  }

  return true;
}

//=============================================================================
/*!
 *  GetParameters
 */
//=============================================================================
bool GEOMImpl_IHealingOperations::GetParameters (const std::string theOperation,
                                                 std::list<std::string>& theParams)
{
  if ( theOperation == "SplitAngle" ) {
    theParams.push_back( "SplitAngle.Angle" );
    theParams.push_back( "SplitAngle.MaxTolerance" );

  } else if ( theOperation == "SplitClosedFaces" ) {
    theParams.push_back( "SplitClosedFaces.NbSplitPoints" );

  } else if ( theOperation == "FixFaceSize" ) {
    theParams.push_back( "FixFaceSize.Tolerance" );

  } else if( theOperation == "DropSmallEdges" ) {
    theParams.push_back( "DropSmallEdges.Tolerance3d" );

  } else if( theOperation == "DropSmallSolids" ) {
    theParams.push_back( "DropSmallSolids.WidthFactorThreshold" );
    theParams.push_back( "DropSmallSolids.VolumeThreshold" );
    theParams.push_back( "DropSmallSolids.MergeSolids" );

  } else if( theOperation == "BSplineRestriction" ) {
    theParams.push_back( "BSplineRestriction.SurfaceMode" );
    theParams.push_back( "BSplineRestriction.Curve3dMode" );
    theParams.push_back( "BSplineRestriction.Curve2dMode" );
    theParams.push_back( "BSplineRestriction.Tolerance3d" );
    theParams.push_back( "BSplineRestriction.Tolerance2d" );
    theParams.push_back( "BSplineRestriction.RequiredDegree" );
    theParams.push_back( "BSplineRestriction.RequiredNbSegments" );
    theParams.push_back( "BSplineRestriction.Continuity3d" );
    theParams.push_back( "BSplineRestriction.Continuity2d" );

  } else if( theOperation == "SplitContinuity" ) {
    theParams.push_back( "SplitContinuity.Tolerance3d" );
    theParams.push_back( "SplitContinuity.SurfaceContinuity" );
    theParams.push_back( "SplitContinuity.CurveContinuity" );

  } else if( theOperation == "ToBezier" ) {
    theParams.push_back( "ToBezier.SurfaceMode" );
    theParams.push_back( "ToBezier.Curve3dMode" );
    theParams.push_back( "ToBezier.Curve2dMode" );
    theParams.push_back( "ToBezier.MaxTolerance" );

  } else if( theOperation == "SameParameter" ) {
    theParams.push_back( "SameParameter.Tolerance3d" );

  } else if( theOperation == "FixShape" ) {
    theParams.push_back( "FixShape.Tolerance3d" );
    theParams.push_back( "FixShape.MaxTolerance3d" );

  } else {
    return false;
  }

  return true;
}

//=============================================================================
/*!
 *  SuppressFaces
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IHealingOperations::SuppressFaces
       (Handle(GEOM_Object) theObject, const Handle(TColStd_HArray1OfInteger)& theFaces)
{
  // set error code, check parameters
  SetErrorCode(KO);

  if (theObject.IsNull()) // if theFaces.IsNull() - it's OK, it means that ALL faces must be removed..
    return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be processed

  // Add a new object
  Handle(GEOM_Object) aNewObject = GetEngine()->AddObject(GEOM_COPY);

  //Add the function
  aFunction = aNewObject->AddFunction(GEOMImpl_HealingDriver::GetID(), SUPPRESS_FACES);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

  // prepare "data container" class IHealing
  GEOMImpl_IHealing HI (aFunction);
  HI.SetFaces(theFaces);
  HI.SetOriginal(aLastFunction);
  HI.SetStatistics( myModifStats );

  //Compute the translation
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction))
    {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aNewObject << " = geompy.SuppressFaces(" << theObject << ", [";

  // list of face ids
  int i = theFaces->Lower(), nb = theFaces->Upper();
  for ( ; i <= nb; i++)
    pd << theFaces->Value( i ) << (( i < nb ) ? ", " : "])");

  SetErrorCode(OK);
  return aNewObject;
}

//=============================================================================
/*!
 *  CloseContour
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IHealingOperations::CloseContour
                    (Handle(GEOM_Object) theObject,
                     const Handle(TColStd_HArray1OfInteger)& theWires,
                     bool isCommonVertex)
{
  // set error code, check parameters
  SetErrorCode(KO);

  if (theObject.IsNull())
  {
    SetErrorCode("NULL object given");
    return NULL;
  }

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be processed

  // Add a new object
  Handle(GEOM_Object) aNewObject = GetEngine()->AddObject( GEOM_COPY );

  //Add the function
  aFunction = aNewObject->AddFunction(GEOMImpl_HealingDriver::GetID(), CLOSE_CONTOUR);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if(aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

  // prepare "data container" class IHealing
  GEOMImpl_IHealing HI(aFunction);
  HI.SetWires( theWires );
  HI.SetIsCommonVertex( isCommonVertex );
  HI.SetOriginal( aLastFunction );
  HI.SetStatistics( myModifStats );

  //Compute the translation
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction))
    {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aNewObject << " = geompy.CloseContour(" << theObject << ", [";

  // list of wire ids
  if (!theWires.IsNull())
  {
    int i = theWires->Lower(), nb = theWires->Upper();
    pd << theWires->Value(i++);
    while (i <= nb)
      pd << ", " << theWires->Value(i++);
  }
  pd << "], " << (int)isCommonVertex << ")";

  SetErrorCode(OK);
  return aNewObject;
}

//=============================================================================
/*!
 *  RemoveIntWires
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IHealingOperations::RemoveIntWires
       (Handle(GEOM_Object) theObject, const Handle(TColStd_HArray1OfInteger)& theWires)
{
  // set error code, check parameters
  SetErrorCode(KO);

  if (theObject.IsNull()) // if theWires is NULL it's OK, it means that ALL wires must be removed
    return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be processed

  // Add a new object
  Handle(GEOM_Object) aNewObject = GetEngine()->AddObject( GEOM_COPY );

  //Add the function
  aFunction = aNewObject->AddFunction(GEOMImpl_HealingDriver::GetID(), REMOVE_INT_WIRES);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

  // prepare "data container" class IHealing
  GEOMImpl_IHealing HI(aFunction);
  HI.SetWires( theWires );
  HI.SetOriginal( aLastFunction );
  HI.SetStatistics( myModifStats );

  //Compute the translation
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction))
    {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aNewObject << " = geompy.SuppressInternalWires(" << theObject << ", [";

  // list of wire ids
  if (!theWires.IsNull()) {
    int i = theWires->Lower(), nb = theWires->Upper();
    for ( ; i <= nb; i++)
      pd << theWires->Value( i ) << (( i < nb ) ? ", " : "])");
  } else {
    pd << "])";
  }

  SetErrorCode(OK);
  return aNewObject;
}

//=============================================================================
/*!
 *  FillHoles
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IHealingOperations::FillHoles (Handle(GEOM_Object) theObject,
                                                            const Handle(TColStd_HArray1OfInteger)& theWires)
{
  // set error code, check parameters
  SetErrorCode(KO);

  if (theObject.IsNull()) // if theWires is NULL it's OK, it means that ALL holes must be removed
    return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be processed

  // Add a new object
  Handle(GEOM_Object) aNewObject = GetEngine()->AddObject( GEOM_COPY );

  //Add the function
  aFunction = aNewObject->AddFunction(GEOMImpl_HealingDriver::GetID(), FILL_HOLES);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

  // prepare "data container" class IHealing
  GEOMImpl_IHealing HI(aFunction);
  HI.SetWires( theWires );
  HI.SetOriginal( aLastFunction );
  HI.SetStatistics( myModifStats );

  //Compute the translation
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction))
    {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aNewObject << " = geompy.SuppressHoles(" << theObject << ", [";

  // list of wire ids
  if ( theWires.IsNull() )
    pd << "])";
  else {
    int i = theWires->Lower(), nb = theWires->Upper();
    for ( ; i <= nb; i++)
      pd << theWires->Value( i ) << (( i < nb ) ? ", " : "])");
  }

  SetErrorCode(OK);
  return aNewObject;
}

//=============================================================================
/*!
 *  Sew
 */
//=============================================================================
Handle(GEOM_Object)
GEOMImpl_IHealingOperations::Sew (std::list<Handle(GEOM_Object)>& theObjects,
                                  double                          theTolerance,
                                  bool                            isAllowNonManifold)
{
  // set error code, check parameters
  SetErrorCode(KO);

  if (theObjects.empty())
    return NULL;

  Handle(TColStd_HSequenceOfTransient) objects =
    GEOM_Object::GetLastFunctions( theObjects );
  if ( objects.IsNull() || objects->IsEmpty() ) {
    SetErrorCode("NULL argument shape");
    return NULL;
  }

  // Add a new object
  Handle(GEOM_Object) aNewObject = GetEngine()->AddObject( GEOM_COPY );

  //Add the function
  int aFunctionType = (isAllowNonManifold ? SEWING_NON_MANIFOLD : SEWING);
  Handle(GEOM_Function) aFunction =
    aNewObject->AddFunction(GEOMImpl_HealingDriver::GetID(), aFunctionType);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

  // prepare "data container" class IHealing
  GEOMImpl_IHealing HI(aFunction);
  HI.SetTolerance( theTolerance );
  HI.SetOriginal( theObjects.front()->GetLastFunction() ); objects->Remove(1);
  HI.SetShapes( objects );
  HI.SetStatistics( myModifStats );

  //Compute the result
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction))
    {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd(aFunction);
  
  pd << aNewObject << " = geompy.Sew(" << theObjects << ", " << theTolerance;

  if (isAllowNonManifold) {
    pd << ", True";
  }

  pd << ")";

  SetErrorCode(OK);
  return aNewObject;
}

//=============================================================================
/*!
 *  RemoveInternalFaces
 */
//=============================================================================
Handle(GEOM_Object)
GEOMImpl_IHealingOperations::RemoveInternalFaces (std::list< Handle(GEOM_Object)> & theSolids)
{
  // set error code, check parameters
  SetErrorCode(KO);

  if (theSolids.empty())
    return NULL;

  Handle(TColStd_HSequenceOfTransient) objects = GEOM_Object::GetLastFunctions( theSolids );
  if ( objects.IsNull() || objects->IsEmpty() ) {
    SetErrorCode("NULL argument shape");
    return NULL;
  }

  // Add a new object
  Handle(GEOM_Object) aNewObject = GetEngine()->AddObject(GEOM_COPY);

  //Add the function
  Handle(GEOM_Function)
    aFunction = aNewObject->AddFunction(GEOMImpl_HealingDriver::GetID(), REMOVE_INTERNAL_FACES);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

  // prepare "data container" class IHealing
  GEOMImpl_IHealing HI (aFunction);
  HI.SetOriginal( theSolids.front()->GetLastFunction() ); objects->Remove(1);
  HI.SetShapes( objects );
  HI.SetStatistics( myModifStats );

  //Compute the result
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction))
    {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aNewObject << " = geompy.RemoveInternalFaces(" << theSolids << ")";

  SetErrorCode(OK);
  return aNewObject;
}

//=============================================================================
/*!
 *  DivideEdge
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IHealingOperations::DivideEdge (Handle(GEOM_Object) theObject,
                                                             int theIndex,
                                                             double theValue,
                                                             bool isByParameter)
{
  // set error code, check parameters
  SetErrorCode(KO);

  if (theObject.IsNull())
    return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be processed

  // Add a new object
  Handle(GEOM_Object) aNewObject = GetEngine()->AddObject( GEOM_COPY );

  //Add the function
  aFunction = aNewObject->AddFunction(GEOMImpl_HealingDriver::GetID(), DIVIDE_EDGE);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

  // prepare "data container" class IHealing
  GEOMImpl_IHealing HI(aFunction);
  HI.SetIndex( theIndex );
  HI.SetDevideEdgeValue( theValue );
  HI.SetIsByParameter( isByParameter );
  HI.SetOriginal( aLastFunction );
  HI.SetStatistics( myModifStats );

  //Compute the translation
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aNewObject << " = geompy.DivideEdge(" << theObject
    << ", " << theIndex << ", " << theValue << ", " << (int)isByParameter << ")";

  SetErrorCode(OK);
  return aNewObject;
}

//=============================================================================
/*!
 *  DivideEdgeByPoint
 */
//=============================================================================
Handle(GEOM_Object)
GEOMImpl_IHealingOperations::DivideEdgeByPoint (Handle(GEOM_Object)               theObject,
                                                int                               theIndex,
                                                std::list< Handle(GEOM_Object)> & thePoints)
{
  // set error code, check parameters
  SetErrorCode(KO);

  if (theObject.IsNull() || thePoints.empty() )
    return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull() )
    return NULL; //There is no function which creates an object to be processed

  Handle(TColStd_HSequenceOfTransient) aPointFunc = GEOM_Object::GetLastFunctions( thePoints );
  if ( aPointFunc.IsNull() || aPointFunc->IsEmpty() ) {
    SetErrorCode("NULL argument points");
    return NULL;
  }

  // Add a new object
  Handle(GEOM_Object) aNewObject = GetEngine()->AddObject( GEOM_COPY );

  //Add the function
  aFunction = aNewObject->AddFunction(GEOMImpl_HealingDriver::GetID(), DIVIDE_EDGE_BY_POINT);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

  // prepare "data container" class IHealing
  GEOMImpl_IHealing HI(aFunction);
  HI.SetIndex     ( theIndex );
  HI.SetOriginal  ( aLastFunction );
  HI.SetShapes    ( aPointFunc );

  HI.SetStatistics( myModifStats );

  //Compute the translation
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction)
    << aNewObject << " = geompy.DivideEdgeByPoint(" << theObject
    << ", " << theIndex << ", " << thePoints << ")";

  SetErrorCode(OK);
  return aNewObject;
}

//=============================================================================
/*!
 *  FuseCollinearEdgesWithinWire
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IHealingOperations::FuseCollinearEdgesWithinWire
                                   (Handle(GEOM_Object) theWire,
                                    std::list<Handle(GEOM_Object)> theVertices)
{
  SetErrorCode(KO);

  if (theWire.IsNull()) return NULL;

  // Add a new object
  Handle(GEOM_Object) aRes = GetEngine()->AddObject(theWire->GetType());

  // Add a new function
  Handle(GEOM_Function) aFunction;
  aFunction = aRes->AddFunction(GEOMImpl_HealingDriver::GetID(), FUSE_COLLINEAR_EDGES);
  if (aFunction.IsNull()) return NULL;

  // Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

  GEOMImpl_IHealing aCI (aFunction);
  aCI.SetStatistics( myModifStats );

  Handle(GEOM_Function) aRefShape = theWire->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;
  aCI.SetOriginal(aRefShape);

  Handle(TColStd_HSequenceOfTransient) aVertices = new TColStd_HSequenceOfTransient;
  std::list<Handle(GEOM_Object)>::iterator it = theVertices.begin();
  for (; it != theVertices.end(); it++) {
    Handle(GEOM_Function) aRefSh = (*it)->GetLastFunction();
    if (aRefSh.IsNull()) {
      SetErrorCode("NULL argument shape for the shape construction");
      return NULL;
    }
    aVertices->Append(aRefSh);
  }
  aCI.SetShapes(aVertices);

  // Compute the new wire
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  // Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aRes << " = geompy.FuseCollinearEdgesWithinWire(" << theWire << ", [";
  // Vertices
  it = theVertices.begin();
  if (it != theVertices.end()) {
    pd << (*it++);
    while (it != theVertices.end()) {
      pd << ", " << (*it++);
    }
  }
  pd << "])";

  SetErrorCode(OK);
  return aRes;
}

//=============================================================================
/*!
 *  GetFreeBoundary
 */
//=============================================================================
bool GEOMImpl_IHealingOperations::GetFreeBoundary (Handle(TColStd_HSequenceOfTransient)& theObjects,
                                                   Handle(TColStd_HSequenceOfTransient)& theClosed,
                                                   Handle(TColStd_HSequenceOfTransient)& theOpen )
{
  // set error code, check parameters
  SetErrorCode(KO);

  if ( theObjects.IsNull() || theObjects->Length() == 0 ||
       theClosed.IsNull()  || theOpen.IsNull() )
    return false;

  TopoDS_Shape aShape;
  TopTools_SequenceOfShape shapes;
  for ( int ind = 1; ind <= theObjects->Length(); ind++)
  {
    Handle(GEOM_Object) aRefShape = Handle(GEOM_Object)::DownCast( theObjects->Value(ind));
    if ( aRefShape.IsNull() )
      return false;
    aShape = aRefShape->GetValue();
    if ( aShape.IsNull() )
      return false;
    shapes.Append( aShape );
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

  // get free boundary shapes

  ShapeAnalysis_FreeBounds anAnalizer(aShape, Standard_False,
                                      Standard_True, Standard_True);
  TopoDS_Compound aClosed = anAnalizer.GetClosedWires();
  TopoDS_Compound anOpen = anAnalizer.GetOpenWires();

  // iterate through shapes and append them to the return sequence
  Handle(GEOM_Object) anObj;
  Handle(GEOM_Function) aFunction;
  TopExp_Explorer anExp;
  for ( anExp.Init( aClosed, TopAbs_WIRE ); anExp.More(); anExp.Next() )
  {
    anObj = GetEngine()->AddObject( GEOM_FREE_BOUNDS );
    aFunction = anObj->AddFunction( GEOMImpl_CopyDriver::GetID(), COPY_WITHOUT_REF );
    TopoDS_Shape aValueShape = anExp.Current();
    aFunction->SetValue( aValueShape );
    theClosed->Append(anObj);
  }
  for ( anExp.Init( anOpen, TopAbs_WIRE ); anExp.More(); anExp.Next() )
  {
    anObj = GetEngine()->AddObject( GEOM_FREE_BOUNDS );
    aFunction = anObj->AddFunction( GEOMImpl_CopyDriver::GetID(), COPY_WITHOUT_REF );
    TopoDS_Shape aValueShape = anExp.Current();
    aFunction->SetValue( aValueShape );
    theOpen->Append(anObj);
  }

  if(!aFunction.IsNull()) {

    //Make a Python command
    GEOM::TPythonDump pd (aFunction);

    Standard_Integer i, aLen = theClosed->Length();
    if (aLen > 0) {
      pd << "(isDone, [";
      for (i = 1; i <= aLen; i++) {
        Handle(GEOM_Object) anObj_i = Handle(GEOM_Object)::DownCast(theClosed->Value(i));
        pd << anObj_i << ((i < aLen) ? ", " : "");
      }
      pd << "], ";
    } else {
      pd << "(isDone, empty_list, ";
    }

    aLen = theOpen->Length();
    if (aLen > 0) {
      pd << "[";
      for (i = 1; i <= aLen; i++) {
        Handle(GEOM_Object) anObj_i = Handle(GEOM_Object)::DownCast(theOpen->Value(i));
        pd << anObj_i << ((i < aLen) ? ", " : "");
      }
      pd << "]";
    } else {
      pd << "empty_list";
    }

    pd << ") = geompy.GetFreeBoundary(" << theObjects << ")";
  }

  SetErrorCode(OK);
  return true;
}


//=============================================================================
/*!
 *  ChangeOrientation
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IHealingOperations::ChangeOrientation (Handle(GEOM_Object) theObject)
{
  // set error code, check parameters
  SetErrorCode(KO);

  if (theObject.IsNull())
    return NULL;

  if (!theObject->IsMainShape()) {
    SetErrorCode("Sub-shape cannot be transformed - need to create a copy");
    return NULL;
  }

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull())
    return NULL; //There is no function which creates an object to be processed

  if (theObject->GetType() == GEOM_VECTOR) { // Mantis issue 21066
    //Add the function
    aFunction = theObject->AddFunction(GEOMImpl_VectorDriver::GetID(), VECTOR_REVERSE);

    //Check if the function is set correctly
    if (aFunction.IsNull()) return NULL;
    if (aFunction->GetDriverGUID() != GEOMImpl_VectorDriver::GetID()) return NULL;

    // prepare "data container" class IVector
    GEOMImpl_IVector aVI (aFunction);
    aVI.SetCurve(aLastFunction);

    myModifStats->Clear();
    myModifStats->AddModif( "Vector reversed" );
  }
  else {
    //Add the function
    aFunction = theObject->AddFunction(GEOMImpl_HealingDriver::GetID(), CHANGE_ORIENTATION);

    //Check if the function is set correctly
    if (aFunction.IsNull()) return NULL;
    if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

    // prepare "data container" class IHealing
    GEOMImpl_IHealing HI (aFunction);
    HI.SetOriginal(aLastFunction);
    HI.SetStatistics( myModifStats );
  }

  //Compute the translation
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.ChangeOrientationShell("
                               << theObject << ")";

  SetErrorCode(OK);
  return theObject;
}

//=============================================================================
/*!
 *  ChangeOrientationCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IHealingOperations::ChangeOrientationCopy (Handle(GEOM_Object) theObject)
{
  // set error code, check parameters
  SetErrorCode(KO);

  if (theObject.IsNull())
    return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull())
    return NULL; //There is no function which creates an object to be processed

  // Add a new object
  Handle(GEOM_Object) aNewObject = GetEngine()->AddObject(GEOM_COPY);

  if (theObject->GetType() == GEOM_VECTOR) { // Mantis issue 21066
    //Add the function
    aFunction = aNewObject->AddFunction(GEOMImpl_VectorDriver::GetID(), VECTOR_REVERSE);

    //Check if the function is set correctly
    if (aFunction.IsNull()) return NULL;
    if (aFunction->GetDriverGUID() != GEOMImpl_VectorDriver::GetID()) return NULL;

    // prepare "data container" class IVector
    GEOMImpl_IVector aVI (aFunction);
    aVI.SetCurve(aLastFunction);

    myModifStats->Clear();
    myModifStats->AddModif( "Vector reversed" );
  }
  else {
    //Add the function
    aFunction = aNewObject->AddFunction(GEOMImpl_HealingDriver::GetID(), CHANGE_ORIENTATION);

    //Check if the function is set correctly
    if (aFunction.IsNull()) return NULL;
    if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

    // prepare "data container" class IHealing
    GEOMImpl_IHealing aHI (aFunction);
    aHI.SetOriginal(aLastFunction);
    aHI.SetStatistics( myModifStats );
  }

  // Compute the result
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aNewObject << " = geompy.ChangeOrientationShellCopy("
                               << theObject << ")";

  SetErrorCode(OK);
  return aNewObject;
}

//=============================================================================
/*!
 *  LimitTolerance
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IHealingOperations::LimitTolerance (Handle(GEOM_Object) theObject,
                                                                 double theTolerance,
                                                                 TopAbs_ShapeEnum theType)
{
  // Set error code, check parameters
  SetErrorCode(KO);

  if (theObject.IsNull())
    return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull())
    return NULL; // There is no function which creates an object to be processed

  // Add a new object
  Handle(GEOM_Object) aNewObject = GetEngine()->AddObject(theObject->GetType());

  // Add the function
  aFunction = aNewObject->AddFunction(GEOMImpl_HealingDriver::GetID(), LIMIT_TOLERANCE);

  if (aFunction.IsNull())
    return NULL;

  // Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_HealingDriver::GetID()) return NULL;

  // Prepare "data container" class IHealing
  GEOMImpl_IHealing HI (aFunction);
  HI.SetOriginal(aLastFunction);
  HI.SetTolerance(theTolerance);
  HI.SetType(theType);
  HI.SetStatistics( myModifStats );

  // Compute
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Healing driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  // Make a Python command
  GEOM::TPythonDump(aFunction) << aNewObject << " = geompy.LimitTolerance("
                               << theObject << ", " << theTolerance << ")";

  SetErrorCode(OK);
  return aNewObject;
}
