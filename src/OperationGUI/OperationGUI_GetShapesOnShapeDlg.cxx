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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : OperationGUI_GetShapesOnShapeDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#include "OperationGUI_GetShapesOnShapeDlg.h"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qcombobox.h>
#include <qlabel.h>

//using namespace std;

//==============================================================================
// class    : OperationGUI_GetShapesOnShapeDlg()
// purpose  : Constructs a OperationGUI_GetShapesOnShapeDlg which is a child of
//            'parent', with the name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_GetShapesOnShapeDlg::OperationGUI_GetShapesOnShapeDlg(GeometryGUI* theGeometryGUI,
                                                                   QWidget* parent)
  :GEOMBase_Skeleton(theGeometryGUI, parent, "OperationGUI_GetShapesOnShapeDlg", false,
                     WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{

  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_SHAPES_ON_SHAPE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setCaption(tr("GEOM_SHAPES_ON_SHAPE_TITLE"));

  GroupConstructors->setTitle(tr("GEOM_SHAPES_ON_SHAPE"));

  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel2List_QTD(this, "GroupPoints");
  //GroupPoints->GroupBox1->setTitle(tr("GEOM_PARTITION"));
  GroupPoints->GroupBox1->setTitle("Input data");
  GroupPoints->TextLabel1->setText(tr("GEOM_SHAPES_ON_SHAPE_ESHAPE"));
  GroupPoints->TextLabel2->setText(tr("GEOM_SHAPES_ON_SHAPE_CSHAPE"));
  GroupPoints->TextLabel3->setText(tr("GEOM_RECONSTRUCTION_LIMIT"));
  GroupPoints->TextLabel4->setText(tr("GEOM_SHAPES_ON_SHAPE_STATE"));
  GroupPoints->PushButton1->setPixmap(image2);
  GroupPoints->PushButton2->setPixmap(image2);
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);

  setHelpFileName("shapesonshape_page.html");
  
  Init();
}


//=================================================================================
// function : ~OperationGUI_GetShapesOnShapeDlg
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_GetShapesOnShapeDlg::~OperationGUI_GetShapesOnShapeDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::Init()
{
  /* type for sub shape selection */
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_SOLID"));
  // commented by skl for IPAL19949 - finder isn't allowed such type
  //GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_SHELL"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_FACE"));
  // commented by skl for IPAL19949 - finder isn't allowed such type
  //GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_WIRE"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_EDGE"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_VERTEX"));

  /* type for state selection */
  GroupPoints->ComboBox2->insertItem(tr("GEOM_STATE_IN"));
  GroupPoints->ComboBox2->insertItem(tr("GEOM_STATE_OUT"));
  GroupPoints->ComboBox2->insertItem(tr("GEOM_STATE_ON"));
  GroupPoints->ComboBox2->insertItem(tr("GEOM_STATE_ONIN"));
  GroupPoints->ComboBox2->insertItem(tr("GEOM_STATE_ONOUT"));

  GroupPoints->LineEdit1->clear();
  GroupPoints->LineEdit2->clear();

   /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  
  connect(GroupPoints->ComboBox1, SIGNAL(activated(int)), this, SLOT(ComboTextChanged()));
  connect(GroupPoints->ComboBox2, SIGNAL(activated(int)), this, SLOT(ComboTextChanged()));

  connect(myGeomGUI->getApp()->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  globalSelection( GEOM_ALLSHAPES );

  initName( GroupConstructors->title() );

  resize(0, 0);
  GroupPoints->TextLabel3->show();
  GroupPoints->ComboBox1->show();
  GroupPoints->ComboBox1->setCurrentItem(0);
  GroupPoints->ComboBox2->show();
  GroupPoints->ComboBox2->setCurrentItem(0);

  myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setFocus();
  connect(myGeomGUI->getApp()->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_GetShapesOnShapeDlg::ClickOnApply()
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
void OperationGUI_GetShapesOnShapeDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  QString aString = "";

  int nbSel = GEOMBase::GetNameOfSelectedIObjects( selectedIO(), aString, true );

  if(nbSel>0) {
    Standard_Boolean aRes = Standard_False;
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), aRes );
    if ( !CORBA::is_nil( aSelectedObject ) && aRes && GEOMBase::IsShape( aSelectedObject ) ) {
      {
        myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
        if      ( myEditCurrentArgument == GroupPoints->LineEdit1 )   myObject1 = aSelectedObject;
        else if ( myEditCurrentArgument == GroupPoints->LineEdit2 )   myObject2 = aSelectedObject;
      }
    }
  }

  myEditCurrentArgument->setText( aString );
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if      ( send == GroupPoints->PushButton1 ) myEditCurrentArgument = GroupPoints->LineEdit1;
  else if ( send == GroupPoints->PushButton2 ) myEditCurrentArgument = GroupPoints->LineEdit2;

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPoints->LineEdit1 || send == GroupPoints->LineEdit2 )
  {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection( GEOM_ALLSHAPES );
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::enterEvent(QEvent * e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_GetShapesOnShapeDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool OperationGUI_GetShapesOnShapeDlg::isValid( QString& msg )
{
  Handle(SALOME_InteractiveObject) IO = firstIObject();
  Standard_Boolean testResult;
  GEOM::GEOM_Object_var anObject = GEOMBase::ConvertIOinGEOMObject( IO, testResult );
  if ( !testResult || anObject->_is_nil() )
    return false;

  return !CORBA::is_nil( myObject1 ) && !CORBA::is_nil( myObject2 );
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_GetShapesOnShapeDlg::execute( ObjectList& objects )
{
  int aLimit = GroupPoints->ComboBox1->currentItem();
  switch (aLimit)
  {
  case 0:  aLimit = GEOM::SOLID ; break;
  case 1:  aLimit = GEOM::FACE  ; break;
  case 2:  aLimit = GEOM::EDGE  ; break;
  case 3:  aLimit = GEOM::VERTEX; break;
  default: aLimit = GEOM::SHAPE ;
  }

  GEOM::shape_state aState;
  switch (GroupPoints->ComboBox2->currentItem())
  {
    case 0:  aState = GEOM::ST_IN; break;
    case 1:  aState = GEOM::ST_OUT; break;
    case 2:  aState = GEOM::ST_ON; break;
    case 3:  aState = GEOM::ST_ONIN; break;
    case 4:  aState = GEOM::ST_ONOUT; break;
    default: break;
  }

  GEOM::GEOM_Object_var anObj = GEOM::GEOM_IShapesOperations::
    _narrow( getOperation() )->GetShapesOnShapeAsCompound(myObject2, myObject1,
                                                          (CORBA::Short) aLimit,
                                                          aState);

  if (!anObj->_is_nil())
    objects.push_back( anObj._retn() );

  return objects.size();
}


//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}


//=======================================================================
//function : ComboTextChanged
//purpose  : 
//=======================================================================
void OperationGUI_GetShapesOnShapeDlg::ComboTextChanged()
{
  bool IsEnabled = GroupPoints->ComboBox1->currentItem() < 3;
}


