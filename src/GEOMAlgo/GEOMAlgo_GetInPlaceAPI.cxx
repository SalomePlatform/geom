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
// File:     GEOMAlgo_GetInPlaceAPI.cxx
// Created:
// Author:   Sergey KHROMOV


#include <GEOMAlgo_GetInPlaceAPI.hxx>
#include <GEOMAlgo_GetInPlace.hxx>
#include <GEOM_Object.hxx>
#include <GEOMUtils.hxx>

#include <Bnd_Box.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <GProp_GProps.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : GetInPlace
//purpose  : 
//=======================================================================
Standard_Boolean GEOMAlgo_GetInPlaceAPI::GetInPlace
                      (const TopoDS_Shape        &theWhere,
                       const TopoDS_Shape        &theWhat,
                             GEOMAlgo_GetInPlace &theGIP)
{
  if (theWhere.IsNull() || theWhat.IsNull()) {
    return Standard_False;
  }

  // Compute confusion tolerance.
  Standard_Real    aTolConf = Precision::Confusion();
  Standard_Integer i;

  for (i = 0; i < 2; ++i) {
    TopExp_Explorer anExp(i == 0 ? theWhere : theWhat, TopAbs_VERTEX);

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Vertex aVtx = TopoDS::Vertex(anExp.Current());
      const Standard_Real aTolVtx = BRep_Tool::Tolerance(aVtx);

      if (aTolVtx > aTolConf) {
        aTolConf = aTolVtx;
      }
    }
  }

  // Compute mass tolerance.
  Bnd_Box       aBoundingBox;
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  Standard_Real aMassTol;

  BRepBndLib::Add(theWhere, aBoundingBox);
  BRepBndLib::Add(theWhat,  aBoundingBox);
  aBoundingBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  aMassTol = Max(aXmax - aXmin, aYmax - aYmin);
  aMassTol = Max(aMassTol, aZmax - aZmin);
  aMassTol *= aTolConf;

  // Searching for the sub-shapes inside the ShapeWhere shape
  theGIP.SetTolerance(aTolConf);
  theGIP.SetTolMass(aMassTol);
  theGIP.SetTolCG(aTolConf);

  theGIP.SetArgument(theWhat);
  theGIP.SetShapeWhere(theWhere);

  theGIP.Perform();

  int iErr = theGIP.ErrorStatus();

  if (iErr) {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : GetInPlaceOld
//purpose  : 
//=======================================================================
Standard_Integer GEOMAlgo_GetInPlaceAPI::GetInPlaceOld
            (const TopoDS_Shape         &theWhere,
             const TopoDS_Shape         &theWhat,
                   TopTools_ListOfShape &theShapesInPlace)
{
  theShapesInPlace.Clear();

  if (theWhere.IsNull() || theWhat.IsNull()) {
    // Error: aWhere and aWhat TopoDS_Shape are Null.
    return 1;
  }

  // Check shape type.
  TopAbs_ShapeEnum iType = GEOMUtils::GetTypeOfSimplePart(theWhat);

  if (iType == TopAbs_SHAPE) {
    // Error: An attempt to extract a shape of not supported type.
    return 2;
  }

  // Compute confusion tolerance.
  Standard_Real    aTolConf = Precision::Confusion();
  Standard_Integer i;

  for (i = 0; i < 2; ++i) {
    TopExp_Explorer anExp(i == 0 ? theWhere : theWhat, TopAbs_VERTEX);

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Vertex aVtx = TopoDS::Vertex(anExp.Current());
      const Standard_Real aTolVtx = BRep_Tool::Tolerance(aVtx);

      if (aTolVtx > aTolConf) {
        aTolConf = aTolVtx;
      }
    }
  }

  // Compute mass tolerance.
  Bnd_Box       aBoundingBox;
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  Standard_Real aMassTol;

  BRepBndLib::Add(theWhere, aBoundingBox);
  BRepBndLib::Add(theWhat,  aBoundingBox);
  aBoundingBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  aMassTol = Max(aXmax - aXmin, aYmax - aYmin);
  aMassTol = Max(aMassTol, aZmax - aZmin);
  aMassTol *= aTolConf;

  // Compute the result.
  TopExp_Explorer     Exp_aWhat  (theWhat,  iType);
  TopExp_Explorer     Exp_aWhere (theWhere, iType);
  Standard_Real       tab_aWhat[4], tab_aWhere[4];
  gp_Pnt              aPnt, aPnt_aWhat;
  TopoDS_Shape        aPntShape;
  TopoDS_Vertex       aVertex;
  bool                isFound = false;
  TopTools_MapOfShape map_aWhere;

  for (; Exp_aWhere.More(); Exp_aWhere.Next()) {
    if (!map_aWhere.Add(Exp_aWhere.Current()))
      continue; // skip repeated shape to avoid mass addition
    GetShapeProperties( Exp_aWhere.Current(), tab_aWhere, aPnt );
    for ( Exp_aWhat.ReInit(); Exp_aWhat.More(); Exp_aWhat.Next() ) {
      GetShapeProperties( Exp_aWhat.Current(), tab_aWhat, aPnt_aWhat );
      if (fabs(tab_aWhat[3] - tab_aWhere[3]) <= aMassTol && aPnt_aWhat.Distance(aPnt) <= aTolConf)
        isFound = true;
      else {
        if (tab_aWhat[3] > tab_aWhere[3]) {
          aPntShape = BRepBuilderAPI_MakeVertex( aPnt ).Shape();
          aVertex   = TopoDS::Vertex( aPntShape );
          BRepExtrema_DistShapeShape aWhereDistance ( aVertex, Exp_aWhere.Current() );
          BRepExtrema_DistShapeShape aWhatDistance  ( aVertex, Exp_aWhat.Current() );
          if (aWhereDistance.IsDone() && aWhatDistance.IsDone() &&
              fabs(aWhereDistance.Value() - aWhatDistance.Value()) <= aTolConf)
          {
            // 0020162: "EDF 961 GEOM : Getinplace is getting additionnal orthogonal faces"
            // aVertex must be projected to the same point on Where and on What
            gp_Pnt pOnWhat  = aWhatDistance.PointOnShape2(1);
            gp_Pnt pOnWhere = aWhereDistance.PointOnShape2(1);
            isFound = (pOnWhat.Distance(pOnWhere) <= aTolConf);
            if ( isFound && iType == TopAbs_FACE )
            {
              // check normals at pOnWhat and pOnWhere
              const double angleTol = M_PI/180.;
              gp_Vec normToWhat  = GetNormal( TopoDS::Face(Exp_aWhat.Current()), aWhatDistance);
              gp_Vec normToWhere = GetNormal( TopoDS::Face(Exp_aWhere.Current()), aWhereDistance);
              if ( normToWhat * normToWhere < 0 )
                normToWhat.Reverse();
              isFound = ( normToWhat.Angle( normToWhere ) < angleTol );
            }
          }
        }
      }
      if ( isFound ) {
        theShapesInPlace.Append(Exp_aWhere.Current());
        //aWhere_Mass += tab_aWhere[3];
        isFound = false;
        break;
      }
    }
  }

  if (theShapesInPlace.Extent() == 0) {
    // Not found any Results
    return 3;
  }

  return 0;
}

//=======================================================================
//function : GetNormal
//purpose  : 
//=======================================================================
gp_Vec GEOMAlgo_GetInPlaceAPI::GetNormal
                         (const TopoDS_Face                &theFace,
                          const BRepExtrema_DistShapeShape &theExtrema)
{
  gp_Vec defaultNorm(1,0,0); // to have same normals on different faces
  try {
    // get UV at extrema point
    Standard_Real u,v, f,l;
    switch ( theExtrema.SupportTypeShape2(1) ) {
    case BRepExtrema_IsInFace: {
      theExtrema.ParOnFaceS2(1, u, v );
      break;
    }
    case BRepExtrema_IsOnEdge: {
      TopoDS_Edge edge = TopoDS::Edge( theExtrema.SupportOnShape2(1));
      Handle(Geom2d_Curve) pcurve =
        BRep_Tool::CurveOnSurface(edge, theFace, f,l);

      theExtrema.ParOnEdgeS2( 1, u );
      gp_Pnt2d uv = pcurve->Value( u );
      u = uv.Coord(1);
      v = uv.Coord(2);
      break;
    }
    case BRepExtrema_IsVertex: return defaultNorm;
    }
    // get derivatives
    BRepAdaptor_Surface surface( theFace, false );
    gp_Vec du, dv; gp_Pnt p;
    surface.D1( u, v, p, du, dv );

    return du ^ dv;

  } catch (Standard_Failure ) {
  }
  return defaultNorm;
}

//=======================================================================
//function : GetShapeProperties
//purpose  : 
//=======================================================================
void GEOMAlgo_GetInPlaceAPI::GetShapeProperties(const TopoDS_Shape  &theShape,
                                                      Standard_Real  theTab[],
                                                      gp_Pnt        &theVertex)
{
  GProp_GProps  aProps;
  gp_Pnt        aCenterMass;
  Standard_Real aShapeSize;

  if    (theShape.ShapeType() == TopAbs_VERTEX) {
    aCenterMass = BRep_Tool::Pnt(TopoDS::Vertex(theShape));
  } else if (theShape.ShapeType() == TopAbs_EDGE) {
    BRepGProp::LinearProperties(theShape,  aProps);
  } else if (theShape.ShapeType() == TopAbs_FACE) {
    BRepGProp::SurfaceProperties(theShape, aProps);
  } else {
    BRepGProp::VolumeProperties(theShape,  aProps);
  }

  if (theShape.ShapeType() == TopAbs_VERTEX) {
    aShapeSize = 1;
  } else {
    aCenterMass = aProps.CentreOfMass();
    aShapeSize  = aProps.Mass();
  }

  theVertex = aCenterMass;
  theTab[0] = theVertex.X();
  theTab[1] = theVertex.Y();
  theTab[2] = theVertex.Z();
  theTab[3] = aShapeSize;
}

//=======================================================================
//function : GetInPlaceByHistory
//purpose  : 
//=======================================================================
Standard_Boolean GEOMAlgo_GetInPlaceAPI::GetInPlaceByHistory
                      (const Handle(GEOM_Function)      &theWhereFunction,
                       const TopTools_IndexedMapOfShape &theWhereIndices,
                       const TopoDS_Shape               &theWhat,
                             TopTools_ListOfShape       &theShapesInPlace)
{
  if (theWhereFunction.IsNull() || theWhat.IsNull())
    return Standard_False;

  if (theWhereIndices.Contains(theWhat)) {
    // entity was not changed by the operation
    theShapesInPlace.Append(theWhat);

    return Standard_True;
  }

  // try to find in history
  TDF_Label aHistoryLabel = theWhereFunction->GetHistoryEntry(Standard_False);

  // search in history for all argument shapes
  Standard_Boolean isFound = Standard_False;
  Standard_Boolean isGood = Standard_False;

  TDF_LabelSequence aLabelSeq;
  theWhereFunction->GetDependency(aLabelSeq);
  Standard_Integer nbArg = aLabelSeq.Length();

  for (Standard_Integer iarg = 1; iarg <= nbArg && !isFound; iarg++) {

    TDF_Label anArgumentRefLabel = aLabelSeq.Value(iarg);

    Handle(GEOM_Object) anArgumentObject = GEOM_Object::GetReferencedObject(anArgumentRefLabel);
    TopoDS_Shape anArgumentShape = anArgumentObject->GetValue();

    TopTools_IndexedMapOfShape anArgumentIndices;
    TopExp::MapShapes(anArgumentShape, anArgumentIndices);

    if (anArgumentIndices.Contains(theWhat)) {
      isFound = Standard_True;
      Standard_Integer aWhatIndex = anArgumentIndices.FindIndex(theWhat);

      // Find corresponding label in history
      TDF_Label anArgumentHistoryLabel =
        theWhereFunction->GetArgumentHistoryEntry(anArgumentRefLabel, Standard_False);
      if (anArgumentHistoryLabel.IsNull()) {
        // Lost History of operation argument. Possibly, all its entities was removed.
        isGood = Standard_True;
      }
      else {
        TDF_Label aWhatHistoryLabel = anArgumentHistoryLabel.FindChild(aWhatIndex, Standard_False);

        if (aWhatHistoryLabel.IsNull()) {
          // Removed entity ? Compound ? Compsolid ? Shell ? Wire
          isGood = Standard_False;
        } else {
          Handle(TDataStd_IntegerArray) anIntegerArray;
          if (!aWhatHistoryLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anIntegerArray)) {
            //Error: Empty modifications history for the sought shape.
            isGood = Standard_False;
          }
          else {
            isGood = Standard_True;
            Standard_Integer imod, aModifLen = anIntegerArray->Array()->Length();
            for (imod = 1; imod <= aModifLen; imod++) {
              const Standard_Integer anIndex =
                anIntegerArray->Array()->Value(imod);

              theShapesInPlace.Append(theWhereIndices.FindKey(anIndex));
            }
          }
        }
      }
    }
  }

  isFound = isGood;

  if (!isFound) {
    // try compound/compsolid/shell/wire element by element
    Standard_Boolean isFoundAny = Standard_False;
    TopTools_MapOfShape mapShape;

    if (theWhat.ShapeType() == TopAbs_COMPOUND ||
        theWhat.ShapeType() == TopAbs_COMPSOLID) {
      // recursive processing of compound/compsolid
      TopoDS_Iterator anIt (theWhat, Standard_True, Standard_True);
      for (; anIt.More(); anIt.Next()) {
        if (mapShape.Add(anIt.Value())) {
          TopoDS_Shape curWhat = anIt.Value();
          isFoundAny = GetInPlaceByHistory(theWhereFunction, theWhereIndices, curWhat, theShapesInPlace);
          if (isFoundAny) isFound = Standard_True;
        }
      }
    }
    else if (theWhat.ShapeType() == TopAbs_SHELL) {
      // try to replace a shell by its faces images
      TopExp_Explorer anExp (theWhat, TopAbs_FACE);
      for (; anExp.More(); anExp.Next()) {
        if (mapShape.Add(anExp.Current())) {
          TopoDS_Shape curWhat = anExp.Current();
          isFoundAny = GetInPlaceByHistory(theWhereFunction, theWhereIndices, curWhat, theShapesInPlace);
          if (isFoundAny) isFound = Standard_True;
        }
      }
    }
    else if (theWhat.ShapeType() == TopAbs_WIRE) {
      // try to replace a wire by its edges images
      TopExp_Explorer anExp (theWhat, TopAbs_EDGE);
      for (; anExp.More(); anExp.Next()) {
        if (mapShape.Add(anExp.Current())) {
          TopoDS_Shape curWhat = anExp.Current();
          isFoundAny = GetInPlaceByHistory(theWhereFunction, theWhereIndices, curWhat, theShapesInPlace);
          if (isFoundAny) isFound = Standard_True;
        }
      }
    }
    else {
      // Removed entity
    }
  }

  return isFound;
}
