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
//  File   : GEOMBase_Tools.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "GEOMToolsGUI.h"

#include "QAD_Config.h"
#include "QAD_FileDlg.h"
#include "QAD_Tools.h"
#include "QAD_MessageBox.h"
#include "QAD_RightFrame.h"

#include "OCCViewer_Viewer3d.h"
#include "VTKViewer_ViewFrame.h"

#include "SALOME_ListIteratorOfListIO.hxx"
#include "SALOMEGUI_NameDlg.h"
#include "SALOMEGUI_ImportOperation.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include <AIS_Drawer.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Prs3d_IsoAspect.hxx>

#include <vtkBMPReader.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTransformTextureCoords.h>
#include <vtkDataSetMapper.h>

#include <qfileinfo.h>
#include <qcolordialog.h>
#include <qspinbox.h>

#include "GEOMToolsGUI_NbIsosDlg.h"        // Method ISOS adjustement
#include "GEOMToolsGUI_TransparencyDlg.h"  // Method TRANSPARENCY adjustement

//=======================================================================
// function : GEOMToolsGUI()
// purpose  : Constructor
//=======================================================================
GEOMToolsGUI::GEOMToolsGUI() :
  QObject()
{
  myGeomBase = new GEOMBase();
  myGeomGUI = GEOMContext::GetGeomGUI();
  myGeom = myGeomGUI->myComponentGeom;
}


//=======================================================================
// function : ~GEOMToolsGUI()
// purpose  : Destructor
//=======================================================================
GEOMToolsGUI::~GEOMToolsGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool GEOMToolsGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  GEOMToolsGUI* myGEOMToolsGUI = new GEOMToolsGUI();
  myGEOMToolsGUI->myGeomGUI->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());

  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  switch (theCommandID)
    {
    case 31: // COPY
      {
	myGEOMToolsGUI->OnEditCopy();
	break;
      }
    case 33: // DELETE
      {
	myGEOMToolsGUI->OnEditDelete();
	break;
      }
    case 111: // IMPORT BREP
      {
	myGEOMToolsGUI->Import(theCommandID);
	break;
      }
    case 112: // IMPORT IGES
      {
	myGEOMToolsGUI->Import(theCommandID);
	break;
      }
    case 113: // IMPORT STEP
      {
	myGEOMToolsGUI->Import(theCommandID);
	break;
      }
    case 121: // EXPORT BREP
      {
	myGEOMToolsGUI->Export(theCommandID);
	break;
      }
    case 122: // EXPORT IGES
      {
	myGEOMToolsGUI->Export(theCommandID);
	break;
      }
    case 123: // EXPORT STEP
      {
	myGEOMToolsGUI->Export(theCommandID);
	break;
      }
    case 411: // SETTINGS - ADD IN STUDY
      {
	QMenuBar* Mb = QAD_Application::getDesktop()->getMainMenuBar();
	QMenuData* pp;
	QMenuItem* item = parent->menuBar()->findItem(411, &pp);
	bool check = !pp->isItemChecked(411);
	pp->setItemChecked(411,check);
	//myGEOMToolsGUI->myGeomGUI->GetSettings_AddInStudy() = check;
	QAD_CONFIG->addSetting("Geometry:SettingsAddInStudy", check);
	break;
      }
    case 412: // SETTINGS - SHADING COLOR
      {
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
	  break;

	OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

	QString SCr = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorRed");
	QString SCg = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorGreen");
	QString SCb = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorBlue");
	QColor color;
	if(!SCr.isEmpty() && !SCg.isEmpty() && !SCb.isEmpty()) {	  
	  color = QColor (SCr.toInt(), SCg.toInt(), SCb.toInt());
	} 
	else {
	  Quantity_Color Default = Quantity_Color();
	  color = QColor ((int)Default.Red()  * 255.0, (int)Default.Green()* 255.0, (int)Default.Blue() * 255.0);
	}
	
	QColor c = QColorDialog::getColor(color, QAD_Application::getDesktop());
	if(c.isValid()) {
	  //myGEOMToolsGUI->myGeomGUI->GetShadingColor() = Quantity_Color(c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0, Quantity_TOC_RGB);
	  
	  AIS_ListOfInteractive List;
	  ic->DisplayedObjects(List);
	  AIS_ListOfInteractive List1;
	  ic->ObjectsInCollector(List1);
	  List.Append(List1);
	  
	  AIS_ListIteratorOfListOfInteractive ite(List);
	  while(ite.More()) {
	    if(ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	      Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	      aSh->SetShadingColor(Quantity_Color(c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0, Quantity_TOC_RGB));
	      ic->Redisplay(aSh, Standard_True, Standard_True);
	    }
	    ite.Next();
	  }
	  
	  ic->UpdateCurrentViewer();
		
	  QAD_CONFIG->addSetting("Geometry:SettingsShadingColorRed", c.red());
	  QAD_CONFIG->addSetting("Geometry:SettingsShadingColorGreen", c.green());
	  QAD_CONFIG->addSetting("Geometry:SettingsShadingColorBlue", c.blue());	
	}
	break;
      }
    case 413: // SETTINGS - ISOS
      {
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
	  break;

	OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

	QString IsoU = QAD_CONFIG->getSetting("Geometry:SettingsIsoU");
	QString IsoV = QAD_CONFIG->getSetting("Geometry:SettingsIsoV");
	if(!IsoU.isEmpty())
	  ic->DefaultDrawer()->UIsoAspect()->SetNumber(IsoU.toInt());
	else
	  IsoU = "1";
	if (!IsoV.isEmpty())
	  ic->DefaultDrawer()->VIsoAspect()->SetNumber(IsoV.toInt());
	else
	  IsoV = "1";
	
	GEOMBase_NbIsosDlg* NbIsosDlg = new GEOMBase_NbIsosDlg(QAD_Application::getDesktop(), tr("GEOM_MEN_ISOS"), TRUE);	
	int UIso = IsoU.toInt();
	int VIso = IsoV.toInt();
	
	NbIsosDlg->SpinBoxU->setValue(UIso);
	NbIsosDlg->SpinBoxV->setValue(VIso);

	if(NbIsosDlg->exec()) {
	  UIso = NbIsosDlg->SpinBoxU->text().toInt();
	  VIso = NbIsosDlg->SpinBoxV->text().toInt();
	  
	  ic->DefaultDrawer()->UIsoAspect()->SetNumber(UIso);
	  ic->DefaultDrawer()->VIsoAspect()->SetNumber(VIso);
	  QAD_CONFIG->addSetting("Geometry:SettingsIsoU", NbIsosDlg->SpinBoxU->text()); /* text format */
	  QAD_CONFIG->addSetting("Geometry:SettingsIsoV", NbIsosDlg->SpinBoxV->text()); /* text format */
	}

	AIS_ListOfInteractive List;
	ic->DisplayedObjects(List);
	AIS_ListOfInteractive List1;
	ic->ObjectsInCollector(List1);
	List.Append(List1);
	
	AIS_ListIteratorOfListOfInteractive ite(List);
	ic->InitCurrent();
	if(ic->NbCurrents())
	  while(ite.More()) {
	    if(ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	      Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	      Handle (AIS_Drawer) CurDrawer;
	      CurDrawer = aSh->Attributes();
	      CurDrawer->UIsoAspect()->SetNumber(UIso);
	      CurDrawer->VIsoAspect()->SetNumber(VIso);
	      ic->SetLocalAttributes(aSh, CurDrawer);
	      ic->Redisplay(aSh);	
	    }
	    ite.Next();
	  }
	ic->UpdateCurrentViewer();
	break;
      }
    case 414: // SETTINGS : STEP VALUE FOR SPIN BOXES
      {
	QString step = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
	if(step.isEmpty())
	  step = "100.0";

	Standard_Boolean res = false;
	double dd = myGEOMToolsGUI->myGeomBase->Parameter(res, QString("%1").arg(step), tr("GEOM_MEN_STEP_LABEL"), tr("GEOM_STEP_TITLE"), 0.001, 10000.0, 3);
	if(res) {
	  QAD_CONFIG->addSetting("Geometry:SettingsGeomStep", QString("%1").arg(dd));

	  /* Emit signal to GeometryGUI_SpinBoxes */
	  myGEOMToolsGUI->myGeomGUI->EmitSignalDefaultStepValueChanged(dd);
	}
	else
	  parent->putInfo(tr("GEOM_PRP_ABORT"));
	break;
      }
    case 804: // ADD IN STUDY - POPUP VIEWER
      {
	const SALOME_ListIO& ListSelected = Sel->StoredIObjects();
	SALOME_ListIteratorOfListIO It(ListSelected);
	for(; It.More(); It.Next()) {
	  Handle(SALOME_InteractiveObject) IObject = It.Value();
	  myGEOMToolsGUI->myGeomBase->AddInStudy(true, IObject);
	}
	/* Is set on the dialog box */
	QApplication::restoreOverrideCursor();
	break;
      }
    case 901: // RENAME
      {
	SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
	for(; It.More(); It.Next()) {
	  Handle(SALOME_InteractiveObject) IObject = It.Value();

	  SALOMEDS::SObject_var obj = aStudy->FindObjectID(IObject->getEntry());
          SALOMEDS::GenericAttribute_var anAttr;
          SALOMEDS::AttributeName_var aName;
	  if(!obj->_is_nil()) {
	    if(obj->FindAttribute(anAttr, "AttributeName")) {
              aName = SALOMEDS::AttributeName::_narrow(anAttr);

	      QString nm = QString(aName->Value());
	      nm = SALOMEGUI_NameDlg::getName(QAD_Application::getDesktop(), nm);
	      if(!nm.isEmpty()) {
		QApplication::setOverrideCursor(Qt::waitCursor);
		QAD_Application::getDesktop()->getActiveStudy()->renameIObject(IObject, nm);
		QApplication::restoreOverrideCursor();
	      }
	    }
	  }
	}
	break;
      }
    case 5103: // CHECK GEOMETRY
      {
	QAD_PyEditor* PyEditor = QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getPyEditor();
	PyEditor->setText("from GEOM_usinggeom import *\n");
	PyEditor->setText(">>> ");
	PyEditor->handleReturn();
	break;
      }
    case 5104: // LOAD SCRIPT
      {
	QStringList filtersList;
	filtersList.append(tr("GEOM_MEN_LOAD_SCRIPT"));
	filtersList.append(tr("GEOM_MEN_ALL_FILES"));

	QString aFile = QAD_FileDlg::getFileName(QAD_Application::getDesktop(), "", filtersList, tr("GEOM_MEN_IMPORT"), true);
	if(!aFile.isEmpty()) {
	  QFileInfo file = aFile;
	  QApplication::setOverrideCursor(Qt::waitCursor);
	  QAD_PyEditor* PyEditor = QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getPyEditor();

	  PyEditor->setText("import geompy\n");
	  PyEditor->handleReturn();

	  QStringList aTextList = QStringList::split(".", file.fileName());
	  PyEditor->setText("geompy.Path('" + file.dirPath() + "')\n");
	  PyEditor->handleReturn();

	  PyEditor->setText("from " + aTextList.first() + " import *\n");
	  PyEditor->handleReturn();
	}
	QApplication::restoreOverrideCursor();
	break;
      }
    case 8032: // COLOR - POPUP VIEWER
      {
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
	  // VTK
	  VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
	  
	  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
	  Handle(SALOME_InteractiveObject) FirstIOS =  Sel->firstIObject();
	  if(!FirstIOS.IsNull()) {
	    QColor initcolor = myRenderInter->GetColor(FirstIOS);
	    QColor c = QColorDialog::getColor( initcolor, QAD_Application::getDesktop());
	    
	    if(c.isValid()) {
	      QApplication::setOverrideCursor(Qt::waitCursor);
	      for(;It.More();It.Next()) {
		Handle(SALOME_InteractiveObject) IOS = It.Value();
		myRenderInter->SetColor(IOS,c);
	      }
	    }
	    QApplication::restoreOverrideCursor();
	  }
	}
	else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  Handle(SALOME_InteractiveObject) IO = Sel->firstIObject();
	  Standard_Boolean found;
	  Handle(GEOM_AISShape) Shape = myGEOMToolsGUI->myGeomBase->ConvertIOinGEOMAISShape(IO, found, true);
	  if(found) {
	    Quantity_Color CSFColor;
	    Shape->Color(CSFColor);
	    
	    QColor c = QColorDialog::getColor(QColor(CSFColor.Red()  * 255.0, CSFColor.Green()* 255.0, CSFColor.Blue() * 255.0), QAD_Application::getDesktop());
	    
	    if(c.isValid()) {
	      CSFColor = Quantity_Color (c.red()/255., c.green()/255., c.blue()/255., Quantity_TOC_RGB);
	      QApplication::setOverrideCursor( Qt::waitCursor );
	    
	      SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
	      for(;It.More();It.Next()) {
		Handle(SALOME_InteractiveObject) IObject = It.Value();
		Standard_Boolean found;
		Handle(GEOM_AISShape) Shape = myGEOMToolsGUI->myGeomBase->ConvertIOinGEOMAISShape(IObject, found, true);
		if(found) {
		  Shape->SetColor(CSFColor);
		  Shape->SetShadingColor(CSFColor);
		}
	      }
	    }
	  }
	}
	QApplication::restoreOverrideCursor();
	break;
      }
    case 8033: // TRANSPARENCY - POPUP VIEWER
      {
	OCCViewer_Viewer3d* v3d;
	Handle(AIS_InteractiveContext) ic;
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC) {
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
	}
	GEOMBase_TransparencyDlg *aDlg = new GEOMBase_TransparencyDlg(parent, "", Sel, ic);
	break;
      }
    case 8034: // ISOS - POPUP VIEWER
      {
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
	  break;

	OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

	ic->InitCurrent();
	if(ic->MoreCurrent()) {
	  Handle(GEOM_AISShape) CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());
	  QString IsoU = QAD_CONFIG->getSetting("Geometry:SettingsIsoU");
	  QString IsoV = QAD_CONFIG->getSetting("Geometry:SettingsIsoV");

	  if(!IsoU.isEmpty())
	    ic->DefaultDrawer()->UIsoAspect()->SetNumber(IsoU.toInt());
	  else
	    IsoU = "1";
	  if (!IsoV.isEmpty())
	    ic->DefaultDrawer()->VIsoAspect()->SetNumber(IsoV.toInt());
	  else
	    IsoV = "1";
	    
	  GEOMBase_NbIsosDlg * NbIsosDlg =
	    new GEOMBase_NbIsosDlg(QAD_Application::getDesktop(), tr("GEOM_MEN_ISOS"), TRUE);
    
	  NbIsosDlg->SpinBoxU->setValue(IsoU.toInt());
	  NbIsosDlg->SpinBoxV->setValue(IsoV.toInt());

	  if(NbIsosDlg->exec()) {
	    QApplication::setOverrideCursor(Qt::waitCursor);
	    for(; ic->MoreCurrent(); ic->NextCurrent()) {
	      Handle(AIS_Drawer) CurDrawer;
	      
	      CurDrawer = ic->Current()->Attributes();
	      CurDrawer->UIsoAspect()->SetNumber(NbIsosDlg->SpinBoxU->text().toInt());
	      CurDrawer->VIsoAspect()->SetNumber(NbIsosDlg->SpinBoxV->text().toInt());
		
	      ic->SetLocalAttributes(CurObject, CurDrawer);
	      ic->Redisplay(CurObject);
	    }
	  }
	}
	QApplication::restoreOverrideCursor();
	break;
      }
    case 9024 : // OPEN - OBJBROSER POPUP
      {
	SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
	Handle(SALOME_InteractiveObject) anIObject;
	for(;It.More();It.Next()) {
	  anIObject = It.Value();
	  SALOMEDS::SObject_var obj = aStudy->FindObjectID(anIObject->getEntry());
	  SALOMEDS::AttributePersistentRef_var aPersist;
	  SALOMEDS::AttributeIOR_var anIOR;
	  if(!obj->_is_nil()) {
	    // this SObject may be GEOM module root SObject
	    SALOMEDS::ChildIterator_var anIter = aStudy->NewChildIterator(obj);
	    SALOMEDS::GenericAttribute_var anAttr;
	    bool useSubItems = false;
	    while (anIter->More() && !useSubItems) {
	      SALOMEDS::SObject_var subobj = anIter->Value();
	      if (subobj->FindAttribute(anAttr, "AttributePersistentRef")) {
		useSubItems = true;
		obj = subobj;
	      }
	      else 
		anIter->Next();
	    }
	    obj->FindAttribute(anAttr, "AttributePersistentRef");
	      
	    while(useSubItems?anIter->More():!anAttr->_is_nil()) { 
	      if(!obj->FindAttribute(anAttr, "AttributeIOR") &&
		  obj->FindAttribute(anAttr, "AttributePersistentRef")) {
		// load
		Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer","GEOM");
		if (!CORBA::is_nil(comp)) {
		  SALOMEDS::Driver_var driver = SALOMEDS::Driver::_narrow(comp);
		  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
		  aStudyBuilder->LoadWith(aStudy->FindComponent("GEOM"),driver);
		} 
		else {
		  MESSAGE("Component is null");
		}
	      }
	      if(useSubItems) {
		anIter->Next();
		obj = anIter->Value();
	      } 
	      else 
		anAttr = SALOMEDS::GenericAttribute::_nil();
	    }
	  }
	}
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


//===============================================================================
// function : OnEditDelete()
// purpose  :
//===============================================================================
void GEOMToolsGUI::OnEditDelete()
{
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  if(Sel->IObjectCount() == 0)
    return;

  if(QAD_MessageBox::warn2(QAD_Application::getDesktop(), tr("GEOM_WRN_WARNING"), tr("GEOM_REALLY_DELETE"), tr("GEOM_BUT_YES"), tr("GEOM_BUT_NO"), 1, 0, 0) != 1)
    return;
       
  int nbSf = QAD_Application::getDesktop()->getActiveStudy()->getStudyFramesCount();
    
  Standard_Boolean found;
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeIOR_var anIOR;
  
  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
  for(;It.More();It.Next()) {
    Handle(SALOME_InteractiveObject) IObject = It.Value();
    if(IObject->hasEntry()) {
      SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
      SALOMEDS::SObject_var SO = aStudy->FindObjectID(IObject->getEntry());
      
      /* Erase child graphical objects */
      SALOMEDS::ChildIterator_var it = aStudy->NewChildIterator(SO);
      for(; it->More();it->Next()) {
	SALOMEDS::SObject_var CSO= it->Value();
	if(CSO->FindAttribute(anAttr, "AttributeIOR") ) {
          anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	  /* Delete child(s) shape in Client : */
	  const TCollection_AsciiString ASCior(anIOR->Value()) ;
	  myGeomGUI->GetShapeReader().RemoveShapeFromBuffer(ASCior);

	  for(int i = 0; i < nbSf; i++) {
	    QAD_StudyFrame* sf = QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame(i);
	    if(sf->getTypeView() == VIEW_OCC) {
	      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getViewer();
	      Handle (AIS_InteractiveContext) myContext = v3d->getAISContext();
	      Handle(GEOM_AISShape) Result = myGeomBase->ConvertIORinGEOMAISShape(anIOR->Value(), found);
	      if(found)
		myContext->Erase(Result, true, false);
	    }
	    else if(sf->getTypeView() == VIEW_VTK) {
	      //vtkRenderer* Renderer = ((VTKViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getRenderer();
	      VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getRWInteractor();
	      GEOM_Actor* ac = myGeomBase->ConvertIORinGEOMActor(anIOR->Value(), found);
	      if(found) {
		//Renderer->RemoveActor(ac);
		if(ac->hasIO()) 
		  myRenderInter->Remove(ac->getIO());
	      }
	    }
	  }
	}
      }
      
      /* Erase main graphical object */
      for(int i = 0; i < nbSf; i++) {
	QAD_StudyFrame* sf = QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame(i);
	if(sf->getTypeView() == VIEW_OCC) {
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getViewer();
	  Handle(AIS_InteractiveContext) myContext = v3d->getAISContext();
	  Handle(GEOM_AISShape) Result = myGeomBase->ConvertIOinGEOMAISShape(IObject, found );
	  if(found)
	    myContext->Erase(Result, true, false);
	}
	else if(sf->getTypeView() == VIEW_VTK) {
	  VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getRWInteractor();
	  myRenderInter->Remove( IObject );
	}
      }
      
      /* Delete main shape in Client : */
      if(SO->FindAttribute(anAttr, "AttributeIOR")) {
        anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	const TCollection_AsciiString ASCIor(anIOR->Value()) ;
	myGeomGUI->GetShapeReader().RemoveShapeFromBuffer(ASCIor);
      }

      /* Erase objects in Study */
      SALOMEDS::SObject_var obj = aStudy->FindObjectID(IObject->getEntry());
      if(!obj->_is_nil()) {
	QAD_Operation* op = new SALOMEGUI_ImportOperation(QAD_Application::getDesktop()->getActiveStudy());
	op->start();
	aStudyBuilder->RemoveObject(obj);
	op->finish();
      }

    } /* IObject->hasEntry() */
  }   /* more/next           */

  /* Clear any previous selection */
  Sel->ClearIObjects(); 
  QAD_Application::getDesktop()->getActiveStudy()->updateObjBrowser();
}


//==============================================================================
// function : OnEditCopy()
// purpose  :
//==============================================================================
void GEOMToolsGUI::OnEditCopy()
{
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection() );
  GEOM::GEOM_Gen::ListOfIOR_var listIOR = new GEOM::GEOM_Gen::ListOfIOR;

  const SALOME_ListIO& List = Sel->StoredIObjects();

  myGeomBase->ConvertListOfIOInListOfIOR(List, listIOR);

  Sel->ClearIObjects();

  for (unsigned int ind = 0; ind < listIOR->length();ind++) {
    GEOM::GEOM_Shape_var aShapeInit = myGeom->GetIORFromString(listIOR[ind]);
    try {
      GEOM::GEOM_Shape_var result = myGeom->MakeCopy(aShapeInit) ;
      result->NameType( aShapeInit->NameType() );
      myGeomBase->Display(result);
    }
    catch  (const SALOME::SALOME_Exception& S_ex) {
      QtCatchCorbaException(S_ex);
    }
  }
  
  QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_READY"));
}


//=====================================================================================
// function : Import
// purpose  : BRep, Iges, Step
//=====================================================================================
bool GEOMToolsGUI::Import(int aState)
{
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  
  GEOM::GEOM_Shape_var aShape;
  QString file;
  QStringList filtersList;
  
  switch(aState)
    {
    case 111 :	// Import BREP
      {
	filtersList.append(tr("GEOM_MEN_IMPORT_BREP"));
	filtersList.append(tr("GEOM_MEN_ALL_FILES"));

	file = QAD_FileDlg::getFileName(QAD_Application::getDesktop(), "", filtersList, tr("GEOM_MEN_IMPORT"), true);
	if(!file.isEmpty()) {
	  QApplication::setOverrideCursor(Qt::waitCursor);
	  try {
	    aShape = myGeom->ImportBREP(file.latin1());
	  }
	  catch(const SALOME::SALOME_Exception& S_ex) {
	    QtCatchCorbaException(S_ex);
	  }
	}
	break;
      }
    case 112 :	// Import IGES
      {
	filtersList.append( tr("GEOM_MEN_IMPORT_IGES") ) ;
	filtersList.append( tr("GEOM_MEN_ALL_FILES") ) ;

	file = QAD_FileDlg::getFileName(QAD_Application::getDesktop(),
					"",
					filtersList,
					tr("GEOM_MEN_IMPORT"),
					true);
	if ( !file.isEmpty() ) {
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  try {
	    aShape = myGeom->ImportIGES( file.latin1() );
	  }
	  catch (const SALOME::SALOME_Exception& S_ex) {
	    QtCatchCorbaException(S_ex);
	  }
	}
	break;
      }
    case 113 : // Import STEP
      {
	filtersList.append( tr("GEOM_MEN_IMPORT_STEP") ) ;
	filtersList.append( tr("GEOM_MEN_ALL_FILES") ) ;

	file = QAD_FileDlg::getFileName(QAD_Application::getDesktop(),
					"",
					filtersList,
					tr("GEOM_MEN_IMPORT"),
					true);
	if ( !file.isEmpty() ) {
	  QApplication::setOverrideCursor( Qt::waitCursor );
	  try {
	    aShape = myGeom->ImportSTEP( file.latin1() );
	  }
	  catch  (const SALOME::SALOME_Exception& S_ex) {
	    QtCatchCorbaException(S_ex);
	  }
	}
	break;
      }
    }
  
  if ( !file.isEmpty() ) {
    QAD_Application::getDesktop()->putInfo( tr("GEOM_PRP_LOADING").arg(QAD_Tools::getFileNameFromPath( file )) );

    SALOMEDS::SComponent_var father = aStudy->FindComponent("GEOM");
    SALOMEDS::SObject_var fatherSF = aStudy->FindObjectID(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->entry());
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeName_var    aName;
    SALOMEDS::AttributePixMap_var  aPixmap;
    int aLocked = false;
    if (father->_is_nil()) {
      QAD_Operation* op = new SALOMEGUI_ImportOperation(QAD_Application::getDesktop()->getActiveStudy() );
      op->start();
      aLocked = aStudy->GetProperties()->IsLocked();
      if (aLocked) aStudy->GetProperties()->SetLocked(false);
      father = aStudyBuilder->NewComponent("GEOM");
      anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributeName");
      aName = SALOMEDS::AttributeName::_narrow(anAttr);
      //      aName->SetValue( tr("GEOM_MEN_COMPONENT") );
      aName->SetValue(QAD_Application::getDesktop()->getComponentUserName( "GEOM" ) );
      anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributePixMap");
      aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
      aPixmap->SetPixMap( "ICON_OBJBROWSER_Geometry" );
      if (aLocked) aStudy->GetProperties()->SetLocked(true);
      op->finish();
    }
//      if (aLocked) return false;
    aStudyBuilder->DefineComponentInstance( father, myGeom );
    father->ComponentIOR(myGeomGUI->GetFatherior());

    QString nameShape = QAD_Tools::getFileNameFromPath(file,false) +  QString("_%1").arg(myGeomGUI->GetNbGeom()++);

    if(myGeomBase->Display(aShape, strdup(nameShape.latin1()))) {
      QAD_Application::getDesktop()->getActiveStudy()->setMessage( tr("GEOM_INF_LOADED").arg(QAD_Tools::getFileNameFromPath( file )) );
      QAD_Application::getDesktop()->putInfo( tr("GEOM_PRP_READY"));
    }
  }
  QApplication::restoreOverrideCursor();
  return true;
}


//=====================================================================================
// function : Export
// purpose  : BRep, Iges, Step
//=====================================================================================
bool GEOMToolsGUI::Export(int aState)
{
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();

  GEOM::GEOM_Shape_var aShape;

  static QString filters[] = { tr("GEOM_MEN_IMPORT_BREP"),
			       tr("GEOM_MEN_IMPORT_IGES"),
			       tr("GEOM_MEN_IMPORT_STEP") };
  
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );

  switch (aState)
    {
    case 121 :
      {
	for(;It.More();It.Next()) {
	  QApplication::restoreOverrideCursor();
	  Handle(SALOME_InteractiveObject) IObject = It.Value();
	  Standard_Boolean found;
	  GEOM::GEOM_Shape_var aShape = myGeomBase->ConvertIOinGEOMShape(IObject, found);
	  //	  Handle(GEOM_AISShape) Shape = myGeomGUI->ConvertIOinGEOMAISShape(IObject, found, true);
	  if ( found ) {
	    QString file = QAD_FileDlg::getFileName(QAD_Application::getDesktop(), 
						    QString( IObject->getName() ) + ".brep",
						    tr("GEOM_MEN_IMPORT_BREP"),
						    tr("GEOM_MEN_EXPORT"),
						    false);
	    if ( !file.isEmpty() && !aShape->_is_nil() ) {
	      QApplication::setOverrideCursor( Qt::waitCursor );
	      //      Standard_Boolean result = BRepTools::Write(Shape->Shape(), strdup(file.latin1()) );
	      try {
		myGeom->ExportBREP(strdup( file.latin1()), aShape);
	      }  
	      catch (const SALOME::SALOME_Exception& S_ex) {
		QtCatchCorbaException(S_ex);
	      }
	    }
	  }
	}
     	break;
      }
    case 122 :
      {
	for(;It.More();It.Next()) {
	  QApplication::restoreOverrideCursor();
	  Handle(SALOME_InteractiveObject) IObject = It.Value();
	  Standard_Boolean found;
	  GEOM::GEOM_Shape_var aShape = myGeomBase->ConvertIOinGEOMShape(IObject, found);
	  //	  Handle(GEOM_AISShape) Shape = myGeomGUI->ConvertIOinGEOMAISShape(IObject, found, true);
	  if ( found ) {
	    QString file = QAD_FileDlg::getFileName(QAD_Application::getDesktop(), 
						    QString( IObject->getName() ) + ".igs",
						    tr("GEOM_MEN_IMPORT_IGES"),
						    tr("GEOM_MEN_EXPORT"),
						    false);
	    if ( !file.isEmpty() && !aShape->_is_nil() ) {
	      QApplication::setOverrideCursor( Qt::waitCursor );
	      try {
		myGeom->ExportIGES(strdup( file.latin1()), aShape);
	      }  
	      catch (const SALOME::SALOME_Exception& S_ex) {
		QtCatchCorbaException(S_ex);
	      }
//  	      //VRV: OCC 4.0 migration
//  	      IGESControl_Controller::Init();
//  	      IGESControl_Writer ICW (Interface_Static::CVal("XSTEP.iges.unit"),
//  					 Interface_Static::IVal("XSTEP.iges.writebrep.mode"));
//  	      //VRV: OCC 4.0 migration

//  	      ICW.AddShape (Shape->Shape());
//  	      ICW.ComputeModel();
//  	      Standard_Boolean result = ICW.Write( strdup(file.latin1()) );
	    }
	  }
	}
	break;
      }

    case 123 :
      {
//  	bool test = false ;
//  	IFSelect_ReturnStatus status ;
//  	//VRV: OCC 4.0 migration
//  	STEPControl_Writer aWriter;
//  	//VRV: OCC 4.0 migration
	QString file;

	for( ; It.More(); It.Next() ) {
//  	  GEOM::GEOM_Shape_var aShape = myGeomBase->ConvertIOinGEOMShape(IObject, found);
  	  QApplication::restoreOverrideCursor();
	  Handle(SALOME_InteractiveObject) IObject = It.Value();
	  Standard_Boolean found;
	  GEOM::GEOM_Shape_var aShape = myGeomBase->ConvertIOinGEOMShape(IObject, found);
	  //	  Handle(GEOM_AISShape) Shape = myGeomBase->ConvertIOinGEOMAISShape(IObject, found, true);
	  if ( found ) {
	    file = QAD_FileDlg::getFileName(QAD_Application::getDesktop(), 
					    QString( IObject->getName() ) + ".stp",
					    tr("GEOM_MEN_IMPORT_STEP"),
					    tr("GEOM_MEN_EXPORT"),
					    false);
	    if ( !file.isEmpty() && !aShape->_is_nil() ) {

	      QApplication::setOverrideCursor( Qt::waitCursor ) ;	
	      try {   
		myGeom->ExportSTEP(strdup( file.latin1()), aShape);   
	      }  
	      catch (const SALOME::SALOME_Exception& S_ex) {
		QtCatchCorbaException(S_ex);
	      }
//  	      //VRV: OCC 4.0 migration
//  	      status = aWriter.Transfer( Shape->Shape(), STEPControl_ManifoldSolidBrep ) ;
//  	      //VRV: OCC 4.0 migration
//  	      test = true ;
//  	      if ( status != IFSelect_RetDone ) {
//  		QApplication::restoreOverrideCursor() ;
//  		return false ;
//  	      }
	    }
	  }
	}
//  	if(test) {
//  	  status = aWriter.Write( strdup(file.latin1()) ) ;
//  	  QApplication::restoreOverrideCursor() ;
//  	  return status ;
//  	}
	break;
      }

    }
  QApplication::restoreOverrideCursor() ;
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  bool OnGUIEvent(int theCommandID, QAD_Desktop* parent)
  {return GEOMToolsGUI::OnGUIEvent(theCommandID, parent);}
}
