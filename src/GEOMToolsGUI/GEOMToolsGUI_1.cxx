//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2004  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org 
//
//            
//
//  File   : GEOMToolsGUI_1.cxx
//  Author : Sergey ANIKIN
//  Module : GEOM
//  $Header$

#include "GEOMToolsGUI.h"
#include "GeometryGUI.h"
#include "GEOM_Actor.h"
#include "GEOMBase.h"

#include "QAD_Desktop.h"
#include "QAD_Config.h"
#include "QAD_FileDlg.h"
#include "QAD_PyEditor.h"
#include "QAD_RightFrame.h"
#include "QAD_MessageBox.h"
#include "SALOMEGUI_NameDlg.h"

#include "OCCViewer_Viewer3d.h"
#include "OCCViewer_ViewFrame.h"
#include "VTKViewer_ViewFrame.h"
#include "VTKViewer_RenderWindowInteractor.h"

#include "SALOME_ListIO.hxx"
#include "SALOME_ListIteratorOfListIO.hxx"

#include <AIS_Drawer.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Prs3d_IsoAspect.hxx>

#include <vtkBMPReader.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTransformTextureCoords.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>

#include <qfileinfo.h>
#include <qcolordialog.h>
#include <qspinbox.h>
#include <qapplication.h>

#include "GEOMToolsGUI_NbIsosDlg.h"        // Method ISOS adjustement
#include "GEOMToolsGUI_TransparencyDlg.h"  // Method TRANSPARENCY adjustement

#include "utilities.h"

using namespace std;

void GEOMToolsGUI::OnSettingsColor()
{
  
  QString aSCr = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorRed");
  QString aSCg = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorGreen");
  QString aSCb = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorBlue");
  QColor anInitColor;
  if(!aSCr.isEmpty() && !aSCg.isEmpty() && !aSCb.isEmpty()) {	  
    anInitColor = QColor (aSCr.toInt(), aSCg.toInt(), aSCb.toInt());
  } 
  else {
    Quantity_Color aDefColor = Quantity_Color();
    anInitColor = 
      QColor ((int)( aDefColor.Red() * 255.0 ), (int)( aDefColor.Green() * 255.0 ), (int)( aDefColor.Blue() * 255.0 ) );
  }
  
  QColor aDialogColor = QColorDialog::getColor(anInitColor, QAD_Application::getDesktop());
  if(aDialogColor.isValid()) {
    QApplication::setOverrideCursor(Qt::waitCursor);
    
    if( QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() != VIEW_OCC &&
	QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() != VIEW_VTK )
      MESSAGE("Settings Color is not supported for current Viewer");
    
    QAD_CONFIG->addSetting("Geometry:SettingsShadingColorRed", aDialogColor.red());
    QAD_CONFIG->addSetting("Geometry:SettingsShadingColorGreen", aDialogColor.green());
    QAD_CONFIG->addSetting("Geometry:SettingsShadingColorBlue", aDialogColor.blue());	
    QApplication::restoreOverrideCursor();
  }
}

void GEOMToolsGUI::OnSettingsIsos()
{
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return;
  
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
}

void GEOMToolsGUI::OnSettingsStep()
{
  QString step = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  if(step.isEmpty())
    step = "100.0";
  
  Standard_Boolean res = false;
  double dd = GEOMBase::Parameter(res, QString("%1").arg(step), tr("GEOM_MEN_STEP_LABEL"), tr("GEOM_STEP_TITLE"), 0.001, 10000.0, 3);
  if(res) {
    QAD_CONFIG->addSetting("Geometry:SettingsGeomStep", QString("%1").arg(dd));
    
    /* Emit signal to GeometryGUI_SpinBoxes */
    GeometryGUI::GetGeomGUI()->EmitSignalDefaultStepValueChanged(dd);
  }
  else
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
}

void GEOMToolsGUI::OnRename()
{
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  
  bool aLocked = aStudy->GetProperties()->IsLocked();
  if ( aLocked ) {
    QAD_MessageBox::warn1 ( (QWidget*)QAD_Application::getDesktop(),
			    QObject::tr("WRN_WARNING"), 
			    QObject::tr("WRN_STUDY_LOCKED"),
			    QObject::tr("BUT_OK") );
    return;
  }
  
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
}

void GEOMToolsGUI::OnCheckGeometry()
{
  QAD_PyEditor* PyEditor = QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getPyEditor();
  PyEditor->setText("from GEOM_usinggeom import *\n");
  PyEditor->handleReturn();
}

void GEOMToolsGUI::OnLoadScript()
{
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  bool aLocked = aStudy->GetProperties()->IsLocked();
  if ( aLocked ) {
    QAD_MessageBox::warn1 ( (QWidget*)QAD_Application::getDesktop(),
			    QObject::tr("WRN_WARNING"), 
			    QObject::tr("WRN_STUDY_LOCKED"),
			    QObject::tr("BUT_OK") );
    return;
  }
  
  QStringList filtersList;
  filtersList.append(tr("GEOM_MEN_LOAD_SCRIPT"));
  filtersList.append(tr("GEOM_MEN_ALL_FILES"));
  
  QString aFile = QAD_FileDlg::getFileName(QAD_Application::getDesktop(), "", filtersList, tr("GEOM_MEN_IMPORT"), true);
  if(!aFile.isEmpty()) {
    QFileInfo file = aFile;
    QApplication::setOverrideCursor(Qt::waitCursor);
    QAD_PyEditor* PyEditor = QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getPyEditor();
    
    QStringList aTextList = QStringList::split(".", file.fileName());
    
    PyEditor->setText("import geompy; geompy.addPath('" + file.dirPath() + "'); from " + aTextList.first() + " import *\n" );
    PyEditor->handleReturn();
  }
  QApplication::restoreOverrideCursor();
}

void GEOMToolsGUI::OnColor()
{
  SALOME_Selection* Sel = SALOME_Selection::Selection(
    QAD_Application::getDesktop()->getActiveStudy()->getSelection() );

  if ( QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK )
  {
    VTKViewer_RenderWindowInteractor* myRenderInter =
      ( (VTKViewer_ViewFrame*)QAD_Application::getDesktop()->
         getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
    
    SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
    Handle(SALOME_InteractiveObject) FirstIOS =  Sel->firstIObject();
    if ( !FirstIOS.IsNull() )
    {
      QColor initcolor = myRenderInter->GetColor(FirstIOS);
      QColor c = QColorDialog::getColor( initcolor, QAD_Application::getDesktop() );
      
      if ( c.isValid() )
      {
        QApplication::setOverrideCursor( Qt::waitCursor );
        for ( ;It.More();It.Next() )
        {
          Handle(SALOME_InteractiveObject) IOS = It.Value();
          myRenderInter->SetColor(IOS,c);
        }
      }
      QApplication::restoreOverrideCursor();
    }
  }
  else if( QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC )
  {
    Handle(AIS_InteractiveObject) anIO = GEOMBase::GetAIS( Sel->firstIObject() );
    if ( !anIO.IsNull() )
    {
      Quantity_Color aColor;
      anIO->Color( aColor );
      
      QColor qColor =  QColorDialog::getColor( QColor( (int)( aColor.Red() * 255.0 ),
                                                       (int)( aColor.Green() * 255.0 ),
                                                       (int)( aColor.Blue() * 255.0 ) ),
                                               QAD_Application::getDesktop());
      
      if ( qColor.isValid() )
      {
        aColor = Quantity_Color( qColor.red() / 255.,
                                 qColor.green() / 255.,
                                 qColor.blue() / 255.,
                                 Quantity_TOC_RGB );
        
        QApplication::setOverrideCursor( Qt::waitCursor );
        
        SALOME_ListIteratorOfListIO anIter ( Sel->StoredIObjects() );
        for( ;anIter.More();anIter.Next() )
        {
          Handle(SALOME_InteractiveObject) IObject = anIter.Value();
          Handle(AIS_InteractiveObject) anIO = GEOMBase::GetAIS( IObject, true );
          if ( !anIO.IsNull() )
          {
            anIO->SetColor( aColor );

            if ( anIO->IsKind( STANDARD_TYPE(GEOM_AISShape) ) )
              Handle(GEOM_AISShape)::DownCast( anIO )->SetShadingColor( aColor );

            anIO->Redisplay( Standard_True );
          }
        }
      }
    }
  }
  
  QApplication::restoreOverrideCursor();
}

void GEOMToolsGUI::OnTransparency()
{
  Handle(AIS_InteractiveContext) ic;
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    ic = v3d->getAISContext();
  }
  new GEOMBase_TransparencyDlg(QAD_Application::getDesktop(), "", Sel, ic);
}

void GEOMToolsGUI::OnNbIsos()
{
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return;
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  
  ic->InitCurrent();
  if(ic->MoreCurrent()) {
    Handle(GEOM_AISShape) CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());
    Handle(AIS_Drawer)    CurDrawer = CurObject->Attributes();
    
    int UIso = CurDrawer->UIsoAspect()->Number();
    int VIso = CurDrawer->VIsoAspect()->Number();
    
    GEOMBase_NbIsosDlg * NbIsosDlg =
      new GEOMBase_NbIsosDlg(QAD_Application::getDesktop(), tr("GEOM_MEN_ISOS"), TRUE);
    
    NbIsosDlg->SpinBoxU->setValue( UIso );
    NbIsosDlg->SpinBoxV->setValue( VIso );
    
    if(NbIsosDlg->exec()) {
      QApplication::setOverrideCursor(Qt::waitCursor);
      for(; ic->MoreCurrent(); ic->NextCurrent()) {
        CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());
	Handle(AIS_Drawer) CurDrawer = CurObject->Attributes();
	
	int nbUIso = NbIsosDlg->SpinBoxU->text().toInt();
	int nbVIso = NbIsosDlg->SpinBoxV->text().toInt();
	
	CurDrawer->SetUIsoAspect( new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 0.5 , nbUIso) );
	CurDrawer->SetVIsoAspect( new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 0.5 , nbVIso) );
	
	ic->SetLocalAttributes(CurObject, CurDrawer);
	ic->Redisplay(CurObject);
      }
    }
  }
  QApplication::restoreOverrideCursor();
}

void GEOMToolsGUI::OnOpen()
{
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();

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
	  SALOMEDS::SComponent_var FComp = obj->GetFatherComponent();
	  if (!CORBA::is_nil(FComp)) {
	    if (FComp->FindAttribute(anAttr, "AttributeName")) {
	      SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(anAttr);
	      QString compName = QAD_Application::getDesktop()->getComponentName(aName->Value());
	      //		    parent->loadComponentData(parent->getComponentName(aName->Value()));
	      Engines::Component_var comp ;
	      if ( compName.compare("SUPERV") == 0 ) {
		comp = QAD_Application::getDesktop()->getEngine( "SuperVisionContainer", compName) ;
	      }
	      else {
		comp = QAD_Application::getDesktop()->getEngine( "FactoryServer", compName);
		if ( comp->_is_nil() )
		  comp = QAD_Application::getDesktop()->getEngine( "FactoryServerPy", compName);
	      }

	      if (!CORBA::is_nil(comp)) {
		SALOMEDS::Driver_var   driver = SALOMEDS::Driver::_narrow(comp);
		if (!CORBA::is_nil(driver)) {
		  SALOMEDS::StudyBuilder_var  B = aStudy->NewBuilder();
		  if (!CORBA::is_nil(B)) {
		    B->LoadWith(FComp,driver);
		  } else {
		    return;
		  }
		} 
		else {
		  MESSAGE("loadComponentData(): Driver is null");
		  return;
		}
	      }
	      else {
		MESSAGE("loadComponentData(): Engine is null");
		return;
	      }
		// 		// load
		// 		Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer","GEOM");
		// 		if (!CORBA::is_nil(comp)) {
		// 		  SALOMEDS::Driver_var driver = SALOMEDS::Driver::_narrow(comp);
		// 		  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
		// 		  SALOMEDS::SComponent_var SC = aStudy->FindComponent("GEOM");
		// 		  if (!CORBA::is_nil(SC))
		// 		    aStudyBuilder->LoadWith(SC,driver);
	    } 
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
}
