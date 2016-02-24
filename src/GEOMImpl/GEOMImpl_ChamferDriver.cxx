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

#include <GEOMImpl_ChamferDriver.hxx>
#include <GEOMImpl_IChamfer.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_ILocalOperations.hxx>
#include <GEOMImpl_Block6Explorer.hxx>
#include <GEOMUtils.hxx>
#include <GEOM_Function.hxx>

#include <BRepLib.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_ChamferDriver::GetID()
{
  static Standard_GUID aChamferDriver("FF1BBB42-5D14-4df2-980B-3A668264EA16");
  return aChamferDriver;
}

//=======================================================================
//function : GEOMImpl_ChamferDriver
//purpose  :
//=======================================================================
GEOMImpl_ChamferDriver::GEOMImpl_ChamferDriver()
{
}

//=======================================================================
//function : isGoodForChamfer
//purpose  :
//=======================================================================
static Standard_Boolean isGoodForChamfer (const TopoDS_Shape& theShape)
{
  if (theShape.ShapeType() == TopAbs_SHELL ||
      theShape.ShapeType() == TopAbs_SOLID ||
      theShape.ShapeType() == TopAbs_COMPSOLID) {
    return Standard_True;
  }

  if (theShape.ShapeType() == TopAbs_COMPOUND) {
    TopTools_MapOfShape mapShape;
    TopoDS_Iterator It (theShape, Standard_False, Standard_False);
    for (; It.More(); It.Next()) {
      if (mapShape.Add(It.Value())) {
        if (!isGoodForChamfer(It.Value())) {
          return Standard_False;
        }
      }
    }
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_ChamferDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IChamfer aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  Handle(GEOM_Function) aRefShape = aCI.GetShape();
  TopoDS_Shape aShapeBase = aRefShape->GetValue();

  // Check the shape type. It have to be shell
  // or solid, or compsolid, or compound of these shapes.
  if (!isGoodForChamfer(aShapeBase)) {
    StdFail_NotDone::Raise
      ("Wrong shape. Must be shell or solid, or compsolid or compound of these shapes");
  }

  BRepFilletAPI_MakeChamfer fill (aShapeBase);

  if (aType == CHAMFER_SHAPE_ALL) {
    // symmetric chamfer on all edges
    double aD = aCI.GetD();
    TopTools_IndexedDataMapOfShapeListOfShape M;
    GEOMImpl_Block6Explorer::MapShapesAndAncestors(aShapeBase, TopAbs_EDGE, TopAbs_FACE, M);
    for (int i = 1; i <= M.Extent(); i++) {
      TopoDS_Edge E = TopoDS::Edge(M.FindKey(i));
      TopoDS_Face F = TopoDS::Face(M.FindFromIndex(i).First());
      if (!BRepTools::IsReallyClosed(E, F) &&
          !BRep_Tool::Degenerated(E) &&
          M.FindFromIndex(i).Extent() == 2)
        fill.Add(aD, E, F);
    }
  }
  else if (aType == CHAMFER_SHAPE_EDGE || aType == CHAMFER_SHAPE_EDGE_AD) {
    // chamfer on edges, common to two faces, with D1 on the first face

    TopoDS_Shape aFace1, aFace2;
    if (GEOMImpl_ILocalOperations::GetSubShape(aShapeBase, aCI.GetFace1(), aFace1) &&
        GEOMImpl_ILocalOperations::GetSubShape(aShapeBase, aCI.GetFace2(), aFace2))
    {
      TopoDS_Face F = TopoDS::Face(aFace1);

      // fill map of edges of the second face
      TopTools_MapOfShape aMap;
      TopExp_Explorer Exp2 (aFace2, TopAbs_EDGE);
      for (; Exp2.More(); Exp2.Next()) {
        aMap.Add(Exp2.Current());
      }

      // find edges of the first face, common with the second face
      TopExp_Explorer Exp (aFace1, TopAbs_EDGE);
      for (; Exp.More(); Exp.Next()) {
        if (aMap.Contains(Exp.Current())) {
          TopoDS_Edge E = TopoDS::Edge(Exp.Current());
          if (!BRepTools::IsReallyClosed(E, F) && !BRep_Tool::Degenerated(E))
          {
            if ( aType == CHAMFER_SHAPE_EDGE )
            {
              double aD1 = aCI.GetD1();
              double aD2 = aCI.GetD2();
              fill.Add(aD1, aD2, E, F);
            }
            else
            {
              double aD = aCI.GetD();
              double anAngle = aCI.GetAngle();
              if ( (anAngle > 0) && (anAngle < (M_PI/2.)) )
                fill.AddDA(aD, anAngle, E, F);
            }
          }
        }
      }
    }
  }
  else if (aType == CHAMFER_SHAPE_FACES || aType == CHAMFER_SHAPE_FACES_AD) {
    // chamfer on all edges of the selected faces, with D1 on the selected face
    // (on first selected face, if the edge belongs to two selected faces)

    int aLen = aCI.GetLength();
    int ind = 1;
    TopTools_MapOfShape aMap;
    TopTools_IndexedDataMapOfShapeListOfShape M;
    GEOMImpl_Block6Explorer::MapShapesAndAncestors(aShapeBase, TopAbs_EDGE, TopAbs_FACE, M);
    for (; ind <= aLen; ind++)
    {
      TopoDS_Shape aShapeFace;
      if (GEOMImpl_ILocalOperations::GetSubShape(aShapeBase, aCI.GetFace(ind), aShapeFace))
      {
        TopoDS_Face F = TopoDS::Face(aShapeFace);
        TopExp_Explorer Exp (F, TopAbs_EDGE);
        for (; Exp.More(); Exp.Next()) {
          if (!aMap.Contains(Exp.Current()))
          {
            TopoDS_Edge E = TopoDS::Edge(Exp.Current());
            if (!BRepTools::IsReallyClosed(E, F) &&
                !BRep_Tool::Degenerated(E) &&
                M.FindFromKey(E).Extent() == 2)
            {
              if (aType == CHAMFER_SHAPE_FACES)
              {
                double aD1 = aCI.GetD1();
                double aD2 = aCI.GetD2();
                fill.Add(aD1, aD2, E, F);
              }
              else
              {
                double aD = aCI.GetD();
                double anAngle = aCI.GetAngle();
                if ( (anAngle > 0) && (anAngle < (M_PI/2.)) )
                  fill.AddDA(aD, anAngle, E, F);
              }
            }
          }
        }
      }
    }
  }
  else if (aType == CHAMFER_SHAPE_EDGES || aType == CHAMFER_SHAPE_EDGES_AD)
  {
    // chamfer on selected edges with lenght param D1 & D2.

    int aLen = aCI.GetLength();
    int ind = 1;
    TopTools_MapOfShape aMap;
    TopTools_IndexedDataMapOfShapeListOfShape M;
    GEOMImpl_Block6Explorer::MapShapesAndAncestors(aShapeBase, TopAbs_EDGE, TopAbs_FACE, M);
    for (; ind <= aLen; ind++)
    {
      TopoDS_Shape aShapeEdge;
      if (GEOMImpl_ILocalOperations::GetSubShape(aShapeBase, aCI.GetEdge(ind), aShapeEdge))
      {
        TopoDS_Edge E = TopoDS::Edge(aShapeEdge);
        const TopTools_ListOfShape& aFacesList = M.FindFromKey(E);
        TopoDS_Face F = TopoDS::Face( aFacesList.First() );
        if (aType == CHAMFER_SHAPE_EDGES)
        {
          double aD1 = aCI.GetD1();
          double aD2 = aCI.GetD2();
          fill.Add(aD1, aD2, E, F);
        }
        else
        {
          double aD = aCI.GetD();
          double anAngle = aCI.GetAngle();
          if ( (anAngle > 0) && (anAngle < (M_PI/2.)) )
            fill.AddDA(aD, anAngle, E, F);
        }
      }
    }
  }
  else {
  }

  fill.Build();
  if (!fill.IsDone()) {
    StdFail_NotDone::Raise("Chamfer can not be computed on the given shape with the given parameters");
  }
  aShape = GEOMUtils::ReduceCompound( fill.Shape() );

  if (aShape.IsNull()) return 0;

  // reduce tolerances
  GEOMUtils::FixShapeTolerance( aShape );

  // fix SameParameter flag
  BRepLib::SameParameter(aShape, 1.E-5, Standard_True);

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

bool GEOMImpl_ChamferDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IChamfer aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "CHAMFER";

  switch ( aType ) {
  case CHAMFER_SHAPE_ALL:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Selected Edges", "all" );
    AddParam( theParams, "D", aCI.GetD() );
    break;
  case CHAMFER_SHAPE_EDGE:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Face 1", aCI.GetFace1() );
    AddParam( theParams, "Face 2", aCI.GetFace2() );
    AddParam( theParams, "D1", aCI.GetD1() );
    AddParam( theParams, "D2", aCI.GetD2() );
    break;
  case CHAMFER_SHAPE_EDGE_AD:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Face 1", aCI.GetFace1() );
    AddParam( theParams, "Face 2", aCI.GetFace2() );
    AddParam( theParams, "D", aCI.GetD() );
    AddParam( theParams, "Angle", aCI.GetAngle() );
    break;
  case CHAMFER_SHAPE_FACES:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Selected Faces" );
    if ( aCI.GetLength() > 1 )
      theParams[1] << aCI.GetLength() << " faces: ";
    for ( int i = 1, nb = aCI.GetLength(); i <= nb; ++i )
      theParams[1] << aCI.GetFace(i) << " ";
    AddParam( theParams, "D1", aCI.GetD1() );
    AddParam( theParams, "D2", aCI.GetD2() );
    break;
  case CHAMFER_SHAPE_FACES_AD:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Selected Faces" );
    if ( aCI.GetLength() > 1 )
      theParams[1] << aCI.GetLength() << " faces: ";
    for ( int i = 1, nb = aCI.GetLength(); i <= nb; ++i )
      theParams[1] << aCI.GetFace(i) << " ";
    AddParam( theParams, "D", aCI.GetD() );
    AddParam( theParams, "Angle", aCI.GetAngle() );
    break;
  case CHAMFER_SHAPE_EDGES:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Selected Edges" );
    if ( aCI.GetLength() > 1 )
      theParams[1] << aCI.GetLength() << " edges: ";
    for ( int i = 1, nb = aCI.GetLength(); i <= nb; ++i )
      theParams[1] << aCI.GetEdge(i) << " ";
    AddParam( theParams, "D1", aCI.GetD1() );
    AddParam( theParams, "D2", aCI.GetD2() );
    break;
  case CHAMFER_SHAPE_EDGES_AD:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Selected Edges" );
    if ( aCI.GetLength() > 1 )
      theParams[1] << aCI.GetLength() << " edges: ";
    for ( int i = 1, nb = aCI.GetLength(); i <= nb; ++i )
      theParams[1] << aCI.GetFace(i) << " ";
    AddParam( theParams, "D", aCI.GetD() );
    AddParam( theParams, "Angle", aCI.GetAngle() );
    break;
  default:
    return false;
  }

  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ChamferDriver,GEOM_BaseDriver);
