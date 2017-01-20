// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : GEOMGUI_TextTreeSelector.cxx
// Author :
//
#include "GEOMGUI_TextTreeSelector.h"
#include "GEOMGUI_TextTreeWdg.h"
#include "GEOMGUI_AnnotationMgr.h"

#include "LightApp_DataOwner.h"
#include "LightApp_DataObject.h"
#include "LightApp_Application.h"
#include <SUIT_Session.h>
#include <SUIT_DataObjectIterator.h>

/*!
  \class GEOMGUI_TextTreeSelector
  \brief Text tree selection handler class. It provides selection synchronization between
  application and text tree widget. This selector listens item selection changed signal of
  text tree widget to emit common selection changed signal of SUIT selector to start selection
  synchronization. In get/setSelection selector processes annotation items. These items have
  specific entry generated in annotation manager, having the "object entry:annotation_id" structure.
*/

/*!
  \brief Constructor.
  \param widget text tree widget
  \param theManager selection manager
*/
GEOMGUI_TextTreeSelector::GEOMGUI_TextTreeSelector( GEOMGUI_TextTreeWdg* theWidget,
                                                    GEOMGUI_AnnotationMgr* theAnnotationMgr,
                                                    SUIT_SelectionMgr* theManager )
: SUIT_Selector( theManager, theWidget ),
  myTextTree( theWidget ), myAnnotationMgr( theAnnotationMgr )
{
  if ( myTextTree ) {
    connect( myTextTree, SIGNAL( itemSelectionChanged() ), this, SLOT( onSelectionChanged() ) );
  }
}

/*!
  \brief Destructor.
*/
GEOMGUI_TextTreeSelector::~GEOMGUI_TextTreeSelector()
{
}

/*!
  \brief Get object browser.
  \return a pointer to the text tree widget
*/
GEOMGUI_TextTreeWdg* GEOMGUI_TextTreeSelector::textTree() const
{
  return myTextTree;
}

/*!
  \brief Get selector unique type.
  \return selector type
*/
QString GEOMGUI_TextTreeSelector::type() const
{ 
  return "TextTree";
}

/*!
  \brief Called when the Object browser selection is changed. It emits signal to synchronize
  selection in application.
*/
void GEOMGUI_TextTreeSelector::onSelectionChanged()
{
  mySelectedList.clear();
  selectionChanged();
}

/*!
  \brief Get list of currently selected annotation objects.
  \param theList list to be filled with the selected objects owners
  The list contains owners for interactive objects of annotations
*/
void GEOMGUI_TextTreeSelector::getSelection( SUIT_DataOwnerPtrList& theList ) const
{
  if ( mySelectedList.count() == 0 ) {

    GEOMGUI_TextTreeSelector* aThisSelector = (GEOMGUI_TextTreeSelector*)this;
    QMap<QString, QList<int> > aSelectedAnnotations;
    myTextTree->getSelected(aSelectedAnnotations);

    QMap<QString, QList<int> >::const_iterator anIt = aSelectedAnnotations.begin(),
                                               aLast = aSelectedAnnotations.end();
    for ( ; anIt != aLast; anIt++ ) {
      QString anEntry = anIt.key();
      QList<int> anIndices = anIt.value();
      QList<int>::const_iterator anIdIt = anIndices.begin(), anIdLast = anIndices.end();
      for ( ; anIdIt != anIdLast; anIdIt++ ) {
        int anIndex = *anIdIt;
        Handle(SALOME_InteractiveObject) anIO = myAnnotationMgr->FindInteractiveObject( anEntry, anIndex );
        if ( anIO.IsNull() )
          continue;

        LightApp_DataOwner* owner = new LightApp_DataOwner( anIO );
        aThisSelector->mySelectedList.append( SUIT_DataOwnerPtr( owner ) );
      }
    }
  }
  theList = mySelectedList;
}

/*!
  \brief Set selection.
  \param theList list of the object owners to be set selected
*/
void GEOMGUI_TextTreeSelector::setSelection( const SUIT_DataOwnerPtrList& theList )
{
  if ( !myTextTree )
    return;

  QMap<QString, QList<int> > aSelectedAnnotations;
  DataObjectList objList;
  for ( SUIT_DataOwnerPtrList::const_iterator it = theList.begin(); 
        it != theList.end(); ++it ) {
    const LightApp_DataOwner* anOwner = dynamic_cast<const LightApp_DataOwner*>( (*it).operator->() );
    if ( !anOwner )
      continue;
    Handle(SALOME_InteractiveObject) anIO = anOwner->IO();
    if ( anIO.IsNull() )
      continue;

    QString anIOEntry = anIO->getEntry();
    QStringList anAnnotationInfo = anIOEntry.split(GEOMGUI_AnnotationMgr::GetEntrySeparator());
    if (anAnnotationInfo.size() != 2 )
      continue;

    QString anEntry = anAnnotationInfo[0];
    int anAnnotationId = anAnnotationInfo[1].toInt();
    if ( anAnnotationId <  0)
      continue;

    QList<int> anIndices;
    if ( aSelectedAnnotations.contains( anEntry ) )
      anIndices = aSelectedAnnotations[anEntry];

    if ( !anIndices.contains( anAnnotationId ) )
      anIndices.append( anAnnotationId );

    aSelectedAnnotations[anEntry] = anIndices;
  }
  myTextTree->setSelected(aSelectedAnnotations);
  mySelectedList.clear();
}

