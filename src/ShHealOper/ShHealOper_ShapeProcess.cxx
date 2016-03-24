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

// File:      ShHealOper_ShapeProcess.cxx
// Created:   14.04.04 10:58:04
// Author:    Galina KULIKOVA
//
#include <ShHealOper_ShapeProcess.hxx>
#include <ShapeProcessAPI_ApplySequence.hxx>
#include <ShapeProcess_Context.hxx>
#include <TCollection_AsciiString.hxx>
#include <Resource_Manager.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <Precision.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Iterator.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <TNaming_CopyShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <ShapeExtend_MsgRegistrator.hxx>
#include <ShapeExtend_DataMapOfShapeListOfMsg.hxx>
#include <ShapeExtend_DataMapIteratorOfDataMapOfShapeListOfMsg.hxx>
#include <Message_ListOfMsg.hxx>
#include <Message_ListIteratorOfListOfMsg.hxx>
#include <Message_Msg.hxx>

//=======================================================================
//function : ShHealOper_ShapeProcess()
//purpose  : Constructor
//=======================================================================

ShHealOper_ShapeProcess::ShHealOper_ShapeProcess (  ) :
       myOperations(TCollection_AsciiString("ShHealing").ToCString(),
                    TCollection_AsciiString("ShapeProcess").ToCString())
{
   //myResource = new Resource_Manager("ShHealing");
   myPrefix = "ShapeProcess";
   mySaveHistoryMode = Standard_False;
   myLevel = TopAbs_EDGE;
   myDone = Standard_False;
   myOperations.Context()->SetDetalisation ( TopAbs_EDGE );
}

//=======================================================================
//function : ShHealOper_ShapeProcess
//purpose  : 
//=======================================================================

ShHealOper_ShapeProcess::ShHealOper_ShapeProcess (const TCollection_AsciiString& theNameResource,
                                                  const TCollection_AsciiString& thePrefix ) :
  myOperations(theNameResource.ToCString(),thePrefix.ToCString())
{
  //myResource = new Resource_Manager(theNameResource);
  myPrefix = thePrefix;
  mySaveHistoryMode = Standard_False;
  myLevel = TopAbs_EDGE;
  myDone = Standard_False;
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void ShHealOper_ShapeProcess::Perform(const TopoDS_Shape& theOldShape,
                                      TopoDS_Shape&       theNewShape)
{
  myMapModifications.Clear();
  //ShapeProcessAPI_ApplySequence aOperations(myResource,myPrefix.ToCString());
  //myDone = Standard_False;
  myOperations.ClearMap();
  ShapeAnalysis_ShapeTolerance aSatol;
  Standard_Real ainitTol = aSatol.Tolerance(theOldShape,0);

  // PAL6487: san -- preserve the original shape from being modified
  TopoDS_Shape anOldShape;
  TColStd_IndexedDataMapOfTransientTransient aMap;
  TNaming_CopyShape::CopyTool(theOldShape, aMap, anOldShape);
  // PAL6487: san -- preserve the original shape from being modified

  theNewShape = myOperations.PrepareShape(anOldShape,mySaveHistoryMode,myLevel);
  if(mySaveHistoryMode)
    myMapModifications = myOperations.Map();
  myDone = !anOldShape.IsSame(theNewShape);
  if(!myDone) {
    Standard_Real aendTol =aSatol.Tolerance(theNewShape,0);
    myDone = (fabs(ainitTol - aendTol) > Precision::Confusion());
    if ( myDone ) {
      if ( ainitTol > aendTol )
        myStatistics.AddModif( "Tolerance fixed (decreased)" );
      else
        myStatistics.AddModif( "Tolerance fixed (increased)" );
    }
  }

  // fill myStatistics with messages
  Handle(ShapeExtend_MsgRegistrator) msg = myOperations.Context()->Messages();
  const ShapeExtend_DataMapOfShapeListOfMsg& shape2msg = msg->MapShape();
  ShapeExtend_DataMapIteratorOfDataMapOfShapeListOfMsg s2msg( shape2msg );
  for ( ; s2msg.More(); s2msg.Next() )
  {
    const Message_ListOfMsg & msgList = s2msg.Value();
    Message_ListIteratorOfListOfMsg mIt( msgList );
    for ( ; mIt.More(); mIt.Next() )
    {
      Message_Msg& m = mIt.Value();
      TCollection_AsciiString txt = m.Get();
      myStatistics.AddModif( txt.ToCString() );
    }
  }
}

//=======================================================================
//function : SetOperators
//purpose  :
//=======================================================================

void ShHealOper_ShapeProcess::SetOperators(const TColStd_SequenceOfAsciiString& theSeqOperators)
{
  
  TCollection_AsciiString anameParam(myPrefix);
  anameParam += ".";
  anameParam += "exec.op";
  TCollection_AsciiString aseq;
  Standard_Integer i =1;
  for( ; i <= theSeqOperators.Length(); i++) {
    aseq += theSeqOperators.Value(i);
    aseq += " ";
  }
    
  myOperations.Context()->ResourceManager()->SetResource(anameParam.ToCString(),aseq.ToCString());
}
//=======================================================================
//function : SetParameter
//purpose  : 
//=======================================================================

void ShHealOper_ShapeProcess::SetParameter(const TCollection_AsciiString& theNameParam,
                                           const TCollection_AsciiString& theVal)
{
  TCollection_AsciiString anameParam(myPrefix);
  anameParam += ".";
  anameParam+= theNameParam;
  if(theVal.IsRealValue())
    myOperations.Context()->ResourceManager()->
      SetResource(anameParam.ToCString(),theVal.RealValue());
  else if(theVal.IsIntegerValue())
    myOperations.Context()->ResourceManager()->
      SetResource(anameParam.ToCString(),theVal.IntegerValue());
  else
    myOperations.Context()->ResourceManager()->
      SetResource(anameParam.ToCString(),theVal.ToCString());

}
//=======================================================================
//function : GetOperators
//purpose  : 
//=======================================================================

Standard_Boolean ShHealOper_ShapeProcess::GetOperators(TColStd_SequenceOfAsciiString& theSeqOperators)
{
  TCollection_AsciiString nameSeq(myPrefix);
  nameSeq +=".exec.op";
  if(!myOperations.Context()->ResourceManager()->Find(nameSeq.ToCString()))
    return Standard_False;
  
  TCollection_AsciiString seqoper = myOperations.Context()->ResourceManager()->Value(nameSeq.ToCString());
  Standard_Integer i;
  for ( i=1; ; i++ ) {
    TCollection_AsciiString anOper = seqoper.Token ( " \t,;", i );
    if ( anOper.Length() <=0 ) break;
    theSeqOperators.Append(anOper);
  }
  return (theSeqOperators.Length());
}
//=======================================================================
//function : GetParameter
//purpose  : 
//=======================================================================

Standard_Boolean ShHealOper_ShapeProcess::GetParameter(const TCollection_AsciiString& theNameParam,
                                                       TCollection_AsciiString& theVal)
{
  TCollection_AsciiString namePar(myPrefix);
  namePar += ".";
  namePar += theNameParam;
  if(!myOperations.Context()->ResourceManager()->Find(namePar.ToCString()))
    return Standard_False;

  theVal = myOperations.Context()->ResourceManager()->Value(namePar.ToCString());
  return Standard_True;
}
