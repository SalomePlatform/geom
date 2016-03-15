// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : EntityGUI_IsolineDlg.cxx

#include "EntityGUI_IsolineDlg.h"
#include <GeometryGUI.h>
#include <DlgRef.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>


//=================================================================================
// class    : EntityGUI_IsolineDlg
// purpose  : 
//=================================================================================
EntityGUI_IsolineDlg::EntityGUI_IsolineDlg (GeometryGUI     *theGeometryGUI,
                                            QWidget         *parent,
                                            bool             modal,
                                            Qt::WindowFlags  fl)
  : GEOMBase_Skeleton (theGeometryGUI, parent, modal, fl),
    myRBGroup         (0)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICO_ISOLINE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image2(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICO_ISOLINE_V")));

  setWindowTitle(tr("GEOM_ISOLINE_TITLE"));

  /***************************************************************/

  mainFrame()->GroupConstructors->setTitle(tr("GEOM_ISOLINE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->close();

  // Construct a group.
  myGroup = new DlgRef_3Radio1Sel1Spin(centralWidget());
  myGroup->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  myGroup->TextLabel1->setText(tr("GEOM_FACE"));
  myGroup->PushButton1->setIcon(image1);
  myGroup->LineEdit1->setReadOnly(true);
  myGroup->RadioButton1->setIcon(image0);
  myGroup->RadioButton2->setIcon(image2);
  myGroup->RadioButton1->setText(tr("GEOM_ISOLINE_U"));
  myGroup->RadioButton2->setText(tr("GEOM_ISOLINE_V"));
  myGroup->RadioButton3->hide();
  myGroup->TextLabel2->setText(tr("GEOM_PARAMETER"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGroup);

  myRBGroup = new QButtonGroup( this );
  myRBGroup->addButton( myGroup->RadioButton1, 0 );
  myRBGroup->addButton( myGroup->RadioButton2, 1 );

  setHelpFileName("create_isoline_page.html");

  Init();
}

//=================================================================================
// function : ~EntityGUI_IsolineDlg()
// purpose  : 
//=================================================================================
EntityGUI_IsolineDlg::~EntityGUI_IsolineDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void EntityGUI_IsolineDlg::Init()
{
  /* min, max, step and decimals for spin box */
  initSpinBox(myGroup->SpinBox_DX, 0., 1., 0.1, "parametric_precision");
  myGroup->SpinBox_DX->setValue(0.5);
  myGroup->RadioButton1->setChecked(true);

  initName(mainFrame()->GroupConstructors->title());

  connect(myGroup->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditFace()));
  connect(myGroup->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this,  SLOT(SetDoubleSpinBoxStep(double)));
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  /* signals and slots connections */
  connect(myGeomGUI,     SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI,     SIGNAL(SignalCloseAllDialogs()),        this, SLOT(ClickOnCancel()));
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(myRBGroup,     SIGNAL(buttonClicked(int)), this, SLOT(TypeChanged(int)));

  myGroup->PushButton1->click();
  SelectionIntoArgument();
  resize(100,100);
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  : Called when selection is changed
//=================================================================================
void EntityGUI_IsolineDlg::SelectionIntoArgument()
{
  myGroup->LineEdit1->setText("");
  myFace.nullify();

  GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_SHAPE );
  TopoDS_Shape aShape;

  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    if (aShape.ShapeType() == TopAbs_FACE) {
      QString aName = GEOMBase::GetName( aSelectedObject.get() );
      myGroup->LineEdit1->setText( aName );

      // clear selection
      disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
      myGeomGUI->getApp()->selectionMgr()->clearSelected();
      connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
              this, SLOT(SelectionIntoArgument()));

      myFace = aSelectedObject;
    }
  }

  displayPreview(true);
}

//=================================================================================
// function : ActivateThisDialog
// purpose  :
//=================================================================================
void EntityGUI_IsolineDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );
  SetEditFace();
  SelectionIntoArgument();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void EntityGUI_IsolineDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : TypeChanged
// purpose  :
//=================================================================================
void EntityGUI_IsolineDlg::TypeChanged(int)
{
  displayPreview(true);
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr EntityGUI_IsolineDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool EntityGUI_IsolineDlg::isValid (QString& msg)
{
  return myFace;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool EntityGUI_IsolineDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_ICurvesOperations_var anOper = GEOM::GEOM_ICurvesOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->MakeIsoline
    (myFace.get(), myGroup->RadioButton1->isChecked(), myGroup->SpinBox_DX->value());

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : addSubshapesToStudy()
// purpose  : Double spin box management
//=================================================================================
void EntityGUI_IsolineDlg::addSubshapesToStudy()
{
  GEOMBase::PublishSubObject(myFace.get());
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void EntityGUI_IsolineDlg::SetDoubleSpinBoxStep (double step)
{
  myGroup->SpinBox_DX->setSingleStep(step);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void EntityGUI_IsolineDlg::ClickOnOk()
{
  setIsApplyAndClose( true );

  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool EntityGUI_IsolineDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate selection and connect selection manager
  myGroup->PushButton1->click();
  return true;
}

//=================================================================================
// function : SetEditFace
// purpose  :
//=================================================================================
void EntityGUI_IsolineDlg::SetEditFace()
{
  myGroup->LineEdit1->setFocus();
  myGroup->PushButton1->setDown(true);
}

//=================================================================================
// function : ValueChangedInSpinBox
// purpose  :
//=================================================================================
void EntityGUI_IsolineDlg::ValueChangedInSpinBox(double newValue)
{
  displayPreview(true);
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> EntityGUI_IsolineDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myFace;
  return res;
}
