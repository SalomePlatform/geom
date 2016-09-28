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

#include <Standard_Stream.hxx>

#include <GEOMImpl_FillingDriver.hxx>
#include <GEOMImpl_IFilling.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOM_Function.hxx>

#include <Basics_OCCTVersion.hxx>

#include <BRepAlgo.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepOffsetAPI_MakeFilling.hxx>

#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <Precision.hxx>
#include <ShapeFix_Face.hxx>

#include <TColGeom_SequenceOfCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>

#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_FillingDriver::GetID()
{
  static Standard_GUID aFillingDriver ("FF1BBB62-5D14-4df2-980B-3A668264EA16");
  return aFillingDriver;
}

//=======================================================================
//function : GEOMImpl_FillingDriver
//purpose  :
//=======================================================================

GEOMImpl_FillingDriver::GEOMImpl_FillingDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_FillingDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  if (aFunction.IsNull()) return 0;
  TopoDS_Shape aShape;
  GEOMImpl_IFilling IF (aFunction);
  if( aFunction->GetType() == BASIC_FILLING )
  {
    GEOMImpl_IFilling IF (aFunction);
    Standard_Integer   mindeg = IF.GetMinDeg();
    Standard_Integer   maxdeg = IF.GetMaxDeg();
    Standard_Real       tol3d = IF.GetTol3D();
    Standard_Boolean isApprox = IF.GetApprox();

    if (mindeg > maxdeg) {
      Standard_RangeError::Raise("Minimal degree can not be more than maximal degree");
    }

    /* we verify the contents of the shape */
    TopExp_Explorer Ex;
    TopoDS_Shape Scurrent;
    Standard_Real First, Last;
    Handle(Geom_Curve) C;

    TopoDS_Compound aComp;
    BRep_Builder B;
    B.MakeCompound(aComp);

    // input is either a list or compound of contours
    TopTools_SequenceOfShape contours;
    Handle(TColStd_HSequenceOfTransient) aShapeFunctions = IF.GetShapes();
    if ( aShapeFunctions.IsNull() || aShapeFunctions->IsEmpty() ) return 0;
    for ( int i = 1; i <= aShapeFunctions->Length(); ++i )
    {
      Handle(GEOM_Function) fun = Handle(GEOM_Function)::DownCast( aShapeFunctions->Value( i ));
      if ( fun.IsNull() ) return 0;
      TopoDS_Shape s = fun->GetValue();
      if ( s.IsNull() ) return 0;
      BRepBuilderAPI_Copy Copy (s);
      if ( Copy.IsDone() )
        contours.Append( Copy.Shape() );
    }

    // 1. Convert argument wires, if any, into BSpline edges
    for ( int i = 1; i <= contours.Length(); ++i )
    {
      Scurrent = contours.Value( i );
      if (Scurrent.ShapeType() != TopAbs_EDGE) {
      
        if (Scurrent.ShapeType() == TopAbs_WIRE)
        {
          const TopoDS_Wire& CurWire = TopoDS::Wire(Scurrent);
          TopoDS_Edge NewEdge = BRepAlgo::ConcatenateWireC0(CurWire);
          if (NewEdge.IsNull())
            Standard_ConstructionError::Raise("Failed to join several edges into one");
          Scurrent = NewEdge;
        }
        else if (Scurrent.ShapeType() == TopAbs_COMPOUND)
        {
          for ( TopoDS_Iterator It( Scurrent ); It.More(); It.Next() )
            contours.Append( It.Value() );
          continue;
        }
        else
        {
          Standard_ConstructionError::Raise("Input must contain only edges or/and wires");
        }
      }
      B.Add(aComp,Scurrent);
    }
    aShape = aComp;

    // 2. The surface construction
    if (!isApprox) {
      // make filling as in old version of SALOME (before 4.1.1)

      Standard_Real      tol2d = IF.GetTol2D();
      Standard_Integer  nbiter = IF.GetNbIter();
      Standard_Integer aMethod = IF.GetMethod();

      GeomFill_SectionGenerator Section;
      Standard_Integer aNumSection = 0;
      Handle(Geom_Curve) aLastC;
      gp_Pnt PL1,PL2;
      for (Ex.Init(aShape, TopAbs_EDGE); Ex.More(); Ex.Next()) {
        Scurrent = Ex.Current();
        if (Scurrent.IsNull() || Scurrent.ShapeType() != TopAbs_EDGE) return 0;
        if (BRep_Tool::Degenerated(TopoDS::Edge(Scurrent))) continue;
        C = BRep_Tool::Curve(TopoDS::Edge(Scurrent), First, Last);
        //if (Scurrent.Orientation() == TopAbs_REVERSED)
        //  // Mantis isuue 0020659: consider the orientation of the edges
        //  C = new Geom_TrimmedCurve(C, Last, First);
        //else
        //  C = new Geom_TrimmedCurve(C, First, Last);
        C = new Geom_TrimmedCurve(C, First, Last);
        gp_Pnt P1,P2;
        C->D0(First,P1);
        C->D0(Last,P2);

        if (aMethod == 1 && Scurrent.Orientation() == TopAbs_REVERSED) {
          C->Reverse();
        }
        else if (aMethod == 2) {
          if (aNumSection == 0) {
            PL1 = P1;
            PL2 = P2;
          }
          else {
            double d1 = PL1.Distance(P1) + PL2.Distance(P2);
            double d2 = PL1.Distance(P2) + PL2.Distance(P1);
            if (d2 < d1) {
              C->Reverse();
              PL1 = P2;
              PL2 = P1;
            }
            else {
              PL1 = P1;
              PL2 = P2;
            }
          }
        }

        Section.AddCurve(C);
        aNumSection++;
      }

      /* a 'tolerance' is used to compare 2 knots : see GeomFill_Generator.cdl */
      Section.Perform(Precision::PConfusion());
      //imn: to fix the bug 23351: "Crash is filling has only one edge"
      //after fix related occt bug 27875 must be removed
      if (aNumSection < 2) {
        Standard_ConstructionError::Raise("Input must contain more than one edge");
      }
      Handle(GeomFill_Line) Line = new GeomFill_Line(aNumSection);

      GeomFill_AppSurf App (mindeg, maxdeg, tol3d, tol2d, nbiter); /* user parameters */
      App.Perform(Line, Section);

      if (!App.IsDone()) return 0;
      Standard_Integer UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots;
      App.SurfShape(UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots);
      Handle(Geom_BSplineSurface) GBS = new Geom_BSplineSurface
        (App.SurfPoles(), App.SurfWeights(), App.SurfUKnots(), App.SurfVKnots(),
         App.SurfUMults(), App.SurfVMults(), App.UDegree(), App.VDegree());

      if (GBS.IsNull()) return 0;
      aShape = BRepBuilderAPI_MakeFace(GBS, Precision::Confusion());
    }
    else {
      // implemented by skl 20.03.2008 for bug 16568
      // make approximation - try to create bspline surface
      // using GeomAPI_PointsToBSplineSurface

      TColGeom_SequenceOfCurve aSeq;
      int MaxNbPoles = 0;

      // add curves from edges to sequence and find maximal
      // number of poles if some of them are bsplines
      for (Ex.Init(aShape, TopAbs_EDGE); Ex.More(); Ex.Next()) {
        Scurrent = Ex.Current();
        if (Scurrent.IsNull() || Scurrent.ShapeType() != TopAbs_EDGE) return 0;
        if (BRep_Tool::Degenerated(TopoDS::Edge(Scurrent))) continue;
        C = BRep_Tool::Curve(TopoDS::Edge(Scurrent), First, Last);
        Handle(Geom_TrimmedCurve) TC = Handle(Geom_TrimmedCurve)::DownCast(C);
        if (TC.IsNull()) {
          Handle(Geom_BSplineCurve) BC = Handle(Geom_BSplineCurve)::DownCast(C);
          if (!BC.IsNull()) {
            MaxNbPoles = Max(MaxNbPoles,BC->NbPoles());
          }
        }
        else {
          Handle(Geom_BSplineCurve) BC = Handle(Geom_BSplineCurve)::DownCast(TC->BasisCurve());
          if (BC.IsNull()) {
            Handle(Geom_TrimmedCurve) TC1 = Handle(Geom_TrimmedCurve)::DownCast(TC->BasisCurve());
            if (!TC1.IsNull()) {
              BC = Handle(Geom_BSplineCurve)::DownCast(TC1->BasisCurve());
            }
          }
          if (!BC.IsNull()) {
            MaxNbPoles = Max(MaxNbPoles,BC->NbPoles());
          }
        }
        aSeq.Append(C);
      }
      // prepare array of points for creation bspline surface
      // size of this array: by U parameter - number of curves,
      // by V parameter - determ using MaxNbPoles but it's
      // value must be between 21(min) and 101(max)
      int nbc = aSeq.Length();
      int nbp = Max(21, 2*MaxNbPoles-1);

      // commented for Mantis issue 0021541
      //if (nbp > 101) nbp = 101;

      TColgp_Array2OfPnt Points (1, nbc, 1, nbp);
      int ic = 1;
      for (; ic <= nbc; ic++) {
        Handle(Geom_Curve) C = aSeq.Value(ic);
        double fp = C->FirstParameter();
        double lp = C->LastParameter();
        double dp = (lp-fp)/(nbp-1);
        int j = 0;
        gp_Pnt P;
        for (; j < nbp; j++) {
          C->D0(fp+dp*j, P);
          Points.SetValue(ic, j+1, P);
       }
      }
      GeomAPI_PointsToBSplineSurface PTB (Points, mindeg, maxdeg, GeomAbs_C2, tol3d);
      Handle(Geom_BSplineSurface) BS = PTB.Surface();
      BRepBuilderAPI_MakeFace BB (BS, Precision::Confusion());
      TopoDS_Face NewF = BB.Face();
      Handle(ShapeFix_Face) sff = new ShapeFix_Face (NewF);
      sff->Perform();
      sff->FixOrientation();
      aShape = sff->Face();
    }
  }
  else if( aFunction->GetType() == FILLING_ON_CONSTRAINTS )
  {
    BRepOffsetAPI_MakeFilling MakeFilling;

    Handle(TColStd_HSequenceOfTransient) aConstraints = IF.GetShapes();

    for ( unsigned int ind = 1; ind <= aConstraints->Length(); ind++ ) {
      TopoDS_Edge E;
      TopoDS_Face F;
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast( aConstraints->Value(ind) );
      if ( !aRefShape->GetValue().IsNull() && aRefShape->GetValue().ShapeType() == TopAbs_EDGE )
        E = TopoDS::Edge(aRefShape->GetValue() );
      else {
        Standard_RangeError::Raise("Wrong parameters");
      }
      if ( ind < aConstraints->Length() ) {
        Handle(GEOM_Function) aRefFaceShape = Handle(GEOM_Function)::DownCast( aConstraints->Value(ind+1) );
        if ( !aRefFaceShape->GetValue().IsNull() && aRefFaceShape->GetValue().ShapeType() == TopAbs_FACE ) {
          F = TopoDS::Face( aRefFaceShape->GetValue() );
          ind++;
        }
      }
      if ( !F.IsNull() )
        MakeFilling.Add( E, F, GeomAbs_G1 );
      else
        MakeFilling.Add( E, GeomAbs_C0 );
    }

    MakeFilling.Build();
    if ( !MakeFilling.IsDone() )
    {
      Standard_RangeError::Raise("Filling on constraints failed");
    }

    aShape = TopoDS::Face( MakeFilling.Shape() );
  }

  /* We test the validity of resulting shape */
  if (!BRepAlgo::IsValid((aShape))) {
    Standard_ConstructionError::Raise("Algorithm has produced an invalid shape result");
  }

  aFunction->SetValue(aShape);

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif
  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_FillingDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IFilling aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "FILLING";

  switch ( aType ) {
  case BASIC_FILLING:
  {
    AddParam( theParams, "Input compound", aCI.GetShapes() );
    AddParam( theParams, "Method", aCI.GetMethod() );
    const char* method[3] =
      { "Standard", "Use edges orientation", "Correct edges orientation" };
    if ( 0 <= aCI.GetMethod() && aCI.GetMethod() < 3 )
      theParams[1] << " = " << method[ aCI.GetMethod() ];
    AddParam( theParams, "Min deg", aCI.GetMinDeg() );
    AddParam( theParams, "Max deg", aCI.GetMaxDeg() );
    AddParam( theParams, "Nb. Iter", aCI.GetNbIter() );
    AddParam( theParams, "Tol. 2D", aCI.GetTol2D() );
    AddParam( theParams, "Tol. 3D", aCI.GetTol3D() );
    AddParam( theParams, "Approximation", aCI.GetApprox() );
    break;
  }
  case FILLING_ON_CONSTRAINTS:
  {
	theOperationName = "FACE";
    AddParam( theParams, "Edges/Faces", aCI.GetShapes() );
    break;
  }
  default:
    return false;
  }

  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_FillingDriver,GEOM_BaseDriver);
