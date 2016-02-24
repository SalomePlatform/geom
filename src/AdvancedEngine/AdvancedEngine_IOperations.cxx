// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  File   : AdvancedEngine_IOperations.cxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "AdvancedEngine_IOperations.hxx"
#include "AdvancedEngine_PipeTShapeDriver.hxx"
#include "AdvancedEngine_IPipeTShape.hxx"
#include "AdvancedEngine_DividedDiskDriver.hxx"
#include "AdvancedEngine_IDividedDisk.hxx"
#include "AdvancedEngine_SmoothingSurfaceDriver.hxx"
#include "AdvancedEngine_ISmoothingSurface.hxx"

#include <Basics_OCCTVersion.hxx>

#include <utilities.h>
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

#include <GEOM_Function.hxx>
#include <GEOM_PythonDump.hxx>
#include <GEOMUtils.hxx>
#include <GEOMAlgo_ClsfSurf.hxx>
#include <GEOMAlgo_FinderShapeOn2.hxx>
#include <GEOMAlgo_Splitter.hxx>

#include <GEOMImpl_Gen.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOMImpl_IBasicOperations.hxx>
#include <GEOMImpl_IBooleanOperations.hxx>
#include <GEOMImpl_IShapesOperations.hxx>
#include <GEOMImpl_ITransformOperations.hxx>
#include <GEOMImpl_IBlocksOperations.hxx>
#include <GEOMImpl_I3DPrimOperations.hxx>
#include <GEOMImpl_ILocalOperations.hxx>
#include <GEOMImpl_IHealingOperations.hxx>
#include <GEOMImpl_IGroupOperations.hxx>
#include <GEOMImpl_GlueDriver.hxx>

#include <TDF_Tool.hxx>
#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TNaming_CopyShape.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>

#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GC_MakeConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>

#include <ShapeAnalysis_Edge.hxx>

#include <cmath>

#include "AdvancedEngine_Types.hxx"

#include <Standard_Stream.hxx>
#include <Standard_Failure.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

#define HALF_LENGTH_MAIN_PIPE     "Main pipe half length" //"Tuyau principal - demi longueur"
#define HALF_LENGTH_INCIDENT_PIPE "Incident pipe half length" //"Tuyau incident - demi longueur"
#define CIRCULAR_QUARTER_PIPE     "Circular quarter of pipe" //"Circulaire - quart de tuyau"
#define THICKNESS                 "Thickness" //"Epaisseur"
#define FLANGE                    "Flange" // "Collerette"
#define CHAMFER_OR_FILLET         "Chamfer or fillet" //"Chanfrein ou Raccord"
#define JUNCTION_FACE_1           "Junction 1" //"Face de jonction 1"
#define JUNCTION_FACE_2           "Junction 2" //"Face de jonction 2"
#define JUNCTION_FACE_3           "Junction 3" //"Face de jonction 3"

#define FIND_GROUPS_BY_POINTS 1

// Undefine below macro to enable workaround about fillet problem in MakePipeTShapeFillet
// VSR 30/12/2014: macro enabled
#define FILLET_FIX_TOLERANCE

//=============================================================================
/*!
 *  Constructor
 */
//=============================================================================
AdvancedEngine_IOperations::AdvancedEngine_IOperations(GEOM_Engine* theEngine, int theDocID) :
  GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("AdvancedEngine_IOperations::AdvancedEngine_IOperations");
  myBasicOperations     = new GEOMImpl_IBasicOperations(GetEngine(), GetDocID());
  myBooleanOperations   = new GEOMImpl_IBooleanOperations(GetEngine(), GetDocID());
  myShapesOperations    = new GEOMImpl_IShapesOperations(GetEngine(), GetDocID());
  myTransformOperations = new GEOMImpl_ITransformOperations(GetEngine(), GetDocID());
  myBlocksOperations    = new GEOMImpl_IBlocksOperations(GetEngine(), GetDocID());
  my3DPrimOperations    = new GEOMImpl_I3DPrimOperations(GetEngine(), GetDocID());
  myLocalOperations     = new GEOMImpl_ILocalOperations(GetEngine(), GetDocID());
  myHealingOperations   = new GEOMImpl_IHealingOperations(GetEngine(), GetDocID());
  myGroupOperations     = new GEOMImpl_IGroupOperations(GetEngine(), GetDocID());
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================
AdvancedEngine_IOperations::~AdvancedEngine_IOperations()
{
  MESSAGE("AdvancedEngine_IOperations::~AdvancedEngine_IOperations");
  delete myBasicOperations;
  delete myBooleanOperations;
  delete myShapesOperations;
  delete myTransformOperations;
  delete myBlocksOperations;
  delete my3DPrimOperations;
  delete myLocalOperations;
  delete myHealingOperations;
  delete myGroupOperations;
}

//=============================================================================
/*!
 *  SetPosition
 */
//=============================================================================
gp_Trsf AdvancedEngine_IOperations::GetPositionTrsf(double theL1, double theL2,
                                                    Handle(GEOM_Object) theP1,
                                                    Handle(GEOM_Object) theP2,
                                                    Handle(GEOM_Object) theP3)
{
  // Old Local Coordinates System oldLCS
  gp_Pnt P0(0, 0, 0);
  gp_Pnt P1(-theL1, 0, 0);
  gp_Pnt P2(theL1, 0, 0);
  gp_Pnt P3(0, 0, theL2);

  gp_Dir oldX(gp_Vec(P1, P2));
  gp_Dir oldZ(gp_Vec(P0, P3));
  gp_Ax3 oldLCS(P0, oldZ, oldX);

  // New Local Coordinates System newLCS
  double LocX, LocY, LocZ;
  gp_Pnt newP1 = BRep_Tool::Pnt(TopoDS::Vertex(theP1->GetValue()));
  gp_Pnt newP2 = BRep_Tool::Pnt(TopoDS::Vertex(theP2->GetValue()));
  gp_Pnt newP3 = BRep_Tool::Pnt(TopoDS::Vertex(theP3->GetValue()));
  LocX = (newP1.X() + newP2.X()) / 2.;
  LocY = (newP1.Y() + newP2.Y()) / 2.;
  LocZ = (newP1.Z() + newP2.Z()) / 2.;
  gp_Pnt newO(LocX, LocY, LocZ);

  gp_Dir newX(gp_Vec(newP1, newP2)); // P1P2 Vector
  gp_Dir newZ(gp_Vec(newO, newP3)); // OP3 Vector
  gp_Ax3 newLCS = gp_Ax3(newO, newZ, newX);

  gp_Trsf aTrsf;
  aTrsf.SetDisplacement(oldLCS, newLCS);

  return aTrsf;
}

//=============================================================================
/*!
 *  CheckCompatiblePosition
 *
 */
//=============================================================================
bool AdvancedEngine_IOperations::CheckCompatiblePosition(double& theL1, double& theL2,
                                                         Handle(GEOM_Object) theP1,
                                                         Handle(GEOM_Object) theP2,
                                                         Handle(GEOM_Object) theP3,
                                                         double theTolerance)
{
  SetErrorCode(KO);
  gp_Pnt P1 = BRep_Tool::Pnt(TopoDS::Vertex(theP1->GetValue()));
  gp_Pnt P2 = BRep_Tool::Pnt(TopoDS::Vertex(theP2->GetValue()));
  gp_Pnt P3 = BRep_Tool::Pnt(TopoDS::Vertex(theP3->GetValue()));

  double d12 = P1.Distance(P2);
  double d13 = P1.Distance(P3);
  double d23 = P2.Distance(P3);
  //    double d2 = newO.Distance(P3);

  if (Abs(d12) <= Precision::Confusion()) {
    SetErrorCode("Junctions points P1 and P2 are identical");
    return false;
  }
  if (Abs(d13) <= Precision::Confusion()) {
    SetErrorCode("Junctions points P1 and P3 are identical");
    return false;
  }
  if (Abs(d23) <= Precision::Confusion()) {
    SetErrorCode("Junctions points P2 and P3 are identical");
    return false;
  }


  double newL1 = 0.5 * d12;
  double newL2 = sqrt(pow(d13,2)-pow(newL1,2));
  //
  // theL1*(1-theTolerance) <= newL1 <= theL1*(1+theTolerance)
  //
  if (fabs(newL1 - theL1) > Precision::Approximation()) {
    if ( (newL1 * (1 - theTolerance) -theL1 <= Precision::Approximation()) &&
         (newL1 * (1 + theTolerance) -theL1 >= Precision::Approximation()) ) {
      //            std::cerr << "theL1 = newL1" << std::endl;
      theL1 = newL1;
    } else {
      theL1 = -1;
      SetErrorCode("Dimension for main pipe (L1) is incompatible with new position");
      return false;
    }
  }

  //
  // theL2*(1-theTolerance) <= newL2  <= theL2*(1+theTolerance)
  //
  if (fabs(newL2 - theL2) > Precision::Approximation()) {
    if ( (newL2 * (1 - theTolerance) -theL2 <= Precision::Approximation()) &&
         (newL2 * (1 + theTolerance) -theL2 >= Precision::Approximation()) ) {
      theL2 = newL2;
    } else {
      theL2 = -1;
      SetErrorCode("Dimension for incident pipe (L2) is incompatible with new position");
      return false;
    }
  }

  SetErrorCode(OK);
  return true;

}

//=============================================================================
/*!
 *  Generate the propagation groups of a Pipe T-Shape used for hexa mesh
 */
//=============================================================================
bool AdvancedEngine_IOperations::MakeGroups(Handle(GEOM_Object) theShape, int shapeType,
                                            double theR1, double theW1, double theL1,
                                            double theR2, double theW2, double theL2,
                                            double theH, double theW, double theRF,
                                            Handle(TColStd_HSequenceOfTransient) theSeq,
                                            gp_Trsf aTrsf)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return false;

  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("Shape is not defined");
    return false;
  }

  gp_Trsf aTrsfInv = aTrsf.Inverted();

//   int expectedGroups = 0;
//   if (shapeType == TSHAPE_BASIC)
//     if (Abs(theR2+theW2-theR1-theW1) <= Precision::Approximation())
//       expectedGroups = 10;
//     else
//       expectedGroups = 11;
//   else if (shapeType == TSHAPE_CHAMFER || shapeType == TSHAPE_FILLET)
//     expectedGroups = 12;

  double aR1Ext = theR1 + theW1;
  double aR2Ext = theR2 + theW2;

  /////////////////////////
  //// Groups of Faces ////
  /////////////////////////

  //
  // Comment the following lines when GetInPlace bug is solved
  // == BEGIN
  // Workaround of GetInPlace bug
  // Create a bounding box that fits the shape
  Handle(GEOM_Object) aBox = my3DPrimOperations->MakeBoxDXDYDZ(2*theL1, 2*aR1Ext, aR1Ext+theL2);
  aBox->GetLastFunction()->SetDescription("");
  myTransformOperations->TranslateDXDYDZ(aBox, -theL1, -aR1Ext, -aR1Ext);
  aBox->GetLastFunction()->SetDescription("");
  // Apply transformation to box
  BRepBuilderAPI_Transform aTransformationBox(aBox->GetValue(), aTrsf, Standard_False);
  TopoDS_Shape aBoxShapeTrsf = aTransformationBox.Shape();
  aBox->GetLastFunction()->SetValue(aBoxShapeTrsf);

  // Get the shell of the box
  Handle(GEOM_Object) aShell = Handle(GEOM_Object)::DownCast
    (myShapesOperations->MakeExplode(aBox, TopAbs_SHELL, true)->Value(1));
  aBox->GetLastFunction()->SetDescription("");
  aShell->GetLastFunction()->SetDescription("");
  // Get the common shapes between shell and shape
  Handle(GEOM_Object) aCommonCompound = myBooleanOperations->MakeBoolean
                            (theShape, aShell, 1, Standard_False); // MakeCommon
  if (aCommonCompound.IsNull()) {
    SetErrorCode(myBooleanOperations->GetErrorCode());
    return false;
  }
  aCommonCompound->GetLastFunction()->SetDescription("");
  // Explode the faces of common shapes => 3 faces
  Handle(TColStd_HSequenceOfTransient) aCommonFaces =
    myShapesOperations->MakeExplode(aCommonCompound, TopAbs_FACE, true);
  aCommonCompound->GetLastFunction()->SetDescription("");
  std::list<Handle(GEOM_Object)> aCompoundOfFacesList;

  for (int i=0 ; i<= aCommonFaces->Length()-4 ; i+=4) {
    std::list<Handle(GEOM_Object)> aFacesList;
    for (int j = 1 ; j <= 4 ; j++) {
      Handle(GEOM_Object) aFace = Handle(GEOM_Object)::DownCast(aCommonFaces->Value(i+j)); // Junction faces
      if (!aFace.IsNull()) {
        aFace->GetLastFunction()->SetDescription("");
        aFacesList.push_back(aFace);
      }
    }
    Handle(GEOM_Object) aCompoundOfFaces = myShapesOperations->MakeCompound(aFacesList);
    if (!aCompoundOfFaces.IsNull()) {
      aCompoundOfFaces->GetLastFunction()->SetDescription("");
      aCompoundOfFacesList.push_back(aCompoundOfFaces);
    }
  }

  if (aCompoundOfFacesList.size() == 3) {
    Handle(GEOM_Object) aPln1 = aCompoundOfFacesList.front();
    aCompoundOfFacesList.pop_front();
    Handle(GEOM_Object) aPln2 = aCompoundOfFacesList.front();
    aCompoundOfFacesList.pop_front();
    Handle(GEOM_Object) aPln3 = aCompoundOfFacesList.front();
    aCompoundOfFacesList.pop_front();
    // == END
    //


    //     Uncomment the following lines when GetInPlace bug is solved
    //     == BEGIN
//     Handle(GEOM_Object) aP1 = myBasicOperations->MakePointXYZ(-theL1, 0, 0);
//     Handle(GEOM_Object) aP2 = myBasicOperations->MakePointXYZ(-0, 0, theL2);
//     Handle(GEOM_Object) aP3 = myBasicOperations->MakePointXYZ(theL1, 0, 0);
//     aP1->GetLastFunction()->SetDescription("");
//     aP2->GetLastFunction()->SetDescription("");
//     aP3->GetLastFunction()->SetDescription("");
//     Handle(GEOM_Object) aV1 = myBasicOperations->MakeVectorDXDYDZ(-1, 0, 0);
//     Handle(GEOM_Object) aV2 = myBasicOperations->MakeVectorDXDYDZ(0, 0, 1);
//     Handle(GEOM_Object) aV3 = myBasicOperations->MakeVectorDXDYDZ(1, 0, 0);
//     aV1->GetLastFunction()->SetDescription("");
//     aV2->GetLastFunction()->SetDescription("");
//     aV3->GetLastFunction()->SetDescription("");
//     Handle(GEOM_Object) aPln1 = myBasicOperations->MakePlanePntVec(aP1, aV1, 2*(aR1Ext+theL2));
//     Handle(GEOM_Object) aPln2 = myBasicOperations->MakePlanePntVec(aP2, aV2, 2*(aR2Ext));
//     Handle(GEOM_Object) aPln3 = myBasicOperations->MakePlanePntVec(aP3, aV3, 2*(aR1Ext+theL2));
//     aPln1->GetLastFunction()->SetDescription("");
//     aPln2->GetLastFunction()->SetDescription("");
//     aPln3->GetLastFunction()->SetDescription("");
//
//     BRepBuilderAPI_Transform aTransformation1(aPln1->GetValue(), aTrsf, Standard_False);
//     TopoDS_Shape aTrsf_Shape1 = aTransformation1.Shape();
//     aPln1->GetLastFunction()->SetValue(aTrsf_Shape1);
//     BRepBuilderAPI_Transform aTransformation2(aPln2->GetValue(), aTrsf, Standard_False);
//     TopoDS_Shape aTrsf_Shape2 = aTransformation2.Shape();
//     aPln2->GetLastFunction()->SetValue(aTrsf_Shape2);
//     BRepBuilderAPI_Transform aTransformation3(aPln3->GetValue(), aTrsf, Standard_False);
//     TopoDS_Shape aTrsf_Shape3 = aTransformation3.Shape();
//     aPln3->GetLastFunction()->SetValue(aTrsf_Shape3);
    //     == END
    //

    Handle(GEOM_Object) junctionFaces1 = myShapesOperations->GetInPlace(theShape, aPln1);
    if (junctionFaces1.IsNull())
      junctionFaces1 = myShapesOperations->GetShapesOnShapeAsCompound
        (aPln1, theShape, TopAbs_FACE,  GEOMAlgo_ST_ONIN);
    if (!junctionFaces1.IsNull()) {
      junctionFaces1->GetLastFunction()->SetDescription("");
      junctionFaces1->SetName("JUNCTION_FACE_1");
      theSeq->Append(junctionFaces1);
    }
    else {
      SetErrorCode("Junction face 1 not found");
      //        theSeq->Append(aPln1);
      //        return false;
    }
    Handle(GEOM_Object) junctionFaces2 = myShapesOperations->GetInPlace(theShape, aPln2);
    if (junctionFaces2.IsNull())
      junctionFaces2 = myShapesOperations->GetShapesOnShapeAsCompound
        (aPln2, theShape, TopAbs_FACE,  GEOMAlgo_ST_ONIN);
    if (!junctionFaces2.IsNull()) {
      junctionFaces2->GetLastFunction()->SetDescription("");
      junctionFaces2->SetName("JUNCTION_FACE_2");
      theSeq->Append(junctionFaces2);
    }
    else {
      SetErrorCode("Junction face 2 not found");
      //        theSeq->Append(aPln2);
      //        return false;
    }
    Handle(GEOM_Object) junctionFaces3 = myShapesOperations->GetInPlace(theShape, aPln3);
    if (junctionFaces3.IsNull())
      junctionFaces3 = myShapesOperations->GetShapesOnShapeAsCompound
        (aPln3, theShape, TopAbs_FACE,  GEOMAlgo_ST_ONIN);
    if (!junctionFaces3.IsNull()) {
      junctionFaces3->GetLastFunction()->SetDescription("");
      junctionFaces3->SetName("JUNCTION_FACE_3");
      theSeq->Append(junctionFaces3);
    }
    else {
      SetErrorCode("Junction face 3 not found");
      //        theSeq->Append(aPln3);
      //        return false;
    }
  // Comment the following lines when GetInPlace bug is solved
  // == BEGIN
  }
  //     == END

  /////////////////////////
  //// Groups of Edges ////
  /////////////////////////
  // Result of propagate

  Handle(GEOM_Function) aFunction = theShape->GetLastFunction();

  TCollection_AsciiString theDesc = aFunction->GetDescription();
  Handle(TColStd_HSequenceOfTransient) aSeqPropagate = myBlocksOperations->Propagate(theShape);
  if (aSeqPropagate.IsNull() || aSeqPropagate->Length() == 0) {
    SetErrorCode("Propagation groups not found");
    return false;
  }
  Standard_Integer aNbGroups = aSeqPropagate->Length();
  // Recover previous description to get rid of Propagate dump
  aFunction->SetDescription(theDesc);

#ifdef FIND_GROUPS_BY_POINTS
  // BEGIN: new groups search

  //              W2  R2
  //            .----.-----.----.
  //           e|    |  |  |    |
  //            |    |  |  |    |
  //            .    |  |  |    .
  //         g / ''..|  |  |..'' \
  //       f  /      '''''''      \
  //  .---.--'..     |  |  |     ..'--.---.
  //  |a    \   '''...........'''   /     |
  //  |-------\------'  |  '------/-------.
  //  |         \       |       /         |
  // c|           \     |     /           |
  //  |    R1       \   |   /             |
  //  |               \ | /               |
  //  ._________________|_________________.
  //  |       L1        |                 |
  //  |                 |                 |
  //  |                 |                 |
  // b|                 |                 |
  //  |                 |                 |
  //  |-----------------|-----------------|
  //  |    W1           |                 |
  //  '-----------------'-----------------'
  //          d

  // "Thickness" group (a)
  gp_Pnt aPntA (-theL1, 0, theR1 + theW1/2.);
  aPntA.Transform(aTrsf);
  BRepBuilderAPI_MakeVertex mkVertexA (aPntA);
  TopoDS_Vertex aVertA = TopoDS::Vertex(mkVertexA.Shape());
  TopoDS_Shape anEdgeA = GEOMUtils::GetEdgeNearPoint(aShape, aVertA);

  // "Circular quarter of pipe" group (b)
  gp_Pnt aPntB (-theL1, -aR1Ext * sin(M_PI/4.), -aR1Ext * sin(M_PI/4.));
  aPntB.Transform(aTrsf);
  BRepBuilderAPI_MakeVertex mkVertexB (aPntB);
  TopoDS_Vertex aVertB = TopoDS::Vertex(mkVertexB.Shape());
  TopoDS_Shape anEdgeB = GEOMUtils::GetEdgeNearPoint(aShape, aVertB);

  // "Circular quarter of pipe" group (c)
  gp_Pnt aPntC (-theL1, -aR1Ext * sin(M_PI/4.), aR1Ext * sin(M_PI/4.));
  aPntC.Transform(aTrsf);
  BRepBuilderAPI_MakeVertex mkVertexC (aPntC);
  TopoDS_Vertex aVertC = TopoDS::Vertex(mkVertexC.Shape());
  TopoDS_Shape anEdgeC = GEOMUtils::GetEdgeNearPoint(aShape, aVertC);

  // "Main pipe half length" group (d)
  gp_Pnt aPntD (-theL1/2., 0, -aR1Ext);
  aPntD.Transform(aTrsf);
  BRepBuilderAPI_MakeVertex mkVertexD (aPntD);
  TopoDS_Vertex aVertD = TopoDS::Vertex(mkVertexD.Shape());
  TopoDS_Shape anEdgeD = GEOMUtils::GetEdgeNearPoint(aShape, aVertD);

  // "Incident pipe half length" group (e)
  double aTol10 = Precision::Confusion() * 10.;
  gp_Pnt aPntE (-aR2Ext, 0, theL2 - aTol10);
  aPntE.Transform(aTrsf);
  BRepBuilderAPI_MakeVertex mkVertexE (aPntE);
  TopoDS_Vertex aVertE = TopoDS::Vertex(mkVertexE.Shape());
  TopoDS_Shape anEdgeE = GEOMUtils::GetEdgeNearPoint(aShape, aVertE);

  // "Flange" group (f)
  double aFx = - aR2Ext - aTol10;
  if (shapeType == TSHAPE_CHAMFER)
    aFx -= theW;
  else if (shapeType == TSHAPE_FILLET)
    aFx -= theRF;
  gp_Pnt aPntF (aFx, 0, aR1Ext);
  aPntF.Transform(aTrsf);
  BRepBuilderAPI_MakeVertex mkVertexF (aPntF);
  TopoDS_Vertex aVertF = TopoDS::Vertex(mkVertexF.Shape());
  TopoDS_Shape anEdgeF = GEOMUtils::GetEdgeNearPoint(aShape, aVertF);

  // "Chamfer or Fillet" group (g)
  TopoDS_Shape anEdgeG;
  if (shapeType == TSHAPE_CHAMFER) {
    gp_Pnt aPntG (-aR2Ext - theW/2., 0, aR1Ext + theH/2.);
    aPntG.Transform(aTrsf);
    BRepBuilderAPI_MakeVertex mkVertexG (aPntG);
    TopoDS_Vertex aVertG = TopoDS::Vertex(mkVertexG.Shape());
    anEdgeG = GEOMUtils::GetEdgeNearPoint(aShape, aVertG);
  }
  else if (shapeType == TSHAPE_FILLET) {
    gp_Pnt aPntG (-aR2Ext - theRF/2., 0, aR1Ext + theRF/2.);
    aPntG.Transform(aTrsf);
    BRepBuilderAPI_MakeVertex mkVertexG (aPntG);
    TopoDS_Vertex aVertG = TopoDS::Vertex(mkVertexG.Shape());
    anEdgeG = GEOMUtils::GetEdgeNearPoint(aShape, aVertG);
  }

  for (int i = 1 ; i <= aNbGroups; i++) {
    Handle(GEOM_Object) aGroup = Handle(GEOM_Object)::DownCast(aSeqPropagate->Value(i));
    if (aGroup.IsNull())
      continue;

    TopoDS_Shape aGroupShape = aGroup->GetValue();
    TopTools_IndexedMapOfShape anEdgesMap;
    TopExp::MapShapes(aGroupShape, TopAbs_EDGE, anEdgesMap);

    if (anEdgesMap.Contains(anEdgeA)) { // a
      aGroup->SetName("THICKNESS");
      theSeq->Append(aGroup);
    }
    else if (anEdgesMap.Contains(anEdgeB)) { // b
      aGroup->SetName("CIRCULAR_QUARTER_PIPE");
      theSeq->Append(aGroup);
    }
    else if (anEdgesMap.Contains(anEdgeC)) { // c
      aGroup->SetName("CIRCULAR_QUARTER_PIPE");
      theSeq->Append(aGroup);
    }
    else if (anEdgesMap.Contains(anEdgeD)) { // d
      aGroup->SetName("HALF_LENGTH_MAIN_PIPE");
      theSeq->Append(aGroup);
    }
    else if (anEdgesMap.Contains(anEdgeE)) { // e
      aGroup->SetName("HALF_LENGTH_INCIDENT_PIPE");
      theSeq->Append(aGroup);
    }
    else if (anEdgesMap.Contains(anEdgeF)) { // f
      aGroup->SetName("FLANGE");
      theSeq->Append(aGroup);
    }
    else if (shapeType == TSHAPE_CHAMFER) { // g
      if (anEdgesMap.Contains(anEdgeG)) {
        aGroup->SetName("CHAMFER");
        theSeq->Append(aGroup);
      }
    }
    else if (shapeType == TSHAPE_FILLET) { // g
      if (anEdgesMap.Contains(anEdgeG)) {
        aGroup->SetName("FILLET");
        theSeq->Append(aGroup);
      }
    }
    else {
    }
  }
  // END: new groups search
#else
  bool addGroup;
  bool circularFoundAndAdded = false;
  bool circularFound10 = false;
  bool incidentPipeFound = false;
  bool mainPipeFound = false;
  bool mainPipeFoundAndAdded = false;
  bool radialFound =false;
  bool flangeFound = false;
  bool flangeFoundAndAdded = false;
  bool chamferOrFilletFound = false;

  for (int i = 1 ; i <= aNbGroups; i++) {
    addGroup = false;

    Handle(GEOM_Object) aGroup = Handle(GEOM_Object)::DownCast(aSeqPropagate->Value(i));
    if (aGroup.IsNull())
      continue;

    TopoDS_Shape aGroupShape = aGroup->GetValue();
    BRepBuilderAPI_Transform aTransformationShapeInv (aGroupShape, aTrsfInv, Standard_False);
    TopoDS_Shape aGroupShapeTrsfInv = aTransformationShapeInv.Shape();

    TopTools_IndexedMapOfShape anEdgesMap;
    TopExp::MapShapes(aGroupShapeTrsfInv,TopAbs_EDGE, anEdgesMap);
    Standard_Integer nbEdges = anEdgesMap.Extent();

    if (shapeType == TSHAPE_BASIC) {
      if ((nbEdges >= 21) || /*R1Ext = R2Ext*/(nbEdges == 17)) { // 17, 17+8*{1,2,3}, 21, 21+8*{1,2,3}
        addGroup = true;
        aGroup->SetName("THICKNESS");
      }
      else if (nbEdges == 6) {
        if (!circularFoundAndAdded) {
          circularFoundAndAdded = true;
          addGroup = true;
          aGroup->SetName("CIRCULAR_QUARTER_PIPE");
        }
      }
      else if (nbEdges == 8) {
        incidentPipeFound = true;
        mainPipeFound = false;
        radialFound = false;
        flangeFound = false;

        TopExp_Explorer Ex(aGroupShapeTrsfInv,TopAbs_VERTEX);
        while (Ex.More()) {
          gp_Pnt aP =  BRep_Tool::Pnt(TopoDS::Vertex(Ex.Current()));
          double x=aP.X(), y=aP.Y(), z=aP.Z();


          if ((Abs(x) > aR2Ext + Precision::Confusion()) ||
              (Abs(y) > aR2Ext + Precision::Confusion())) {
            incidentPipeFound = false;
          }

          if ( z < -Precision::Confusion()) {
            // length of main pipe
            mainPipeFound = true;
            if (!mainPipeFoundAndAdded) {
              mainPipeFoundAndAdded = true;
              addGroup = true;
              aGroup->SetName("HALF_LENGTH_MAIN_PIPE");
            }
          }

          else if (Abs(x) > (theL1-Precision::Confusion())) {
            // discretisation circulaire
            radialFound = true;
            if (!circularFoundAndAdded) {
              circularFoundAndAdded = true;
              addGroup = true;
              aGroup->SetName("CIRCULAR_QUARTER_PIPE");
            }
          }
          Ex.Next();
        }
        if (incidentPipeFound) {
          addGroup = true;
          aGroup->SetName("HALF_LENGTH_INCIDENT_PIPE");
        }
        if (!addGroup && (!incidentPipeFound &&
                          !radialFound &&
                          !mainPipeFound &&
                          !flangeFound)) {
          // Flange (collerette)
          flangeFound = true;
          addGroup = true;
          aGroup->SetName("FLANGE");
        }
      }
      else
        continue;
    }
    else if (shapeType == TSHAPE_CHAMFER || shapeType == TSHAPE_FILLET) {
      if (nbEdges >= 25) { // 25, 25+8, 25+16, 25+24
        addGroup = true;
        aGroup->SetName("THICKNESS");
      }
      else if ((nbEdges == 10) || (nbEdges == 6)) {
        if (!circularFoundAndAdded) {
          addGroup = true;
          circularFoundAndAdded = true;
          aGroup->SetName("CIRCULAR_QUARTER_PIPE");
          if (nbEdges == 10) {
            circularFound10 = true;
          }
        }
        else if (!circularFound10 && nbEdges == 10) {
          circularFound10 = true;
          addGroup = true;
          aGroup->SetName("CIRCULAR_QUARTER_PIPE");
        }
      }
      else if (nbEdges == 8) {
        incidentPipeFound = true;
        mainPipeFound = true;
        flangeFound = false;

        bool isNearZ0 = false;
        bool isBelowZ0 = false;

        TopExp_Explorer Ex (aGroupShapeTrsfInv,TopAbs_VERTEX);
        while (Ex.More()) {
          gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(Ex.Current()));
          double x=aP.X(), y=aP.Y(), z=aP.Z();

          // tuy_princ_long_avant & tuy_princ_long_apres
          //bool isMain = (((z < Precision::Confusion()) || (x < Precision::Confusion())) &&
          //               ((y <= aR1Ext + Precision::Confusion()) ||
          //                (y <= -(aR1Ext + Precision::Confusion())) ||
          //                (y <= theR1 + Precision::Confusion()) ||
          //                (y == -(theR1 + Precision::Confusion()))));
          bool isMain = ((z < Precision::Confusion() || x < Precision::Confusion()) &&
                         (fabs(y) > theR1 - Precision::Confusion() ||
                          fabs(y) < Precision::Confusion()));

          if (!isMain) {
            mainPipeFound = false;
          }

          // collerette
          //if (z < Precision::Confusion() && !isMain) {
          //  flangeFound = true;
          //  if (!flangeFoundAndAdded) {
          //    flangeFoundAndAdded = true;
          //    addGroup = true;
          //    aGroup->SetName("FLANGE");
          //  }
          //}
          if (fabs(z) < Precision::Confusion()) isNearZ0 = true;
          if (z < - Precision::Confusion()) isBelowZ0 = true;

          // tuyau incident
          if ((Abs(x) > aR2Ext + Precision::Confusion()) ||
              (Abs(y) > aR2Ext + Precision::Confusion())) {
            incidentPipeFound = false;
          }
          Ex.Next();
        }
        if (mainPipeFound) {
          addGroup = true;
          aGroup->SetName("HALF_LENGTH_MAIN_PIPE");
        }
        if (incidentPipeFound) {
          addGroup = true;
          aGroup->SetName("HALF_LENGTH_INCIDENT_PIPE");
        }
        if (isNearZ0 && !isBelowZ0) {
          flangeFound = true;
          if (!flangeFoundAndAdded) {
            flangeFoundAndAdded = true;
            addGroup = true;
            aGroup->SetName("FLANGE");
          }
        }
        if (!addGroup && (!incidentPipeFound &&
                          !mainPipeFound &&
                          !flangeFound &&
                          !chamferOrFilletFound)) {
          addGroup = true;
          chamferOrFilletFound = true;
          if (shapeType == TSHAPE_CHAMFER)
            aGroup->SetName("CHAMFER");
          else
            aGroup->SetName("FILLET");
        }
      }
      else
        continue;
    }
    // Add group to the list
    if (addGroup)
      theSeq->Append(aGroup);
  }
#endif

  SetErrorCode(OK);
  return true;
}

//=============================================================================
/*!
 *  Return faces that are laying on surface.
 */
//=============================================================================
bool AdvancedEngine_IOperations::GetFacesOnSurf
                     (const TopoDS_Shape &theShape,
                      const Handle(Geom_Surface)& theSurface,
                      const Standard_Real theTolerance,
                      TopTools_ListOfShape &theFaces)
{
  GEOMAlgo_FinderShapeOn2   aFinder;
  Handle(GEOMAlgo_ClsfSurf) aClsfSurf = new GEOMAlgo_ClsfSurf;

  aClsfSurf->SetSurface(theSurface);
  aFinder.SetShape(theShape);
  aFinder.SetTolerance(theTolerance);
  aFinder.SetClsf(aClsfSurf);
  aFinder.SetShapeType(TopAbs_FACE);
  aFinder.SetState(GEOMAlgo_ST_ON);

  // Sets the minimal number of inner points for the faces that do not have own
  // inner points at all (for e.g. rectangular planar faces have just 2 triangles).
  // Default value=3
  aFinder.SetNbPntsMin(3);
  // Sets the maximal number of inner points for edges or faces.
  // It is usefull for the cases when this number is very big (e.g =2000) to improve
  // the performance. If this value =0, all inner points will be taken into account.
  // Default value=0
  aFinder.SetNbPntsMax(100);
  aFinder.Perform();

  // Interprete results
  Standard_Integer iErr = aFinder.ErrorStatus();
  // the detailed description of error codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iErr) {
    MESSAGE(" iErr : " << iErr);
    TCollection_AsciiString aMsg (" iErr : ");
    aMsg += TCollection_AsciiString(iErr);
    SetErrorCode(aMsg);
    return false;
  }
  Standard_Integer iWrn = aFinder.WarningStatus();
  // the detailed description of warning codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iWrn) {
    MESSAGE(" *** iWrn : " << iWrn);
  }

  const TopTools_ListOfShape &aListRes = aFinder.Shapes(); // the result
  TopTools_ListIteratorOfListOfShape anIter (aListRes);

  for (; anIter.More(); anIter.Next()) {
    theFaces.Append(anIter.Value());
  }

  return true;
}

//=============================================================================
/*!
 *  Creates and returns conical face.
 */
//=============================================================================
TopoDS_Shape AdvancedEngine_IOperations::MakeConicalFace
                                  (const gp_Ax2 &theAxis,
                                   const double theRadius,
                                   const double theRadiusThin,
                                   const double theHeight,
                                   const gp_Trsf &theTrsf)
{
  BRepPrimAPI_MakeCone aMkCone (theAxis, theRadius, theRadiusThin, theHeight);
  TopoDS_Shape aResult;
  
  aMkCone.Build();
  if (aMkCone.IsDone()) {
    TopExp_Explorer anExp(aMkCone.Shape(), TopAbs_FACE);

    for (; anExp.More(); anExp.Next()) {
      TopoDS_Face aFace = TopoDS::Face(anExp.Current());

      if (aFace.IsNull() == Standard_False) {
        BRepAdaptor_Surface anAdaptor(aFace, Standard_False);

        if (anAdaptor.GetType() == GeomAbs_Cone) {
          // This is a conical face. Transform and return it.
          BRepBuilderAPI_Transform aTransf(aFace, theTrsf, Standard_False);
          
          aResult = aTransf.Shape();
          break;
        }
      }
    }
  }

  return aResult;
}

//=============================================================================
/*!
 *  Generate the internal group of a Pipe T-Shape
 */
//=============================================================================
bool AdvancedEngine_IOperations::MakeInternalGroup
                      (const Handle(GEOM_Object) &theShape,
                       const double theR1, const double theLen1,
                       const double theR2, const double theLen2,
                       const double theRL, double theTransLenL,
                       const double theRR, double theTransLenR,
                       const double theRI, double theTransLenI,
                       const Handle(TColStd_HSequenceOfTransient) &theSeq,
                       const gp_Trsf &theTrsf)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) {
    return false;
  }

  TopoDS_Shape aShape = theShape->GetValue();

  if (aShape.IsNull()) {
    SetErrorCode("Shape is not defined");
    return false;
  }

  // Compute tolerance
  Standard_Real aMaxTol = -RealLast();
  TopExp_Explorer anExp(aShape, TopAbs_VERTEX);

  for (; anExp.More(); anExp.Next()) {
    TopoDS_Vertex aVertex = TopoDS::Vertex(anExp.Current());

    if (aVertex.IsNull() == Standard_False) {
      const Standard_Real aTol = BRep_Tool::Tolerance(aVertex);

      if (aTol > aMaxTol) {
        aMaxTol = aTol;
      }
    }
  }

  // Construct internal surfaces.
  Standard_Integer i = 0;
  const Standard_Integer aMaxNbSurf = 5;
  Handle(Geom_Surface) aSurface[aMaxNbSurf];
  TopTools_ListOfShape aConicalFaces;
  Standard_Real aTolConf = Precision::Confusion();

  // 1. Construct the internal surface of main pipe.
  gp_Ax2 anAxis1 (gp::Origin(), gp::DX(), gp::DZ());
  gp_Ax2 anAxis2 (gp::Origin(), gp::DZ(), gp::DX());

  aSurface[i++] = new Geom_CylindricalSurface(anAxis1, theR1);

  // 2. Construct the internal surface of incident pipe.
  aSurface[i++] = new Geom_CylindricalSurface(anAxis2, theR2);

  // 3. Construct the internal surface of left reduction pipe.
  if (theRL > aTolConf) {
    aSurface[i++] = new Geom_CylindricalSurface(anAxis1, theRL);

    if (theTransLenL > aTolConf) {
      // 3.1. Construct the internal surface of left transition pipe.
      gp_Pnt aPLeft (-theLen1, 0., 0.);
      gp_Ax2 anAxisLeft (aPLeft, -gp::DX(), gp::DZ());
      TopoDS_Shape aConeLeft =
        MakeConicalFace(anAxisLeft, theR1, theRL, theTransLenL, theTrsf);

      if (aConeLeft.IsNull() == Standard_False) {
        aConicalFaces.Append(aConeLeft);
      }
    }
  }

  // 4. Construct the internal surface of right reduction pipe.
  if (theRR > aTolConf) {
    // There is no need to construct another cylinder of the same radius. Skip it.
    if (Abs(theRR - theRL) > aTolConf) {
      aSurface[i++] = new Geom_CylindricalSurface(anAxis1, theRR);
    }

    if (theTransLenL > aTolConf) {
      // 4.1. Construct the internal surface of right transition pipe.
      gp_Pnt aPRight (theLen1, 0., 0.);
      gp_Ax2 anAxisRight (aPRight, gp::DX(), gp::DZ());
      TopoDS_Shape aConeRight =
        MakeConicalFace(anAxisRight, theR1, theRR, theTransLenR, theTrsf);

      if (aConeRight.IsNull() == Standard_False) {
        aConicalFaces.Append(aConeRight);
      }
    }
  }

  // 5. Construct the internal surface of incident reduction pipe.
  if (theRI > aTolConf) {
    aSurface[i++] = new Geom_CylindricalSurface(anAxis2, theRI);

    if (theTransLenI > aTolConf) {
      // 5.1. Construct the internal surface of incident transition pipe.
      gp_Pnt aPInci (0., 0., theLen2);
      gp_Ax2 anAxisInci (aPInci, gp::DZ(), gp::DX());
      TopoDS_Shape aConeInci =
        MakeConicalFace(anAxisInci, theR2, theRI, theTransLenI, theTrsf);

      if (aConeInci.IsNull() == Standard_False) {
        aConicalFaces.Append(aConeInci);
      }
    }
  }

  // Get faces that are laying on cylindrical surfaces.
  TopTools_ListOfShape aFaces;
  gp_Trsf anInvTrsf = theTrsf.Inverted();

  for (i = 0; i < aMaxNbSurf; i++) {
    if (aSurface[i].IsNull()) {
      break;
    }

    aSurface[i]->Transform(theTrsf);

    TopTools_ListOfShape aLocalFaces;

    if (!GetFacesOnSurf(aShape, aSurface[i], aMaxTol, aLocalFaces)) {
      return false;
    }

    if (i < 2) {
      // Check if the result contains outer cylinders.
      // It is required for main and incident pipes.
      TopTools_ListIteratorOfListOfShape anIter(aLocalFaces);

      while (anIter.More()) {
        TopExp_Explorer anExp(anIter.Value(), TopAbs_VERTEX);
        Standard_Boolean isInside = Standard_False;

        // Get a vertex from this shape
        if (anExp.More()) {
          TopoDS_Vertex aVtx = TopoDS::Vertex(anExp.Current());

          if (aVtx.IsNull() == Standard_False) {
            gp_Pnt aPnt = BRep_Tool::Pnt(aVtx);

            aPnt.Transform(anInvTrsf);

            if (i == 0) {
              // Check if the point is inside the main pipe.
              isInside = (Abs(aPnt.X()) <= theLen1);
            } else { // i == 1
              // Check if the point is inside the incident pipe.
              isInside = (aPnt.Z() <= theLen2);
            }
          }
        }

        if (isInside) {
          // Keep this face.
          anIter.Next();
        } else {
          // Remove this face.
          aLocalFaces.Remove(anIter);
        }
      }
    }

    aFaces.Append(aLocalFaces);
  }

  // Get faces that are laying on conical faces.
  if (aConicalFaces.IsEmpty() == Standard_False) {
    Handle(GEOM_Object) aCone =
      GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);
    Handle(GEOM_Function) aFunction =
      aCone->AddFunction(AdvancedEngine_PipeTShapeDriver::GetID(), TSHAPE_BASIC);
    TopTools_ListIteratorOfListOfShape aFIter(aConicalFaces);
    Handle(GEOM_Object) aConeFromShape;

    for (; aFIter.More(); aFIter.Next()) {
      aFunction->SetValue(aFIter.Value());
      aConeFromShape = myShapesOperations->GetInPlace(theShape, aCone);

      if (aConeFromShape.IsNull() == Standard_False) {
        aConeFromShape->GetLastFunction()->SetDescription("");
        TopoDS_Shape aConeFaces = aConeFromShape->GetValue();
        TopExp_Explorer anExp(aConeFaces, TopAbs_FACE);

        for (; anExp.More(); anExp.Next()) {
          TopoDS_Face aConeFace = TopoDS::Face(anExp.Current());

          if (aConeFace.IsNull() == Standard_False) {
            aFaces.Append(aConeFace);
          }
        }
      }
    }
  }

  // Create a group of internal faces.
  if (aFaces.IsEmpty() == Standard_False) {
    Handle(GEOM_Object) aGroup = myGroupOperations->CreateGroup(theShape, TopAbs_FACE);

    if (aGroup.IsNull() == Standard_False) {
      aGroup->GetLastFunction()->SetDescription("");
      aGroup->SetName("INTERNAL_FACES");

      TopTools_IndexedMapOfShape anIndices;
      Handle(TColStd_HSequenceOfInteger) aSeqIDs = new TColStd_HSequenceOfInteger;

      TopExp::MapShapes(aShape, anIndices);

      TopTools_ListIteratorOfListOfShape anIter(aFaces);

      for (; anIter.More(); anIter.Next()) {
        const TopoDS_Shape &aFace = anIter.Value();
        const Standard_Integer anIndex = anIndices.FindIndex(aFace);

        if (anIndex > 0) {
          aSeqIDs->Append(anIndex);
        }
      }

      myGroupOperations->UnionIDs(aGroup, aSeqIDs);
      aGroup->GetLastFunction()->SetDescription("");
      theSeq->Append(aGroup);
    }
  }

  SetErrorCode(OK);

  return true;
}

bool AdvancedEngine_IOperations::MakePipeTShapePartition(Handle(GEOM_Object) theShape,
                                                         double theR1, double theW1, double theL1,
                                                         double theR2, double theW2, double theL2,
                                                         double theH, double theW,
                                                         double theRF, bool isNormal)
{
  SetErrorCode(KO);

  // Build tools for partition operation:
  // 1 face and 2 planes
  // Face
  Handle(GEOM_Object) arete_intersect_int, arete_intersect_ext;
  Handle(GEOM_Object) wire_t, wire_t2, face_t, face_t2;
  Handle(GEOM_Object) chan_racc;
  Handle(GEOM_Object) vi1, vi2;
  Handle(GEOM_Object) Te3;

  try {
    OCC_CATCH_SIGNALS;
    Handle(GEOM_Object) Vector_Z = myBasicOperations->MakeVectorDXDYDZ(0, 0, 1);
    Vector_Z->GetLastFunction()->SetDescription("");

    // Useful values
    double aSize = 2*(theL1 + theL2);
    double aR1Ext = theR1 + theW1;
    double aR2Ext = theR2 + theW2;
    double theVertCylinderRadius = aR2Ext + theW + theRF;
    double theHoriCylinderRadius = aR1Ext + theH + theRF;

    // Common edges on internal cylinder
    Handle(GEOM_Object) box_i = my3DPrimOperations->MakeBoxDXDYDZ(theR2, theR2, theR1);
    box_i->GetLastFunction()->SetDescription("");
    box_i = myTransformOperations->TranslateDXDYDZ(box_i, -theR2, -theR2, 0);
    box_i->GetLastFunction()->SetDescription("");

    Handle(GEOM_Function) aFunction = theShape->GetLastFunction();
    TCollection_AsciiString theDesc = aFunction->GetDescription();
    Handle(TColStd_HSequenceOfTransient) edges_i =
      myShapesOperations->GetShapesOnBox(box_i, theShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
    // Recover previous description to get rid of Propagate dump
    aFunction->SetDescription(theDesc);
    if (edges_i.IsNull() || edges_i->Length() == 0) {
      SetErrorCode("Internal edges not found");
      return false;
    }
    for (int i=1; i<=edges_i->Length();i++) {
      Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast(edges_i->Value(i));
      anObj->GetLastFunction()->SetDescription("");
    }
    arete_intersect_int = Handle(GEOM_Object)::DownCast(edges_i->Value(1));

    // search for vertices located on both internal pipes
    aFunction = theShape->GetLastFunction();
    theDesc = aFunction->GetDescription();
    Handle(TColStd_HSequenceOfTransient) vertices_i =
      myShapesOperations->GetShapesOnBox(box_i, theShape, TopAbs_VERTEX, GEOMAlgo_ST_ONIN);
    // Recover previous description to get rid of Propagate dump
    aFunction->SetDescription(theDesc);
    if (vertices_i.IsNull() || vertices_i->Length() == 0) {
      SetErrorCode("Internal vertices not found");
      return false;
    }

    double d1min = theR2+theW2, d2min=theR2+theW2;
    for (int i = 1; i <= vertices_i->Length(); i++) {
      Handle(GEOM_Object) v = Handle(GEOM_Object)::DownCast(vertices_i->Value(i));
      v->GetLastFunction()->SetDescription("");
      TopoDS_Vertex aVertex = TopoDS::Vertex(v->GetValue());
      gp_Pnt aP = BRep_Tool::Pnt(aVertex);
      if (Abs(aP.X()) <= Precision::Confusion()) {
        if (Abs(aP.Y()) < d1min) {
          vi1 = v;
          d1min = Abs(aP.Y());
        }
      } else if (Abs(aP.Y()) <= Precision::Confusion()) {
        if (Abs(aP.X()) < d2min) {
          vi2 = v;
          d2min = Abs(aP.X());
        }
      }
    }
    if (vi1.IsNull() || vi2.IsNull()) {
      SetErrorCode("Cannot find internal intersection vertices");
      return false;
    }

    std::list<Handle(GEOM_Object)> theShapes;

    if (isNormal) {
      Handle(GEOM_Object) ve1, ve2;
      TopoDS_Vertex vertex1, vertex2;

      Handle(GEOM_Object) box_e = my3DPrimOperations->MakeBoxDXDYDZ(aR2Ext, aR2Ext, aR1Ext);
      box_e->GetLastFunction()->SetDescription("");
      box_e = myTransformOperations->TranslateDXDYDZ(box_e, -aR2Ext, -aR2Ext, 0);
      box_e->GetLastFunction()->SetDescription("");

      // search for vertices located on both external pipes
      aFunction = theShape->GetLastFunction();
      theDesc = aFunction->GetDescription();
      Handle(TColStd_HSequenceOfTransient) vertices_e =
        myShapesOperations->GetShapesOnBox(box_e, theShape, TopAbs_VERTEX, GEOMAlgo_ST_ONIN);
      // Recover previous description to get rid of Propagate dump
      aFunction->SetDescription(theDesc);
      if (vertices_e.IsNull() || vertices_e->Length() == 0) {
        SetErrorCode("External vertices not found");
        return false;
      }

      double d1max = 0, d2max = 0;
      for (int i = 1; i <= vertices_e->Length(); i++) {
        Handle(GEOM_Object) v = Handle(GEOM_Object)::DownCast(vertices_e->Value(i));
        v->GetLastFunction()->SetDescription("");
        TopoDS_Vertex aVertex = TopoDS::Vertex(v->GetValue());
        gp_Pnt aP = BRep_Tool::Pnt(aVertex);
        if (Abs(aP.X()) <= Precision::Confusion()) {
          if (Abs(aP.Y()) > d1max) {
            ve1 = v;
            vertex1 = aVertex;
            d1max = Abs(aP.Y());
          }
        } else if (Abs(aP.Y()) <= Precision::Confusion()) {
          if (Abs(aP.X()) > d2max) {
            ve2 = v;
            vertex2 = aVertex;
            d2max = Abs(aP.X());
          }
        }
      }
      if (ve1.IsNull() || ve2.IsNull()) {
        SetErrorCode("Cannot find external intersection vertices");
        return false;
      }
      Handle(GEOM_Object) edge_e1, edge_e2;

      // Common edges on external cylinder
      aFunction = theShape->GetLastFunction();
      theDesc = aFunction->GetDescription();
      Handle(TColStd_HSequenceOfTransient) edges_e =
        myShapesOperations->GetShapesOnBox(box_e, theShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
      // Recover previous description to get rid of Propagate dump
      aFunction->SetDescription(theDesc);
      if (edges_e.IsNull() || edges_e->Length() == 0) {
        SetErrorCode("External edges not found");
        return false;
      }
      ShapeAnalysis_Edge sae;
      for (int i=1; i<=edges_e->Length();i++) {
        Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast(edges_e->Value(i));
        anObj->GetLastFunction()->SetDescription("");
        TopoDS_Edge anEdge = TopoDS::Edge(anObj->GetValue());
        if ( !anEdge.IsNull() && 
             (sae.FirstVertex(anEdge).IsSame(vertex1) || sae.LastVertex(anEdge).IsSame(vertex1)) && 
             (sae.FirstVertex(anEdge).IsSame(vertex2) || sae.LastVertex(anEdge).IsSame(vertex2))) {
          arete_intersect_ext = anObj;
        }
      }

      edge_e1 = myBasicOperations->MakeLineTwoPnt(ve1, vi1);
      if (edge_e1.IsNull()) {
        SetErrorCode("Edge 1 could not be built");
        return false;
      }

      edge_e2 = myBasicOperations->MakeLineTwoPnt(ve2, vi2);
      if (edge_e2.IsNull()) {
        SetErrorCode("Edge 2 could not be built");
        return false;
      }

      edge_e1->GetLastFunction()->SetDescription("");
      edge_e2->GetLastFunction()->SetDescription("");

      std::list<Handle(GEOM_Object)> edge_e_elist;
      edge_e_elist.push_back(arete_intersect_int);
      edge_e_elist.push_back(edge_e1);
      edge_e_elist.push_back(arete_intersect_ext);
      edge_e_elist.push_back(edge_e2);
      wire_t = myShapesOperations->MakeWire(edge_e_elist, 1e-7);
      if (wire_t.IsNull()) {
        SetErrorCode("Impossible to build wire");
        return false;
      }
      wire_t->GetLastFunction()->SetDescription("");
      face_t = myShapesOperations->MakeFace(wire_t, false);
      if (face_t.IsNull()) {
        SetErrorCode("Impossible to build face");
        return false;
      }
      face_t->GetLastFunction()->SetDescription("");

      theShapes.push_back(theShape);
      theShapes.push_back(vi1);
      theShapes.push_back(vi2);
      theShapes.push_back(ve1);
      theShapes.push_back(ve2);
      theShapes.push_back(edge_e1);
      theShapes.push_back(edge_e2);
      theShapes.push_back(wire_t);
      theShapes.push_back(face_t);
    }
    else {
      Handle(GEOM_Object) P1, P2, P3, P4, P5, P6;
      int idP1, idP2, idP3, idP4;
      int PZX, PZY;
      double ZX=0, ZY=0;
      std::vector<int> LX;
      std::vector<int> LY;
      Handle(GEOM_Object) box_e = my3DPrimOperations->MakeBoxDXDYDZ
        (theVertCylinderRadius, theVertCylinderRadius, theHoriCylinderRadius);
      box_e->GetLastFunction()->SetDescription("");
      box_e = myTransformOperations->TranslateDXDYDZ
        (box_e, -theVertCylinderRadius, -theVertCylinderRadius, 0);
      box_e->GetLastFunction()->SetDescription("");

      aFunction = theShape->GetLastFunction();
      theDesc = aFunction->GetDescription();
      Handle(TColStd_HSequenceOfTransient) extremVertices =
        myShapesOperations->GetShapesOnBox(box_e, theShape, TopAbs_VERTEX, GEOMAlgo_ST_ONIN);
      // Recover previous description to get rid of Propagate dump
      aFunction->SetDescription(theDesc);

      if (extremVertices.IsNull() || extremVertices->Length() == 0) {
        if (theRF == 0)
          SetErrorCode("Vertices on chamfer not found");
        else
          SetErrorCode("Vertices on fillet not found");
        return false;
      }

      theShapes.push_back(theShape);
      theShapes.push_back(box_e);
      if (extremVertices->Length() != 6) {
        //           for (int i=1; i<=extremVertices->Length(); i++){
        //             theShapes.push_back(Handle(GEOM_Object)::DownCast(extremVertices->Value(i)));
        //           }
        //           Handle(GEOM_Object) aCompound = myShapesOperations->MakeCompound(theShapes);
        //           TopoDS_Shape aCompoundShape = aCompound->GetValue();
        //           theShape->GetLastFunction()->SetValue(aCompoundShape);
        SetErrorCode("Bad number of vertices on chamfer found");
        return false;
      }

      for (int i=1; i<=extremVertices->Length(); i++){
        Handle(GEOM_Object) aV = Handle(GEOM_Object)::DownCast(extremVertices->Value(i));
        aV->GetLastFunction()->SetDescription("");
        gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(aV->GetValue()));

        if (Abs(aP.X()) <= Precision::Confusion()) {
          if (Abs(aP.Y()) - theR2 > Precision::Confusion()) {
            LX.push_back(i);
            if  (aP.Z()-ZX > Precision::Confusion()) {
              ZX = aP.Z();
              PZX = i;
            }
          }
        }
        else {
          if (Abs(aP.X()) - theR2 > Precision::Confusion()) {
            LY.push_back(i);
            if (aP.Z() - ZY > Precision::Confusion()) {
              ZY = aP.Z();
              PZY = i;
            }
          }
        }
      }

      idP2 = PZX;
      idP4 = PZY;
      idP1 = LX.at(0);
      if (LX.at(0) == PZX)
        idP1 = LX.at(1);
      idP3 = LY.at(0);
      if (LY.at(0) == PZY)
        idP3 = LY.at(1);

      P1 = Handle(GEOM_Object)::DownCast(extremVertices->Value(idP1));
      P2 = Handle(GEOM_Object)::DownCast(extremVertices->Value(idP2));
      P3 = Handle(GEOM_Object)::DownCast(extremVertices->Value(idP3));
      P4 = Handle(GEOM_Object)::DownCast(extremVertices->Value(idP4));

      Handle(GEOM_Object) Cote_1 = myBasicOperations->MakeLineTwoPnt(P1, vi1);
      if (Cote_1.IsNull()) {
        SetErrorCode("Impossible to build edge in thickness");
        return false;
      }
      Cote_1->GetLastFunction()->SetDescription("");

      Handle(GEOM_Object) Cote_2 = myBasicOperations->MakeLineTwoPnt(vi2, P3);
      if (Cote_2.IsNull()) {
        SetErrorCode("Impossible to build edge in thickness");
        return false;
      }
      Cote_2->GetLastFunction()->SetDescription("");

      // edge_chan_princ = arete du chanfrein (ou raccord) sur le tuyau principal
      // edge_chan_inc = arete du chanfrein (ou raccord) sur le tuyau incident
      //         std::cerr << "Getting chamfer edge on main pipe" << std::endl;
      Handle(GEOM_Object) edge_chan_princ = myBlocksOperations->GetEdge(theShape, P1, P3);
      if (edge_chan_princ.IsNull()) {
        SetErrorCode("Impossible to find edge on main pipe");
        return false;
      }
      edge_chan_princ->GetLastFunction()->SetDescription("");

      Handle(GEOM_Object) edge_chan_inc = myBlocksOperations->GetEdge(theShape, P2, P4);
      if (edge_chan_inc.IsNull()) {
        SetErrorCode("Impossible to find edge on incident pipe");
        return false;
      }
      edge_chan_inc->GetLastFunction()->SetDescription("");

      std::list<Handle(GEOM_Object)> edgeList1;
      edgeList1.push_back(edge_chan_princ);
      edgeList1.push_back(Cote_1);
      edgeList1.push_back(arete_intersect_int);
      edgeList1.push_back(Cote_2);

      //         std::cerr << "Creating wire 1" << std::endl;
      wire_t = myShapesOperations->MakeWire(edgeList1, 1e-7);
      if (wire_t.IsNull()) {
        SetErrorCode("Impossible to build wire");
        return false;
      }
      wire_t->GetLastFunction()->SetDescription("");

      //         std::cerr << "Creating face 1" << std::endl;
      face_t = myShapesOperations->MakeFace(wire_t, false);
      if (face_t.IsNull()) {
        SetErrorCode("Impossible to build face");
        return false;
      }
      face_t->GetLastFunction()->SetDescription("");
      theShapes.push_back(face_t);

      // Create a prism from edge_chan_inc
      Handle(GEOM_Object) aPrismDir = myBasicOperations->MakeVectorDXDYDZ(1., 1., 0.);

      if (aPrismDir.IsNull()) {
        SetErrorCode("Impossible to build Prism direction");
        return false;
      }
      aPrismDir->GetLastFunction()->SetDescription("");
      face_t2 = my3DPrimOperations->MakePrismVecH(edge_chan_inc, aPrismDir, theR2 + theW2);

      if (face_t2.IsNull()) {
        SetErrorCode("Impossible to build face");
        return false;
      }
      face_t2->GetLastFunction()->SetDescription("");
      theShapes.push_back(face_t2);
    }

    // Planes
    Handle(GEOM_Object) aP0 = myBasicOperations->MakePointXYZ(0, 0, 0);
    Handle(GEOM_Object) aVZ = myBasicOperations->MakeVectorDXDYDZ(0, 0, 1);
    Handle(GEOM_Object) aVXZ = myBasicOperations->MakeVectorDXDYDZ(aR1Ext, 0, 0.5*(theL1+theVertCylinderRadius));
    Handle(GEOM_Object) aPlnOZ = myBasicOperations->MakePlanePntVec(aP0, aVZ, aSize);
    Handle(GEOM_Object) aPlnOXZ = myBasicOperations->MakePlanePntVec(aP0, aVXZ, aSize);
    aP0->GetLastFunction()->SetDescription("");
    aVZ->GetLastFunction()->SetDescription("");
    aVXZ->GetLastFunction()->SetDescription("");
    aPlnOZ->GetLastFunction()->SetDescription("");
    aPlnOXZ->GetLastFunction()->SetDescription("");
    theShapes.push_back(aPlnOZ);
    theShapes.push_back(aPlnOXZ);

    // Partition
    Handle(TColStd_HSequenceOfTransient) partitionShapes = new TColStd_HSequenceOfTransient;
    Handle(TColStd_HSequenceOfTransient) theTools = new TColStd_HSequenceOfTransient;
    Handle(TColStd_HSequenceOfTransient) theKeepInside = new TColStd_HSequenceOfTransient;
    Handle(TColStd_HSequenceOfTransient) theRemoveInside = new TColStd_HSequenceOfTransient;
    Handle(TColStd_HArray1OfInteger) theMaterials;

    partitionShapes->Append(theShape);
    theTools->Append(aPlnOZ);
    if (Abs(aR1Ext - aR2Ext) > Precision::Confusion())
      theTools->Append(aPlnOXZ);
    theTools->Append(face_t);
    if (!isNormal)
      theTools->Append(face_t2);

    Te3 = myBooleanOperations->MakePartition
              (partitionShapes, theTools, theKeepInside, theRemoveInside,
              TopAbs_SOLID, false, theMaterials, 0, false, Standard_False);
    if (Te3.IsNull()) {
      SetErrorCode("Impossible to build partition of TShape");
      return false;
    }
    Te3->GetLastFunction()->SetDescription("");

    // Last verification: result should be a block
    std::list<GEOMImpl_IBlocksOperations::BCError> errList;
    if (!myBlocksOperations->CheckCompoundOfBlocks(Te3, -1, errList)) {
      SetErrorCode("TShape is not a compound of block");
      return false;
    }

//     // BEGIN Compound of created shapes - Only for debug purpose
//     theShapes.clear();
//     theShapes.push_back(theShape);
//     theShapes.push_back(aPlnOZ);
//     if (Abs(aR1Ext - aR2Ext) > Precision::Confusion() )
//       theShapes.push_back(aPlnOXZ);
//     theShapes.push_back(face_t);
//     if (!isNormal)
//       theShapes.push_back(face_t2);
//
//     Handle(GEOM_Object) aCompound = myShapesOperations->MakeCompound(theShapes);
//     TopoDS_Shape aCompoundShape = aCompound->GetValue();
//     theShape->GetLastFunction()->SetValue(aCompoundShape);
//     // END Compound of created shapes - Only for debug purpose

    TopoDS_Shape aShape = Te3->GetValue();
    theShape->GetLastFunction()->SetValue(aShape);
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return false;
  }

  SetErrorCode(OK);
  return true;
}

// Mirror and glue faces
bool AdvancedEngine_IOperations::MakePipeTShapeMirrorAndGlue(Handle(GEOM_Object) theShape,
                                                             double theR1, double theW1, double theL1,
                                                             double theR2, double theW2, double theL2)
{
  SetErrorCode(KO);

  // Useful values
  double aSize = 2*(theL1 + theL2);
  double aR1Ext = theR1 + theW1;

  // Planes
  Handle(GEOM_Object) aP0 = myBasicOperations->MakePointXYZ(0, 0, 0);
  aP0->GetLastFunction()->SetDescription("");
  Handle(GEOM_Object) aVX = myBasicOperations->MakeVectorDXDYDZ(1, 0, 0);
  Handle(GEOM_Object) aVY = myBasicOperations->MakeVectorDXDYDZ(0, 1, 0);
  aVX->GetLastFunction()->SetDescription("");
  aVY->GetLastFunction()->SetDescription("");
  Handle(GEOM_Object) aPlane_OX = myBasicOperations->MakePlanePntVec(aP0, aVX, 2*(aR1Ext + theL2));
  Handle(GEOM_Object) aPlane_OY = myBasicOperations->MakePlanePntVec(aP0, aVY, aSize);
  aPlane_OX->GetLastFunction()->SetDescription("");
  aPlane_OY->GetLastFunction()->SetDescription("");

  Handle(GEOM_Object) Te4 = myTransformOperations->MirrorPlaneCopy(theShape, aPlane_OX);
  if (Te4.IsNull()) {
    SetErrorCode("Impossible to build mirror of quarter TShape");
    return false;
  }

  Handle(GEOM_Object) Te5 = myTransformOperations->MirrorPlaneCopy(theShape, aPlane_OY);
  if (Te5.IsNull()) {
    SetErrorCode("Impossible to build mirror of half TShape");
    return false;
  }

  Handle(GEOM_Object) Te6 = myTransformOperations->MirrorPlaneCopy(Te4, aPlane_OY);
  if (Te6.IsNull()) {
    SetErrorCode("Impossible to build mirror of half TShape");
    return false;
  }

  std::list<Handle(GEOM_Object)> aShapesList;
  aShapesList.push_back(theShape);
  aShapesList.push_back(Te4);
  aShapesList.push_back(Te5);
  aShapesList.push_back(Te6);
  Handle(GEOM_Object) Te7 = myShapesOperations->MakeCompound(aShapesList);
  if (Te7.IsNull()) {
    SetErrorCode("Impossible to build compound");
    return false;
  }

  // Copy source shape
  TopoDS_Shape aShapeCopy;
  TColStd_IndexedDataMapOfTransientTransient aMapTShapes;
  TNaming_CopyShape::CopyTool(Te7->GetValue(), aMapTShapes, aShapeCopy);

  std::list<Handle(GEOM_Object)> Te7list( 1, Te7 );
  Handle(GEOM_Object) Te8 = myShapesOperations->MakeGlueFaces(Te7list, 1e-7, true);
  if (Te8.IsNull()) {
    SetErrorCode("Impossible to glue faces of TShape");
    return false;
  }

  TopoDS_Shape aShape = Te8->GetValue();
  BRepCheck_Analyzer anAna (aShape, Standard_True);

  if (!anAna.IsValid()) {
    // Try to do gluing with the tolerance equal to maximal
    // tolerance of vertices of the source shape.
    Standard_Real aTolMax = -RealLast();

    for (TopExp_Explorer ExV (aShapeCopy, TopAbs_VERTEX); ExV.More(); ExV.Next()) {
      TopoDS_Vertex aVertex = TopoDS::Vertex(ExV.Current());
      Standard_Real aTol = BRep_Tool::Tolerance(aVertex);

      if (aTol > aTolMax) {
        aTolMax = aTol;
      }
    }

    // Perform gluing
    Te7->GetLastFunction()->SetValue(aShapeCopy);
    Te8 = myShapesOperations->MakeGlueFaces(Te7list, aTolMax, true);

    if (Te8.IsNull()) {
      SetErrorCode("Impossible to glue faces of TShape");
      return false;
    }

    aShape = Te8->GetValue();
  }


  theShape->GetLastFunction()->SetValue(aShape);

  Te4->GetLastFunction()->SetDescription("");
  Te5->GetLastFunction()->SetDescription("");
  Te6->GetLastFunction()->SetDescription("");
  Te7->GetLastFunction()->SetDescription("");
  Te8->GetLastFunction()->SetDescription("");

  SetErrorCode(OK);
  return true;
}

//=======================================================================
//function : MakePipeTShapeThicknessReduction
//purpose  : Static method. Add thiskness reduction elements at the three
//                          open ends of the T-Shape.
//=======================================================================
TopoDS_Shape AdvancedEngine_IOperations::MakePipeTShapeThicknessReduction
                                  (TopoDS_Shape theShape,
                                   double r1, double w1, double l1,
                                   double r2, double w2, double l2,
                                   double rL, double wL, double ltransL, double lthinL,
                                   double rR, double wR, double ltransR, double lthinR,
                                   double rI, double wI, double ltransI, double lthinI,
                                   bool fuseReductions)
{
  // Add thickness reduction elements
  // at the three extremities: Left, Right and Incident
  //
  // ---------------------.
  //   W                   \
  // ---------------------. \
  //   ^                   \ '-----------------.
  //   |R                   \        Wthin     |
  //   |                     '-----------------'
  //   v                             Rthin
  // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--
  //                     Ltrans    Lthin

  TopoDS_Shape aResult = theShape;
  double aTol = Precision::Confusion();

  gp_Vec aVX = gp::DX(), aVZ = gp::DZ();

  // Left reduction (rL, wL, ltransL, lthinL)
  if (rL > aTol && wL > aTol && ltransL > aTol) {
    gp_Pnt aPLeft (-l1, 0, 0);
    gp_Ax2 anAxesLeft (aPLeft, -aVX, aVZ);
    TopoDS_Shape aReductionLeft = AdvancedEngine_IOperations::MakeThicknessReduction
      (anAxesLeft, r1, w1, rL, wL, ltransL, lthinL, fuseReductions);

    if (fuseReductions) {
      BRepAlgoAPI_Fuse fuseL (aResult, aReductionLeft);
      if (!fuseL.IsDone())
        StdFail_NotDone::Raise("Cannot fuse Te with left reduction");
      aResult = fuseL.Shape();
    }
    else {
      BRep_Builder B;
      TopoDS_Compound C;
      B.MakeCompound(C);
      B.Add(C, aResult);
      B.Add(C, aReductionLeft);
      aResult = C;
    }
  }

  // Right reduction
  if (rR > aTol && wR > aTol && ltransR > aTol) {
    gp_Pnt aPRight (l1, 0, 0);
    gp_Ax2 anAxesRight (aPRight, aVX, aVZ);
    TopoDS_Shape aReductionRight = AdvancedEngine_IOperations::MakeThicknessReduction
      (anAxesRight, r1, w1, rR, wR, ltransR, lthinR, fuseReductions);

    if (fuseReductions) {
      BRepAlgoAPI_Fuse fuseR (aResult, aReductionRight);
      if (!fuseR.IsDone())
        StdFail_NotDone::Raise("Cannot fuse Te with right reduction");
      aResult = fuseR.Shape();
    }
    else {
      BRep_Builder B;
      TopoDS_Compound C;
      B.MakeCompound(C);
      B.Add(C, aResult);
      B.Add(C, aReductionRight);
      aResult = C;
    }
  }

  // Incident reduction
  if (rI > aTol && wI > aTol && ltransI > aTol) {
    gp_Pnt aPInci (0, 0, l2);
    gp_Ax2 anAxesInci (aPInci, aVZ, aVX);
    TopoDS_Shape aReductionInci = AdvancedEngine_IOperations::MakeThicknessReduction
      (anAxesInci, r2, w2, rI, wI, ltransI, lthinI, fuseReductions);

    if (fuseReductions) {
      BRepAlgoAPI_Fuse fuseInci (aResult, aReductionInci);
      if (!fuseInci.IsDone())
        StdFail_NotDone::Raise("Cannot fuse Te with incident reduction");
      aResult = fuseInci.Shape();
    }
    else {
      BRep_Builder B;
      TopoDS_Compound C;
      B.MakeCompound(C);
      B.Add(C, aResult);
      B.Add(C, aReductionInci);
      aResult = C;
    }
  }

  // Get rid of extra compounds
  TopTools_ListOfShape listShapeRes;
  GEOMUtils::AddSimpleShapes(aResult, listShapeRes);
  aResult = listShapeRes.First(); // useful for the case "fuseReductions == true"

  if (!fuseReductions && listShapeRes.Extent() > 1) {
    // Simplify T-Shape compound (get rid of sub-compounds) and glue duplicated faces
    BRep_Builder B;
    TopoDS_Compound C;
    B.MakeCompound(C);

    TopTools_ListIteratorOfListOfShape itSub (listShapeRes);
    for (; itSub.More(); itSub.Next())
      B.Add(C, itSub.Value());

    // GlueFaces
    aResult = GEOMImpl_GlueDriver::GlueFaces(C, Precision::Confusion(), Standard_True);
  }

  return aResult;
}

//=======================================================================
//function : MakeThicknessReduction
//purpose  : Static method. Create one thickness reduction element.
//=======================================================================
TopoDS_Shape AdvancedEngine_IOperations::MakeThicknessReduction (gp_Ax2 theAxes,
                                                                 const double R, const double W,
                                                                 const double Rthin, const double Wthin,
                                                                 const double Ltrans, const double Lthin,
                                                                 bool fuse)
{
  double aTol = Precision::Confusion();
  if (Rthin < aTol || Wthin < aTol || Ltrans < aTol) {
    StdFail_NotDone::Raise("Cannot build thickness reduction: too small values");
  }
  bool isThinPart = (Lthin > aTol);

  //     .
  //   W |\
  //     . \
  //   ^  \ '-----------------.
  //   |R  \|                 | Wthin
  //   |    '-----------------'
  //   v                        Rthin
  // --.--.--.--.--.--.--.--.--.--.--.--.--> theAxes.Direction()
  //     Ltrans     Lthin

  double RExt = R + W;
  double RthinExt = Rthin + Wthin;

  gp_Dir aNormal = theAxes.Direction();
  gp_Dir anXDir  = theAxes.XDirection();
  gp_Pnt aPntCyl (theAxes.Location().XYZ() + aNormal.XYZ()*Ltrans);
  gp_Ax2 anAxesCyl (aPntCyl, aNormal, anXDir);

  // Build the transition part
  BRepPrimAPI_MakeCone ConeExt (theAxes, RExt, RthinExt, Ltrans);
  BRepPrimAPI_MakeCone ConeInt (theAxes, R, Rthin, Ltrans);
  ConeExt.Build();
  ConeInt.Build();
  if (!ConeExt.IsDone() || !ConeInt.IsDone())
    StdFail_NotDone::Raise("Cannot build cones of thickness reduction");
  BRepAlgoAPI_Cut cut1 (ConeExt.Shape(), ConeInt.Shape());
  if (!cut1.IsDone())
    StdFail_NotDone::Raise("Coudn't build transition part of thickness reduction");
  TopoDS_Shape aReduction = cut1.Shape();

  // Build the thin part, if required
  TopoDS_Shape aThinPart;
  if (isThinPart) {
    BRepPrimAPI_MakeCylinder CExt (anAxesCyl, RthinExt, Lthin);
    BRepPrimAPI_MakeCylinder CInt (anAxesCyl, Rthin, Lthin);
    CExt.Build();
    CInt.Build();
    if (!CExt.IsDone() || !CInt.IsDone())
      StdFail_NotDone::Raise("Cannot build cylinders of thickness reduction");
    BRepAlgoAPI_Cut cut2 (CExt.Shape(), CInt.Shape());
    if (!cut2.IsDone())
      StdFail_NotDone::Raise("Coudn't build thin part of thickness reduction");
    aThinPart = cut2.Shape();
  }

  // Join parts
  if (fuse) {
    if (isThinPart) {
      BRepAlgoAPI_Fuse fuse1 (aReduction, aThinPart);
      if (!fuse1.IsDone())
        StdFail_NotDone::Raise("Cannot fuse parts of thickness reduction");
      aReduction = fuse1.Shape();
    }
  }
  else {
    // Partition the reduction on blocks
    gp_Ax3 anAxesPln1 (aPntCyl, theAxes.XDirection(), aNormal);
    gp_Ax3 anAxesPln2 (aPntCyl, theAxes.YDirection(), aNormal);
    gp_Pln aPln1 (anAxesPln1);
    gp_Pln aPln2 (anAxesPln2);
    double aSize = Ltrans + Lthin + R + Rthin + Wthin; // to guarantee enough size in all directions
    TopoDS_Shape aTool1 = BRepBuilderAPI_MakeFace(aPln1, -aSize, +aSize, -aSize, +aSize).Shape();
    TopoDS_Shape aTool2 = BRepBuilderAPI_MakeFace(aPln2, -aSize, +aSize, -aSize, +aSize).Shape();

    GEOMAlgo_Splitter PS;
    PS.AddArgument(aReduction);
    if (isThinPart)
      PS.AddArgument(aThinPart);
    PS.AddTool(aTool1);
    PS.AddTool(aTool2);
    PS.SetLimit(TopAbs_SOLID);
    PS.Perform();

    aReduction = PS.Shape();
  }

  return aReduction;
}

//=============================================================================
/*!
 *  MakePipeTShape
 *  \brief Create a T-shape object with specified caracteristics for the main and
 *         the incident pipes (radius, width, half-length).
 *         Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
  AdvancedEngine_IOperations::MakePipeTShape(double theR1, double theW1, double theL1,
                                             double theR2, double theW2, double theL2,
                                             double theRL, double theWL, double theLtransL, double theLthinL,
                                             double theRR, double theWR, double theLtransR, double theLthinR,
                                             double theRI, double theWI, double theLtransI, double theLthinI,
                                             bool theHexMesh)
{
  MESSAGE("AdvancedEngine_IOperations::MakePipeTShape");
  SetErrorCode(KO);
  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);

  //Add a new shape function with parameters
  Handle(GEOM_Function) aFunction = aShape->AddFunction(AdvancedEngine_PipeTShapeDriver::GetID(), TSHAPE_BASIC);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != AdvancedEngine_PipeTShapeDriver::GetID()) return NULL;

  AdvancedEngine_IPipeTShape aData (aFunction);

  aData.SetR1(theR1);
  aData.SetW1(theW1);
  aData.SetL1(theL1);
  aData.SetR2(theR2);
  aData.SetW2(theW2);
  aData.SetL2(theL2);
  aData.SetHexMesh(theHexMesh);

  bool isTRL = (theRL + theWL + theLtransL + theLthinL) > Precision::Confusion();
  bool isTRR = (theRR + theWR + theLtransR + theLthinR) > Precision::Confusion();
  bool isTRI = (theRI + theWI + theLtransI + theLthinI) > Precision::Confusion();

  //Compute the resulting value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("TShape driver failed");
      return NULL;
    }

    if (theHexMesh) {
      if (!MakePipeTShapePartition(aShape, theR1, theW1, theL1, theR2, theW2, theL2))
        return NULL;
      if (!MakePipeTShapeMirrorAndGlue(aShape, theR1, theW1, theL1, theR2, theW2, theL2))
        return NULL;
    }

    if (isTRL || isTRR || isTRI) {
      // Add thickness reduction elements
      // at the three extremities: Left, Right and Incident
      TopoDS_Shape aResShape =
        MakePipeTShapeThicknessReduction(aShape->GetValue(), theR1, theW1, theL1, theR2, theW2, theL2,
                                         theRL, theWL, theLtransL, theLthinL,
                                         theRR, theWR, theLtransR, theLthinR,
                                         theRI, theWI, theLtransI, theLthinI,
                                         !theHexMesh);
      aFunction->SetValue(aResShape);
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
  aSeq->Append(aShape);

  try {
    if (theHexMesh) {
      // Get the groups
      if (!MakeGroups(aShape, TSHAPE_BASIC, theR1, theW1, theL1, theR2, theW2, theL2,
                      0., 0., 0., aSeq, gp_Trsf()))
        return NULL;
    }

    // Get internal group.
    if (!MakeInternalGroup(aShape, theR1, theL1, theR2, theL2, theRL, theLtransL,
                           theRR, theLtransR, theRI, theLtransI,
                           aSeq, gp_Trsf())) {
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aListRes("[");
  // Iterate over the sequence aSeq
  Standard_Integer aNbGroups = aSeq->Length();
  Standard_Integer i = 1;
  for (; i <= aNbGroups; i++) {
    Handle(Standard_Transient) anItem = aSeq->Value(i);
    if (anItem.IsNull()) continue;
    Handle(GEOM_Object) aGroup = Handle(GEOM_Object)::DownCast(anItem);
    if (aGroup.IsNull()) continue;
    //Make a Python command
    TDF_Tool::Entry(aGroup->GetEntry(), anEntry);
    aListRes += anEntry + ", ";
  }
  aListRes.Trunc(aListRes.Length() - 2);

  GEOM::TPythonDump pd (aFunction);

  pd << aListRes.ToCString() << "] = geompy.MakePipeTShape("
     << theR1 << ", " << theW1 << ", " << theL1 << ", "
     << theR2 << ", " << theW2 << ", " << theL2 << ", "
     << theHexMesh;

  // thickness reduction
  if (isTRL)
    pd << ", theRL=" << theRL << ", theWL=" << theWL
       << ", theLtransL=" << theLtransL << ", theLthinL=" << theLthinL;
  if (isTRR)
    pd << ", theRR=" << theRR << ", theWR=" << theWR
       << ", theLtransR=" << theLtransR << ", theLthinR=" << theLthinR;
  if (isTRI)
    pd << ", theRI=" << theRI << ", theWI=" << theWI
       << ", theLtransI=" << theLtransI << ", theLthinI=" << theLthinI;

  pd << ")";

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  MakePipeTShapeWithPosition
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length).
 *  The extremities of the main pipe are located on junctions points P1 and P2.
 *  The extremity of the incident pipe is located on junction point P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \param theP1 1st junction point of main pipe
 *  \param theP2 2nd junction point of main pipe
 *  \param theP3 Junction point of incident pipe
 *  \return List of GEOM_Objects, containing the created shape and propagation groups..
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
AdvancedEngine_IOperations::MakePipeTShapeWithPosition
                             (double theR1, double theW1, double theL1,
                              double theR2, double theW2, double theL2,
                              double theRL, double theWL, double theLtransL, double theLthinL,
                              double theRR, double theWR, double theLtransR, double theLthinR,
                              double theRI, double theWI, double theLtransI, double theLthinI,
                              bool theHexMesh,
                              Handle(GEOM_Object) theP1,
                              Handle(GEOM_Object) theP2,
                              Handle(GEOM_Object) theP3)
{
  SetErrorCode(KO);
  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);
  /////////////////
  // TSHAPE CODE
  /////////////////
  //Add a new shape function with parameters
  Handle(GEOM_Function) aFunction = aShape->AddFunction(AdvancedEngine_PipeTShapeDriver::GetID(), TSHAPE_BASIC);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != AdvancedEngine_PipeTShapeDriver::GetID()) return NULL;

  // Check new position
  if (!CheckCompatiblePosition(theL1, theL2, theP1, theP2, theP3, 0.01)) {
    return NULL;
  }

  AdvancedEngine_IPipeTShape aData(aFunction);

  aData.SetR1(theR1);
  aData.SetW1(theW1);
  aData.SetL1(theL1);
  aData.SetR2(theR2);
  aData.SetW2(theW2);
  aData.SetL2(theL2);
  aData.SetHexMesh(theHexMesh);

  bool isTRL = (theRL + theWL + theLtransL + theLthinL) > Precision::Confusion();
  bool isTRR = (theRR + theWR + theLtransR + theLthinR) > Precision::Confusion();
  bool isTRI = (theRI + theWI + theLtransI + theLthinI) > Precision::Confusion();

  //Compute the resulting value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("TShape driver failed");
      return NULL;
    }

    if (theHexMesh) {
      if (!MakePipeTShapePartition(aShape, theR1, theW1, theL1, theR2, theW2, theL2))
        return NULL;
      if (!MakePipeTShapeMirrorAndGlue(aShape, theR1, theW1, theL1, theR2, theW2, theL2))
        return NULL;
    }

    if (isTRL || isTRR || isTRI) {
      // Add thickness reduction elements
      // at the three extremities: Left, Right and Incident
      TopoDS_Shape aResShape =
        MakePipeTShapeThicknessReduction(aShape->GetValue(), theR1, theW1, theL1, theR2, theW2, theL2,
                                         theRL, theWL, theLtransL, theLthinL,
                                         theRR, theWR, theLtransR, theLthinR,
                                         theRI, theWI, theLtransI, theLthinI,
                                         !theHexMesh);
      aFunction->SetValue(aResShape);
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  TopoDS_Shape Te = aShape->GetValue();

  // Set Position
  gp_Trsf aTrsf = GetPositionTrsf(theL1, theL2, theP1, theP2, theP3);
  BRepBuilderAPI_Transform aTransformation(Te, aTrsf, Standard_False);
  TopoDS_Shape aTrsf_Shape = aTransformation.Shape();
  aFunction->SetValue(aTrsf_Shape);

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
  aSeq->Append(aShape);

  try {
    if (theHexMesh) {
      // Get the groups
      if (!MakeGroups(aShape,TSHAPE_BASIC, theR1, theW1, theL1, theR2, theW2, theL2,
                      0., 0., 0., aSeq, aTrsf)) {
        return NULL;
      }
    }

    // Get internal group.
    if (!MakeInternalGroup(aShape, theR1, theL1, theR2, theL2, theRL, theLtransL,
                           theRR, theLtransR, theRI, theLtransI,
                           aSeq, aTrsf)) {
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aListRes("[");
  // Iterate over the sequence aSeq
  Standard_Integer aNbGroups = aSeq->Length();
  Standard_Integer i = 1;
  for (; i <= aNbGroups; i++) {
    Handle(Standard_Transient) anItem = aSeq->Value(i);
    if (anItem.IsNull()) continue;
    Handle(GEOM_Object) aGroup = Handle(GEOM_Object)::DownCast(anItem);
    if (aGroup.IsNull()) continue;
    //Make a Python command
    TDF_Tool::Entry(aGroup->GetEntry(), anEntry);
    aListRes += anEntry + ", ";
  }
  aListRes.Trunc(aListRes.Length() - 2);

  GEOM::TPythonDump pd (aFunction);

  pd << aListRes.ToCString() << "] = geompy.MakePipeTShape("
     << theR1 << ", " << theW1 << ", " << theL1 << ", "
     << theR2 << ", " << theW2 << ", " << theL2 << ", "
     << theHexMesh << ", " << theP1 << ", " << theP2 << ", " << theP3;

  // thickness reduction
  if (isTRL)
    pd << ", theRL=" << theRL << ", theWL=" << theWL
       << ", theLtransL=" << theLtransL << ", theLthinL=" << theLthinL;
  if (isTRR)
    pd << ", theRR=" << theRR << ", theWR=" << theWR
       << ", theLtransR=" << theLtransR << ", theLthinR=" << theLthinR;
  if (isTRI)
    pd << ", theRI=" << theRI << ", theWI=" << theWI
       << ", theLtransI=" << theLtransI << ", theLthinI=" << theLthinI;

  pd << ")";

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  MakePipeTShapeChamfer
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A chamfer is created
 *  on the junction of the pipes.
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theH Height of chamfer.
 *  \param theW Width of chamfer.
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
AdvancedEngine_IOperations::MakePipeTShapeChamfer
                             (double theR1, double theW1, double theL1,
                              double theR2, double theW2, double theL2,
                              double theRL, double theWL, double theLtransL, double theLthinL,
                              double theRR, double theWR, double theLtransR, double theLthinR,
                              double theRI, double theWI, double theLtransI, double theLthinI,
                              double theH, double theW,
                              bool theHexMesh)
{
  SetErrorCode(KO);
  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);
  //Add a new shape function with parameters
  Handle(GEOM_Function) aFunction = aShape->AddFunction(AdvancedEngine_PipeTShapeDriver::GetID(), TSHAPE_CHAMFER);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != AdvancedEngine_PipeTShapeDriver::GetID()) return NULL;

  AdvancedEngine_IPipeTShape aData(aFunction);

  aData.SetR1(theR1);
  aData.SetW1(theW1);
  aData.SetL1(theL1);
  aData.SetR2(theR2);
  aData.SetW2(theW2);
  aData.SetL2(theL2);
  aData.SetH(theH);
  aData.SetW(theW);
  aData.SetHexMesh(theHexMesh);

  bool isTRL = (theRL + theWL + theLtransL + theLthinL) > Precision::Confusion();
  bool isTRR = (theRR + theWR + theLtransR + theLthinR) > Precision::Confusion();
  bool isTRI = (theRI + theWI + theLtransI + theLthinI) > Precision::Confusion();

  //Compute the resulting value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("TShape driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  // BEGIN of chamfer
  TopoDS_Shape aShapeShape = aShape->GetValue();
  TopTools_IndexedMapOfShape anEdgesIndices;
  TopExp::MapShapes(aShapeShape, anEdgesIndices);
  // Common edges on external cylinders
  Handle(GEOM_Object) box_e;
  if (theHexMesh) {
    box_e = my3DPrimOperations->MakeBoxDXDYDZ(theR2+theW2, theR2+theW2, theR1+theW1);
  }
  else {
    box_e = my3DPrimOperations->MakeBoxDXDYDZ(2*(theR2+theW2), 2*(theR2+theW2), theR1+theW1);
  }
  box_e->GetLastFunction()->SetDescription("");
  box_e = myTransformOperations->TranslateDXDYDZ(box_e, -(theR2+theW2), -(theR2+theW2), 0);
  box_e->GetLastFunction()->SetDescription("");

  Handle(TColStd_HSequenceOfInteger) edges_e =
    myShapesOperations->GetShapesOnBoxIDs(box_e, aShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
  box_e->GetLastFunction()->SetDescription("");

  if (edges_e.IsNull() || edges_e->Length() == 0) {
    SetErrorCode("External edges not found");
    return NULL;
  }
  int nbEdgesInChamfer = 0;
  std::list<int> theEdges;
  for (int i=1; i<=edges_e->Length();i++) {
    int edgeID = edges_e->Value(i);
    TopoDS_Shape theEdge = anEdgesIndices.FindKey(edgeID);
    TopExp_Explorer Ex(theEdge,TopAbs_VERTEX);
    int iv=0;
    while (Ex.More()) {
      iv ++;
      gp_Pnt aPt = BRep_Tool::Pnt(TopoDS::Vertex(Ex.Current()));
      if (Abs(aPt.Z() - (theR1+theW1)) <= Precision::Confusion()) {
        nbEdgesInChamfer ++;
        theEdges.push_back(edgeID);
      }
      Ex.Next();
    }
    if (theHexMesh && nbEdgesInChamfer == 1)
      break;
  }
  Handle(GEOM_Object) aChamfer;
  try {
    aChamfer = myLocalOperations->MakeChamferEdges(aShape, theW, theH, theEdges);
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }
  if (aChamfer.IsNull()) {
    SetErrorCode("Chamfer can not be computed on the given shape with the given parameters");
    return NULL;
  }
  aChamfer->GetLastFunction()->SetDescription("");

  TopoDS_Shape aChamferShape = aChamfer->GetValue();
  aFunction->SetValue(aChamferShape);
  // END of chamfer

  if (theHexMesh) {
    if (!MakePipeTShapePartition(aShape, theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, 0, false))
      return NULL;
    if (!MakePipeTShapeMirrorAndGlue(aShape, theR1, theW1, theL1, theR2, theW2, theL2))
      return NULL;
  }

  // Add thickness reduction elements
  // at the three extremities: Left, Right and Incident
  try {
    OCC_CATCH_SIGNALS;
    if (isTRL || isTRR || isTRI) {
      TopoDS_Shape aResShape =
        MakePipeTShapeThicknessReduction(aShape->GetValue(), theR1, theW1, theL1, theR2, theW2, theL2,
                                         theRL, theWL, theLtransL, theLthinL,
                                         theRR, theWR, theLtransR, theLthinR,
                                         theRI, theWI, theLtransI, theLthinI,
                                         !theHexMesh);
      aFunction->SetValue(aResShape);
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
  aSeq->Append(aShape);

  try {
    if (theHexMesh) {
      // Get the groups
      if (!MakeGroups(aShape, TSHAPE_CHAMFER, theR1, theW1, theL1, theR2, theW2, theL2,
                      theH, theW, 0., aSeq, gp_Trsf()))
        return NULL;
    }

    // Get internal group.
    if (!MakeInternalGroup(aShape, theR1, theL1, theR2, theL2, theRL, theLtransL,
                           theRR, theLtransR, theRI, theLtransI,
                           aSeq, gp_Trsf())) {
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aListRes("[");
  // Iterate over the sequence aSeq
  Standard_Integer aNbGroups = aSeq->Length();
  Standard_Integer i = 1;
  for (; i <= aNbGroups; i++) {
    Handle(Standard_Transient) anItem = aSeq->Value(i);
    if (anItem.IsNull()) continue;
    Handle(GEOM_Object) aGroup = Handle(GEOM_Object)::DownCast(anItem);
    if (aGroup.IsNull()) continue;
    //Make a Python command
    TDF_Tool::Entry(aGroup->GetEntry(), anEntry);
    aListRes += anEntry + ", ";
  }
  aListRes.Trunc(aListRes.Length() - 2);

  GEOM::TPythonDump pd (aFunction);

  pd << aListRes.ToCString() << "] = geompy.MakePipeTShapeChamfer("
     << theR1 << ", " << theW1 << ", " << theL1 << ", "
     << theR2 << ", " << theW2 << ", " << theL2 << ", "
     << theH << ", " << theW << ", " << theHexMesh;

  // thickness reduction
  if (isTRL)
    pd << ", theRL=" << theRL << ", theWL=" << theWL
       << ", theLtransL=" << theLtransL << ", theLthinL=" << theLthinL;
  if (isTRR)
    pd << ", theRR=" << theRR << ", theWR=" << theWR
       << ", theLtransR=" << theLtransR << ", theLthinR=" << theLthinR;
  if (isTRI)
    pd << ", theRI=" << theRI << ", theWI=" << theWI
       << ", theLtransI=" << theLtransI << ", theLthinI=" << theLthinI;

  pd << ")";

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  MakePipeTShapeChamferWithPosition
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A chamfer is created
 *  on the junction of the pipes.
 *  The extremities of the main pipe are located on junctions points P1 and P2.
 *  The extremity of the incident pipe is located on junction point P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theH Height of chamfer.
 *  \param theW Width of chamfer.
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \param theP1 1st junction point of main pipe
 *  \param theP2 2nd junction point of main pipe
 *  \param theP3 Junction point of incident pipe
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
AdvancedEngine_IOperations::MakePipeTShapeChamferWithPosition
                             (double theR1, double theW1, double theL1,
                              double theR2, double theW2, double theL2,
                              double theRL, double theWL, double theLtransL, double theLthinL,
                              double theRR, double theWR, double theLtransR, double theLthinR,
                              double theRI, double theWI, double theLtransI, double theLthinI,
                              double theH, double theW,
                              bool theHexMesh,
                              Handle(GEOM_Object) theP1,
                              Handle(GEOM_Object) theP2,
                              Handle(GEOM_Object) theP3)
{
  SetErrorCode(KO);
  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);
  //Add a new shape function with parameters
  Handle(GEOM_Function) aFunction = aShape->AddFunction(AdvancedEngine_PipeTShapeDriver::GetID(), TSHAPE_CHAMFER);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != AdvancedEngine_PipeTShapeDriver::GetID()) return NULL;

  // Check new position
  if (!CheckCompatiblePosition(theL1, theL2, theP1, theP2, theP3, 0.01)) {
    return NULL;
  }

  AdvancedEngine_IPipeTShape aData(aFunction);

  aData.SetR1(theR1);
  aData.SetW1(theW1);
  aData.SetL1(theL1);
  aData.SetR2(theR2);
  aData.SetW2(theW2);
  aData.SetL2(theL2);
  aData.SetH(theH);
  aData.SetW(theW);
  aData.SetHexMesh(theHexMesh);

  bool isTRL = (theRL + theWL + theLtransL + theLthinL) > Precision::Confusion();
  bool isTRR = (theRR + theWR + theLtransR + theLthinR) > Precision::Confusion();
  bool isTRI = (theRI + theWI + theLtransI + theLthinI) > Precision::Confusion();

  //Compute the resulting value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("TShape driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  // BEGIN of chamfer
  TopoDS_Shape aShapeShape = aShape->GetValue();
  TopTools_IndexedMapOfShape anEdgesIndices;
  TopExp::MapShapes(aShapeShape, anEdgesIndices);
  // Common edges on external cylinders
  Handle(GEOM_Object) box_e;
  if (theHexMesh) {
    box_e = my3DPrimOperations->MakeBoxDXDYDZ(theR2+theW2, theR2+theW2, theR1+theW1);
  }
  else {
    box_e = my3DPrimOperations->MakeBoxDXDYDZ(2*(theR2+theW2), 2*(theR2+theW2), theR1+theW1);
  }
  box_e->GetLastFunction()->SetDescription("");
  box_e = myTransformOperations->TranslateDXDYDZ(box_e, -(theR2+theW2), -(theR2+theW2), 0);
  box_e->GetLastFunction()->SetDescription("");

  Handle(TColStd_HSequenceOfInteger) edges_e =
    myShapesOperations->GetShapesOnBoxIDs(box_e, aShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
  box_e->GetLastFunction()->SetDescription("");

  if (edges_e.IsNull() || edges_e->Length() == 0) {
    SetErrorCode("External edges not found");
    return NULL;
  }
  int nbEdgesInChamfer = 0;
  std::list<int> theEdges;
  for (int i=1; i<=edges_e->Length();i++) {
    int edgeID = edges_e->Value(i);
    TopoDS_Shape theEdge = anEdgesIndices.FindKey(edgeID);
    TopExp_Explorer Ex(theEdge,TopAbs_VERTEX);
    while (Ex.More()) {
      gp_Pnt aPt = BRep_Tool::Pnt(TopoDS::Vertex(Ex.Current()));
      if (Abs(aPt.Z() - (theR1+theW1)) <= Precision::Confusion()) {
        nbEdgesInChamfer ++;
        theEdges.push_back(edgeID);
      }
      Ex.Next();
    }
    if (theHexMesh && nbEdgesInChamfer == 1)
      break;
  }
  Handle(GEOM_Object) aChamfer;
  try {
    aChamfer = myLocalOperations->MakeChamferEdges(aShape, theW, theH, theEdges);
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }
  if (aChamfer.IsNull()) {
    SetErrorCode("Chamfer can not be computed on the given shape with the given parameters");
    return NULL;
  }
  aChamfer->GetLastFunction()->SetDescription("");

  TopoDS_Shape aChamferShape = aChamfer->GetValue();
  aFunction->SetValue(aChamferShape);
  // END of chamfer

  if (theHexMesh) {
    if (!MakePipeTShapePartition(aShape, theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, 0, false))
      return NULL;
    if (!MakePipeTShapeMirrorAndGlue(aShape, theR1, theW1, theL1, theR2, theW2, theL2))
      return NULL;
  }

  // Add thickness reduction elements
  // at the three extremities: Left, Right and Incident
  try {
    OCC_CATCH_SIGNALS;
    if (isTRL || isTRR || isTRI) {
      TopoDS_Shape aResShape =
        MakePipeTShapeThicknessReduction(aShape->GetValue(), theR1, theW1, theL1, theR2, theW2, theL2,
                                         theRL, theWL, theLtransL, theLthinL,
                                         theRR, theWR, theLtransR, theLthinR,
                                         theRI, theWI, theLtransI, theLthinI,
                                         !theHexMesh);
      aFunction->SetValue(aResShape);
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  // Set Position
  gp_Trsf aTrsf = GetPositionTrsf(theL1, theL2, theP1, theP2, theP3);
  BRepBuilderAPI_Transform aTransformation (aShape->GetValue(), aTrsf, Standard_False);
  TopoDS_Shape aTrsf_Shape = aTransformation.Shape();
  aFunction->SetValue(aTrsf_Shape);

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
  aSeq->Append(aShape);

  try {
    if (theHexMesh) {
      // Get the groups
      if (!MakeGroups(aShape, TSHAPE_CHAMFER, theR1, theW1, theL1, theR2, theW2, theL2,
                      theH, theW, 0., aSeq, aTrsf))
        return NULL;
    }

    // Get internal group.
    if (!MakeInternalGroup(aShape, theR1, theL1, theR2, theL2, theRL, theLtransL,
                           theRR, theLtransR, theRI, theLtransI,
                           aSeq, aTrsf)) {
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aListRes("[");
  // Iterate over the sequence aSeq
  Standard_Integer aNbGroups = aSeq->Length();
  Standard_Integer i = 1;
  for (; i <= aNbGroups; i++) {
    Handle(Standard_Transient) anItem = aSeq->Value(i);
    if (anItem.IsNull()) continue;
    Handle(GEOM_Object) aGroup = Handle(GEOM_Object)::DownCast(anItem);
    if (aGroup.IsNull()) continue;
    //Make a Python command
    TDF_Tool::Entry(aGroup->GetEntry(), anEntry);
    aListRes += anEntry + ", ";
  }
  aListRes.Trunc(aListRes.Length() - 2);

  GEOM::TPythonDump pd (aFunction);

  pd << aListRes.ToCString() << "] = geompy.MakePipeTShapeChamfer("
     << theR1 << ", " << theW1 << ", " << theL1 << ", "
     << theR2 << ", " << theW2 << ", " << theL2 << ", "
     << theH << ", " << theW << ", " << theHexMesh << ", "
     << theP1 << ", " << theP2 << ", " << theP3;

  // thickness reduction
  if (isTRL)
    pd << ", theRL=" << theRL << ", theWL=" << theWL
       << ", theLtransL=" << theLtransL << ", theLthinL=" << theLthinL;
  if (isTRR)
    pd << ", theRR=" << theRR << ", theWR=" << theWR
       << ", theLtransR=" << theLtransR << ", theLthinR=" << theLthinR;
  if (isTRI)
    pd << ", theRI=" << theRI << ", theWI=" << theWI
       << ", theLtransI=" << theLtransI << ", theLthinI=" << theLthinI;

  pd << ")";

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  MakePipeTShapeFillet
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A fillet is created
 *  on the junction of the pipes.
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theRF Radius of curvature of fillet.
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
AdvancedEngine_IOperations::MakePipeTShapeFillet
                             (double theR1, double theW1, double theL1,
                              double theR2, double theW2, double theL2,
                              double theRL, double theWL, double theLtransL, double theLthinL,
                              double theRR, double theWR, double theLtransR, double theLthinR,
                              double theRI, double theWI, double theLtransI, double theLthinI,
                              double theRF, bool theHexMesh)
{
  SetErrorCode(KO);
  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);
  //Add a new shape function with parameters
  Handle(GEOM_Function) aFunction = aShape->AddFunction(AdvancedEngine_PipeTShapeDriver::GetID(), TSHAPE_FILLET);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != AdvancedEngine_PipeTShapeDriver::GetID()) return NULL;

  AdvancedEngine_IPipeTShape aData(aFunction);

  aData.SetR1(theR1);
  aData.SetW1(theW1);
  aData.SetL1(theL1);
  aData.SetR2(theR2);
  aData.SetW2(theW2);
  aData.SetL2(theL2);
  aData.SetRF(theRF);
  aData.SetHexMesh(theHexMesh);

  bool isTRL = (theRL + theWL + theLtransL + theLthinL) > Precision::Confusion();
  bool isTRR = (theRR + theWR + theLtransR + theLthinR) > Precision::Confusion();
  bool isTRI = (theRI + theWI + theLtransI + theLthinI) > Precision::Confusion();

  //Compute the resulting value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("TShape driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  // BEGIN of fillet
  TopoDS_Shape aShapeShape = aShape->GetValue();
  TopTools_IndexedMapOfShape anEdgesIndices;
  TopExp::MapShapes(aShapeShape, anEdgesIndices);
  // Common edges on external cylinders
  Handle(GEOM_Object) box_e;
  if (theHexMesh) {
    box_e = my3DPrimOperations->MakeBoxDXDYDZ(theR2+theW2, theR2+theW2, theR1+theW1);
  }
  else {
    box_e = my3DPrimOperations->MakeBoxDXDYDZ(2*(theR2+theW2), 2*(theR2+theW2), theR1+theW1);
  }
  box_e->GetLastFunction()->SetDescription("");
  box_e = myTransformOperations->TranslateDXDYDZ(box_e, -(theR2+theW2), -(theR2+theW2), 0);
  box_e->GetLastFunction()->SetDescription("");

  Handle(TColStd_HSequenceOfInteger) edges_e =
    myShapesOperations->GetShapesOnBoxIDs(box_e, aShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
  box_e->GetLastFunction()->SetDescription("");

  if (edges_e.IsNull() || edges_e->Length() == 0) {
    SetErrorCode("External edges not found");
    return NULL;
  }
  int nbEdgesInFillet = 0;
  std::list<int> theEdges;
  for (int i=1; i<=edges_e->Length();i++) {
    int edgeID = edges_e->Value(i);
    TopoDS_Shape theEdge = anEdgesIndices.FindKey(edgeID);
    TopExp_Explorer Ex(theEdge,TopAbs_VERTEX);
    while (Ex.More()) {
      gp_Pnt aPt = BRep_Tool::Pnt(TopoDS::Vertex(Ex.Current()));
      if (Abs(aPt.Z() - (theR1+theW1)) <= Precision::Confusion()) {
        nbEdgesInFillet ++;
        theEdges.push_back(edgeID);
      }
      Ex.Next();
    }
    if (theHexMesh && nbEdgesInFillet == 1)
      break;
  }

  Handle(GEOM_Object) aFillet;
  try {
    aFillet = myLocalOperations->MakeFilletEdges(aShape, theRF, theEdges);
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }
  if (aFillet.IsNull()) {
    //SetErrorCode("Fillet can not be computed on the given shape with the given parameters");
    SetErrorCode(myLocalOperations->GetErrorCode());
    return NULL;
  }
  aFillet->GetLastFunction()->SetDescription("");

  TopoDS_Shape aFilletShape = aFillet->GetValue();

#ifdef FILLET_FIX_TOLERANCE
  // VSR: 30/12/2014: temporary workaround about Fillet problem
  if (theHexMesh) {
    GEOMUtils::FixShapeTolerance(aFilletShape, TopAbs_FACE);
  }
  else {
    GEOMUtils::FixShapeCurves(aFilletShape);
  }
#endif

  aFunction->SetValue(aFilletShape);
  // END of fillet

// VSR: debug issues 0021568 and 0021550 (15/05/2012) - BEGIN (1)
// the following block, when enabled, leads to partitioning problems
#if 0
// VSR: debug issues 0021568 and 0021550 (15/05/2012) - END (1)
  // BEGIN: Limit tolerances (debug)
  Handle(GEOM_Object) aCorr1 = myHealingOperations->LimitTolerance(aShape, 1e-07);
  TopoDS_Shape aCorr1Shape = aCorr1->GetValue();
  aShape->GetLastFunction()->SetValue(aCorr1Shape);
  aCorr1->GetLastFunction()->SetDescription("");
  // END: Limit tolerances (debug)
// VSR: debug issues 0021568 and 0021550 (15/05/2012) - BEGIN (2)
#endif
// VSR: debug issues 0021568 and 0021550 (15/05/2012) - END (2)

  if (theHexMesh) {
    if (!MakePipeTShapePartition(aShape, theR1, theW1, theL1, theR2, theW2, theL2, 0, 0, theRF, false))
      return NULL;
    if (!MakePipeTShapeMirrorAndGlue(aShape, theR1, theW1, theL1, theR2, theW2, theL2))
      return NULL;
  }

  // Add thickness reduction elements
  // at the three extremities: Left, Right and Incident
  try {
    OCC_CATCH_SIGNALS;
    if (isTRL || isTRR || isTRI) {
      TopoDS_Shape aResShape =
        MakePipeTShapeThicknessReduction(aShape->GetValue(), theR1, theW1, theL1, theR2, theW2, theL2,
                                         theRL, theWL, theLtransL, theLthinL,
                                         theRR, theWR, theLtransR, theLthinR,
                                         theRI, theWI, theLtransI, theLthinI,
                                         !theHexMesh);
      aFunction->SetValue(aResShape);
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
  aSeq->Append(aShape);

  try {
    if (theHexMesh) {
      // Get the groups
      if (!MakeGroups(aShape, TSHAPE_FILLET, theR1, theW1, theL1, theR2, theW2, theL2,
                      0., 0., theRF, aSeq, gp_Trsf()))
        return NULL;
    }

    // Get internal group.
    if (!MakeInternalGroup(aShape, theR1, theL1, theR2, theL2, theRL, theLtransL,
                           theRR, theLtransR, theRI, theLtransI,
                           aSeq, gp_Trsf())) {
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aListRes("[");
  // Iterate over the sequence aSeq
  Standard_Integer aNbGroups = aSeq->Length();
  Standard_Integer i = 1;
  for (; i <= aNbGroups; i++) {
    Handle(Standard_Transient) anItem = aSeq->Value(i);
    if (anItem.IsNull()) continue;
    Handle(GEOM_Object) aGroup = Handle(GEOM_Object)::DownCast(anItem);
    if (aGroup.IsNull()) continue;
    //Make a Python command
    TDF_Tool::Entry(aGroup->GetEntry(), anEntry);
    aListRes += anEntry + ", ";
  }
  aListRes.Trunc(aListRes.Length() - 2);

  GEOM::TPythonDump pd (aFunction);

  pd << aListRes.ToCString() << "] = geompy.MakePipeTShapeFillet("
     << theR1 << ", " << theW1 << ", " << theL1 << ", "
     << theR2 << ", " << theW2 << ", " << theL2 << ", "
     << theRF << ", " << theHexMesh;

  // thickness reduction
  if (isTRL)
    pd << ", theRL=" << theRL << ", theWL=" << theWL
       << ", theLtransL=" << theLtransL << ", theLthinL=" << theLthinL;
  if (isTRR)
    pd << ", theRR=" << theRR << ", theWR=" << theWR
       << ", theLtransR=" << theLtransR << ", theLthinR=" << theLthinR;
  if (isTRI)
    pd << ", theRI=" << theRI << ", theWI=" << theWI
       << ", theLtransI=" << theLtransI << ", theLthinI=" << theLthinI;

  pd << ")";

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  MakePipeTShapeFilletWithPosition
 *  \brief Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A fillet is created
 *  on the junction of the pipes.
 *  The extremities of the main pipe are located on junctions points P1 and P2.
 *  The extremity of the incident pipe is located on junction point P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theRF Radius of curvature of fillet
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \param theP1 1st junction point of main pipe
 *  \param theP2 2nd junction point of main pipe
 *  \param theP3 Junction point of incident pipe
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
AdvancedEngine_IOperations::MakePipeTShapeFilletWithPosition
                             (double theR1, double theW1, double theL1,
                              double theR2, double theW2, double theL2,
                              double theRL, double theWL, double theLtransL, double theLthinL,
                              double theRR, double theWR, double theLtransR, double theLthinR,
                              double theRI, double theWI, double theLtransI, double theLthinI,
                              double theRF, bool theHexMesh,
                              Handle(GEOM_Object) theP1,
                              Handle(GEOM_Object) theP2,
                              Handle(GEOM_Object) theP3)
{
  SetErrorCode(KO);
  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);
  //Add a new shape function with parameters
  Handle(GEOM_Function) aFunction = aShape->AddFunction(AdvancedEngine_PipeTShapeDriver::GetID(), TSHAPE_FILLET);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != AdvancedEngine_PipeTShapeDriver::GetID()) return NULL;

  // Check new position
  if (!CheckCompatiblePosition(theL1, theL2, theP1, theP2, theP3, 0.01)) {
    return NULL;
  }

  AdvancedEngine_IPipeTShape aData(aFunction);

  aData.SetR1(theR1);
  aData.SetW1(theW1);
  aData.SetL1(theL1);
  aData.SetR2(theR2);
  aData.SetW2(theW2);
  aData.SetL2(theL2);
  aData.SetRF(theRF);
  aData.SetHexMesh(theHexMesh);

  bool isTRL = (theRL + theWL + theLtransL + theLthinL) > Precision::Confusion();
  bool isTRR = (theRR + theWR + theLtransR + theLthinR) > Precision::Confusion();
  bool isTRI = (theRI + theWI + theLtransI + theLthinI) > Precision::Confusion();

  //Compute the resulting value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("TShape driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  // BEGIN of fillet
  TopoDS_Shape aShapeShape = aShape->GetValue();
  TopTools_IndexedMapOfShape anEdgesIndices;
  TopExp::MapShapes(aShapeShape, anEdgesIndices);
  // Common edges on external cylinders
  Handle(GEOM_Object) box_e;
  if (theHexMesh) {
    box_e = my3DPrimOperations->MakeBoxDXDYDZ(theR2+theW2, theR2+theW2, theR1+theW1);
  }
  else {
    box_e = my3DPrimOperations->MakeBoxDXDYDZ(2*(theR2+theW2), 2*(theR2+theW2), theR1+theW1);
  }
  box_e->GetLastFunction()->SetDescription("");
  box_e = myTransformOperations->TranslateDXDYDZ(box_e, -(theR2+theW2), -(theR2+theW2), 0);
  box_e->GetLastFunction()->SetDescription("");

  Handle(TColStd_HSequenceOfInteger) edges_e =
    myShapesOperations->GetShapesOnBoxIDs(box_e, aShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
  box_e->GetLastFunction()->SetDescription("");

  if (edges_e.IsNull() || edges_e->Length() == 0) {
    SetErrorCode("External edges not found");
    return NULL;
  }
  int nbEdgesInFillet = 0;
  std::list<int> theEdges;
  for (int i=1; i<=edges_e->Length();i++) {
    int edgeID = edges_e->Value(i);
    TopoDS_Shape theEdge = anEdgesIndices.FindKey(edgeID);
    TopExp_Explorer Ex(theEdge,TopAbs_VERTEX);
    while (Ex.More()) {
      gp_Pnt aPt = BRep_Tool::Pnt(TopoDS::Vertex(Ex.Current()));
      if (Abs(aPt.Z() - (theR1+theW1)) <= Precision::Confusion()) {
        nbEdgesInFillet ++;
        theEdges.push_back(edgeID);
      }
      Ex.Next();
    }
    if (theHexMesh && nbEdgesInFillet == 1)
      break;
  }

  Handle(GEOM_Object) aFillet;
  try {
    aFillet = myLocalOperations->MakeFilletEdges(aShape, theRF, theEdges);
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }
  if (aFillet.IsNull()) {
    SetErrorCode("Fillet can not be computed on the given shape with the given parameters");
    return NULL;
  }
  aFillet->GetLastFunction()->SetDescription("");

  TopoDS_Shape aFilletShape = aFillet->GetValue();

#ifdef FILLET_FIX_TOLERANCE
  // VSR: 30/12/2014: temporary workaround about Fillet problem
  if (theHexMesh) {
    GEOMUtils::FixShapeTolerance(aFilletShape, TopAbs_FACE);
  }
  else {
    GEOMUtils::FixShapeCurves(aFilletShape);
  }
#endif

  aFunction->SetValue(aFilletShape);
  // END of fillet

// VSR: debug issues 0021568 and 0021550 (15/05/2012) - BEGIN (3)
// the following block, when enabled, leads to partitioning problems
#if 0
// VSR: debug issues 0021568 and 0021550 (15/05/2012) - END (3)
  // BEGIN: Limit tolerances (debug)
  Handle(GEOM_Object) aCorr1 = myHealingOperations->LimitTolerance(aShape, 1e-07);
  TopoDS_Shape aCorr1Shape = aCorr1->GetValue();
  aShape->GetLastFunction()->SetValue(aCorr1Shape);
  aCorr1->GetLastFunction()->SetDescription("");
  // END: Limit tolerances (debug)
// VSR: debug issues 0021568 and 0021550 (15/05/2012) - BEGIN (4)
#endif
// VSR: debug issues 0021568 and 0021550 (15/05/2012) - END (4)

  if (theHexMesh) {
    if (!MakePipeTShapePartition(aShape, theR1, theW1, theL1, theR2, theW2, theL2, 0, 0, theRF, false))
      return NULL;
    if (!MakePipeTShapeMirrorAndGlue(aShape, theR1, theW1, theL1, theR2, theW2, theL2))
      return NULL;
  }

  // Add thickness reduction elements
  // at the three extremities: Left, Right and Incident
  try {
    OCC_CATCH_SIGNALS;
    if (isTRL || isTRR || isTRI) {
      TopoDS_Shape aResShape =
        MakePipeTShapeThicknessReduction(aShape->GetValue(), theR1, theW1, theL1, theR2, theW2, theL2,
                                         theRL, theWL, theLtransL, theLthinL,
                                         theRR, theWR, theLtransR, theLthinR,
                                         theRI, theWI, theLtransI, theLthinI,
                                         !theHexMesh);
      aFunction->SetValue(aResShape);
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  // Set Position
  gp_Trsf aTrsf = GetPositionTrsf(theL1, theL2, theP1, theP2, theP3);
  BRepBuilderAPI_Transform aTransformation (aShape->GetValue(), aTrsf, Standard_False);
  TopoDS_Shape aTrsf_Shape = aTransformation.Shape();
  aFunction->SetValue(aTrsf_Shape);

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
  aSeq->Append(aShape);

  try {
    if (theHexMesh) {
      // Get the groups
      if (!MakeGroups(aShape, TSHAPE_FILLET, theR1, theW1, theL1, theR2, theW2, theL2,
                      0., 0., theRF, aSeq, aTrsf))
        return NULL;
    }

    // Get internal group.
    if (!MakeInternalGroup(aShape, theR1, theL1, theR2, theL2, theRL, theLtransL,
                           theRR, theLtransR, theRI, theLtransI,
                           aSeq, aTrsf)) {
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aListRes("[");
  // Iterate over the sequence aSeq
  Standard_Integer aNbGroups = aSeq->Length();
  Standard_Integer i = 1;
  for (; i <= aNbGroups; i++) {
    Handle(Standard_Transient) anItem = aSeq->Value(i);
    if (anItem.IsNull()) continue;
    Handle(GEOM_Object) aGroup = Handle(GEOM_Object)::DownCast(anItem);
    if (aGroup.IsNull()) continue;
    //Make a Python command
    TDF_Tool::Entry(aGroup->GetEntry(), anEntry);
    aListRes += anEntry + ", ";
  }
  aListRes.Trunc(aListRes.Length() - 2);

  GEOM::TPythonDump pd (aFunction);

  pd << aListRes.ToCString() << "] = geompy.MakePipeTShapeFillet("
     << theR1 << ", " << theW1 << ", " << theL1 << ", "
     << theR2  << ", " << theW2 << ", " << theL2 << ", "
     << theRF << ", " << theHexMesh << ", "
     << theP1 << ", " << theP2 << ", " << theP3;

  // thickness reduction
  if (isTRL)
    pd << ", theRL=" << theRL << ", theWL=" << theWL
       << ", theLtransL=" << theLtransL << ", theLthinL=" << theLthinL;
  if (isTRR)
    pd << ", theRR=" << theRR << ", theWR=" << theWR
       << ", theLtransR=" << theLtransR << ", theLthinR=" << theLthinR;
  if (isTRI)
    pd << ", theRI=" << theRI << ", theWI=" << theWI
       << ", theLtransI=" << theLtransI << ", theLthinI=" << theLthinI;

  pd << ")";

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  This function allows to create a disk already divided into blocks. It can be
 *  used to create divided pipes for later meshing in hexaedra.
 *  \param theR Radius of the disk
 *  \param theRatio Relative size of the central square diagonal against the disk diameter
 *  \param theOrientation Plane on which the disk will be built
 *  \param thePattern The division pattern of the disk (hexagon or square in the center)
 *  \return New GEOM_Object, containing the created shape.
 */
//=============================================================================
Handle(GEOM_Object) AdvancedEngine_IOperations::MakeDividedDisk (double theR, double theRatio, 
                                                                 int theOrientation, int thePattern)
{
  SetErrorCode(KO);
  
  if (theOrientation != 1 &&
      theOrientation != 2 &&
      theOrientation != 3)
  {
    SetErrorCode("theOrientation must be 1(=OXY), 2(=OYZ) or 3(=OZX)");
    return NULL;
  }
  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_DIVIDEDDISK);

  //Add a new shape function with parameters
  Handle(GEOM_Function) aFunction = aShape->AddFunction(AdvancedEngine_DividedDiskDriver::GetID(), DIVIDEDDISK_R_RATIO);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != AdvancedEngine_DividedDiskDriver::GetID()) return NULL;

  AdvancedEngine_IDividedDisk aData (aFunction);

  aData.SetR(theR);
  aData.SetRatio(theRatio);
  aData.SetOrientation(theOrientation);
  aData.SetType(thePattern);

  //Compute the resulting value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("DividedDisk driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }
  
  std::string aPatternStr;
  
  switch(thePattern)
  {
    case 0:
      aPatternStr = "GEOM.SQUARE";
      break;
    case 1:
      aPatternStr = "GEOM.HEXAGON";
      break;
  }
  
  //Make a Python command
  GEOM::TPythonDump(aFunction) << aShape << " = geompy.MakeDividedDisk(" << theR << ", " << theOrientation << ", " << aPatternStr.c_str() << ")";

  SetErrorCode(OK);

  return aShape;
}

//=============================================================================
/*!
 *  This function allows to create a disk already divided into blocks. It can be
 *  used to create divided pipes for later meshing in hexaedra.
 *  \param theR Radius of the disk
 *  \param theRatio Relative size of the central square diagonal against the disk diameter
 *  \return New GEOM_Object, containing the created shape.
 */
//=============================================================================
Handle(GEOM_Object) AdvancedEngine_IOperations::MakeDividedDiskPntVecR (Handle(GEOM_Object) thePnt, 
                                                                        Handle(GEOM_Object) theVec, 
                                                                        double theR, 
                                                                        double theRatio,
                                                                        int    thePattern)
{
  SetErrorCode(KO);

  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_DIVIDEDDISK);

  //Add a new shape function with parameters
  Handle(GEOM_Function) aFunction = aShape->AddFunction(AdvancedEngine_DividedDiskDriver::GetID(), DIVIDEDDISK_R_VECTOR_PNT);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != AdvancedEngine_DividedDiskDriver::GetID()) return NULL;

  AdvancedEngine_IDividedDisk aData (aFunction);
  
  Handle(GEOM_Function) aRefPnt = thePnt->GetLastFunction();
  Handle(GEOM_Function) aRefVec = theVec->GetLastFunction();

  if (aRefPnt.IsNull() || aRefVec.IsNull()) return NULL;

  aData.SetCenter(aRefPnt);
  aData.SetVector(aRefVec);

  aData.SetR(theR);
  aData.SetRatio(theRatio);
  aData.SetType(thePattern);

  //Compute the resulting value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("DividedDisk driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }
  
  std::string aPatternStr;
  
  switch(thePattern)
  {
    case 0:
      aPatternStr = "GEOM.SQUARE";
      break;
    case 1:
      aPatternStr = "GEOM.HEXAGON";
      break;
  }
  

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aShape << " = geompy.MakeDividedDiskPntVecR(" << thePnt << ", " << theVec << ", " << theR << ", " << aPatternStr.c_str() << ")";

  SetErrorCode(OK);

  return aShape;
}

//=============================================================================
/*!
 *  Builds a cylinder prepared for hexa meshes
 *  \param theR Radius of the cylinder
 *  \param theH Height of the cylinder
 *  \return New GEOM_Object, containing the created shape.
 */
//=============================================================================
Handle(GEOM_Object) AdvancedEngine_IOperations::MakeDividedCylinder (double theR, 
                                                                     double theH,
                                                                     int    thePattern)
{
  SetErrorCode(KO);
  
  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_DIVIDEDCYLINDER);

  Handle(GEOM_Object) aBaseShape = MakeDividedDisk(theR, 67.0, 1, thePattern);
  aBaseShape->GetLastFunction()->SetDescription("");   // Erase dump of MakeDividedDisk
  
  aShape = my3DPrimOperations->MakePrismDXDYDZ(aBaseShape,0.0,0.0,theH, -1.0);
        
  Handle(GEOM_Function) aFunction =  aShape->GetLastFunction();
  aFunction->SetDescription("");   // Erase dump of MakePrismDXDYDZ
  aShape->SetType(GEOM_DIVIDEDCYLINDER);
  
  std::string aPatternStr;
  
  switch(thePattern)
  {
    case 0:
      aPatternStr = "GEOM.SQUARE";
      break;
    case 1:
      aPatternStr = "GEOM.HEXAGON";
      break;
  }
  
  //Make a Python command
  GEOM::TPythonDump(aFunction) << aShape << " = geompy.MakeDividedCylinder(" << theR << ", " << theH << ", " << aPatternStr.c_str() << ")";

  SetErrorCode(OK);

  return aShape;
}
//=============================================================================
/*!
 *  Create a smoothing surface from a set of points
 *  \param thelPoints list of points or compounds of points
 *  \param theNbMax maximum number of Bezier pieces in the resulting surface.
 *  \param theDegMax maximum degree of the resulting BSpline surface
 *  \param theDMax specifies maximum value of the GeomPlate_PlateG0Criterion criterion.
 *  \return New GEOM_Object, containing the created shape.
 */
//=============================================================================
Handle(GEOM_Object) AdvancedEngine_IOperations::MakeSmoothingSurface (std::list<Handle(GEOM_Object)> thelPoints, 
                                                                      int                            theNbMax,
                                                                      int                            theDegMax,
                                                                      double                         theDMax)
{
  SetErrorCode(KO);

  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_SMOOTHINGSURFACE);

  //Add a new shape function with parameters
  Handle(GEOM_Function) aFunction = aShape->AddFunction(AdvancedEngine_SmoothingSurfaceDriver::GetID(), SMOOTHINGSURFACE_LPOINTS);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != AdvancedEngine_SmoothingSurfaceDriver::GetID()) return NULL;

  AdvancedEngine_ISmoothingSurface aData (aFunction);

  int aLen = thelPoints.size();
  aData.SetLength(aLen);
  int ind = 1;
  std::list<Handle(GEOM_Object)>::iterator it = thelPoints.begin();
  for (; it != thelPoints.end(); it++, ind++) {
    Handle(GEOM_Function) aRefObj = (*it)->GetLastFunction();
    if (aRefObj.IsNull()) {
      SetErrorCode("NULL point or compound for bSplineFaceShape");
      return NULL;
    }
    aData.SetPntOrComp(ind, aRefObj);
  }

  aData.SetNbMax(theNbMax);
  aData.SetDegMax(theDegMax);
  aData.SetDMax(theDMax);

  //Compute the resulting value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("SmoothingSurface driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aShape << " = geompy.MakeSmoothingSurface([";
  it = thelPoints.begin();
  pd << (*it++);
  while (it != thelPoints.end()) {
    pd << ", " << (*it++);
  }
  pd << "], "
     << theNbMax << ", "
     << theDegMax << ", "
     << theDMax <<")";

  SetErrorCode(OK);

  return aShape;
}
