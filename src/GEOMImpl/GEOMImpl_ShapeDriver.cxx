
using namespace std;
#include "GEOMImpl_ShapeDriver.hxx"
#include "GEOMImpl_IShapes.hxx"
#include "GEOMImpl_IShapesOperations.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepAlgo_Sewing.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepTools_Quilt.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <Precision.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_ShapeDriver::GetID()
{
  static Standard_GUID aShapeDriver("FF1BBB54-5D14-4df2-980B-3A668264EA16");
  return aShapeDriver;
}


//=======================================================================
//function : GEOMImpl_ShapeDriver
//purpose  :
//=======================================================================
GEOMImpl_ShapeDriver::GEOMImpl_ShapeDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_ShapeDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IShapes aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;
  BRep_Builder B;

  if (aType == WIRE_EDGES) {
    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    unsigned int ind, nbshapes = aShapes->Length();

    // add edges
    BRepBuilderAPI_MakeWire MW;
    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("Shape for wire construction is null");
      }
      if (aShape_i.ShapeType() == TopAbs_EDGE)
	MW.Add(TopoDS::Edge(aShape_i));
      else if (aShape_i.ShapeType() == TopAbs_WIRE)
	MW.Add(TopoDS::Wire(aShape_i));
      else
        Standard_TypeMismatch::Raise
          ("Shape for wire construction is neither an edge nor a wire");
    }

    if (!MW.IsDone()) {
      Standard_ConstructionError::Raise("Wire construction failed");
    }
    aShape = MW;

  } else if (aType == FACE_WIRE) {
    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    if (aShapeBase.IsNull() || aShapeBase.ShapeType() != TopAbs_WIRE) {
      Standard_NullObject::Raise
        ("Shape for face construction is null or not a wire");
    }
    TopoDS_Wire W = TopoDS::Wire(aShapeBase);
    BRepBuilderAPI_MakeFace MF (W, aCI.GetIsPlanar());
    if (!MF.IsDone()) {
      Standard_ConstructionError::Raise("Face construction failed");
    }
    aShape = MF.Shape();

  } else if (aType == FACE_WIRES) {
    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();

    // first wire
    Handle(GEOM_Function) aRefWire = Handle(GEOM_Function)::DownCast(aShapes->Value(1));
    TopoDS_Shape aWire = aRefWire->GetValue();
    if (aWire.IsNull() || aWire.ShapeType() != TopAbs_WIRE) {
      Standard_NullObject::Raise("Shape for face construction is null or not a wire");
    }
    TopoDS_Wire W = TopoDS::Wire(aWire);

    // basic face
    BRepBuilderAPI_MakeFace MF (W, aCI.GetIsPlanar());
    if (!MF.IsDone()) {
      Standard_ConstructionError::Raise("Face construction failed");
    }
    TopoDS_Shape FFace = MF.Shape();
    if (!FFace.IsNull()) {
      unsigned int ind, nbshapes = aShapes->Length();
      if (nbshapes == 1) {
	aShape = FFace;

      } else if (nbshapes >= 2) {
	TopoDS_Compound C;
	BRep_Builder aBuilder;
	aBuilder.MakeCompound(C);
	BRepAlgo_FaceRestrictor FR;

	TopAbs_Orientation OriF = FFace.Orientation();
	TopoDS_Shape aLocalS = FFace.Oriented(TopAbs_FORWARD);
	FR.Init(TopoDS::Face(aLocalS), Standard_False, Standard_True);

	for (ind = 1; ind <= nbshapes; ind++) {
          Handle(GEOM_Function) aRefWire_i =
            Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
          TopoDS_Shape aWire_i = aRefWire_i->GetValue();
          if (aWire_i.IsNull() || aWire_i.ShapeType() != TopAbs_WIRE) {
            Standard_NullObject::Raise("Shape for face construction is null or not a wire");
          }

	  FR.Add(TopoDS::Wire(aWire_i));
	}

	FR.Perform();

	if (FR.IsDone()) {
	  int k = 0;
	  TopoDS_Shape aFace;
	  for (; FR.More(); FR.Next()) {
	    aFace = FR.Current().Oriented(OriF);
	    aBuilder.Add(C, aFace);
	    k++;
	  }
	  if (k == 1) {
	    aShape = aFace;
	  } else {
	    aShape = C;
	  }
	}
      }
    }
  } else if (aType == SHELL_FACES) {
    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    unsigned int ind, nbshapes = aShapes->Length();

    // add faces
    BRepAlgo_Sewing aSewing(Precision::Confusion()*10.0);
    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("Face for shell construction is null");
      }
      aSewing.Add(aShape_i);
    }

    aSewing.Perform();

    TopExp_Explorer exp (aSewing.SewedShape(), TopAbs_SHELL);
    Standard_Integer ish = 0;
    for (; exp.More(); exp.Next()) {
      aShape = exp.Current();
      ish++;
    }

    if (ish != 1)
      aShape = aSewing.SewedShape();

  } else if (aType == SOLID_SHELL) {
    Handle(GEOM_Function) aRefShell = aCI.GetBase();
    TopoDS_Shape aShapeShell = aRefShell->GetValue();
    if (aShapeShell.IsNull() || aShapeShell.ShapeType() != TopAbs_SHELL) {
      Standard_NullObject::Raise("Shape for solid construction is null or not a shell");
    }

    BRepCheck_Shell chkShell(TopoDS::Shell(aShapeShell));
    if(chkShell.Closed() == BRepCheck_NotClosed) return 0;

    TopoDS_Solid Sol;
    B.MakeSolid(Sol);
    B.Add(Sol, aShapeShell);
    BRepClass3d_SolidClassifier SC (Sol);
    SC.PerformInfinitePoint(Precision::Confusion());
    if (SC.State() == TopAbs_IN) {
      B.MakeSolid(Sol);
      B.Add(Sol, aShapeShell.Reversed());
    }

    aShape = Sol;

  } else if (aType == SOLID_SHELLS) {
    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    unsigned int ind, nbshapes = aShapes->Length();
    Standard_Integer ish = 0;
    TopoDS_Solid Sol;
    TopoDS_Compound Res;
    B.MakeCompound(Res);

    // add shapes
    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShapeShell = aRefShape->GetValue();
      if (aShapeShell.IsNull()) {
        Standard_NullObject::Raise("Shell for solid construction is null");
      }
      if (aShapeShell.ShapeType() == TopAbs_SHELL) {
        B.MakeSolid(Sol);
        B.Add(Sol, aShapeShell);
        BRepClass3d_SolidClassifier SC (Sol);
        SC.PerformInfinitePoint(Precision::Confusion());
        if (SC.State() == TopAbs_IN) {
          B.MakeSolid(Sol);
          B.Add(Sol, aShapeShell.Reversed());
        }
        B.Add(Res, Sol);
        ish++;
      }
    }
    if (ish == 1) aShape = Sol;
    else          aShape = Res;

  } else if (aType == COMPOUND_SHAPES) {
    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    unsigned int ind, nbshapes = aShapes->Length();

    // add shapes
    TopoDS_Compound C;
    B.MakeCompound(C);
    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("Shape for compound construction is null");
      }
      B.Add(C, aShape_i);
    }

    aShape = C;

  } else if (aType == REVERSE_ORIENTATION) {
    Handle(GEOM_Function) aRefShape = aCI.GetBase();
    TopoDS_Shape aShape_i = aRefShape->GetValue();
    if (aShape_i.IsNull()) {
       Standard_NullObject::Raise("Shape for reverse is null");
    }
  
    BRepBuilderAPI_Copy Copy(aShape_i);
    if( Copy.IsDone() ) {
      TopoDS_Shape tds = Copy.Shape();
      if( tds.IsNull() ) {
	Standard_ConstructionError::Raise("Orientation aborted : Can not reverse the shape");
      }

      if( tds.Orientation() == TopAbs_FORWARD)
	tds.Orientation(TopAbs_REVERSED) ;
      else
	tds.Orientation(TopAbs_FORWARD) ;

      aShape = tds;
    } 
  }

  if (aShape.IsNull()) return 0;

  // Check shape validity
  BRepCheck_Analyzer ana (aShape, false);
  if (!ana.IsValid()) {
    Standard_ConstructionError::Raise("Algorithm have produced an invalid shape result");
  }

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}


//=======================================================================
//function :  GEOMImpl_ShapeDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_ShapeDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_ShapeDriver",
			                                 sizeof(GEOMImpl_ShapeDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_ShapeDriver) Handle(GEOMImpl_ShapeDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_ShapeDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_ShapeDriver))) {
       _anOtherObject = Handle(GEOMImpl_ShapeDriver)((Handle(GEOMImpl_ShapeDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
