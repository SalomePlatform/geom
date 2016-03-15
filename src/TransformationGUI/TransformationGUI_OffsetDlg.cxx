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
// File   : TransformationGUI_OffsetDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "TransformationGUI_OffsetDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

//=================================================================================
// class    : TransformationGUI_OffsetDlg()
// purpose  : Constructs a TransformationGUI_OffsetDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_OffsetDlg::TransformationGUI_OffsetDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                                          bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_OFFSET" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_OFFSET_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_OFFSET" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();
 
  GroupPoints = new DlgRef_1Sel1Spin1Check( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_OFFSET" ) );
  GroupPoints->CheckButton1->setText( tr( "GEOM_CREATE_COPY" ) );

  // san -- modification of an exisitng object by offset is not allowed
  GroupPoints->CheckButton1->hide();

  GroupPoints->PushButton1->setIcon( image1 );
  
  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  
  /***************************************************************/

  setHelpFileName( "offset_operation_page.html" );
  
  Init();
}


//=================================================================================
// function : ~TransformationGUI_OffsetDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_OffsetDlg::~TransformationGUI_OffsetDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::Init()
{  
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );

  myObjects.clear();
  
  /* Get setting of step value from file configuration */
  double step = 1;
   
  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( GroupPoints->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
  GroupPoints->SpinBox_DX->setValue( 1e-05 );
  
  // Activate Create a Copy mode
  GroupPoints->CheckButton1->setChecked( true );
  CreateCopyModeChanged();

  mainFrame()->GroupBoxPublish->show();

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  
  connect( GroupPoints->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( myGeomGUI->getApp()->selectionMgr(), 
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  connect( GroupPoints->SpinBox_DX,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox() ) );
  connect( GroupPoints->CheckButton1, SIGNAL( toggled( bool ) ),        this, SLOT( CreateCopyModeChanged() ) );
  
  initName( tr( "GEOM_OFFSET" ) );

  globalSelection( GEOM_ALLSHAPES );
  resize(100,100);
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_OffsetDlg::ClickOnApply()
{
  if ( !onAccept( GroupPoints->CheckButton1->isChecked() ) )
    return false;
  
  initName();
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void TransformationGUI_OffsetDlg::SelectionIntoArgument()
{
  myObjects = getSelected( TopAbs_SHAPE, -1 );
  if ( !myObjects.isEmpty() ) {
    QString aName = myObjects.count() > 1 ? QString( "%1_objects").arg( myObjects.count() ) : GEOMBase::GetName( myObjects[0].get() );
    myEditCurrentArgument->setText( aName );
  }
  else {
    myEditCurrentArgument->setText("");
  }

  processPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if ( send == GroupPoints->PushButton1 ) {
    GroupPoints->PushButton1->setDown(true);
    myEditCurrentArgument = GroupPoints->LineEdit1;
    myEditCurrentArgument->setFocus();
    SelectionIntoArgument();
  }
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void TransformationGUI_OffsetDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), 
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  globalSelection( GEOM_ALLSHAPES );
  myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setFocus();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::ValueChangedInSpinBox()
{
  processPreview();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_OffsetDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_OffsetDlg::isValid( QString& msg )
{
  bool ok = GroupPoints->SpinBox_DX->isValid( msg, !IsPreview() ) && !myObjects.isEmpty();
  for ( int i = 0; i < myObjects.count() && ok; i++ ) {
    GEOM::shape_type aType = myObjects[i]->GetShapeType();
    ok = aType == GEOM::FACE || aType == GEOM::SHELL || aType == GEOM::SOLID;
    if ( !ok )
      msg = tr( "ERROR_SHAPE_TYPE" );
  }
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_OffsetDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;
  
  GEOM::GEOM_ITransformOperations_var anOper = GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  if ( GroupPoints->CheckButton1->isChecked() || IsPreview() ) {
    for ( int i = 0; i < myObjects.count(); i++ ) {
      
      anObj = anOper->OffsetShapeCopy( myObjects[i].get(), GetOffset() );
      if ( !anObj->_is_nil() ) {
        if(!IsPreview()) {
          anObj->SetParameters(GroupPoints->SpinBox_DX->text().toLatin1().constData());
        }
        objects.push_back( anObj._retn() );
      }
    }
  }
  else {
    for ( int i = 0; i < myObjects.count(); i++ ) {
      anObj = anOper->OffsetShape( myObjects[i].get(), GetOffset() );
      if ( !anObj->_is_nil() )
        objects.push_back( anObj._retn() );
    }
  }
  res = true;
    
  return res;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::restoreSubShapes( SALOMEDS::Study_ptr   theStudy,
                                                    SALOMEDS::SObject_ptr theSObject )
{
  if ( mainFrame()->CheckBoxRestoreSS->isChecked() ) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO( theStudy, theSObject, GEOM::ListOfGO(),
                                         /*theFindMethod=*/GEOM::FSM_Transformed,
                                         /*theInheritFirstArg=*/true,
                                         mainFrame()->CheckBoxAddPrefix->isChecked() );
  }
}

//=================================================================================
// function : GetOffset()
// purpose  :
//=================================================================================
double TransformationGUI_OffsetDlg::GetOffset() const
{
  return GroupPoints->SpinBox_DX->value();
}

//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::CreateCopyModeChanged()
{
  mainFrame()->GroupBoxName->setEnabled( GroupPoints->CheckButton1->isChecked() );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> TransformationGUI_OffsetDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  if ( GroupPoints->CheckButton1->isChecked() )
    res.append( myObjects );
  return res;
}
