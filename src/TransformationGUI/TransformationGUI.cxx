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
//  File   : TransformationGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "TransformationGUI.h"

#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include "TransformationGUI_MultiTranslationDlg.h"   // Method MULTI TRANSLATION
#include "TransformationGUI_MultiRotationDlg.h"      // Method MULTI ROTATION
#include "TransformationGUI_TranslationDlg.h"        // Method TRANSLATION
#include "TransformationGUI_RotationDlg.h"           // Method ROTATION
#include "TransformationGUI_MirrorDlg.h"             // Method MIRROR
#include "TransformationGUI_ScaleDlg.h"              // Method SCALE

//=======================================================================
// function : TransformationGUI()
// purpose  : Constructor
//=======================================================================
TransformationGUI::TransformationGUI() :
  QObject()
{
  myGeomBase = new GEOMBase();
  myGeomGUI = GEOMContext::GetGeomGUI();
  myGeom = myGeomGUI->myComponentGeom;
}


//=======================================================================
// function : ~TransformationGUI()
// purpose  : Destructor
//=======================================================================
TransformationGUI::~TransformationGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool TransformationGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  TransformationGUI* myTransformationGUI = new TransformationGUI();
  myTransformationGUI->myGeomGUI->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());

  switch (theCommandID)
    {
    case 5021: // TRANSLATION
      {	
  	TransformationGUI_TranslationDlg *aDlg = new TransformationGUI_TranslationDlg(parent, "", myTransformationGUI, Sel);
	break;
      }
    case 5022: // ROTATION
      {	
  	TransformationGUI_RotationDlg *aDlg = new TransformationGUI_RotationDlg(parent, "", myTransformationGUI, Sel);
	break;
      }
    case 5023: // MIRROR
      {	
	TransformationGUI_MirrorDlg *aDlg = new TransformationGUI_MirrorDlg(parent, "", myTransformationGUI, Sel);
	break;
      }
    case 5024: // SCALE
      {	
  	TransformationGUI_ScaleDlg *aDlg = new TransformationGUI_ScaleDlg(parent, "", myTransformationGUI, Sel );
	break;
      }
    case 5025: // MULTI TRANSLATION
      {	
  	TransformationGUI_MultiTranslationDlg *aDlg = new TransformationGUI_MultiTranslationDlg(parent, "", myTransformationGUI, Sel);
	break;
      }
    case 5026: // MULTI ROTATION
      {	
  	TransformationGUI_MultiRotationDlg *aDlg = new TransformationGUI_MultiRotationDlg(parent, "", myTransformationGUI, Sel);
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


//=================================================================================
// function : MakeTranslationAndDisplay()
// purpose  : Translate a shape
//=================================================================================
void TransformationGUI::MakeTranslationAndDisplay(GEOM::GEOM_Shape_ptr Shape, gp_Vec V)
{
  try {
    GEOM::GEOM_Shape_var result = myGeom->MakeTranslation(Shape, V.X(), V.Y(), V.Z());
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return;
    }
    result->NameType(Shape->NameType());
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }  
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=======================================================================================
// function : MakeRotationAndDisplay()
// purpose  :
//=======================================================================================
void TransformationGUI::MakeRotationAndDisplay(GEOM::GEOM_Shape_ptr Shape, const gp_Pnt loc,
					       const gp_Dir dir, const Standard_Real angle)
{
  try {
    const GEOM::AxisStruct axis = myGeom->MakeAxisStruct(loc.X(), loc.Y(), loc.Z(),
							 dir.X(), dir.Y(), dir.Z());
    GEOM::GEOM_Shape_var result = myGeom->MakeRotation(Shape, axis, angle);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return ;
    }
    result->NameType(Shape->NameType());
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }  
  return;
}


//=====================================================================================
// function : MakeMirrorAndDisplay()
// purpose  :
//=====================================================================================
void TransformationGUI::MakeMirrorAndDisplay(GEOM::GEOM_Shape_ptr Shape1, GEOM::GEOM_Shape_ptr Shape2)
{
  try {
    GEOM::GEOM_Shape_var result = myGeom->MakeMirrorByPlane(Shape1, Shape2);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE")); 
      return;
    }  
    result->NameType(Shape1->NameType());
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeScaleAndDisplay()
// purpose  :
//=====================================================================================
void TransformationGUI::MakeScaleAndDisplay(GEOM::GEOM_Shape_ptr Shape, const gp_Pnt centralPoint,
					    const Standard_Real factor)
{
  try {
    GEOM::PointStruct P = myGeom->MakePointStruct(centralPoint.X(), centralPoint.Y(), centralPoint.Z());
    GEOM::GEOM_Shape_var result = myGeom->MakeScaleTransform(Shape, P, factor);
    result->NameType(Shape->NameType());
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
    else 
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NULLSHAPE")); 
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=================================================================================
// function : MakeMultiTranslation1DAndDisplay()
// purpose  : Multi-Translate a shape
//=================================================================================
void TransformationGUI::MakeMultiTranslation1DAndDisplay(GEOM::GEOM_Shape_ptr Shape, const gp_Dir Dir,
							 const double Step, const short NbTimes) 
{
  try {
    GEOM::PointStruct d = myGeom->MakePointStruct(Dir.X(), Dir.Y(), Dir.Z());
    GEOM::DirStruct dstruct = myGeom->MakeDirection(d);

    GEOM::GEOM_Shape_var result = myGeom->MakeMultiTranslation1D(Shape, dstruct, Step, NbTimes);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
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


//=================================================================================
// function : MakeMultiTranslation2DAndDisplay()
// purpose  : Multi-Translate a shape
//=================================================================================
void TransformationGUI::MakeMultiTranslation2DAndDisplay(GEOM::GEOM_Shape_ptr Shape,
							 const gp_Dir Dir1,const double Step1, const short NbTimes1,
							 const gp_Dir Dir2, const double Step2, const short NbTimes2) 
{
  try {
    GEOM::PointStruct d1 = myGeom->MakePointStruct(Dir1.X(), Dir1.Y(), Dir1.Z());
    GEOM::DirStruct dstruct1 = myGeom->MakeDirection(d1);
    GEOM::PointStruct d2 = myGeom->MakePointStruct(Dir2.X(), Dir2.Y(), Dir2.Z());
    GEOM::DirStruct dstruct2 = myGeom->MakeDirection(d2);

    GEOM::GEOM_Shape_var result = myGeom->MakeMultiTranslation2D(Shape, dstruct1, Step1, NbTimes1,
								 dstruct2, Step2, NbTimes2);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
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


//=================================================================================
// function : MakeMultiRotation1DAndDisplay()
// purpose  : Multi-Rotate a shape
//=================================================================================
void TransformationGUI::MakeMultiRotation1DAndDisplay(GEOM::GEOM_Shape_ptr Shape, const gp_Dir Dir, 
						      const gp_Pnt Loc, const short NbTimes) 
{
  try {
    GEOM::PointStruct d = myGeom->MakePointStruct(Dir.X(), Dir.Y(), Dir.Z());
    GEOM::DirStruct dstruct = myGeom->MakeDirection(d) ;
    GEOM::PointStruct pstruct = myGeom->MakePointStruct(Loc.X(), Loc.Y(), Loc.Z());

    GEOM::GEOM_Shape_var result = myGeom->MakeMultiRotation1D(Shape, dstruct, pstruct, NbTimes);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
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


//=================================================================================
// function : MakeMultiRotation2DAndDisplay()
// purpose  : Multi-Rotate a shape
//=================================================================================
void TransformationGUI::MakeMultiRotation2DAndDisplay(GEOM::GEOM_Shape_ptr Shape, const gp_Dir Dir,
						      const gp_Pnt Loc, const double Ang, const short NbTimes1,
						const double Step, const short NbTimes2) 
{
  try {
    GEOM::PointStruct d = myGeom->MakePointStruct(Dir.X(), Dir.Y(), Dir.Z());
    GEOM::DirStruct dstruct = myGeom->MakeDirection(d);
    GEOM::PointStruct pstruct = myGeom->MakePointStruct(Loc.X(), Loc.Y(), Loc.Z());

    GEOM::GEOM_Shape_var result = myGeom->MakeMultiRotation2D(Shape, dstruct, pstruct, 
							      Ang, NbTimes1, Step, NbTimes2);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
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
  {return TransformationGUI::OnGUIEvent(theCommandID, parent);}
}
