// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
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

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//=================================================================================
// class    : TransformationGUI_OffsetDlg()
// purpose  : Constructs a TransformationGUI_OffsetDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_OffsetDlg::TransformationGUI_OffsetDlg
                             (GeometryGUI* theGeometryGUI, QWidget* parent,
                              bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_OFFSET")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_OFFSET_PARTIAL")));

  setWindowTitle(tr("GEOM_OFFSET_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_OFFSET"));
  mainFrame()->RadioButton1->setIcon(image1);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton1->setToolTip(tr("TOOLTIP_OFFSET"));
  mainFrame()->RadioButton2->setToolTip(tr("TOOLTIP_OFFSET_PARTIAL"));
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();
  mainFrame()->RadioButton1->setChecked(true);

  GroupPoints = new DlgRef_2Sel1Spin2Check( centralWidget() );

  GroupPoints->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );

  GroupPoints->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_FACES" ) );
  GroupPoints->TextLabel3->setText( tr( "GEOM_OFFSET" ) );

  GroupPoints->PushButton1->setIcon( image0 );
  GroupPoints->PushButton2->setIcon( image0 );

  GroupPoints->CheckButton1->setText( tr( "GEOM_JOIN_BY_PIPES" ) );
  GroupPoints->CheckButton1->setChecked( true );
  GroupPoints->CheckButton2->setAttribute( Qt::WA_DeleteOnClose );
  GroupPoints->CheckButton2->close();

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
  GroupPoints->LineEdit2->setReadOnly( true );

  myObjects.clear();
  myFaces.clear();

  /* min, max, step and decimals for spin boxes & initial values */
  double step = 1;
  initSpinBox( GroupPoints->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
  GroupPoints->SpinBox_DX->setValue( 1e-05 );

  mainFrame()->GroupBoxPublish->show();

  /* signals and slots connections */
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(this,          SIGNAL(constructorsClicked(int)),
          this,          SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()),
          this,                     SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()),
          this,                     SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->SpinBox_DX,   SIGNAL(valueChanged(double)),
          this,                      SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->CheckButton1, SIGNAL(toggled(bool)),
          this,                      SLOT(JoinModeChanged()));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this,                                SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_OFFSET"));

  ConstructorsClicked(0);
}

//==============================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//==============================================================================
void TransformationGUI_OffsetDlg::ConstructorsClicked (int constructorId)
{
  switch (constructorId) {
  case 0:
    // disable faces selection
    GroupPoints->TextLabel2->hide();
    GroupPoints->PushButton2->hide();
    GroupPoints->LineEdit2->hide();

    // enable joint type
    GroupPoints->CheckButton1->show();
    break;
  case 1:
    // enable faces selection
    GroupPoints->TextLabel2->show();
    GroupPoints->PushButton2->show();
    GroupPoints->LineEdit2->show();

    // disable joint type
    GroupPoints->CheckButton1->hide();
    break;
  default:
    break;
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  GroupPoints->PushButton1->click();
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
  if ( !onAccept( /*copy=*/true ))
    return false;

  initName();

  // activate selection and connect selection manager
  ConstructorsClicked(getConstructorId());

  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void TransformationGUI_OffsetDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    if (getConstructorId() == 0) {
      myObjects = getSelected( TopAbs_SHAPE, -1 );
      myEditCurrentArgument->setText(GEOMBase::GetName(myObjects));
    }
    else if (getConstructorId() == 1) {
      myFaces.clear();
      GroupPoints->LineEdit2->setText("");
      myObjects = getSelected(TopAbs_SHAPE, 1); // only one object allowed
      if (!myObjects.isEmpty()) {
        myEditCurrentArgument->setText(GEOMBase::GetName(myObjects[0].get()));
        GroupPoints->PushButton2->click();
      }
    }
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
    myFaces.clear();
    myFaces = getSelected(TopAbs_FACE, -1);
    myEditCurrentArgument->setText(GEOMBase::GetName(myFaces));
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
    GroupPoints->PushButton2->setDown(false);
    myEditCurrentArgument = GroupPoints->LineEdit1;
    globalSelection(GEOM_ALLSHAPES);
  }
  else if (send == GroupPoints->PushButton2) {
    GroupPoints->PushButton1->setDown(false);
    myEditCurrentArgument = GroupPoints->LineEdit2;
    globalSelection(); // close local contexts, if any
    if (myObjects.size() > 0)
      localSelection(myObjects[0].get(), TopAbs_FACE);
  }

  myEditCurrentArgument->setFocus();
  send->setDown(true);
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

  ConstructorsClicked(getConstructorId());
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
  return getGeomEngine()->GetITransformOperations();
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_OffsetDlg::isValid( QString& msg )
{
  bool isOk = GroupPoints->SpinBox_DX->isValid( msg, !IsPreview() ) && !myObjects.isEmpty();

  if (isOk) {
    switch (getConstructorId()) {
    case 0:
      for ( int i = 0; i < myObjects.count() && isOk; i++ ) {
        GEOM::shape_type aType = myObjects[i]->GetShapeType();
        isOk = aType == GEOM::FACE || aType == GEOM::SHELL || aType == GEOM::SOLID;
        if ( !isOk )
          msg = tr( "ERROR_SHAPE_TYPE" );
      }
      break;
    case 1:
      isOk = myObjects.size() == 1 && !myFaces.isEmpty();
      break;
    default:
      break;
    }
  }

  return isOk;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_OffsetDlg::execute( ObjectList& objects )
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ITransformOperations_var anOper =
    GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  if (getConstructorId() == 0) {
    for (int i = 0; i < myObjects.count(); i++) {
      anObj = anOper->OffsetShapeCopy(myObjects[i].get(), GetOffset(), GetIsJoinByPipes());
      if (!anObj->_is_nil()) {
        if (!IsPreview()) {
          anObj->SetParameters(GroupPoints->SpinBox_DX->text().toUtf8().constData());
        }
        objects.push_back(anObj._retn());
        res = true;
      }
    }
  }
  else if (getConstructorId() == 1) {
    if (myObjects.count() == 1) {
      TopoDS_Shape aShape;
      if (GEOMBase::GetShape(myObjects[0].get(), aShape)) {
        TopTools_IndexedMapOfShape aMainMap;
        TopExp::MapShapes(aShape, aMainMap);

        QList<int> aListIDs;
        for (int i = 0; i < myFaces.count(); i++) {
          TopoDS_Shape aFace;
          if (GEOMBase::GetShape(myFaces[i].get(), aFace)) {
            int anIndex = aMainMap.FindIndex(aFace);
            if (anIndex >= 0) {
              aListIDs << anIndex;
            }
          }
        }

        GEOM::ListOfLong_var aFacesIDsList = new GEOM::ListOfLong();
        if (!aListIDs.empty()) {
          aFacesIDsList->length(aListIDs.length());
          for (int i = 0; i < aListIDs.length(); i++) {
            aFacesIDsList[i] = aListIDs[i];
          }
        }

        anObj = anOper->OffsetShapePartialCopy
          (myObjects[0].get(), GetOffset(), aFacesIDsList);
        if (!anObj->_is_nil()) {
          if (!IsPreview()) {
            anObj->SetParameters(GroupPoints->SpinBox_DX->text().toUtf8().constData());
          }
          objects.push_back(anObj._retn());
          res = true;
        }
      }
    }
  }

  return res;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::restoreSubShapes( SALOMEDS::SObject_ptr theSObject )
{
  if ( mainFrame()->CheckBoxRestoreSS->isChecked() ) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO( theSObject, GEOM::ListOfGO(),
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

//=======================================================================
//function : GetIsJoinByPipes
//purpose  :
//=======================================================================

bool TransformationGUI_OffsetDlg::GetIsJoinByPipes() const
{
  return GroupPoints->CheckButton1->isChecked();
}

//=================================================================================
// function :  JoinModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::JoinModeChanged()
{
  processPreview();
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
