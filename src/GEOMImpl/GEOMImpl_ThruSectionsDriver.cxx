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

#include <GEOMImpl_ThruSectionsDriver.hxx>
#include <GEOMImpl_IThruSections.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>
#include <GEOMUtils.hxx>

#include <TColStd_HSequenceOfTransient.hxx>
#include <Precision.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

#include <Standard_TypeMismatch.hxx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_ThruSectionsDriver::GetID()
{
  static Standard_GUID aThruSectionsDriver("FF1BB971-E99C-4f89-B989-5B48E061049B");
  return aThruSectionsDriver;
}


//=======================================================================
//function : GEOMImpl_ThruSectionsDriver
//purpose  :
//=======================================================================
GEOMImpl_ThruSectionsDriver::GEOMImpl_ThruSectionsDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_ThruSectionsDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IThruSections aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  Standard_Boolean isSolid = (aCI.GetSolidMode() == 1);
  Handle(TColStd_HSequenceOfTransient) aSeqSections = aCI.GetSections();

  if( aSeqSections.IsNull())
    return 0;

  Standard_Integer aNbSections = aSeqSections->Length();
  Standard_Real aPreci = Max(aCI.GetPrecision(),Precision::Confusion());
  if(!aNbSections )
    return 0;

  BRepOffsetAPI_ThruSections aBuilder(isSolid,aType ==THRUSECTIONS_RULED,aPreci);
  
 
  aBuilder.CheckCompatibility(Standard_False);
  //added sections for building surface
  Standard_Integer i =1;
  Standard_Integer nbAdded =0;
  for( ; i <= aNbSections; i++,nbAdded++)
  {
    Handle(Standard_Transient) anItem = aSeqSections->Value(i);
    if(anItem.IsNull())
      continue;

    Handle(GEOM_Function) aSection = Handle(GEOM_Function)::DownCast(anItem);
    if(aSection.IsNull())
      continue;

    TopoDS_Shape aShapeSection = aSection->GetValue();
    TopAbs_ShapeEnum aTypeSect = aShapeSection.ShapeType();
    if(aTypeSect == TopAbs_WIRE)
      aBuilder.AddWire(TopoDS::Wire(aShapeSection));

    else if(aTypeSect == TopAbs_EDGE) {
      TopoDS_Edge anEdge = TopoDS::Edge(aShapeSection);
      TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge);
      aBuilder.AddWire(aWire);
    }
    else if(aTypeSect == TopAbs_VERTEX) {
      TopoDS_Vertex aVert = TopoDS::Vertex(aShapeSection);
      aBuilder.AddVertex(aVert);
    }
    else
       nbAdded--; 
  }  
  if(!nbAdded)
     Standard_TypeMismatch::Raise("ThruSections aborted : invalid types of sections");
  //make surface by sections
  aBuilder.Build();
  TopoDS_Shape aShape = aBuilder.Shape();
  if (aShape.IsNull()) {
    return 0;
  }

  if ( !GEOMUtils::CheckShape(aShape) && !GEOMUtils::FixShapeTolerance(aShape) ) {
    //algoritm thru section creats on the arcs invalid shapes gka
    //  Standard_ConstructionError::Raise("Algorithm have produced an invalid shape result");
  }

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

bool GEOMImpl_ThruSectionsDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IThruSections aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "MakeThruSections";

  switch ( aType ) {
  case THRUSECTIONS_RULED:
  case THRUSECTIONS_SMOOTHED:
    AddParam( theParams, "Sections", aCI.GetSections() );
    AddParam( theParams, "Is solid", aCI.GetSolidMode() );
    AddParam( theParams, "Precision", aCI.GetPrecision() );
    AddParam( theParams, "Is ruled", aType == THRUSECTIONS_RULED );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ThruSectionsDriver,GEOM_BaseDriver);
