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
//  File   : PrimitiveGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "PrimitiveGUI.h"

#include "SALOMEGUI_QtCatchCorbaException.hxx"
#include <Precision.hxx>

#include "PrimitiveGUI_BoxDlg.h"      // Method BOX
#include "PrimitiveGUI_CylinderDlg.h" // Method CYLINDER
#include "PrimitiveGUI_SphereDlg.h"   // Method SPHERE
#include "PrimitiveGUI_TorusDlg.h"    // Method TORUS
#include "PrimitiveGUI_ConeDlg.h"     // Method CONE

//=======================================================================
// function : PrimitiveGUI()
// purpose  : Constructor
//=======================================================================
PrimitiveGUI::PrimitiveGUI() :
  QObject()
{
  myGeomBase = new GEOMBase();
  myGeomGUI = GEOMContext::GetGeomGUI();
  myGeom = myGeomGUI->myComponentGeom;
}


//=======================================================================
// function : ~PrimitiveGUI()
// purpose  : Destructor
//=======================================================================
PrimitiveGUI::~PrimitiveGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool PrimitiveGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  PrimitiveGUI* aPrimitiveGUI = new PrimitiveGUI();
  aPrimitiveGUI->myGeomGUI->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  switch (theCommandID)
    {
    case 4021: // BOX
      {
	PrimitiveGUI_BoxDlg* aDlg = new PrimitiveGUI_BoxDlg(parent, "", aPrimitiveGUI, Sel);
	break;
      }
    case 4022: // CYLINDER
      {
	PrimitiveGUI_CylinderDlg* aDlg = new PrimitiveGUI_CylinderDlg(parent, "", aPrimitiveGUI, Sel);
	break;
      }
    case 4023: // SPHERE
      {
	PrimitiveGUI_SphereDlg* aDlg = new PrimitiveGUI_SphereDlg(parent, "", aPrimitiveGUI, Sel);
	break;
      }
    case 4024: // TORUS
      {
	PrimitiveGUI_TorusDlg* aDlg = new PrimitiveGUI_TorusDlg(parent, "", aPrimitiveGUI, Sel);
	break;
      }
    case 4025: // CONE
      {
	PrimitiveGUI_ConeDlg* aDlg = new PrimitiveGUI_ConeDlg(parent, "", aPrimitiveGUI, Sel);
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


//=======================================================================
// function : MakeBoxAndDisplay()
// purpose  : 
//=======================================================================
void PrimitiveGUI::MakeBoxAndDisplay(const gp_Pnt P1, const gp_Pnt P2)
{
  try {
    GEOM::GEOM_Shape_var box = myGeom->MakeBox(P1.X(), P1.Y(), P1.Z(), P2.X(), P2.Y(), P2.Z());
    box->NameType(tr("GEOM_BOX"));
    if(myGeomBase->Display(box))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
}


//=====================================================================================
// function : MakeCylinderAndDisplay()
// purpose  :
//=====================================================================================
void PrimitiveGUI::MakeCylinderAndDisplay(const gp_Pnt BasePoint, const gp_Dir aDir,
					 const double Radius, const double aHeight)
{
  try {
    if(Radius <= Precision::Confusion() || aHeight <= Precision::Confusion())
      return;

    GEOM::PointStruct pstruct = myGeom->MakePointStruct(BasePoint.X(), BasePoint.Y(), BasePoint.Z());
    GEOM::PointStruct d = myGeom->MakePointStruct(aDir.X(), aDir.Y(), aDir.Z());
    GEOM::DirStruct dstruct = myGeom->MakeDirection(d);
    
    GEOM::GEOM_Shape_var result = myGeom->MakeCylinder(pstruct, dstruct, Radius, aHeight);
    if (result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return;
    }
    result->NameType(tr("GEOM_CYLINDER"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeSphere()
// purpose  :
//=====================================================================================
void PrimitiveGUI::MakeSphereAndDisplay(const gp_Pnt aCenterPoint, const double aRadius)
{
  try {
    if(aRadius <= Precision::Confusion())
      return;

    GEOM::GEOM_Shape_ptr result = myGeom->MakeSphere(aCenterPoint.X(),aCenterPoint.Y(),aCenterPoint.Z(), aRadius);
    result->NameType(tr("GEOM_SPHERE"));
    if (myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch (const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeTorusAndDisplay()
// purpose  :
//=====================================================================================
void PrimitiveGUI::MakeTorusAndDisplay(const gp_Pnt BasePoint, const gp_Dir aDir,
				      const double Radius1, const double Radius2)
{
  try {
    if(Radius1 <= Precision::Confusion() || Radius2 <= Precision::Confusion())
      return;
    
    GEOM::PointStruct pstruct = myGeom->MakePointStruct(BasePoint.X(), BasePoint.Y(), BasePoint.Z());
    GEOM::PointStruct d = myGeom ->MakePointStruct(aDir.X(), aDir.Y(), aDir.Z());
    GEOM::DirStruct dstruct = myGeom->MakeDirection(d);
    
    GEOM::GEOM_Shape_var result = myGeom->MakeTorus(pstruct, dstruct, Radius1, Radius2);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return;
    }
    result->NameType(tr("GEOM_TORUS"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeConeAndDisplay()
// purpose  :
//=====================================================================================
void PrimitiveGUI::MakeConeAndDisplay(const gp_Pnt BasePoint, const gp_Dir aDir,
				     const double Radius1, const double Radius2, const double aHeight)
{
  try {
    if((Radius1 <= Precision::Confusion() && Radius2 <= Precision::Confusion()) || aHeight <= Precision::Confusion())
      return;

    GEOM::PointStruct pstruct = myGeom->MakePointStruct(BasePoint.X(), BasePoint.Y(), BasePoint.Z());
    GEOM::PointStruct d = myGeom->MakePointStruct(aDir.X(), aDir.Y(), aDir.Z());
    GEOM::DirStruct dstruct = myGeom->MakeDirection(d);

    GEOM::GEOM_Shape_var result = myGeom->MakeCone(pstruct, dstruct, Radius1, Radius2, aHeight);  
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return;
    }
    result->NameType(tr("GEOM_CONE"));
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
  {return PrimitiveGUI::OnGUIEvent(theCommandID, parent);}
}
