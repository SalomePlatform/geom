// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include <Standard_Stream.hxx>

#include <GEOM_Object.hxx>
#include <GEOM_Engine.hxx>
#include <GEOM_Solver.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Data.hxx>
#include <TDF_Reference.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDocStd_Owner.hxx>
#include <TDocStd_Document.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Comment.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>

#define TYPE 2
#define FUNCTION_LABEL(theNb) (_label.FindChild(1).FindChild((theNb)))
#define FREE_LABEL 3

//=======================================================================
//function : GetObjectID
//purpose  :
//=======================================================================
const Standard_GUID& GEOM_Object::GetObjectID()
{
  static Standard_GUID anObjectID("FF1BBB01-5D14-4df2-980B-3A668264EA16");
  return anObjectID;
}    

//=======================================================================
//function : GetSubShapeID
//purpose  :
//=======================================================================
const Standard_GUID& GEOM_Object::GetSubShapeID()
{
  static Standard_GUID anObjectID("FF1BBB68-5D14-4df2-980B-3A668264EA16");
  return anObjectID;
}
              
//=============================================================================
/*!
 *  GetObject
 */
//=============================================================================
Handle(GEOM_Object) GEOM_Object::GetObject(TDF_Label& theLabel)
{
  if (!theLabel.IsAttribute(GetObjectID())) return NULL;

  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theLabel, anEntry);

  Handle(TDocStd_Document) aDoc = TDocStd_Owner::GetDocument(theLabel.Data());
  if(aDoc.IsNull()) return NULL;

  Handle(TDataStd_Integer) anID;
  if(!aDoc->Main().FindAttribute(TDataStd_Integer::GetID(), anID)) return NULL;
  

  GEOM_Engine* anEngine=  GEOM_Engine::GetEngine();
  if(anEngine == NULL) return NULL;
  return anEngine->GetObject(anID->Get(), anEntry.ToCString());
}

//=============================================================================
/*!
 *  GetReferencedObject
 */
//=============================================================================
Handle(GEOM_Object) GEOM_Object::GetReferencedObject(TDF_Label& theLabel)
{
  Handle(TDF_Reference) aRef;
  if (!theLabel.FindAttribute(TDF_Reference::GetID(), aRef)) return NULL;

  // Get TreeNode of a referenced function
  Handle(TDataStd_TreeNode) aT, aFather;
  if (!TDataStd_TreeNode::Find(aRef->Get(), aT)) return NULL;

  // Get TreeNode of Object of the referenced function
  aFather = aT->Father();
  if (aFather.IsNull()) return NULL;

  // Get label of the referenced object
  TDF_Label aLabel = aFather->Label();

  
  return GEOM_Object::GetObject(aLabel);
}

//=============================================================================
/*!
 *  Constructor: private
 */
//=============================================================================
GEOM_Object::GEOM_Object(TDF_Label& theEntry)
: _label(theEntry), _ior("") 
{
  if(!theEntry.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), _root)) 
    _root = TDataStd_TreeNode::Set(theEntry);
}

//=============================================================================
/*!
 *  Constructor: public
 */
//=============================================================================
GEOM_Object::GEOM_Object(TDF_Label& theEntry, int theType)
: _label(theEntry), _ior("") 
{
  theEntry.ForgetAllAttributes(Standard_True);

  if(!theEntry.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), _root)) 
    _root = TDataStd_TreeNode::Set(theEntry);

  TDataStd_Integer::Set(theEntry.FindChild(TYPE), theType);

  TDataStd_UAttribute::Set(theEntry, GetObjectID());
}

//=============================================================================
/*!
 *  GetType
 */
//=============================================================================
int GEOM_Object::GetType()
{
  Handle(TDataStd_Integer) aType;
  if(!_label.FindChild(TYPE).FindAttribute(TDataStd_Integer::GetID(), aType)) return -1;
  
  return aType->Get();
}

//=============================================================================
/*!
 *  SetType
 */
//=============================================================================
void GEOM_Object::SetType(int theType)
{
  TDataStd_Integer::Set(_label.FindChild(TYPE), theType);
  return;
}


//=============================================================================
/*!
 *  GetDocID
 */
//=============================================================================
int GEOM_Object::GetDocID()
{
  Handle(TDocStd_Document) aDoc = TDocStd_Owner::GetDocument(_label.Data());
  if(aDoc.IsNull()) return -1;

  Handle(TDataStd_Integer) anID;
  if(!aDoc->Main().FindAttribute(TDataStd_Integer::GetID(), anID)) return -1;
  
  return anID->Get();
}


//=============================================================================
/*!
 *  GetValue
 */
//=============================================================================
TopoDS_Shape GEOM_Object::GetValue()
{
  TopoDS_Shape aShape;

  Handle(GEOM_Function) aFunction = GetLastFunction();

  if (!aFunction.IsNull())
    aShape = aFunction->GetValue();
  
  return aShape;
}

//=============================================================================
/*!
 *  SetName
 */
//=============================================================================
void GEOM_Object::SetName(const char* theName)
{
  TDataStd_Name::Set(_label, (char*)theName);
}

//=============================================================================
/*!
 *  GetName
 */
//=============================================================================
char* GEOM_Object::GetName()
{
  Handle(TDataStd_Name) aNameAttr;
  if(!_label.FindAttribute(TDataStd_Name::GetID(), aNameAttr)) return NULL;
  
  TCollection_AsciiString aName(aNameAttr->Get());
  return aName.ToCString();
}

//=============================================================================
/*!
 *  SetAuxData
 */
//=============================================================================
void GEOM_Object::SetAuxData(const char* theData)
{
  TDataStd_Comment::Set(_label, (char*)theData);
}

//=============================================================================
/*!
 *  GetAuxData
 */
//=============================================================================
TCollection_AsciiString GEOM_Object::GetAuxData()
{
  TCollection_AsciiString aData;

  Handle(TDataStd_Comment) aCommentAttr;
  if (_label.FindAttribute(TDataStd_Comment::GetID(), aCommentAttr))
    aData = aCommentAttr->Get();

  return aData;
}


//=============================================================================
/*!
 *  IsSubShape
 */
//============================================================================= 
bool GEOM_Object::IsMainShape()
{
  Handle(GEOM_Function) aFunction = GetFunction(1);
  if(aFunction.IsNull() || aFunction->GetDriverGUID() != GetSubShapeID()) return true; // mkr : IPAL9921
  return false;
}


//=============================================================================
/*!
 *  AddFunction
 */
//=============================================================================
Handle(GEOM_Function) GEOM_Object::AddFunction(const Standard_GUID& theGUID, int theFunctionType)
{
  Standard_Integer nb = GetNbFunctions();
  if(nb == 1 && theGUID == GetSubShapeID()) return NULL; //It's impossible to add a function to sub shape
  nb++;
  TDF_Label aChild = FUNCTION_LABEL(nb);

  Handle(TDataStd_TreeNode) aNode = TDataStd_TreeNode::Set(aChild);
  _root->Append(aNode);

  Handle(GEOM_Function) aFunction = new GEOM_Function(aChild, theGUID, theFunctionType);

  return aFunction;

}

//=============================================================================
/*!
 *  GetNbFunctions
 */
//=============================================================================
int GEOM_Object::GetNbFunctions()
{
  Standard_Integer nb = 0;
  for(TDataStd_ChildNodeIterator CI(_root); CI.More(); CI.Next()) nb++;
  return nb;
}

//=============================================================================
/*!
 *  GetFunction
 */
//=============================================================================
Handle(GEOM_Function) GEOM_Object::GetFunction(int theFunctionNumber)
{
  TDF_Label aChild = FUNCTION_LABEL(theFunctionNumber);
  return GEOM_Function::GetFunction(aChild);
}

//=============================================================================
/*!
 *  GetlastFunction
 */
//=============================================================================
Handle(GEOM_Function) GEOM_Object::GetLastFunction()
{
  Standard_Integer nb = GetNbFunctions();
  if(nb) return GetFunction(nb);
 
  return NULL;
}


//=============================================================================
/*!
 *  GetAllDependency
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOM_Object::GetAllDependency()
{
  Handle(TColStd_HSequenceOfTransient) anArray;
  TDF_LabelSequence aSeq;
  Standard_Integer nb = GetNbFunctions();
  if(nb == 0) return anArray;
  for(Standard_Integer i=1; i<=nb; i++) {
    Handle(GEOM_Function) aFunction = GetFunction(i);
    if(aFunction.IsNull()) continue;
    aFunction->GetDependency(aSeq);
  }

  Standard_Integer aLength = aSeq.Length();
  if(aLength > 0) {    
    anArray = new TColStd_HSequenceOfTransient;
    for(Standard_Integer j =1; j<=aLength; j++)
      anArray->Append(GetReferencedObject(aSeq(j)));
  }
  
  return anArray;
}

//=============================================================================
/*!
 *  GetLastDependency
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOM_Object::GetLastDependency()
{
  Handle(TColStd_HSequenceOfTransient) anArray;
  Handle(GEOM_Function) aFunction = GetLastFunction();
  if (aFunction.IsNull()) return anArray;

  TDF_LabelSequence aSeq;
  aFunction->GetDependency(aSeq);
  Standard_Integer aLength = aSeq.Length();
  if (aLength > 0) {
    anArray = new TColStd_HSequenceOfTransient;
    for (Standard_Integer i = 1; i <= aLength; i++)
      anArray->Append(GetReferencedObject(aSeq(i)));
  }

  return anArray;
}

//=============================================================================
/*!
 *  GetFreeLabel
 */
//=============================================================================
TDF_Label GEOM_Object::GetFreeLabel()
{
  return _label.FindChild(FREE_LABEL);
}

//=======================================================================
//function :  GEOM_Object_Type_
//purpose  :
//======================================================================= 
Standard_EXPORT Handle_Standard_Type& GEOM_Object_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(MMgt_TShared);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(MMgt_TShared); 
  static Handle_Standard_Type aType2 = STANDARD_TYPE(Standard_Transient);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOM_Object",
			                                 sizeof(GEOM_Object),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);
  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//======================================================================= 

const Handle(GEOM_Object) Handle(GEOM_Object)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOM_Object) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOM_Object))) {
       _anOtherObject = Handle(GEOM_Object)((Handle(GEOM_Object)&)AnObject);
     }
  }

  return _anOtherObject ;
}


