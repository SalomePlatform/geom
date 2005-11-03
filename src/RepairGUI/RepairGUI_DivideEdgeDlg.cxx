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
//  File   : RepairGUI_DivideEdgeDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "RepairGUI_DivideEdgeDlg.h"

#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"
#include "SUIT_Session.h"
#include "SALOME_ListIteratorOfListIO.hxx"

#include "GEOMImpl_Types.hxx"

#include <TopAbs.hxx>

#include <qlabel.h>

using namespace std;

//=================================================================================
// class    : RepairGUI_DivideEdgeDlg()
// purpose  : Constructs a RepairGUI_DivideEdgeDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_DivideEdgeDlg::RepairGUI_DivideEdgeDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                  const char* name, bool modal, WFlags fl)
  : GEOMBase_Skeleton(parent, name, modal, WStyle_Customize |
                      WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  myGeomGUI = theGeometryGUI;

  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_DIVIDE_EDGE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_DIVIDE_EDGE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_DIVIDE_EDGE_TITLE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel_Ext(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ADD_POINT"));
  GroupPoints->TextLabel1->setText(tr("GEOM_EDGE"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );

  myIsParameterGr = new QButtonGroup( 2, Qt::Vertical, GroupPoints );
  myIsParameterGr->setFrameStyle( QFrame::NoFrame );
  myIsParameterGr->setRadioButtonExclusive( true );
  myIsParameterGr->insert( new QRadioButton( tr( "GEOM_BY_PARAMETER" ), myIsParameterGr ), 0 );
  myIsParameterGr->insert( new QRadioButton( tr( "GEOM_BY_LENGTH" ), myIsParameterGr ), 1 );
  myIsParameterGr->find( 0 )->toggle();

  QGridLayout* aLay = new QGridLayout( 0, 1, 2, 0, 6, "aLay" );
  myValEdt = new QtxDblSpinBox(0., 1., 0.1, GroupPoints->GroupBox1);
  myValEdt->setPrecision( 3 );
  myValEdt->setValue( 0.5 );
  QLabel* aLbl1 = new QLabel( tr( "GEOM_VALUE" ), GroupPoints->GroupBox1 );
  aLay->addWidget( aLbl1, 0, 0 );
  aLay->addWidget( myValEdt, 0, 1 );

  Layout1->addWidget(GroupPoints, 2, 0);
  GroupPoints->getGroupBoxLayout()->addMultiCellWidget(myIsParameterGr, 1, 1, 0, 2);
  GroupPoints->getGroupBoxLayout()->addLayout( aLay, 2, 0 );
  /***************************************************************/

  Init();
}


//=================================================================================
// function : ~RepairGUI_DivideEdgeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_DivideEdgeDlg::~RepairGUI_DivideEdgeDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myObject = GEOM::GEOM_Object::_nil();
  myIndex = -1;

  //myGeomGUI->SetState( 0 );
  initSelection();

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( tr( "DEVIDE_EDGE_NEW_OBJECT_NAME" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_DivideEdgeDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_DivideEdgeDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  myEditCurrentArgument->setText("");
  myObject = GEOM::GEOM_Object::_nil();
  myIndex = -1;

  initSelection();

  return true;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//          : used only by SelectButtonC1A1 (LineEditC1A1)
//=================================================================================
void RepairGUI_DivideEdgeDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  myObject = GEOM::GEOM_Object::_nil();
  myIndex = -1;

  if ( IObjectCount() == 1 )
  {
    Handle(SALOME_InteractiveObject) anIO = firstIObject();
    Standard_Boolean aRes;
    GEOM::GEOM_Object_var aSelectedObj = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
    if ( !CORBA::is_nil( aSelectedObj ) && aRes )
    {
      TopoDS_Shape aShape;
      if ( myGeomBase->GetShape( aSelectedObj, aShape, TopAbs_SHAPE ) )
      {
        const int aType = aShape.ShapeType();
        if ( aType <= TopAbs_EDGE ) // edge, wire, face, shell, solid, compound
        {
          GEOM::short_array anIndexes;

          TColStd_IndexedMapOfInteger aMap;
          SalomeApp_Application* anApp =
            (SalomeApp_Application*)(SUIT_Session::session()->activeApplication());
          anApp->selectionMgr()->GetIndexes( anIO, aMap );

          if ( !aMap.IsEmpty() ) // subshape selection
          {
            myIndex = aMap( 1 );
            myObject = aSelectedObj;
            myEditCurrentArgument->setText( tr( "GEOM_EDGE" ) + "_1" );
          }
          else if ( aType == TopAbs_EDGE ) // single shape selection
          {
            myIndex = -1;
            myObject = aSelectedObj;
            myEditCurrentArgument->setText( GEOMBase::GetName( myObject ) );
          }
          else // face, shell, solid or compound was selected, and NOT its subshape.
          {
            myIndex = -1;
            myObject = GEOM::GEOM_Object::_nil();
          }
        }
      }
    }
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::SetEditCurrentArgument()
{
  if( sender() == GroupPoints->PushButton1 )
  {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::LineEditReturnPressed()
{
  if( sender() == GroupPoints->LineEdit1 )
  {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::DeactivateActiveDialog()
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  myObject = GEOM::GEOM_Object::_nil();
  myIndex = -1;

  //myGeomGUI->SetState( 0 );
  initSelection();
}

//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_DivideEdgeDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::closeEvent(QCloseEvent* e)
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_DivideEdgeDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_DivideEdgeDlg::isValid( QString& msg )
{
  return !myObject->_is_nil();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_DivideEdgeDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj = GEOM::GEOM_IHealingOperations::_narrow( getOperation() )->DivideEdge
    ( myObject, myIndex, myValEdt->value(), getIsByParameter() );
  bool aResult = !anObj->_is_nil();
  if ( aResult )
    objects.push_back( anObj._retn() );

  return aResult;
}

//=================================================================================
// function : getIsByParameter
// purpose  :
//=================================================================================
bool RepairGUI_DivideEdgeDlg::getIsByParameter() const
{
  return myIsParameterGr->find( 0 )->isOn();
}

//=================================================================================
// function : initSelection
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::initSelection()
{
  GEOM::GEOM_Object_var aNullGeomObject;
  localSelection( aNullGeomObject, TopAbs_EDGE ); // load local selection on ALL objects
}
