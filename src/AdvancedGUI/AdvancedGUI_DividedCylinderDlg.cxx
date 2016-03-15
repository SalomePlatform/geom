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

#include "AdvancedGUI_DividedCylinderDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// OCCT Includes
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// Constructor
//=================================================================================
AdvancedGUI_DividedCylinderDlg::AdvancedGUI_DividedCylinderDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false)
{
  QPixmap imageOp  (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_DIVIDEDCYLINDER_R_H")));
  QPixmap imageSel (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_DIVIDEDCYLINDER_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_DIVIDEDCYLINDER"));
  mainFrame()->RadioButton1->setIcon(imageOp);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupParams = new DlgRef_2Spin(centralWidget());
  GroupParams->GroupBox1->setTitle(tr("GEOM_BOX_OBJ"));
  GroupParams->TextLabel1->setText(tr("GEOM_RADIUS"));
  GroupParams->TextLabel2->setText(tr("GEOM_HEIGHT"));
  
  GroupPattern = new DlgRef_3Radio(centralWidget());
  GroupPattern->GroupBox1->setTitle(tr("GEOM_PATTERN"));
  GroupPattern->RadioButton1->setText(tr("GEOM_SQUARE"));
  GroupPattern->RadioButton2->setText(tr("GEOM_HEXAGON"));
  GroupPattern->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  GroupPattern->RadioButton3->close();

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupParams);
  layout->addWidget(GroupPattern);
  /***************************************************************/

  setHelpFileName("create_dividedcylinder_page.html");

  Init();
}

//=================================================================================
// Destructor
//=================================================================================
AdvancedGUI_DividedCylinderDlg::~AdvancedGUI_DividedCylinderDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void AdvancedGUI_DividedCylinderDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  // min, max, step and decimals for spin boxes & initial values
  initSpinBox(GroupParams->SpinBox_DX, 0.00001, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupParams->SpinBox_DY, 0.00001, COORD_MAX, step, "length_precision" );
  
  // init variables
  double aRadius = 100;
  double aHeight = 300;
  GroupParams->SpinBox_DX->setValue(aRadius);
  GroupParams->SpinBox_DY->setValue(aHeight);
  
  GroupPattern->RadioButton1->setChecked(true);
  myPattern = GEOM::SQUARE;

  // Signal/slot connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(myGeomGUI,     SIGNAL(SignalDefaultStepValueChanged(double)),
          this,          SLOT(SetDoubleSpinBoxStep(double)));
  
  connect(GroupParams->SpinBox_DX,     SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupParams->SpinBox_DY,     SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  
  connect(GroupPattern->RadioButton1,  SIGNAL(clicked()),            this, SLOT(RadioButtonClicked()));
  connect(GroupPattern->RadioButton2,  SIGNAL(clicked()),            this, SLOT(RadioButtonClicked()));

  initName(tr("GEOM_DIVIDEDCYLINDER"));
  
  resize(minimumSizeHint());
  displayPreview(true);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void AdvancedGUI_DividedCylinderDlg::SetDoubleSpinBoxStep (double step)
{
  //@@ set double spin box step for all spin boxes here @@//
}

//=================================================================================
// function : RadioButtonClicked()
// purpose  : Radio button management
//=================================================================================
void AdvancedGUI_DividedCylinderDlg::RadioButtonClicked()
{ 
  QRadioButton* send = (QRadioButton*)sender();
  
  // Division pattern
  if (send == GroupPattern->RadioButton1)
    myPattern = GEOM::SQUARE;
  else if (send == GroupPattern->RadioButton2)
    myPattern = GEOM::HEXAGON;
  
  displayPreview(true);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void AdvancedGUI_DividedCylinderDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool AdvancedGUI_DividedCylinderDlg::ClickOnApply()
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
void AdvancedGUI_DividedCylinderDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  displayPreview(true);
}

//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void AdvancedGUI_DividedCylinderDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void AdvancedGUI_DividedCylinderDlg::ValueChangedInSpinBox()
{
  //@@ connect custom spin boxes or other widget to this slot in the Init() method for automatic preview update @@//
  displayPreview(true);
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr AdvancedGUI_DividedCylinderDlg::createOperation()
{
  return getGeomEngine()->GetPluginOperations(getStudyId(), "AdvancedEngine");
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool AdvancedGUI_DividedCylinderDlg::isValid (QString& msg)
{
  bool ok = true;

  //@@ add custom validation actions here @@//

  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool AdvancedGUI_DividedCylinderDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::IAdvancedOperations_var anOper = GEOM::IAdvancedOperations::_narrow(getOperation());

  //@@ retrieve input values from the widgets here @@//
  CORBA::Double theR = GroupParams->SpinBox_DX->value();
  CORBA::Double theH = GroupParams->SpinBox_DY->value();

  // call engine function
  anObj = anOper->MakeDividedCylinder(theR, theH, myPattern);
  res = !anObj->_is_nil();
  if (res && !IsPreview())
  {
    QStringList aParameters;
    aParameters << GroupParams->SpinBox_DX->text(); // R parameter
    aParameters << GroupParams->SpinBox_DY->text(); // H parameter
    if ( aParameters.count() > 0 ) anObj->SetParameters(aParameters.join(":").toLatin1().constData());
  }
  
  if (res)
    objects.push_back(anObj._retn());

  return res;
}
