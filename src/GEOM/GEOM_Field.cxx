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

#include "GEOM_Field.hxx"

#include "GEOM_IField.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_PythonDump.hxx"

#include <Standard_MultiplyDefined.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include "utilities.h"

#include <limits>

using namespace GEOM;

namespace
{
  //================================================================================
  /*!
   * \brief Returns a funtion with a given type OR the 1st function
   */
  //================================================================================

  Handle(GEOM_Function) getFunction(int theFunType, GEOM_BaseObject* obj )
  {
    Handle(GEOM_Function) fun;
    int nbFuns = obj->GetNbFunctions();
    while ( nbFuns >= 1 )
    {
      fun = obj->GetFunction( nbFuns-- );
      const int funType = fun->GetType();
      if ( funType == theFunType )
        return fun;
    }
    return fun; // function 1
  }
}

//=============================================================================
/*!
 *  Constructor: private
 */
//=============================================================================

GEOM_Field::GEOM_Field(const TDF_Label& theEntry)
  : GEOM_BaseObject(theEntry), nbSubShapes(-1)
{
}

//=============================================================================
/*!
 *  Constructor: public
 */
//=============================================================================

GEOM_Field::GEOM_Field(const TDF_Label&    theEntry, int /*typ*/)
  : GEOM_BaseObject( theEntry, GEOM_FIELD_OBJTYPE ), nbSubShapes(-1)
{
}

//================================================================================
/*!
 * \brief Sets the basic data that do not change (except compNames?)
 */
//================================================================================

void GEOM_Field::Init(const Handle(GEOM_Object)&                     theShape,
                      const char*                                    theName,
                      const int                                      theDataType,
                      const int                                      theDimension,
                      const Handle(TColStd_HArray1OfExtendedString)& theCompNames)
{
  Handle(GEOM_Function) fun = GetFunction(1);
  if ( !fun.IsNull() )
    Standard_MultiplyDefined::Raise( "Reinitialization of GEOM_Field is forbiden" );
  fun = AddFunction( GetFieldID(), FUN_ADD_FIELD );

  GEOM_IField data( fun );
  data.SetShape     ( theShape->GetLastFunction() );
  data.SetDataType  ( theDataType );
  data.SetDimension ( theDimension );
  data.SetComponents( theCompNames );

  TPythonDump py( fun ); // prevent dump of SetName
  SetName( theName );
  // PythonDump to be done by the operation creating this field
}

//=============================================================================
/*!
 *  GetField
 */
//=============================================================================

Handle(GEOM_Field) GEOM_Field::GetField(const TDF_Label& theLabel)
{
  Handle(GEOM_BaseObject) base = GEOM_BaseObject::GetObject(theLabel);
  return Handle(GEOM_Field)::DownCast( base );
}

//=======================================================================
//function : GetFieldID
//purpose  :
//=======================================================================

const Standard_GUID& GEOM_Field::GetFieldID()
{
  static Standard_GUID anID("FF1BBB01-5252-4df2-980B-3A668264EA16");
  return anID;
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================

GEOM_Field::~GEOM_Field()
{
}

//=============================================================================
/*!
 *  Returns a shape this GEOM_Field lies on
 */
//=============================================================================

Handle(GEOM_Object) GEOM_Field::GetShape()
{
  Handle(GEOM_Object) shapeObject;

  Handle(GEOM_Function) fun = GetFunction(1);
  if ( !fun.IsNull() )
  {
    GEOM_IField data( fun );
    Handle(GEOM_Function) shapeFun = data.GetShape();
    if ( !shapeFun.IsNull() )
    {
      TDF_Label shapeLabel = shapeFun->GetOwnerEntry();
      shapeObject = GEOM_Object::GetObject( shapeLabel );
    }
  }

  return shapeObject;
}

//=======================================================================
//function : GetNbSubShapes
//purpose  : Returns number of sub-shapes.
//           Size of data arrays == GetNbSubShapes() * GetComponents()->Extent()
//=======================================================================

int GEOM_Field::GetNbSubShapes()
{
  if ( nbSubShapes < 0 )
    nbSubShapes = GetNbSubShapes( GetShape(), GetDimension() );

  return nbSubShapes;
}

//=======================================================================
//function : GetNbSubShapes
//purpose  : Returns number of sub-shapes of given dimension
//=======================================================================

int GEOM_Field::GetNbSubShapes(const Handle(GEOM_Object)& shObj,
                               const int                  dim)
{
  int nbSubShapes = 0;
  if ( shObj.IsNull() ) return nbSubShapes;

  TopoDS_Shape shape = shObj->GetValue();
  if (shape.IsNull() ) return nbSubShapes;

  if ( dim == -1 )
  {
    nbSubShapes = 1;
  }
  else
  {
    TopAbs_ShapeEnum type;
    switch( dim ) {
    case 0: type = TopAbs_VERTEX; break;
    case 1: type = TopAbs_EDGE; break;
    case 2: type = TopAbs_FACE; break;
    case 3: type = TopAbs_SOLID; break;
    default: return nbSubShapes;
    }
    TopTools_IndexedMapOfShape map;
    TopExp::MapShapes( shape, type, map );
    nbSubShapes = map.Extent();
  }
  return nbSubShapes;
}

//=======================================================================
//function : GetNbComponents
//purpose  : Returns number of components
//=======================================================================

int GEOM_Field::GetNbComponents()
{
  Handle(TColStd_HArray1OfExtendedString) comps = GetComponents();
  return comps.IsNull() ? 0 : comps->Length();
}

//=======================================================================
//function : GetArrayLength
//purpose  : Returns size of data array == GetNbSubShapes() * GetComponents()->Extent()
//=======================================================================

int GEOM_Field::GetArrayLength()
{
  return GetNbComponents() * GetNbSubShapes();
}

//=======================================================================
//function : GetDataType
//purpose  : Returns a data type of this GEOM_Field
//=======================================================================

int GEOM_Field::GetDataType()
{
  Handle(GEOM_Function) fun = GetFunction(1);
  if ( !fun.IsNull() )
    return GEOM_IField( fun ).GetDataType();
  return -1;
}

//=======================================================================
//function : GetDataTypeString
//purpose  : Returns one of "Bool","Int","Double","String"
//=======================================================================

TCollection_AsciiString GEOM_Field::GetDataTypeString(int type)
{
  const char* typeNames[] = { "Bool","Int","Double","String" };
  if ( type < 0 || type > 3 )
    return type;
  return typeNames[ type ];
}

//=======================================================================
//function : GetDimension
//purpose  : Returns dimension of the shape the field lies on:
//           0 - VERTEX, 1 - EDGE, 2 - FACE, 3 - SOLID, -1 - whole shape
//=======================================================================

int GEOM_Field::GetDimension()
{
  Handle(GEOM_Function) fun = GetFunction(1);
  if ( !fun.IsNull() )
    return GEOM_IField( fun ).GetDimension();
  return -1;
}

//=======================================================================
//function : SetComponents
//purpose  : Sets names of components
//=======================================================================

void GEOM_Field::SetComponents( const Handle(TColStd_HArray1OfExtendedString)& compNames )
{
  // By spec. modification of components is not required, but just in case...
  Handle(GEOM_Function) fun = GetLastFunction();
  if ( fun->GetType() != FUN_ADD_FIELD )
  {
    fun = AddFunction( GetFieldID(), FUN_CHANGE_COMP_NAMES );
    //TPythonDump( fun ) << this << ".setComponents( "
  }
  GEOM_IField data( fun );
  data.SetComponents( compNames );
}

//=======================================================================
//function : GetComponents
//purpose  : Returns names of components
//=======================================================================

Handle(TColStd_HArray1OfExtendedString) GEOM_Field::GetComponents()
{
  Handle(GEOM_Function) fun = getFunction( FUN_CHANGE_COMP_NAMES, this );
  if ( !fun.IsNull() )
    return GEOM_IField( fun ).GetComponents();
  return NULL;
}

//=======================================================================
//function : getFunctionToSetValues
//purpose  : dump any HArray into a string
//=======================================================================
template< class HandleArray1 >
TCollection_AsciiString arrayToSting( const HandleArray1& ar,
                                      const char*            quote="")
{
  TCollection_AsciiString s;
  char prefix[] = "[ ";
  if ( !ar.IsNull() )
    for ( int i = ar->Lower(), nb = ar->Upper(); i <= nb; ++i )
    {
      s += prefix;
      s += quote;
      s += TCollection_AsciiString( ar->Value( i ));
      s += quote;
      prefix[0] = ',';
    }
  if ( !s.IsEmpty() )
    s += " ]";

  return s;
}

//=======================================================================
//function : GetComponentsForPython
//purpose  : Returns names of components in a python syntax
//=======================================================================

TCollection_AsciiString GEOM_Field::GetComponentsForPython()
{
  return arrayToSting( GetComponents(), "'" );
}

//=======================================================================
//function : AddStep
//purpose  : Adds a step
//=======================================================================

Handle(GEOM_FieldStep) GEOM_Field::AddStep(const int stepID, const int stamp)
{
  Handle(GEOM_FieldStep) step = GetStep( stepID );
  if ( !step.IsNull() )
    return NULL;

  GEOM_Engine* anEngine = GEOM_Engine::GetEngine();
  if(anEngine == NULL) return NULL;

  step = Handle(GEOM_FieldStep)::DownCast
    ( anEngine->AddBaseObject( GetDocID(),GEOM_FIELD_STEP_OBJTYPE ));
  if ( step.IsNull())
    return step;

  // set all step data
  Handle(GEOM_Field) field = GEOM_Field::GetField( GetEntry() );
  step->Init( field, stepID, stamp );

  Handle(TDataStd_TreeNode) aRoot, aNode;
  aRoot = TDataStd_TreeNode::Set( GetEntry(),       GetFieldID() );
  aNode = TDataStd_TreeNode::Set( step->GetEntry(), GetFieldID() );
  aRoot->Append(aNode);

  // Dump just in case if step.SetValues() would fail which normally
  // replaces this dump.
  // field.addStep(step, stamp, values)
  TCollection_AsciiString defaultVal( GetDataType() == 3 ? "''" : "0" );
  TPythonDump( step->GetFunction(1) )
    << step << " = "
    << this << ".addStep( "
    << stepID << ", "
    << stamp << ", "
    << "[" << defaultVal << "]*" << GetArrayLength() << " )";

  return step;
}

//=======================================================================
//function : RemoveStep
//purpose  : Removes a step
//=======================================================================

void GEOM_Field::RemoveStep(const int stepID)
{
  Handle(GEOM_FieldStep) step = GetStep( stepID );
  if ( step.IsNull() )
    return;

  Handle(TDataStd_TreeNode) aNode =
    TDataStd_TreeNode::Set( step->GetEntry(), GetFieldID() );
  aNode->Remove(); // Removes this tree node attribute from its father

  // Dump of removed objects is not produced anayway
  //Handle(GEOM_Function) fun = AddFunction( GetFieldID(), FUN_REMOVE_STEP );
  //TPythonDump( fun ) << this << ".removeStep( " << stepID << " )";

  GEOM_Engine* anEngine = GEOM_Engine::GetEngine();
  if ( anEngine )
    anEngine->RemoveObject( step );
}

//=======================================================================
//function : GetStep
//purpose  : Returns a step
//=======================================================================

Handle(GEOM_FieldStep) GEOM_Field::GetStep(const int stepID)
{
  Handle(GEOM_FieldStep) step;

  Handle(TDataStd_TreeNode) aRoot, aNode;
  if ( !GetEntry().FindAttribute( GetFieldID(), aRoot ))
    return step;

  TDataStd_ChildNodeIterator anIter (aRoot);
  for (; anIter.More(); anIter.Next())
  {
    aNode = anIter.Value();
    step = GEOM_FieldStep::GetFieldStep( aNode->Label() );
    if ( !step.IsNull() && step->GetID() == stepID )
      return step;
  }
  return NULL;
}

//=======================================================================
//function : GetSteps
//purpose  : Returns all steps
//=======================================================================

std::list< Handle(GEOM_FieldStep)> GEOM_Field::GetSteps()
{
  std::list< Handle(GEOM_FieldStep) > stepList;

  Handle(TDataStd_TreeNode) aRoot, aNode;
  if ( !GetEntry().FindAttribute( GetFieldID(), aRoot ))
    return stepList;

  Handle(GEOM_FieldStep) step;
  TDataStd_ChildNodeIterator anIter (aRoot);
  for (; anIter.More(); anIter.Next())
  {
    aNode = anIter.Value();
    step = GEOM_FieldStep::GetFieldStep( aNode->Label() );
    if ( !step.IsNull() )
      stepList.push_back( step );
  }
  return stepList;
}

//=============================================================================
/*!
 *  Constructor: private
 */
//=============================================================================

GEOM_FieldStep::GEOM_FieldStep(const TDF_Label& theEntry)
  : GEOM_BaseObject(theEntry)
{
}

//=============================================================================
/*!
 *  Constructor: public
 */
//=============================================================================

GEOM_FieldStep::GEOM_FieldStep(const TDF_Label& theLabel, int /*type*/ )
  : GEOM_BaseObject( theLabel, GEOM_FIELD_STEP_OBJTYPE )
{
}

//================================================================================
/*!
 * \brief Sets the basic data
 */
//================================================================================

void GEOM_FieldStep::Init(const Handle(GEOM_Field)& theField,
                          const int                 theID,
                          const int                 theStamp)
{
  Handle(GEOM_Function) fun = GetFunction(1);
  if ( !fun.IsNull() )
    Standard_MultiplyDefined::Raise( "Reinitialization of GEOM_FieldStep is forbiden" );
  fun = AddFunction( GEOM_Field::GetFieldID(), GEOM_Field::FUN_ADD_STEP );

  GEOM_IField data( fun );
  data.SetField    ( theField->GetFunction(1) );
  data.SetStepID   ( theID );
  data.SetStepStamp( theStamp );
  // PythonDump to be done by the operation creating this field step
}

//=============================================================================
/*!
 *  GetField
 */
//=============================================================================

Handle(GEOM_FieldStep) GEOM_FieldStep::GetFieldStep(const TDF_Label& theLabel)
{
  Handle(GEOM_BaseObject) base = GEOM_BaseObject::GetObject(theLabel);
  return Handle(GEOM_FieldStep)::DownCast( base );
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================

GEOM_FieldStep::~GEOM_FieldStep()
{
}

//=======================================================================
//function : GetField
//purpose  : Returns the Field this GEOM_FieldStep belongs to
//=======================================================================

Handle(GEOM_Field) GEOM_FieldStep::GetField()
{
  Handle(GEOM_Field) field;

  Handle(GEOM_Function) fun= GetFunction(1);
  if ( !fun.IsNull() )
  {
    GEOM_IField data( fun );
    Handle(GEOM_Function) fldFun = data.GetField();
    if ( !fldFun.IsNull() )
      field = GEOM_Field::GetField( fldFun->GetOwnerEntry() );
  }
  return field;
}

//=======================================================================
//function : GetID
//purpose  : Returns the stamp step id
//=======================================================================

int GEOM_FieldStep::GetID()
{
  Handle(GEOM_Function) fun= GetFunction(1);
  if ( !fun.IsNull() )
    return GEOM_IField( fun ).GetStepID();
  return std::numeric_limits<int>::max();
}

//=======================================================================
//function : SetStamp
//purpose  : Sets the stamp of the step
//=======================================================================

void GEOM_FieldStep::SetStamp(const int stamp)
{
  if ( GetStamp() != stamp )
  {
    // it's stamp modification: field.setStamp(step, stamp)
    Handle(GEOM_Function) fun =
      AddFunction( GEOM_Field::GetFieldID(), GEOM_Field::FUN_CHANGE_STEP_STAMP );

    GEOM_IField data( fun );
    data.SetStepStamp( stamp );

    TPythonDump( fun ) <<
      GetField() << ".setStamp( " << GetID() << ", " << stamp << " )";
  }
}

//=======================================================================
//function : GetStamp
//purpose  : Returns the stamp of the step
//=======================================================================

int GEOM_FieldStep::GetStamp()
{
  // find the last function changing the stamp
  Handle(GEOM_Function) fun = getFunction( GEOM_Field::FUN_CHANGE_STEP_STAMP, this );
  if ( !fun.IsNull() )
    return GEOM_IField( fun ).GetStepStamp();
  return std::numeric_limits<int>::max();
}

//=======================================================================
//function : getFunctionToSetValues
//purpose  : Finds a function to store new values and dumps to Python
//=======================================================================

Handle(GEOM_Function)
GEOM_FieldStep::getFunctionToSetValuesAndDump( const TCollection_AsciiString& valueStr )
{
  Handle(GEOM_Function) fun = GetLastFunction();
  if ( fun->GetType() == GEOM_Field::FUN_ADD_STEP &&
       !fun->HasData( GEOM_IField::STEP_VALUES, GetDataID() ))
  {
    // it's creation of the step: field.addStep(step, stamp, values)
    GEOM_IField data( fun );
    TPythonDump( fun ) << this << " = " << GetField() << ".addStep( " <<
      data.GetStepID() << ", " << data.GetStepStamp() << ", " << valueStr << " )";
  }
  else
  {
    // it's value modification: field.setValues(step, values)
    fun = AddFunction( GEOM_Field::GetFieldID(), GEOM_Field::FUN_CHANGE_VALUE );
    GEOM_IField data( GetFunction(1) );
    TPythonDump( fun ) << GetField() << ".setValues( " <<
      data.GetStepID() << ", " << valueStr << " )";
  }
  return fun;
}

//=======================================================================
//function : SetValues
//purpose  : Set int or bool values
//=======================================================================

bool GEOM_FieldStep::SetValues( const Handle(TColStd_HArray1OfInteger)& values )
{
  Handle(GEOM_Field) field = GetField();
  if ( field.IsNull() ||
       values.IsNull() ||
       field->GetArrayLength() != values->Length() )
    return false;

  // fix bool values to be 0 or 1 only
  if ( field->GetDataType() == 0 )
    for ( int i = values->Lower(), nb = values->Upper(); i <= nb; ++i )
      values->SetValue( i , bool( values->Value( i )));

  Handle(GEOM_Function) fun =
    getFunctionToSetValuesAndDump( arrayToSting( values ));

  GEOM_IField data( fun );
  data.SetValues( values );
  return true;
}

//=======================================================================
//function : SetValues
//purpose  : Sets double values
//=======================================================================

bool GEOM_FieldStep::SetValues( const Handle(TColStd_HArray1OfReal)& values )
{
  Handle(GEOM_Field) field = GetField();
  if ( field.IsNull() ||
       values.IsNull() ||
       field->GetArrayLength() != values->Length() )
    return false;

  Handle(GEOM_Function) fun =
    getFunctionToSetValuesAndDump( arrayToSting( values ));

  GEOM_IField data( fun );
  data.SetValues( values );
  return true;
}

//=======================================================================
//function : SetValues
//purpose  : Sets string values
//=======================================================================

bool GEOM_FieldStep::SetValues( const Handle(TColStd_HArray1OfExtendedString)& values )
{
  Handle(GEOM_Field) field = GetField();
  if ( field.IsNull() ||
       values.IsNull() ||
       field->GetArrayLength() != values->Length() )
    return false;

  Handle(GEOM_Function) fun =
    getFunctionToSetValuesAndDump( arrayToSting( values, "'" ));

  GEOM_IField data( fun );
  data.SetValues( values );
  return true;
}

//=======================================================================
//function : GetIntValues
//purpose  : Returns int or bool values
//=======================================================================

Handle(TColStd_HArray1OfInteger) GEOM_FieldStep::GetIntValues()
{
  Handle(GEOM_Function) fun = getFunction( GEOM_Field::FUN_CHANGE_VALUE, this );
  if ( !fun.IsNull() )
    return GEOM_IField( fun ).GetIntValues();
  return NULL;
}

//=======================================================================
//function : GetDoubleValues
//purpose  : Returns double values
//=======================================================================

Handle(TColStd_HArray1OfReal) GEOM_FieldStep::GetDoubleValues()
{
  Handle(GEOM_Function) fun = getFunction( GEOM_Field::FUN_CHANGE_VALUE, this );
  if ( !fun.IsNull() )
    return GEOM_IField( fun ).GetDoubleValues();
  return NULL;
}

//=======================================================================
//function : GetStringValues
//purpose  : Returns string values
//=======================================================================

Handle(TColStd_HArray1OfExtendedString) GEOM_FieldStep::GetStringValues()
{
  Handle(GEOM_Function) fun = getFunction( GEOM_Field::FUN_CHANGE_VALUE, this );
  if ( !fun.IsNull() )
    return GEOM_IField( fun ).GetStringValues();
  return NULL;
}

//=======================================================================
//function : GetDataID
//purpose  : Returns GUID of CAF data array
//=======================================================================

const Standard_GUID& GEOM_FieldStep::GetDataID()
{
  int dataType = 2;
  Handle(GEOM_Field) f = GetField();
  if ( !f.IsNull() )
    dataType = f->GetDataType();

  switch ( dataType ) {
  case 0: // bool
  case 1: // int
    return TDataStd_IntegerArray::GetID();
  case 2: // double
    return TDataStd_RealArray::GetID();
  default:; // string
  }
  return TDataStd_ExtStringArray::GetID();
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT(GEOM_Field, GEOM_BaseObject );
OCCT_IMPLEMENT_STANDARD_RTTIEXT(GEOM_FieldStep, GEOM_BaseObject );
