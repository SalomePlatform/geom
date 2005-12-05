// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
#include "GEOMGUI_OCCSelector.h"

#include <LightApp_DataSubOwner.h>

#include <OCCViewer_ViewModel.h>

#include <SALOME_InteractiveObject.hxx>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <AIS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <NCollection_DataMap.hxx>


//================================================================
// Function : GEOMGUI_OCCSelector
// Purpose  : 
//================================================================
GEOMGUI_OCCSelector::GEOMGUI_OCCSelector( OCCViewer_Viewer* viewer, SUIT_SelectionMgr* mgr )
: LightApp_OCCSelector( viewer, mgr )
{
}

//================================================================
// Function : ~GEOMGUI_OCCSelector
// Purpose  : 
//================================================================
GEOMGUI_OCCSelector::~GEOMGUI_OCCSelector()
{
}

//================================================================
// Function : getSelection
// Purpose  : 
//================================================================
void GEOMGUI_OCCSelector::getSelection( SUIT_DataOwnerPtrList& aList ) const
{
  OCCViewer_Viewer* vw = viewer();
  if ( !vw )
    return;

  Handle(AIS_InteractiveContext) ic = vw->getAISContext();
  
  if ( ic->HasOpenedContext() )
    {
      for ( ic->InitSelected(); ic->MoreSelected(); ic->NextSelected() )
	{
	  Handle(SelectMgr_EntityOwner) anOwner = ic->SelectedOwner();
	  if ( anOwner.IsNull() )
	    continue;
	  
	  Handle(AIS_InteractiveObject) io = Handle(AIS_InteractiveObject)::DownCast( anOwner->Selectable() );
	  
	  QString entryStr = entry( io );
	  int index = -1; 
	  
	  if ( anOwner->ComesFromDecomposition() ) // == Local Selection
	    {
	      TopoDS_Shape subShape = anOwner->Shape();
	      Handle(AIS_Shape) aisShape = Handle(AIS_Shape)::DownCast( io );
	      if ( !aisShape.IsNull() )
		{
		  TopoDS_Shape bigShape = aisShape->Shape();
		  
		  TopTools_IndexedMapOfShape subShapes;
		  TopExp::MapShapes( bigShape, subShapes );
		  index = subShapes.FindIndex( subShape );
		}
	    }
	  
	  if ( !entryStr.isEmpty() )
	    {
	      LightApp_DataOwner* owner;
	      if ( index > -1 ) // Local Selection
		owner = new LightApp_DataSubOwner( entryStr, index );
	      else // Global Selection
		owner = new LightApp_DataOwner( entryStr );

	      aList.append( SUIT_DataOwnerPtr( owner ) );
	    }
	}
    }
  else
    {
      for ( ic->InitCurrent(); ic->MoreCurrent(); ic->NextCurrent() )
	{
	  Handle(AIS_InteractiveObject) io = ic->Current();
	  
	  QString entryStr = entry( io );
	  
	  if ( !entryStr.isEmpty() )
	    {
	      LightApp_DataOwner* owner = new LightApp_DataOwner( entryStr );
	      aList.append( SUIT_DataOwnerPtr( owner ) );
	    }
	}
    }
}

//================================================================
// Function : getEntityOwners
// Purpose  : 
//================================================================
static void getEntityOwners( const Handle(AIS_InteractiveObject)& theObj,
			     const Handle(AIS_InteractiveContext)& theIC,
			     SelectMgr_IndexedMapOfOwner& theMap )
{
  if ( theObj.IsNull() || theIC.IsNull() )
    return;

  TColStd_ListOfInteger modes;
  theIC->ActivatedModes( theObj, modes );

  TColStd_ListIteratorOfListOfInteger itr( modes );
  for (; itr.More(); itr.Next() ) {
    int m = itr.Value();
    if ( !theObj->HasSelection( m ) )
      continue;

    Handle(SelectMgr_Selection) sel = theObj->Selection( m );

    for ( sel->Init(); sel->More(); sel->Next() ) {
      Handle(SelectBasics_SensitiveEntity) entity = sel->Sensitive();
      if ( entity.IsNull() )
	continue;

      Handle(SelectMgr_EntityOwner) owner =
	Handle(SelectMgr_EntityOwner)::DownCast(entity->OwnerId());
      if ( !owner.IsNull() )
	theMap.Add( owner );
    }
  }
}

//================================================================
// Function : setSelection
// Purpose  : 
//================================================================
void GEOMGUI_OCCSelector::setSelection( const SUIT_DataOwnerPtrList& aList )
{
  OCCViewer_Viewer* vw = viewer();
  if ( !vw )
    return;

  Handle(AIS_InteractiveContext) ic = vw->getAISContext();

#ifndef WNT
  NCollection_DataMap<TCollection_AsciiString, TColStd_IndexedMapOfInteger> indexesMap; // "entry - list_of_int" map for LOCAL selection
#else
  NCollection_DataMap<Standard_CString, TColStd_IndexedMapOfInteger> indexesMap; // "entry - list_of_int" map for LOCAL selection
#endif
  QMap<QString,int> globalSelMap; // only Key=entry from this map is used.  value(int) is NOT used at all.
  SelectMgr_IndexedMapOfOwner ownersmap; // map of owners to be selected
  
  AIS_ListOfInteractive aDispList;
  ic->DisplayedObjects( aDispList );

  // build a map of data owner indexes to be selected.
  // "entry - to - list_of_ids" map
  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const LightApp_DataSubOwner* subOwner = dynamic_cast<const LightApp_DataSubOwner*>( (*itr).operator->() );
    if ( subOwner )
    {
      QString entry = subOwner->entry();
#ifndef WNT
      if ( indexesMap.IsBound( TCollection_AsciiString((char*)entry.latin1())))
#else
	  if ( indexesMap.IsBound( (char*)entry.latin1()))
#endif
      {
	TColStd_IndexedMapOfInteger& subIndexes = indexesMap.ChangeFind((char*)entry.latin1());
	subIndexes.Add( subOwner->index() );
	//indexesMap.replace( entry, subIndexes );
      }
      else
      {
	TColStd_IndexedMapOfInteger subIndexes;
	subIndexes.Add( subOwner->index() );
	indexesMap.Bind((char*)entry.latin1(), subIndexes);
      }
    } 
    else // the owner is NOT a sub owner, maybe it is a DataOwner == GLOBAL selection
    {
      const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*itr).operator->() );
      if ( owner )
      {
	globalSelMap[owner->entry()] = 1;
      }
    }
  }

  // get all owners.  Fill "entry - list_of_owners" map.
  for ( AIS_ListIteratorOfListOfInteractive it( aDispList ); it.More(); it.Next() )
  {
    Handle(AIS_InteractiveObject) io = it.Value();
    QString entryStr = entry( io );
    if ( !entryStr.isEmpty() )
    {
      //EntryToListOfOwnerMap entryOwnersMap; // "entry - list_of_owners" map.  temporary use.
      SelectMgr_IndexedMapOfOwner owners;
      getEntityOwners( io, ic, owners ); // get all owners

      for  ( int i = 1, n = owners.Extent(); i <= n; i++ ) 
      {
	Handle(SelectMgr_EntityOwner) anOwner = owners( i );
	if ( anOwner.IsNull() || !anOwner->HasShape() )
	  continue;

	// GLOBAL selection
	if ( !anOwner->ComesFromDecomposition() && globalSelMap.contains( entryStr ) ) 
	{
	  ownersmap.Add( anOwner );
	}
	// LOCAL selection
	else
	{
	  Handle(AIS_Shape) aisShape = Handle(AIS_Shape)::DownCast( io );

	  if ( !aisShape.IsNull() && indexesMap.IsBound( (char*)entryStr.latin1() ) )
	  {
	    TopoDS_Shape shape = aisShape->Shape();
	    TopTools_IndexedMapOfShape aMapOfShapes;
	    TopExp::MapShapes( shape, aMapOfShapes );
	    const TColStd_IndexedMapOfInteger& subIndexes = indexesMap.ChangeFind((char*)entryStr.latin1());

	    const TopoDS_Shape& aSubShape = anOwner->Shape();
	    int  aSubShapeId = aMapOfShapes.FindIndex( aSubShape );

	    // check if the "sub_shape_index" is found in the "map of indexes for this entry",
	    // which was passes in the parameter
	    if ( subIndexes.Contains( aSubShapeId ) )
	    {
	      ownersmap.Add( anOwner );
	    }
	  }
        } // end of local selection
      } // end of for(owners)
    }// end of if(entry)
  }// end of for(AIS_all_ios)

  vw->unHighlightAll( false );

  // DO the selection
  for  ( int i = 1, n = ownersmap.Extent(); i <= n; i++ ) 
  {
    Handle(SelectMgr_EntityOwner) owner = ownersmap( i );
    if ( owner->State() )
      continue;

    if ( ic->HasOpenedContext() )
      ic->AddOrRemoveSelected( owner, false );
    else
      ic->AddOrRemoveSelected( Handle(AIS_InteractiveObject)::DownCast(owner->Selectable()), false );
  }

  vw->update();
}
