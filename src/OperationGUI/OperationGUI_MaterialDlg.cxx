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

// GEOM GEOMGUI : GUI for Geometry component
// File   : OperationGUI_MaterialDlg.cxx
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//
#include "OperationGUI_MaterialDlg.h"
#include "OperationGUI_PartitionDlg.h"

#include <DlgRef.h>
#include <GEOMBase.h>
#include <GeometryGUI.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

//=================================================================================
// class    : OperationGUI_MaterialDlg()
// purpose  : Constructs a OperationGUI_MaterialDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_MaterialDlg::OperationGUI_MaterialDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                                    GEOM::ListOfGO ListShapes, bool modal )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal )
{
  myListShapes = ListShapes;
  
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_PARTITION" ) ) );

  setWindowTitle( tr( "GEOM_MATERIAL_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_PARTITION" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1List1Spin1Btn( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_PARTITION" ) );

  QStringList columns;
  columns << tr( "GEOM_MATERIAL_SHAPE" ) << tr( "GEOM_MATERIAL_MATERIAL" );
  GroupPoints->ListView1->setHeaderLabels( columns );
  GroupPoints->ListView1->setSelectionMode( QAbstractItemView::ExtendedSelection );
  GroupPoints->ListView1->setSortingEnabled( false );

  GroupPoints->TextLabel1->setText( tr( "GEOM_MATERIAL_ID" ) );
  GroupPoints->PushButton1->setText( tr( "GEOM_MATERIAL_SET" ) );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  /***************************************************************/

  setHelpFileName("partition_page.html"); 

  Init();
}

//=================================================================================
// function : ~OperationGUI_MaterialDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_MaterialDlg::~OperationGUI_MaterialDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_MaterialDlg::Init()
{
  // get materials list from the parent dialog
  OperationGUI_PartitionDlg* aParentDlg =
    qobject_cast<OperationGUI_PartitionDlg*>( parentWidget() );
  if ( aParentDlg )
    myListMaterials = aParentDlg->GetListMaterials();

  /* list filling */
  for ( int ind = 0; ind < myListShapes.length(); ind++ ) {
    GEOM::GEOM_Object_var anObject = myListShapes[ind];
    if ( !anObject->_is_nil() ) {
      QStringList labels;
      labels << GEOMBase::GetName( anObject );
      labels << ( ind < myListMaterials.length() ? 
                  QString::number( myListMaterials[ind] ) : QString( "0" ) );
      GroupPoints->ListView1->addTopLevelItem( new QTreeWidgetItem( labels ) );
    }
  }

  /* signals and slots connections */
  connect( buttonOk(), SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  buttonApply()->setAttribute( Qt::WA_DeleteOnClose );
  buttonApply()->close();
  buttonCancel()->setText( tr( "GEOM_BUT_CANCEL" ) );
  
  connect( GroupPoints->PushButton1, SIGNAL( clicked() ), this, SLOT( SetMaterial() ) );
  
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_MaterialDlg::ClickOnOk()
{
  SUIT_Session::session()->activeApplication()->putInfo( "" );

  int nbSh = myListShapes.length();  
  myListMaterials.length( nbSh );

  QTreeWidgetItemIterator it( GroupPoints->ListView1 );
  for ( int i = 0; *it; it++, i++ )
    myListMaterials[i] = (*it)->text( 1 ).toInt();

  // set materials list to the parent dialog
  OperationGUI_PartitionDlg* aParentDlg =
    qobject_cast<OperationGUI_PartitionDlg*>( parentWidget() );
  if ( aParentDlg )
    aParentDlg->SetListMaterials( myListMaterials );

  ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void OperationGUI_MaterialDlg::SelectionIntoArgument()
{
  /*QString aString = ""; // name of selection

  int nbSel = GEOMBase::GetNameOfSelectedIObjects(selectedIO(), aString);
  if (nbSel < 1) {
    return;
  }

  //myGeomBase->ConvertListOfIOInListOfIOR(selectedIO(), myListShapes);

  // no simulation
  return;*/
}


//=================================================================================
// function : SetMaterial()
// purpose  :
//=================================================================================
void OperationGUI_MaterialDlg::SetMaterial()
{
  QString aMatIdStr = QString::number( GroupPoints->SpinBox1->value() );
  QList<QTreeWidgetItem*> selectedItems = GroupPoints->ListView1->selectedItems();

  QListIterator<QTreeWidgetItem*> it( selectedItems );
  while ( it.hasNext() ) {
    it.next()->setText( 1, aMatIdStr );
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_MaterialDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_MaterialDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    this->ActivateThisDialog();
}
