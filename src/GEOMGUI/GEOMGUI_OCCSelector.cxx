// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// File   : GEOMGUI_OCCSelector.cxx
// Author : Alexander SOLOVYOV, Open CASCADE S.A.S. (alexander.solovyov@opencascade.com)
//
#include "GEOMGUI_OCCSelector.h"

#include <Basics_OCCTVersion.hxx>

#include <LightApp_DataSubOwner.h>

#include <OCCViewer_ViewModel.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Study.h>
#include <SALOME_InteractiveObject.hxx>

#include <StdSelect_BRepOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <SelectBasics_SensitiveEntity.hxx>

#include <AIS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
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
  if (!vw)
    return;

  Handle(AIS_InteractiveContext) ic = vw->getAISContext();

  if (ic->HasOpenedContext())
  {
    TopoDS_Shape curBigShape;
    TopTools_IndexedMapOfShape subShapes;

    for (ic->InitSelected(); ic->MoreSelected(); ic->NextSelected())
    {
      Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast(ic->SelectedOwner());
      if (anOwner.IsNull())
        continue;

      Handle(AIS_InteractiveObject) io = Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());

      QString entryStr = entry(io);
      int index = -1;

      if (anOwner->ComesFromDecomposition()) // == Local Selection
      {
        TopoDS_Shape subShape = anOwner->Shape();
        Handle(AIS_Shape) aisShape = Handle(AIS_Shape)::DownCast(io);
        if (!aisShape.IsNull())
        {
          TopoDS_Shape bigShape = aisShape->Shape();

          if (!bigShape.IsEqual(curBigShape))
          {
            curBigShape = bigShape;
            subShapes.Clear();
            TopExp::MapShapes(bigShape, subShapes);
          }
          index = subShapes.FindIndex(subShape);
        }
      }

      if (!entryStr.isEmpty())
      {
        Handle(SALOME_InteractiveObject) anIO = Handle(SALOME_InteractiveObject)::DownCast(io->GetOwner()); 
        LightApp_DataOwner* owner;
        if (index > -1) // Local Selection
          owner = new LightApp_DataSubOwner (entryStr, index);
        else if ( !anIO.IsNull() ) // Global Selection
          owner = new LightApp_DataOwner( anIO );

        aList.append(SUIT_DataOwnerPtr(owner));
      }
    }
  }
  else
  {
    for (ic->InitCurrent(); ic->MoreCurrent(); ic->NextCurrent())
    {
      Handle(AIS_InteractiveObject) io = ic->Current();

      QString entryStr = entry( io );

      if ( !entryStr.isEmpty() )
      {
        Handle(SALOME_InteractiveObject) anIO = Handle(SALOME_InteractiveObject)::DownCast(io->GetOwner()); 
        if ( !anIO.IsNull() ) {
          LightApp_DataOwner* owner = new LightApp_DataOwner( anIO );
          aList.append( SUIT_DataOwnerPtr( owner ) );
        }
      }
    }
  }

  // add externally selected objects
  SUIT_DataOwnerPtrList::const_iterator anExtIter;
  for (anExtIter = mySelectedExternals.begin(); anExtIter != mySelectedExternals.end(); anExtIter++)
  {
    aList.append(*anExtIter);
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
#if OCC_VERSION_LARGE > 0x06080100
      const Handle(SelectMgr_SensitiveEntity) aHSenEntity = sel->Sensitive();
      if( aHSenEntity.IsNull() )
        continue;

      Handle(SelectBasics_SensitiveEntity) entity = aHSenEntity->BaseSensitive();
#else
      Handle(SelectBasics_SensitiveEntity) entity = sel->Sensitive();
#endif
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
  if ( !vw || !vw->isSelectionEnabled())
    return;

  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
  bool anAutoBringToFront = aResourceMgr->booleanValue( "Geometry", "auto_bring_to_front", false );

  Handle(AIS_InteractiveContext) ic = vw->getAISContext();

  // "entry - list_of_int" map for LOCAL selection
  NCollection_DataMap<TCollection_AsciiString, TColStd_IndexedMapOfInteger> indexesMap;

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
      if ( indexesMap.IsBound( TCollection_AsciiString(entry.toLatin1().data())))
      {
        TColStd_IndexedMapOfInteger& subIndexes = indexesMap.ChangeFind(entry.toLatin1().data());
        subIndexes.Add( subOwner->index() );
        //indexesMap.replace( entry, subIndexes );
      }
      else
      {
        TColStd_IndexedMapOfInteger subIndexes;
        subIndexes.Add( subOwner->index() );
        indexesMap.Bind(entry.toLatin1().data(), subIndexes);
      }
    }
    else // the owner is NOT a sub owner, maybe it is a DataOwner == GLOBAL selection
    {
      const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*itr).operator->() );
      if ( owner )
      {
        SalomeApp_Study* appStudy =
          dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
        QString anEntry = appStudy->referencedToEntry( owner->entry() );

        globalSelMap[anEntry] = 1;
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

      int i, n = owners.Extent();

      // 1. Prepare map of shapes for local selection
      TopTools_IndexedMapOfShape aMapOfShapes;
      bool isLocal = false;

      Handle(StdSelect_BRepOwner) anOwner;
      for (i = 1; i <= n && !isLocal; i++)
      {
        anOwner = Handle(StdSelect_BRepOwner)::DownCast(owners( i ));
        if (!anOwner.IsNull() && anOwner->HasShape())
        {
          if (anOwner->ComesFromDecomposition() || !globalSelMap.contains(entryStr))
          {
            // has a local selection
            Handle(AIS_Shape) aisShape = Handle(AIS_Shape)::DownCast( io );
            if (!aisShape.IsNull() && indexesMap.IsBound(entryStr.toLatin1().data()))
            {
              isLocal = true;
              TopoDS_Shape shape = aisShape->Shape();
              aMapOfShapes.Clear();
              TopExp::MapShapes(shape, aMapOfShapes);
            }
          }
        }
      }

      // 2. Process all owners
      for (i = 1; i <= n; i++)
      {
        anOwner = Handle(StdSelect_BRepOwner)::DownCast(owners( i ));

        if ( anOwner.IsNull() || !anOwner->HasShape() )
          continue;

        // GLOBAL selection
        if ( !anOwner->ComesFromDecomposition() && globalSelMap.contains( entryStr ) )
        {
          ownersmap.Add( anOwner );
          globalSelMap[entryStr]++;
        }
        // LOCAL selection
        else
        {
          if (isLocal)
          {
            const TColStd_IndexedMapOfInteger& subIndexes =
              indexesMap.ChangeFind(entryStr.toLatin1().data());

            const TopoDS_Shape& aSubShape = anOwner->Shape();
            int aSubShapeId = aMapOfShapes.FindIndex( aSubShape );

            // check if the "sub_shape_index" is found in the "map of indexes for this entry",
            // which was passes in the parameter
            if ( subIndexes.Contains( aSubShapeId ) )
            {
              ownersmap.Add( anOwner );
            }
          }
        } // end of LOCAL selection
      } // end of for(owners)
    } // end of if(entry)
  } // end of for(AIS_all_ios)

  vw->unHighlightAll( false );

  // DO the selection
  int i = 1, n = ownersmap.Extent();
  bool isAutoHilight = ic->AutomaticHilight();
  ic->SetAutomaticHilight(Standard_False); //Bug 17269: for better performance
  for  (; i <= n; i++)
  {
    Handle(SelectMgr_EntityOwner) owner = ownersmap( i );
    if ( owner->State() )
      continue;

    if ( ic->HasOpenedContext() )
      ic->AddOrRemoveSelected( owner, false );
    else
      ic->AddOrRemoveSelected( Handle(AIS_InteractiveObject)::DownCast(owner->Selectable()), false );
  }

  ic->SetAutomaticHilight(isAutoHilight); //Bug 17269: restore mode

  //rnv: In case Automatic Bring To Front viewer will be updated later
  if(!anAutoBringToFront) {
    if (n < 3000)
      ic->HilightSelected(Standard_True);
    else
      vw->update();
  }

  // fill extra selected
  mySelectedExternals.clear();
  for ( SUIT_DataOwnerPtrList::const_iterator itr2 = aList.begin(); itr2 != aList.end(); ++itr2 )
  {
    const LightApp_DataSubOwner* subOwner =
      dynamic_cast<const LightApp_DataSubOwner*>( (*itr2).operator->() );
    if ( !subOwner )
    {
      const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( (*itr2).operator->() );
      if ( owner )
      {
        SalomeApp_Study* appStudy =
          dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
        QString anEntry = appStudy->referencedToEntry( owner->entry() );
        if (globalSelMap[anEntry] == 1) mySelectedExternals.append(*itr2);
      }
    }
  }
}
