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
#include "STLPlugin_GUI.h"
#include "STLPlugin_ExportDlg.h"

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

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(STLPlugin)

typedef GEOM::GenericObjPtr<GEOM::ISTLOperations> STLOpPtr;

//=======================================================================
// function : STLPlugin_GUI()
// purpose  : Constructor
//=======================================================================
STLPlugin_GUI::STLPlugin_GUI( GeometryGUI* parent ) : GEOMPluginGUI( parent )
{
}

//=======================================================================
// function : ~STLPlugin_GUI
// purpose  : Destructor
//=======================================================================
STLPlugin_GUI::~STLPlugin_GUI()
{
}

//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool STLPlugin_GUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  QString cmd;
  switch ( theCommandID ) {
  case 1:
    cmd = "Export_STL"; break;
  case 2:
    cmd = "Import_STL"; break;
  default:
    break;
  }
  return OnGUIEvent( cmd, parent );
}

//=======================================================================
// function : OnGUIEvent()
// purpose  :
//=======================================================================
bool STLPlugin_GUI::OnGUIEvent( const QString& theCommandID, SUIT_Desktop* parent )
{
  bool result = false;

  if( theCommandID == "Export_STL" ) {
    result = exportSTL( parent );
  }
  else if (theCommandID == "Import_STL") {
    result = importSTL( parent );
  }
  else {
    getGeometryGUI()->getApp()->putInfo( tr("GEOM_PRP_COMMAND").arg( theCommandID ) );
  }

  return true;
}

//=======================================================================
// function : importSTL
// purpose  :
//=======================================================================
bool STLPlugin_GUI::importSTL( SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*> ( app->activeStudy() );
  if ( !study ) return false;

  SALOMEDS::Study_var dsStudy = GeometryGUI::ClientStudyToStudy( study->studyDS() );
  GEOM::GEOM_IOperations_var op = GeometryGUI::GetGeomGen()->GetPluginOperations( dsStudy->StudyId(), "STLPluginEngine" );
  STLOpPtr stlOp = GEOM::ISTLOperations::_narrow( op );
  if ( stlOp.isNull() ) return false;
  
  QStringList fileNames = app->getOpenFileNames( SUIT_FileDlg::getLastVisitedPath().isEmpty() ? QDir::currentPath() : QString(""),
						 tr( "STL_FILES" ),
						 tr( "IMPORT_TITLE" ),
						 parent );
  if ( fileNames.count() > 0 )
  {
    QStringList entryList;
    QStringList errors;
    
    foreach( QString fileName, fileNames )
    {
      SUIT_OverrideCursor wc;
      GEOM_Operation transaction( app, stlOp.get() );
      
      try
      {
	app->putInfo( tr( "GEOM_PRP_LOADING" ).arg( fileName ) );
	transaction.start();
	GEOM::ListOfGO_var result = stlOp->ImportSTL( fileName.toUtf8().constData() );
	if ( result->length() > 0 && stlOp->IsDone() )
	{
	  GEOM::GEOM_Object_var main = result[0];
	  QString publishName = GEOMBase::GetDefaultName( SUIT_Tools::file( fileName, true ) );
	  SALOMEDS::SObject_var so = GeometryGUI::GetGeomGen()->PublishInStudy( dsStudy,
										SALOMEDS::SObject::_nil(),
										main.in(),
										publishName.toUtf8().constData() );
	  
	  entryList.append( so->GetID() );
	  transaction.commit();
	  GEOM_Displayer( study ).Display( main.in() );
          main->UnRegister();
	}
	else
	{
	  transaction.abort();
	  errors.append( QString( "%1 : %2" ).arg( fileName ).arg( stlOp->GetErrorCode() ) );
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
// function : exportSTL
// purpose  :
//=======================================================================
bool STLPlugin_GUI::exportSTL( SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*> ( app->activeStudy() );
  if ( !study ) return false;

  SALOMEDS::Study_var dsStudy = GeometryGUI::ClientStudyToStudy( study->studyDS() );
  GEOM::GEOM_IOperations_var op = GeometryGUI::GetGeomGen()->GetPluginOperations( dsStudy->StudyId(), "STLPluginEngine" );
  STLOpPtr stlOp = GEOM::ISTLOperations::_narrow( op );
  if ( stlOp.isNull() ) return false;

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

    bool isASCII = false;
    double deflection = 0.;
    bool isRelative = false;
    QString fileName = STLPlugin_ExportDlg::getFileName( io,
							 tr( "STL_FILES" ),
							 tr( "EXPORT_TITLE" ),
							 parent,
							 isASCII,
							 deflection,
							 isRelative );

    if ( fileName.isEmpty() )
      return false;
    
    SUIT_OverrideCursor wc;
    
    GEOM_Operation transaction( app, stlOp.get() );
    
    try
    {
      app->putInfo( tr( "GEOM_PRP_EXPORT" ).arg( fileName ) );
      transaction.start();
      
      stlOp->ExportSTL( obj, fileName.toUtf8().constData(), isASCII, deflection, isRelative  );
      
      if ( stlOp->IsDone() )
      {
	transaction.commit();
      }
      else
      {
	transaction.abort();
	SUIT_MessageBox::critical( parent,
				   tr( "GEOM_ERROR" ),
				   tr( "GEOM_PRP_ABORT" ) + "\n" + tr( stlOp->GetErrorCode() ) );
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
    return new STLPlugin_GUI( parent );
  }
}
