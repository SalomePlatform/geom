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
#include "VTKPlugin_GUI.h"
#include "VTKPlugin_ExportDlg.h"

// GUI includes
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>
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
#include CORBA_SERVER_HEADER(VTKPlugin)

typedef GEOM::GenericObjPtr<GEOM::IVTKOperations> VTKOpPtr;

//=======================================================================
// function : VTKPlugin_GUI()
// purpose  : Constructor
//=======================================================================
VTKPlugin_GUI::VTKPlugin_GUI( GeometryGUI* parent ) : GEOMPluginGUI( parent )
{
}

//=======================================================================
// function : ~VTKPlugin_GUI
// purpose  : Destructor
//=======================================================================
VTKPlugin_GUI::~VTKPlugin_GUI()
{
}

//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool VTKPlugin_GUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  QString cmd;
  switch ( theCommandID ) {
  case 1:
    cmd = "Export_VTK"; break;
  default:
    break;
  }
  return OnGUIEvent( cmd, parent );
}

//=======================================================================
// function : OnGUIEvent()
// purpose  :
//=======================================================================
bool VTKPlugin_GUI::OnGUIEvent( const QString& theCommandID, SUIT_Desktop* parent )
{
  bool result = false;

  if( theCommandID == "Export_VTK" ) {
    result = exportVTK( parent );
  }
  else {
    getGeometryGUI()->getApp()->putInfo( tr("GEOM_PRP_COMMAND").arg( theCommandID ) );
  }

  return true;
}

//=======================================================================
// function : exportVTK
// purpose  :
//=======================================================================
bool VTKPlugin_GUI::exportVTK( SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*> ( app->activeStudy() );
  if ( !study ) return false;

  SALOMEDS::Study_var dsStudy = GeometryGUI::ClientStudyToStudy( study->studyDS() );
  GEOM::GEOM_IOperations_var op = GeometryGUI::GetGeomGen()->GetPluginOperations( dsStudy->StudyId(), "VTKPluginEngine" );
  VTKOpPtr vtkOp = GEOM::IVTKOperations::_narrow( op );
  if ( vtkOp.isNull() ) return false;

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

    double deflection = 0.;
    QString fileName = VTKPlugin_ExportDlg::getFileName( io,
							 tr( "VTK_FILES" ),
							 tr( "EXPORT_TITLE" ),
							 parent,
							 deflection );

    if ( fileName.isEmpty() )
      return false;
    
    SUIT_OverrideCursor wc;
    
    GEOM_Operation transaction( app, vtkOp.get() );
    
    try
    {
      app->putInfo( tr( "GEOM_PRP_EXPORT" ).arg( fileName ) );
      transaction.start();
      
      vtkOp->ExportVTK( obj, fileName.toUtf8().constData(), deflection  );
      
      if ( vtkOp->IsDone() )
      {
	transaction.commit();
      }
      else
      {
	transaction.abort();
	SUIT_MessageBox::critical( parent,
				   tr( "GEOM_ERROR" ),
				   tr( "GEOM_PRP_ABORT" ) + "\n" + tr( vtkOp->GetErrorCode() ) );
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
    return new VTKPlugin_GUI( parent );
  }
}
