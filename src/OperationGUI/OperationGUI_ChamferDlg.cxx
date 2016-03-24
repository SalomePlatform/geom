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
// File   : OperationGUI_ChamferDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.

#include "OperationGUI_ChamferDlg.h"
#include <SalomeApp_DoubleSpinBox.h>

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <OCCViewer_ViewModel.h>

// OCCT Includes
#include <TColStd_MapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : OperationGUI_ChamferDlg()
// purpose  : Constructs a OperationGUI_ChamferDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_ChamferDlg::OperationGUI_ChamferDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_CHAMFER_ALL")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_CHAMFER_EDGE_FROM_FACE")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_CHAMFER_FACE")));
  QPixmap image4 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_CHAMFER_EDGE")));

  QPixmap iconSelect (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_CHAMFER_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_CHAMFER"));

  mainFrame()->RadioButton1->setIcon(image1);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setIcon(image3);
  mainFrame()->RadioButton4->show();
  mainFrame()->RadioButton4->setIcon(image4);

  // Create first group

  myGrp1 = new QGroupBox(tr("GEOM_CHAMFER_ALL"), centralWidget());

  QGridLayout* aLayout = new QGridLayout(myGrp1);
  aLayout->setMargin(9); aLayout->setSpacing(6);

  createSelWg(tr("GEOM_MAIN_OBJECT"), iconSelect, myGrp1, aLayout, MainObj1);

  int row = aLayout->rowCount();
  aLayout->addWidget(new QLabel(tr("D"), myGrp1), row, 0);
  aLayout->addWidget((mySpinBox[ SpinBox1 ] = new SalomeApp_DoubleSpinBox(myGrp1)), row++, 2);
  aLayout->setRowStretch(row, 10);

  // Create second group

  myGrp2 = new QGroupBox(tr("GEOM_CHAMFER_EDGE"), centralWidget());

  aLayout = new QGridLayout(myGrp2);
  aLayout->setMargin(9); aLayout->setSpacing(6);

  createSelWg(tr("GEOM_MAIN_OBJECT"), iconSelect, myGrp2, aLayout, MainObj2);
  createSelWg(tr("FACE_1"),           iconSelect, myGrp2, aLayout, Face1);
  createSelWg(tr("FACE_2"),           iconSelect, myGrp2, aLayout, Face2);

  createRadioWg(tr("GEOM_D1"), tr("GEOM_D2"),    myGrp2, aLayout, RadioButton21, SpinBox21, SpinBox22);
  createRadioWg(tr("GEOM_D"),  tr("GEOM_ANGLE"), myGrp2, aLayout, RadioButton22, SpinBox23, SpinBox24);

  aLayout->setRowStretch(aLayout->rowCount(), 10);

  // Create third group

  myGrp3 = new QGroupBox(tr("GEOM_CHAMFER_FACES"), centralWidget());

  aLayout = new QGridLayout(myGrp3);
  aLayout->setMargin(9); aLayout->setSpacing(6);

  createSelWg(tr("GEOM_MAIN_OBJECT"), iconSelect, myGrp3, aLayout, MainObj3);
  createSelWg(tr("SELECTED_FACES"),   iconSelect, myGrp3, aLayout, Faces);

  createRadioWg(tr("GEOM_D1"), tr("GEOM_D2"),    myGrp3, aLayout, RadioButton31, SpinBox31, SpinBox32);
  createRadioWg(tr("GEOM_D"),  tr("GEOM_ANGLE"), myGrp3, aLayout, RadioButton32, SpinBox33, SpinBox34);

  aLayout->setRowStretch(aLayout->rowCount(), 10);

  // Create fourth group

  myGrp4 = new QGroupBox(tr("GEOM_CHAMFER_EDGES"), centralWidget());

  aLayout = new QGridLayout(myGrp4);
  aLayout->setMargin(9); aLayout->setSpacing(6);

  createSelWg(tr("GEOM_MAIN_OBJECT"), iconSelect, myGrp4, aLayout, MainObj4);
  createSelWg(tr("SELECTED_EDGE"),    iconSelect, myGrp4, aLayout, Edges);

  createRadioWg(tr("GEOM_D1"), tr("GEOM_D2"),    myGrp4, aLayout, RadioButton41, SpinBox41, SpinBox42);
  createRadioWg(tr("GEOM_D"),  tr("GEOM_ANGLE"), myGrp4, aLayout, RadioButton42, SpinBox43, SpinBox44);

  aLayout->setRowStretch(aLayout->rowCount(), 10);

  // Add groups to layout

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGrp1);
  layout->addWidget(myGrp2);
  layout->addWidget(myGrp3);
  layout->addWidget(myGrp4);
  /***************************************************************/

  // Set range of spinboxes
  double SpecificStep = 10.0;
  QMap< int, SalomeApp_DoubleSpinBox* >::iterator anIter;
  for (anIter = mySpinBox.begin(); anIter != mySpinBox.end(); ++anIter) {
    if (anIter.key() == SpinBox44 || anIter.key() == SpinBox34 || anIter.key() == SpinBox24)
      initSpinBox(anIter.value(), 0.001, 89.999, 5, "angle_precision" );
    else
      initSpinBox(anIter.value(), 0.001, COORD_MAX, SpecificStep, "length_precision" );
  }

  setHelpFileName("chamfer_operation_page.html");

  // Initialisation
  Init();
  myRadioButton[ RadioButton21 ]->click();
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
void OperationGUI_ChamferDlg::Init()
{
  // Set Initial values of spinboxes
  QMap< int, SalomeApp_DoubleSpinBox* >::iterator anIter;
  for (anIter = mySpinBox.begin(); anIter != mySpinBox.end(); ++anIter)
    anIter.value()->setValue(5);

  // Clear line edits
  QMap< int, QLineEdit* >::iterator anIterLE;
  for (anIterLE = mySelName.begin(); anIterLE != mySelName.end(); ++anIterLE)
    anIterLE.value()->setText("");

  myShape = GEOM::GEOM_Object::_nil();

  myFaces.Clear();
  myEdges.Clear();
  myFace[ Face1 ] = -1;
  myFace[ Face2 ] = -1;

  showOnlyPreviewControl();

  // signals and slots connections

  // main buttons
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()   ));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  // group box
  connect(this,          SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  // push buttons
  QMap< int, QPushButton* >::iterator anIterBtn;
  for (anIterBtn = mySelBtn.begin(); anIterBtn != mySelBtn.end(); ++anIterBtn)
    connect(anIterBtn.value(), SIGNAL(clicked()),
            this, SLOT(SetEditCurrentArgument()));

  // line edits
  QMap< int, QLineEdit* >::iterator anIterLE2;
  for (anIterLE2 = mySelName.begin(); anIterLE2 != mySelName.end(); ++anIterLE2)
    connect(anIterLE2.value(), SIGNAL(returnPressed()),
            this, SLOT(LineEditReturnPressed()));

  // spin boxes
  QMap< int, SalomeApp_DoubleSpinBox* >::iterator anIterSpin;
  for (anIterSpin = mySpinBox.begin(); anIterSpin != mySpinBox.end(); ++anIterSpin)
    connect(anIterSpin.value(), SIGNAL(valueChanged(double)),
            this, SLOT(ValueChangedInSpinBox(double)));

  // radio buttons
  QMap< int, QRadioButton* >::iterator anIterRadio;
  for (anIterRadio = myRadioButton.begin(); anIterRadio != myRadioButton.end(); ++anIterRadio)
    connect(anIterRadio.value(), SIGNAL(clicked()),
            this, SLOT(RadioButtonPressed()));

  initName(tr("GEOM_CHAMFER"));

  myConstructorId = -1;
  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void OperationGUI_ChamferDlg::ConstructorsClicked (int constructorId)
{
   // Activate next widget
  if (myGeomGUI->getApp()->desktop()->activeWindow()->getViewManager()->getType()
      != OCCViewer_Viewer::Type()) {
    mainFrame()->RadioButton1->setChecked(true);
    return;
  }

  if (myConstructorId == constructorId)
    return;

  //disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  // Get values from previous widget
  QString D1 = "5", D2 = "5", D = "5", Angle = "5";
  if (myConstructorId == 0)
    D1 = D2 = mySpinBox[ SpinBox1 ]->text();
  else if (myConstructorId == 1) {
    D1 = mySpinBox[ SpinBox21 ]->text();
    D2 = mySpinBox[ SpinBox22 ]->text();
    D =  mySpinBox[ SpinBox23 ]->text();
    Angle = mySpinBox[ SpinBox24 ]->text();
  }
  else if (myConstructorId == 2) {
    D1 = mySpinBox[ SpinBox31 ]->text();
    D2 = mySpinBox[ SpinBox32 ]->text();
    D =  mySpinBox[ SpinBox33 ]->text();
    Angle = mySpinBox[ SpinBox34 ]->text();
  }
  else if (myConstructorId == 3) {
    D1 = mySpinBox[ SpinBox41 ]->text();
    D2 = mySpinBox[ SpinBox42 ]->text();
    D =  mySpinBox[ SpinBox43 ]->text();
    Angle = mySpinBox[ SpinBox44 ]->text();
  }

  myConstructorId = constructorId;

  switch (constructorId) {
  case 0:
    myGrp2->hide();
    myGrp3->hide();
    myGrp4->hide();
    myGrp1->show();
    mySpinBox[ SpinBox1 ]->setText(D1);
    break;
  case 1:
    myGrp1->hide();
    myGrp3->hide();
    myGrp4->hide();
    myGrp2->show();
    mySpinBox[ SpinBox21 ]->setText(D1);
    mySpinBox[ SpinBox22 ]->setText(D2);
    mySpinBox[ SpinBox23 ]->setText(D);
    mySpinBox[ SpinBox24 ]->setText(Angle);
    break;
  case 2:
    myGrp1->hide();
    myGrp2->hide();
    myGrp4->hide();
    myGrp3->show();
    mySpinBox[ SpinBox31 ]->setText(D1);
    mySpinBox[ SpinBox32 ]->setText(D2);
    mySpinBox[ SpinBox32 ]->setText(D2);
    mySpinBox[ SpinBox33 ]->setText(D);
    mySpinBox[ SpinBox34 ]->setText(Angle);
    break;
  case 3:
    myGrp1->hide();
    myGrp2->hide();
    myGrp3->hide();
    myGrp4->show();
    mySpinBox[ SpinBox41 ]->setText(D1);
    mySpinBox[ SpinBox42 ]->setText(D2);
    mySpinBox[ SpinBox43 ]->setText(D);
    mySpinBox[ SpinBox44 ]->setText(Angle);
    break;
  default:
    break;
  }

  if      (constructorId == 0) mySelBtn[ MainObj1 ]->click();
  else if (constructorId == 1) mySelBtn[ MainObj2 ]->click();
  else if (constructorId == 2) mySelBtn[ MainObj3 ]->click();
  else                         mySelBtn[ MainObj4 ]->click();

  enableWidgets();

  if (myInitial) {
    myInitial = false;
    SelectionIntoArgument();
  }
  else {
    if (!myShape->_is_nil()) {
      myEditCurrentArgument->setText(GEOMBase::GetName(myShape));
      GEOMBase_Skeleton::LineEditReturnPressed();
      switch (getConstructorId()) {
      case 1:
        if (myFace[ Face1 ] == -1)
          mySelBtn[ Face1 ]->click();
        break;
      case 2:
        if (myFaces.Extent() == 0)
          mySelBtn[ Faces ]->click();
        break;
      case 3:
        if (myEdges.Extent() == 0)
          mySelBtn[ Edges ]->click();
        break;
      default:
        break;
      }
    }
    else {
      myEditCurrentArgument->setText("");
    }

    processPreview();
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_ChamferDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate selection and connect selection manager
  ConstructorsClicked(getConstructorId());
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void OperationGUI_ChamferDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  // Get index of current selection focus
  int aCurrFocus = -1;
  QMap< int, QLineEdit* >::iterator anIter;
  for (anIter = mySelName.begin(); anIter != mySelName.end(); ++anIter) {
    if (myEditCurrentArgument == anIter.value()) {
      aCurrFocus = anIter.key();
      break;
    }
  }

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  // If selection of main object is activated
  if (aCurrFocus == MainObj1 || aCurrFocus == MainObj2 || aCurrFocus == MainObj3 || aCurrFocus == MainObj4)
  {
    myShape = GEOM::GEOM_Object::_nil();
    if (aSelList.Extent() == 1) {
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

      if ( !anObj->_is_nil() ) {
        myShape = anObj;
        myEditCurrentArgument->setText(GEOMBase::GetName(anObj));
        processPreview();
      }
    }

    enableWidgets();
  }
  // If face selection of second tab is activated
  else if (aCurrFocus == Face1 || aCurrFocus == Face2)
  {
    myFace[ aCurrFocus ] = -1;
    if (aSelList.Extent() == 1) {
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

      if ( !anObj->_is_nil() ) {
         TColStd_IndexedMapOfInteger anIndexes;
         aSelMgr->GetIndexes(aSelList.First(), anIndexes);

         if (anIndexes.Extent() == 1) {
            int anIndex = anIndexes(1);
            QString aFaceName = QString(GEOMBase::GetName(anObj)) + ":%1";
            myEditCurrentArgument->setText(aFaceName.arg(anIndex));
            myFace[ aCurrFocus ] = anIndex;
            processPreview();
         }
      }
    }
  }
  // If face selection of third or fourth tab is activated
  else if (aCurrFocus == Faces || aCurrFocus == Edges) {
    if (aCurrFocus == Faces) myFaces.Clear();
    else myEdges.Clear();
    if (aSelList.Extent() == 1) {
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

      if ( !anObj->_is_nil() ) {
        TColStd_IndexedMapOfInteger anIndexes;
        aSelMgr->GetIndexes(aSelList.First(), anIndexes);

        if (anIndexes.Extent() > 0) {
          QString aName;
          if (anIndexes.Extent() == 1) {
            int anIndex = anIndexes(1);

            aName = QString(GEOMBase::GetName(anObj)) + QString(":%1").arg(anIndex);
          }
          else {
            aName = tr("GEOM_MEN_POPUP_NAME").arg(anIndexes.Extent());
          }

          myEditCurrentArgument->setText(aName);
          int aConstructorId = getConstructorId();
          if (aConstructorId == 2)
            myFaces = anIndexes;
          else if (aConstructorId == 3)
            myEdges = anIndexes;

          processPreview();
        }
      }
    }
  }

  // clear selection
  if (aCurrFocus != Face1 && aCurrFocus != Face2 &&
      aCurrFocus != Faces && aCurrFocus != Edges) {
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    myGeomGUI->getApp()->selectionMgr()->clearSelected();
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()));
  }

  switch (getConstructorId()) {
  case 1:
    if (aCurrFocus == MainObj2) {
      if (!myShape->_is_nil() && myFace[ Face1 ] == -1)
        mySelBtn[ Face1 ]->click();
    }
    else if (aCurrFocus == Face1) {
      if (myFace[ Face1 ] != -1 && myFace[ Face2 ] == -1)
        mySelBtn[ Face2 ]->click();
    }
    else if (aCurrFocus == Face2) {
      if (myFace[ Face2 ] != -1 && myShape->_is_nil())
        mySelBtn[ MainObj2 ]->click();
    }
    break;
  case 2:
    if (aCurrFocus == MainObj3) {
      if (!myShape->_is_nil() && myFaces.Extent() == 0)
        mySelBtn[ Faces ]->click();
    }
    break;
  case 3:
    if (aCurrFocus == MainObj4) {
      if (!myShape->_is_nil() && myEdges.Extent() == 0)
        mySelBtn[ Edges ]->click();
    }
    break;
  default:
    break;
  }

  //rnv: To fix the bug IPAL22041 TC5.1.5: "Fillet Construcion" dialog loses current selection.
  // Restore selection of the main shape, if need,
  // because it was canceled.
  aSelMgr->selectedObjects(aSelList);
  if (aSelList.Extent() == 0 && !myShape->_is_nil()) {
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    ObjectList list;
        list.push_back(myShape);
    selectObjects(list);
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()));
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  QMap< int, QPushButton* >::iterator anIter;
  for (anIter = mySelBtn.begin(); anIter != mySelBtn.end(); ++anIter) {
    if (anIter.value() == send) {
      mySelName[ anIter.key() ]->setFocus();
      myEditCurrentArgument = mySelName[ anIter.key() ];
    }
    else {
      anIter.value()->setDown(false);
      mySelName[ anIter.key() ]->setEnabled(false);
    }
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  activateSelection();

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  processPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();

  QMap< int, QLineEdit* >::iterator anIterLE;
  for (anIterLE = mySelName.begin(); anIterLE != mySelName.end(); ++anIterLE)
    if (anIterLE.value() == send)
      myEditCurrentArgument = anIterLE.value();

  GEOMBase_Skeleton::LineEditReturnPressed();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    this->ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ValueChangedInSpinBox (double)
{
  processPreview();
}

//=================================================================================
// function : createSelWg()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::createSelWg (const QString& theLbl,
                                           QPixmap&       thePix,
                                           QWidget*       theParent,
                                           QGridLayout*   theLayout,
                                           const int      theId)
{
  QLabel* lab = new QLabel(theLbl, theParent);
  mySelBtn[ theId ] = new QPushButton(theParent);
  mySelBtn[ theId ]->setIcon(thePix);
  mySelName[ theId ] = new QLineEdit(theParent);
  mySelName[ theId ]->setReadOnly(true);
  int row = theLayout->rowCount();
  theLayout->addWidget(lab,                row, 0);
  theLayout->addWidget(mySelBtn[ theId ],  row, 1);
  theLayout->addWidget(mySelName[ theId ], row, 2, 1, 4); // take into account createRadioWg()
}

//=================================================================================
// function : createRadioWg()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::createRadioWg(const QString& theLbl1,
                                             const QString& theLbl2,
                                             QWidget*       theParent,
                                             QGridLayout*   theLayout,
                                             const int      theRbId,
                                             const int      theSpin1Id,
                                             const int      theSpin2Id)
{
  myRadioButton[ theRbId ] = new QRadioButton(theParent);
  QLabel* lab1 = new QLabel(theLbl1, theParent);
  QLabel* lab2 = new QLabel(theLbl2, theParent);
  mySpinBox[ theSpin1Id ]  = new SalomeApp_DoubleSpinBox(theParent);
  mySpinBox[ theSpin2Id ]  = new SalomeApp_DoubleSpinBox(theParent);
  int row = theLayout->rowCount();
  theLayout->addWidget(myRadioButton[ theRbId ], row, 0);
  theLayout->addWidget(lab1,                     row, 2);
  theLayout->addWidget(mySpinBox[ theSpin1Id ],  row, 3);
  theLayout->addWidget(lab2,                     row, 4);
  theLayout->addWidget(mySpinBox[ theSpin2Id ],  row, 5);
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void OperationGUI_ChamferDlg::activateSelection()
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  if (!myShape->_is_nil() &&
      (myEditCurrentArgument == mySelName[ Face1 ] ||
       myEditCurrentArgument == mySelName[ Face2 ] ||
       myEditCurrentArgument == mySelName[ Faces ])) {
    localSelection(myShape, TopAbs_FACE);
  }
  else if (!myShape->_is_nil() && myEditCurrentArgument == mySelName[ Edges ]) {
    localSelection(myShape, TopAbs_EDGE);
  }
  else {
    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_SHELL);
    aMap.Add(GEOM_SOLID);
    aMap.Add(GEOM_COMPOUND);
    globalSelection(aMap);
  }

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : enableWidgets
// purpose  : Enable widgets of faces in accordance with value of main object
//=================================================================================
void OperationGUI_ChamferDlg::enableWidgets()
{
  int anId = getConstructorId();

  bool toEnable = !myShape->_is_nil();

  if (anId == 1)
  {
    //mySelName[ Face1 ]->setEnabled(toEnable);
    //mySelName[ Face2 ]->setEnabled(toEnable);
    mySelBtn[ Face1 ]->setEnabled(toEnable);
    mySelBtn[ Face2 ]->setEnabled(toEnable);

    if (!toEnable)
    {
      mySelName[ Face1 ]->setText("");
      mySelName[ Face2 ]->setText("");
      myFace[ Face1 ] = -1;
      myFace[ Face2 ] = -1;
    }
  }
  else if (anId == 2)
  {
    //mySelName[ Faces ]->setEnabled(toEnable);
    mySelBtn[ Faces ]->setEnabled(toEnable);
    if (!toEnable) {
      mySelName[ Faces ]->setText("");
      myFaces.Clear();
    }
  }
  else if (anId == 3) {
    //mySelName[ Edges ]->setEnabled(toEnable);
    mySelBtn[ Edges ]->setEnabled(toEnable);

    if (!toEnable) {
      mySelName[ Edges ]->setText("");
      myEdges.Clear();
    }
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_ChamferDlg::createOperation()
{
  return getGeomEngine()->GetILocalOperations(getStudyId());
}

//=================================================================================
// function : isValid()
// purpose  : Verify validity of input data
//=================================================================================
bool OperationGUI_ChamferDlg::isValid (QString& msg)
{
  bool ok = true;
  switch (getConstructorId())
  {
    case 0:
      ok = mySpinBox[ SpinBox1 ]->isValid( msg, !IsPreview() );
      return !myShape->_is_nil() && ok;
    case 1:
      if (myRadioButton[ RadioButton21 ]->isChecked())
      {
        ok = mySpinBox[ SpinBox21 ]->isValid( msg, !IsPreview() ) && ok;
        ok = mySpinBox[ SpinBox22 ]->isValid( msg, !IsPreview() ) && ok;
      }
      else
      {
        ok = mySpinBox[ SpinBox23 ]->isValid( msg, !IsPreview() ) && ok;
        ok = mySpinBox[ SpinBox24 ]->isValid( msg, !IsPreview() ) && ok;
      }
      return !myShape->_is_nil() && myFace[ Face1 ] > 0 && myFace[ Face2 ] > 0 && ok;
    case 2:
      if (myRadioButton[ RadioButton31 ]->isChecked())
      {
        ok = mySpinBox[ SpinBox31 ]->isValid( msg, !IsPreview() ) && ok;
        ok = mySpinBox[ SpinBox32 ]->isValid( msg, !IsPreview() ) && ok;
      }
      else
      {
        ok = mySpinBox[ SpinBox33 ]->isValid( msg, !IsPreview() ) && ok;
        ok = mySpinBox[ SpinBox34 ]->isValid( msg, !IsPreview() ) && ok;
      }
      return !myShape->_is_nil() && myFaces.Extent() > 0 && ok;
    case 3:
      if (myRadioButton[ RadioButton41 ]->isChecked())
      {
        ok = mySpinBox[ SpinBox41 ]->isValid( msg, !IsPreview() ) && ok;
        ok = mySpinBox[ SpinBox42 ]->isValid( msg, !IsPreview() ) && ok;
      }
      else
      {
        ok = mySpinBox[ SpinBox43 ]->isValid( msg, !IsPreview() ) && ok;
        ok = mySpinBox[ SpinBox44 ]->isValid( msg, !IsPreview() ) && ok;
      }
      return !myShape->_is_nil() && myEdges.Extent() > 0 && ok;
    default: return false;
  }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_ChamferDlg::execute (ObjectList& objects)
{
  QStringList aParameters;
  GEOM::GEOM_Object_var anObj;
  bool flag = (myRadioButton[ RadioButton21 ]->isChecked() &&
               myRadioButton[ RadioButton31 ]->isChecked() &&
               myRadioButton[ RadioButton41 ]->isChecked());

  int anId = getConstructorId();

  GEOM::GEOM_ILocalOperations_var anOper = GEOM::GEOM_ILocalOperations::_narrow(getOperation());

  if (anId == 0) {
    anObj = anOper->MakeChamferAll(myShape, mySpinBox[ SpinBox1 ]->value());
    if (!anObj->_is_nil())
      aParameters << mySpinBox[ SpinBox1 ]->text();
  }
  else if (anId == 1) {
    if (flag) {
      anObj = anOper->MakeChamferEdge(myShape,
                                      mySpinBox[ SpinBox21 ]->value(),
                                      mySpinBox[ SpinBox22 ]->value(),
                                      myFace[ Face1 ],
                                      myFace[ Face2 ]);
      if (!anObj->_is_nil())
      {
        aParameters << mySpinBox[ SpinBox21 ]->text();
        aParameters << mySpinBox[ SpinBox22 ]->text();
      }
    }
    else {
      anObj = anOper->MakeChamferEdgeAD(myShape,
                                        mySpinBox[ SpinBox23 ]->value(),
                                        mySpinBox[ SpinBox24 ]->value() * M_PI / 180.,
                                        myFace[ Face1 ],
                                        myFace[ Face2 ]);
      if (!anObj->_is_nil())
      {
        aParameters << mySpinBox[ SpinBox23 ]->text();
        aParameters << mySpinBox[ SpinBox24 ]->text();
      }
    }
  }
  else if (anId == 2)
  {
    GEOM::ListOfLong_var anArray = new GEOM::ListOfLong;
    anArray->length(myFaces.Extent());

    for (int i = 1, n = myFaces.Extent(); i <= n; i++)
      anArray[ i - 1 ] = myFaces(i);

    if (flag) {
      anObj = anOper->MakeChamferFaces(myShape,
                                       mySpinBox[ SpinBox31 ]->value(),
                                       mySpinBox[ SpinBox32 ]->value(),
                                       anArray);
      if (!anObj->_is_nil())
      {
        aParameters << mySpinBox[ SpinBox31 ]->text();
        aParameters << mySpinBox[ SpinBox32 ]->text();
      }
    }
    else {
      anObj = anOper->MakeChamferFacesAD(myShape,
                                         mySpinBox[ SpinBox33 ]->value(),
                                         mySpinBox[ SpinBox34 ]->value() * M_PI / 180.,
                                         anArray);
      if (!anObj->_is_nil())
      {
        aParameters << mySpinBox[ SpinBox33 ]->text();
        aParameters << mySpinBox[ SpinBox34 ]->text();
      }
    }
  }
  else if (anId == 3) {
    GEOM::ListOfLong_var anArray = new GEOM::ListOfLong;
    anArray->length(myEdges.Extent());
    for (int i = 1, n = myEdges.Extent(); i <= n; i++)
      anArray[ i - 1 ] = myEdges(i);
    if (flag) {
      anObj = anOper->MakeChamferEdges(myShape,
                                       mySpinBox[ SpinBox41 ]->value(),
                                       mySpinBox[ SpinBox42 ]->value(),
                                       anArray);
      if (!anObj->_is_nil())
      {
        aParameters << mySpinBox[ SpinBox41 ]->text();
        aParameters << mySpinBox[ SpinBox42 ]->text();
      }
    }
    else {
      anObj = anOper->MakeChamferEdgesAD(myShape,
                                         mySpinBox[ SpinBox43 ]->value(),
                                         mySpinBox[ SpinBox44 ]->value() * M_PI / 180.,
                                         anArray);
      if (!anObj->_is_nil())
      {
        aParameters << mySpinBox[ SpinBox43 ]->text();
        aParameters << mySpinBox[ SpinBox44 ]->text();
      }
    }
  }

  if (!anObj->_is_nil())
  {
    if (!IsPreview())
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    objects.push_back(anObj._retn());
  }

  return true;
}

//=================================================================================
// function : RadioButtonPressed()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::RadioButtonPressed()
{
  const QObject* s = sender();
  bool flag = s == myRadioButton[ RadioButton21 ] ||
              s == myRadioButton[ RadioButton31 ] ||
              s == myRadioButton[ RadioButton41 ];

  myRadioButton[ RadioButton21 ]->blockSignals(true);
  myRadioButton[ RadioButton22 ]->blockSignals(true);
  myRadioButton[ RadioButton31 ]->blockSignals(true);
  myRadioButton[ RadioButton32 ]->blockSignals(true);
  myRadioButton[ RadioButton41 ]->blockSignals(true);
  myRadioButton[ RadioButton42 ]->blockSignals(true);

  myRadioButton[ RadioButton21 ]->setChecked(flag);
  myRadioButton[ RadioButton31 ]->setChecked(flag);
  myRadioButton[ RadioButton41 ]->setChecked(flag);
  myRadioButton[ RadioButton22 ]->setChecked(!flag);
  myRadioButton[ RadioButton32 ]->setChecked(!flag);
  myRadioButton[ RadioButton42 ]->setChecked(!flag);
  mySpinBox[ SpinBox21 ]->setEnabled(flag);
  mySpinBox[ SpinBox22 ]->setEnabled(flag);
  mySpinBox[ SpinBox31 ]->setEnabled(flag);
  mySpinBox[ SpinBox32 ]->setEnabled(flag);
  mySpinBox[ SpinBox41 ]->setEnabled(flag);
  mySpinBox[ SpinBox42 ]->setEnabled(flag);
  mySpinBox[ SpinBox23 ]->setEnabled(!flag);
  mySpinBox[ SpinBox24 ]->setEnabled(!flag);
  mySpinBox[ SpinBox33 ]->setEnabled(!flag);
  mySpinBox[ SpinBox34 ]->setEnabled(!flag);
  mySpinBox[ SpinBox43 ]->setEnabled(!flag);
  mySpinBox[ SpinBox44 ]->setEnabled(!flag);

  myRadioButton[ RadioButton21 ]->blockSignals(false);
  myRadioButton[ RadioButton22 ]->blockSignals(false);
  myRadioButton[ RadioButton31 ]->blockSignals(false);
  myRadioButton[ RadioButton32 ]->blockSignals(false);
  myRadioButton[ RadioButton41 ]->blockSignals(false);
  myRadioButton[ RadioButton42 ]->blockSignals(false);

  processPreview();
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> OperationGUI_ChamferDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myShape);
  res << aGeomObjPtr;
  return res;
}
