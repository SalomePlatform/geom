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
//  File   : RepairGUI_RemoveHolesDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "RepairGUI_RemoveHolesDlg.h"

#include "QAD_RightFrame.h"
#include "QAD_Desktop.h"
#include "QAD_MessageBox.h"

#include "GEOM_Displayer.h"
#include "OCCViewer_Viewer3d.h"
#include "SALOME_ListIteratorOfListIO.hxx"

#include "GEOMImpl_Types.hxx"

#include <TopAbs.hxx>


//=================================================================================
// class    : RepairGUI_RemoveHolesDlg()
// purpose  : Constructs a RepairGUI_RemoveHolesDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_RemoveHolesDlg::RepairGUI_RemoveHolesDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_SUPPRESS_HOLES")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_REMOVE_HOLES_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_REMOVE_HOLES_TITLE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel_Ext(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_HOLES"));
  GroupPoints->TextLabel1->setText(tr("GEOM_SELECTED_SHAPE"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);

  myAllChk = new QCheckBox( tr( "GEOM_REMOVE_ALL_HOLES" ), GroupPoints->GroupBox1 );

  QGridLayout* aSelectWiresLay = new QGridLayout( 0, 1, 1, 0, 6, "aSelectWiresLay");

  mySelectWiresBtn = new QPushButton( GroupPoints->GroupBox1, "mySelectWiresBtn" );
  mySelectWiresBtn->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, mySelectWiresBtn->sizePolicy().hasHeightForWidth() ) );
  mySelectWiresBtn->setText( trUtf8( "" ) );
  mySelectWiresBtn->setPixmap(image1);

  mySelectWiresEdt = new QLineEdit( GroupPoints->GroupBox1, "mySelectWiresEdt" );
  mySelectWiresEdt->setReadOnly( true );

  mySelectWiresLbl = new QLabel( tr( "GEOM_WIRES_TO_REMOVE" ), GroupPoints->GroupBox1, "TextLabel1" );
  mySelectWiresLbl->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, mySelectWiresLbl->sizePolicy().hasHeightForWidth() ) );

  aSelectWiresLay->addWidget( mySelectWiresLbl, 0, 0 );
	aSelectWiresLay->addWidget( mySelectWiresBtn, 0, 1 );
  aSelectWiresLay->addWidget( mySelectWiresEdt, 0, 2 );
  QSpacerItem* spacer = new QSpacerItem( 0, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
  aSelectWiresLay->addItem( spacer, 1, 2 );

  QGridLayout* aDetectLay = new QGridLayout( 0, 1, 1, 0, 6, "aDetectLay" );  
	myFreeBoundBtn = new QPushButton( tr( "GEOM_DETECT" ), GroupPoints->GroupBox1 );
  QLabel* aLbl = new QLabel( tr( "GEOM_FREE_BOUNDARIES" ), GroupPoints->GroupBox1 );
  aDetectLay->addWidget( aLbl, 0, 0 );
  aDetectLay->addWidget( myFreeBoundBtn, 0, 1 );
  
  GroupPoints->getGroupBoxLayout()->addMultiCellWidget( myAllChk, 1, 1, 0, 2 );
  GroupPoints->getGroupBoxLayout()->addLayout( aSelectWiresLay, 2, 0 );
  GroupPoints->getGroupBoxLayout()->addLayout( aDetectLay, 3, 0 );
  /***************************************************************/

  Init();
}


//=================================================================================
// function : ~RepairGUI_RemoveHolesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_RemoveHolesDlg::~RepairGUI_RemoveHolesDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  
  myObject = GEOM::GEOM_Object::_nil();
  myWiresInd = new GEOM::short_array();
  myWiresInd->length( 0 );

  myGeomGUI->SetState( 0 );
  initSelection();

  myClosed = -1;
  myOpen = -1;
  
  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(mySelectWiresBtn, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(mySelectWiresEdt, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  connect( myAllChk, SIGNAL( clicked() ), this, SLOT( onRemoveAllClicked() ) );
  connect( myFreeBoundBtn, SIGNAL(clicked()), this, SLOT(onDetect()) );

  initName( tr( "REMOVE_HOLES_NEW_OBJ_NAME" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_RemoveHolesDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}



//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_RemoveHolesDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setText("");
  mySelectWiresEdt->setText("");
  myObject = GEOM::GEOM_Object::_nil();
  myWiresInd->length( 0 );

  initSelection();

  return true;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection
//=================================================================================
void RepairGUI_RemoveHolesDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");
  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) myObject = GEOM::GEOM_Object::_nil();
  else if ( myEditCurrentArgument == mySelectWiresEdt ) myWiresInd->length( 0 );

  if ( mySelection->IObjectCount() == 1 )
  {
    Handle(SALOME_InteractiveObject) anIO = mySelection->firstIObject();

    if ( myEditCurrentArgument == GroupPoints->LineEdit1 )	// face selection
    {
      Standard_Boolean aRes;
      myObject = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
      if ( aRes && GEOMBase::IsShape( myObject ) )
        myEditCurrentArgument->setText( GEOMBase::GetName( myObject ) );
      else
        myObject = GEOM::GEOM_Object::_nil();
    }
    else if ( myEditCurrentArgument == mySelectWiresEdt && !myAllChk->isChecked() )
    {
      TColStd_IndexedMapOfInteger aMap;
      mySelection->GetIndex( anIO, aMap );
      const int n = aMap.Extent();
      myWiresInd->length( n );
      for ( int i = 1; i <= n; i++ )
	myWiresInd[i-1] = aMap( i );
      if ( n )
	myEditCurrentArgument->setText( QString::number( n ) + "_" + tr( "GEOM_WIRE" ) + tr( "_S_" ) );
    }
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::SetEditCurrentArgument()
{
  const QObject* send = sender();
  if ( send == GroupPoints->PushButton1 )
    myEditCurrentArgument = GroupPoints->LineEdit1; 
  else if ( send == mySelectWiresBtn && !myObject->_is_nil() )
    myEditCurrentArgument = mySelectWiresEdt;
  if ( myEditCurrentArgument )
  {
    initSelection();
    myEditCurrentArgument->setFocus();
    SelectionIntoArgument();
  }
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::LineEditReturnPressed()
{
  const QObject* send = sender();
  if( send == GroupPoints->LineEdit1 || send == mySelectWiresEdt )
  {
    myEditCurrentArgument = (QLineEdit*)send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::DeactivateActiveDialog()
{
  myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setText("");
  mySelectWiresEdt->setText("");
  myObject = GEOM::GEOM_Object::_nil();
  myWiresInd->length( 0 );

  myClosed = -1;
  myOpen = -1;

  myGeomGUI->SetState( 0 );
  initSelection();
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_RemoveHolesDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::closeEvent(QCloseEvent* e)
{
  myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_RemoveHolesDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_RemoveHolesDlg::isValid( QString& msg )
{
  myClosed = -1;
  return !myObject->_is_nil() && ( IsPreview() || myAllChk->isChecked() || myWiresInd->length() );
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_RemoveHolesDlg::execute( ObjectList& objects )
{
  bool aResult = false;
  if ( IsPreview() ) // called from onDetect(): detect free boundary edges, highlight them (add to objects), display message dialog
  {
    GEOM::ListOfGO_var aClosed, anOpen;

    aResult = GEOM::GEOM_IHealingOperations::_narrow( getOperation() )->GetFreeBoundary( myObject, aClosed, anOpen );

    if ( aResult )
    {
      myClosed = aClosed->length();
      myOpen = anOpen->length();
      int i;
      for ( i = 0; i < myClosed; i++ )
	objects.push_back( aClosed[i]._retn() );
      for ( i = 0; i < myOpen; i++ )
	objects.push_back( anOpen[i]._retn() );
    }
    else
      myClosed = -1;
  }
  else
  {
    GEOM::GEOM_Object_var anObj = GEOM::GEOM_IHealingOperations::_narrow( getOperation() )->FillHoles( myObject, myWiresInd );
    aResult = !anObj->_is_nil();
    if ( aResult )
      objects.push_back( anObj._retn() );
  }
	
  return aResult;
}

//=================================================================================
// function : onRemoveAllClicked
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::onRemoveAllClicked()
{
  bool b = myAllChk->isOn();
  mySelectWiresLbl->setEnabled( !b );
  mySelectWiresBtn->setEnabled( !b );
  mySelectWiresEdt->setEnabled( !b );
  if ( b )
  {
    mySelectWiresEdt->setText( "" );
    myWiresInd->length( 0 );
  }
}

//=================================================================================
// function : initSelection
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::initSelection()
{
  if ( myEditCurrentArgument == GroupPoints->LineEdit1 )
  {
    TColStd_MapOfInteger aTypes;
    aTypes.Add( GEOM_COMPOUND );
    aTypes.Add( GEOM_SOLID );
    aTypes.Add( GEOM_SHELL );
    aTypes.Add( GEOM_FACE );
    
    globalSelection( aTypes );
  }
  else if ( myEditCurrentArgument == mySelectWiresEdt )
  {
    localSelection( myObject, TopAbs_EDGE );
    localSelection( myObject, TopAbs_WIRE );
  }
}


//=================================================================================
// function : onDetect
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::onDetect()
{
  displayPreview( false, true, true, 3 );

  // field myClosed,myOpen is initialized in execute() method, called by displayPreview().
  QString msg;
  if ( myClosed != -1 )
    msg = tr( "GEOM_FREE_BOUNDS_MSG" ).arg( myClosed + myOpen ).arg( myClosed ).arg( myOpen );
  else
    msg = tr( "GEOM_FREE_BOUNDS_ERROR" );
  QAD_MessageBox::info1( this, tr( "GEOM_FREE_BOUNDS_TLT" ), msg, "Close" );
}

