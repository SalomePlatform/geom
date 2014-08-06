// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GEOMImpl_PolylineDriver.hxx"

#include "GEOMImpl_ICurveParametric.hxx"
#include "GEOMImpl_IPolyline.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_PolylineDriver::GetID()
{
  static Standard_GUID aPolylineDriver("FF1BBB31-5D14-4df2-980B-3A668264EA16");
  return aPolylineDriver; 
}


//=======================================================================
//function : GEOMImpl_PolylineDriver
//purpose  : 
//=======================================================================
GEOMImpl_PolylineDriver::GEOMImpl_PolylineDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_PolylineDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPolyline aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();
  
  TopoDS_Shape aShape;

  if (aType == POLYLINE_POINTS) {

    bool useCoords = aCI.GetConstructorType() == COORD_CONSTRUCTOR;
    TColgp_Array1OfPnt points(1, (useCoords ? aCI.GetLength() : 1) );
    if(useCoords) {
      Handle(TColStd_HArray1OfReal) aCoordsArray = aCI.GetCoordinates();
      int anArrayLength = aCoordsArray->Length();
      for (int i = 0, j = 1; i <= (anArrayLength-3); i += 3) {
        gp_Pnt aPnt = gp_Pnt(aCoordsArray->Value(i+1), aCoordsArray->Value(i+2), aCoordsArray->Value(i+3));
        points.SetValue(j,aPnt);
        j++;
      } 
    }

    int aLen = aCI.GetLength();
    int ind = 1;
    BRepBuilderAPI_MakePolygon aMakePoly;
    for (; ind <= aLen; ind++)
    {
      if(useCoords) {
        aMakePoly.Add(BRepBuilderAPI_MakeVertex(points.Value(ind)));
      } else {
        Handle(GEOM_Function) aRefPoint = aCI.GetPoint(ind);
        TopoDS_Shape aShapePnt = aRefPoint->GetValue();
        if (aShapePnt.ShapeType() != TopAbs_VERTEX) {
          Standard_TypeMismatch::Raise
            ("Polyline creation aborted : arguments are not a vertexes");
          return 0;
        }
        if (aShapePnt.ShapeType() == TopAbs_VERTEX) {
          aMakePoly.Add(TopoDS::Vertex(aShapePnt));
          //if (!aMakePoly.Added()) return 0;
        }
      }
    }
    // Compare first and last point coordinates and close polyline if it's the same.
    if ( aLen > 2 ) {
      TopoDS_Vertex aV1;
      if( useCoords ) {
        aV1 = BRepBuilderAPI_MakeVertex(points.Value(1));
      } else {
        Handle(GEOM_Function) aFPoint = aCI.GetPoint(1);
        TopoDS_Shape aFirstPnt = aFPoint->GetValue();
        aV1 = TopoDS::Vertex(aFirstPnt);
      }
      
      TopoDS_Vertex aV2;
      if( useCoords ) {
        aV2 = BRepBuilderAPI_MakeVertex(points.Value(aLen));
      } else {
        Handle(GEOM_Function) aLPoint = aCI.GetPoint(aLen);
        TopoDS_Shape aLastPnt = aLPoint->GetValue();
        aV2 = TopoDS::Vertex(aLastPnt);
      }
      
      if ( (!aV1.IsNull() && !aV2.IsNull() && aV1.IsSame(aV2)) ||
           aCI.GetIsClosed())
        aMakePoly.Close();
    }
    
    if (aMakePoly.IsDone()) {
      aShape = aMakePoly.Wire();
    }
  }
  else {
  }

  if (aShape.IsNull()) return 0;
  
  aFunction->SetValue(aShape);
  
  log.SetTouched(Label()); 
  
  return 1;    
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_PolylineDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPolyline        aCI( function );
  GEOMImpl_ICurveParametric aIP( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "CURVE";

  switch ( aType ) {
  case POLYLINE_POINTS:
    AddParam( theParams, "Type", "Polyline");
    if ( aIP.HasData() )
    {
      AddParam( theParams, "X(t) equation", aIP.GetExprX() );
      AddParam( theParams, "Y(t) equation", aIP.GetExprY() );
      AddParam( theParams, "Z(t) equation", aIP.GetExprZ() );
      AddParam( theParams, "Min t", aIP.GetParamMin() );
      AddParam( theParams, "Max t", aIP.GetParamMax() );
      if ( aIP.GetParamNbStep() )
        AddParam( theParams, "Number of steps", aIP.GetParamNbStep() );
      else
        AddParam( theParams, "t step", aIP.GetParamStep() );
    }
    else
    {
      GEOM_Param& pntParam = AddParam( theParams, "Points");
      if ( aCI.GetConstructorType() == COORD_CONSTRUCTOR )
      {
        Handle(TColStd_HArray1OfReal) coords = aCI.GetCoordinates();
        if ( coords->Length() > 3 )
          pntParam << ( coords->Length() ) / 3 << " points: ";
        for ( int i = coords->Lower(), nb = coords->Upper(); i <= nb; )
          pntParam << "( " << coords->Value( i++ )
                   << ", " << coords->Value( i++ )
                   << ", " << coords->Value( i++ ) << " ) ";
      }
      else
      {
        if ( aCI.GetLength() > 1 )
          pntParam << aCI.GetLength() << " points: ";
        for ( int i = 1, nb = aCI.GetLength(); i <= nb; ++i )
          pntParam << aCI.GetPoint( i ) << " ";
      }
      AddParam( theParams, "Is closed", aCI.GetIsClosed() );
    }
    break;
  default:
    return false;
  }

  return true;
}

IMPLEMENT_STANDARD_HANDLE (GEOMImpl_PolylineDriver,GEOM_BaseDriver);
IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_PolylineDriver,GEOM_BaseDriver);
