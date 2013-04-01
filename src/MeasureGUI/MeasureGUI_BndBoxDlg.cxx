// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
// File   : MeasureGUI_BndBoxDlg.cxx
// Author : Nicolas REJNERI, Open CASCADE S.A.S.

#include "MeasureGUI_BndBoxDlg.h"
#include "MeasureGUI_Widgets.h"

#include "GeometryGUI.h"

#include <GEOMBase.h>
#include <DlgRef.h>

#include <BRepPrimAPI_MakeBox.hxx>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_OverrideCursor.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Tools.h>
#include <LightApp_SelectionMgr.h>

//=================================================================================
// class    : MeasureGUI_BndBoxDlg()
// purpose  : Constructs a MeasureGUI_BndBoxDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_BndBoxDlg::MeasureGUI_BndBoxDlg (GeometryGUI* GUI, QWidget* parent)
  : GEOMBase_Skeleton(GUI, parent)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BOUNDING_BOX")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_BNDBOX_TITLE"));

  /***************************************************************/

  mainFrame()->GroupConstructors->setTitle(tr("GEOM_BNDBOX"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  myGrp = new MeasureGUI_1Sel6LineEdit (centralWidget());
  myGrp->GroupBox1->setTitle( tr( "GEOM_BNDBOX_OBJDIM" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->TextLabel2->setText( tr( "GEOM_MIN" ) );
  myGrp->TextLabel3->setText( tr( "GEOM_MAX" ) );
  myGrp->TextLabel4->setText( tr( "GEOM_X" ) );
  myGrp->TextLabel5->setText( tr( "GEOM_Y" ) );
  myGrp->TextLabel6->setText( tr( "GEOM_Z" ) );
  myGrp->LineEdit11->setReadOnly( true );
  myGrp->LineEdit12->setReadOnly( true );
  myGrp->LineEdit21->setReadOnly( true );
  myGrp->LineEdit22->setReadOnly( true );
  myGrp->LineEdit31->setReadOnly( true );
  myGrp->LineEdit32->setReadOnly( true );
  myGrp->LineEdit11->setMinimumWidth(120);
  myGrp->LineEdit12->setMinimumWidth(120);
  myGrp->LineEdit21->setMinimumWidth(120);
  myGrp->LineEdit22->setMinimumWidth(120);
  myGrp->LineEdit31->setMinimumWidth(120);
  myGrp->LineEdit32->setMinimumWidth(120);
  myGrp->PushButton1->setIcon( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp );

  /***************************************************************/

  myHelpFileName = "bounding_box_page.html";

  /* Initialisation */
  Init();
}

//=================================================================================
// function : ~MeasureGUI_BndBoxDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_BndBoxDlg::~MeasureGUI_BndBoxDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_BndBoxDlg::Init()
{
  myEditCurrentArgument = myGrp->LineEdit1;

  connect(buttonOk(),         SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(),      SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(myGrp->PushButton1, SIGNAL(clicked()), this, SLOT(SelectionIntoArgument()));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_BNDBOX"));
  globalSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void MeasureGUI_BndBoxDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool MeasureGUI_BndBoxDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  return true;
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_BndBoxDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  globalSelection();
  redisplayPreview();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_BndBoxDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_BndBoxDlg::processObject()
{
  double aXMin, aXMax, aYMin, aYMax, aZMin, aZMax;

  if (!getParameters(aXMin, aXMax, aYMin, aYMax, aZMin, aZMax)) {
    myEditCurrentArgument->setText("");
    myGrp->LineEdit11->setText("");
    myGrp->LineEdit12->setText("");
    myGrp->LineEdit21->setText("");
    myGrp->LineEdit22->setText("");
    myGrp->LineEdit31->setText("");
    myGrp->LineEdit32->setText("");
  }
  else {
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    int aPrecision = resMgr->integerValue( "Geometry", "length_precision", 6 );

    myGrp->LineEdit11->setText( DlgRef::PrintDoubleValue( aXMin, aPrecision ) );
    myGrp->LineEdit12->setText( DlgRef::PrintDoubleValue( aXMax, aPrecision ) );

    myGrp->LineEdit21->setText( DlgRef::PrintDoubleValue( aYMin, aPrecision ) );
    myGrp->LineEdit22->setText( DlgRef::PrintDoubleValue( aYMax, aPrecision ) );

    myGrp->LineEdit31->setText( DlgRef::PrintDoubleValue( aZMin, aPrecision ) );
    myGrp->LineEdit32->setText( DlgRef::PrintDoubleValue( aZMax, aPrecision ) );
  }
}

//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
bool MeasureGUI_BndBoxDlg::getParameters (double& theXmin, double& theXmax,
                                          double& theYmin, double& theYmax,
                                          double& theZmin, double& theZmax)
{
  if (myObj->_is_nil())
    return false;

  GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow(getOperation());
  anOper->GetBoundingBox(myObj, theXmin, theXmax, theYmin, theYmax, theZmin, theZmax);

  return anOper->IsDone();
}

//=================================================================================
// function : buildPrs
// purpose  :
//=================================================================================
SALOME_Prs* MeasureGUI_BndBoxDlg::buildPrs()
{
  double aXMin, aYMin, aZMin, aXMax, aYMax, aZMax;

  if (!getParameters(aXMin, aXMax, aYMin, aYMax, aZMin, aZMax))
    return 0;

  TopoDS_Shape aShape = BRepPrimAPI_MakeBox(gp_Pnt(aXMin, aYMin, aZMin),
                                            gp_Pnt(aXMax, aYMax, aZMax)).Shape();

  return !aShape.IsNull() ? getDisplayer()->BuildPrs(aShape) : 0;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void MeasureGUI_BndBoxDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  myObj = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    processObject();
    erasePreview();
    return;
  }

  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First());

  if (aSelectedObject->_is_nil()) {
    processObject();
    erasePreview();
    return;
  }

  myObj = aSelectedObject;
  myEditCurrentArgument->setText(GEOMBase::GetName(myObj));
  processObject();
  redisplayPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_BndBoxDlg::createOperation()
{
  return getGeomEngine()->GetIMeasureOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool MeasureGUI_BndBoxDlg::isValid (QString&)
{
  return !myObj->_is_nil();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool MeasureGUI_BndBoxDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->MakeBoundingBox(myObj);

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : redisplayPreview()
// purpose  :
//=================================================================================
void MeasureGUI_BndBoxDlg::redisplayPreview()
{
  QString aMess;
  if (!isValid(aMess)) {
    erasePreview(true);
    return;
  }

  erasePreview(false);

  try {
    SUIT_OverrideCursor();

    getDisplayer()->SetColor(Quantity_NOC_VIOLET);
    getDisplayer()->SetToActivate(false);
    
    if (SALOME_Prs* aPrs = buildPrs())
      displayPreview(aPrs);
  }
  catch (const SALOME::SALOME_Exception& e) {
    SalomeApp_Tools::QtCatchCorbaException(e);
  }
}
