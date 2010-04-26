//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  File   : GEOMImpl_IAdvancedOperations.cxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include <Standard_Stream.hxx>
#include "GEOMImpl_IBasicOperations.hxx"
#include "GEOMImpl_IShapesOperations.hxx"
#include "GEOMImpl_IBlocksOperations.hxx"
#include "GEOMImpl_IAdvancedOperations.hxx"
#include "GEOMImpl_ILocalOperations.hxx"
#include "GEOMImpl_Types.hxx"
#include <GEOMImpl_Gen.hxx>

#include <utilities.h>
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

#include "GEOM_Function.hxx"
#include "GEOM_PythonDump.hxx"

#include <GEOMImpl_PipeTShapeDriver.hxx>
#include <GEOMImpl_IPipeTShape.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax3.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRep_Tool.hxx>
#include <cmath>
/*@@ insert new functions before this line @@*/

#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TDF_Tool.hxx>
#include <Standard_Failure.hxx>
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
//=============================================================================
/*!
 *  Constructor
 */
//=============================================================================
GEOMImpl_IAdvancedOperations::GEOMImpl_IAdvancedOperations(GEOM_Engine* theEngine, int theDocID) :
    GEOM_IOperations(theEngine, theDocID) {
    MESSAGE("GEOMImpl_IAdvancedOperations::GEOMImpl_IAdvancedOperations");
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================
GEOMImpl_IAdvancedOperations::~GEOMImpl_IAdvancedOperations() {
    MESSAGE("GEOMImpl_IAdvancedOperations::~GEOMImpl_IAdvancedOperations");
}

//=============================================================================
/*!
 *  SetPosition
 */
//=============================================================================
gp_Trsf GEOMImpl_IAdvancedOperations::GetPositionTrsf(double theL1, double theL2, Handle(GEOM_Object) theP1,
		Handle(GEOM_Object) theP2, Handle(GEOM_Object) theP3) {
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
bool GEOMImpl_IAdvancedOperations::CheckCompatiblePosition(double& theL1, double& theL2, Handle(GEOM_Object) theP1,
        Handle(GEOM_Object) theP2, Handle(GEOM_Object) theP3, double theTolerance) {
    SetErrorCode(KO);
    gp_Pnt P1 = BRep_Tool::Pnt(TopoDS::Vertex(theP1->GetValue()));
    gp_Pnt P2 = BRep_Tool::Pnt(TopoDS::Vertex(theP2->GetValue()));
    gp_Pnt P3 = BRep_Tool::Pnt(TopoDS::Vertex(theP3->GetValue()));

    double d12 = P1.Distance(P2);
	double d13 = P1.Distance(P3);
	double d23 = P2.Distance(P3);
	//    double d2 = newO.Distance(P3);
//	std::cerr << "theL1: " << theL1 << std::endl;
//	std::cerr << "theL2: " << theL2 << std::endl;
//	std::cerr << "d12: " << d12 << std::endl;
//	std::cerr << "d13: " << d13 << std::endl;
//	std::cerr << "d23: " << d23 << std::endl;

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
//    std::cerr << "newL1: " << newL1 << std::endl;
//    std::cerr << "newL2: " << newL2 << std::endl;
    //
    // theL1*(1-theTolerance) <= newL1 <= theL1*(1+theTolerance)
    //
//    std::cerr << "1 - theTolerance: " << 1 - theTolerance << std::endl;

//    std::cerr << "fabs(newL1 - theL1): " << fabs(newL1 - theL1) << std::endl;
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
//    std::cerr << "fabs(newL2 - theL2): " << fabs(newL2 - theL2) << std::endl;
    if (fabs(newL2 - theL2) > Precision::Approximation()) {
        if ( (newL2 * (1 - theTolerance) -theL2 <= Precision::Approximation()) &&
        		(newL2 * (1 + theTolerance) -theL2 >= Precision::Approximation()) ) {
//            std::cerr << "theL2 = newL2" << std::endl;
            theL2 = newL2;
        } else {
            theL2 = -1;
            SetErrorCode("Dimension for incident pipe (L2) is incompatible with new position");
            return false;
        }
    }
//    std::cerr << "theL1: " << theL1 << std::endl;
//    std::cerr << "theL2: " << theL2 << std::endl;

    SetErrorCode(OK);
    return true;

}

//=============================================================================
/*!
 *  Generate the propagation groups of a Pipe T-Shape used for hexa mesh
 */
//=============================================================================
bool GEOMImpl_IAdvancedOperations::MakeGroups(/*std::vector<GEOM_IOperations*> theOperations, */Handle(GEOM_Object) theShape,
        int shapeType, double theR1, double theW1, double theL1, double theR2, double theW2, double theL2, 
        Handle(TColStd_HSequenceOfTransient) theSeq, gp_Trsf aTrsf) {
    SetErrorCode(KO);

    if (theShape.IsNull()) return false;

    TopoDS_Shape aShape = theShape->GetValue();
    if (aShape.IsNull()) {
        SetErrorCode("Shape is not defined");
        return false;
    }

    gp_Trsf aTrsfInv = aTrsf.Inverted();
    
    int expectedGroups = 0;
    if (shapeType == TSHAPE_BASIC)
      if (Abs(theR2+theW2-theR1-theW1) <= Precision::Approximation())
        expectedGroups = 10;
      else
        expectedGroups = 11;
    else if (shapeType == TSHAPE_CHAMFER || shapeType == TSHAPE_FILLET)
      expectedGroups = 12;
    
    double aR1Ext = theR1 + theW1;
    double aR2Ext = theR2 + theW2;
    
    /////////////////////////
    //// Groups of Faces ////
    /////////////////////////
/*
    GEOMImpl_I3DPrimOperations* a3DPrimOperations = (GEOMImpl_I3DPrimOperations*) &theOperations[0];
    GEOMImpl_IBlocksOperations* aBlocksOperations = (GEOMImpl_IBlocksOperations*) &theOperations[2];
    GEOMImpl_IBooleanOperations* aBooleanOperations = (GEOMImpl_IBooleanOperations*) &theOperations[3];
    GEOMImpl_IShapesOperations* aShapesOperations = (GEOMImpl_IShapesOperations*) &theOperations[4];
    GEOMImpl_ITransformOperations* aTransformOperations = (GEOMImpl_ITransformOperations*) &theOperations[5];*/

    GEOMImpl_IBooleanOperations* aBooleanOperations = new GEOMImpl_IBooleanOperations(GetEngine(), GetDocID());
    GEOMImpl_IShapesOperations* aShapesOperations = new GEOMImpl_IShapesOperations(GetEngine(), GetDocID());
    GEOMImpl_ITransformOperations* aTransformOperations = new GEOMImpl_ITransformOperations(GetEngine(), GetDocID());
    GEOMImpl_IBlocksOperations* aBlocksOperations = new GEOMImpl_IBlocksOperations(GetEngine(), GetDocID());
    GEOMImpl_I3DPrimOperations* a3DPrimOperations = new GEOMImpl_I3DPrimOperations(GetEngine(), GetDocID());
    
    //
    // Comment the following lines when GetInPlace bug is solved
    // == BEGIN
    // Workaround of GetInPlace bug
    // Create a bounding box that fits the shape
    Handle(GEOM_Object) aBox = a3DPrimOperations->MakeBoxDXDYDZ(2*theL1, 2*aR1Ext, aR1Ext+theL2);
    aBox->GetLastFunction()->SetDescription("");
    aTransformOperations->TranslateDXDYDZ(aBox, -theL1, -aR1Ext, -aR1Ext);
    aBox->GetLastFunction()->SetDescription("");
    // Apply transformation to box
    BRepBuilderAPI_Transform aTransformationBox(aBox->GetValue(), aTrsf, Standard_False);
    TopoDS_Shape aBoxShapeTrsf = aTransformationBox.Shape();
    aBox->GetLastFunction()->SetValue(aBoxShapeTrsf);
    
    // Get the shell of the box
    Handle(GEOM_Object) aShell = Handle(GEOM_Object)::DownCast(aShapesOperations->MakeExplode(aBox, TopAbs_SHELL, true)->Value(1));
    aBox->GetLastFunction()->SetDescription("");
    aShell->GetLastFunction()->SetDescription("");
    // Get the common shapes between shell and shape
    Handle(GEOM_Object) aCommonCompound = aBooleanOperations->MakeBoolean (theShape, aShell, 1); // MakeCommon
    aCommonCompound->GetLastFunction()->SetDescription("");
    // Explode the faces of common shapes => 3 faces
    Handle(TColStd_HSequenceOfTransient) aCommonFaces = aShapesOperations->MakeExplode(aCommonCompound, TopAbs_FACE, true);
    aCommonCompound->GetLastFunction()->SetDescription("");
    std::list<Handle(GEOM_Object)> aCompoundOfFacesList;
    
//     std::cerr << "aCommonFaces->Length(): " << aCommonFaces->Length() << std::endl;
    for (int i=0 ; i<= aCommonFaces->Length()-4 ; i+=4) {
        std::list<Handle(GEOM_Object)> aFacesList;
//         std::cerr << "Create compound for junction face " << i+1 << std::endl;
        for (int j = 1 ; j <= 4 ; j++) {
            Handle(GEOM_Object) aFace = Handle(GEOM_Object)::DownCast(aCommonFaces->Value(i+j)); // Junction faces
            if (!aFace.IsNull()) {
                aFace->GetLastFunction()->SetDescription("");
                aFacesList.push_back(aFace);
            }
        }
        Handle(GEOM_Object) aCompoundOfFaces = aShapesOperations->MakeCompound(aFacesList);
        if (!aCompoundOfFaces.IsNull()) {
            aCompoundOfFaces->GetLastFunction()->SetDescription("");
            // Apply transformation to compound of faces
//             BRepBuilderAPI_Transform aTransformationCompoundOfFaces(aCompoundOfFaces->GetValue(), aTrsf, Standard_False);
//             TopoDS_Shape aTrsf_CompoundOfFacesShape = aTransformationCompoundOfFaces.Shape();
//             aCompoundOfFaces->GetLastFunction()->SetValue(aTrsf_CompoundOfFacesShape);
            aCompoundOfFacesList.push_back(aCompoundOfFaces);
        }
    }

//     std::cerr << "aCompoundOfFacesList.size(): " << aCompoundOfFacesList.size() << std::endl;
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
//         Handle(GEOM_Object) aP1 = aBasicOperations->MakePointXYZ(-theL1, 0, 0);
//         Handle(GEOM_Object) aP2 = aBasicOperations->MakePointXYZ(-0, 0, theL2);
//         Handle(GEOM_Object) aP3 = aBasicOperations->MakePointXYZ(theL1, 0, 0);
//         aP1->GetLastFunction()->SetDescription("");
//         aP2->GetLastFunction()->SetDescription("");
//         aP3->GetLastFunction()->SetDescription("");
//         Handle(GEOM_Object) aV1 = aBasicOperations->MakeVectorDXDYDZ(-1, 0, 0);
//         Handle(GEOM_Object) aV2 = aBasicOperations->MakeVectorDXDYDZ(0, 0, 1);
//         Handle(GEOM_Object) aV3 = aBasicOperations->MakeVectorDXDYDZ(1, 0, 0);
//         aV1->GetLastFunction()->SetDescription("");
//         aV2->GetLastFunction()->SetDescription("");
//         aV3->GetLastFunction()->SetDescription("");
//         Handle(GEOM_Object) aPln1 = aBasicOperations->MakePlanePntVec(aP1, aV1, 2*(theR1+theW1+theL2));
//         Handle(GEOM_Object) aPln2 = aBasicOperations->MakePlanePntVec(aP2, aV2, 2*(theR2+theW2));
//         Handle(GEOM_Object) aPln3 = aBasicOperations->MakePlanePntVec(aP3, aV3, 2*(theR1+theW1+theL2));
//         aPln1->GetLastFunction()->SetDescription("");
//         aPln2->GetLastFunction()->SetDescription("");
//         aPln3->GetLastFunction()->SetDescription("");

//         BRepBuilderAPI_Transform aTransformation1(aPln1->GetValue(), aTrsf, Standard_False);
//         TopoDS_Shape aTrsf_Shape1 = aTransformation1.Shape();
//         aPln1->GetLastFunction()->SetValue(aTrsf_Shape1);
//         BRepBuilderAPI_Transform aTransformation2(aPln2->GetValue(), aTrsf, Standard_False);
//         TopoDS_Shape aTrsf_Shape2 = aTransformation2.Shape();
//         aPln2->GetLastFunction()->SetValue(aTrsf_Shape2);
//         BRepBuilderAPI_Transform aTransformation3(aPln3->GetValue(), aTrsf, Standard_False);
//         TopoDS_Shape aTrsf_Shape3 = aTransformation3.Shape();
//         aPln3->GetLastFunction()->SetValue(aTrsf_Shape3);
//     == END
//
    
        Handle(GEOM_Object) junctionFaces1 = aShapesOperations->GetInPlace(theShape, aPln1);
        if (junctionFaces1.IsNull())
            junctionFaces1 = aShapesOperations->GetShapesOnShapeAsCompound(aPln1, theShape, TopAbs_FACE,  GEOMAlgo_ST_ONIN);
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
        Handle(GEOM_Object) junctionFaces2 = aShapesOperations->GetInPlace(theShape, aPln2);
        if (junctionFaces2.IsNull())
            junctionFaces2 = aShapesOperations->GetShapesOnShapeAsCompound(aPln2, theShape, TopAbs_FACE,  GEOMAlgo_ST_ONIN);
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
        Handle(GEOM_Object) junctionFaces3 = aShapesOperations->GetInPlace(theShape, aPln3);
        if (junctionFaces3.IsNull())
            junctionFaces3 = aShapesOperations->GetShapesOnShapeAsCompound(aPln3, theShape, TopAbs_FACE,  GEOMAlgo_ST_ONIN);
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
    }
    /////////////////////////
    //// Groups of Edges ////
    /////////////////////////
    // Result of propagate

    Handle(GEOM_Function) aFunction = theShape->GetLastFunction();
    

    // Apply inverted transformation to shape
//    BRepBuilderAPI_Transform aTransformationShapeInv(aShape, aTrsfInv, Standard_False);
//    TopoDS_Shape aShapeTrsfInv = aTransformationShapeInv.Shape();
//    aFunction->SetValue(aShapeTrsfInv);
    
    TCollection_AsciiString theDesc = aFunction->GetDescription();
    Handle(TColStd_HSequenceOfTransient) aSeqPropagate = aBlocksOperations->Propagate(theShape);
    if (aSeqPropagate.IsNull() || aSeqPropagate->Length() == 0) {
        SetErrorCode("Propagation groups not found");
        return false;
    }
    Standard_Integer nbEdges, aNbGroups = aSeqPropagate->Length();
    // Recover previous description to get rid of Propagate dump
    aFunction->SetDescription(theDesc);
    
    
    // Apply transformation to shape
//    BRepBuilderAPI_Transform aTransformationShape(theShape->GetValue(), aTrsf, Standard_False);
//    TopoDS_Shape aShapeTrsf = aTransformationShape.Shape();
//    aFunction->SetValue(aShapeTrsf);
    
    bool addGroup;
    bool circularFoundAndAdded = false;
    bool incidentPipeFound = false;
    bool mainPipeFound = false;
    bool mainPipeFoundAndAdded = false;
    bool radialFound =false;
    bool flangeFound = false;
    bool flangeFoundAndAdded = false;
    bool chamferOrFilletFound = false;
    
    for (int i=1 ; i<= aNbGroups; i++) {
      addGroup = false;
    
      Handle(GEOM_Object) aGroup = Handle(GEOM_Object)::DownCast(aSeqPropagate->Value(i));
      if(aGroup.IsNull())
        continue;
      
      TopoDS_Shape aGroupShape = aGroup->GetValue();
      BRepBuilderAPI_Transform aTransformationShapeInv(aGroupShape, aTrsfInv, Standard_False);
      TopoDS_Shape aGroupShapeTrsfInv = aTransformationShapeInv.Shape();
      
      TopTools_IndexedMapOfShape anEdgesMap;
      TopExp::MapShapes(aGroupShapeTrsfInv,TopAbs_EDGE, anEdgesMap);
      nbEdges = anEdgesMap.Extent();

      if (shapeType == TSHAPE_BASIC) {
        if ((nbEdges == 21) || /*R1Ext = R2Ext*/(nbEdges == 17)){
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
          radialFound =false;
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
        if (nbEdges == 25) {
          addGroup = true;
          aGroup->SetName("THICKNESS");
        }
        else if ((nbEdges == 10) || (nbEdges == 6)) {
          if (!circularFoundAndAdded) {
            addGroup = true;
            circularFoundAndAdded = true;
            aGroup->SetName("CIRCULAR_QUARTER_PIPE");
          }
        }
        else if (nbEdges == 8) {
          incidentPipeFound = true;
          mainPipeFound = false;
          flangeFound = false;
          
          TopExp_Explorer Ex(aGroupShapeTrsfInv,TopAbs_VERTEX);
          while (Ex.More()) {
            gp_Pnt aP =  BRep_Tool::Pnt(TopoDS::Vertex(Ex.Current()));
            double x=aP.X(), y=aP.Y(), z=aP.Z();
            
            // tuy_princ_long_avant & tuy_princ_long_apres
            bool isMain = (((z < Precision::Confusion()) || (x < Precision::Confusion())) &&
            ((y <= aR1Ext + Precision::Confusion()) ||
            (y <= -(aR1Ext + Precision::Confusion())) ||
            (y <= theR1 + Precision::Confusion()) ||
            (y == -(theR1 + Precision::Confusion()))));
            
            
            if (!isMain) {
              mainPipeFound = false;
            }
            
            // collerette
            if (z < Precision::Confusion()) {
              flangeFound = true;
              if (!flangeFoundAndAdded) {
                flangeFoundAndAdded = true;
                addGroup = true;
                aGroup->SetName("FLANGE");
              }
            }
            
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

    SetErrorCode(OK);
    return true;
}



bool GEOMImpl_IAdvancedOperations::MakePipeTShapePartition(/*std::vector<GEOM_IOperations*> theOperations, */Handle(GEOM_Object) theShape, 
      double theR1, double theW1, double theL1, double theR2, double theW2, double theL2, double theH, double theW, double theRF, bool isNormal) {
    SetErrorCode(KO);
/*
    GEOMImpl_I3DPrimOperations* a3DPrimOperations = (GEOMImpl_I3DPrimOperations*) &theOperations[0];
    GEOMImpl_IBasicOperations* aBasicOperations = (GEOMImpl_IBasicOperations*) &theOperations[1];
    GEOMImpl_IBlocksOperations* aBlocksOperations = (GEOMImpl_IBlocksOperations*) &theOperations[2];
    GEOMImpl_IBooleanOperations* aBooleanOperations = (GEOMImpl_IBooleanOperations*) &theOperations[3];
    GEOMImpl_IShapesOperations* aShapesOperations = (GEOMImpl_IShapesOperations*) &theOperations[4];
    GEOMImpl_ITransformOperations* aTransformOperations = (GEOMImpl_ITransformOperations*) &theOperations[5];*/

    GEOMImpl_IBasicOperations* aBasicOperations = new GEOMImpl_IBasicOperations(GetEngine(), GetDocID());
    GEOMImpl_IBooleanOperations* aBooleanOperations = new GEOMImpl_IBooleanOperations(GetEngine(), GetDocID());
    GEOMImpl_IShapesOperations* aShapesOperations = new GEOMImpl_IShapesOperations(GetEngine(), GetDocID());
    GEOMImpl_ITransformOperations* aTransformOperations = new GEOMImpl_ITransformOperations(GetEngine(), GetDocID());
    GEOMImpl_IBlocksOperations* aBlocksOperations = new GEOMImpl_IBlocksOperations(GetEngine(), GetDocID());
    GEOMImpl_I3DPrimOperations* a3DPrimOperations = new GEOMImpl_I3DPrimOperations(GetEngine(), GetDocID());

    // Build tools for partition operation:
    // 1 face and 2 planes
    // Face
    Handle(GEOM_Object) arete_intersect_int;
    Handle(GEOM_Object) wire_t, wire_t2, face_t, face_t2;
    Handle(GEOM_Object) chan_racc;
    Handle(GEOM_Object) vi1, vi2;

    Handle(GEOM_Object) Vector_Z = aBasicOperations->MakeVectorDXDYDZ(0, 0, 1);
    Vector_Z->GetLastFunction()->SetDescription("");

    // Useful values
    double aSize = 2*(theL1 + theL2);
    double aR1Ext = theR1 + theW1;
    double aR2Ext = theR2 + theW2;
    double theVertCylinderRadius = aR2Ext + theW + theRF;
    double theHoriCylinderRadius = aR1Ext + theH + theRF;

    // Common edges on internal cylinder
//    std::cerr << "Search for internal edges" << std::endl;
    Handle(GEOM_Object) box_i = a3DPrimOperations->MakeBoxDXDYDZ(theR2, theR2, theR1);
    box_i->GetLastFunction()->SetDescription("");
    box_i = aTransformOperations->TranslateDXDYDZ(box_i, -theR2, -theR2, 0);
    box_i->GetLastFunction()->SetDescription("");
    
    Handle(GEOM_Function) aFunction = theShape->GetLastFunction();
    TCollection_AsciiString theDesc = aFunction->GetDescription();
    Handle(TColStd_HSequenceOfTransient) edges_i = aShapesOperations->GetShapesOnBox(box_i, theShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
    // Recover previous description to get rid of Propagate dump
    aFunction->SetDescription(theDesc);
//     Handle(TColStd_HSequenceOfTransient) edges_i = GetCommonShapesOnCylinders(theShape, TopAbs_EDGE, theR1, theR2);
    if (edges_i.IsNull() || edges_i->Length() == 0) {
//        std::cerr << "Internal edges not found" << std::endl;
        SetErrorCode("Internal edges not found");
        return false;
    }
//    std::cerr << "Internal edges found" << std::endl;
    for (int i=1; i<=edges_i->Length();i++) {
        Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast(edges_i->Value(i));
        anObj->GetLastFunction()->SetDescription("");
    }
    arete_intersect_int = Handle(GEOM_Object)::DownCast(edges_i->Value(1));

//    std::cerr << "Search for internal vertices" << std::endl;
    // search for vertices located on both internal pipes
    aFunction = theShape->GetLastFunction();
    theDesc = aFunction->GetDescription();
    Handle(TColStd_HSequenceOfTransient) vertices_i = aShapesOperations->GetShapesOnBox(box_i, theShape, TopAbs_VERTEX, GEOMAlgo_ST_ONIN);
    // Recover previous description to get rid of Propagate dump
    aFunction->SetDescription(theDesc);
//     Handle(TColStd_HSequenceOfTransient) vertices_i = GetCommonShapesOnCylinders(theShape, TopAbs_VERTEX, theR1, theR2);
    if (vertices_i.IsNull() || vertices_i->Length() == 0) {
//        std::cerr << "Internal vertices not found" << std::endl;
        SetErrorCode("Internal vertices not found");
        return false;
    }

    for (int i = 1; i <= vertices_i->Length(); i++) {
        Handle(GEOM_Object) v = Handle(GEOM_Object)::DownCast(vertices_i->Value(i));
        v->GetLastFunction()->SetDescription("");
        TopoDS_Vertex aVertex = TopoDS::Vertex(v->GetValue());
        gp_Pnt aP = BRep_Tool::Pnt(aVertex);
        if (Abs(aP.X()) <= Precision::Confusion()) {
          if (Abs(aP.Y()) - theR2 <= Precision::Confusion())
            vi1 = v;
        } else if (Abs(aP.Y()) <= Precision::Confusion()) {
          if (Abs(aP.X()) - theR1 <= Precision::Confusion())
            vi2 = v;
        }
    }
//    std::cerr << "Internal vertices found" << std::endl;

    std::list<Handle(GEOM_Object)> theShapes;
            
    if (isNormal) {
        Handle(GEOM_Object) ve1, ve2;

        Handle(GEOM_Object) box_e = a3DPrimOperations->MakeBoxDXDYDZ(aR2Ext, aR2Ext, aR1Ext);
        box_e->GetLastFunction()->SetDescription("");
        box_e = aTransformOperations->TranslateDXDYDZ(box_e, -aR2Ext, -aR2Ext, 0);
        box_e->GetLastFunction()->SetDescription("");
        // Common edges on external cylinder
//        std::cerr << "Search for external edges" << std::endl;
        aFunction = theShape->GetLastFunction();
        theDesc = aFunction->GetDescription();
        Handle(TColStd_HSequenceOfTransient) edges_e = aShapesOperations->GetShapesOnBox(box_e, theShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
        // Recover previous description to get rid of Propagate dump
        aFunction->SetDescription(theDesc);
//         Handle(TColStd_HSequenceOfTransient) edges_e = GetCommonShapesOnCylinders(theShape, TopAbs_EDGE, aR1Ext, aR2Ext);
        if (edges_e.IsNull() || edges_e->Length() == 0) {
//            std::cerr << "External edges not found" << std::endl;
            SetErrorCode("External edges not found");
            return false;
        }
        for (int i=1; i<=edges_e->Length();i++) {
            Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast(edges_e->Value(i));
            anObj->GetLastFunction()->SetDescription("");
        }
//        std::cerr << "External edges found" << std::endl;

//        std::cerr << "Search for external vertices" << std::endl;
        // search for vertices located on both external pipes
        aFunction = theShape->GetLastFunction();
        theDesc = aFunction->GetDescription();
        Handle(TColStd_HSequenceOfTransient) vertices_e = aShapesOperations->GetShapesOnBox(box_e, theShape, TopAbs_VERTEX, GEOMAlgo_ST_ONIN);
        // Recover previous description to get rid of Propagate dump
        aFunction->SetDescription(theDesc);
//         Handle(TColStd_HSequenceOfTransient) vertices_e = GetCommonShapesOnCylinders(theShape, TopAbs_VERTEX, aR1Ext, aR2Ext);
        if (vertices_e.IsNull() || vertices_e->Length() == 0) {
//            std::cerr << "External vertices not found" << std::endl;
            SetErrorCode("External vertices not found");
            return false;
        }

        for (int i = 1; i <= vertices_e->Length(); i++) {
            Handle(GEOM_Object) v = Handle(GEOM_Object)::DownCast(vertices_e->Value(i));
            v->GetLastFunction()->SetDescription("");
            TopoDS_Vertex aVertex = TopoDS::Vertex(v->GetValue());
            gp_Pnt aP = BRep_Tool::Pnt(aVertex);
            if (Abs(aP.X()) <= Precision::Confusion()) {
                if (Abs(aP.Y()) - theR2 > Precision::Confusion())
                    ve1 = v;
            } else if (Abs(aP.Y()) <= Precision::Confusion()) {
                if (Abs(aP.X()) - theR2 > Precision::Confusion())
                    ve2 = v;
            }
        }
//        std::cerr << "External vertices found" << std::endl;
        Handle(GEOM_Object) edge_e1, edge_e2;
        try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
            OCC_CATCH_SIGNALS;
#endif
            edge_e1 = aBasicOperations->MakeLineTwoPnt(ve1, vi1);
            if (edge_e1.IsNull()) {
                SetErrorCode("Edge 1 could not be built");
                return false;
            }
        } catch (Standard_Failure) {
            Handle(Standard_Failure) aFail = Standard_Failure::Caught();
            SetErrorCode(aFail->GetMessageString());
            return false;
        }

        try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
            OCC_CATCH_SIGNALS;
#endif
            edge_e2 = aBasicOperations->MakeLineTwoPnt(ve2, vi2);
            if (edge_e2.IsNull()) {
                SetErrorCode("Edge 2 could not be built");
                return false;
            }
        } catch (Standard_Failure) {
            Handle(Standard_Failure) aFail = Standard_Failure::Caught();
            SetErrorCode(aFail->GetMessageString());
            return false;
        }
        
        edge_e1->GetLastFunction()->SetDescription("");
        edge_e2->GetLastFunction()->SetDescription("");

        std::list<Handle(GEOM_Object)> edge_e_elist;
        edge_e_elist.push_back(arete_intersect_int);
        edge_e_elist.push_back(edge_e1);
        edge_e_elist.push_back(Handle(GEOM_Object)::DownCast(edges_e->Value(1)));
        edge_e_elist.push_back(edge_e2);
        wire_t = aShapesOperations->MakeWire(edge_e_elist, 1e-7);
        if (wire_t.IsNull()) {
            SetErrorCode("Impossible to build wire");
            return false;
        }
        wire_t->GetLastFunction()->SetDescription("");
        face_t = aShapesOperations->MakeFace(wire_t, false);
        if (face_t.IsNull()) {
            SetErrorCode("Impossible to build face");
            return false;
        }
        face_t->GetLastFunction()->SetDescription("");
    }
    else {
        Handle(GEOM_Object) P1, P2, P3, P4, P5, P6;
        int idP1, idP2, idP3, idP4;
        int PZX, PZY;
        double ZX=0, ZY=0;
        std::vector<int> LX;
        std::vector<int> LY;
//         Handle(TColStd_HSequenceOfTransient) extremVertices;
        Handle(GEOM_Object) box_e = a3DPrimOperations->MakeBoxDXDYDZ(theVertCylinderRadius, theVertCylinderRadius, theHoriCylinderRadius);
        box_e->GetLastFunction()->SetDescription("");
        box_e = aTransformOperations->TranslateDXDYDZ(box_e, -theVertCylinderRadius, -theVertCylinderRadius, 0);
        box_e->GetLastFunction()->SetDescription("");

        aFunction = theShape->GetLastFunction();
        theDesc = aFunction->GetDescription();
        Handle(TColStd_HSequenceOfTransient) extremVertices = aShapesOperations->GetShapesOnBox(box_e, theShape, TopAbs_VERTEX, GEOMAlgo_ST_ONIN);
        // Recover previous description to get rid of Propagate dump
        aFunction->SetDescription(theDesc);
        
//         extremVertices = aShapesOperations->GetShapesOnCylinder(theShape, TopAbs_VERTEX, Vector_Z, theVertCylinderRadius, GEOMAlgo_ST_ONIN);
        if (extremVertices.IsNull() || extremVertices->Length() == 0) {
//            std::cerr << "extremVertices.IsNull() || extremVertices->Length() == 0" << std::endl;
            if (theRF == 0)
                SetErrorCode("Vertices on chamfer not found");
            else
                SetErrorCode("Vertices on fillet not found");
            return false;
        }
        
//         std::cerr << "Found " << extremVertices->Length() << " vertices" << std::endl;
        theShapes.push_back(theShape);
        theShapes.push_back(box_e);
        if (extremVertices->Length() != 6) {
//           for (int i=1; i<=extremVertices->Length(); i++){
//             theShapes.push_back(Handle(GEOM_Object)::DownCast(extremVertices->Value(i)));
//           }
//           Handle(GEOM_Object) aCompound = aShapesOperations->MakeCompound(theShapes);
//           TopoDS_Shape aCompoundShape = aCompound->GetValue();
//           theShape->GetLastFunction()->SetValue(aCompoundShape);
          SetErrorCode("Bad number of vertices on chamfer found");
          return false;
        }
        
//         std::cerr << "BEGIN of parsing list of vertices" << std::endl;
        for (int i=1; i<=extremVertices->Length(); i++){
            Handle(GEOM_Object) aV = Handle(GEOM_Object)::DownCast(extremVertices->Value(i));
            aV->GetLastFunction()->SetDescription("");
//             std::cerr << "Vertex #" <<  i << std::endl;
            gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(aV->GetValue()));
//             std::cerr << "aP.X() " <<  aP.X() << std::endl;
//             std::cerr << "aP.Y() " <<  aP.Y() << std::endl;
//             std::cerr << "aP.Z() " <<  aP.Z() << std::endl;
//             if (Abs(aP.Z() - theL2) < Precision::Confusion()) {
// //                std::cerr << "Vertex = L2 ==> OUT" << std::endl;
//                 continue;
//             }
//             if (aP.Z() < 0) {
// //                std::cerr << "Vertex < 0 ==> OUT" << std::endl;
//                 continue;
//             }

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
//         std::cerr << "END of parsing list of vertices" << std::endl;
//         std::cerr << "LX:";
//         for (int i=0;i<LX.size();i++)
//           std::cerr << " " << LX.at(i);
//         std::cerr << std::endl;
//         std::cerr << "LY:";
//         for (int i=0;i<LY.size();i++)
//           std::cerr << " " << LY.at(i);
//         std::cerr << std::endl;
//         std::cerr << "PZX: " << PZX << std::endl;
//         std::cerr << "PZY: " << PZY << std::endl;

        idP2 = PZX;
        idP4 = PZY;
        idP1 = LX.at(0);
        if (LX.at(0) == PZX)
          idP1 = LX.at(1);
        idP3 = LY.at(0);
        if (LY.at(0) == PZY)
          idP3 = LY.at(1);

//         std::cerr << "idP1: " << idP1 << std::endl;
//         std::cerr << "idP2: " << idP2 << std::endl;
//         std::cerr << "idP3: " << idP3 << std::endl;
//         std::cerr << "idP4: " << idP4 << std::endl;

        P1 = Handle(GEOM_Object)::DownCast(extremVertices->Value(idP1));
        P2 = Handle(GEOM_Object)::DownCast(extremVertices->Value(idP2));
        P3 = Handle(GEOM_Object)::DownCast(extremVertices->Value(idP3));
        P4 = Handle(GEOM_Object)::DownCast(extremVertices->Value(idP4));

//         std::cerr << "Building edge 1 in thickness" << std::endl;
        Handle(GEOM_Object) Cote_1 = aBasicOperations->MakeLineTwoPnt(P1, vi1);
        if (Cote_1.IsNull()) {
            SetErrorCode("Impossilbe to build edge in thickness");
            return false;
        }
        Cote_1->GetLastFunction()->SetDescription("");
        
//         std::cerr << "Building edge 2 in thickness" << std::endl;
        Handle(GEOM_Object) Cote_2 = aBasicOperations->MakeLineTwoPnt(vi2, P3);
        if (Cote_2.IsNull()) {
            SetErrorCode("Impossilbe to build edge in thickness");
            return false;
        }
        Cote_2->GetLastFunction()->SetDescription("");

        // edge_chan_princ = arete du chanfrein (ou raccord) sur le tuyau principal
        // edge_chan_inc = arete du chanfrein (ou raccord) sur le tuyau incident
//         std::cerr << "Getting chamfer edge on main pipe" << std::endl;
        Handle(GEOM_Object) edge_chan_princ = aBlocksOperations->GetEdge(theShape, P1, P3);
        if (edge_chan_princ.IsNull()) {
            SetErrorCode("Impossilbe to find edge on main pipe");
            return false;
        }
        edge_chan_princ->GetLastFunction()->SetDescription("");
        
//         std::cerr << "Getting chamfer edge on incident pipe" << std::endl;
        Handle(GEOM_Object) edge_chan_inc = aBlocksOperations->GetEdge(theShape, P2, P4);
        if (edge_chan_inc.IsNull()) {
            SetErrorCode("Impossilbe to find edge on incident pipe");
            return false;
        }
        edge_chan_inc->GetLastFunction()->SetDescription("");

        std::list<Handle(GEOM_Object)> edgeList1;
        edgeList1.push_back(edge_chan_princ);
        edgeList1.push_back(Cote_1);
        edgeList1.push_back(arete_intersect_int);
        edgeList1.push_back(Cote_2);
        
//         std::cerr << "Creating wire 1" << std::endl;
        wire_t = aShapesOperations->MakeWire(edgeList1, 1e-7);
        if (wire_t.IsNull()) {
            SetErrorCode("Impossible to build wire");
            return false;
        }
        wire_t->GetLastFunction()->SetDescription("");
        
//         std::cerr << "Creating face 1" << std::endl;
        face_t = aShapesOperations->MakeFace(wire_t, false);
        if (face_t.IsNull()) {
            SetErrorCode("Impossible to build face");
            return false;
        }
        face_t->GetLastFunction()->SetDescription("");
        theShapes.push_back(face_t);
          
        gp_Pnt aP2 = BRep_Tool::Pnt(TopoDS::Vertex(P2->GetValue()));
        gp_Pnt aP5 = BRep_Tool::Pnt(TopoDS::Vertex(vi1->GetValue()));
        double deltaZ = aP2.Z() - aP5.Z();
//         std::cerr << "Creating new point from vi1 with deltaZ = " << deltaZ << std::endl;
        Handle(GEOM_Object) P5bis = aTransformOperations->TranslateDXDYDZCopy(vi1, 0, 0, deltaZ);
        if (P5bis.IsNull()) {
            SetErrorCode("Impossible to translate vertex");
            return false;
        }
        P5bis->GetLastFunction()->SetDescription("");

        gp_Pnt aP4 = BRep_Tool::Pnt(TopoDS::Vertex(P4->GetValue()));
        gp_Pnt aP6 = BRep_Tool::Pnt(TopoDS::Vertex(vi2->GetValue()));
        deltaZ = aP4.Z() - aP6.Z();
//         std::cerr << "Creating new point from vi2 with deltaZ = " << deltaZ << std::endl;
        Handle(GEOM_Object) P6bis = aTransformOperations->TranslateDXDYDZCopy(vi2, 0, 0, deltaZ);
        if (P6bis.IsNull()) {
            SetErrorCode("Impossible to translate vertex");
            return false;
        }
        P6bis->GetLastFunction()->SetDescription("");

//         std::cerr << "Creating new line 1 from 2 previous points" << std::endl;
        Handle(GEOM_Object) Cote_3 = aBasicOperations->MakeLineTwoPnt(P5bis, P2);
        if (Cote_3.IsNull()) {
            SetErrorCode("Impossilbe to build edge in thickness");
            return false;
        }
        Cote_3->GetLastFunction()->SetDescription("");
        
//         std::cerr << "Creating new line 2 from 2 previous points" << std::endl;
        Handle(GEOM_Object) Cote_4 = aBasicOperations->MakeLineTwoPnt(P6bis, P4);
        if (Cote_4.IsNull()) {
            SetErrorCode("Impossilbe to build edge in thickness");
            return false;
        }
        Cote_4->GetLastFunction()->SetDescription("");
        
//         std::cerr << "Creating new line 3 from 2 previous points" << std::endl;
        Handle(GEOM_Object) Cote_5 = aBasicOperations->MakeLineTwoPnt(P5bis, P6bis);
        if (Cote_4.IsNull()) {
            SetErrorCode("Impossilbe to build edge in thickness");
            return false;
        }
        Cote_5->GetLastFunction()->SetDescription("");

        std::list<Handle(GEOM_Object)> edgeList2;
        edgeList2.push_back(edge_chan_inc);
        edgeList2.push_back(Cote_3);
        edgeList2.push_back(Cote_5);
        edgeList2.push_back(Cote_4);
//         std::cerr << "Creating wire 2" << std::endl;
        wire_t2 = aShapesOperations->MakeWire(edgeList2, 1e-7);
        if (wire_t2.IsNull()) {
            SetErrorCode("Impossible to build wire");
            return false;
        }
        wire_t2->GetLastFunction()->SetDescription("");
//         std::cerr << "Creating face 2" << std::endl;
        face_t2 = aShapesOperations->MakeFace(wire_t2, false);
        if (face_t2.IsNull()) {
            SetErrorCode("Impossible to build face");
            return false;
        }
        face_t2->GetLastFunction()->SetDescription("");
        theShapes.push_back(face_t2);
    }

    // Planes
    Handle(GEOM_Object) aP0 = aBasicOperations->MakePointXYZ(0, 0, 0);
    Handle(GEOM_Object) aVZ = aBasicOperations->MakeVectorDXDYDZ(0, 0, 1);
    Handle(GEOM_Object) aVXZ = aBasicOperations->MakeVectorDXDYDZ(aR1Ext, 0, 0.5*(theL1+theVertCylinderRadius));
    Handle(GEOM_Object) aPlnOZ = aBasicOperations->MakePlanePntVec(aP0, aVZ, aSize);
    Handle(GEOM_Object) aPlnOXZ = aBasicOperations->MakePlanePntVec(aP0, aVXZ, aSize);
    aP0->GetLastFunction()->SetDescription("");
    aVZ->GetLastFunction()->SetDescription("");
    aVXZ->GetLastFunction()->SetDescription("");
    aPlnOZ->GetLastFunction()->SetDescription("");
    aPlnOXZ->GetLastFunction()->SetDescription("");
    theShapes.push_back(aPlnOZ);
    theShapes.push_back(aPlnOXZ);

      // Partition
//    Handle(GEOM_Object) Part0 = aBooleanOperations->MakeHalfPartition(theShape, face_t);
//    if (Part0.IsNull()) {
//        std::cerr << "Impossible to build partition between TShape and 1st face" << std::endl;
//        SetErrorCode("Impossible to build partition between TShape and 1st face");
//        return false;
//    }
//    Part0->GetLastFunction()->SetDescription("");
//
//    Handle(GEOM_Object) Te3 ;
//    if (isNormal) {
//      if (Abs(aR1Ext - aR2Ext) <= Precision::Approximation()) {
//        std::cerr << "External radius are identical: we do not make partition with plane OXZ" << std::endl;
//        Te3 = aBooleanOperations->MakeHalfPartition(Part0, aPlnOZ);
//      }
//      else {
//        Handle(GEOM_Object) Part1 = aBooleanOperations->MakeHalfPartition(Part0, aPlnOXZ);
//        if (Part1.IsNull()) {
//          std::cerr << "Impossible to build partition between TShape and plane OXZ" << std::endl;
//          SetErrorCode("Impossible to build partition between TShape and plane OXZ");
//          return false;
//        }
//        Part1->GetLastFunction()->SetDescription("");
//        Te3 = aBooleanOperations->MakeHalfPartition(Part1, aPlnOZ);
//      }
//      if (Te3.IsNull()) {
//          std::cerr << "Impossible to build partition between TShape and plane OZ" << std::endl;
//          SetErrorCode("Impossible to build partition between TShape and plane OZ");
//          return false;
//      }
//      Te3->GetLastFunction()->SetDescription("");
//    }
//    else {
//      if (Abs(aR1Ext - aR2Ext) <= Precision::Approximation()){ // We should never go here
//        SetErrorCode("Impossible to build TShape");
//        return false;
//      }
//      else {
//        Handle(GEOM_Object) Part1 = aBooleanOperations->MakeHalfPartition(Part0, aPlnOXZ);
//        if (Part1.IsNull()) {
//        std::cerr << "Impossible to build partition between TShape and plane OXZ" << std::endl;
//          SetErrorCode("Impossible to build partition between TShape and plane OXZ");
//          return false;
//        }
//        Part1->GetLastFunction()->SetDescription("");
//        Handle(GEOM_Object) Part2 = aBooleanOperations->MakeHalfPartition(Part1, aPlnOZ);
//        if (Part2.IsNull()) {
//        std::cerr << "Impossible to build partition between TShape and plane OZ" << std::endl;
//          SetErrorCode("Impossible to build partition between TShape and plane OZ");
//          return false;
//        }
//        Part2->GetLastFunction()->SetDescription("");
//        Te3 = aBooleanOperations->MakeHalfPartition(Part2, face_t2);
//        if (Te3.IsNull()) {
//            std::cerr << "Impossible to build partition between TShape and 2nd face" << std::endl;
//            SetErrorCode("Impossible to build partition between TShape and 2nd face");
//            return false;
//        }
//        Te3->GetLastFunction()->SetDescription("");
//      }
//    }

     Handle(TColStd_HSequenceOfTransient) partitionShapes = new TColStd_HSequenceOfTransient;
     Handle(TColStd_HSequenceOfTransient) theTools = new TColStd_HSequenceOfTransient;
     Handle(TColStd_HSequenceOfTransient) theKeepInside = new TColStd_HSequenceOfTransient;
     Handle(TColStd_HSequenceOfTransient) theRemoveInside = new TColStd_HSequenceOfTransient;
     Handle(TColStd_HArray1OfInteger) theMaterials;
     partitionShapes->Append(theShape);
     theTools->Append(aPlnOZ);
     theTools->Append(aPlnOXZ);
     theTools->Append(face_t);
     if (!isNormal)
         theTools->Append(face_t2);

     Handle(GEOM_Object) Te3 = aBooleanOperations->MakePartition(partitionShapes, theTools, theKeepInside, theRemoveInside, TopAbs_SOLID, false, theMaterials, 0, false);
     if (Te3.IsNull()) {
         SetErrorCode("Impossible to build partition of TShape");
//         Handle(GEOM_Object) aCompound = aShapesOperations->MakeCompound(theShapes);
//         TopoDS_Shape aCompoundShape = aCompound->GetValue();
//         theShape->GetLastFunction()->SetValue(aCompoundShape);
         return false;
     }
     Te3->GetLastFunction()->SetDescription("");


    TopoDS_Shape aShape = Te3->GetValue();
    theShape->GetLastFunction()->SetValue(aShape);
    
    SetErrorCode(OK);
    return true;
}

// Mirror and glue faces
bool GEOMImpl_IAdvancedOperations::MakePipeTShapeMirrorAndGlue(/*std::vector<GEOM_IOperations*> theOperations, */Handle(GEOM_Object) theShape, 
      double theR1, double theW1, double theL1, double theR2, double theW2, double theL2) {
    SetErrorCode(KO);
    
    // Useful values
    double aSize = 2*(theL1 + theL2);
    double aR1Ext = theR1 + theW1;
    /*
    GEOMImpl_IBasicOperations* aBasicOperations = (GEOMImpl_IBasicOperations*) &theOperations[1];
    GEOMImpl_IShapesOperations* aShapesOperations = (GEOMImpl_IShapesOperations*) &theOperations[4];
    GEOMImpl_ITransformOperations* aTransformOperations = (GEOMImpl_ITransformOperations*) &theOperations[5];*/

    GEOMImpl_IBasicOperations* aBasicOperations = new GEOMImpl_IBasicOperations(GetEngine(), GetDocID());
    GEOMImpl_IShapesOperations* aShapesOperations = new GEOMImpl_IShapesOperations(GetEngine(), GetDocID());
    GEOMImpl_ITransformOperations* aTransformOperations = new GEOMImpl_ITransformOperations(GetEngine(), GetDocID());

    // Planes
    Handle(GEOM_Object) aP0 = aBasicOperations->MakePointXYZ(0, 0, 0);
    aP0->GetLastFunction()->SetDescription("");
    Handle(GEOM_Object) aVX = aBasicOperations->MakeVectorDXDYDZ(1, 0, 0);
    Handle(GEOM_Object) aVY = aBasicOperations->MakeVectorDXDYDZ(0, 1, 0);
    aVX->GetLastFunction()->SetDescription("");
    aVY->GetLastFunction()->SetDescription("");
    Handle(GEOM_Object) aPlane_OX = aBasicOperations->MakePlanePntVec(aP0, aVX, 2*(aR1Ext + theL2));
    Handle(GEOM_Object) aPlane_OY = aBasicOperations->MakePlanePntVec(aP0, aVY, aSize);
    aPlane_OX->GetLastFunction()->SetDescription("");
    aPlane_OY->GetLastFunction()->SetDescription("");

    Handle(GEOM_Object) Te4 = aTransformOperations->MirrorPlaneCopy(theShape, aPlane_OX);
    if (Te4.IsNull()) {
        SetErrorCode("Impossible to build mirror of quarter TShape");
        return false;
    }

//    std::list<Handle(GEOM_Object)> aShapes1, aShapes2;
//    aShapes1.push_back(Te3);
//    aShapes1.push_back(Te4);
//    Handle(GEOM_Object) Te5 = aShapesOperations->MakeCompound(aShapes1);
//    if (Te4.IsNull()) {
//        SetErrorCode("Impossible to build compound");
//        return false;
//    }
//    Te5->GetLastFunction()->SetDescription("");

    Handle(GEOM_Object) Te5 = aTransformOperations->MirrorPlaneCopy(theShape, aPlane_OY);
    if (Te5.IsNull()) {
        SetErrorCode("Impossible to build mirror of half TShape");
        return false;
    }

    Handle(GEOM_Object) Te6 = aTransformOperations->MirrorPlaneCopy(Te4, aPlane_OY);
    if (Te6.IsNull()) {
        SetErrorCode("Impossible to build mirror of half TShape");
        return false;
    }

    std::list<Handle(GEOM_Object)> aShapesList;
    aShapesList.push_back(theShape);
    aShapesList.push_back(Te4);
    aShapesList.push_back(Te5);
    aShapesList.push_back(Te6);
    Handle(GEOM_Object) Te7 = aShapesOperations->MakeCompound(aShapesList);
    if (Te7.IsNull()) {
        SetErrorCode("Impossible to build compound");
        return false;
    }

    Handle(GEOM_Object) Te8 = aShapesOperations->MakeGlueFaces(Te7, 1e-7, true);
    if (Te8.IsNull()) {
        SetErrorCode("Impossible to glue faces of TShape");
        return false;
    }

    TopoDS_Shape aShape = Te8->GetValue();
//     TopTools_IndexedMapOfShape aMapOfShapes;
//    TopExp::MapShapes(aShape, aMapOfShapes);
//     TopExp::MapShapes(aShape, TopAbs_COMPOUND, aMapOfShapes);

//    std::cerr << "aMapOfShapes.Extent(): " << aMapOfShapes.Extent() << std::endl;
//    if (aMapOfShapes.Extent() != 1){
//        SetErrorCode("Result of partition is not correct");
//        return false;
//    }

    theShape->GetLastFunction()->SetValue(aShape);

    Te4->GetLastFunction()->SetDescription("");
    Te5->GetLastFunction()->SetDescription("");
    Te6->GetLastFunction()->SetDescription("");
    Te7->GetLastFunction()->SetDescription("");
    Te8->GetLastFunction()->SetDescription("");

    SetErrorCode(OK);
    return true;
}

//=============================================================================
/*!
 *  MakePipeTShape
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length).
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
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
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IAdvancedOperations::MakePipeTShape(double theR1, double theW1,
        double theL1, double theR2, double theW2, double theL2, bool theHexMesh) {
    MESSAGE("GEOMImpl_IAdvancedOperations::MakePipeTShape");
    SetErrorCode(KO);
    //Add a new object
//     std::cerr << "Add a new object" << std::endl;
    Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);

    //Add a new shape function with parameters
//     std::cerr << "Add a new shape function with parameters" << std::endl;
    Handle(GEOM_Function) aFunction = aShape->AddFunction(GEOMImpl_PipeTShapeDriver::GetID(), TSHAPE_BASIC);
    if (aFunction.IsNull()) return NULL;

    //Check if the function is set correctly
//     std::cerr << "Check if the function is set correctly" << std::endl;
    if (aFunction->GetDriverGUID() != GEOMImpl_PipeTShapeDriver::GetID()) return NULL;

    GEOMImpl_IPipeTShape aData(aFunction);

    aData.SetR1(theR1);
    aData.SetW1(theW1);
    aData.SetL1(theL1);
    aData.SetR2(theR2);
    aData.SetW2(theW2);
    aData.SetL2(theL2);
    aData.SetHexMesh(theHexMesh);

//     std::cerr << "Compute the resulting value" << std::endl;
    //Compute the resulting value
    try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
        OCC_CATCH_SIGNALS;
#endif
        if (!GetSolver()->ComputeFunction(aFunction)) {
            SetErrorCode("TShape driver failed");
//            MESSAGE("TShape driver failed");
            return NULL;
        }
//         std::cerr << "aShape->GetName(): " << aShape->GetName() << std::endl;
    } catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        SetErrorCode(aFail->GetMessageString());
        return NULL;
    }
    
    GEOMImpl_IBasicOperations* aBasicOperations = new GEOMImpl_IBasicOperations(GetEngine(), GetDocID());
    GEOMImpl_IBooleanOperations* aBooleanOperations = new GEOMImpl_IBooleanOperations(GetEngine(), GetDocID());
    GEOMImpl_IShapesOperations* aShapesOperations = new GEOMImpl_IShapesOperations(GetEngine(), GetDocID());
    GEOMImpl_ITransformOperations* aTransformOperations = new GEOMImpl_ITransformOperations(GetEngine(), GetDocID());
    GEOMImpl_IBlocksOperations* aBlocksOperations = new GEOMImpl_IBlocksOperations(GetEngine(), GetDocID());
    GEOMImpl_I3DPrimOperations* a3DPrimOperations = new GEOMImpl_I3DPrimOperations(GetEngine(), GetDocID());
    std::vector<GEOM_IOperations*> theOperations;
    theOperations.push_back(a3DPrimOperations);
    theOperations.push_back(aBasicOperations);
    theOperations.push_back(aBlocksOperations);
    theOperations.push_back(aBooleanOperations);
    theOperations.push_back(aShapesOperations);
    theOperations.push_back(aTransformOperations);
    
    if (theHexMesh) {
//         std::cerr << "Creating partition" << std::endl;
        if (!MakePipeTShapePartition(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2))
          return NULL;
//         std::cerr << "Done" << std::endl;
//         std::cerr << "Creating mirrors and glue" << std::endl;
        if (!MakePipeTShapeMirrorAndGlue(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2))
          return NULL;
//         std::cerr << "Done" << std::endl;
    }

    Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
//     std::cerr << "Add shape in result list" << std::endl;
    aSeq->Append(aShape);

    if (theHexMesh) {
        /*
         * Get the groups: BEGIN
         */
        if (!MakeGroups(/*theOperations, */aShape, TSHAPE_BASIC, theR1, theW1, theL1, theR2, theW2, theL2, aSeq, gp_Trsf())) {
//            SetErrorCode("Make groups failed");
            return NULL;
        }

        TCollection_AsciiString aListRes, anEntry;
        // Iterate over the sequence aSeq
        Standard_Integer aNbGroups = aSeq->Length();
        Standard_Integer i = 2;
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

        //Make a Python command
        GEOM::TPythonDump(aFunction) << "[" << aShape << ", " << aListRes.ToCString() << "] = geompy.MakePipeTShape("
                << theR1 << ", " << theW1 << ", " << theL1 << ", " << theR2 << ", " << theW2 << ", " << theL2 << ", "
                << theHexMesh << ")";
    }
    /*
     * Get the groups: END
     */
    else {
        //Make a Python command
        GEOM::TPythonDump(aFunction) << "[" << aShape << "] = geompy.MakePipeTShape(" << theR1 << ", " << theW1 << ", "
                << theL1 << ", " << theR2 << ", " << theW2 << ", " << theL2 << ", " << theHexMesh << ")";
    }

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
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IAdvancedOperations::MakePipeTShapeWithPosition(double theR1,
        double theW1, double theL1, double theR2, double theW2, double theL2, bool theHexMesh,
        Handle(GEOM_Object) theP1, Handle(GEOM_Object) theP2, Handle(GEOM_Object) theP3) {
    SetErrorCode(KO);
    //Add a new object
    Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);
    /////////////////
    // TSHAPE CODE
    /////////////////
    //Add a new shape function with parameters
    Handle(GEOM_Function) aFunction = aShape->AddFunction(GEOMImpl_PipeTShapeDriver::GetID(), TSHAPE_BASIC);
    if (aFunction.IsNull()) return NULL;

    //Check if the function is set correctly
    if (aFunction->GetDriverGUID() != GEOMImpl_PipeTShapeDriver::GetID()) return NULL;

    // Check new position
    if (!CheckCompatiblePosition(theL1, theL2, theP1, theP2, theP3, 0.01)) {
        return NULL;
    }

    GEOMImpl_IPipeTShape aData(aFunction);

    aData.SetR1(theR1);
    aData.SetW1(theW1);
    aData.SetL1(theL1);
    aData.SetR2(theR2);
    aData.SetW2(theW2);
    aData.SetL2(theL2);
    aData.SetHexMesh(theHexMesh);

    //Compute the resulting value
    try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
        OCC_CATCH_SIGNALS;
#endif
        if (!GetSolver()->ComputeFunction(aFunction)) {
            SetErrorCode("TShape driver failed");
            return NULL;
        }
    } catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        SetErrorCode(aFail->GetMessageString());
        return NULL;
    }

    GEOMImpl_IBasicOperations* aBasicOperations = new GEOMImpl_IBasicOperations(GetEngine(), GetDocID());
    GEOMImpl_IBooleanOperations* aBooleanOperations = new GEOMImpl_IBooleanOperations(GetEngine(), GetDocID());
    GEOMImpl_IShapesOperations* aShapesOperations = new GEOMImpl_IShapesOperations(GetEngine(), GetDocID());
    GEOMImpl_ITransformOperations* aTransformOperations = new GEOMImpl_ITransformOperations(GetEngine(), GetDocID());
    GEOMImpl_IBlocksOperations* aBlocksOperations = new GEOMImpl_IBlocksOperations(GetEngine(), GetDocID());
    GEOMImpl_I3DPrimOperations* a3DPrimOperations = new GEOMImpl_I3DPrimOperations(GetEngine(), GetDocID());
    std::vector<GEOM_IOperations*> theOperations;
    theOperations.push_back(a3DPrimOperations);
    theOperations.push_back(aBasicOperations);
    theOperations.push_back(aBlocksOperations);
    theOperations.push_back(aBooleanOperations);
    theOperations.push_back(aShapesOperations);
    theOperations.push_back(aTransformOperations);
    
    if (theHexMesh) {
//        std::cerr << "Creating partition" << std::endl;
        if (!MakePipeTShapePartition(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2))
          return NULL;
//        std::cerr << "Done" << std::endl;
//        std::cerr << "Creating mirrors and glue" << std::endl;
        if (!MakePipeTShapeMirrorAndGlue(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2))
          return NULL;
//        std::cerr << "Done" << std::endl;
    }

    TopoDS_Shape Te = aShape->GetValue();

    // Set Position
    gp_Trsf aTrsf = GetPositionTrsf(theL1, theL2, theP1, theP2, theP3);
    BRepBuilderAPI_Transform aTransformation(Te, aTrsf, Standard_False);
    TopoDS_Shape aTrsf_Shape = aTransformation.Shape();
    aFunction->SetValue(aTrsf_Shape);
    Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
    aSeq->Append(aShape);

    if (theHexMesh) {
        //
        // Get the groups: BEGIN
        //
        if (!MakeGroups(/*theOperations, */aShape,TSHAPE_BASIC, theR1, theW1, theL1, theR2, theW2, theL2, aSeq, aTrsf)) {
//            SetErrorCode("Make groups failed");
            return NULL;
        }

        TCollection_AsciiString aListRes, anEntry;
        // Iterate over the sequence aSeq
        Standard_Integer aNbGroups = aSeq->Length();
        Standard_Integer i = 2;
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

        //Make a Python command
        GEOM::TPythonDump(aFunction) << "[" << aShape << ", " << aListRes.ToCString() << "] = geompy.MakePipeTShape("
                << theR1 << ", " << theW1 << ", " << theL1 << ", " << theR2 << ", " << theW2 << ", " << theL2 << ", "
                << theHexMesh << ", " << theP1 << ", " << theP2 << ", " << theP3 << ")";
    }
    //
    // Get the groups: END
    //

    else {
        //Make a Python command
        GEOM::TPythonDump(aFunction) << "[" << aShape << "] = geompy.MakePipeTShape(" << theR1 << ", " << theW1 << ", "
                << theL1 << ", " << theR2 << ", " << theW2 << ", " << theL2 << ", " << theHexMesh << ", " << theP1
                << ", " << theP2 << ", " << theP3 << ")";
    }

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
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IAdvancedOperations::MakePipeTShapeChamfer(double theR1, double theW1,
        double theL1, double theR2, double theW2, double theL2, double theH, double theW, bool theHexMesh) {
    SetErrorCode(KO);
    //Add a new object
    Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);
    //Add a new shape function with parameters
    Handle(GEOM_Function) aFunction = aShape->AddFunction(GEOMImpl_PipeTShapeDriver::GetID(), TSHAPE_CHAMFER);
    if (aFunction.IsNull()) return NULL;

    //Check if the function is set correctly
    if (aFunction->GetDriverGUID() != GEOMImpl_PipeTShapeDriver::GetID()) return NULL;

    GEOMImpl_IPipeTShape aData(aFunction);

    aData.SetR1(theR1);
    aData.SetW1(theW1);
    aData.SetL1(theL1);
    aData.SetR2(theR2);
    aData.SetW2(theW2);
    aData.SetL2(theL2);
    aData.SetH(theH);
    aData.SetW(theW);
    aData.SetHexMesh(theHexMesh);

    //Compute the resulting value
    try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
        OCC_CATCH_SIGNALS;
#endif
        if (!GetSolver()->ComputeFunction(aFunction)) {
            SetErrorCode("TShape driver failed");
            return NULL;
        }
    } catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        SetErrorCode(aFail->GetMessageString());
        return NULL;
    }

    GEOMImpl_IBasicOperations* aBasicOperations = new GEOMImpl_IBasicOperations(GetEngine(), GetDocID());
    GEOMImpl_IBooleanOperations* aBooleanOperations = new GEOMImpl_IBooleanOperations(GetEngine(), GetDocID());
    GEOMImpl_IShapesOperations* aShapesOperations = new GEOMImpl_IShapesOperations(GetEngine(), GetDocID());
    GEOMImpl_ITransformOperations* aTransformOperations = new GEOMImpl_ITransformOperations(GetEngine(), GetDocID());
    GEOMImpl_IBlocksOperations* aBlocksOperations = new GEOMImpl_IBlocksOperations(GetEngine(), GetDocID());
    GEOMImpl_I3DPrimOperations* a3DPrimOperations = new GEOMImpl_I3DPrimOperations(GetEngine(), GetDocID());
    GEOMImpl_ILocalOperations* aLocalOperations = new GEOMImpl_ILocalOperations(GetEngine(), GetDocID());
    std::vector<GEOM_IOperations*> theOperations;
    theOperations.push_back(a3DPrimOperations);
    theOperations.push_back(aBasicOperations);
    theOperations.push_back(aBlocksOperations);
    theOperations.push_back(aBooleanOperations);
    theOperations.push_back(aShapesOperations);
    theOperations.push_back(aTransformOperations);

    // BEGIN of chamfer
    TopoDS_Shape aShapeShape = aShape->GetValue();
    TopTools_IndexedMapOfShape anEdgesIndices;
    TopExp::MapShapes(aShapeShape, anEdgesIndices);
    // Common edges on external cylinders
    Handle(GEOM_Object) box_e;
    if (theHexMesh) {
      box_e = a3DPrimOperations->MakeBoxDXDYDZ(theR2+theW2, theR2+theW2, theR1+theW1);
    }
    else {
      box_e = a3DPrimOperations->MakeBoxDXDYDZ(2*(theR2+theW2), 2*(theR2+theW2), theR1+theW1);
    }
    box_e->GetLastFunction()->SetDescription("");
    box_e = aTransformOperations->TranslateDXDYDZ(box_e, -(theR2+theW2), -(theR2+theW2), 0);
    box_e->GetLastFunction()->SetDescription("");
    
    Handle(TColStd_HSequenceOfInteger) edges_e = aShapesOperations->GetShapesOnBoxIDs(box_e, aShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
    box_e->GetLastFunction()->SetDescription("");

    if (edges_e.IsNull() || edges_e->Length() == 0) {
        SetErrorCode("External edges not found");
        return false;
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
      aChamfer = aLocalOperations->MakeChamferEdges(aShape, theW, theH, theEdges);
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

 //   bool doMesh = false;
    if (theHexMesh) {
//        doMesh = true;
        if (!MakePipeTShapePartition(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, 0, false)) {
            MESSAGE("PipeTShape partition failed");
//            doMesh = false;
            return NULL;
        }
        if (!MakePipeTShapeMirrorAndGlue(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2)) {
          MESSAGE("PipeTShape mirrors and glue failed");
//          doMesh = false;
          return NULL;
        }
    }

    Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
    aSeq->Append(aShape);

//    if (doMesh) {
    if (theHexMesh) {
//
//         Get the groups: BEGIN
//
//        if (!MakeGroups(/*theOperations, */aShape, TSHAPE_CHAMFER, theR1, theW1, theL1, theR2, theW2, theL2, aSeq, gp_Trsf())) {
//            //Make a Python command
//            GEOM::TPythonDump(aFunction) << "[" << aShape << "] = geompy.MakePipeTShapeChamfer(" << theR1 << ", " << theW1
//                    << ", " << theL1 << ", " << theR2 << ", " << theW2 << ", " << theL2 << ", " << theH << ", " << theW
//                    << ", " << theHexMesh << ")";
//        }
//        else {
        if (!MakeGroups(/*theOperations, */aShape, TSHAPE_CHAMFER, theR1, theW1, theL1, theR2, theW2, theL2, aSeq, gp_Trsf()))
	  return NULL;

	TCollection_AsciiString aListRes, anEntry;
	// Iterate over the sequence aSeq
	Standard_Integer aNbGroups = aSeq->Length();
	Standard_Integer i = 2;
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

	//Make a Python command
	GEOM::TPythonDump(aFunction) << "[" << aShape << ", " << aListRes.ToCString()
	  << "] = geompy.MakePipeTShapeChamfer(" << theR1 << ", " << theW1 << ", " << theL1 << ", " << theR2
	  << ", " << theW2 << ", " << theL2 << ", " << theH << ", " << theW << ", " << theHexMesh << ")";
	//        }
    }
//
//     Get the groups: END
//
    else {
        //Make a Python command
        GEOM::TPythonDump(aFunction) << "[" << aShape << "] = geompy.MakePipeTShapeChamfer(" << theR1 << ", " << theW1
                << ", " << theL1 << ", " << theR2 << ", " << theW2 << ", " << theL2 << ", " << theH << ", " << theW
                << ", " << theHexMesh << ")";
    }

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
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IAdvancedOperations::MakePipeTShapeChamferWithPosition(double theR1,
        double theW1, double theL1, double theR2, double theW2, double theL2, double theH, double theW,
        bool theHexMesh, Handle(GEOM_Object) theP1, Handle(GEOM_Object) theP2, Handle(GEOM_Object) theP3) {
    SetErrorCode(KO);
    //Add a new object
    Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);
    //Add a new shape function with parameters
    Handle(GEOM_Function) aFunction = aShape->AddFunction(GEOMImpl_PipeTShapeDriver::GetID(), TSHAPE_CHAMFER);
    if (aFunction.IsNull()) return NULL;

    //Check if the function is set correctly
    if (aFunction->GetDriverGUID() != GEOMImpl_PipeTShapeDriver::GetID()) return NULL;

    // Check new position
    if (!CheckCompatiblePosition(theL1, theL2, theP1, theP2, theP3, 0.01)) {
        return NULL;
    }

    GEOMImpl_IPipeTShape aData(aFunction);

    aData.SetR1(theR1);
    aData.SetW1(theW1);
    aData.SetL1(theL1);
    aData.SetR2(theR2);
    aData.SetW2(theW2);
    aData.SetL2(theL2);
    aData.SetH(theH);
    aData.SetW(theW);
    aData.SetHexMesh(theHexMesh);

    //Compute the resulting value
    try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
        OCC_CATCH_SIGNALS;
#endif
        if (!GetSolver()->ComputeFunction(aFunction)) {
            SetErrorCode("TShape driver failed");
            return NULL;
        }
    } catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        SetErrorCode(aFail->GetMessageString());
        return NULL;
    }

    GEOMImpl_IBasicOperations* aBasicOperations = new GEOMImpl_IBasicOperations(GetEngine(), GetDocID());
    GEOMImpl_IBooleanOperations* aBooleanOperations = new GEOMImpl_IBooleanOperations(GetEngine(), GetDocID());
    GEOMImpl_IShapesOperations* aShapesOperations = new GEOMImpl_IShapesOperations(GetEngine(), GetDocID());
    GEOMImpl_ITransformOperations* aTransformOperations = new GEOMImpl_ITransformOperations(GetEngine(), GetDocID());
    GEOMImpl_IBlocksOperations* aBlocksOperations = new GEOMImpl_IBlocksOperations(GetEngine(), GetDocID());
    GEOMImpl_I3DPrimOperations* a3DPrimOperations = new GEOMImpl_I3DPrimOperations(GetEngine(), GetDocID());
    GEOMImpl_ILocalOperations* aLocalOperations = new GEOMImpl_ILocalOperations(GetEngine(), GetDocID());
    std::vector<GEOM_IOperations*> theOperations;
    theOperations.push_back(a3DPrimOperations);
    theOperations.push_back(aBasicOperations);
    theOperations.push_back(aBlocksOperations);
    theOperations.push_back(aBooleanOperations);
    theOperations.push_back(aShapesOperations);
    theOperations.push_back(aTransformOperations);

    // BEGIN of chamfer
    TopoDS_Shape aShapeShape = aShape->GetValue();
    TopTools_IndexedMapOfShape anEdgesIndices;
    TopExp::MapShapes(aShapeShape, anEdgesIndices);
    // Common edges on external cylinders
    Handle(GEOM_Object) box_e;
    if (theHexMesh) {
      box_e = a3DPrimOperations->MakeBoxDXDYDZ(theR2+theW2, theR2+theW2, theR1+theW1);
    }
    else {
      box_e = a3DPrimOperations->MakeBoxDXDYDZ(2*(theR2+theW2), 2*(theR2+theW2), theR1+theW1);
    }
    box_e->GetLastFunction()->SetDescription("");
    box_e = aTransformOperations->TranslateDXDYDZ(box_e, -(theR2+theW2), -(theR2+theW2), 0);
    box_e->GetLastFunction()->SetDescription("");
    
    Handle(TColStd_HSequenceOfInteger) edges_e = aShapesOperations->GetShapesOnBoxIDs(box_e, aShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
    box_e->GetLastFunction()->SetDescription("");

    if (edges_e.IsNull() || edges_e->Length() == 0) {
        SetErrorCode("External edges not found");
        return false;
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
      aChamfer = aLocalOperations->MakeChamferEdges(aShape, theW, theH, theEdges);
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
        if (!MakePipeTShapePartition(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, 0, false))
          return NULL;
        if (!MakePipeTShapeMirrorAndGlue(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2))
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
    if (theHexMesh) {
        /*
         * Get the groups: BEGIN
         */
        if (!MakeGroups(/*theOperations, */aShape, TSHAPE_CHAMFER, theR1, theW1, theL1, theR2, theW2, theL2, aSeq, aTrsf))
            return NULL;

        TCollection_AsciiString aListRes, anEntry;
        // Iterate over the sequence aSeq
        Standard_Integer aNbGroups = aSeq->Length();
        Standard_Integer i = 2;
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

        //Make a Python command
        GEOM::TPythonDump(aFunction) << "[" << aShape << ", " << aListRes.ToCString()
                << "] = geompy.MakePipeTShapeChamfer(" << theR1 << ", " << theW1 << ", " << theL1 << ", " << theR2
                << ", " << theW2 << ", " << theL2 << ", " << theH << ", " << theW << ", " << theHexMesh << ", "
                << theP1 << ", " << theP2 << ", " << theP3 << ")";
    }
    /*
     * Get the groups: END
     */
    else {
        //Make a Python command
        GEOM::TPythonDump(aFunction) << "[" << aShape << "] = geompy.MakePipeTShapeChamfer(" << theR1 << ", " << theW1
                << ", " << theL1 << ", " << theR2 << ", " << theW2 << ", " << theL2 << ", " << theH << ", " << theW
                << ", " << theHexMesh << ", " << theP1 << ", " << theP2 << ", " << theP3 << ")";
    }

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
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IAdvancedOperations::MakePipeTShapeFillet(double theR1, double theW1,
        double theL1, double theR2, double theW2, double theL2, double theRF, bool theHexMesh) {
    SetErrorCode(KO);
    //Add a new object
    Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);
    //Add a new shape function with parameters
    Handle(GEOM_Function) aFunction = aShape->AddFunction(GEOMImpl_PipeTShapeDriver::GetID(), TSHAPE_FILLET);
    if (aFunction.IsNull()) return NULL;

    //Check if the function is set correctly
    if (aFunction->GetDriverGUID() != GEOMImpl_PipeTShapeDriver::GetID()) return NULL;

    GEOMImpl_IPipeTShape aData(aFunction);

    aData.SetR1(theR1);
    aData.SetW1(theW1);
    aData.SetL1(theL1);
    aData.SetR2(theR2);
    aData.SetW2(theW2);
    aData.SetL2(theL2);
    aData.SetRF(theRF);
    aData.SetHexMesh(theHexMesh);

    //Compute the resulting value
    try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
        OCC_CATCH_SIGNALS;
#endif
        if (!GetSolver()->ComputeFunction(aFunction)) {
            SetErrorCode("TShape driver failed");
            return NULL;
        }
    } catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        SetErrorCode(aFail->GetMessageString());
        return NULL;
    }

    GEOMImpl_IBasicOperations* aBasicOperations = new GEOMImpl_IBasicOperations(GetEngine(), GetDocID());
    GEOMImpl_IBooleanOperations* aBooleanOperations = new GEOMImpl_IBooleanOperations(GetEngine(), GetDocID());
    GEOMImpl_IShapesOperations* aShapesOperations = new GEOMImpl_IShapesOperations(GetEngine(), GetDocID());
    GEOMImpl_ITransformOperations* aTransformOperations = new GEOMImpl_ITransformOperations(GetEngine(), GetDocID());
    GEOMImpl_IBlocksOperations* aBlocksOperations = new GEOMImpl_IBlocksOperations(GetEngine(), GetDocID());
    GEOMImpl_I3DPrimOperations* a3DPrimOperations = new GEOMImpl_I3DPrimOperations(GetEngine(), GetDocID());
    GEOMImpl_ILocalOperations* aLocalOperations = new GEOMImpl_ILocalOperations(GetEngine(), GetDocID());
    std::vector<GEOM_IOperations*> theOperations;
    theOperations.push_back(a3DPrimOperations);
    theOperations.push_back(aBasicOperations);
    theOperations.push_back(aBlocksOperations);
    theOperations.push_back(aBooleanOperations);
    theOperations.push_back(aShapesOperations);
    theOperations.push_back(aTransformOperations);

    // BEGIN of fillet
    TopoDS_Shape aShapeShape = aShape->GetValue();
    TopTools_IndexedMapOfShape anEdgesIndices;
    TopExp::MapShapes(aShapeShape, anEdgesIndices);
    // Common edges on external cylinders
    Handle(GEOM_Object) box_e;
    if (theHexMesh) {
      box_e = a3DPrimOperations->MakeBoxDXDYDZ(theR2+theW2, theR2+theW2, theR1+theW1);
    }
    else {
      box_e = a3DPrimOperations->MakeBoxDXDYDZ(2*(theR2+theW2), 2*(theR2+theW2), theR1+theW1);
    }
    box_e->GetLastFunction()->SetDescription("");
    box_e = aTransformOperations->TranslateDXDYDZ(box_e, -(theR2+theW2), -(theR2+theW2), 0);
    box_e->GetLastFunction()->SetDescription("");
    
    Handle(TColStd_HSequenceOfInteger) edges_e = aShapesOperations->GetShapesOnBoxIDs(box_e, aShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
    box_e->GetLastFunction()->SetDescription("");

    if (edges_e.IsNull() || edges_e->Length() == 0) {
        SetErrorCode("External edges not found");
        return false;
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
    	aFillet = aLocalOperations->MakeFilletEdges(aShape, theRF, theEdges);
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
    aFunction->SetValue(aFilletShape);
    // END of fillet

    if (theHexMesh) {
        if (!MakePipeTShapePartition(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2, 0, 0, theRF, false))
          return NULL;
        if (!MakePipeTShapeMirrorAndGlue(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2))
          return NULL;
    }

    Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
    aSeq->Append(aShape);
    if (theHexMesh) {
        /*
         * Get the groups: BEGIN
         */
        if (!MakeGroups(/*theOperations, */aShape, TSHAPE_FILLET, theR1, theW1, theL1, theR2, theW2, theL2, aSeq, gp_Trsf()))
            return NULL;

        TCollection_AsciiString aListRes, anEntry;
        // Iterate over the sequence aSeq
        Standard_Integer aNbGroups = aSeq->Length();
        Standard_Integer i = 2;
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

        //Make a Python command
        GEOM::TPythonDump(aFunction) << "[" << aShape << ", " << aListRes.ToCString()
                << "] = geompy.MakePipeTShapeFillet(" << theR1 << ", " << theW1 << ", " << theL1 << ", " << theR2
                << ", " << theW2 << ", " << theL2 << ", " << theRF << ", " << theHexMesh << ")";
    }
    /*
     * Get the groups: END
     */
    else {
        //Make a Python command
        GEOM::TPythonDump(aFunction) << "[" << aShape << "] = geompy.MakePipeTShapeFillet(" << theR1 << ", " << theW1
                << ", " << theL1 << ", " << theR2 << ", " << theW2 << ", " << theL2 << ", " << theRF << ", "
                << theHexMesh << ")";
    }


    SetErrorCode(OK);

    return aSeq;

}

//=============================================================================
/*!
 *  MakePipeTShapeFilletWithPosition
 *  Create a T-shape object with specified caracteristics for the main and
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
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IAdvancedOperations::MakePipeTShapeFilletWithPosition(double theR1,
        double theW1, double theL1, double theR2, double theW2, double theL2, double theRF, bool theHexMesh,
        Handle(GEOM_Object) theP1, Handle(GEOM_Object) theP2, Handle(GEOM_Object) theP3) {
    SetErrorCode(KO);
    //Add a new object
    Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_TSHAPE);
    //Add a new shape function with parameters
    Handle(GEOM_Function) aFunction = aShape->AddFunction(GEOMImpl_PipeTShapeDriver::GetID(), TSHAPE_FILLET);
    if (aFunction.IsNull()) return NULL;

    //Check if the function is set correctly
    if (aFunction->GetDriverGUID() != GEOMImpl_PipeTShapeDriver::GetID()) return NULL;

    // Check new position
    if (!CheckCompatiblePosition(theL1, theL2, theP1, theP2, theP3, 0.01)) {
        return NULL;
    }

    GEOMImpl_IPipeTShape aData(aFunction);

    aData.SetR1(theR1);
    aData.SetW1(theW1);
    aData.SetL1(theL1);
    aData.SetR2(theR2);
    aData.SetW2(theW2);
    aData.SetL2(theL2);
    aData.SetRF(theRF);
    aData.SetHexMesh(theHexMesh);

    //Compute the resulting value
    try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
        OCC_CATCH_SIGNALS;
#endif
        if (!GetSolver()->ComputeFunction(aFunction)) {
            SetErrorCode("TShape driver failed");
            return NULL;
        }
    } catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        SetErrorCode(aFail->GetMessageString());
        return NULL;
    }

    GEOMImpl_IBasicOperations* aBasicOperations = new GEOMImpl_IBasicOperations(GetEngine(), GetDocID());
    GEOMImpl_IBooleanOperations* aBooleanOperations = new GEOMImpl_IBooleanOperations(GetEngine(), GetDocID());
    GEOMImpl_IShapesOperations* aShapesOperations = new GEOMImpl_IShapesOperations(GetEngine(), GetDocID());
    GEOMImpl_ITransformOperations* aTransformOperations = new GEOMImpl_ITransformOperations(GetEngine(), GetDocID());
    GEOMImpl_IBlocksOperations* aBlocksOperations = new GEOMImpl_IBlocksOperations(GetEngine(), GetDocID());
    GEOMImpl_I3DPrimOperations* a3DPrimOperations = new GEOMImpl_I3DPrimOperations(GetEngine(), GetDocID());
    GEOMImpl_ILocalOperations* aLocalOperations = new GEOMImpl_ILocalOperations(GetEngine(), GetDocID());
    std::vector<GEOM_IOperations*> theOperations;
    theOperations.push_back(a3DPrimOperations);
    theOperations.push_back(aBasicOperations);
    theOperations.push_back(aBlocksOperations);
    theOperations.push_back(aBooleanOperations);
    theOperations.push_back(aShapesOperations);
    theOperations.push_back(aTransformOperations);

    // BEGIN of fillet
    TopoDS_Shape aShapeShape = aShape->GetValue();
    TopTools_IndexedMapOfShape anEdgesIndices;
    TopExp::MapShapes(aShapeShape, anEdgesIndices);
    // Common edges on external cylinders
    Handle(GEOM_Object) box_e;
    if (theHexMesh) {
      box_e = a3DPrimOperations->MakeBoxDXDYDZ(theR2+theW2, theR2+theW2, theR1+theW1);
    }
    else {
      box_e = a3DPrimOperations->MakeBoxDXDYDZ(2*(theR2+theW2), 2*(theR2+theW2), theR1+theW1);
    }
    box_e->GetLastFunction()->SetDescription("");
    box_e = aTransformOperations->TranslateDXDYDZ(box_e, -(theR2+theW2), -(theR2+theW2), 0);
    box_e->GetLastFunction()->SetDescription("");
    
    Handle(TColStd_HSequenceOfInteger) edges_e = aShapesOperations->GetShapesOnBoxIDs(box_e, aShape, TopAbs_EDGE, GEOMAlgo_ST_IN);
    box_e->GetLastFunction()->SetDescription("");

    if (edges_e.IsNull() || edges_e->Length() == 0) {
        SetErrorCode("External edges not found");
        return false;
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
    	aFillet = aLocalOperations->MakeFilletEdges(aShape, theRF, theEdges);
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
    aFunction->SetValue(aFilletShape);
    // END of fillet

    if (theHexMesh) {
        if (!MakePipeTShapePartition(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2, 0, 0, theRF, false))
          return NULL;
        if (!MakePipeTShapeMirrorAndGlue(/*theOperations, */aShape, theR1, theW1, theL1, theR2, theW2, theL2))
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
    if (theHexMesh) {
        /*
         * Get the groups: BEGIN
         */
        if (!MakeGroups(/*theOperations, */aShape, TSHAPE_FILLET, theR1, theW1, theL1, theR2, theW2, theL2, aSeq, aTrsf))
            return NULL;

        TCollection_AsciiString aListRes, anEntry;
        // Iterate over the sequence aSeq
        Standard_Integer aNbGroups = aSeq->Length();
        Standard_Integer i = 2;
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

        //Make a Python command
        GEOM::TPythonDump(aFunction) << "[" << aShape << ", " << aListRes.ToCString()
                << "] = geompy.MakePipeTShapeFillet(" << theR1 << ", " << theW1 << ", " << theL1 << ", " << theR2
                << ", " << theW2 << ", " << theL2 << ", " << theRF << ", " << theHexMesh << ", " << theP1 << ", "
                << theP2 << ", " << theP3 << ")";
    }
    /*
     * Get the groups: END
     */
    else {
        //Make a Python command
        GEOM::TPythonDump(aFunction) << "[" << aShape << "] = geompy.MakePipeTShapeFillet(" << theR1 << ", " << theW1
                << ", " << theL1 << ", " << theR2 << ", " << theW2 << ", " << theL2 << ", " << theRF << ", "
                << theHexMesh << ", " << theP1 << ", " << theP2 << ", " << theP3 << ")";
    }

    SetErrorCode(OK);

    return aSeq;
}

/*@@ insert new functions before this line @@*/
