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
//
// GEOM OBJECT : interactive object for Geometry entities visualization
// File   : GEOM_AISVector.cxx
// Author : Julia DOROVSKIKH
// $Header$
//
#include <GEOM_AISVector.hxx>

// OCCT Includes
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Arrow.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Graphic3d_Group.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

IMPLEMENT_STANDARD_HANDLE(GEOM_AISVector, GEOM_AISShape)
IMPLEMENT_STANDARD_RTTIEXT(GEOM_AISVector, GEOM_AISShape)

//=======================================================================
//function : GEOM_AISVector
//purpose  : Constructor
//=======================================================================
GEOM_AISVector::GEOM_AISVector (const TopoDS_Shape& theShape, const Standard_CString theName)
  : GEOM_AISShape(theShape, theName)
{
}

//=======================================================================
//function : Compute
//purpose  : Compute a presentation
//=======================================================================
void GEOM_AISVector::Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                              const Handle(Prs3d_Presentation)& thePrs,
                              const Standard_Integer theMode)
{
  GEOM_AISShape::Compute(thePresentationManager, thePrs, theMode);

  if (myshape.ShapeType() == TopAbs_EDGE)
  {
    TopoDS_Vertex aV1, aV2;
    TopoDS_Edge anEdgeE = TopoDS::Edge(myshape);
    TopExp::Vertices(anEdgeE, aV1, aV2);
    gp_Pnt aP1 = BRep_Tool::Pnt(aV1);
    gp_Pnt aP2 = BRep_Tool::Pnt(aV2);

    gp_Vec aVec (aP1, aP2);
    Standard_Real aDist = aVec.Magnitude();
    if (aDist > gp::Resolution())
    {
      gp_Dir aDir (aVec);

      Handle(Graphic3d_Group) aG = Prs3d_Root::CurrentGroup(thePrs);

      //thePrs->Color(myShadingColor.Name());
      //aG->BeginPrimitives();
      Prs3d_Arrow::Draw(thePrs, aP2, aDir, PI/180.*5., aDist/10.);
      //aG->EndPrimitives();
    }
  }
  //thePrs->ReCompute(); // for hidden line recomputation if necessary...
}
