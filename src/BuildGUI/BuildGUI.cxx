//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : BooleanGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "BuildGUI.h"

#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include "BuildGUI_EdgeDlg.h"       // Method EDGE
#include "BuildGUI_WireDlg.h"       // Method WIRE
#include "BuildGUI_FaceDlg.h"       // Method FACE
#include "BuildGUI_ShellDlg.h"      // Method SHELL
#include "BuildGUI_SolidDlg.h"      // Method SOLID
#include "BuildGUI_CompoundDlg.h"   // Method COMPOUND

//=======================================================================
// function : BuildGUI()
// purpose  : Constructor
//=======================================================================
BuildGUI::BuildGUI() :
  QObject()
{
  myGeomBase = new GEOMBase();
  myGeomGUI = GEOMContext::GetGeomGUI();
  myGeom = myGeomGUI->myComponentGeom;
}


//=======================================================================
// function : ~BuildGUI()
// purpose  : Destructor
//=======================================================================
BuildGUI::~BuildGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool BuildGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  BuildGUI* myBuildGUI = new BuildGUI();
  myBuildGUI->myGeomGUI->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());

  switch (theCommandID)
    {
    case 4081: // GEOM::EDGE
      {
	BuildGUI_EdgeDlg *aDlg = new BuildGUI_EdgeDlg(parent, "", myBuildGUI, Sel);
	break;
      }
    case 4082: // GEOM::WIRE
      {
	BuildGUI_WireDlg *aDlg = new BuildGUI_WireDlg(parent, "", myBuildGUI, Sel);
	break;
      }
    case 4083: // GEOM::FACE
      {
	BuildGUI_FaceDlg *aDlg = new BuildGUI_FaceDlg(parent, "", myBuildGUI, Sel);
	break;
      }
    case 4084: // GEOM::SHELL
      {
	BuildGUI_ShellDlg *aDlg = new BuildGUI_ShellDlg(parent, "", myBuildGUI, Sel);
	break;
      }
    case 4085: // GEOM::SOLID
      {
	BuildGUI_SolidDlg *aDlg = new BuildGUI_SolidDlg(parent, "", myBuildGUI, Sel);
	break;
      }
    case 4086: // GEOM::COMPOUND
      {
	BuildGUI_CompoundDlg *aDlg = new BuildGUI_CompoundDlg(parent, "", myBuildGUI, Sel);
	break;
      }
    default:
      {
	parent->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
	break;
      }
    }
  return true;
}


//=====================================================================================
// function : MakeLinearEdgeAndDisplay()
// purpose  :
//=====================================================================================
void BuildGUI::MakeLinearEdgeAndDisplay(const gp_Pnt P1, const gp_Pnt P2)
{
  try {
    GEOM::PointStruct ps1 = myGeom->MakePointStruct(P1.X(), P1.Y(), P1.Z());
    GEOM::PointStruct ps2 = myGeom->MakePointStruct(P2.X(), P2.Y(), P2.Z());
    GEOM::GEOM_Shape_var result = myGeom->MakeEdge(ps1, ps2);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE"));
      return;
    }
    result->NameType(tr("GEOM_EDGE"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeWireAndDisplay()
// purpose  :
//=====================================================================================
void BuildGUI::MakeWireAndDisplay(GEOM::GEOM_Gen::ListOfIOR& listShapesIOR)
{
  try {
    GEOM::GEOM_Shape_var result = myGeom->MakeWire(listShapesIOR);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE"));
      return;
    }
    result->NameType(tr("GEOM_WIRE"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeFaceAndDisplay()
// purpose  :
//=====================================================================================
void BuildGUI::MakeFaceAndDisplay(GEOM::GEOM_Gen::ListOfIOR& listShapesIOR,
				  const Standard_Boolean wantPlanar)
{
  try {
    GEOM::GEOM_Shape_var result = myGeom->MakeFaces(listShapesIOR, wantPlanar);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE"));
      return;
    }
    if (wantPlanar)
      result->NameType(tr("GEOM_PLANE"));
    else
      result->NameType(tr("GEOM_FACE"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeShellAndDisplay()
// purpose  :
//=====================================================================================
void BuildGUI::MakeShellAndDisplay(GEOM::GEOM_Gen::ListOfIOR& listShapesIOR)
{
  try {
    GEOM::GEOM_Shape_var result = myGeom->MakeShell(listShapesIOR);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE"));
      return;
    }
    result->NameType(tr("GEOM_SHELL"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeSolidAndDisplay()
// purpose  :
//=====================================================================================
void BuildGUI::MakeSolidAndDisplay(GEOM::GEOM_Gen::ListOfIOR& listShapesIOR)
{
  try {
    GEOM::GEOM_Shape_var result = myGeom->MakeSolid(listShapesIOR);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE"));
      return;
    }
    result->NameType(tr("GEOM_SOLID"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeCompoundAndDisplay()
// purpose  :
//=====================================================================================
void BuildGUI::MakeCompoundAndDisplay(GEOM::GEOM_Gen::ListOfIOR& listShapesIOR)
{
  try {
    GEOM::GEOM_Shape_var result = myGeom->MakeCompound(listShapesIOR);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE"));
      return;
    }
    result->NameType(tr("GEOM_COMPOUND"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  bool OnGUIEvent(int theCommandID, QAD_Desktop* parent)
  {return BuildGUI::OnGUIEvent(theCommandID, parent);}
}
