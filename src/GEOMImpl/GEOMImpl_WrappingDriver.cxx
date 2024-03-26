// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

#include <GEOMImpl_WrappingDriver.hxx>

#include <GEOMAlgo_Splitter.hxx>
#include <GEOMUtils.hxx>

#include <GEOMImpl_ShapeDriver.hxx>
#include <GEOMImpl_IWrap.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_Block6Explorer.hxx>
#include <GEOM_Object.hxx>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#if OCC_VERSION_LARGE < 0x07070000
#include <BRepAdaptor_HSurface.hxx>
#else
#include <BRepAdaptor_Surface.hxx>
#endif

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomPlate_Surface.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_BuildAveragePlane.hxx>

#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Vertex.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <ShapeFix_Face.hxx>

#include <Standard_NullObject.hxx>

#include <numeric>
#include <vector>

#include <Eigen/Dense>

//=======================================================================
// function : GetID
// purpose  :
//=======================================================================
const Standard_GUID &GEOMImpl_WrappingDriver::GetID()
{
  static Standard_GUID aWrapDriver("FF1BBB71-729D-4E83-8232-78E74FC5637C");
  return aWrapDriver;
}

//=======================================================================
// function : GEOMImpl_WrappingDriver
// purpose  :
//=======================================================================
GEOMImpl_WrappingDriver::GEOMImpl_WrappingDriver()
{
}

//=======================================================================
// function : createPointsOnEdges
// purpose  : create points on edges 
//=======================================================================
static void createPointsOnEdges(const Handle(TColStd_HSequenceOfTransient) & theEdgesFuncs,
                                std::vector<gp_Pnt> &thePoints)
{
  for (int i = 1; i <= theEdgesFuncs->Length(); i++)
  {
    Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(theEdgesFuncs->Value(i));
    TopoDS_Shape aShape_i = aRefShape->GetValue();
    if (aShape_i.IsNull())
    {
      Standard_NullObject::Raise("Edge for face construction is null");
    }
    TopExp_Explorer exp(aShape_i, TopAbs_EDGE);
    for (; exp.More(); exp.Next())
    {
      Standard_Real aFP, aLP, aP;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(exp.Current()), aFP, aLP);
      if (!aCurve.IsNull())
      {
        for (int aPar = 1; aPar <= 9; aPar++)
        {
          aP = aFP + (aLP - aFP) * (aPar * 0.1);
          gp_Pnt aPnt = aCurve->Value(aP);
          thePoints.push_back(aPnt);
        }
      }
    }
  }
}

//================================================================================
// function : maxDistanceToFace
// purpose  : finds max distanse between points and a face
//================================================================================
static Standard_Real maxDistanceToFace(const std::vector<gp_Pnt>& thePoints,
                                       const TopoDS_Face& theFace,
                                       const Standard_Real theTolerance)
{
  Standard_Real U, V;
  Standard_Real aMaxDist = 0.;
  for(auto& aPnt : thePoints)
  {
    gp_Pnt aProj = GEOMUtils::ProjectPointOnFace(aPnt, theFace,U,V,theTolerance);
    Standard_Real aDist = aProj.Distance(aPnt);
    if(aDist > aMaxDist)
    {
      aMaxDist = aDist;
    }
  }
  return aMaxDist;
}

//================================================================================
// function : divideSphericalShape
// purpose  : divide spherical shape into two piece 
//            and choose that part that has the points
//================================================================================
static void divideSphericalShape(TopoDS_Shape &theShape,
                                 const TopoDS_Wire &theWire,
                                 const std::vector<gp_Pnt> &thePoints,
                                 const Standard_Real theTolerance)
{
    TopExp_Explorer anExp(theShape, TopAbs_FACE);
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    TopoDS_Face aToolFace;
    GEOMImpl_Block6Explorer::MakeFace(theWire, false, aToolFace);
    if(!aToolFace.IsNull())
    {
        //split sphere and choose right part 
        GEOMAlgo_Splitter PS;
        PS.AddArgument(aFace);
        PS.AddTool(aToolFace);
        PS.SetLimit(TopAbs_FACE);
        PS.Perform();
        TopoDS_Shape aResultShape = PS.Shape();
        if(!aResultShape.IsNull())
        {
            anExp.Init(aResultShape, TopAbs_FACE);
            for (; anExp.More(); anExp.Next()) 
            {
                Standard_Real aDist = maxDistanceToFace(thePoints, TopoDS::Face(anExp.Current()), theTolerance);
                if(aDist < theTolerance)
                {
                    theShape = TopoDS::Face(anExp.Current());
                    break;
                }
            }
        }
    }
}

//================================================================================
// function : makeFaceFromPointsAndWire
// purpose  : Create face from set of points with the same approach as for 
//            geompy.makeSmoothingSurface. Cut resulting surface with a wire
//================================================================================
static TopoDS_Shape makeFaceFromPointsAndWire(const TopoDS_Wire &theWire,
                                              const std::vector<gp_Pnt> &theAllPoints)
{
  int nbPoints = theAllPoints.size();
  Handle(TColgp_HArray1OfPnt) anArrayofPnt = new TColgp_HArray1OfPnt(1, nbPoints);
  for (int i = 0; i < nbPoints; i++) {
    gp_Pnt aP = theAllPoints[i];
    anArrayofPnt->SetValue(i+1, aP);
  }

  // Try to build smoothing surface
  GeomPlate_BuildAveragePlane gpbap(anArrayofPnt,anArrayofPnt->Length(),Precision::Confusion(),1,1);
  Handle(Geom_Plane) plane(gpbap.Plane());

  Standard_Real Umin, Umax, Vmin, Vmax;
  gpbap.MinMaxBox(Umin,Umax,Vmin,Vmax);
  
  TopoDS_Face aInitShape;
  BRepBuilderAPI_MakeFace mf(plane,Umin,Umax,Vmin,Vmax,Precision::Confusion());
  aInitShape =  mf.Face();

  GeomPlate_BuildPlateSurface aBuilder(3,10);
  // ** Initialization of surface
#if OCC_VERSION_LARGE < 0x07070000
  Handle(BRepAdaptor_HSurface) HSI = new BRepAdaptor_HSurface();
  HSI->ChangeSurface().Initialize(aInitShape);
  aBuilder.LoadInitSurface( BRep_Tool::Surface(HSI->ChangeSurface().Face()));
#else
  Handle(BRepAdaptor_Surface) HSI = new BRepAdaptor_Surface();
  HSI->Initialize(aInitShape);
  aBuilder.LoadInitSurface( BRep_Tool::Surface(HSI->Face()) );
#endif

  Standard_Integer j, j1, j2;
  j1 = anArrayofPnt->Lower();
  j2 = anArrayofPnt->Upper();
  for (j=j1; j<=j2 ; j++)
  {
    gp_Pnt aPnt = anArrayofPnt->Value(j); 
    Handle(GeomPlate_PointConstraint) PCont = new GeomPlate_PointConstraint(aPnt,0);
    aBuilder.Add(PCont);
  }
  aBuilder.Perform();
  Handle(GeomPlate_Surface) gpPlate = aBuilder.Surface();
  if(gpPlate.IsNull())
  {
    Standard_ConstructionError::Raise("Not possible to build a face with given input, GeomPlate_Surface failed");
  }
  
  Standard_Real seuil = Max(0.0001,10*aBuilder.G0Error());
  GeomPlate_MakeApprox Mapp(gpPlate,0.0001,2,8,seuil);
  Handle(Geom_Surface) aSurf(Mapp.Surface());
  
  //cut surface with a face
  TopoDS_Shape aShape;
  BRepBuilderAPI_MakeFace aMkFace(aSurf, theWire);
  if (aMkFace.IsDone())
  {
    aShape = aMkFace.Shape();
  }

  TopoDS_Face aFace = TopoDS::Face(aShape);
  Handle(ShapeFix_Face) aFix = new ShapeFix_Face(aFace);
  aFix->Perform();
  aFix->FixOrientation();
  aFace = aFix->Face();

  return aFace;
}

//=======================================================================
// function : Execute
// purpose  :
//=======================================================================
Standard_Integer GEOMImpl_WrappingDriver::Execute(Handle(TFunction_Logbook) & log) const
{
  if (Label().IsNull())
    return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IWrap aCI(aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == WRAPPING_FACE)
  {
    Handle(TColStd_HSequenceOfTransient) anEdges = aCI.GetEdges();
    Handle(TColStd_HSequenceOfTransient) aVertices = aCI.GetVertices();

    int nbEdge = anEdges->Length();
    int nbVertices = aVertices->Length();

    if (nbEdge < 1 || nbVertices < 1)
    {
      Standard_ConstructionError::Raise("No edges or vertices given");
    }

    Standard_Real aTolerance = aCI.GetTolerance();
    if (aTolerance < Precision::Confusion())
      aTolerance = Precision::Confusion();

    std::vector<gp_Pnt> anAllPoints, aPoints;
    createPointsOnEdges(anEdges, anAllPoints);
    for (int i = 1; i <= aVertices->Length(); i++)
    {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aVertices->Value(i));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull())
      {
        Standard_NullObject::Raise("Vertex for face construction is null");
      }
      TopExp_Explorer exp (aShape_i, TopAbs_VERTEX);
      for (; exp.More(); exp.Next())
      {
        gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(exp.Current()));
        anAllPoints.push_back(aP);
        aPoints.push_back(aP);
      }
    }

    if(anAllPoints.size() == 0)
    {
      Standard_NullObject::Raise("Shape creation imposible, no valid objects were given");
    }

    //create wire from edges
    TopoDS_Wire aWire = GEOMImpl_ShapeDriver::MakeWireFromEdges(anEdges, aTolerance);
    if(aWire.IsNull())
    {
      Standard_NullObject::Raise("Given edges does not create closed contour");
    }

    aShape = createWrappedFace(aWire, anAllPoints, aPoints, aTolerance);
  }

  if (aShape.IsNull())
    return 0;

  aFunction->SetValue(aShape);

  log->SetTouched(Label());

  return 1;
}

//================================================================================
/*!
 * \brief Create wrapped face from vector of points and wire
 */
//================================================================================
TopoDS_Shape GEOMImpl_WrappingDriver::createWrappedFace(const TopoDS_Wire &theWire,
                                                        const std::vector<gp_Pnt> &theAllPoints,
                                                        const std::vector<gp_Pnt> &thePoints,
                                                        const Standard_Real theTolerance) const
{
  TopoDS_Shape aShape;

  gp_Pnt aCenter(0,0,0);
  Standard_Real aRadius = 100;
  if(isSphere(theAllPoints, aCenter, aRadius, theTolerance))
  {
    aShape = BRepPrimAPI_MakeSphere(aCenter, aRadius).Shape();
    divideSphericalShape(aShape, theWire, thePoints, theTolerance);
    return aShape;
  }

  aShape = makeFaceFromPointsAndWire(theWire, theAllPoints);
  return aShape;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================
Standard_Boolean GEOMImpl_WrappingDriver::isSphere(const std::vector<gp_Pnt>& thePoints,
                                 gp_Pnt& theCenter, 
                                 Standard_Real& theRadius,
                                 const Standard_Real theTolerance)const
{
  int aNumPoints = thePoints.size();
  if(aNumPoints == 0)
  {
    return false;
  }

  // Create coefficient matrix A and right-hand side vector f
  Eigen::MatrixXd A(aNumPoints, 4);
  Eigen::VectorXd f(aNumPoints);

  Standard_Real X(.0);
  Standard_Real Y(.0);
  Standard_Real Z(.0);

  for (int i = 0; i < aNumPoints; ++i) 
  {
    X = thePoints[i].X();
    Y = thePoints[i].Y();
    Z = thePoints[i].Z();
    A(i, 0) = X * 2;
    A(i, 1) = Y * 2;
    A(i, 2) = Z * 2;
    A(i, 3) = 1.0;
    f(i) = X * X + Y * Y + Z * Z;
  }

  // Solve linear equations to get coefficients
  Eigen::VectorXd c = A.colPivHouseholderQr().solve(f);

  Standard_Real t = c[0] * c[0] + c[1] * c[1] + c[2] * c[2] + c[3];
  theRadius = std::sqrt(t);
  theCenter.SetCoord(c[0], c[1], c[2]);

  //check that all points belong to the sphere within theTolerance
  std::vector<Standard_Real> aDists;
  for(const auto& aPnt : thePoints)
  {
    aDists.push_back(aPnt.Distance(theCenter));
  }
  if (!aDists.empty()) 
  {
    Standard_Real sumDistances = std::accumulate(aDists.begin(), aDists.end(), 0.0);
    Standard_Real averageDistance = sumDistances / aDists.size();

    if (std::fabs(averageDistance - theRadius) > theTolerance) {
        return false;
    }
  }

  return true;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_WrappingDriver::
    GetCreationInformation(std::string &theOperationName,
                           std::vector<GEOM_Param> &theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IWrap aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "WRAPPEDFACE";

  switch ( aType ) {
  case WRAPPING_FACE:
    AddParam(theParams, "Edges", aCI.GetEdges());
    AddParam(theParams, "Vertices", aCI.GetVertices());
    AddParam(theParams, "Tolerance", aCI.GetTolerance());
    break;
  default:
    return false;
  }
  
  return true;
}

IMPLEMENT_STANDARD_RTTIEXT(GEOMImpl_WrappingDriver, GEOM_BaseDriver)