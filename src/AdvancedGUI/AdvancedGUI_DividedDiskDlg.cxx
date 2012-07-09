//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

#include "AdvancedGUI_DividedDiskDlg.h"

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
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// Constructor
//=================================================================================
AdvancedGUI_DividedDiskDlg::AdvancedGUI_DividedDiskDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false)
{
  QPixmap imageOp  (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_DIVIDEDDISK_R_RATIO")));
  QPixmap imageSel (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_DIVIDEDDISK_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_DIVIDEDDISK"));
  mainFrame()->RadioButton1->setIcon(imageOp);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupParams      = new DlgRef_1Spin(centralWidget());
  GroupParams->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupParams->TextLabel1->setText(tr("GEOM_RADIUS"));
  
  GroupOrientation = new DlgRef_3Radio(centralWidget());
  GroupOrientation->GroupBox1->setTitle(tr("GEOM_ORIENTATION"));
  GroupOrientation->RadioButton1->setText(tr("GEOM_WPLANE_OXY"));
  GroupOrientation->RadioButton2->setText(tr("GEOM_WPLANE_OYZ"));
  GroupOrientation->RadioButton3->setText(tr("GEOM_WPLANE_OZX"));
  //@@ setup dialog box layout here @@//

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupParams);
  layout->addWidget(GroupOrientation);
  /***************************************************************/

  setHelpFileName("create_divideddisk_page.html");

  Init();
}

//=================================================================================
// Destructor
//=================================================================================
AdvancedGUI_DividedDiskDlg::~AdvancedGUI_DividedDiskDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void AdvancedGUI_DividedDiskDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  // min, max, step and decimals for spin boxes & initial values
  initSpinBox(GroupParams->SpinBox_DX, 0.00001, COORD_MAX, step, "length_precision" );
  GroupParams->SpinBox_DX->setValue(100);
  
  GroupOrientation->RadioButton1->setChecked(true);
  myOrientation = 1;

  // Signal/slot connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(myGeomGUI,     SIGNAL(SignalDefaultStepValueChanged(double)),
          this,          SLOT(SetDoubleSpinBoxStep(double)));

  connect(GroupParams->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  
  connect(GroupOrientation->RadioButton1, SIGNAL(clicked()), this, SLOT(RadioButtonClicked()));
  connect(GroupOrientation->RadioButton2, SIGNAL(clicked()), this, SLOT(RadioButtonClicked()));
  connect(GroupOrientation->RadioButton3, SIGNAL(clicked()), this, SLOT(RadioButtonClicked()));

  initName(tr("GEOM_DIVIDEDDISK"));
  
  resize(minimumSizeHint());
  displayPreview(true);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void AdvancedGUI_DividedDiskDlg::SetDoubleSpinBoxStep (double step)
{
  //@@ set double spin box step for all spin boxes here @@//
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void AdvancedGUI_DividedDiskDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool AdvancedGUI_DividedDiskDlg::ClickOnApply()
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
void AdvancedGUI_DividedDiskDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  displayPreview(true);
}

//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void AdvancedGUI_DividedDiskDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : RadioBittonClicked()
// purpose  : Radio button management
//=================================================================================
void AdvancedGUI_DividedDiskDlg::RadioButtonClicked()
{ 
  if (GroupOrientation->RadioButton1->isChecked())
    myOrientation = 1;
  else if (GroupOrientation->RadioButton2->isChecked())
    myOrientation = 2;
  else if (GroupOrientation->RadioButton3->isChecked())
    myOrientation = 3;
   
//   gp_Pnt theOrigin = gp::Origin();
//   gp_Dir DirZ = gp::DZ();
//   gp_Dir DirX = gp::DX();
//   gp_Dir DirY = gp::DY();
//   
//   if (GroupOrientation->RadioButton1->isChecked())
//     myWPlane = gp_Ax3(theOrigin, DirZ, DirX);
//   else if (GroupOrientation->RadioButton2->isChecked())
//     myWPlane = gp_Ax3(theOrigin, DirX, DirY);
//   else if (GroupOrientation->RadioButton3->isChecked())
//     myWPlane = gp_Ax3(theOrigin, DirY, DirZ);
  
  displayPreview(true);
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void AdvancedGUI_DividedDiskDlg::ValueChangedInSpinBox()
{
  //@@ connect custom spin boxes or other widget to this slot in the Init() method for automatic preview update @@//
  displayPreview(true);
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr AdvancedGUI_DividedDiskDlg::createOperation()
{
  return getGeomEngine()->GetIAdvancedOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool AdvancedGUI_DividedDiskDlg::isValid (QString& msg)
{
  bool ok = true;

  //@@ add custom validation actions here @@//

  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool AdvancedGUI_DividedDiskDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IAdvancedOperations_var anOper = GEOM::GEOM_IAdvancedOperations::_narrow(getOperation());

  //@@ retrieve input values from the widgets here @@//
  CORBA::Double theR = GroupParams->SpinBox_DX->value(); //@@ init parameter value from dialog box @@;
  CORBA::Double theRatio = 50; //@@ init parameter value from dialog box @@;

  // call engine function
  anObj = anOper->MakeDividedDisk(theR, theRatio, myOrientation);
  res = !anObj->_is_nil();
  if (res && !IsPreview())
  {
    QStringList aParameters;
    //@@ put stringified input parameters to the string list here to store in the data model for notebook @@//
    //aParameters << @@ stringified parameter value @@; // R parameter
    //aParameters << @@ stringified parameter value @@; // Ratio parameter
    if ( aParameters.count() > 0 ) anObj->SetParameters(aParameters.join(":").toLatin1().constData());
  }
  
  if (res)
    objects.push_back(anObj._retn());

  return res;
}
