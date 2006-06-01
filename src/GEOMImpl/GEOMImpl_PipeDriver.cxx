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

#include <GEOMImpl_PipeDriver.hxx>
#include <GEOMImpl_IPipe.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRep_Tool.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <GEOMImpl_IPipeDiffSect.hxx>

#include <Standard_NullObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_ConstructionError.hxx>
#include "utilities.h"
#include <TopExp_Explorer.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_PipeDriver::GetID()
{
  static Standard_GUID aPipeDriver("FF1BBB19-5D14-4df2-980B-3A668264EA16");
  return aPipeDriver;
}


//=======================================================================
//function : GEOMImpl_PipeDriver
//purpose  :
//=======================================================================
GEOMImpl_PipeDriver::GEOMImpl_PipeDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_PipeDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  GEOMImpl_IPipe* aCI= 0;
  Standard_Integer aType = aFunction->GetType();
  if(aType == PIPE_BASE_PATH)
    aCI = new GEOMImpl_IPipe(aFunction);
  else if(aType == PIPE_DIFFERENT_SECTIONS)
    aCI = new GEOMImpl_IPipeDiffSect(aFunction);
  else
    return 0;

  Handle(GEOM_Function) aRefPath = aCI->GetPath();
  TopoDS_Shape aShapePath = aRefPath->GetValue();


  if (aShapePath.IsNull())
  {
    cout<<"Driver : path is null"<<endl;
    if(aCI) delete aCI;
    Standard_NullObject::Raise("MakePipe aborted : null path argument");
  }
  
  // Get path contour
  TopoDS_Wire aWirePath;
  if (aShapePath.ShapeType() == TopAbs_WIRE) {
    aWirePath = TopoDS::Wire(aShapePath);
  } 
  else {
    if (aShapePath.ShapeType() == TopAbs_EDGE) {
         
        TopoDS_Edge anEdge = TopoDS::Edge(aShapePath);
        aWirePath = BRepBuilderAPI_MakeWire(anEdge);
    } 
    else {
      if(aCI) delete aCI;
      Standard_TypeMismatch::Raise("MakePipe aborted : path shape is neither a wire nor an edge");
    } 
  }
  
  TopoDS_Shape aShape;

  if (aType == PIPE_BASE_PATH) 
  {
     
    Handle(GEOM_Function) aRefBase = aCI->GetBase();
   
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    
    if (aShapeBase.IsNull()) {
      if(aCI) delete aCI;
      Standard_NullObject::Raise("MakePipe aborted : null base argument");
    }

    // Make pipe
    aShape = BRepOffsetAPI_MakePipe(aWirePath, aShapeBase);
  }

  //building pipe with different sections
  else if (aType == PIPE_DIFFERENT_SECTIONS) 
  {
    GEOMImpl_IPipeDiffSect* aCIDS = (GEOMImpl_IPipeDiffSect*)aCI;
    //GEOMImpl_IPipeDiffSect* aCIDS = static_cast<GEOMImpl_IPipeDiffSect*>(aCI);
    BRepOffsetAPI_MakePipeShell aBuilder(aWirePath);
    Handle(TColStd_HSequenceOfTransient) aBasesObjs = aCIDS->GetBases ();
    Handle(TColStd_HSequenceOfTransient) aLocObjs = aCIDS->GetLocations ();
    Standard_Boolean aWithContact = (aCIDS->GetWithContactMode());
    Standard_Boolean aWithCorrect = (aCIDS->GetWithCorrectionMode());

    Standard_Integer i =1, nbBases = aBasesObjs->Length(), 
      nbLocs = (aLocObjs.IsNull() ? 0 :aLocObjs->Length());
    
    if(nbLocs && nbLocs != nbBases)
    {
      if(aCI) delete aCI;
      Standard_ConstructionError::Raise("Number of sections is not equal to number of locations ");
    }
    TopTools_SequenceOfShape aSeqBases;
    TopTools_SequenceOfShape aSeqLocs;
    TopTools_SequenceOfShape aSeqFaces;
    for( ; i <= nbBases; i++) 
    {
      Handle(Standard_Transient) anItem = aBasesObjs->Value(i);
      if(anItem.IsNull())
	continue;
      Handle(GEOM_Function) aRefBase = Handle(GEOM_Function)::DownCast(anItem);
      if(aRefBase.IsNull())
	continue;
      TopoDS_Shape aShapeBase = aRefBase->GetValue();
      if(aShapeBase.IsNull())
	continue;
      TopAbs_ShapeEnum aTypeBase = aShapeBase.ShapeType();

      //if for section was specified face with a few wires then a few
      //    pipes were build and make solid 
      if(aTypeBase == TopAbs_FACE)
      {
	//for case one path should be used other type function
        aSeqFaces.Append(aShapeBase);
        TopExp_Explorer aExpW(aShapeBase,TopAbs_WIRE);
	for( ; aExpW.More(); aExpW.Next())
	{
	  TopoDS_Shape aWireProf = aExpW.Current();
	  aSeqBases.Append(aWireProf);
	}
      }
      else if(aTypeBase == TopAbs_WIRE || aTypeBase == TopAbs_VERTEX) 
        aSeqBases.Append(aShapeBase);
      else if(aTypeBase == TopAbs_EDGE)
      {
        TopoDS_Edge anEdge = TopoDS::Edge(aShapeBase);
	TopoDS_Shape aWireProf = BRepBuilderAPI_MakeWire(anEdge);
	aSeqBases.Append(aWireProf);
      }
      if(nbLocs) 
      {
        Handle(Standard_Transient) anItemLoc = aLocObjs->Value(i);
	if(anItemLoc.IsNull())
	  continue;
	Handle(GEOM_Function) aRefLoc = Handle(GEOM_Function)::DownCast(anItemLoc);
        TopoDS_Shape aShapeLoc = aRefLoc->GetValue();
	if(aShapeLoc.IsNull() || aShapeLoc.ShapeType() != TopAbs_VERTEX)
	  continue;
	aSeqLocs.Append(aShapeLoc);
      }
    }
    
    nbLocs = aSeqLocs.Length();
    Standard_Integer nbShapes = aSeqBases.Length();
    Standard_Integer step = nbShapes/nbBases;
    
    if(nbShapes < nbBases || fmod((double)nbShapes, (double)nbBases))
    {
      if(aCI) delete aCI;
      Standard_ConstructionError::Raise("Invalid sections were specified for building pipe");
    }
     
    Standard_Integer ind =0;	 
    for( i=1; i  <= nbShapes && ind < nbShapes; i++) //i+nbBases <= nbShapes
    {
      TopTools_SequenceOfShape usedBases;
      Standard_Integer j = 1;
      for( ; j <= nbBases ; j++) 
      {
	ind = i + (j-1)*step;
	
	 TopoDS_Shape aWireProf = aSeqBases.Value(ind);
	 usedBases.Append(aWireProf);
	 if(nbLocs) 
	 {
	   TopoDS_Shape aShapeLoc = aSeqLocs.Value(j);
           TopoDS_Vertex aVert = TopoDS::Vertex(aShapeLoc);
           aBuilder.Add(aWireProf,aVert,aWithContact,aWithCorrect);
	 }
	 else
           aBuilder.Add(aWireProf,aWithContact,aWithCorrect);
      }
      if(!aBuilder.IsReady())
      {
	if(aCI) delete aCI;
        Standard_ConstructionError::Raise("Invalid input data for building PIPE: bases are invalid");
      }
      aBuilder.Build();
      aShape = aBuilder.Shape();
      aSeqFaces.Append(aShape);	
      for( j = 1; j <=usedBases.Length(); j++)
        aBuilder.Delete(usedBases.Value(j));
    }
    	
    //for case if section is face 
    if(aSeqFaces.Length() >1)
    {
      BRep_Builder aB;
      TopoDS_Compound aComp;
      aB.MakeCompound(aComp);
      for( i = 1; i <= aSeqFaces.Length(); i++)
        aB.Add(aComp,aSeqFaces.Value(i));
      aShape = aComp;
    }
  }
  if (aShape.IsNull()) return 0;

  BRepCheck_Analyzer ana (aShape, Standard_False);
  if (!ana.IsValid()) {
    if(aCI) delete aCI;
    Standard_ConstructionError::Raise("Algorithm have produced an invalid shape result");
  }

  aFunction->SetValue(aShape);

  log.SetTouched(Label());
  if(aCI) delete aCI;
  return 1;
}


//=======================================================================
//function :  GEOMImpl_PipeDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_PipeDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_PipeDriver",
			                                 sizeof(GEOMImpl_PipeDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_PipeDriver) Handle(GEOMImpl_PipeDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_PipeDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_PipeDriver))) {
       _anOtherObject = Handle(GEOMImpl_PipeDriver)((Handle(GEOMImpl_PipeDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
