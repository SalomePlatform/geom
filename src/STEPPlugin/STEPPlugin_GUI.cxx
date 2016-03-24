// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "STEPPlugin_GUI.h"

// GUI includes
#include <SUIT_Desktop.h>
#include <SUIT_FileDlg.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_Tools.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SALOME_ListIO.hxx>

// GEOM includes
#include "GeometryGUI.h"
#include "GEOM_Operation.h"
#include "GEOMBase.h"
#include "GEOM_Displayer.h"
#include "GEOM_GenericObjPtr.h"
#include "STEPPlugin_ExportDlg.h"
#include "STEPPlugin_ImportDlg.h"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(STEPPlugin)

typedef GEOM::GenericObjPtr<GEOM::ISTEPOperations> STEPOpPtr;

//=======================================================================
// function : STEPPlugin_GUI()
// purpose  : Constructor
//=======================================================================
STEPPlugin_GUI::STEPPlugin_GUI( GeometryGUI* parent ) : GEOMPluginGUI( parent )
{
}

//=======================================================================
// function : ~STEPPlugin_GUI
// purpose  : Destructor
//=======================================================================
STEPPlugin_GUI::~STEPPlugin_GUI()
{
}

//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool STEPPlugin_GUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  QString cmd;
  switch ( theCommandID ) {
  case 1:
    cmd = "Export_STEP"; break;
  case 2:
    cmd = "Import_STEP"; break;
  default:
    break;
  }
  return OnGUIEvent( cmd, parent );
}

//=======================================================================
// function : OnGUIEvent()
// purpose  :
//=======================================================================
bool STEPPlugin_GUI::OnGUIEvent( const QString& theCommandID, SUIT_Desktop* parent )
{
  bool result = false;
  
  if ( theCommandID == "Export_STEP" )
  {
    result = exportSTEP( parent );
  }
  else if ( theCommandID == "Import_STEP" )
  {
    result = importSTEP( parent );
  }
  else
  {
    getGeometryGUI()->getApp()->putInfo( tr("GEOM_PRP_COMMAND").arg( theCommandID ) );
  }

  return result;
}

//=======================================================================
// function : importSTEP
// purpose  :
//=======================================================================
bool STEPPlugin_GUI::importSTEP( SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*> ( app->activeStudy() );
  if ( !study ) return false;

  SALOMEDS::Study_var dsStudy = GeometryGUI::ClientStudyToStudy( study->studyDS() );
  GEOM::GEOM_IOperations_var op = GeometryGUI::GetGeomGen()->GetPluginOperations( dsStudy->StudyId(), "STEPPluginEngine" );
  STEPOpPtr stepOp = GEOM::ISTEPOperations::_narrow( op );
  if ( stepOp.isNull() ) return false;

  bool        isCreateAssemblies = true;
  QStringList fileNames          = STEPPlugin_ImportDlg::getOpenFileNames
        (SUIT_FileDlg::getLastVisitedPath().isEmpty() ? QDir::currentPath() : QString(""),
         tr("STEP_FILES"), tr("IMPORT_TITLE"), parent, isCreateAssemblies);

  if ( fileNames.count() > 0 )
  {
    QStringList entryList;
    QStringList errors;
    SUIT_MessageBox::StandardButton stepAnswer = SUIT_MessageBox::NoButton;
    
    for ( int i = 0; i < fileNames.count(); i++ )
    {
      QString fileName = fileNames.at( i );
      SUIT_OverrideCursor wc;
      GEOM_Operation transaction( app, stepOp.get() );
      bool ignoreUnits = false;
      
      try
      {
	app->putInfo( tr( "GEOM_PRP_LOADING" ).arg( SUIT_Tools::file( fileName, true ) ) );
	transaction.start();
	
	CORBA::String_var units = stepOp->ReadValue( fileName.toUtf8().constData(), "LEN_UNITS" );
	QString unitsStr( units.in() );
	bool unitsOK = unitsStr.isEmpty() || unitsStr == "M" || unitsStr.toLower() == "metre";
	
	if ( !unitsOK )
	{
	  if( stepAnswer == SUIT_MessageBox::NoToAll )
	  {
	    ignoreUnits = true;
	  }
	  else if( stepAnswer != SUIT_MessageBox::YesToAll )
	  {
	    SUIT_MessageBox::StandardButtons btns = SUIT_MessageBox::Yes | SUIT_MessageBox::No | SUIT_MessageBox::Cancel;
	    if ( i < fileNames.count()-1 ) btns = btns | SUIT_MessageBox::YesToAll | SUIT_MessageBox::NoToAll;
	    stepAnswer = SUIT_MessageBox::question( parent,
						    tr( "WRN_WARNING" ),
						    tr( "SCALE_DIMENSIONS" ).arg( unitsStr ),
						    btns,
						    SUIT_MessageBox::No );
	    switch ( stepAnswer )
	    {
	    case SUIT_MessageBox::Cancel:
	      return true;             // cancel (break) import operation
	    case SUIT_MessageBox::Yes:
	    case SUIT_MessageBox::YesToAll:
	      break;                   // scaling is confirmed
	    case SUIT_MessageBox::No:
	    case SUIT_MessageBox::NoAll:
	      ignoreUnits = true;      // scaling is rejected
	    default:
	      break;
	    }
	  }
	}
	
	GEOM::ListOfGO_var result = stepOp->ImportSTEP
          (fileName.toUtf8().constData(), ignoreUnits, isCreateAssemblies);
	if ( result->length() > 0 && stepOp->IsDone() )
	{
	  GEOM::GEOM_Object_var main = result[0];
	  QString publishName = GEOMBase::GetDefaultName( SUIT_Tools::file( fileName, true ) );
	  SALOMEDS::SObject_var so = GeometryGUI::GetGeomGen()->PublishInStudy( dsStudy,
										SALOMEDS::SObject::_nil(),
										main.in(),
										publishName.toUtf8().constData() );
	  
	  entryList.append( so->GetID() );
	  for ( int i = 1, n = result->length(); i < n; i++ ) {
	    GEOM::GEOM_Object_ptr group = result[i];
	    CORBA::String_var grpName = group->GetName();
	    GeometryGUI::GetGeomGen()->AddInStudy( dsStudy, group, grpName.in(), main );
	  }
	  transaction.commit();
	  GEOM_Displayer( study ).Display( main.in() );
          main->UnRegister();
	}
	else
	{
	  transaction.abort();
	  errors.append( QString( "%1 : %2" ).arg( fileName ).arg( stepOp->GetErrorCode() ) );
	}
      }
      catch( const SALOME::SALOME_Exception& e )
      {
	transaction.abort();
      }
    }

    getGeometryGUI()->updateObjBrowser( true );
    app->browseObjects( entryList );
          
    if ( errors.count() > 0 )
    {
      SUIT_MessageBox::critical( parent,
				 tr( "GEOM_ERROR" ),
				 tr( "GEOM_IMPORT_ERRORS" ) + "\n" + errors.join( "\n" ) );
    }
  }
  return fileNames.count() > 0;
}

//=======================================================================
// function : exportSTEP
// purpose  :
//=======================================================================
bool STEPPlugin_GUI::exportSTEP( SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*> ( app->activeStudy() );
  if ( !study ) return false;

  SALOMEDS::Study_var dsStudy = GeometryGUI::ClientStudyToStudy( study->studyDS() );
  GEOM::GEOM_IOperations_var op = GeometryGUI::GetGeomGen()->GetPluginOperations( dsStudy->StudyId(), "STEPPluginEngine" );
  STEPOpPtr stepOp = GEOM::ISTEPOperations::_narrow( op );
  if ( stepOp.isNull() ) return false;

  LightApp_SelectionMgr* sm = app->selectionMgr();
  if ( !sm ) return false;
  
  SALOME_ListIO selectedObjects;
  sm->selectedObjects( selectedObjects );
  bool ok = false;
  
  SALOME_ListIteratorOfListIO it( selectedObjects );
  for ( ; it.More(); it.Next() )
  {
    Handle(SALOME_InteractiveObject) io = it.Value();
    GEOM::GEOM_Object_var obj = GEOMBase::ConvertIOinGEOMObject( io );
    
    if ( CORBA::is_nil( obj ) ) continue;
    
    GEOM::length_unit anUnit;
    QString fileName = STEPPlugin_ExportDlg::getFileName
					(QString( io->getName() ),
					 tr( "STEP_FILES" ),
					 tr( "EXPORT_TITLE" ),
					 parent, anUnit);
    
    if ( fileName.isEmpty() )
      return false;
    
    SUIT_OverrideCursor wc;
    
    GEOM_Operation transaction( app, stepOp.get() );
    
    try
    {
      app->putInfo( tr( "GEOM_PRP_EXPORT" ).arg( fileName ) );
      transaction.start();
      
      stepOp->ExportSTEP( obj, fileName.toUtf8().constData(), anUnit);
      
      if ( stepOp->IsDone() )
      {
	transaction.commit();
      }
      else
      {
	transaction.abort();
	SUIT_MessageBox::critical( parent,
				   tr( "GEOM_ERROR" ),
				   tr( "GEOM_PRP_ABORT" ) + "\n" + tr( stepOp->GetErrorCode() ) );
	return false;
      }
    }
    catch ( const SALOME::SALOME_Exception& e )
    {
      transaction.abort();
      return false;
    }
    ok = true;
  }
  
  if ( !ok )
  {
    SUIT_MessageBox::warning( parent,
			      tr( "WRN_WARNING" ),
			      tr( "GEOM_WRN_NO_APPROPRIATE_SELECTION" ) );
  }
  return ok;
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WIN32
    __declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new STEPPlugin_GUI( parent );
  }
}
