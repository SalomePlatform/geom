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
//  File   : GEOMBase.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "GEOMBase.h"

////  SALOME Includes
# include "Utils_ORB_INIT.hxx"
# include "Utils_SINGLETON.hxx"

#include "QAD_RightFrame.h"
#include "QAD_MessageBox.h"
#include "QAD_Resource.h"

#include "GEOM_AssemblyBuilder.h"
#include "VTKViewer_ViewFrame.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_Viewer3d.h"

#include "SALOME_ListIteratorOfListIO.hxx"
#include "SALOMEGUI_ImportOperation.h"
#include "SALOMEGUI_NameDlg.h"            

// // Open CASCADE Includes
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepPrimAPI_MakeCone.hxx>

#include <Geom_Circle.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <Precision.hxx>

// // QT Includes
#include <qmessagebox.h>

#include "GEOMBase_aParameterDlg.h"

//=================================================================================
// class   : CustomItem
// purpose : Set Font to a text.
//=================================================================================
class CustomItem : public QCustomMenuItem
{
public:
    CustomItem(const QString& s, const QFont& f)
      :string(s), font(f){};
    ~CustomItem(){}
  
    void paint(QPainter* p, const QColorGroup& /*cg*/, bool /*act*/,
	       bool /*enabled*/, int x, int y, int w, int h)
    {
      p->setFont(font);
      p->drawText(x, y, w, h, AlignHCenter | AlignVCenter | ShowPrefix | DontClip, string);
    }

    QSize sizeHint()
    {
      return QFontMetrics(font).size(AlignHCenter | AlignVCenter | ShowPrefix | DontClip, string);
    }

private:
  QString string;
  QFont font;

};


//=======================================================================
// function : GEOMBase()
// purpose  : Constructor
//=======================================================================
GEOMBase::GEOMBase() :
  QObject()
{
  myGeomGUI = GEOMContext::GetGeomGUI();
  myGeom = myGeomGUI->myComponentGeom;

  /* Shading Color */
  QString SCr = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorRed");
  QString SCg = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorGreen");
  QString SCb = QAD_CONFIG->getSetting("Geometry:SettingsShadingColorBlue");
  if(!SCr.isEmpty() && !SCg.isEmpty() && !SCb.isEmpty())   
    myShadingColor = Quantity_Color(SCr.toInt()/255., SCg.toInt()/255., SCb.toInt()/255., Quantity_TOC_RGB);
  else
    myShadingColor = Quantity_Color(Quantity_NOC_GOLDENROD);

  QString AddInStudy = QAD_CONFIG->getSetting("Geometry:SettingsAddInStudy");
  if(!AddInStudy.isEmpty())
    mySettings_AddInStudy = AddInStudy.toInt();
  else
    mySettings_AddInStudy = 1;
}


//=======================================================================
// function : ~GEOMBase()
// purpose  : Destructor
//=======================================================================
GEOMBase::~GEOMBase()
{
}


//=================================================================================
// function : CustomPopup()
// purpose  : [static]
//=================================================================================
bool GEOMBase::CustomPopup(QAD_Desktop* parent, QPopupMenu* popup, const QString& theContext,
			   const QString& theParent, const QString& theObject)
{
  GEOMBase* myGeomBase = new GEOMBase();
  QAD_Study* ActiveStudy = parent->getActiveStudy();

  /* Deactivate any non modal dialog box to get the neutral point */
  myGeomBase->myGeomGUI->EmitSignalDeactivateDialog();

  SALOME_Selection* Sel = SALOME_Selection::Selection(ActiveStudy->getSelection());
  int nbSel = Sel->IObjectCount();
  
  if((nbSel == 0))
    return false;

  if(ActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)ActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
    if(theParent.compare("Viewer") == 0) {
      if(theObject.compare("Component") == 0) {
	popup->removeItem(QAD_DisplayOnly_Popup_ID);
	return true;
      }
      else {
	QFont f = QApplication::font();
	f.setBold(TRUE);
	if(nbSel == 1) {
	  Handle(SALOME_InteractiveObject) IObject = Sel->firstIObject();
	  popup->removeItem(QAD_TopLabel_Popup_ID);
	  popup->insertItem(new CustomItem (QString(IObject->getName()), f), QAD_TopLabel_Popup_ID, 0);
	  if(IObject->hasEntry())
	    popup->setItemEnabled(804, false); //Add in Study Menu
	  else
	    popup->setItemEnabled(804, true);  //Add in Study Menu
    
	  if(IObject->IsKind(STANDARD_TYPE(GEOM_InteractiveObject))) {
	    Standard_Boolean found;
	    Handle(GEOM_AISShape) Result = myGeomBase->ConvertIOinGEOMAISShape(IObject, found, true);

	    if(found) {
	      if(Result->DisplayMode() == 1)
		popup->changeItem(8031, tr("GEOM_MEN_WIREFRAME")); //Shading/Wireframe Menu
	      else
		popup->changeItem(8031, tr("GEOM_MEN_SHADING"));   //Shading/Wireframe Menu
	    }
	  }

	  if(!(v3d->isInViewer(IObject) && v3d->isVisible(IObject)))
	    popup->removeItem(QAD_Erase_Popup_ID);
	  else
	    popup->removeItem(QAD_Display_Popup_ID);    
	}
	else {
	  popup->removeItem(QAD_DisplayOnly_Popup_ID);
	  popup->removeItem(QAD_TopLabel_Popup_ID);
	  popup->insertItem(new CustomItem (tr("GEOM_MEN_POPUP_NAME").arg(nbSel), f), QAD_TopLabel_Popup_ID, 0);
	  popup->setItemEnabled(804, false); //Add in Study Menu
	}
      }
      return true;
    }
    else if(theParent.compare("ObjectBrowser") == 0) {
      popup->removeItem(QAD_TopLabel_Popup_ID);
      int id = popup->idAt(0); // separator
      if(id < 0)
	popup->removeItem(id);
	    
      // checking for GEOM label in the selected list
      SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
      Handle(SALOME_InteractiveObject) anIObject;

      bool useSubItems = false;
      bool needOpen = false;
      bool needDisplay = false;
      bool needErase = false;
      SALOMEDS::GenericAttribute_var aTmpAttr;
      for(;It.More();It.Next()) {
	anIObject = It.Value();
	if(!anIObject->hasEntry())
	  continue;

	if(v3d->isInViewer(anIObject) && v3d->isVisible(anIObject)) 
	  needErase = true; 
	else 
	  needDisplay = true;
	SALOMEDS::SObject_var obj = ActiveStudy->getStudyDocument()->FindObjectID(anIObject->getEntry());
	if(!obj->_is_nil()) {
	  GEOM::GEOM_Shape_var aShape;
	  if(obj->FindAttribute(aTmpAttr, "AttributeIOR")) {
	    char *str = SALOMEDS::AttributeIOR::_narrow(aTmpAttr)->Value();
	    if(str && strlen(str))
	      aShape = myGeomBase->myGeom->GetIORFromString(str);
	  }
	  else if(obj->FindAttribute(aTmpAttr, "AttributePersistentRef"))
	    needOpen = true;
	  
	  if(aShape->_is_nil()) {
	    SALOMEDS::ChildIterator_var anIter = ActiveStudy->getStudyDocument()->NewChildIterator(obj);
	    while(anIter->More()) {
	      SALOMEDS::SObject_var subobj = anIter->Value();
	      if(subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		useSubItems = true;
		needErase = true;
		needDisplay = true;
	      }
	      else if(subobj->FindAttribute(aTmpAttr, "AttributePersistentRef")) {
		needOpen = true;
		useSubItems = true;
	      }
	      anIter->Next();
	    }
	  }
	}
      }
      // remove useless popup items
      if(nbSel != 1)
	popup->removeItem(901); // rename
      if(useSubItems)
	popup->removeItem(QAD_DisplayOnly_Popup_ID); // display only

      if(!needOpen) {
	int index = popup->indexOf(9024);  // open
	popup->removeItem(9024);
	popup->removeItemAt(index); // separator under Open

	if (!needDisplay)
	  popup->removeItem(QAD_Display_Popup_ID);
	if (!needErase)
	  popup->removeItem(QAD_Erase_Popup_ID);

	if(!needDisplay && !needErase) {
	  int id = popup->idAt(popup->count()-1); // last item
	  popup->removeItem(id); // separator
	}
      }
      else {
	popup->removeItem(QAD_DisplayOnly_Popup_ID); // display only
	popup->removeItem(QAD_Display_Popup_ID);
	popup->removeItem(QAD_Erase_Popup_ID);
	if (nbSel != 1) {
	  int id = popup->idAt(popup->count()-1); // last item
	  popup->removeItem(id); // separator
	}
      }
      return true;
    }
  }
  else if(ActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
    // MESSAGE ( " CUSTOM POPUP VIEWER VTK ")
    if((theParent.compare("Viewer") == 0)) {
      popup->setItemEnabled(8034, false); //Isos Menu
      if(theObject.compare("Component") == 0) {
	popup->removeItem(QAD_DisplayOnly_Popup_ID);
	return true;
      }

      //int id = popup->idAt(0);
      QFont f = QApplication::font();
      f.setBold(TRUE);
  
      if(nbSel == 1) {
	Handle(SALOME_InteractiveObject) IObject = Sel->firstIObject();
	popup->removeItem(QAD_TopLabel_Popup_ID);
	popup->insertItem(new CustomItem (QString(IObject->getName()), f), QAD_TopLabel_Popup_ID, 0);
	if(IObject->hasEntry()) {
	  popup->setItemEnabled(804, false); //Add in Study Menu
	  SALOMEDS::Study_var aStudy = ActiveStudy->getStudyDocument();
	  SALOMEDS::SObject_var SO = aStudy->FindObjectID(IObject->getEntry());
	  SALOMEDS::GenericAttribute_var anAttr;
	  SALOMEDS::AttributeIOR_var anIOR;
	  if(!SO->_is_nil()) {
	    if(SO->FindAttribute(anAttr, "AttributeIOR")) {
	      anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	      Standard_Boolean found;
	      GEOM_Actor* Result = myGeomBase->ConvertIORinGEOMActor(anIOR->Value(), found, true);
	      if(found) {
		if(Result->getDisplayMode() == 1)
		  popup->changeItem(8031, tr("GEOM_MEN_WIREFRAME")); //Shading/Wireframe Menu
		else
		  popup->changeItem(8031, tr("GEOM_MEN_SHADING"));   //Shading/Wireframe Menu
			      
		if(!Result->GetVisibility())
		  popup->removeItem(QAD_Erase_Popup_ID);
		else
		  popup->removeItem(QAD_Display_Popup_ID);		  
	      }
	      else
		popup->removeItem(QAD_Erase_Popup_ID);
	    }
	  }
	}
	else
	  popup->setItemEnabled(804, true); //Add in Study Menu
      }
      else {
	popup->removeItem(QAD_DisplayOnly_Popup_ID);
	popup->removeItem(QAD_TopLabel_Popup_ID);
	popup->insertItem( new CustomItem (tr("GEOM_MEN_POPUP_NAME").arg(nbSel), f), QAD_TopLabel_Popup_ID, 0);
	popup->setItemEnabled(804, false); //Add in Study Menu
      }
      return true;
    }
    else if(theParent.compare("ObjectBrowser") == 0) {
      popup->removeItem(QAD_TopLabel_Popup_ID);
      int id = popup->idAt(0); // separator
      if(id < 0)
	popup->removeItem(id);

      // checking for GEOM label in the selected list
      SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
      Handle(SALOME_InteractiveObject) anIObject;
      
      bool useSubItems = false;
      bool needOpen = false;
      bool needDisplay = false;
      bool needErase = false;
      SALOMEDS::GenericAttribute_var aTmpAttr;
      VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)ActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
      for(;It.More();It.Next()) {
	anIObject = It.Value();
	if(!anIObject->hasEntry())
	  continue;

	if(myRenderInter->isInViewer(anIObject) && myRenderInter->isVisible(anIObject)) 
	  needErase = true; 
	else 
	  needDisplay = true;

	SALOMEDS::SObject_var obj = ActiveStudy->getStudyDocument()->FindObjectID(anIObject->getEntry());
	if(!obj->_is_nil()) {
	  GEOM::GEOM_Shape_var aShape;
	  if(obj->FindAttribute(aTmpAttr, "AttributeIOR")) {
	    char *str = SALOMEDS::AttributeIOR::_narrow(aTmpAttr)->Value();
	    if(str && strlen(str))
	      aShape = myGeomBase->myGeom->GetIORFromString(str);
	  }
	  else if(obj->FindAttribute(aTmpAttr, "AttributePersistentRef"))
	    needOpen = true;

	  if(aShape->_is_nil()) {
	    SALOMEDS::ChildIterator_var anIter = ActiveStudy->getStudyDocument()->NewChildIterator(obj);
	    while(anIter->More()) {
	      SALOMEDS::SObject_var subobj = anIter->Value();
	      if(subobj->FindAttribute(aTmpAttr, "AttributeIOR")) {
		useSubItems = true;
		needDisplay = true;
		needErase = true;
	      }
	      else if(subobj->FindAttribute(aTmpAttr, "AttributePersistentRef")) {
		needOpen = true;
		useSubItems = true;
	      }
	      anIter->Next();
	    }
	  }
	}
      }
      // remove useless popup items
      if(nbSel != 1)
	popup->removeItem(901); // rename
      if(useSubItems)
	popup->removeItem(QAD_DisplayOnly_Popup_ID); // display only

      if(!needOpen) {
	int index = popup->indexOf(9024); // open
	popup->removeItem(9024);
	popup->removeItemAt(index); // separator under Open
	  
	if(!needDisplay)
	  popup->removeItem(QAD_Display_Popup_ID);
	if(!needErase)
	  popup->removeItem(QAD_Erase_Popup_ID);
	if(!needDisplay && !needErase) {
	  int id = popup->idAt(popup->count()-1); // last item
	  popup->removeItem(id); // separator
	}
      }
      else {
	popup->removeItem(QAD_DisplayOnly_Popup_ID); // display only
	popup->removeItem(QAD_Display_Popup_ID);
	popup->removeItem(QAD_Erase_Popup_ID);
	if(nbSel!=1) {
	  int id = popup->idAt(popup->count()-1); // last item
	  popup->removeItem(id); // separator
	}
      }
      return true;
    }
  } 
  else { // other viewer types not supported.
    while(1) {
      int id = popup->idAt(0);
      if(id <= QAD_TopLabel_Popup_ID && id != -1)
	popup->removeItemAt(0);
      else
	break;
    }
    popup->removeItem(QAD_DisplayOnly_Popup_ID);	
    popup->removeItem(QAD_Display_Popup_ID);
    popup->removeItem(QAD_Erase_Popup_ID);
    int id = popup->idAt(popup->count() - 1); // last item
    if(id < 0 && id != -1)
      popup->removeItem(id); // separator
    return false;
  }
  return false;
}


//=====================================================================================
// function : Display()
// purpose  : Displays a CORBA shape
//=====================================================================================
bool GEOMBase::Display(GEOM::GEOM_Shape_ptr aShape, Standard_CString name)
{
  // MESSAGE ( "GEOMBase::Display init ") 
  Handle(GEOM_InteractiveObject) IO;
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  Sel->ClearIObjects();

  if(aShape->_is_nil()) {
    QAD_MessageBox::warn1 (QAD_Application::getDesktop(), tr("GEOM_WRN_WARNING"), tr("GEOM_PRP_ABORT"), tr("GEOM_BUT_OK"));
    return false;
  }
  
  TopoDS_Shape shape = myGeomGUI->GetShapeReader().GetShape(myGeom, aShape);
  if(shape.IsNull())
    return false;

  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var aName;
  SALOMEDS::AttributePixMap_var aPixmap;

  SALOMEDS::SComponent_var father = aStudy->FindComponent("GEOM");
  int aLocked = false;
  if(father->_is_nil()) {
    QAD_Operation* op = new SALOMEGUI_ImportOperation(QAD_Application::getDesktop()->getActiveStudy());
    op->start();
    aLocked = aStudy->GetProperties()->IsLocked();
    if(aLocked) 
      aStudy->GetProperties()->SetLocked(false);
    father = aStudyBuilder->NewComponent("GEOM");
    anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributeName");
    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    aName->SetValue(QAD_Application::getDesktop()->getComponentUserName("GEOM"));
    anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributePixMap");
    aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
    aPixmap->SetPixMap("ICON_OBJBROWSER_Geometry");
    QAD_Application::getDesktop()->getActiveStudy()->updateObjBrowser();
    if(aLocked)
      aStudy->GetProperties()->SetLocked(true);
    op->finish();
  }

  aStudyBuilder->DefineComponentInstance(father, myGeom);
  father->ComponentIOR(myGeomGUI->GetFatherior());

  TCollection_AsciiString nameG("");
  Standard_CString Type;
  if(TCollection_AsciiString(name).IsEqual(Standard_CString(""))) {
    if(TCollection_AsciiString(aShape->NameType()).IsEqual(Standard_CString(""))) {
      Standard_CString type;
      GetShapeTypeString(shape,type);
      aShape->NameType(type);
      nameG += TCollection_AsciiString(type) + TCollection_AsciiString("_") + TCollection_AsciiString(myGeomGUI->GetNbGeom()++) + TCollection_AsciiString("\0");
    } 
    else
      nameG += TCollection_AsciiString(aShape->NameType()) + TCollection_AsciiString("_") + TCollection_AsciiString(myGeomGUI->GetNbGeom()++) + TCollection_AsciiString("\0");
  } 
  else 
    nameG = TCollection_AsciiString(name);
  
  // VTK
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
    VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
    
    vtkRenderer *theRenderer = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();
    int themode = myRenderInter->GetDisplayMode();

    vtkActorCollection* theActors = GEOM_AssemblyBuilder::BuildActors(shape, 0, themode, Standard_True);
    theActors->InitTraversal();
    vtkActor* anActor = (vtkActor*)theActors->GetNextActor();

    IO = new GEOM_InteractiveObject(aShape->Name(), myGeomGUI->GetFatherior(), "GEOM");
    while(!(anActor==NULL)) {
      GEOM_Actor* GActor = GEOM_Actor::SafeDownCast(anActor);
      GActor->setIO(IO);
      GActor->setName(nameG.ToCString());
      
      theRenderer->AddActor(GActor);
      anActor = (vtkActor*)theActors->GetNextActor();
    }
  } 
  // OCC
  else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
    Handle(GEOM_AISShape) theResult = new GEOM_AISShape(shape, nameG.ToCString());
    theResult->SetShadingColor(myShadingColor);
    IO = new GEOM_InteractiveObject(aShape->Name(), myGeomGUI->GetFatherior(), "GEOM");
    theResult->setIO(IO);
    theResult->setName(nameG.ToCString());
    
    /* Precaution : close any local context to permit the display */
    if(ic->HasOpenedContext())
      ic->CloseAllContexts();

    ic->Display(theResult);
  }

  // DCQ Sel->AddIObject(IO, false);
  QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame()->Repaint();
  if(mySettings_AddInStudy)
    AddInStudy(false, IO);

  return true;
}


//=====================================================================================
// function : AddInStudy() 
// purpose  : anIOShape or a selected shape
//=====================================================================================
bool GEOMBase::AddInStudy(bool selection, const Handle(SALOME_InteractiveObject)& anIO)
{
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  if (!(!mySettings_AddInStudy || selection)) {
    Sel->ClearIObjects();
  }

  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeName_var aName;
  SALOMEDS::AttributePixMap_var aPixmap;
  SALOMEDS::AttributeIOR_var anIOR;
  SALOMEDS::AttributeSelectable_var aSelAttr;
  
  SALOMEDS::SComponent_var father = aStudy->FindComponent("GEOM");
  int aLocked = false;
  if(father->_is_nil()) {
    QAD_Operation* op = new SALOMEGUI_ImportOperation(QAD_Application::getDesktop()->getActiveStudy());
    op->start();
    aLocked = aStudy->GetProperties()->IsLocked();
    if(aLocked)
      aStudy->GetProperties()->SetLocked(false);
    father = aStudyBuilder->NewComponent("GEOM");
    anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributeName");
    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    aName->SetValue( QAD_Application::getDesktop()->getComponentUserName("GEOM"));
    anAttr = aStudyBuilder->FindOrCreateAttribute(father, "AttributePixMap");
    aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
    aPixmap->SetPixMap("ICON_OBJBROWSER_Geometry");
    if (aLocked)
      aStudy->GetProperties()->SetLocked(true);
    op->finish();
  }

  aStudyBuilder->DefineComponentInstance(father, myGeom);
  father->ComponentIOR(myGeomGUI->GetFatherior());
  
  SALOMEDS::SObject_var fatherSF = aStudy->FindObjectID(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->entry());
  
  Handle(GEOM_AISShape) GAISShape;
  GEOM_Actor* GActor;
  Handle(GEOM_InteractiveObject) GIO;
  bool found = false;

  // VTK
  if (QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
    vtkRenderer *Renderer = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();

    vtkActorCollection* theActors = Renderer->GetActors();
    theActors->InitTraversal();
    vtkActor *ac = theActors->GetNextActor();
    while(!(ac==NULL)) {
      if(ac->IsA("GEOM_Actor")) {
	GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(ac);
	if(anActor->hasIO()) {
	  Handle(SALOME_InteractiveObject) IO = anActor->getIO();
	  if(IO->IsKind(STANDARD_TYPE(GEOM_InteractiveObject))) {
	    GIO = Handle(GEOM_InteractiveObject)::DownCast(IO);
	    if(anIO->isSame(GIO)) {
	      found = true;
	      GActor = anActor;
	      break;
	    }
	  }
	}
      }
      ac = theActors->GetNextActor();
    }
    
    if(!found)
      return false;
  }
  // OCC
  else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
    
    AIS_ListOfInteractive List;
    ic->DisplayedObjects(List);
    AIS_ListIteratorOfListOfInteractive ite(List);
    while(ite.More()) {
      if(ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	if(aSh->hasIO()) {
	  Handle(SALOME_InteractiveObject) IO = aSh->getIO();
	  if ( IO->IsKind(STANDARD_TYPE(GEOM_InteractiveObject))) {
	    GIO = Handle(GEOM_InteractiveObject)::DownCast(IO);
	    if(anIO->isSame(GIO)) {
	      found = true;
	      GAISShape = aSh;
	      break;
	    }
	  }
	}
      }
      ite.Next();
    }

    if(!found)
      return false;
  }

  if(!mySettings_AddInStudy || selection) {
    QString Name = SALOMEGUI_NameDlg::getName(QAD_Application::getDesktop(), anIO->getName());
    if(!Name.isEmpty()) {
      // VTK
      if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK)
	GActor->setName(strdup(Name.latin1()));
      // OCC
      else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC)
	GAISShape->setName(strdup(Name.latin1()));
    } 
    else
      return false;
  }

  // open transaction
  QAD_Operation* op = new SALOMEGUI_ImportOperation(QAD_Application::getDesktop()->getActiveStudy());
  op->start();

  SALOMEDS::SObject_var newObj = aStudyBuilder->NewObject(father);

  GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(GIO->getIOR());

  /* For the shape inserted into the study we set its field 'studyshapeid'    */
  /* so the shape will contain its corresponding entry in the study Ocaf doc. */
  aShape->StudyShapeId(newObj->GetID());

  GIO->setEntry(newObj->GetID());

  anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeIOR");
  anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
  anIOR->SetValue(aShape->Name());

  anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributeName");
  aName = SALOMEDS::AttributeName::_narrow(anAttr);

  anAttr = aStudyBuilder->FindOrCreateAttribute(newObj, "AttributePixMap");
  aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
  if(aShape->ShapeType() == GEOM::COMPOUND)
    aPixmap->SetPixMap("ICON_OBJBROWSER_COMPOUND");
  else if(aShape->ShapeType() == GEOM::COMPSOLID)
    aPixmap->SetPixMap("ICON_OBJBROWSER_COMPSOLID");
  else if(aShape->ShapeType() == GEOM::SOLID)
    aPixmap->SetPixMap("ICON_OBJBROWSER_SOLID");
  else if(aShape->ShapeType() == GEOM::SHELL)
    aPixmap->SetPixMap("ICON_OBJBROWSER_SHELL");
  else if(aShape->ShapeType() == GEOM::FACE)
    aPixmap->SetPixMap("ICON_OBJBROWSER_FACE");
  else if(aShape->ShapeType() == GEOM::WIRE)
    aPixmap->SetPixMap("ICON_OBJBROWSER_WIRE");
  else if(aShape->ShapeType() == GEOM::EDGE)
    aPixmap->SetPixMap("ICON_OBJBROWSER_EDGE");
  else if(aShape->ShapeType() == GEOM::VERTEX)
    aPixmap->SetPixMap("ICON_OBJBROWSER_VERTEX");

  // VTK
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
    GActor->setIO(GIO);
    aName->SetValue(GActor->getName());
  }
  // OCC
  else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    GAISShape->setIO(GIO);
    aName->SetValue(GAISShape->getName());
  }

  SALOMEDS::SObject_var newObj1 = aStudyBuilder->NewObject(fatherSF);
  aStudyBuilder->Addreference(newObj1,newObj);

  GEOM::GEOM_Gen::ListOfIOR_var listIOR = new GEOM::GEOM_Gen::ListOfIOR;
  listIOR = myGeom->GetReferencedObjects(aShape);

  if(listIOR->length()>0) {
    SALOMEDS::SObject_var Arguments = aStudyBuilder->NewObject(newObj);
    anAttr = aStudyBuilder->FindOrCreateAttribute(Arguments, "AttributeName");
    aName = SALOMEDS::AttributeName::_narrow(anAttr);
    aName->SetValue(tr("GEOM_ARGUMENTS"));
    anAttr = aStudyBuilder->FindOrCreateAttribute(Arguments, "AttributeSelectable");
    aSelAttr = SALOMEDS::AttributeSelectable::_narrow(anAttr);
    aSelAttr->SetSelectable(false);

    bool ObjectReferenced = false;
    for (unsigned int ind = 0; ind < listIOR->length();ind++) {
      SALOMEDS::SObject_var theObj = aStudy->FindObjectIOR(listIOR[ind]);

      if(!theObj->_is_nil()) {
	SALOMEDS::SObject_var RefObject = aStudyBuilder->NewObject(Arguments);
	aStudyBuilder->Addreference(RefObject, theObj);
	ObjectReferenced = true;
      }
    }

    if(!ObjectReferenced)
      aStudyBuilder->RemoveObject(Arguments);
  }
  op->finish();

  if(!mySettings_AddInStudy || selection)
    QAD_Application::getDesktop()->getActiveStudy()->updateObjBrowser();
  else {
    QAD_Application::getDesktop()->getActiveStudy()->updateObjBrowser(false);
    // DCQ Sel->AddIObject(GIO);
  }
  return true;
}


//=====================================================================================
// function : GetShapeFromIOR()
// purpose  : exist also as static method !
//=====================================================================================
TopoDS_Shape GEOMBase::GetShapeFromIOR(QString IOR)
{
  TopoDS_Shape result;
  if(IOR.stripWhiteSpace().isEmpty())
    return result;
  ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance();
  CORBA::ORB_var& _orb = init.orb() ;
  CORBA::Object_var obj = _orb->string_to_object((char*)(IOR.latin1()));
  if(CORBA::is_nil(obj))
    return result;
  GEOM::GEOM_Shape_var aShape = myGeom->GetIORFromString(IOR);
  if(!aShape->_is_nil())
    result = myGeomGUI->GetShapeReader().GetShape(myGeom, aShape);
  return result;
}


//=====================================================================================
// function : GetIndex()
// purpose  : Get the index of a sub shape in a main shape : index start at 1
//=====================================================================================
int GEOMBase::GetIndex(const TopoDS_Shape& subshape, const TopoDS_Shape& shape, int /*ShapeType*/) 
{ 
  if(shape.IsNull() || subshape.IsNull()) 
    return -1; 
  
  int index = 1; 
  if(subshape.ShapeType() == TopAbs_COMPOUND) { 
    TopoDS_Iterator it; 
    TopTools_ListOfShape CL; 
    CL.Append(shape); 
    TopTools_ListIteratorOfListOfShape itC; 
    for(itC.Initialize(CL); itC.More(); itC.Next()) { 
      for(it.Initialize(itC.Value()); it.More(); it.Next()) { 
	if(it.Value().ShapeType() == TopAbs_COMPOUND) {
	  if(it.Value().IsSame(subshape)) 
	    return index; 
	  else 
	    index++; 
	  CL.Append(it.Value()); 
	}
      } 
    }
  }
  else { 
    TopExp_Explorer Exp(shape, subshape.ShapeType()); 
    TopTools_MapOfShape M; 
    while(Exp.More()) {
      if(M.Add(Exp.Current())) { 
	if(Exp.Current().IsSame(subshape)) 
	  return index; 
	index++; 
      } 
      Exp.Next(); 
    } 
  } 
  return -1; 
}


//=======================================================================
// function : GetTopoFromSelection()
// purpose  : Define tds from a single selection and retuen true
//=======================================================================
bool GEOMBase::GetTopoFromSelection(SALOME_Selection *Sel, TopoDS_Shape& tds) 
{
  if(Sel->IObjectCount() != 1)
    return false;
  
  Handle(SALOME_InteractiveObject) IO = Sel->firstIObject();
  /* case SObject */
  if(IO->hasEntry()) {
    SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
    SALOMEDS::SObject_var obj = aStudy->FindObjectID(IO->getEntry());
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeIOR_var anIOR;
    if(!obj->_is_nil()) {
      if(obj->FindAttribute(anAttr, "AttributeIOR")) {
	anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	tds = this->GetShapeFromIOR(anIOR->Value());
	if(tds.IsNull())
	  return false;
	else
	  return true;
      }
    }
  }

  if(IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject))) {
    Standard_CString ior = "";
    Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast(IO);
    ior = GIObject->getIOR();
    tds = this->GetShapeFromIOR(ior);
    if(tds.IsNull())
      return false;
    else
      return true;
  }
  
  return false;
}


//=======================================================================
// function : GetNameOfSelectedIObjects()
// purpose  : Define the name geom++ or other name of mono or multi sel.
//=======================================================================
int GEOMBase::GetNameOfSelectedIObjects(SALOME_Selection* Sel, QString& aName)
{
  int nbSel = Sel->IObjectCount();
  if(nbSel == 1) {
    Handle(SALOME_InteractiveObject) IObject = Sel->firstIObject();
    aName = IObject->getName();
  } 
  else {
    aName = tr("%1_objects").arg(nbSel);
  }
  return nbSel;
}


//=================================================================================
// function : GetShapeTypeString()
// purpose  : for a single shape
//=================================================================================
bool GEOMBase::GetShapeTypeString(const TopoDS_Shape& aShape, Standard_CString& aTypeString)
{
  if(aShape.IsNull()) {
    aTypeString = "aNullShape";
    return false;
  }
  switch(aShape.ShapeType())
    {
    case TopAbs_COMPOUND:
      {
	aTypeString = strdup(tr("GEOM_COMPOUND"));
	return true;
      }
    case  TopAbs_COMPSOLID:
      {
	aTypeString = strdup(tr("GEOM_COMPOUNDSOLID")) ; 
	return true ;
      }
    case TopAbs_SOLID:
      {
	aTypeString = strdup(tr("GEOM_SOLID")) ; 
	return true ;
      }
    case TopAbs_SHELL:
      {
	aTypeString = strdup(tr("GEOM_SHELL")) ; 
	return true ;
      }
    case TopAbs_FACE:
      { 
	BRepAdaptor_Surface surf(TopoDS::Face(aShape));
	if(surf.GetType() == GeomAbs_Plane) {
	  aTypeString = strdup(tr("GEOM_PLANE"));
	  return true;
	}
	else if(surf.GetType() == GeomAbs_Cylinder) {
	  aTypeString = strdup(tr("GEOM_SURFCYLINDER"));
	  return true;
	}
	else if(surf.GetType() == GeomAbs_Sphere) {
	  aTypeString = strdup(tr("GEOM_SURFSPHERE"));
	  return true ;
	}
	else if(surf.GetType() == GeomAbs_Torus) {
	  aTypeString = strdup(tr("GEOM_SURFTORUS"));
	  return true ;
	}
	else if(surf.GetType() == GeomAbs_Cone) {
	  aTypeString = strdup(tr("GEOM_SURFCONE"));
	  return true ;
	}
	else {
	  aTypeString = strdup(tr("GEOM_FACE"));
	  return true;
	}
      }
    case TopAbs_WIRE:
      {
	aTypeString = strdup(tr("GEOM_WIRE")); 
	return true;
      }
    case TopAbs_EDGE:
      {
	BRepAdaptor_Curve curv(TopoDS::Edge(aShape));
	if(curv.GetType() == GeomAbs_Line) {
	  if((Abs(curv.FirstParameter()) >= 1E6) || (Abs(curv.LastParameter()) >= 1E6))
	    aTypeString = strdup(tr("GEOM_LINE"));
	  else
	    aTypeString = strdup(tr("GEOM_EDGE"));
	  return true;
	}
	else if(curv.GetType() == GeomAbs_Circle) {
	  if(curv.IsClosed())
	    aTypeString = strdup(tr("GEOM_CIRCLE"));
	  else
	    aTypeString = strdup(tr("GEOM_ARC"));
	return true;
      } 
	else {
	  aTypeString = strdup(tr("GEOM_EDGE"));
	  return true;
	}
      }
    case TopAbs_VERTEX:
      {
	aTypeString = strdup(tr("GEOM_VERTEX"));
	return true;
      }
    case TopAbs_SHAPE:
      {
	aTypeString = strdup(tr("GEOM_SHAPE"));
	return true;
      }
    }
  return false;
}


//=======================================================================
// function : ConvertIORinGEOMAISShape()
// purpose  :
//=======================================================================
Handle(GEOM_AISShape) GEOMBase::ConvertIORinGEOMAISShape(const char * IOR, Standard_Boolean& testResult, bool onlyInActiveView)
{
  Handle(GEOM_AISShape) resultShape;
  testResult = false;
  int nbSf = QAD_Application::getDesktop()->getActiveStudy()->getStudyFramesCount();
  for(int i = 0; i < nbSf; i++) {
    QAD_StudyFrame* sf = QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame(i);
    if(sf->getTypeView() == VIEW_OCC) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getViewer();
      Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

      AIS_ListOfInteractive List;
      ic->DisplayedObjects(List);
      AIS_ListOfInteractive List1;
      ic->ObjectsInCollector(List1);
      List.Append(List1);

      AIS_ListIteratorOfListOfInteractive ite(List);
      while(ite.More()) {
	if(ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	  Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	  if(aSh->hasIO()) {
	    Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(aSh->getIO());
	    Standard_CString theIOR = GIO->getIOR();
	    if(strcmp(IOR, theIOR) == 0) {
	      if(onlyInActiveView) {
		if(sf == QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()) {
		  testResult = true;
		  resultShape = aSh;
		  return resultShape; 
		}
	      } 
	      else {
		testResult = true;
		resultShape = aSh;
		return resultShape; 
	      }
	    }
	  }
	}
	ite.Next();
      }  
    }
  }
  return  resultShape;
}


//=======================================================================
// function : ConvertIORinGEOMActor()
// purpose  :
//=======================================================================
GEOM_Actor* GEOMBase::ConvertIORinGEOMActor(const char* IOR, Standard_Boolean& testResult, bool onlyInActiveView)
{
  int nbSf = QAD_Application::getDesktop()->getActiveStudy()->getStudyFramesCount();
  for(int i = 0; i < nbSf; i++) {
    QAD_StudyFrame* sf = QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame(i);
    if(sf->getTypeView() == VIEW_VTK) {
      vtkRenderer* Renderer = ((VTKViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getRenderer();
      vtkActorCollection* theActors = Renderer->GetActors();
      theActors->InitTraversal();
      vtkActor *ac = theActors->GetNextActor();
      while(!(ac==NULL)) {
	if( ac->IsA("GEOM_Actor")) {
	  GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(ac);
	  if(anActor->hasIO()) {
	    Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(anActor->getIO());
	    Standard_CString theIOR = GIO->getIOR();
	    if(strcmp(IOR, theIOR) == 0) {
	      if(onlyInActiveView) {
		if(sf == QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()) {
		  testResult = true;
		  return anActor;
		}
	      } 
	      else {
		testResult = true;
		return anActor;
	      }
	    }
	  }
	}
	ac = theActors->GetNextActor();
      }
    }
  }
  testResult = false;
  return GEOM_Actor::New();
}


//=======================================================================
// function : ConvertIOinGEOMAISShape()
// purpose  :
//=======================================================================
Handle(GEOM_AISShape) GEOMBase::ConvertIOinGEOMAISShape(const Handle(SALOME_InteractiveObject)& IO, Standard_Boolean& testResult, bool onlyInActiveView)
{
  Handle(GEOM_AISShape) res;
  int nbSf = QAD_Application::getDesktop()->getActiveStudy()->getStudyFramesCount();
  for(int i = 0; i < nbSf; i++) {
    QAD_StudyFrame* sf = QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame(i);
    if(sf->getTypeView() == VIEW_OCC) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)sf->getRightFrame()->getViewFrame())->getViewer();
      Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

      AIS_ListOfInteractive List;
      ic->DisplayedObjects(List);
      AIS_ListOfInteractive List1;
      ic->ObjectsInCollector(List1);
      List.Append(List1);
      
      AIS_ListIteratorOfListOfInteractive ite(List);
      while(ite.More()) {
	if(ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	  Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	  if(aSh->hasIO()) {
	    Handle(GEOM_InteractiveObject) GIO = Handle(GEOM_InteractiveObject)::DownCast(aSh->getIO());
	    if(GIO->isSame(IO)) {
	      if(onlyInActiveView) {
		if(sf == QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()) {
		  testResult = true;
		  return aSh;
		}
	      } 
	      else {
		testResult = true;
		return aSh;
	      }
	    }
	  }
	}
	ite.Next();
      }  
    }
  }
  testResult = false;
  return res;
}


//=======================================================================
// function : ConvertIOinGEOMShape()
// purpose  :
//=======================================================================
GEOM::GEOM_Shape_ptr GEOMBase::ConvertIOinGEOMShape(const Handle(SALOME_InteractiveObject)& IO, Standard_Boolean& testResult)
{
  GEOM::GEOM_Shape_var aShape;
  testResult = false;
  
  /* case SObject */
  if(IO->hasEntry()) {
    SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
    SALOMEDS::SObject_var obj = aStudy->FindObjectID(IO->getEntry());
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeIOR_var anIOR;
    if(!obj->_is_nil()) {
      if(obj->FindAttribute(anAttr, "AttributeIOR")) {
	anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	aShape = myGeom->GetIORFromString(anIOR->Value());
	if(!CORBA::is_nil(aShape))
	  testResult = true;
	return aShape._retn();
      }
    }
  }
  /* case Graphical Object */
  if(IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject))) {
    Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast(IO);
    Standard_CString ior = GIObject->getIOR();

    aShape = myGeom->GetIORFromString(ior);
    if(!CORBA::is_nil(aShape))
      testResult = true;
    return aShape._retn();
  }
  return aShape._retn();
}


//=======================================================================
// function : ConvertListOfIOInListOfIOR()
// purpose  : 
//=======================================================================
void GEOMBase::ConvertListOfIOInListOfIOR(const SALOME_ListIO& aList, GEOM::GEOM_Gen::ListOfIOR& listIOR)
{
  int nbSel = aList.Extent();  
  listIOR.length(nbSel);
  int j=0;
  SALOME_ListIteratorOfListIO It(aList);
  for(int i=0; It.More(); It.Next(), i++) {
    Handle(SALOME_InteractiveObject) IObject = It.Value();
    if(IObject->hasEntry()) {
      SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
      SALOMEDS::SObject_var obj = aStudy->FindObjectID(IObject->getEntry());
      SALOMEDS::GenericAttribute_var anAttr;
      SALOMEDS::AttributeIOR_var anIOR;
      if(!obj->_is_nil() && obj->FindAttribute(anAttr, "AttributeIOR")) {
	anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance();
	CORBA::ORB_var& _orb = init.orb();
	CORBA::String_var theValue = anIOR->Value();
	CORBA::Object_var theObj = _orb->string_to_object(theValue);
	if(theObj->_is_a("IDL:GEOM/GEOM_Shape:1.0")) {
	  listIOR[j] = strdup(theValue);
	  j++;
	}
      }
    } 
    else if(IObject->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject))) {
      Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast(IObject);
      Standard_CString ior = GIObject->getIOR();
      listIOR[j] = strdup(ior);
      j++;
    }
  }
  listIOR.length(j);
}


//================================================================================
// function : SetDisplayedObjectList()
// purpose  :
//================================================================================
// void GEOMBase::SetDisplayedObjectList()
// {
//   if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
//     return;

//   OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
//   myGeomGUI->GetListDisplayedObject().Clear();

//   Handle (AIS_InteractiveContext) aContext = v3d->getAISContext();
//   aContext->DisplayedObjects(myGeomGUI->GetListDisplayedObject());
// }


//=====================================================================================
// function : DisplaySimulationShape() 
// purpose  : Displays 'this->mySimulationShape' a pure graphical shape from a TopoDS_Shape
//=====================================================================================
void GEOMBase::DisplaySimulationShape(const TopoDS_Shape& S) 
{
  if(S.IsNull())
    return;
  	
  //NRI DEBUG : 14/02/2002
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return;
	
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
  try {
    /* erase any previous */
    ic->Erase(mySimulationShape, Standard_True, Standard_False);
    ic->ClearPrs(mySimulationShape);
    mySimulationShape = new AIS_Shape(TopoDS_Shape());
    mySimulationShape->Set(S);
    mySimulationShape->SetColor(Quantity_NOC_VIOLET);
    ic->Deactivate(mySimulationShape);
    ic->Display(mySimulationShape, Standard_False);
    ic->UpdateCurrentViewer();
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in GEOMBase::DisplaySimulationShape ");
  } 
  mySimulationShape->UnsetColor();
  return;
}


//==================================================================================
// function : EraseSimulationShape()
// purpose  : Clears the display of 'mySimulationShape' a pure graphical shape
//==================================================================================
void GEOMBase::EraseSimulationShape()
{
  int count = QAD_Application::getDesktop()->getActiveStudy()->getStudyFramesCount();
  for(int i = 0; i < count; i++) {
    if(QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame(i)->getTypeView() == VIEW_OCC) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame(i)->getRightFrame()->getViewFrame())->getViewer();
      Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
      ic->Erase(mySimulationShape, Standard_True, Standard_False);
      ic->ClearPrs(mySimulationShape);
      ic->UpdateCurrentViewer();
    } 
  }
}


//=================================================================================
// function : CreateArrowForLinearEdge()
// purpose  : Create a cone topology to be used to display an arrow in the middle
//          : of an edge showing its orientation. (For simulation and Viewer OCC only)
//=================================================================================
bool GEOMBase::CreateArrowForLinearEdge(const TopoDS_Shape& tds, TopoDS_Shape& ArrowCone)
{
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() != VIEW_OCC || tds.ShapeType() != TopAbs_EDGE)
    return false;
  
  OCCViewer_ViewPort* vp = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewPort();
  Handle( V3d_View) view3d = ((OCCViewer_ViewPort3d*)vp)->getView();
  Standard_Real Width, Height;
  view3d->Size(Width, Height);
  const Standard_Real aHeight = (Width + Height) / 50.0;
  
  try {
    Standard_Real first, last;
    Handle(Geom_Curve) curv = BRep_Tool::Curve(TopoDS::Edge(tds), first, last);
    if(!curv->IsCN(1))
      return false;

    const Standard_Real param = (first+last) / 2.0;
    gp_Pnt middleParamPoint;
    gp_Vec V1;
    curv->D1( param, middleParamPoint, V1);
    if(V1.Magnitude() < Precision::Confusion())
      return false;
    
    /* Topology orientation not geom orientation */
    if(tds.Orientation() == TopAbs_REVERSED)
      V1 *= -1.0;
    
    gp_Ax2 anAxis( middleParamPoint, gp_Dir(V1));
    const Standard_Real radius1 = aHeight / 5.0;
    if(radius1 > 10.0 * Precision::Confusion() && aHeight > 10.0 * Precision::Confusion()) {
      ArrowCone = BRepPrimAPI_MakeCone( anAxis, radius1, 0.0, aHeight ).Shape();
      return true;
    }
  }
  catch(Standard_Failure) {
    // OCC failures are hard to catch in GUI.
    // This  because of the position for  #include <Standard_ErrorHandler.hxx> that is very critic to find 
    // in SALOME environment : compilation error !
  }
  return false;
}


//=================================================================================
// function : VertexToPoint()
// purpose  : If S can be converted in a gp_Pnt returns true and the result is P
//=================================================================================
bool GEOMBase::VertexToPoint(const TopoDS_Shape& S, gp_Pnt& P)
{
  if(S.IsNull() || S.ShapeType() != TopAbs_VERTEX)
    return false;
  P = BRep_Tool::Pnt(TopoDS::Vertex(S));
  return true;
}


//=================================================================================
// function : GetBipointDxDyDz()
// purpose  : 
//=================================================================================
void GEOMBase::GetBipointDxDyDz(gp_Pnt P1, gp_Pnt P2, double& dx, double& dy, double& dz)
{
  dx = P2.X() - P1.X();
  dy = P2.Y() - P1.Y();
  dz = P2.Z() - P1.Z();
  return;
}


//=================================================================================
// function : LinearEdgeExtremities()
// purpose  : If S can be converted in a linear edge and if initial an final points
//          : distance is sufficient, returns true else returns false.
//          : Resulting points are respectively P1 and P2
//=================================================================================
bool GEOMBase::LinearEdgeExtremities(const TopoDS_Shape& S,  gp_Pnt& P1, gp_Pnt& P2)
{
  if(S.IsNull() || S.ShapeType() != TopAbs_EDGE)
    return false;
  BRepAdaptor_Curve curv(TopoDS::Edge(S));
  if(curv.GetType() != GeomAbs_Line)
    return false;

  curv.D0(curv.FirstParameter(), P1);
  curv.D0(curv.LastParameter(), P2);

  if(P1.Distance(P2) <= Precision::Confusion())
    return false;

  return true;
}


//=======================================================================
// function : Parameter()
// purpose  : return a parameter (float) from a dialog box
//
//  avalue1    : is a float or integer used as a default value displayed
//  aTitle1    : is the title for aValue1
//  aTitle     : is the main title
//  bottom     : maximum value to be entered
//  top	       : minimum value to be entered
//  decimals   : number of decimals
//=======================================================================
double GEOMBase::Parameter(Standard_Boolean& res, const char* aValue1, const char* aTitle1, const char* aTitle, const double bottom, const double top, const int decimals)
{
  GEOMBase_aParameterDlg * Dialog = new GEOMBase_aParameterDlg(aValue1, aTitle1, QAD_Application::getDesktop(), aTitle, TRUE, 0, bottom, top, decimals);
  int r = Dialog->exec();
  float X = 0.0;
  if(r == QDialog::Accepted) {
    res = Standard_True;
    X = Dialog->getValue();
  } 
  else
    res = Standard_False;
  delete Dialog;
  return X;
}


//=======================================================================
// function : SelectionByNameInDialogs()
// purpose  : Called when user has entered a name of object in a LineEdit.
//          : The selection is changed. Dialog box will receive the 
//          : corresponding signal to manage this event.
//=======================================================================
bool GEOMBase::SelectionByNameInDialogs(QWidget* aWidget, const QString& objectUserName, SALOME_Selection* Sel)
{
  /* Find SObject with name in component GEOM */
  SALOMEDS::Study_var ST = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::Study::ListOfSObject_var listSO = new SALOMEDS::Study::ListOfSObject;
  listSO = ST->FindObjectByName(objectUserName, "GEOM");
  
  if(listSO->length() < 1) {
    const QString caption  = tr("GEOM_WRN_WARNING");
    const QString text = tr("GEOM_NAME_INCORRECT");
    const QString button0  = tr("GEOM_BUT_OK");
    QMessageBox::warning(aWidget, caption, text, button0);
    return false;
  }
  /* More than one object with same name */
  if(listSO->length() > 1) {
    const QString caption  = tr("GEOM_WRN_WARNING");
    const QString text = tr("GEOM_IDENTICAL_NAMES_SELECT_BY_MOUSE");
    const QString button0  = tr("GEOM_BUT_OK") ;
    QMessageBox::warning(aWidget, caption, text, button0) ;
    return false;
  }

  SALOMEDS::SObject_var theObj = listSO[0];
  /* Create a SALOME_InteractiveObject with a SALOME::SObject */
  Standard_CString anEntry = theObj->GetID();
  Handle(SALOME_InteractiveObject) SI = new SALOME_InteractiveObject(anEntry, "GEOM", strdup(objectUserName));

  /* Add as a selected object       */
  /* Clear any previous selection : */
  /* Warning the LineEdit is purged because of signal currentSelectionChanged ! */ 
  Sel->ClearIObjects(); 
  Sel->AddIObject(SI);
  return true;
}


//=======================================================================
// function : DefineDlgPosition()
// purpose  : Define x and y the default position for a dialog box
//=======================================================================
bool GEOMBase::DefineDlgPosition(QWidget* aDlg, int& x, int& y)
{
  /* Here the position is on the bottom right corner - 10 */
  QAD_Desktop* PP = QAD_Application::getDesktop();
  x = abs(PP->x() + PP->size().width() - aDlg->size().width() - 10);
  y = abs(PP->y() + PP->size().height() - aDlg->size().height() - 10);
  return true;  
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  bool CustomPopup(QAD_Desktop* parent, QPopupMenu* popup, const QString & theContext,
		   const QString & theParent, const QString & theObject)
  {return GEOMBase::CustomPopup(parent, popup, theContext, theParent, theObject);}
}
