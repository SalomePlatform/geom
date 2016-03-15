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
// File   : PrimitiveGUI_SphereDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "PrimitiveGUI_SphereDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : PrimitiveGUI_SphereDlg()
// purpose  : Constructs a PrimitiveGUI_SphereDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
PrimitiveGUI_SphereDlg::PrimitiveGUI_SphereDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                                bool modal, Qt::WindowFlags fl )
  :GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_SPHERE_P" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_SPHERE_DXYZ" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_SPHERE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_SPHERE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image1 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1Sel1Spin( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_SPHERE_CR" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_CENTER" ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_RADIUS" ) );
  GroupPoints->PushButton1->setIcon( image2 );

  GroupDimensions = new DlgRef_1Spin( centralWidget() );
  GroupDimensions->GroupBox1->setTitle( tr( "GEOM_SPHERE_RO" ) );
  GroupDimensions->TextLabel1->setText( tr( "GEOM_RADIUS" ) );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  layout->addWidget( GroupDimensions );
  /***************************************************************/

  setHelpFileName( "create_sphere_page.html" );

  Init();
}


//=================================================================================
// function : ~PrimitiveGUI_SphereDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
PrimitiveGUI_SphereDlg::~PrimitiveGUI_SphereDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void PrimitiveGUI_SphereDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );
  
  myPoint.nullify();
  
  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );

  /* min, max, step and decimals for spin boxes */
  initSpinBox( GroupPoints->SpinBox_DX, 0.000001, COORD_MAX, step, "length_precision" );
  initSpinBox( GroupDimensions->SpinBox_DX, 0.000001, COORD_MAX, step, "length_precision" );
  GroupPoints->SpinBox_DX->setValue( 100.0 );
  GroupDimensions->SpinBox_DX->setValue( 100.0 );
  
  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this,          SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupDimensions->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox() ) );
  connect( GroupPoints->SpinBox_DX,     SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox() ) );

  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_SPHERE" ) );

  setConstructorId( 1 ); // simplest constructor
  ConstructorsClicked( 1 );
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void PrimitiveGUI_SphereDlg::SetDoubleSpinBoxStep( double step )
{
  GroupPoints->SpinBox_DX->setSingleStep(step);
  GroupDimensions->SpinBox_DX->setSingleStep(step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void PrimitiveGUI_SphereDlg::ConstructorsClicked( int constructorId )
{
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  
  switch ( constructorId ) {
  case 0:
    {
      globalSelection(); // close local contexts, if any
      localSelection( TopAbs_VERTEX );

      GroupDimensions->hide();
      GroupPoints->show();
      
      myEditCurrentArgument = GroupPoints->LineEdit1;
      GroupPoints->LineEdit1->setText( "" );
      myPoint.nullify();
      
      connect( myGeomGUI->getApp()->selectionMgr(), 
               SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
      break;
    }
  case 1:
    {
      globalSelection(); // close local contexts, if any
      
      GroupPoints->hide();
      GroupDimensions->show();
      
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );
  SelectionIntoArgument();

  displayPreview(true);
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void PrimitiveGUI_SphereDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool PrimitiveGUI_SphereDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;
  
  initName();
  ConstructorsClicked( getConstructorId() );
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void PrimitiveGUI_SphereDlg::SelectionIntoArgument()
{
  if ( getConstructorId() != 0 )
    return;

  myEditCurrentArgument->setText( "" );

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    myPoint.nullify();
    return;
  }

  GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_VERTEX );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );
    myPoint = aSelectedObject;
  }

  displayPreview(true);
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void PrimitiveGUI_SphereDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if ( send == GroupPoints->PushButton1 ) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
    globalSelection(); // close local contexts, if any
    localSelection( TopAbs_VERTEX );
    SelectionIntoArgument();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void PrimitiveGUI_SphereDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );
  
  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void PrimitiveGUI_SphereDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void PrimitiveGUI_SphereDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void PrimitiveGUI_SphereDlg::ValueChangedInSpinBox()
{
  displayPreview(true);
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr PrimitiveGUI_SphereDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool PrimitiveGUI_SphereDlg::isValid( QString& msg  )
{
  bool ok = false;
  if( getConstructorId() == 0 )
    ok = GroupPoints->SpinBox_DX->isValid( msg, !IsPreview() ) && myPoint;
  else if( getConstructorId() == 1 )
    ok = GroupDimensions->SpinBox_DX->isValid( msg, !IsPreview() );
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool PrimitiveGUI_SphereDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_I3DPrimOperations_var anOper = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());

  switch ( getConstructorId() ) {
  case 0 :
    {
      if ( myPoint ) {
        anObj = anOper->MakeSpherePntR( myPoint.get(), getRadius() );
        if (!anObj->_is_nil() && !IsPreview())
        {
          QStringList aParameters;
          aParameters << GroupPoints->SpinBox_DX->text();
          anObj->SetParameters(aParameters.join(":").toLatin1().constData());
        }
        res = true;
      }
      break;
    }
  case 1 :
    {
      anObj = anOper->MakeSphereR( getRadius() );
      if (!anObj->_is_nil() && !IsPreview())
      {
        QStringList aParameters;
        aParameters << GroupDimensions->SpinBox_DX->text();
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());
      }
      res = true;
      break;
    }
  }
  
  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );
  
  return res;
}


//=================================================================================
// function : getRadius()
// purpose  :
//=================================================================================
double PrimitiveGUI_SphereDlg::getRadius() const
{
  int aConstructorId = getConstructorId();
  if ( aConstructorId == 0 )
    return GroupPoints->SpinBox_DX->value();
  else if ( aConstructorId == 1 )
    return GroupDimensions->SpinBox_DX->value();
  return 0;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void PrimitiveGUI_SphereDlg::addSubshapesToStudy()
{
  if ( getConstructorId() == 0 ) {
    GEOMBase::PublishSubObject( myPoint.get() );
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> PrimitiveGUI_SphereDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myPoint;
  return res;
}
