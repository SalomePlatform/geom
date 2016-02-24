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

#include "AdvancedEngine_DividedDiskDriver.hxx"
#include "AdvancedEngine_IDividedDisk.hxx"
#include "AdvancedEngine_Types.hxx"

#include "GEOM_Function.hxx"

// OCCT includes
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <Geom_Plane.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>

#include <StdFail_NotDone.hxx>

#include <TopExp.hxx>

#include <utilities.h>

enum
{
  SQUARE,
  HEXAGON
};

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& AdvancedEngine_DividedDiskDriver::GetID()
{
  static Standard_GUID aGUID("0b01da9a-c5da-11e1-8d80-78e7d1879630");
  return aGUID;
}

//=======================================================================
//function : AdvancedEngine_DividedDiskDriver
//purpose  :
//=======================================================================
AdvancedEngine_DividedDiskDriver::AdvancedEngine_DividedDiskDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer AdvancedEngine_DividedDiskDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  AdvancedEngine_IDividedDisk aData (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  // Getting data
  double R       = aData.GetR();
  double Ratio   = aData.GetRatio();
  int    Pattern = aData.GetType();
  
  // Build reference disk (in the global coordinate system)
  TopoDS_Shape aDisk;
  
  if (Pattern == SQUARE)
    aDisk = MakeDiskSquare( R, Ratio );
  else if (Pattern == HEXAGON)
    aDisk = MakeDiskHexagon( R, Ratio );
  
  if (aType == DIVIDEDDISK_R_RATIO) 
  { 
    int theOrientation = aData.GetOrientation();        
    aShape = TransformShape(aDisk, theOrientation);   
  }
  else if (aType == DIVIDEDDISK_R_VECTOR_PNT)
  {
    Handle(GEOM_Function) aRefPoint  = aData.GetCenter();
    Handle(GEOM_Function) aRefVector = aData.GetVector();
    TopoDS_Shape aShapePnt = aRefPoint->GetValue();
    TopoDS_Shape aShapeVec = aRefVector->GetValue();
    
    if (aShapePnt.ShapeType() == TopAbs_VERTEX &&
        aShapeVec.ShapeType() == TopAbs_EDGE) 
    {
      gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt));
      TopoDS_Edge anE = TopoDS::Edge(aShapeVec);
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(anE, V1, V2, Standard_True);
      if (!V1.IsNull() && !V2.IsNull()) 
      {
        gp_Vec aVec (BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
        gp_Dir aDir(aVec);
        aShape = TransformShape(aDisk, aPnt, aDir); 
      }
    }   
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  return 1;
}


//=======================================================================
//function : MakeDiskHexagon
//purpose  :
//=======================================================================
TopoDS_Shell AdvancedEngine_DividedDiskDriver::MakeDiskHexagon(double R, double Ratio) const
{
  // Geometry
  gp_Dir ZDir(0,0,1);
  gp_Dir XDir(1,0,0);
  gp_Pnt Orig(0,0,0);
  
  // Circle
  gp_Ax1 Ax1(Orig,ZDir);
  gp_Ax2 Ax(Orig,ZDir,XDir);
  gp_Circ aCircle(Ax, R);
  
  // Points
//   gp_Pnt P4(0.01*Ratio*R,0,0); 
//   gp_Pnt P3(R,0,0);
//   gp_Pnt P2 = P3.Rotated(Ax1,-M_PI/6.0);
//   gp_Pnt P1(P4.X(), 
//             P4.X()/sqrt(3.0),0);
  gp_Pnt P1(0.01*Ratio*R*sqrt(3.0)/2,0,0);
  gp_Pnt P2(R,0,0);
  gp_Pnt P3 = P2.Rotated(Ax1,M_PI/6.0);
  gp_Pnt P4(P1.X(), 
            P1.X()/sqrt(3.0),0);

  
  //surfaces
  gp_Ax2 anAx (gp::XOY());
  Handle(Geom_Plane) aPlane = new Geom_Plane (anAx);
  
  // Topology
  
  // Vertices
  TopoDS_Vertex O  = BRepBuilderAPI_MakeVertex(Orig);
  TopoDS_Vertex V1_init = BRepBuilderAPI_MakeVertex(P1);
  TopoDS_Vertex V2_init = BRepBuilderAPI_MakeVertex(P2);
  TopoDS_Vertex V3 = BRepBuilderAPI_MakeVertex(P3);
  TopoDS_Vertex V4 = BRepBuilderAPI_MakeVertex(P4);
  
  TopoDS_Vertex V1 = V1_init;
  TopoDS_Vertex V2 = V2_init;
  
  //Rotation
  gp_Trsf myTrsf;
  myTrsf.SetRotation(Ax1, M_PI/3.0);
  
  BRepBuilderAPI_Transform xform(myTrsf);
  xform.Perform(V1,Standard_True);
  TopoDS_Vertex V1_60 = TopoDS::Vertex(xform.Shape()); 
  xform.Perform(V2,Standard_True);
  TopoDS_Vertex V2_60 = TopoDS::Vertex(xform.Shape());
  
  // Declaration of shapes (used in the loop) 
  TopoDS_Edge E1, E2, E3, E4, E5, E6, E7, E8, E9;
  TopoDS_Wire W1, W2, W3;
  TopoDS_Face F1, F2, F3;   
  TopoDS_Shell S;
  
  BRep_Builder aBuilder;
  aBuilder.MakeShell(S);
  
  // Initialisation of edges
  TopoDS_Edge E1_init = BRepBuilderAPI_MakeEdge(V1,TopoDS::Vertex(V2.Reversed()));
  E1 = E1_init;
  TopoDS_Edge E8_init = BRepBuilderAPI_MakeEdge(O,TopoDS::Vertex(V1.Reversed()));
  E8 = E8_init;
  
  for (int i=1;i<=6;i++)
  { 
    // Edges
    
    // for Face1
    E2 = BRepBuilderAPI_MakeEdge(aCircle, V2, TopoDS::Vertex(V3.Reversed())); 
    E3 = BRepBuilderAPI_MakeEdge(V3,TopoDS::Vertex(V4.Reversed()));
    E4 = BRepBuilderAPI_MakeEdge(V4,TopoDS::Vertex(V1.Reversed()));
      
    // for Face2
    if (i==6)
    {
      E5 = BRepBuilderAPI_MakeEdge(aCircle, V3, TopoDS::Vertex(V2_init.Reversed()));
      E7 = BRepBuilderAPI_MakeEdge(V1_init,TopoDS::Vertex(V4.Reversed()));
    }
    else
    {
      E5 = BRepBuilderAPI_MakeEdge(aCircle, V3, TopoDS::Vertex(V2_60.Reversed()));
      E7 = BRepBuilderAPI_MakeEdge(V1_60,TopoDS::Vertex(V4.Reversed()));
    }    
    E6 = BRepBuilderAPI_MakeEdge(V2_60,TopoDS::Vertex(V1_60.Reversed()));
    
    // for Face3
    E9 = BRepBuilderAPI_MakeEdge(V1_60,TopoDS::Vertex(O.Reversed()));
    
    
    // Wires
    
    //Wire1
    aBuilder.MakeWire(W1);
    if (i==1)
      aBuilder.Add(W1,E1);
    else
      aBuilder.Add(W1,TopoDS::Edge(E1.Reversed()));
    aBuilder.Add(W1,E2);
    aBuilder.Add(W1,E3);
    aBuilder.Add(W1,E4);
    
    // Wire 2
    aBuilder.MakeWire(W2);
    aBuilder.Add(W2,TopoDS::Edge(E3.Reversed()));
    aBuilder.Add(W2,E5);
    if (i==6)
      aBuilder.Add(W2,TopoDS::Edge(E1_init.Reversed()));
    else
      aBuilder.Add(W2,E6);
    aBuilder.Add(W2,E7);
    
    // Wire3
    aBuilder.MakeWire(W3);
    if (i==1)
      aBuilder.Add(W3,E8);
    else 
      aBuilder.Add(W3,TopoDS::Edge(E8.Reversed()));    
    aBuilder.Add(W3,TopoDS::Edge(E4.Reversed()));
    aBuilder.Add(W3,TopoDS::Edge(E7.Reversed()));
    if (i==6)
      aBuilder.Add(W3,TopoDS::Edge(E8_init.Reversed()));
    else
      aBuilder.Add(W3,E9);
      
    // Faces creation
    F1 = BRepBuilderAPI_MakeFace(aPlane,W1);
    F2 = BRepBuilderAPI_MakeFace(aPlane,W2);
    F3 = BRepBuilderAPI_MakeFace(aPlane,W3);
    
    //Shell
    aBuilder.Add(S, F1);
    aBuilder.Add(S, F2);
    aBuilder.Add(S, F3);
          
    // rotation
    V1=V1_60;
    V2=V2_60;
    
    xform.Perform(V1_60,Standard_True);
    V1_60 = TopoDS::Vertex(xform.Shape());
    xform.Perform(V2_60,Standard_True);
    V2_60 = TopoDS::Vertex(xform.Shape());
    xform.Perform(V3,Standard_True);
    V3    = TopoDS::Vertex(xform.Shape());
    xform.Perform(V4,Standard_True);
    V4    = TopoDS::Vertex(xform.Shape());
    
    // "Increment" of edges
    E1=E6;
    E8=E9;         
  }
  
  return S;
}

//=======================================================================
//function : MakeDiskSquare
//purpose  :
//=======================================================================
TopoDS_Shape AdvancedEngine_DividedDiskDriver::MakeDiskSquare(double R, double Ratio) const
{
  // Geometry
  gp_Dir ZDir(0,0,1);
  gp_Dir XDir(1,0,0);
  gp_Pnt Orig(0,0,0);
  
  // Circle
  gp_Ax1 Ax1(Orig,ZDir);
  gp_Ax2 Ax(Orig,ZDir,XDir);
  gp_Circ aCircle(Ax, R);
  
  // Points
  gp_Pnt P1(0.01*Ratio*R,0,0);
  gp_Pnt P2(R,0,0);
  
  //surfaces
  gp_Ax2 anAx (gp::XOY());
  Handle(Geom_Plane) aPlane = new Geom_Plane (anAx);
  
  // Topology
  
  // Vertices
  TopoDS_Vertex V1_init = BRepBuilderAPI_MakeVertex(P1);
  TopoDS_Vertex V2_init = BRepBuilderAPI_MakeVertex(P2);
  
  TopoDS_Vertex V1 = V1_init;
  TopoDS_Vertex V2 = V2_init;
  
  //Rotation
  gp_Trsf myTrsf;
  myTrsf.SetRotation(Ax1, M_PI/2);
  
  BRepBuilderAPI_Transform xform(myTrsf);
  xform.Perform(V1,Standard_True);
  TopoDS_Vertex V1_rotated = TopoDS::Vertex(xform.Shape()); 
  xform.Perform(V2,Standard_True);
  TopoDS_Vertex V2_rotated = TopoDS::Vertex(xform.Shape());
  
  // Declaration of shapes (used in the loop) 
  TopoDS_Edge E1, E2, E3, E4;
  TopoDS_Wire W1, W2;
  TopoDS_Face F1, F2;   
  TopoDS_Shell S;
  
  BRep_Builder aBuilder;
  aBuilder.MakeWire(W2);  // Central Wire
  aBuilder.MakeShell(S);  // Shell
  
  // Initialisation of edges
  TopoDS_Edge E1_init = BRepBuilderAPI_MakeEdge(V1,TopoDS::Vertex(V2.Reversed()));
  E1 = E1_init;
  
  for (int i=1;i<=4;i++)
  { 
    // Edges
    // for Face1
   
    E3 = BRepBuilderAPI_MakeEdge(V2_rotated,TopoDS::Vertex(V1_rotated.Reversed()));
    if (i == 4)
    {
      E2 = BRepBuilderAPI_MakeEdge(aCircle, V2, TopoDS::Vertex(V2_init.Reversed())); 
      E4 = BRepBuilderAPI_MakeEdge(V1_init,TopoDS::Vertex(V1.Reversed()));
    }
    else
    {
      E2 = BRepBuilderAPI_MakeEdge(aCircle, V2, TopoDS::Vertex(V2_rotated.Reversed())); 
      E4 = BRepBuilderAPI_MakeEdge(V1_rotated,TopoDS::Vertex(V1.Reversed()));
    }
    
    // Wires
    //Wire1
    aBuilder.MakeWire(W1);
    if (i==1)
      aBuilder.Add(W1,E1);
    else
      aBuilder.Add(W1,TopoDS::Edge(E1.Reversed()));
    aBuilder.Add(W1,E2);
    if (i==4)
      aBuilder.Add(W1,TopoDS::Edge(E1_init.Reversed()));
    else
      aBuilder.Add(W1,E3);
    aBuilder.Add(W1,E4);
    
    // Wire central
    aBuilder.Add(W2,TopoDS::Edge(E4.Reversed()));
    
    // Faces creation
    F1 = BRepBuilderAPI_MakeFace(aPlane,W1);
    
    //Shell
    aBuilder.Add(S, F1);
    
    // rotation
    V1=V1_rotated;
    V2=V2_rotated;
    
    xform.Perform(V1_rotated,Standard_True);
    V1_rotated = TopoDS::Vertex(xform.Shape());
    xform.Perform(V2_rotated,Standard_True);
    V2_rotated = TopoDS::Vertex(xform.Shape());
    
    // "Increment" of edges
    E1=E3;        
  }
  // Central square Face 
  F2 = BRepBuilderAPI_MakeFace(aPlane,W2);
  aBuilder.Add(S, F2);
  
  return S;
}


//=======================================================================
//function :  TrasformShape(TopoDS_Shape aShape,int theOrientation)
//purpose  :  Perform shape transformation accordingly with specified
//            orientation
//=======================================================================
TopoDS_Shape AdvancedEngine_DividedDiskDriver::TransformShape(TopoDS_Shape theShape, int theOrientation) const
{
  gp_Dir N, Vx;
  gp_Pnt theOrigin = gp::Origin();
  
  switch(theOrientation)
  {
    case 1:
    {
      N = gp::DZ();
      Vx = gp::DX();
      break;
    }
    case 2:
    {
      N = gp::DX();
      Vx = gp::DY();
      break;
    }
    case 3:
    {
      N = gp::DY();
      Vx = gp::DZ();
      break;
    }
  }
    
  gp_Ax3 aWPlane = gp_Ax3(theOrigin, N, Vx);
  
  return WPlaneTransform(theShape, aWPlane);
}

//=======================================================================
//function :  TrasformShape(TopoDS_Shape aShape, gp_Dir V, gp_Pnt P)
//purpose  :  Perform shape transformation accordingly with specified
//            pnt and direction
//=======================================================================
TopoDS_Shape AdvancedEngine_DividedDiskDriver::TransformShape(TopoDS_Shape theShape, gp_Pnt P, gp_Dir V) const
{
  gp_Ax3 aWPlane( P, V );
  return WPlaneTransform(theShape, aWPlane);
}

//=======================================================================
//function :  WPlaneTransform
//purpose  :  Perform shape transformation accordingly with the given 
//            Working Plane  
//=======================================================================
TopoDS_Shape AdvancedEngine_DividedDiskDriver::WPlaneTransform(TopoDS_Shape theShape, gp_Ax3 theWPlane) const
{
  gp_Trsf aTrans;
  aTrans.SetTransformation(theWPlane);
  aTrans.Invert();
  BRepBuilderAPI_Transform aTransformation (theShape, aTrans, Standard_False);
  return aTransformation.Shape();
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool AdvancedEngine_DividedDiskDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  AdvancedEngine_IDividedDisk aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "DIVIDEDDISK";

  switch ( aType ) {
  case DIVIDEDDISK_R_RATIO:
    AddParam( theParams, "Radius", aCI.GetR() );
    AddParam( theParams, "Ratio", aCI.GetRatio() );
    AddParam( theParams, "Orientation", aCI.GetOrientation() );
    AddParam( theParams, "Division pattern", aCI.GetType() );
    break;
  case DIVIDEDDISK_R_VECTOR_PNT:
    AddParam( theParams, "Center Point", aCI.GetCenter() );
    AddParam( theParams, "Vector", aCI.GetVector() );
    AddParam( theParams, "Radius", aCI.GetR() );
    AddParam( theParams, "Division pattern", aCI.GetType() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (AdvancedEngine_DividedDiskDriver,GEOM_BaseDriver);
