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
//  $Header$

#include "GEOMToolsGUI.h"
#include "GeometryGUI.h"
#include "GEOM_Actor.h"
#include "GEOMBase.h"
#include "GEOM_Operation.h"
#include "GEOM_Displayer.h"

#include "QAD_Desktop.h"
#include "QAD_FileDlg.h"
#include "QAD_Tools.h"
#include "QAD_MessageBox.h"
#include "QAD_RightFrame.h"
#include "QAD_WaitCursor.h"

#include "OCCViewer_Viewer3d.h"
#include "VTKViewer_ViewFrame.h"
#include "VTKViewer_RenderWindowInteractor.h"

#include "SALOME_ListIteratorOfListIO.hxx"
#include "SALOMEGUI_ImportOperation.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include <qapplication.h>
#include <qmap.h>

#include "utilities.h"

using namespace std;

GEOMToolsGUI* GEOMToolsGUI::myGUIObject = 0;

typedef QMap<QString, QString> FilterMap;

//=======================================================================
// function : getFileName
// purpose  : Selection of a file name for Import/Export. Returns also 
//            the selected file type code through <filter> argument.
//=======================================================================
static QString getFileName( QWidget*           parent, 
			    const QString&     initial, 
			    const FilterMap&   filterMap, 
			    const QString&     caption,
			    bool               open,
			    QString&           format )
{
  static QString lastUsedFilter;
  QStringList filters;
  for ( FilterMap::const_iterator it = filterMap.begin(); it != filterMap.end(); ++it )
    filters.push_back( it.key() );

  QAD_FileDlg* fd = new QAD_FileDlg( parent, open, true, true );    
  if ( !caption.isEmpty() )
    fd->setCaption( caption );

  if ( !initial.isEmpty() )
    fd->setSelection( initial );

  if ( !lastUsedFilter.isEmpty() && filterMap.contains( lastUsedFilter ) )
    fd->setSelectedFilter( lastUsedFilter );

  fd->setFilters( filters );

  fd->exec();
  QString filename = fd->selectedFile();
  format = filterMap[fd->selectedFilter()];
  lastUsedFilter = fd->selectedFilter();
  delete fd;
  qApp->processEvents();
  return filename;
}

//=======================================================================
// function : GetGEOMToolsGUI()
// purpose  : Get the only GEOMToolsGUI object [ static ]
//=======================================================================
GEOMToolsGUI* GEOMToolsGUI::GetGEOMToolsGUI()
{
  if ( myGUIObject == 0 ) {
    // init GEOMToolsGUI only once
    myGUIObject = new GEOMToolsGUI();
  }
  return myGUIObject;
}

//=======================================================================
// function : GEOMToolsGUI()
// purpose  : Constructor
//=======================================================================
GEOMToolsGUI::GEOMToolsGUI() : GEOMGUI()
{
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
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());

  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  switch (theCommandID)
    {
    case 31: // COPY
      {
	OnEditCopy();
	break;
      }
    case 33: // DELETE
      {
	OnEditDelete();
	break;
      }
    case 111: // IMPORT BREP
    case 112: // IMPORT IGES
    case 113: // IMPORT STEP
      {
	Import();
	break;
      }
    case 121: // EXPORT BREP
    case 122: // EXPORT IGES
    case 123: // EXPORT STEP
      {
	Export();
	break;
      }
    case 411: // SETTINGS - ADD IN STUDY
      {
	// SAN -- TO BE REMOVED !!!
	break;
      }
    case 412: // SETTINGS - SHADING COLOR
      {
	OnSettingsColor();
	break;
      }
    case 413: // SETTINGS - ISOS
      {
	OnSettingsIsos();
	break;
      }
    case 414: // SETTINGS : STEP VALUE FOR SPIN BOXES
      {
	OnSettingsStep();
	break;
      }
    case 804: // ADD IN STUDY - POPUP VIEWER
      {
	// SAN -- TO BE REMOVED !!!!
	break;
      }
    case 901: // RENAME
      {
	OnRename();
	break;
      }
    case 5103: // CHECK GEOMETRY
      {
	OnCheckGeometry();
	break;
      }
    case 5104: // LOAD SCRIPT
      {
	OnLoadScript();
	break;
      }
    case 8032: // COLOR - POPUP VIEWER
      {
	OnColor();
	break;
      }
    case 8033: // TRANSPARENCY - POPUP VIEWER
      {
	OnTransparency();
	break;
      }
    case 8034: // ISOS - POPUP VIEWER
      {
	OnNbIsos();
	break;
      }
    case 9024 : // OPEN - OBJBROSER POPUP
      {
	OnOpen();
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
   SALOME_Selection* Sel = SALOME_Selection::Selection(
    QAD_Application::getDesktop()->getActiveStudy()->getSelection() );
    
  if ( Sel->IObjectCount() == 0 )
    return;
  
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  
  bool aLocked = aStudy->GetProperties()->IsLocked();
  if ( aLocked ) {
    QAD_MessageBox::warn1 ( (QWidget*)QAD_Application::getDesktop(),
			    QObject::tr("WRN_WARNING"), 
			    QObject::tr("WRN_STUDY_LOCKED"),
			    QObject::tr("BUT_OK") );
    return;
  }
  
  if ( QAD_MessageBox::warn2( QAD_Application::getDesktop(),
                              tr( "GEOM_WRN_WARNING" ),
                              tr( "GEOM_REALLY_DELETE" ),
                              tr( "GEOM_BUT_YES" ),
                              tr( "GEOM_BUT_NO" ), 1, 0, 0 ) != 1 )
    return;

  int nbSf = QAD_Application::getDesktop()->getActiveStudy()->getStudyFramesCount();

  Standard_Boolean found;
  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::AttributeIOR_var     anIOR;

  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
  
  QAD_Operation* op = new SALOMEGUI_ImportOperation( QAD_Application::getDesktop()->getActiveStudy() );

  op->start();
  
  Standard_Boolean    deleted = false;
  
  for ( ;It.More();It.Next() )
  {
    Handle( SALOME_InteractiveObject ) IObject = It.Value();
    
    if ( !IObject->hasEntry() )
      continue;

    SALOMEDS::SObject_var SO = aStudy->FindObjectID( IObject->getEntry() );

    /* Erase child graphical objects */

    SALOMEDS::ChildIterator_var it = aStudy->NewChildIterator( SO );
    for ( ; it->More();it->Next() )
    {
      SALOMEDS::SObject_var CSO = it->Value();

      if ( CSO->FindAttribute( anAttr, "AttributeIOR" ) )
      {
        anIOR = SALOMEDS::AttributeIOR::_narrow( anAttr );

        // Delete child( s ) shape in Client :

        const TCollection_AsciiString ASCior( anIOR->Value() ) ;
        GeometryGUI::GetGeomGUI()->GetShapeReader().RemoveShapeFromBuffer( ASCior );

        for ( int i = 0; i < nbSf; i++ )
        {
          GEOM::GEOM_Object_var aGeomObj = GEOM::GEOM_Object::_narrow( CSO->GetObject() );
          if ( !aGeomObj->_is_nil() )
            GEOM_Displayer().Erase( aGeomObj, true );
        }
      }
    }

    /* Erase main graphical object */

    for ( int i = 0; i < nbSf; i++ )
    {
      QAD_StudyFrame* sf = QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame( i );
      GEOM_Displayer().Erase( IObject, true );
    }

    /* Delete main shape in Client : */

    if ( SO->FindAttribute( anAttr, "AttributeIOR" ) )
    {
      anIOR = SALOMEDS::AttributeIOR::_narrow( anAttr );
      const TCollection_AsciiString ASCIor( anIOR->Value() ) ;
      GeometryGUI::GetGeomGUI()->GetShapeReader().RemoveShapeFromBuffer( ASCIor );
    }

    /* Erase objects in Study */

    SALOMEDS::SObject_var obj = aStudy->FindObjectID( IObject->getEntry() );
    if ( !obj->_is_nil() )
    {
      SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
      aStudyBuilder->RemoveObject( obj );
      
      GEOM::GEOM_Object_var aGeomObj = GEOM::GEOM_Object::_narrow( obj->GetObject() );
      if ( !aGeomObj->_is_nil() )
        GeometryGUI::GetGeomGUI()->GetGeomGen()->RemoveObject( aGeomObj );
      
      deleted = true;
    }
  }   

  if ( deleted )
    op->finish();
  else
    op->abort();

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
/* 
 SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection() );
  GEOM::string_array_var listIOR = new GEOM::string_array;

  const SALOME_ListIO& List = Sel->StoredIObjects();

  myGeomBase->ConvertListOfIOInListOfIOR(List, listIOR);

  Sel->ClearIObjects();

  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  int aStudyID = aStudy->StudyId();    

  for (unsigned int ind = 0; ind < listIOR->length();ind++) {
    GEOM::GEOM_Object_var aShapeInit = myGeom->GetIORFromString(listIOR[ind]);
    try {
      GEOM::GEOM_IInsertOperations_var IOp =  myGeom->GetIInsertOperations(aStudyID);
      GEOM::GEOM_Object_var result = IOp->MakeCopy(aShapeInit) ;
      myGeomBase->Display(result);
    }
    catch  (const SALOME::SALOME_Exception& S_ex) {
      QtCatchCorbaException(S_ex);
    }
  }
  
  QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_READY"));
*/
}


//=====================================================================================
// function : Import
// purpose  : BRep, Iges, Step
//=====================================================================================
bool GEOMToolsGUI::Import()
{
  QAD_Study* aDoc = QAD_Application::getDesktop()->getActiveStudy();
  SALOMEDS::Study_var aStudy = aDoc->getStudyDocument();

  bool aLocked = aStudy->GetProperties()->IsLocked();
  if ( aLocked ) {
    QAD_MessageBox::warn1 ( (QWidget*)QAD_Application::getDesktop(),
			   QObject::tr("WRN_WARNING"), 
			   QObject::tr("WRN_STUDY_LOCKED"),
			   QObject::tr("BUT_OK") );
    return false;
  }

  GEOM::GEOM_IInsertOperations_var aInsOp = GeometryGUI::GetGeomGUI()->GetGeomGen()->GetIInsertOperations( aStudy->StudyId() );
  if ( aInsOp->_is_nil() )
    return false;
  
  GEOM::GEOM_Object_var anObj;

  // Obtain a list of available import formats
  FilterMap aMap;
  GEOM::string_array_var aFormats, aPatterns;
  aInsOp->ImportTranslators( aFormats, aPatterns );
  for ( int i = 0, n = aFormats->length(); i < n; i++ ) 
    aMap.insert( (char*)aPatterns[i], (char*)aFormats[i] );

  QString fileType;

  QString file = getFileName(QAD_Application::getDesktop(), "", aMap, tr("GEOM_MEN_IMPORT"), true, fileType );
  if( file.isEmpty() || fileType.isEmpty() ) 
    return false;
  
  GEOM_Operation* anOp = new GEOM_Operation( aDoc, aInsOp.in() );
  try {
    QAD_WaitCursor wc;
    QAD_Application::getDesktop()->putInfo( tr("GEOM_PRP_LOADING").arg(QAD_Tools::getFileNameFromPath( file )) );

    anOp->start();

    anObj = aInsOp->Import( file.latin1(), fileType.latin1() );  

    if ( !anObj->_is_nil() && aInsOp->IsDone() ) {
      anObj->SetName( GEOMBase::GetDefaultName( QObject::tr( "GEOM_IMPORT" ) ).latin1() );
      QString aPublishObjName = GEOMBase::GetDefaultName( QAD_Tools::getFileNameFromPath( file ));
      GeometryGUI::GetGeomGUI()->GetGeomGen()->PublishInStudy(
        aStudy, SALOMEDS::SObject::_nil(), anObj, aPublishObjName );

      GEOM_Displayer().Display( anObj.in() );

      anOp->finish();      
    }
    else {
      anOp->abort();
      wc.stop();
      QAD_MessageBox::error1( QAD_Application::getDesktop(), 
			     QObject::tr( "GEOM_ERROR" ),
			     QObject::tr("GEOM_PRP_ABORT") + "\n" + QString( aInsOp->GetErrorCode() ), 
			     QObject::tr("BUT_OK") );
    }
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
    anOp->abort();
    return false;
  }

  return true;
}


//=====================================================================================
// function : Export
// purpose  : BRep, Iges, Step
//=====================================================================================
bool GEOMToolsGUI::Export()
{
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  GEOM::GEOM_IInsertOperations_var aInsOp = GeometryGUI::GetGeomGUI()->GetGeomGen()->GetIInsertOperations( aStudy->StudyId() );
  if ( aInsOp->_is_nil() )
    return false;

  // Obtain a list of available export formats
  FilterMap aMap;
  GEOM::string_array_var aFormats, aPatterns;
  aInsOp->ExportTranslators( aFormats, aPatterns );
  for ( int i = 0, n = aFormats->length(); i < n; i++ ) 
    aMap.insert( (char*)aPatterns[i], (char*)aFormats[i] );
  
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );

  for(;It.More();It.Next()) {
    Handle(SALOME_InteractiveObject) IObject = It.Value();
    Standard_Boolean found;
    GEOM::GEOM_Object_var anObj = GEOMBase::ConvertIOinGEOMObject(IObject, found);

    if ( !found || anObj->_is_nil() )
      continue;

    QString fileType;
    QString file = getFileName(QAD_Application::getDesktop(), 
			       QString( IObject->getName() ),
			       aMap,
			       tr("GEOM_MEN_EXPORT"),
			       false,
			       fileType);

    // User has pressed "Cancel" --> stop the operation
    if ( file.isEmpty() || fileType.isEmpty() )
      return false;

    //      Standard_Boolean result = BRepTools::Write(Shape->Shape(), strdup(file.latin1()) );
    try {
      QAD_WaitCursor wc;
      aInsOp->Export( anObj, file, fileType.latin1() );
      if ( !aInsOp->IsDone() ) {
	wc.stop();
	QAD_MessageBox::error1( QAD_Application::getDesktop(), 
			       QObject::tr( "GEOM_ERROR" ),
			       QObject::tr("GEOM_PRP_ABORT") + "\n" + QString( aInsOp->GetErrorCode() ), 
			       QObject::tr("BUT_OK") );
	return false;
      }
    }  
    catch (const SALOME::SALOME_Exception& S_ex) {
      QtCatchCorbaException(S_ex);
    }
  }

  return true; 
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  GEOMGUI* GetLibGUI()
  {
    return GEOMToolsGUI::GetGEOMToolsGUI();
  }
}
