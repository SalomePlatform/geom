//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include <Standard_Stream.hxx>

#include <GEOMImpl_SmoothingSurfaceDriver.hxx>
#include <GEOMImpl_ISmoothingSurface.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <TFunction_Logbook.hxx>
#include <StdFail_NotDone.hxx>

//@@ include required header files here @@//
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS.hxx>

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
const Standard_GUID& GEOMImpl_SmoothingSurfaceDriver::GetID()
{
  static Standard_GUID aGUID("1C3A0F30-729D-4E83-8232-78E74FC5637C");
  return aGUID;
}

//=======================================================================
//function : GEOMImpl_SmoothingSurfaceDriver
//purpose  :
//=======================================================================
GEOMImpl_SmoothingSurfaceDriver::GEOMImpl_SmoothingSurfaceDriver()
{
}

//=======================================================================
//function : MakeSmoothingSurfaceUnClosed
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_SmoothingSurfaceDriver::MakeSmoothingSurfaceUnClosed(Handle_TColgp_HArray1OfPnt myListOfPoints) const
{
  TopoDS_Face aInitShape;
  
  // Create an average Plane
  //Handle(TColgp_HArray1OfPnt) HAP = new TColgp_HArray1OfPnt(1,myListOfPoints.Length())
  GeomPlate_BuildAveragePlane gpbap(myListOfPoints,myListOfPoints->Length(),Precision::Confusion(),1,1);
  Handle(Geom_Plane) plane(gpbap.Plane());
  Standard_Real Umin, Umax, Vmin, Vmax;
  gpbap.MinMaxBox(Umin,Umax,Vmin,Vmax);
  cout << "Vals : " << Umin << ", " << Umax << ", " << Vmin << ", " << Vmax << endl;
  BRepBuilderAPI_MakeFace mf(plane,Umin,Umax,Vmin,Vmax,Precision::Confusion());
  aInitShape =  mf.Face();
  //return aInitShape;

  GeomPlate_BuildPlateSurface aBuilder(3,10);
  // ** Initialization of surface
  Handle(BRepAdaptor_HSurface) HSI = new BRepAdaptor_HSurface();
  HSI->ChangeSurface().Initialize(aInitShape);
  aBuilder.LoadInitSurface( BRep_Tool::Surface(HSI->ChangeSurface().Face()));

  Standard_Integer j, j1, j2;
  cout << "Init surface" << endl;
  j1 = myListOfPoints->Lower();
  j2 = myListOfPoints->Upper();
  for (j=j1; j<=j2 ; j++)
  {
    gp_Pnt aPnt = myListOfPoints->Value(j); 
    Handle(GeomPlate_PointConstraint) PCont = new GeomPlate_PointConstraint(aPnt,0);
    aBuilder.Add(PCont);
  }
  cout << "avant Perform surface" << endl;
  aBuilder.Perform();
  cout << "Perform surface" << endl;

  // A ce niveau : surface algo
  Handle(GeomPlate_Surface) gpPlate = aBuilder.Surface();
  
  Standard_Integer nbcarreau=2;
  Standard_Integer degmax=8;
  Standard_Real seuil;
  seuil = Max(0.0001,10*aBuilder.G0Error());
  GeomPlate_MakeApprox Mapp(gpPlate,0.0001,nbcarreau,degmax,seuil);
  cout << "Approx surface" << endl;

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
Standard_Integer GEOMImpl_SmoothingSurfaceDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  if (aFunction.IsNull()) return 0;

  Standard_Integer aType = aFunction->GetType();

  if (aType != SMOOTHINGSURFACE_LPOINTS) return 0;
  cout << "Youhou : " << aType << endl;

  GEOMImpl_ISmoothingSurface aData (aFunction);
  
  Standard_Integer nbPoints = aData.GetLength();

  Handle(TColgp_HArray1OfPnt) anArrayofPnt = new TColgp_HArray1OfPnt(1,nbPoints);
  for (int ind=1;ind<=nbPoints;ind++)
  {
    Handle(GEOM_Function) aPoint = aData.GetPoint(ind);
    TopoDS_Shape aShapePnt = aPoint->GetValue();
    TopoDS_Vertex dsPoint;
    dsPoint = TopoDS::Vertex( aShapePnt );
    gp_Pnt aPnt = BRep_Tool::Pnt( dsPoint );
    anArrayofPnt->SetValue(ind,aPnt);
  }

  TopoDS_Shape aShape;
  aShape = GEOMImpl_SmoothingSurfaceDriver::MakeSmoothingSurfaceUnClosed(anArrayofPnt);

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}

//=======================================================================
//function :  GEOMImpl_SmoothingSurfaceDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_SmoothingSurfaceDriver_Type_()
{
  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_SmoothingSurfaceDriver",
                                                         sizeof(GEOMImpl_SmoothingSurfaceDriver),
                                                         1,
                                                         (Standard_Address)_Ancestors,
                                                         (Standard_Address)NULL);
  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_SmoothingSurfaceDriver) Handle(GEOMImpl_SmoothingSurfaceDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_SmoothingSurfaceDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_SmoothingSurfaceDriver))) {
       _anOtherObject = Handle(GEOMImpl_SmoothingSurfaceDriver)((Handle(GEOMImpl_SmoothingSurfaceDriver)&)AnObject);
     }
  }

  return _anOtherObject;
}
