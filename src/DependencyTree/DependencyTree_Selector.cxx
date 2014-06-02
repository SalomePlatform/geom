// Copyright (C) 2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "DependencyTree_Selector.h"
#include "DependencyTree_View.h"
#include "DependencyTree_ViewModel.h"
#include "DependencyTree_Object.h"

// GUI includes
#include <LightApp_DataOwner.h>

//GEOM includes
#include <GEOMBase.h>

#include <iostream>

DependencyTree_Selector::DependencyTree_Selector( DependencyTree_ViewModel* theModel, SUIT_SelectionMgr* theSelMgr )
:LightApp_GVSelector( (GraphicsView_Viewer*)theModel, theSelMgr )
{
  myView = dynamic_cast<DependencyTree_View*>( theModel->getActiveViewPort() );
}

DependencyTree_Selector::~DependencyTree_Selector()
{
}

//=================================================================================
// function : getSelection()
// purpose  : get list of selected Data Owner objects.
//=================================================================================
void DependencyTree_Selector::getSelection( SUIT_DataOwnerPtrList& theList ) const
{
  for( myView->initSelected(); myView->moreSelected(); myView->nextSelected() )
    if( DependencyTree_Object* treeObject = dynamic_cast<DependencyTree_Object*>( myView->selectedObject() ) ) {
      const char* entry;
      const char* name;
      QString studyEntry = treeObject->getGeomObject()->GetStudyEntry();
      if( studyEntry.isEmpty() ) {
        entry = treeObject->getEntry().c_str();
        name = "TEMP_IO_UNPUBLISHED";
      }
      else {
        entry = studyEntry.toStdString().c_str();
        name = "TEMP_IO";
      }
      Handle(SALOME_InteractiveObject) tmpIO =
        new SALOME_InteractiveObject( entry, "GEOM", name);

      theList.append( new LightApp_DataOwner( tmpIO ) );
  }
}

//=================================================================================
// function : setSelection()
// purpose  : set to selected list of Data Owner objects.
//=================================================================================
void DependencyTree_Selector::setSelection( const SUIT_DataOwnerPtrList& theList )
{
  myView->clearSelected();

  for ( SUIT_DataOwnerPtrList::const_iterator it = theList.begin(); it != theList.end(); ++it ) {
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*it).operator->() );
    if ( owner )
      if( !owner->IO().IsNull() ) {
        const char* name = owner->IO()->getName();
        const char* entry;
        if( strcmp( owner->IO()->getComponentDataType(), "GEOM" ) != 0 )
          return;

        if( strcmp( name, "TEMP_IO_UNPUBLISHED" ) == 0 )
          entry = owner->IO()->getEntry();
    	else {
          GEOM::GEOM_Object_var geomObject = GEOMBase::ConvertIOinGEOMObject( owner->IO() );
          if( geomObject->_is_nil() )
            return;
          entry = geomObject->GetEntry();
    	}
        DependencyTree_Object* object = myView->getObjectByEntry( QString( entry ) );
        if( object ) {
           myView->setSelected( object );
           object->select( object->pos().x(), object->pos().y(), object->getRect() );
        }
      }
  }
}
