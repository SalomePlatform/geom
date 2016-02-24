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

#include "GEOMImpl_PolylineDriver.hxx"

#include "GEOMImpl_ICurveParametric.hxx"
#include "GEOMImpl_ICurvesOperations.hxx"
#include "GEOMImpl_IPolyline.hxx"
#include "GEOMImpl_IPolyline2D.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"
#include <GEOMUtils.hxx>
#include <Sketcher_Utils.hxx>

#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Ax3.hxx>
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
//function : MakePolyline2D
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_PolylineDriver::MakePolyline2D(LOGBOOK& log) const
{
  if (Label().IsNull()) {
    return 0;
  }

  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  GEOMImpl_IPolyline2D  aCI(aFunction);
  Standard_Integer      aType = aFunction->GetType();
  TopoDS_Shape          aShape;

  // Get data.
  Handle(TColStd_HArray1OfExtendedString) aNames       = aCI.GetNames();
  Handle(TColStd_HArray1OfByte)           aTypes       = aCI.GetTypes();
  Handle(TColStd_HArray1OfByte)           aClosedFlags = aCI.GetClosedFlags();
  std::list <std::list <double> >         aCoords;
  gp_Ax3                                  aWPlane;

  aCI.GetCoords(aCoords);

  // Check the data validity
  if (aNames.IsNull()) {
    return 0;
  }

  Standard_Integer aNbSections = aNames->Length();

  if (aTypes.IsNull() || aNbSections != aTypes->Length()) {
    return 0;
  }

  if (aClosedFlags.IsNull() || aNbSections != aClosedFlags->Length()) {
    return 0;
  }

  if (aNbSections != aCoords.size()) {
    return 0;
  }

  if (aType == POLYLINE2D_PLN_COORDS) {
    Handle(TColStd_HArray1OfReal) aPlaneCoords = aCI.GetWorkingPlaneDbls();

    if (aPlaneCoords.IsNull()) {
      return 0;
    }

    if (aPlaneCoords->Length() != 9) {
      return 0;
    }

    Standard_Integer i = aPlaneCoords->Lower();
    gp_Pnt aOrigin(aPlaneCoords->Value(i), aPlaneCoords->Value(i + 1),
                 aPlaneCoords->Value(i + 2));
    gp_Dir aDirZ(aPlaneCoords->Value(i + 3), aPlaneCoords->Value(i + 4),
                 aPlaneCoords->Value(i + 5));
    gp_Dir aDirX(aPlaneCoords->Value(i + 6), aPlaneCoords->Value(i + 7),
                 aPlaneCoords->Value(i + 8));
    aWPlane = gp_Ax3(aOrigin, aDirZ, aDirX);
  } else if (aType == POLYLINE2D_PLN_OBJECT) {
    Handle(GEOM_Function) aRefFace = aCI.GetWorkingPlane();
    TopoDS_Shape aShape = aRefFace->GetValue();

    aWPlane = GEOMUtils::GetPosition(aShape);
  } else {
    return 0;
  }

  // Construct a shape.
  Standard_Integer iN = aNames->Lower();
  Standard_Integer iT = aTypes->Lower();
  Standard_Integer iC = aClosedFlags->Lower();
  std::list <std::list <double> >::const_iterator anIter = aCoords.begin();
  BRep_Builder aBuilder;
  Standard_Boolean isEmpty = Standard_True;

  if (aNbSections > 1) {
    aBuilder.MakeCompound(TopoDS::Compound(aShape));
  }

  for (; anIter != aCoords.end(); ++anIter, ++iN, ++iT, ++iC) {
    Standard_Integer aType = aTypes->Value(iT);
    TopoDS_Shape     aSection;

    if (aType == GEOMImpl_ICurvesOperations::Polyline) {
      aSection = Sketcher_Utils::MakePolyline
          (*anIter, aClosedFlags->Value(iC), aWPlane);
    } else if (aType == GEOMImpl_ICurvesOperations::Interpolation) {
      aSection = Sketcher_Utils::MakeInterpolation
        (*anIter, aClosedFlags->Value(iC), aWPlane);
    }

    if (aNbSections > 1) {
      // There are multiple sections.
      if (aSection.IsNull() == Standard_False) {
        aBuilder.Add(aShape, aSection);
        isEmpty = Standard_False;
      }
    } else {
      // There is only one section.
      isEmpty = aSection.IsNull();
      aShape  = aSection;
    }
  }

  if (isEmpty) {
    return 0;
  }

  aFunction->SetValue(aShape);
#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  return 1;
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_PolylineDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  Standard_Integer aType = aFunction->GetType();

  if (aType == POLYLINE2D_PLN_COORDS || aType == POLYLINE2D_PLN_OBJECT) {
    return MakePolyline2D(log);
  }

  GEOMImpl_IPolyline aCI (aFunction);
  
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

bool GEOMImpl_PolylineDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPolyline        aCI( function );
  GEOMImpl_ICurveParametric aIP( function );
  Standard_Integer aType = function->GetType();

  switch ( aType ) {
  case POLYLINE_POINTS:
    theOperationName = "CURVE";
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
  case POLYLINE2D_PLN_COORDS:
  case POLYLINE2D_PLN_OBJECT:
    {
      theOperationName = "SKETCH";

      GEOMImpl_IPolyline2D                    aP2d(function);
      Handle(TColStd_HArray1OfExtendedString) aNames = aP2d.GetNames();

      if (aNames.IsNull() == Standard_False) {
        if (aNames->Length() == 1) {
          // This is the single curve. Make its full dump.
          AddParam(theParams, "Name", aNames->Value(aNames->Lower()));

          Handle(TColStd_HArray1OfByte) aTypes = aP2d.GetTypes();

          if (aTypes.IsNull() == Standard_False && aTypes->Length() == 1) {
            Standard_Integer aType = aTypes->Value(aTypes->Lower());

            if (aType == GEOMImpl_ICurvesOperations::Polyline) {
              AddParam(theParams, "Type") << "Polyline";
            } else if (aType == GEOMImpl_ICurvesOperations::Interpolation) {
              AddParam(theParams, "Type") << "Interpolation";
            }
          }

          Handle(TColStd_HArray1OfByte) aCloseds = aP2d.GetClosedFlags();

          if (aCloseds.IsNull() == Standard_False && aCloseds->Length() == 1) {
            const char *aYesNo =
              aCloseds->Value(aCloseds->Lower()) ? "Yes" : "No";

            AddParam(theParams, "Is closed", aYesNo);
          }

          std::list <std::list <double> > aCoords;

          aP2d.GetCoords(aCoords);

          if (aCoords.size() == 1) {
            AddParam(theParams, "Number of points", aCoords.front().size()/2);
          }
        } else {
          // There are more than 1 curve.
          Standard_Integer                aNbCurves = aNames->Length();
          Standard_Integer                i;
          std::list <std::list <double> > aCoords;

          AddParam(theParams, "Number of curves", aNbCurves);
          aP2d.GetCoords(aCoords);

          Standard_Integer aNbCoords = aCoords.size();
          std::list <std::list <double> >::const_iterator
                           anIt      = aCoords.begin();

          for (i = 0; i < aNbCurves; i++) {
            TCollection_AsciiString aName("Curve ");
            TCollection_ExtendedString
              aValue(aNames->Value(aNames->Lower() + i));

            aName.AssignCat(i + 1);

            if (anIt != aCoords.end()) {
              aValue.AssignCat(" (");
              aValue.AssignCat(Standard_Integer(anIt->size()));
              aValue.AssignCat(" points)");
              anIt++;
            }

            AddParam(theParams, aName.ToCString(), aValue);
          }
        }
      }

      if (aType == POLYLINE2D_PLN_COORDS) {
        Handle(TColStd_HArray1OfReal) aPln = aP2d.GetWorkingPlaneDbls();

        if (aPln.IsNull() == Standard_False && aPln->Length() == 9) {
          Standard_Integer i = aPln->Lower();

          AddParam( theParams, "Origin")
            << aPln->Value(i)     << " "
            << aPln->Value(i + 1) << " "
            << aPln->Value(i + 2);
          AddParam( theParams, "OZ")
            << aPln->Value(i + 3) << " "
            << aPln->Value(i + 4) << " "
            << aPln->Value(i + 5);
          AddParam( theParams, "OX")
            << aPln->Value(i + 6) << " "
            << aPln->Value(i + 7) << " "
            << aPln->Value(i + 8);
        }
      } else {
        AddParam(theParams, "Working plane", aP2d.GetWorkingPlane(), "XOY");
      }
    }
    break;
  default:
    return false;
  }

  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_PolylineDriver,GEOM_BaseDriver);
