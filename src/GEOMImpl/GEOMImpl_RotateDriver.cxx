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

#include <GEOMImpl_RotateDriver.hxx>
#include <GEOMImpl_IRotate.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOM_Function.hxx>

#include <GEOMUtils.hxx>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <Geom_Line.hxx>
#include <GProp_GProps.hxx>

#include <Precision.hxx>

#include <gp_Trsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_RotateDriver::GetID()
{
  static Standard_GUID aRotateDriver("FF1BBB56-5D14-4df2-980B-3A668264EA16");
  return aRotateDriver;
}


//=======================================================================
//function : GEOMImpl_RotateDriver
//purpose  :
//=======================================================================

GEOMImpl_RotateDriver::GEOMImpl_RotateDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_RotateDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  if (aFunction.IsNull()) return 0;

  GEOMImpl_IRotate RI(aFunction);
  gp_Trsf aTrsf;
  gp_Pnt aCP, aP1, aP2;
  Standard_Integer aType = aFunction->GetType();
  Handle(GEOM_Function) anOriginalFunction = RI.GetOriginal();
  if (anOriginalFunction.IsNull()) return 0;
  TopoDS_Shape aShape, anOriginal = anOriginalFunction->GetValue();
  if (anOriginal.IsNull()) return 0;

  if (aType == ROTATE || aType == ROTATE_COPY) {
    Handle(GEOM_Function) anAxis = RI.GetAxis();
    if (anAxis.IsNull()) return 0;
    TopoDS_Shape A = anAxis->GetValue();
    // do not take into account edge orientation (it is correct)
    gp_Vec aV = GEOMUtils::GetVector(A, Standard_False);
    TopoDS_Edge anEdge = TopoDS::Edge(A);
    gp_Pnt aP1 = BRep_Tool::Pnt(TopExp::FirstVertex(anEdge));
    gp_Dir aDir (aV);
    gp_Ax1 anAx1 (aP1, aDir);

    Standard_Real anAngle = RI.GetAngle();
    if (fabs(anAngle) < Precision::Angular()) anAngle += 2.*M_PI; // NPAL19665,19769
    aTrsf.SetRotation(anAx1, anAngle);

    //NPAL18620: performance problem: multiple locations are accumulated
    //           in shape and need a great time to process
    //BRepBuilderAPI_Transform aTransformation(anOriginal, aTrsf, Standard_False);
    //aShape = aTransformation.Shape();
    TopLoc_Location aLocOrig = anOriginal.Location();
    gp_Trsf aTrsfOrig = aLocOrig.Transformation();
    //TopLoc_Location aLocRes (aTrsf * aTrsfOrig); // gp_Trsf::Multiply() has a bug
    aTrsfOrig.PreMultiply(aTrsf);
    TopLoc_Location aLocRes (aTrsfOrig);
    aShape = anOriginal.Located(aLocRes);
  }
  else if (aType ==  ROTATE_THREE_POINTS || aType == ROTATE_THREE_POINTS_COPY) {
    Handle(GEOM_Function) aCentPoint = RI.GetCentPoint();
    Handle(GEOM_Function) aPoint1 = RI.GetPoint1();
    Handle(GEOM_Function) aPoint2 = RI.GetPoint2();
    if(aCentPoint.IsNull() || aPoint1.IsNull() || aPoint2.IsNull()) return 0;
    TopoDS_Shape aCV = aCentPoint->GetValue();
    TopoDS_Shape aV1 = aPoint1->GetValue();
    TopoDS_Shape aV2 = aPoint2->GetValue();
    if(aCV.IsNull() || aCV.ShapeType() != TopAbs_VERTEX) return 0;
    if(aV1.IsNull() || aV1.ShapeType() != TopAbs_VERTEX) return 0;
    if(aV2.IsNull() || aV2.ShapeType() != TopAbs_VERTEX) return 0;

    aCP = BRep_Tool::Pnt(TopoDS::Vertex(aCV));
    aP1 = BRep_Tool::Pnt(TopoDS::Vertex(aV1));
    aP2 = BRep_Tool::Pnt(TopoDS::Vertex(aV2));

    gp_Vec aVec1 (aCP, aP1);
    gp_Vec aVec2 (aCP, aP2);
    gp_Dir aDir (aVec1 ^ aVec2);
    gp_Ax1 anAx1 (aCP, aDir);
    Standard_Real anAngle = aVec1.Angle(aVec2);
    if (fabs(anAngle) < Precision::Angular()) anAngle += 2.*M_PI; // NPAL19665
    aTrsf.SetRotation(anAx1, anAngle);
    //NPAL18620: performance problem: multiple locations are accumulated
    //           in shape and need a great time to process
    //BRepBuilderAPI_Transform aTransformation(anOriginal, aTrsf, Standard_False);
    //aShape = aTransformation.Shape();
    TopLoc_Location aLocOrig = anOriginal.Location();
    gp_Trsf aTrsfOrig = aLocOrig.Transformation();
    //TopLoc_Location aLocRes (aTrsf * aTrsfOrig); // gp_Trsf::Multiply() has a bug
    aTrsfOrig.PreMultiply(aTrsf);
    TopLoc_Location aLocRes (aTrsfOrig);
    aShape = anOriginal.Located(aLocRes);
  }
  else if (aType == ROTATE_1D || aType == ROTATE_1D_STEP) {
    //Get direction
    gp_Pnt aP1 = gp::Origin();
    gp_Dir D = gp::DZ();
    Handle(GEOM_Function) anAxis = RI.GetAxis();
    if (!anAxis.IsNull()) {
      TopoDS_Shape A = anAxis->GetValue();
      // do not take into account edge orientation (it is correct)
      gp_Vec aV = GEOMUtils::GetVector(A, Standard_False);
      TopoDS_Edge anEdge = TopoDS::Edge(A);
      aP1 = BRep_Tool::Pnt(TopExp::FirstVertex(anEdge));
      D = gp_Dir(aV);
    }
    gp_Ax1 AX1 (aP1, D);

    Standard_Integer nbtimes = RI.GetNbIter1();
    Standard_Real angle = 360. / nbtimes;
    if (aType == ROTATE_1D_STEP)
      angle = RI.GetAngle();

    TopoDS_Compound aCompound;
    BRep_Builder B;
    B.MakeCompound( aCompound );

    TopLoc_Location aLocOrig = anOriginal.Location();
    gp_Trsf aTrsfOrig = aLocOrig.Transformation();

    for (int i = 0; i < nbtimes; i++ ) {
      if (i == 0) { // NPAL19665
        B.Add(aCompound, anOriginal);
      }
      else {
        aTrsf.SetRotation(AX1, i * angle * M_PI / 180.);
        //TopLoc_Location aLocRes (aTrsf * aTrsfOrig); // gp_Trsf::Multiply() has a bug
        gp_Trsf aTrsfNew (aTrsfOrig);
        aTrsfNew.PreMultiply(aTrsf);
        TopLoc_Location aLocRes (aTrsfNew);
        B.Add(aCompound, anOriginal.Located(aLocRes));
      }
      //NPAL18620: performance problem: multiple locations are accumulated
      //           in shape and need a great time to process
      //BRepBuilderAPI_Transform aBRepTransformation(anOriginal, aTrsf, Standard_False);
      //B.Add(aCompound, aBRepTransformation.Shape());
    }

    aShape = aCompound;
  }
  else if (aType == ROTATE_2D) {
    //Get direction
    gp_Pnt aP1 = gp::Origin();
    gp_Dir D = gp::DZ();
    Handle(GEOM_Function) anAxis = RI.GetAxis();
    if (!anAxis.IsNull()) {
      TopoDS_Shape A = anAxis->GetValue();
      // do not take into account edge orientation (it is correct)
      gp_Vec aV = GEOMUtils::GetVector(A, Standard_False);
      TopoDS_Edge anEdge = TopoDS::Edge(A);
      aP1 = BRep_Tool::Pnt(TopExp::FirstVertex(anEdge));
      D = gp_Dir(aV);
    }
    gp_Ax1 AX1 (aP1, D);

    gp_Trsf aTrsf1;
    gp_Trsf aTrsf2;

    gp_XYZ aDir2 = RI.GetDir2(); // can be set by previous execution
    if (aDir2.Modulus() < gp::Resolution()) {
      // Calculate direction as vector from the axis to the shape's center
      gp_Pnt P1;
      GProp_GProps System;

      if (anOriginal.ShapeType() == TopAbs_VERTEX) {
        P1 = BRep_Tool::Pnt(TopoDS::Vertex( anOriginal ));
      }
      else if ( anOriginal.ShapeType() == TopAbs_EDGE || anOriginal.ShapeType() == TopAbs_WIRE ) {
        BRepGProp::LinearProperties(anOriginal, System);
        P1 = System.CentreOfMass();
      }
      else if ( anOriginal.ShapeType() == TopAbs_FACE || anOriginal.ShapeType() == TopAbs_SHELL ) {
        BRepGProp::SurfaceProperties(anOriginal, System);
        P1 = System.CentreOfMass();
      }
      else {
        BRepGProp::VolumeProperties(anOriginal, System);
        P1 = System.CentreOfMass();
      }

      Handle(Geom_Line) Line = new Geom_Line(AX1);
      GeomAPI_ProjectPointOnCurve aPrjTool( P1, Line );
      gp_Pnt P2 = aPrjTool.NearestPoint();

      if ( P1.IsEqual(P2, Precision::Confusion() ) ) return 0;

      aDir2 = gp_XYZ(P1.X()-P2.X(), P1.Y()-P2.Y(), P1.Z()-P2.Z());

      // Attention: this abnormal action is done for good working of
      // TransformLikeOther(), used by RestoreSubShapes functionality
      RI.SetDir2(aDir2);
    }

    gp_Vec Vec (aDir2);
    Vec.Normalize();

    Standard_Integer nbtimes2 = RI.GetNbIter2();
    Standard_Integer nbtimes1 = RI.GetNbIter1();
    Standard_Real step = RI.GetStep();
    Standard_Real ang = RI.GetAngle();

    TopLoc_Location aLocOrig = anOriginal.Location();
    gp_Trsf aTrsfOrig = aLocOrig.Transformation();

    gp_Vec aVec;
    TopoDS_Compound aCompound;
    BRep_Builder B;
    B.MakeCompound( aCompound );

    Standard_Real DX, DY, DZ;

    for (int i = 0; i < nbtimes2; i++ ) {
      DX = i * step * Vec.X();
      DY = i * step * Vec.Y();
      DZ = i * step * Vec.Z();
      aVec.SetCoord( DX, DY, DZ );
      aTrsf1.SetTranslation(aVec);

      for (int j = 0; j < nbtimes1; j++ ) {
        if (j == 0) { // NPAL19665
          TopLoc_Location aLocRes (aTrsf1 * aTrsfOrig);
          B.Add(aCompound, anOriginal.Located(aLocRes));
        }
        else {
          aTrsf2.SetRotation(AX1, j * ang * M_PI / 180.);
          //TopLoc_Location aLocRes (aTrsf2 * aTrsf1 * aTrsfOrig); // gp_Trsf::Multiply() has a bug
          gp_Trsf aTrsfNew (aTrsfOrig);
          aTrsfNew.PreMultiply(aTrsf1);
          aTrsfNew.PreMultiply(aTrsf2);
          TopLoc_Location aLocRes (aTrsfNew);
          B.Add(aCompound, anOriginal.Located(aLocRes));
        }
        //NPAL18620: performance problem: multiple locations are accumulated
        //           in shape and need a great time to process
        //BRepBuilderAPI_Transform aBRepTrsf1 (anOriginal, aTrsf1, Standard_False);
        //BRepBuilderAPI_Transform aBRepTrsf2 (aBRepTrsf1.Shape(), aTrsf2, Standard_False);
        //B.Add(aCompound, aBRepTrsf2.Shape());
      }
    }

    aShape = aCompound;
  }
  else return 0;


  if (aShape.IsNull()) return 0;

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

bool GEOMImpl_RotateDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IRotate aCI( function );
  Standard_Integer aType = function->GetType();

  switch ( aType ) {
  case ROTATE:
  case ROTATE_COPY:
    theOperationName = "ROTATION";
    AddParam( theParams, "Object", aCI.GetOriginal() );
    AddParam( theParams, "Axis", aCI.GetAxis() );
    AddParam( theParams, "Angle", aCI.GetAngle() );
    break;
  case ROTATE_THREE_POINTS:
  case ROTATE_THREE_POINTS_COPY:
    theOperationName = "ROTATION";
    AddParam( theParams, "Object", aCI.GetOriginal() );
    AddParam( theParams, "Central Point", aCI.GetCentPoint() );
    AddParam( theParams, "Point 1", aCI.GetPoint1() );
    AddParam( theParams, "Point 2", aCI.GetPoint2() );
    break;
  case ROTATE_1D:
    theOperationName = "MUL_ROTATION";
    AddParam( theParams, "Main Object", aCI.GetOriginal() );
    AddParam( theParams, "Axis", aCI.GetAxis(), "DZ" );
    AddParam( theParams, "Nb. Times", aCI.GetNbIter1() );
    break;
  case ROTATE_1D_STEP:
    theOperationName = "MUL_ROTATION";
    AddParam( theParams, "Main Object", aCI.GetOriginal() );
    AddParam( theParams, "Axis", aCI.GetAxis(), "DZ" );
    AddParam( theParams, "Angular step", aCI.GetAngle() );
    AddParam( theParams, "Nb. Times", aCI.GetNbIter1() );
    break;
  case ROTATE_2D:
    theOperationName = "MUL_ROTATION";
    AddParam( theParams, "Main Object", aCI.GetOriginal() );
    AddParam( theParams, "Axis", aCI.GetAxis(), "DZ" );
    AddParam( theParams, "Angular step", aCI.GetAngle() );
    AddParam( theParams, "Angular Nb. Times", aCI.GetNbIter1() );
    AddParam( theParams, "Radial step", aCI.GetStep() );
    AddParam( theParams, "Radial Nb. Times", aCI.GetNbIter2() );
    break;
  default:
    return false;
  }

  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_RotateDriver,GEOM_BaseDriver);
