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
//  File   : OperationGUI_ChamferDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM

//  $Header$

#include "OperationGUI_ChamferDlg.h"
#include "DlgRef_SpinBox.h"
#include "QAD_Desktop.h"
#include "qpixmap.h"
#include <list>

#include "GEOMImpl_Types.hxx"

//=================================================================================
// class    : OperationGUI_ChamferDlg()
// purpose  : Constructs a OperationGUI_ChamferDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_ChamferDlg::OperationGUI_ChamferDlg( QWidget* parent, SALOME_Selection* Sel )
: GEOMBase_Skeleton(parent, "ChamferDlg", Sel, false,
    WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{

  mySelection = Sel;
  myConstructorId = -1;

  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CHAMFER_ALL")));
  QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CHAMFER_EDGE")));
  QPixmap image3(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CHAMFER_FACE")));
  QPixmap iconSelect(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption( tr( "GEOM_CHAMFER_TITLE" ) );

  GroupConstructors->setTitle( tr( "GEOM_CHAMFER" ) );

  RadioButton1->setPixmap( image1 );
  RadioButton2->setPixmap( image2 );
  RadioButton3->setPixmap( image3 );

  // Create first group

  myGrp1 = new QGroupBox( 1, Qt::Horizontal, tr( "GEOM_CHAMFER_ALL" ), this );

  QGroupBox* aSelGrp1 = new QGroupBox( 3, Qt::Horizontal, myGrp1 );
  aSelGrp1->setFrameStyle( QFrame::NoFrame );
  aSelGrp1->setInsideMargin( 0 );

  createSelWg( tr( "GEOM_MAIN_OBJECT" ), iconSelect, aSelGrp1, MainObj1 );

  QGroupBox* aSpinGrp = new QGroupBox( 1, Qt::Vertical, myGrp1 );
  aSpinGrp->setFrameStyle( QFrame::NoFrame );
  aSpinGrp->setInsideMargin( 0 );

  new QLabel( tr( "D" ), aSpinGrp );
  mySpinBox[ SpinBox1 ] = new DlgRef_SpinBox( aSpinGrp );

  ( new QFrame( myGrp1 ) )->setSizePolicy(
    QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  // Create second group

  myGrp2 = new QGroupBox( 1, Qt::Horizontal, tr( "GEOM_CHAMFER_EDGES" ), this );

  QGroupBox* aSelGrp2 = new QGroupBox( 3, Qt::Horizontal, myGrp2 );
  aSelGrp2->setFrameStyle( QFrame::NoFrame );
  aSelGrp2->setInsideMargin( 0 );

  createSelWg( tr( "GEOM_MAIN_OBJECT" ), iconSelect, aSelGrp2, MainObj2 );
  createSelWg( tr( "FACE_1" ), iconSelect, aSelGrp2, Face1 );
  createSelWg( tr( "FACE_2" ), iconSelect, aSelGrp2, Face2 );

  aSpinGrp = new QGroupBox( 1, Qt::Vertical, myGrp2 );
  aSpinGrp->setFrameStyle( QFrame::NoFrame );
  aSpinGrp->setInsideMargin( 0 );

  new QLabel( tr( "GEOM_D1" ), aSpinGrp );
  mySpinBox[ SpinBox21 ] = new DlgRef_SpinBox( aSpinGrp );

  new QLabel( tr( "GEOM_D2" ), aSpinGrp );
  mySpinBox[ SpinBox22 ] = new DlgRef_SpinBox( aSpinGrp );

  // Create third group

  myGrp3 = new QGroupBox( 1, Qt::Horizontal, tr( "GEOM_CHAMFER_FACES" ), this );

  QGroupBox* aSelGrp3 = new QGroupBox( 3, Qt::Horizontal, myGrp3 );
  aSelGrp3->setFrameStyle( QFrame::NoFrame );
  aSelGrp3->setInsideMargin( 0 );

  createSelWg( tr( "GEOM_MAIN_OBJECT" ), iconSelect, aSelGrp3, MainObj3 );
  createSelWg( tr( "SELECTED_FACES" ), iconSelect, aSelGrp3, Faces );

  aSpinGrp = new QGroupBox( 1, Qt::Vertical, myGrp3 );
  aSpinGrp->setFrameStyle( QFrame::NoFrame );
  aSpinGrp->setInsideMargin( 0 );

  new QLabel( tr( "GEOM_D1" ), aSpinGrp );
  mySpinBox[ SpinBox31 ] = new DlgRef_SpinBox( aSpinGrp );

  new QLabel( tr( "GEOM_D2" ), aSpinGrp );
  mySpinBox[ SpinBox32 ] = new DlgRef_SpinBox( aSpinGrp );

  ( new QLabel( myGrp3 ) )->setSizePolicy(
    QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  // Add groups to layout

  Layout1->addWidget( myGrp1, 2, 0 );
  Layout1->addWidget( myGrp2, 2, 0 );
  Layout1->addWidget( myGrp3, 2, 0 );

  // Set range of spinboxes

  double SpecificStep = 10.0;
  QMap< int, DlgRef_SpinBox* >::iterator anIter;
  for ( anIter = mySpinBox.begin(); anIter != mySpinBox.end(); ++anIter )
    anIter.data()->RangeStepAndValidator( 0.001, 999.999, SpecificStep, 3 );

  /* Initialisations */
  Init( mySelection );
}


//=================================================================================
// function : ~OperationGUI_ChamferDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_ChamferDlg::~OperationGUI_ChamferDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::Init( SALOME_Selection* Sel )
{
  mySelection = Sel;
  myConstructorId = -1;
  reset();
  RadioButton1->setChecked( true );
  myEditCurrentArgument = mySelName[ MainObj1 ];

  /* signals and slots connections */

  // main buttons
  connect( buttonOk   , SIGNAL( clicked() ), this, SLOT( ClickOnOk()    ) );
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  // group box
  connect( GroupConstructors, SIGNAL( clicked( int ) ),
           this, SLOT( ConstructorsClicked( int ) ) );

  // push buttons
  QMap< int, QPushButton* >::iterator anIterBtn;
  for ( anIterBtn = mySelBtn.begin(); anIterBtn != mySelBtn.end(); ++anIterBtn )
    connect( anIterBtn.data(), SIGNAL( clicked() ),
             this, SLOT( SetEditCurrentArgument() ) );

  // line edits
  QMap< int, QLineEdit* >::iterator anIterLE2;
  for ( anIterLE2 = mySelName.begin(); anIterLE2 != mySelName.end(); ++anIterLE2 )
    connect( anIterLE2.data(), SIGNAL( returnPressed() ),
             this, SLOT( LineEditReturnPressed() ) );

  // spin boxes
  QMap< int, DlgRef_SpinBox* >::iterator anIterSpin;
  for ( anIterSpin = mySpinBox.begin(); anIterSpin != mySpinBox.end(); ++anIterSpin )
    connect( anIterSpin.data(), SIGNAL( valueChanged( double ) ),
             this, SLOT( ValueChangedInSpinBox( double ) ) );

  // selection
  connect( mySelection, SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_CHAMFER" ) );

  myGrp2->hide();
  myGrp3->hide();
  myGrp1->show();

  this->show();
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void OperationGUI_ChamferDlg::ConstructorsClicked( int constructorId )
{
   // Activate next widget
 if ( QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() != VIEW_OCC )
 {
    RadioButton1->setChecked( true );
    return;
 }
  
  if ( myConstructorId == constructorId )
    return;

  // Get values from previous widget
  double D1 = 5, D2 = 5;
  if ( myConstructorId == 0 )
    D1 = D2 = mySpinBox[ SpinBox1 ]->GetValue();
  else if ( myConstructorId == 1 )
  {
    D1 = mySpinBox[ SpinBox21 ]->GetValue();
    D2 = mySpinBox[ SpinBox22 ]->GetValue();
  }
  else if ( myConstructorId == 2 )
  {
    D1 = mySpinBox[ SpinBox31 ]->GetValue();
    D2 = mySpinBox[ SpinBox32 ]->GetValue();
  }

  myConstructorId = constructorId;

   switch ( constructorId )
  {
    case 0:
        myGrp1->show();
        myGrp2->hide();
        myGrp3->hide();
        mySpinBox[ SpinBox1 ]->SetValue( D1 );
    break;
    case 1:
        myGrp1->hide();
        myGrp2->show();
        myGrp3->hide();
        mySpinBox[ SpinBox21 ]->SetValue( D1 );
        mySpinBox[ SpinBox22 ]->SetValue( D2 );
    break;
    case 2:
        myGrp1->hide();
        myGrp2->hide();
        myGrp3->show();
        mySpinBox[ SpinBox31 ]->SetValue( D1 );
        mySpinBox[ SpinBox32 ]->SetValue( D2 );        
    break;
    default:
    break;
  }

  if      ( constructorId == 0 ) myEditCurrentArgument = mySelName[ MainObj1 ];
  else if ( constructorId == 1 ) myEditCurrentArgument = mySelName[ MainObj2 ];
  else                           myEditCurrentArgument = mySelName[ MainObj3 ];

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
void OperationGUI_ChamferDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_ChamferDlg::ClickOnApply()
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
void OperationGUI_ChamferDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText( "" );

  // Get index of current selection focus
  int aCurrFocus = -1;
  QMap< int, QLineEdit* >::iterator anIter;
  for ( anIter = mySelName.begin(); anIter != mySelName.end(); ++anIter )
    if ( myEditCurrentArgument == anIter.data() )
    {
      aCurrFocus = anIter.key();
      break;
    }

  // If selection of main object is activated
  if ( aCurrFocus == MainObj1 || aCurrFocus == MainObj2 || aCurrFocus == MainObj3 )
  {
    if ( mySelection->IObjectCount() == 1 )
    {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( mySelection->firstIObject(), aResult );

      if ( aResult && !anObj->_is_nil() )
      {
        myShape = anObj;
        mySelName[ aCurrFocus ]->setText( GEOMBase::GetName( anObj ) );
        displayPreview();
        enableWidgets();
        return;
      }
    }

    myShape = GEOM::GEOM_Object::_nil();
    enableWidgets();
  }
  // If face selection of second tab is activated
  else if ( aCurrFocus == Face1 || aCurrFocus == Face2 )
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

         if ( anIndexes.Extent() == 1 )
         {
            int anIndex = anIndexes( 1 );
            QString aFaceName = QString( GEOMBase::GetName( anObj ) ) + ":%1";
            myEditCurrentArgument->setText( aFaceName.arg( anIndex ) );
            myFace[ aCurrFocus ] = anIndex;
            displayPreview();
            return;
         }
      }
    }

    myFace[ aCurrFocus ] = -1;
  }
  // If face selection of third tab is activated
  else if ( aCurrFocus == Faces )
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
           QString aFaceName;
           if ( anIndexes.Extent() == 1 )
           {
             int anIndex = anIndexes( 1 );

             aFaceName = QString( GEOMBase::GetName( anObj ) ) + QString( ":%1" ).arg( anIndex );
           }
           else
             aFaceName = tr( "GEOM_MEN_POPUP_NAME" ).arg( anIndexes.Extent() );

           myEditCurrentArgument->setText( aFaceName );
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
void OperationGUI_ChamferDlg::LineEditReturnPressed()
{
  QLineEdit* aSender = ( QLineEdit* )sender();

  QMap< int, QLineEdit* >::iterator anIterLE;
  for ( anIterLE = mySelName.begin(); anIterLE != mySelName.end(); ++anIterLE )
    if ( anIterLE.data() == aSender )
      myEditCurrentArgument = anIterLE.data();

  GEOMBase_Skeleton::LineEditReturnPressed();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::SetEditCurrentArgument()
{
  QPushButton* aSender = ( QPushButton* )sender();

  QMap< int, QPushButton* >::iterator anIter;
  for ( anIter = mySelBtn.begin(); anIter != mySelBtn.end(); ++anIter )
    if ( anIter.data() == aSender )
    {
      mySelName[ anIter.key() ]->setFocus();
      myEditCurrentArgument = mySelName[ anIter.key() ];
    }

  activateSelection();
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::DeactivateActiveDialog()
{
  // disconnect selection
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ActivateThisDialog()
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
void OperationGUI_ChamferDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    this->ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ValueChangedInSpinBox( double newValue )
{
  displayPreview();
}


//=================================================================================
// function : createSelWg()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::createSelWg( const QString& theLbl,
                                           QPixmap&       thePix,
                                           QWidget*       theParent,
                                           const int      theId )
{
  new QLabel( theLbl, theParent );
  mySelBtn[ theId ] = new QPushButton( theParent );
  mySelBtn[ theId ]->setPixmap( thePix );
  mySelName[ theId ] = new QLineEdit( theParent );
  mySelName[ theId ]->setReadOnly( true );
}

//=================================================================================
// function : reset()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::reset()
{
  // Set Initial values of spinboxes
  QMap< int, DlgRef_SpinBox* >::iterator anIter;
  for ( anIter = mySpinBox.begin(); anIter != mySpinBox.end(); ++anIter )
    anIter.data()->SetValue( 5 );

  // clear line edits
  QMap< int, QLineEdit* >::iterator anIterLE;
  for ( anIterLE = mySelName.begin(); anIterLE != mySelName.end(); ++anIterLE )
    anIterLE.data()->setText( "" );

  // constructor id
  int aConstructorId = getConstructorId();

  if      ( aConstructorId == 0 ) myEditCurrentArgument = mySelName[ MainObj1 ];
  else if ( aConstructorId == 1 ) myEditCurrentArgument = mySelName[ MainObj2 ];
  else                            myEditCurrentArgument = mySelName[ MainObj3 ];

  myShape = GEOM::GEOM_Object::_nil();

  myFaces.Clear();
  myFace[ Face1 ] = -1;
  myFace[ Face2 ] = -1;

  erasePreview( true );

  activateSelection();

  enableWidgets();
}

//=================================================================================
// function : getConstructorId()
// purpose  :
//=================================================================================
int OperationGUI_ChamferDlg::getConstructorId() const
{
  return GroupConstructors->id( GroupConstructors->selected() );
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void OperationGUI_ChamferDlg::activateSelection()
{
  if (  !myShape->_is_nil() &&
       ( myEditCurrentArgument == mySelName[ Face1 ] ||
         myEditCurrentArgument == mySelName[ Face2 ] ||
         myEditCurrentArgument == mySelName[ Faces ] ) )
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
void OperationGUI_ChamferDlg::enableWidgets()
{
  int anId = getConstructorId();

  bool toEnable = !myShape->_is_nil();
  
  if ( anId == 1 )
  {
    mySelName[ Face1 ]->setEnabled( toEnable );
    mySelName[ Face2 ]->setEnabled( toEnable );
    mySelBtn[ Face1 ]->setEnabled( toEnable );
    mySelBtn[ Face2 ]->setEnabled( toEnable );

    if ( !toEnable )
    {
      mySelName[ Face1 ]->setText( "" );
      mySelName[ Face2 ]->setText( "" );
      myFace[ Face1 ] = -1;
      myFace[ Face2 ] = -1;
    }
  }
  else if ( anId == 2 )
  {
    mySelName[ Faces ]->setEnabled( toEnable );

    if ( !toEnable )
      myFaces.Clear();
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_ChamferDlg::createOperation()
{
  return getGeomEngine()->GetILocalOperations( getStudyId() );
}

//=================================================================================
// function : ClickOnApply()
// purpose  : Verify validity of input data
//=================================================================================
bool OperationGUI_ChamferDlg::isValid( QString& )
{
  switch ( getConstructorId() )
  {
    case 0: return !myShape->_is_nil();
    case 1: return !myShape->_is_nil() && myFace[ Face1 ] > 0 && myFace[ Face2 ] > 0;
    case 2: return !myShape->_is_nil() && myFaces.Extent() > 0;
    default: return false;
  }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_ChamferDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  int anId = getConstructorId();
  if ( anId == 0 )
    anObj = GEOM::GEOM_ILocalOperations::_narrow(
      getOperation() )->MakeChamferAll( myShape,
                                        mySpinBox[ SpinBox1 ]->GetValue() );
  else if ( anId == 1 )
    anObj = GEOM::GEOM_ILocalOperations::_narrow(
      getOperation() )->MakeChamferEdge( myShape,
                                         mySpinBox[ SpinBox21 ]->GetValue(),
                                         mySpinBox[ SpinBox22 ]->GetValue(),
                                         myFace[ Face1 ],
                                         myFace[ Face2 ] );
  else if ( anId == 2 )
  {

    int i = 0;
    GEOM::ListOfLong_var anArray = new GEOM::ListOfLong;
    anArray->length( myFaces.Extent() );


    for ( int i = 1, n = myFaces.Extent(); i <= n; i++ )
      anArray[ i - 1 ] = myFaces( i );             
    
    anObj = GEOM::GEOM_ILocalOperations::_narrow(
      getOperation() )->MakeChamferFaces( myShape,
                                          mySpinBox[ SpinBox31 ]->GetValue(),
                                          mySpinBox[ SpinBox32 ]->GetValue(),
                                          anArray );
  }


  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}









