// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include <GEOMImpl_BooleanDriver.hxx>
#include <GEOMImpl_IBoolean.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_GlueDriver.hxx>
#include <GEOM_Function.hxx>
#include <GEOMUtils.hxx>

#include <TNaming_CopyShape.hxx>

#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeFix_Shape.hxx>

#include <BRep_Builder.hxx>
#include <BRepAlgo.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepCheck_Analyzer.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TColStd_IndexedDataMapOfTransientTransient.hxx>

#include <Precision.hxx>

#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_BooleanDriver::GetID()
{
  static Standard_GUID aBooleanDriver("FF1BBB21-5D14-4df2-980B-3A668264EA16");
  return aBooleanDriver;
}

//=======================================================================
//function : GEOMImpl_BooleanDriver
//purpose  :
//=======================================================================
GEOMImpl_BooleanDriver::GEOMImpl_BooleanDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_BooleanDriver::Execute (TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IBoolean aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  switch (aType) {
  case BOOLEAN_COMMON:
  case BOOLEAN_CUT:
  case BOOLEAN_FUSE:
  case BOOLEAN_SECTION:
    {
      Handle(GEOM_Function) aRefShape1 = aCI.GetShape1();
      Handle(GEOM_Function) aRefShape2 = aCI.GetShape2();
      TopoDS_Shape aShape1 = aRefShape1->GetValue();
      TopoDS_Shape aShape2 = aRefShape2->GetValue();

      if (!aShape1.IsNull() && !aShape2.IsNull()) {
        // check arguments for Mantis issue 0021019
        BRepCheck_Analyzer ana (aShape1, Standard_True);
        if (!ana.IsValid())
          StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");
        ana.Init(aShape2);
        if (!ana.IsValid())
          StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");

        aShape = performOperation (aShape1, aShape2, aType);

        if (aShape.IsNull())
          return 0;
      }
    }
    break;
  case BOOLEAN_COMMON_LIST:
  case BOOLEAN_FUSE_LIST:
    {
      Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
      const Standard_Integer nbShapes = aShapes->Length();
      Standard_Integer i;
      Handle(GEOM_Function) aRefShape;
      TopoDS_Shape aShape2;
      Standard_Integer aSimpleType =
        (aType == BOOLEAN_FUSE_LIST ? BOOLEAN_FUSE : BOOLEAN_COMMON);

      if (nbShapes > 0) {
        aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(1));
        aShape = aRefShape->GetValue();

        if (!aShape.IsNull()) {
          BRepCheck_Analyzer anAna (aShape, Standard_True);

          if (!anAna.IsValid()) {
            StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");
          }

          for (i = 2; i <= nbShapes; i++) {
            aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(i));
            aShape2 = aRefShape->GetValue();
            anAna.Init(aShape2);

            if (!anAna.IsValid()) {
              StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");
            }

            aShape = performOperation (aShape, aShape2, aSimpleType);

            if (aShape.IsNull()) {
              return 0;
            }
          }
        }
      }
    }
    break;
  case BOOLEAN_CUT_LIST:
    {
      Handle(GEOM_Function) aRefObject = aCI.GetShape1();

      aShape = aRefObject->GetValue();

      if (!aShape.IsNull()) {
        // check arguments for Mantis issue 0021019
        BRepCheck_Analyzer anAna (aShape, Standard_True);

        if (!anAna.IsValid()) {
          StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");
        }

        Handle(TColStd_HSequenceOfTransient) aTools = aCI.GetShapes();
        const Standard_Integer nbShapes = aTools->Length();
        Standard_Integer i;
        Handle(GEOM_Function) aRefTool;
        TopoDS_Shape aTool;

        for (i = 1; i <= nbShapes; i++) {
          aRefTool = Handle(GEOM_Function)::DownCast(aTools->Value(i));
          aTool = aRefTool->GetValue();
          anAna.Init(aTool);

          if (!anAna.IsValid()) {
            StdFail_NotDone::Raise("Boolean operation will not be performed, because argument shape is not valid");
          }

          aShape = performOperation (aShape, aTool, BOOLEAN_CUT);

          if (aShape.IsNull()) {
            return 0;
          }
        }
      }
    }
    break;
  default:
    break;
  }

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}

//=======================================================================
//function : performOperation
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_BooleanDriver::performOperation
                               (const TopoDS_Shape theShape1,
                                const TopoDS_Shape theShape2,
                                const Standard_Integer theType)const
{
  TopoDS_Shape aShape;

  // perform COMMON operation
  if (theType == BOOLEAN_COMMON) {
    BRep_Builder B;
    TopoDS_Compound C;
    B.MakeCompound(C);

    TopTools_ListOfShape listShape1, listShape2;
    GEOMUtils::AddSimpleShapes(theShape1, listShape1);
    GEOMUtils::AddSimpleShapes(theShape2, listShape2);

    Standard_Boolean isCompound =
      (listShape1.Extent() > 1 || listShape2.Extent() > 1);

    TopTools_ListIteratorOfListOfShape itSub1 (listShape1);
    for (; itSub1.More(); itSub1.Next()) {
      TopoDS_Shape aValue1 = itSub1.Value();
      TopTools_ListIteratorOfListOfShape itSub2 (listShape2);
      for (; itSub2.More(); itSub2.Next()) {
        TopoDS_Shape aValue2 = itSub2.Value();
        BRepAlgoAPI_Common BO (aValue1, aValue2);
        if (!BO.IsDone()) {
          StdFail_NotDone::Raise("Common operation can not be performed on the given shapes");
        }
        if (isCompound) {
          TopoDS_Shape aStepResult = BO.Shape();

          // check result of this step: if it is a compound (boolean operations
          // allways return a compound), we add all sub-shapes of it.
          // This allows to avoid adding empty compounds,
          // resulting from COMMON on two non-intersecting shapes.
          if (aStepResult.ShapeType() == TopAbs_COMPOUND) {
            TopoDS_Iterator aCompIter (aStepResult);
            for (; aCompIter.More(); aCompIter.Next()) {
              // add shape in a result
              B.Add(C, aCompIter.Value());
            }
          }
          else {
            // add shape in a result
            B.Add(C, aStepResult);
          }
        }
        else
          aShape = BO.Shape();
      }
    }

    if (isCompound) {
      // As GlueFaces has been improved to keep all kind of shapes
      TopExp_Explorer anExp (C, TopAbs_VERTEX);
      if (anExp.More())
        aShape = GEOMImpl_GlueDriver::GlueFaces(C, Precision::Confusion(), Standard_True);
      else
        aShape = C;
    }
  }

  // perform CUT operation
  else if (theType == BOOLEAN_CUT) {
    BRep_Builder B;
    TopoDS_Compound C;
    B.MakeCompound(C);

    TopTools_ListOfShape listShapes, listTools;
    GEOMUtils::AddSimpleShapes(theShape1, listShapes);
    GEOMUtils::AddSimpleShapes(theShape2, listTools);

    Standard_Boolean isCompound = (listShapes.Extent() > 1);

    TopTools_ListIteratorOfListOfShape itSub1 (listShapes);
    for (; itSub1.More(); itSub1.Next()) {
      TopoDS_Shape aCut = itSub1.Value();
      // tools
      TopTools_ListIteratorOfListOfShape itSub2 (listTools);
      for (; itSub2.More(); itSub2.Next()) {
        TopoDS_Shape aTool = itSub2.Value();
        BRepAlgoAPI_Cut BO (aCut, aTool);
        if (!BO.IsDone()) {
          StdFail_NotDone::Raise("Cut operation can not be performed on the given shapes");
        }
        aCut = BO.Shape();
      }
      if (isCompound) {
        // check result of this step: if it is a compound (boolean operations
        // allways return a compound), we add all sub-shapes of it.
        // This allows to avoid adding empty compounds,
        // resulting from CUT of parts
        if (aCut.ShapeType() == TopAbs_COMPOUND) {
          TopoDS_Iterator aCompIter (aCut);
          for (; aCompIter.More(); aCompIter.Next()) {
            // add shape in a result
            B.Add(C, aCompIter.Value());
          }
        }
        else {
          // add shape in a result
          B.Add(C, aCut);
        }
      }
      else
        aShape = aCut;
    }

    if (isCompound) {
      // As GlueFaces has been improved to keep all kind of shapes
      TopExp_Explorer anExp (C, TopAbs_VERTEX);
      if (anExp.More())
        aShape = GEOMImpl_GlueDriver::GlueFaces(C, Precision::Confusion(), Standard_True);
      else
        aShape = C;
    }
  }

  // perform FUSE operation
  else if (theType == BOOLEAN_FUSE) {
    // Perform
    BRepAlgoAPI_Fuse BO (theShape1, theShape2);
    if (!BO.IsDone()) {
      StdFail_NotDone::Raise("Fuse operation can not be performed on the given shapes");
    }
    aShape = BO.Shape();
  }

  // perform SECTION operation
  else if (theType == BOOLEAN_SECTION) {
    BRep_Builder B;
    TopoDS_Compound C;
    B.MakeCompound(C);

    TopTools_ListOfShape listShape1, listShape2;
    GEOMUtils::AddSimpleShapes(theShape1, listShape1);
    GEOMUtils::AddSimpleShapes(theShape2, listShape2);

    Standard_Boolean isCompound =
      (listShape1.Extent() > 1 || listShape2.Extent() > 1);

    TopTools_ListIteratorOfListOfShape itSub1 (listShape1);
    for (; itSub1.More(); itSub1.Next()) {
      TopoDS_Shape aValue1 = itSub1.Value();
      TopTools_ListIteratorOfListOfShape itSub2 (listShape2);
      for (; itSub2.More(); itSub2.Next()) {
        TopoDS_Shape aValue2 = itSub2.Value();
        BRepAlgoAPI_Section BO (aValue1, aValue2, Standard_False);
        // Set approximation to have an attached 3D BSpline geometry to each edge,
        // where analytic curve is not possible. Without this flag in some cases
        // we obtain BSpline curve of degree 1 (C0), which is slowly
        // processed by some algorithms (Partition for example).
        BO.Approximation(Standard_True);
        //modified by NIZNHY-PKV Tue Oct 18 14:34:16 2011f
        BO.ComputePCurveOn1(Standard_True);
        BO.ComputePCurveOn2(Standard_True);
        //modified by NIZNHY-PKV Tue Oct 18 14:34:18 2011t
  
        BO.Build();
        if (!BO.IsDone()) {
          StdFail_NotDone::Raise("Section operation can not be performed on the given shapes");
        }
        if (isCompound) {
          TopoDS_Shape aStepResult = BO.Shape();

          // check result of this step: if it is a compound (boolean operations
          // allways return a compound), we add all sub-shapes of it.
          // This allows to avoid adding empty compounds,
          // resulting from SECTION on two non-intersecting shapes.
          if (aStepResult.ShapeType() == TopAbs_COMPOUND) {
            TopoDS_Iterator aCompIter (aStepResult);
            for (; aCompIter.More(); aCompIter.Next()) {
              // add shape in a result
              B.Add(C, aCompIter.Value());
            }
          }
          else {
            // add shape in a result
            B.Add(C, aStepResult);
          }
        }
        else
          aShape = BO.Shape();
      }
    }

    if (isCompound) {
      // As GlueFaces has been improved to keep all kind of shapes
      TopExp_Explorer anExp (C, TopAbs_VERTEX);
      if (anExp.More())
        aShape = GEOMImpl_GlueDriver::GlueFaces(C, Precision::Confusion(), Standard_True);
      else
        aShape = C;
    }
  }

  // UNKNOWN operation
  else {
  }

  if (aShape.IsNull()) return aShape;

  // as boolean operations always produce compound, lets simplify it
  // for the case, if it contains only one sub-shape
  TopTools_ListOfShape listShapeRes;
  GEOMUtils::AddSimpleShapes(aShape, listShapeRes);
  if (listShapeRes.Extent() == 1) {
    aShape = listShapeRes.First();
    if (aShape.IsNull()) return aShape;
  }

  // 08.07.2008 skl for bug 19761 from Mantis
  BRepCheck_Analyzer ana (aShape, Standard_True);
  ana.Init(aShape);
  if (!ana.IsValid()) {
    ShapeFix_ShapeTolerance aSFT;
    aSFT.LimitTolerance(aShape, Precision::Confusion(),
                        Precision::Confusion(), TopAbs_SHAPE);
    Handle(ShapeFix_Shape) aSfs = new ShapeFix_Shape(aShape);
    aSfs->Perform();
    aShape = aSfs->Shape();
    ana.Init(aShape);
    if (!ana.IsValid())
      Standard_ConstructionError::Raise("Boolean operation aborted : non valid shape result");
  }

  // BEGIN: Mantis issue 0021060: always limit tolerance of BOP result
  // 1. Get shape parameters for comparison
  int nbTypes [TopAbs_SHAPE];
  {
    for (int iType = 0; iType < TopAbs_SHAPE; ++iType)
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
  }

  // 2. Limit tolerance
  TopoDS_Shape aShapeCopy;
  TColStd_IndexedDataMapOfTransientTransient aMapTShapes;
  TNaming_CopyShape::CopyTool(aShape, aMapTShapes, aShapeCopy);
  ShapeFix_ShapeTolerance aSFT;
  aSFT.LimitTolerance(aShapeCopy, Precision::Confusion(), Precision::Confusion(), TopAbs_SHAPE);
  Handle(ShapeFix_Shape) aSfs = new ShapeFix_Shape (aShapeCopy);
  aSfs->Perform();
  aShapeCopy = aSfs->Shape();

  // 3. Check parameters
  ana.Init(aShapeCopy);
  if (ana.IsValid()) {
    int iType, nbTypesCopy [TopAbs_SHAPE];

    for (iType = 0; iType < TopAbs_SHAPE; ++iType)
      nbTypesCopy[iType] = 0;
    nbTypesCopy[aShapeCopy.ShapeType()]++;

    TopTools_MapOfShape aMapOfShape;
    aMapOfShape.Add(aShapeCopy);
    TopTools_ListOfShape aListOfShape;
    aListOfShape.Append(aShapeCopy);

    TopTools_ListIteratorOfListOfShape itL (aListOfShape);
    for (; itL.More(); itL.Next()) {
      TopoDS_Iterator it (itL.Value());
      for (; it.More(); it.Next()) {
        TopoDS_Shape s = it.Value();
        if (aMapOfShape.Add(s)) {
          aListOfShape.Append(s);
          nbTypesCopy[s.ShapeType()]++;
        }
      }
    }

    bool isEqual = true;
    for (iType = 0; iType < TopAbs_SHAPE && isEqual; ++iType) {
      if (nbTypes[iType] != nbTypesCopy[iType])
        isEqual = false;
    }
    if (isEqual)
      aShape = aShapeCopy;
  }
  // END: Mantis issue 0021060

  return aShape;
}

//=======================================================================
//function :  GEOMImpl_BooleanDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_BooleanDriver_Type_()
{
  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_BooleanDriver",
                                                         sizeof(GEOMImpl_BooleanDriver),
                                                         1,
                                                         (Standard_Address)_Ancestors,
                                                         (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_BooleanDriver) Handle(GEOMImpl_BooleanDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_BooleanDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
    if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_BooleanDriver))) {
      _anOtherObject = Handle(GEOMImpl_BooleanDriver)((Handle(GEOMImpl_BooleanDriver)&)AnObject);
    }
  }

  return _anOtherObject;
}
