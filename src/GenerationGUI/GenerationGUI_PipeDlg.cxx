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
// File   : GenerationGUI_PipeDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "GenerationGUI_PipeDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <GEOMUtils.hxx>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Shape.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : GenerationGUI_PipeDlg()
// purpose  : Constructs a GenerationGUI_PipeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_PipeDlg::GenerationGUI_PipeDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                              bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton    (theGeometryGUI, parent, modal, fl),
    myGenGroupCheckGP    (0),
    myPrefixLblGP        (0),
    myPrefixEditGP       (0),
    myStepByStepCheckGMP (0),
    myGenGroupCheckGMP   (0),
    myPrefixLblGMP       (0),
    myPrefixEditGMP      (0)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PIPE")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image2 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PIPE_BINORMAL")));
  QPixmap image3 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PIPE_SECTION")));

  setWindowTitle(tr("GEOM_PIPE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_PIPE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setIcon(image3);
  // mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  // mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_3Sel1Check(centralWidget());

  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_BASE_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_PATH_OBJECT"));
  GroupPoints->TextLabel3->setText(tr("GEOM_BINORMAL"));
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->PushButton2->setIcon(image1);
  GroupPoints->PushButton3->setIcon(image1);
  GroupPoints->CheckButton1->setText(tr("GEOM_SELECT_UNPUBLISHED_EDGES"));

  // Add widgets for group generation
  QGridLayout *aLayoutGP = (QGridLayout *)GroupPoints->GroupBox1->layout();

  myGenGroupCheckGP =
    new QCheckBox(tr("GEOM_GENERATE_GROUPS"), GroupPoints->GroupBox1);
  myPrefixLblGP     =
    new QLabel (tr("GEOM_GROUP_NAME_PREFIX"), GroupPoints->GroupBox1);
  myPrefixEditGP    = new QLineEdit(GroupPoints->GroupBox1);

  aLayoutGP->addWidget(myGenGroupCheckGP, 4, 0, 1, 3);
  aLayoutGP->addWidget(myPrefixLblGP,     5, 0, 1, 2);
  aLayoutGP->addWidget(myPrefixEditGP,    5, 2);

  GroupMakePoints = new DlgRef_3Sel2Check3Spin(centralWidget());

  GroupMakePoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupMakePoints->TextLabel1->setText(tr("GEOM_BASE_OBJECT"));
  GroupMakePoints->TextLabel2->setText(tr("GEOM_LOCATIONS"));
  GroupMakePoints->TextLabel3->setText(tr("GEOM_PATH_OBJECT"));
  GroupMakePoints->PushButton1->setIcon(image1);
  GroupMakePoints->PushButton2->setIcon(image1);
  GroupMakePoints->PushButton3->setIcon(image1);
  GroupMakePoints->CheckBox1->setText(tr("GEOM_WITH_CONTACT"));
  GroupMakePoints->CheckBox2->setText(tr("GEOM_WITH_CORRECTION"));
  GroupMakePoints->SpinBox1->setAttribute(Qt::WA_DeleteOnClose);
  GroupMakePoints->SpinBox2->setAttribute(Qt::WA_DeleteOnClose);
  GroupMakePoints->SpinBox3->setAttribute(Qt::WA_DeleteOnClose);
  GroupMakePoints->TextLabel4->setAttribute(Qt::WA_DeleteOnClose);
  GroupMakePoints->TextLabel5->setAttribute(Qt::WA_DeleteOnClose);
  GroupMakePoints->TextLabel6->setAttribute(Qt::WA_DeleteOnClose);
  GroupMakePoints->SpinBox1->close();
  GroupMakePoints->SpinBox2->close();
  GroupMakePoints->SpinBox3->close();
  GroupMakePoints->TextLabel4->close();
  GroupMakePoints->TextLabel5->close();
  GroupMakePoints->TextLabel6->close();

  // Add widgets for group generation
  QGridLayout *aLayoutGMP = (QGridLayout *)GroupMakePoints->GroupBox1->layout();

  myStepByStepCheckGMP =
    new QCheckBox(tr("GEOM_STEP_BY_STEP"), GroupMakePoints->GroupBox1);
  myGenGroupCheckGMP   =
    new QCheckBox(tr("GEOM_GENERATE_GROUPS"), GroupMakePoints->GroupBox1);
  myPrefixLblGMP       =
    new QLabel (tr("GEOM_GROUP_NAME_PREFIX"), GroupMakePoints->GroupBox1);
  myPrefixEditGMP      = new QLineEdit(GroupMakePoints->GroupBox1);

  aLayoutGMP->addWidget(myStepByStepCheckGMP, 8,  0, 1, 3);
  aLayoutGMP->addWidget(myGenGroupCheckGMP,   9,  0, 1, 3);
  aLayoutGMP->addWidget(myPrefixLblGMP,       10, 0, 1, 2);
  aLayoutGMP->addWidget(myPrefixEditGMP,      10, 2);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  layout->addWidget(GroupMakePoints);
  /***************************************************************/

  setHelpFileName("create_extrusion_alongpath_page.html");

  // Initialisation
  Init();
}

//=================================================================================
// function : ~GenerationGUI_PipeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_PipeDlg::~GenerationGUI_PipeDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::Init()
{
  // init variables
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);
  GroupPoints->LineEdit3->setReadOnly(true);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");
  GroupPoints->LineEdit3->setText("");
  myPath.nullify();
  myVec.nullify();

  GroupPoints->CheckButton1->setEnabled(false);

  GroupMakePoints->LineEdit1->setReadOnly(true);
  GroupMakePoints->LineEdit2->setReadOnly(true);
  GroupMakePoints->LineEdit3->setReadOnly(true);

  GroupMakePoints->LineEdit1->setText("");
  GroupMakePoints->LineEdit2->setText("");
  GroupMakePoints->LineEdit3->setText("");

  showOnlyPreviewControl();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this,          SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->CheckButton1,   SIGNAL(toggled(bool)), this, SLOT(SelectionTypeButtonClicked()));

  connect(GroupMakePoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupMakePoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupMakePoints->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupMakePoints->CheckBox1, SIGNAL(clicked()), this, SLOT(processPreview()));
  connect(GroupMakePoints->CheckBox2, SIGNAL(clicked()), this, SLOT(processPreview()));
  connect(myStepByStepCheckGMP,       SIGNAL(clicked()), this, SLOT(processPreview()));
  connect(myGenGroupCheckGP,    SIGNAL(toggled(bool)), this, SLOT(GenGroupClicked(bool)));
  connect(myGenGroupCheckGMP,   SIGNAL(toggled(bool)), this, SLOT(GenGroupClicked(bool)));
  connect(myStepByStepCheckGMP, SIGNAL(toggled(bool)), this, SLOT(StepByStepClicked(bool)));

  initName(tr("GEOM_PIPE"));
  resize(100,100);

  GroupPoints->TextLabel3->hide();
  GroupPoints->PushButton3->hide();
  GroupPoints->LineEdit3->hide();
  ConstructorsClicked(0);

  GroupPoints->PushButton1->click();
  SelectionIntoArgument();
  updateGenGroup();
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GenerationGUI_PipeDlg::ConstructorsClicked( int constructorId )
{
  erasePreview();

  switch (constructorId) {
  case 0:
    GroupMakePoints->hide();
    GroupPoints->show();
    GroupPoints->TextLabel3->hide();
    GroupPoints->PushButton3->hide();
    GroupPoints->LineEdit3->hide();
    GroupPoints->PushButton1->click();
    myGenGroupCheckGP->setChecked(false);
    resetGenGroup(myGenGroupCheckGP, false, true);
    break;
  case 1:
    GroupMakePoints->hide();
    GroupPoints->show();
    GroupPoints->TextLabel3->show();
    GroupPoints->PushButton3->show();
    GroupPoints->LineEdit3->show();
    GroupPoints->PushButton1->click();
    myGenGroupCheckGP->setChecked(false);
    resetGenGroup(myGenGroupCheckGP, false, true);
    break;
  case 2:
    GroupPoints->hide();
    GroupMakePoints->show();
    GroupMakePoints->PushButton1->click();
    myStepByStepCheckGMP->setChecked(false);
    myGenGroupCheckGMP->setChecked(false);
    resetGenGroup(myGenGroupCheckGMP, false, true);
    break;
  default:
    break;
  }

  qApp->processEvents();
  updateGeometry();
  resize(100,100);

  SelectionIntoArgument();
  processPreview();
}

//=================================================================================
// function : SelectionBittonClicked()
// purpose  : Selection type Radio button management
//=================================================================================
void GenerationGUI_PipeDlg::SelectionTypeButtonClicked()
{
  globalSelection();
  if ( GroupPoints->CheckButton1->isChecked() ) {
    localSelection( TopAbs_EDGE );
  } else {
    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_COMPOUND);
    aMap.Add(GEOM_WIRE);
    aMap.Add(GEOM_LINE);
    globalSelection(aMap);
  }
  if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
    myEditCurrentArgument->setText("");
    myPath.nullify();
    updateGenGroup();
  }
  processPreview();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void GenerationGUI_PipeDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
    myBaseObjects.clear();
    QList<GEOM::GeomObjPtr> objects = getSelected( TopAbs_SHAPE, -1 );
    for ( int i = 0; i < objects.count(); i++ ) {
      GEOM::shape_type stype = objects[i]->GetMaxShapeType();
      GEOM::shape_type maxallowed = getConstructorId() == 1 ? GEOM::FACE : GEOM::SHELL;
      GEOM::shape_type minallowed = getConstructorId() == 1 ? GEOM::EDGE : GEOM::VERTEX;
      if ( stype < maxallowed || stype > minallowed )
        continue;
      myBaseObjects << objects[i];
    }
    if ( !myBaseObjects.isEmpty() ) {
      QString aName = myBaseObjects.count() > 1 ? QString( "%1_objects").arg( myBaseObjects.count() ) : GEOMBase::GetName( myBaseObjects[0].get() );
      myEditCurrentArgument->setText( aName );
    }
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
    QList<TopAbs_ShapeEnum> types;
    types << TopAbs_EDGE << TopAbs_WIRE;
    myPath = getSelected( types );
    if ( myPath ) {
      QString aName = GEOMBase::GetName( myPath.get() );
      myEditCurrentArgument->setText( aName );
      if ( getConstructorId() == 1 && !myVec )
        GroupPoints->PushButton3->click();
      else if ( myBaseObjects.isEmpty() )
        GroupPoints->PushButton1->click();
    }
    updateGenGroup();
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit3) {
    myVec = getSelected( TopAbs_EDGE );
    if ( myVec ) {
      QString aName = GEOMBase::GetName( myVec.get() );
      myEditCurrentArgument->setText( aName );
      if ( myBaseObjects.isEmpty() )
        GroupPoints->PushButton1->click();
      else if ( !myPath )
        GroupPoints->PushButton2->click();
    }
  }
  else if ( myEditCurrentArgument == GroupMakePoints->LineEdit1 ) {
    QList<TopAbs_ShapeEnum> types;
    types << TopAbs_EDGE << TopAbs_WIRE << TopAbs_FACE << TopAbs_SHELL;
    QList<GEOM::GeomObjPtr> objects = getSelected( types, -1 );
    GEOMBase::Synchronize( myBaseObjects, objects );
    if ( !myBaseObjects.isEmpty() ) {
      QString aName = myBaseObjects.count() > 1 ? QString( "%1_objects").arg( myBaseObjects.count() ) : GEOMBase::GetName( myBaseObjects[0].get() );
      myEditCurrentArgument->setText( aName );
    }
  }
  else if ( myEditCurrentArgument == GroupMakePoints->LineEdit2 ) {
    localSelection( TopAbs_VERTEX );
    QList<GEOM::GeomObjPtr> objects = getSelected( TopAbs_VERTEX, -1 );
    GEOMBase::Synchronize( myLocations, objects );
    if ( !myLocations.isEmpty() ) {
      QString aName = myLocations.count() > 1 ? QString( "%1_objects").arg( myLocations.count() ) : GEOMBase::GetName( myLocations[0].get() );
      myEditCurrentArgument->setText( aName );
    }
  }
  else if ( myEditCurrentArgument == GroupMakePoints->LineEdit3 ) {
    QList<TopAbs_ShapeEnum> types;
    types << TopAbs_EDGE << TopAbs_WIRE;
    myPath = getSelected( types );
    if ( myPath ) {
      QString aName = GEOMBase::GetName( myPath.get() );
      myEditCurrentArgument->setText( aName );
    }
    updateGenGroup();
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection(GEOM_ALLSHAPES);
  GroupPoints->PushButton1->setDown(false);
  GroupPoints->PushButton2->setDown(false);
  GroupPoints->PushButton3->setDown(false);
  GroupPoints->LineEdit1->setEnabled(false);
  GroupPoints->LineEdit2->setEnabled(false);
  GroupPoints->LineEdit3->setEnabled(false);
  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GroupPoints->CheckButton1->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;

    if ( GroupPoints->CheckButton1->isChecked() ) {
      localSelection( TopAbs_EDGE );
    } else {
      TColStd_MapOfInteger aMap;
      aMap.Add(GEOM_COMPOUND);
      aMap.Add(GEOM_WIRE);
      aMap.Add(GEOM_LINE);
      globalSelection(aMap);
    }
    GroupPoints->CheckButton1->setEnabled(true);
  }
  else if(send == GroupPoints->PushButton3) {
    myEditCurrentArgument = GroupPoints->LineEdit3;
    GroupPoints->CheckButton1->setEnabled(false);
    localSelection(TopAbs_EDGE);
  }

  GroupMakePoints->PushButton1->setDown(false);
  GroupMakePoints->PushButton2->setDown(false);
  GroupMakePoints->PushButton3->setDown(false);
  GroupMakePoints->LineEdit1->setEnabled(false);
  GroupMakePoints->LineEdit2->setEnabled(false);
  GroupMakePoints->LineEdit3->setEnabled(false);
  if (send == GroupMakePoints->PushButton1) {
    myEditCurrentArgument = GroupMakePoints->LineEdit1;
  }
  else if(send == GroupMakePoints->PushButton2) {
    myEditCurrentArgument = GroupMakePoints->LineEdit2;
  }
  else if (send == GroupMakePoints->PushButton3) {
    myEditCurrentArgument = GroupMakePoints->LineEdit3;
  }

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  processPreview();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked(getConstructorId());
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void GenerationGUI_PipeDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_PipeDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::isValid (QString&)
{
  bool ok = false;
  switch ( getConstructorId() ) {
  case 0 :
    ok = !myBaseObjects.isEmpty() && myPath;
    break;
  case 1 :
    ok = !myBaseObjects.isEmpty() && myPath && myVec;
    break;
  case 2 :
    ok = !myBaseObjects.isEmpty() && ( myLocations.isEmpty() || myBaseObjects.count() == myLocations.count() ) && myPath;
    break;
  default:
    break;
  }
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::execute (ObjectList& objects)
{
  GEOM::ListOfGO_var aList;

  GEOM::GEOM_I3DPrimOperations_var anOper = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());
  bool doGroups = !IsPreview();

  myGroupObjectsMap.clear();

  switch( getConstructorId() ) {
  case 0:
  case 1:
    if (doGroups) {
      doGroups = myGenGroupCheckGP->isEnabled() &&
                 myGenGroupCheckGP->isChecked();
    }

    for (int i = 0; i < myBaseObjects.count(); i++) {
      switch ( getConstructorId() ) {
      case 0 :
        aList = anOper->MakePipe(myBaseObjects[i].get(),
                                 myPath.get(), doGroups);
        break;
      case 1 :
        aList = anOper->MakePipeBiNormalAlongVector
              (myBaseObjects[i].get(), myPath.get(), myVec.get(), doGroups);
        break;
      }

      if (aList->length() > 0) {
        if (doGroups) {
          addGroups(aList);
        }

        if (!aList[0]->_is_nil()) {
          objects.push_back(aList[0]._retn());
        }
      }
    }
    break;
  case 2:
    {
      GEOM::ListOfGO_var myBaseGO = new GEOM::ListOfGO();
      GEOM::ListOfGO_var myLocationsGO = new GEOM::ListOfGO();
      myBaseGO->length( myBaseObjects.count() );
      myLocationsGO->length( myLocations.count() );
      for (int i = 0; i < myBaseObjects.count(); i++) {
        myBaseGO[i] = myBaseObjects[i].copy();
      }
      for (int i = 0; i < myLocations.count(); i++) {
        myLocationsGO[i] = myLocations[i].copy();
      }

      if (doGroups) {
        doGroups = myGenGroupCheckGMP->isEnabled() &&
                   myGenGroupCheckGMP->isChecked();
      }

      bool isWithContact    = GroupMakePoints->CheckBox1->isEnabled() &&
                              GroupMakePoints->CheckBox1->isChecked();
      bool isWithCorrection = GroupMakePoints->CheckBox2->isEnabled() &&
                              GroupMakePoints->CheckBox2->isChecked();

      aList = anOper->MakePipeWithDifferentSections
                          (myBaseGO.in(), myLocationsGO.in(), myPath.get(),
                           isWithContact, isWithCorrection,
                           myStepByStepCheckGMP->isChecked(),
                           doGroups);

      if (aList->length() > 0) {
        if (doGroups) {
          addGroups(aList);
        }

        if (!aList[0]->_is_nil()) {
          objects.push_back(aList[0]._retn());
        }
      }
    }
    break;
  default:
    break;
  }
  return true;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void GenerationGUI_PipeDlg::addSubshapesToStudy()
{
  GEOMBase::PublishSubObject( myPath.get() );
  GEOMBase::PublishSubObject( myVec.get() );
}

//=================================================================================
// function : extractPrefix
// purpose  : redefined from GEOMBase_Helper class
//=================================================================================
bool GenerationGUI_PipeDlg::extractPrefix() const
{
  return myBaseObjects.count() > 1;
}

//=================================================================================
// function : restoreSubShapes
// purpose  : virtual method to restore tree of argument's sub-shapes under
//            the resulting shape. Redefined from GEOMBase_Helper class.
//=================================================================================
void GenerationGUI_PipeDlg::restoreSubShapes
              (SALOMEDS::Study_ptr theStudy, SALOMEDS::SObject_ptr theSObject)
{
  QCheckBox *aGenGroupCheck = NULL;
  QLineEdit *aPrefixEdit    = NULL;

  switch (getConstructorId()) {
  case 0 :
  case 1 :
    aGenGroupCheck = myGenGroupCheckGP;
    aPrefixEdit    = myPrefixEditGP;
    break;
  case 2 :
    aGenGroupCheck = myGenGroupCheckGMP;
    aPrefixEdit    = myPrefixEditGMP;
    break;
  default:
    break;
  }

  if (aGenGroupCheck == NULL || !aGenGroupCheck->isChecked()) {
    return;
  }

  // get Object from SObject
  GEOM::GEOM_Object_var aFather = GEOM::GEOM_Object::_narrow
            (theSObject->GetObject());

  if (CORBA::is_nil(aFather)) {
    return;
  }

  ObjectMap::const_iterator anIter    =
    myGroupObjectsMap.find(aFather->GetEntry());
  QString                   aPrefix   = aPrefixEdit->text();
  bool                      hasPrefix = !aPrefix.isEmpty();

  if (anIter != myGroupObjectsMap.end()) {
    QList<GEOM::GeomObjPtr>::const_iterator it = anIter->second.begin();

    for (; it != anIter->second.end(); it++) {
      // Compose the name
      QString aName;

      if (hasPrefix) {
        aName = aPrefix + "_";
      }

      aName += tr((*it)->GetName());
      getGeomEngine()->AddInStudy(theStudy, (*it).get(),
                                  aName.toStdString().c_str(), aFather);
    }
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> GenerationGUI_PipeDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res(myBaseObjects);
  res << myPath << myVec;
  for (int i = 0; i < myLocations.count(); i++)
    res << myLocations[i];
  return res;
}

//=================================================================================
// function : GenGroupClicked
// purpose  : Slot to treat checking "Generate groups" check box.
//=================================================================================
void GenerationGUI_PipeDlg::GenGroupClicked(bool isChecked)
{
  resetGenGroup((QCheckBox *)sender(), isChecked, false);
}

//=================================================================================
// function : StepByStepClicked
// purpose  : Slot to treat checking "Step-by-step generation" check box.
//=================================================================================
void GenerationGUI_PipeDlg::StepByStepClicked(bool isChecked)
{
  GroupMakePoints->CheckBox1->setEnabled(!isChecked);
  GroupMakePoints->CheckBox2->setEnabled(!isChecked);
}

//=================================================================================
// function : updateGenGroup
// purpose  : Update "Generate groups" widgets depending on the path.
//=================================================================================
void GenerationGUI_PipeDlg::updateGenGroup()
{
  bool isEnable = true;

  if (myPath) {
    // Check if the path is closed.
    TopoDS_Shape aShapePath;

    if (GEOMBase::GetShape(myPath.get(), aShapePath)) {
      isEnable = GEOMUtils::IsOpenPath(aShapePath);
    }
  }

  QCheckBox *aGenGroupCheck = NULL;

  switch (getConstructorId()) {
  case 0 :
  case 1 :
    aGenGroupCheck = myGenGroupCheckGP;
    break;
  case 2 :
    aGenGroupCheck = myGenGroupCheckGMP;
    break;
  default:
    break;
  }

  if (aGenGroupCheck != NULL) {
    const bool isChecked = aGenGroupCheck->isChecked();

    aGenGroupCheck->setEnabled(isEnable);
    resetGenGroup(aGenGroupCheck, isEnable && isChecked, false);
  }
}

//=================================================================================
// function : resetGenGroup
// purpose  : Resets data of "Generate groups" widgets.
//=================================================================================
void GenerationGUI_PipeDlg::resetGenGroup(QCheckBox  *theGenGroup,
                                          const bool  isChecked,
                                          const bool  isClearPrefix)
{
  QLabel    *aPrefixLbl  = NULL;
  QLineEdit *aPrefixEdit = NULL;

  if (theGenGroup == myGenGroupCheckGP) {
    aPrefixLbl  = myPrefixLblGP;
    aPrefixEdit = myPrefixEditGP;
  } else if (theGenGroup == myGenGroupCheckGMP) {
    aPrefixLbl  = myPrefixLblGMP;
    aPrefixEdit = myPrefixEditGMP;
  }

  if (aPrefixLbl != NULL) {
    aPrefixLbl->setEnabled(isChecked);
    aPrefixEdit->setEnabled(isChecked);

    if (isClearPrefix) {
      aPrefixEdit->setText("");
    }
  }
}

//=================================================================================
// function : addGroups
// purpose  : Add result groups to the list of groups.
//=================================================================================
void GenerationGUI_PipeDlg::addGroups(GEOM::ListOfGO_var &theResult)
{
  const int aNbObj = theResult->length();

  if (aNbObj > 0) {
    if (!theResult[0]->_is_nil()) {
      QString anEntry = theResult[0]->GetEntry();
      int     i;

      for (i = 1; i < aNbObj; ++i) {
        if (!theResult[i]->_is_nil()) {
          myGroupObjectsMap[anEntry].append(GEOM::GeomObjPtr(theResult[i]));
        }
      }
    }
  }
}
