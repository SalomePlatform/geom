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
//  File   : OperationGUI_FilletDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "OperationGUI_FilletDlg.h"
#include "DlgRef_1Sel1Spin.h"
#include "DlgRef_2Sel1Spin.h"
#include "QAD_Desktop.h"
#include "qpixmap.h"
#include <TColStd_IndexedMapOfInteger.hxx>
#include <list>

#include "GEOMImpl_Types.hxx"

//=================================================================================
// class    : OperationGUI_FilletDlg()
// purpose  : Constructs a OperationGUI_FilletDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_FilletDlg::OperationGUI_FilletDlg( QWidget* parent, SALOME_Selection* Sel )
  :GEOMBase_Skeleton( parent, "OperationGUI_FilletDlg", Sel, false,
     WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  myConstructorId = -1;
  mySelection = Sel;
  
  QPixmap image0( QAD_Desktop::getResourceManager()->loadPixmap( "GEOM", tr( "ICON_DLG_FILLET_ALL" ) ) );
  QPixmap image1( QAD_Desktop::getResourceManager()->loadPixmap( "GEOM", tr( "ICON_DLG_FILLET_EDGE" ) ) );
  QPixmap image2( QAD_Desktop::getResourceManager()->loadPixmap( "GEOM", tr( "ICON_DLG_FILLET_FACE" ) ) );
  
  QPixmap iconSelect( QAD_Desktop::getResourceManager()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setCaption( tr( "GEOM_FILLET_TITLE" ) );

  /***************************************************************/
  GroupConstructors->setTitle( tr( "GEOM_FILLET" ) );
  RadioButton1->setPixmap( image0 );
  RadioButton2->setPixmap( image1 );
  RadioButton3->setPixmap( image2 );

  Group1 = new DlgRef_1Sel1Spin( this, "Group1" );
  Group1->GroupBox1->setTitle( tr( "GEOM_FILLET_ALL" ) );
  Group1->TextLabel1->setText( tr( "GEOM_MAIN_OBJECT" ) );
  Group1->TextLabel2->setText( tr( "GEOM_RADIUS" ) );
  Group1->PushButton1->setPixmap( iconSelect );
  Group1->LineEdit1->setReadOnly( true );

  Group2 = new DlgRef_2Sel1Spin( this, "Group2" );
  Group2->GroupBox1->setTitle( tr( "GEOM_FILLET_EDGES" ) );
  Group2->TextLabel1->setText( tr( "GEOM_MAIN_OBJECT" ) );
  Group2->TextLabel2->setText( tr( "SELECTED_EDGES" ) );
  Group2->TextLabel3->setText( tr( "GEOM_RADIUS" ) );
  Group2->PushButton1->setPixmap( iconSelect );
  Group2->PushButton2->setPixmap( iconSelect );
  Group2->LineEdit1->setReadOnly( true );
  Group2->LineEdit2->setReadOnly( true );

  Group3 = new DlgRef_2Sel1Spin( this, "Group3" );
  Group3->GroupBox1->setTitle(tr( "GEOM_FILLET_FACES" ) );
  Group3->TextLabel1->setText(tr( "GEOM_MAIN_OBJECT" ) );
  Group3->TextLabel2->setText(tr( "SELECTED_FACES" ) );
  Group3->TextLabel3->setText(tr( "GEOM_RADIUS" ) );
  Group3->PushButton1->setPixmap( iconSelect );
  Group3->PushButton2->setPixmap( iconSelect );
  Group3->LineEdit1->setReadOnly( true );
  Group3->LineEdit2->setReadOnly( true );

  Layout1->addWidget( Group1, 2, 0 );
  Layout1->addWidget( Group2, 2, 0 );
  Layout1->addWidget( Group3, 2, 0 );
  /***************************************************************/

  double SpecificStep = 10.0;
  Group1->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, SpecificStep, 3);
  Group2->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, SpecificStep, 3);
  Group3->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, SpecificStep, 3);

  /* Initialisations */
  Init( mySelection );
}


//=================================================================================
// function : ~OperationGUI_FilletDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_FilletDlg::~OperationGUI_FilletDlg()
{  
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::Init( SALOME_Selection* Sel )
{
  mySelection = Sel;
  myConstructorId = -1;
  reset();
  RadioButton1->setChecked( true );
  myEditCurrentArgument = Group1->LineEdit1;

  // main buttons
  connect( buttonOk   , SIGNAL( clicked() ), this, SLOT( ClickOnOk()    ) );
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  // group box
  connect( GroupConstructors, SIGNAL( clicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  // push buttons
  connect(Group1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  // line edits
  connect(Group1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group2->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group3->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  // spin boxes  
  connect(Group1->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group2->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

    // selection
  connect( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  

  initName( tr( "GEOM_FILLET" ) );

  Group2->hide();
  Group3->hide();
  Group1->show();

  this->show();
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void OperationGUI_FilletDlg::ConstructorsClicked( int constructorId )
{
  if ( QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() != VIEW_OCC )
  {
    RadioButton1->setChecked( true );
    return;
  }

  if ( myConstructorId == constructorId )
    return;

  // Get radius from previous widget
  double R = 5;
  if      ( myConstructorId == 0 ) R = Group1->SpinBox_DX->GetValue();
  else if ( myConstructorId == 1 ) R = Group2->SpinBox_DX->GetValue();
  else                             R = Group3->SpinBox_DX->GetValue();

  myConstructorId = constructorId;

  switch ( constructorId )
  {
    case 0:
        Group1->show();
        Group2->hide();
        Group3->hide();
        Group1->SpinBox_DX->SetValue( R );
    break;
    case 1:
        Group1->hide();
        Group2->show();
        Group3->hide();
        Group2->SpinBox_DX->SetValue( R );
    break;
    case 2:
        Group1->hide();
        Group2->hide();
        Group3->show();
        Group3->SpinBox_DX->SetValue( R );
    break;
    default:
    break;
  }

  if      ( constructorId == 0 ) myEditCurrentArgument = Group1->LineEdit1;
  else if ( constructorId == 1 ) myEditCurrentArgument = Group2->LineEdit1;
  else                           myEditCurrentArgument = Group3->LineEdit1;

  activateSelection();
  enableWidgets();

  if ( !myShape->_is_nil() )
  {
    myEditCurrentArgument->setText( GEOMBase::GetName( myShape ) );
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
  else
    myEditCurrentArgument->setText( "" );

  displayPreview();  
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_FilletDlg::ClickOnApply()
{
    if ( !onAccept() )
    return false;

  initName();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void OperationGUI_FilletDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText( "" );

  // If selection of main object is activated
  if ( myEditCurrentArgument == Group1->LineEdit1 ||
       myEditCurrentArgument == Group2->LineEdit1 ||
       myEditCurrentArgument == Group3->LineEdit1 )
  {
    if ( mySelection->IObjectCount() == 1 )
    {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( mySelection->firstIObject(), aResult );

      if ( aResult && !anObj->_is_nil() )
      {
        myShape = anObj;
        myEditCurrentArgument->setText( GEOMBase::GetName( anObj ) );
        displayPreview();
        enableWidgets();
        return;
      }
    }

    myShape = GEOM::GEOM_Object::_nil();
    enableWidgets();
  }
  // If face or edge selection is activated
  else if ( myEditCurrentArgument == Group2->LineEdit2 ||
            myEditCurrentArgument == Group3->LineEdit2 )
  {
    if ( mySelection->IObjectCount() == 1 )
    {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( mySelection->firstIObject(), aResult );

      if ( aResult && !anObj->_is_nil() )
      {
         TColStd_IndexedMapOfInteger anIndexes;
         mySelection->GetIndex( mySelection->firstIObject(), anIndexes );

         if ( anIndexes.Extent() > 0 )
         {
           QString aName;
           if ( anIndexes.Extent() == 1 )
           {
             int anIndex = anIndexes( 1 );

             aName = QString( GEOMBase::GetName( anObj ) ) + QString( ":%1" ).arg( anIndex );
           }
           else
             aName = tr( "GEOM_MEN_POPUP_NAME" ).arg( anIndexes.Extent() );

           myEditCurrentArgument->setText( aName );

           if ( myConstructorId == 1 )
             myEdges = anIndexes;
           else
             myFaces = anIndexes;
           
           displayPreview();
           return;
         }
      }
    }
    myFaces.Clear();
  }
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::LineEditReturnPressed()
{
  QLineEdit* send = ( QLineEdit* )sender();
  
  if ( send == Group1->LineEdit1 )
    myEditCurrentArgument = Group1->LineEdit1;
  else if ( send == Group2->LineEdit1 )
    myEditCurrentArgument = Group2->LineEdit1;
  else if ( send == Group3->LineEdit1 )
    myEditCurrentArgument = Group3->LineEdit1;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if ( send == Group1->PushButton1 )
  {
    Group1->LineEdit1->setFocus();
    myEditCurrentArgument = Group1->LineEdit1;
  }
  else if( send == Group2->PushButton1 )
  {
    Group2->LineEdit1->setFocus();
    myEditCurrentArgument = Group2->LineEdit1;
  }
  else if( send == Group2->PushButton2 )
  {
    Group2->LineEdit2->setFocus();
    myEditCurrentArgument = Group2->LineEdit2;
  }
  else if ( send == Group3->PushButton1 )
  {
    Group3->LineEdit1->setFocus();
    myEditCurrentArgument = Group3->LineEdit1;
  }
  else if( send == Group3->PushButton2 )
  {
    Group3->LineEdit1->setFocus();
    myEditCurrentArgument = Group3->LineEdit2;
  }

  activateSelection();
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( mySelection, SIGNAL(currentSelectionChanged()),
           this, SLOT( SelectionIntoArgument() ) );

  activateSelection();
  displayPreview();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::enterEvent( QEvent* e )
{
  if ( !GroupConstructors->isEnabled() )
    this->ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::ValueChangedInSpinBox( double newValue )
{
  displayPreview();
}

//=================================================================================
// function : reset()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::reset()
{
  // Set Initial values of spinboxes
  Group1->SpinBox_DX->SetValue( 5 );
  Group2->SpinBox_DX->SetValue( 5 );
  Group3->SpinBox_DX->SetValue( 5 );

  Group1->LineEdit1->setText( "" );
  Group2->LineEdit1->setText( "" );
  Group2->LineEdit2->setText( "" );
  Group3->LineEdit1->setText( "" );
  Group3->LineEdit2->setText( "" );

  // constructor id
  int aConstructorId = getConstructorId();

  if      ( aConstructorId == 0 ) myEditCurrentArgument = Group1->LineEdit1;
  else if ( aConstructorId == 1 ) myEditCurrentArgument = Group2->LineEdit1;
  else                            myEditCurrentArgument = Group3->LineEdit1;

  myShape = GEOM::GEOM_Object::_nil();

  myFaces.Clear();
  myEdges.Clear();

  erasePreview( true );

  activateSelection();

  enableWidgets();
}

//=================================================================================
// function : getConstructorId()
// purpose  :
//=================================================================================
int OperationGUI_FilletDlg::getConstructorId() const
{
  return GroupConstructors->id( GroupConstructors->selected() );
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void OperationGUI_FilletDlg::activateSelection()
{

  if ( !myShape->_is_nil() && myEditCurrentArgument == Group2->LineEdit2 )
    localSelection( myShape, TopAbs_EDGE );
  else if ( !myShape->_is_nil() && myEditCurrentArgument == Group3->LineEdit2 )
    localSelection( myShape, TopAbs_FACE );
  else
  {
    TColStd_MapOfInteger aMap;
    aMap.Add( GEOM_SHELL );
    aMap.Add( GEOM_SOLID );
    aMap.Add( GEOM_COMPOUND );
    globalSelection( aMap );
  }

  SelectionIntoArgument();
}

//=================================================================================
// function : enableWidgets
// purpose  : Enable widgets of faces in accordance with value of main object
//=================================================================================
void OperationGUI_FilletDlg::enableWidgets()
{
  int anId = getConstructorId();

  bool toEnable = !myShape->_is_nil();

  if ( anId == 1 )
  {
    Group2->LineEdit2->setEnabled( toEnable );
    Group2->PushButton2->setEnabled( toEnable );
    
    if ( !toEnable )
    {
      Group2->LineEdit2->setText( "" );
      myEdges.Clear();
    }
  }
  else if ( anId == 2 )
  {
    Group3->LineEdit2->setEnabled( toEnable );
    Group3->PushButton2->setEnabled( toEnable );

    if ( !toEnable )
    {
      Group3->LineEdit2->setText( "" );
      myFaces.Clear();
    }
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_FilletDlg::createOperation()
{
  return getGeomEngine()->GetILocalOperations( getStudyId() );
}

//=================================================================================
// function : ClickOnApply()
// purpose  : Verify validity of input data
//=================================================================================
bool OperationGUI_FilletDlg::isValid( QString& )
{
  switch ( getConstructorId() )
  {
    case 0: return !myShape->_is_nil();
    case 1: return !myShape->_is_nil() && myEdges.Extent() > 0;
    case 2: return !myShape->_is_nil() && myFaces.Extent() > 0;
    default: return false;
  }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_FilletDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  int anId = getConstructorId();
  if ( anId == 0 )
    anObj = GEOM::GEOM_ILocalOperations::_narrow(
      getOperation() )->MakeFilletAll( myShape,
                                       getRadius() );
  else if ( anId == 1 )
  {
    GEOM::ListOfLong_var aList = new GEOM::ListOfLong;
    aList->length( myEdges.Extent() );

    for ( int i = 1, n = myEdges.Extent(); i <= n; i++ )
      aList[ i - 1 ] = myEdges( i );

    anObj = GEOM::GEOM_ILocalOperations::_narrow(
      getOperation() )->MakeFilletEdges( myShape, getRadius(), aList );
  }
  else if ( anId == 2 )
  {
    int i = 0;
    GEOM::ListOfLong_var aList = new GEOM::ListOfLong;
    aList->length( myFaces.Extent() );

    for ( int i = 1, n = myFaces.Extent(); i <= n; i++ )
      aList[ i - 1 ] = myFaces( i );

    anObj = GEOM::GEOM_ILocalOperations::_narrow(
      getOperation() )->MakeFilletFaces( myShape, getRadius(), aList );
  }

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}

//=================================================================================
// function : getRadius
// purpose  : Get radius     
//=================================================================================
double OperationGUI_FilletDlg::getRadius() const
{
  int anId = getConstructorId();
  if      ( anId == 0 ) return Group1->SpinBox_DX->GetValue();
  else if ( anId == 1 ) return Group2->SpinBox_DX->GetValue();
  else                  return Group3->SpinBox_DX->GetValue();
}







