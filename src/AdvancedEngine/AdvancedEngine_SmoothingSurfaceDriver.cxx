// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "AdvancedEngine_SmoothingSurfaceDriver.hxx"
#include "AdvancedEngine_ISmoothingSurface.hxx"
#include "AdvancedEngine_Types.hxx"

#include "GEOM_Function.hxx"

#include <StdFail_NotDone.hxx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopExp_Explorer.hxx>

#include <TColgp_SequenceOfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_SequenceOfXY.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <BRepAdaptor_HSurface.hxx>

#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAlgoAPI_Cut.hxx>

#include <GeomPlate_Surface.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PlateG0Criterion.hxx>
#include <GeomPlate_BuildAveragePlane.hxx>

#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>

#include <GProp_GProps.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#include <GC_MakePlane.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& AdvancedEngine_SmoothingSurfaceDriver::GetID()
{
  static Standard_GUID aGUID("1C3A0F30-729D-4E83-8232-78E74FC5637C");
  return aGUID;
}

//=======================================================================
//function : AdvancedEngine_SmoothingSurfaceDriver
//purpose  :
//=======================================================================
AdvancedEngine_SmoothingSurfaceDriver::AdvancedEngine_SmoothingSurfaceDriver()
{
}

//=======================================================================
//function : MakeSmoothingSurfaceUnClosed
//purpose  :
//=======================================================================
TopoDS_Shape AdvancedEngine_SmoothingSurfaceDriver::MakeSmoothingSurfaceUnClosed
                          (const Handle(TColgp_HArray1OfPnt) &theListOfPoints,
                           const Standard_Integer            theNbMax,
                           const Standard_Integer            theDegMax,
                           const Standard_Real               theDMax) const
{
  TopoDS_Face aInitShape;
  
  // Create an average Plane
  //Handle(TColgp_HArray1OfPnt) HAP = new TColgp_HArray1OfPnt(1,theListOfPoints.Length())
  GeomPlate_BuildAveragePlane gpbap(theListOfPoints,theListOfPoints->Length(),Precision::Confusion(),1,1);
  Handle(Geom_Plane) plane(gpbap.Plane());
  Standard_Real Umin, Umax, Vmin, Vmax;
  gpbap.MinMaxBox(Umin,Umax,Vmin,Vmax);
  // cout << "Vals : " << Umin << ", " << Umax << ", " << Vmin << ", " << Vmax << endl;
  BRepBuilderAPI_MakeFace mf(plane,Umin,Umax,Vmin,Vmax,Precision::Confusion());
  aInitShape =  mf.Face();
  //return aInitShape;

  GeomPlate_BuildPlateSurface aBuilder(3,10);
  // ** Initialization of surface
  Handle(BRepAdaptor_HSurface) HSI = new BRepAdaptor_HSurface();
  HSI->ChangeSurface().Initialize(aInitShape);
  aBuilder.LoadInitSurface( BRep_Tool::Surface(HSI->ChangeSurface().Face()));

  Standard_Integer j, j1, j2;
  // cout << "Init surface" << endl;
  j1 = theListOfPoints->Lower();
  j2 = theListOfPoints->Upper();
  for (j=j1; j<=j2 ; j++)
  {
    gp_Pnt aPnt = theListOfPoints->Value(j); 
    Handle(GeomPlate_PointConstraint) PCont = new GeomPlate_PointConstraint(aPnt,0);
    aBuilder.Add(PCont);
  }
  // cout << "avant Perform surface" << endl;
  aBuilder.Perform();
  // cout << "Perform surface" << endl;

  // A ce niveau : surface algo
  Handle(GeomPlate_Surface) gpPlate = aBuilder.Surface();
  
  Standard_Integer nbcarreau = (theNbMax > 0 ? theNbMax : 2);
  Standard_Integer degmax = (theDegMax > 0 ? theDegMax : 8);
  Standard_Real seuil = theDMax;

  if (seuil <= 0.) {
    seuil = Max(0.0001,10*aBuilder.G0Error());
  }

  GeomPlate_MakeApprox Mapp(gpPlate,0.0001,nbcarreau,degmax,seuil);
  // cout << "Approx surface" << endl;

  Handle (Geom_Surface) Surf (Mapp.Surface());
 
  aBuilder.Surface()->Bounds( Umin, Umax, Vmin, Vmax);
  
  BRepBuilderAPI_MakeFace MF(Surf,Umin, Umax, Vmin, Vmax, Precision::Confusion());
  TopoDS_Shape aShape = MF.Shape();
  
  return aShape;
}


//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer AdvancedEngine_SmoothingSurfaceDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  if (aFunction.IsNull()) return 0;

  Standard_Integer aType = aFunction->GetType();

  if (aType != SMOOTHINGSURFACE_LPOINTS) return 0;
  // cout << "Youhou : " << aType << endl;

  AdvancedEngine_ISmoothingSurface aData (aFunction);

  // Fill the map of vertices.
  Standard_Integer    aNbShapes = aData.GetLength();
  TopTools_MapOfShape aMapPoints;
  Standard_Integer    i;

  for (i = 1; i <= aNbShapes; i++) {
    Handle(GEOM_Function) aFShape = aData.GetPntOrComp(i);
    TopoDS_Shape aShape = aFShape->GetValue();

    if (aShape.ShapeType() == TopAbs_VERTEX) {
      aMapPoints.Add(aShape);
    } else {
      TopExp_Explorer anExp(aShape, TopAbs_VERTEX);

      for (; anExp.More(); anExp.Next()) {
        aMapPoints.Add(anExp.Current());
      }
    }
  }

  // Add points to the array of points.
  const Standard_Integer      aNbPoints    = aMapPoints.Extent();
  Handle(TColgp_HArray1OfPnt) anArrayofPnt =
      new TColgp_HArray1OfPnt(1, aNbPoints);
  TopTools_MapIteratorOfMapOfShape anIter(aMapPoints);

  for (i = 1; anIter.More(); anIter.Next(), i++) {
    TopoDS_Vertex aPoint = TopoDS::Vertex(anIter.Key());
    gp_Pnt aPnt = BRep_Tool::Pnt(aPoint);

    anArrayofPnt->SetValue(i, aPnt);
  }

  const Standard_Integer aNbMax  = aData.GetNbMax();
  const Standard_Integer aDegMax = aData.GetDegMax();
  const Standard_Real    aDMax   = aData.GetDMax();

  // Make smoothing surface.
  TopoDS_Shape aShape = AdvancedEngine_SmoothingSurfaceDriver::
    MakeSmoothingSurfaceUnClosed(anArrayofPnt, aNbMax, aDegMax, aDMax);

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

bool AdvancedEngine_SmoothingSurfaceDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  AdvancedEngine_ISmoothingSurface aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "SMOOTHINGSURFACE";

  switch ( aType ) {
  case SMOOTHINGSURFACE_LPOINTS:
    {
      AddParam( theParams, "Points" );
      if ( aCI.GetLength() > 1 )
        theParams[0] << aCI.GetLength() << " points: ";
      for ( int i = 1, nb = aCI.GetLength(); i <= nb; ++i )
        theParams[0] << aCI.GetPntOrComp( i ) << " ";

      const Standard_Integer aNbMax  = aCI.GetNbMax();
      const Standard_Integer aDegMax = aCI.GetDegMax();
      const Standard_Real    aDMax   = aCI.GetDMax();

      AddParam(theParams, "Max nbr of Bezier pieces", aCI.GetNbMax());
      AddParam(theParams, "Max BSpline surface degree", aCI.GetDegMax());
      AddParam(theParams, "3D tolerance of initial approximation", aCI.GetDMax());
    }
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (AdvancedEngine_SmoothingSurfaceDriver,GEOM_BaseDriver);
