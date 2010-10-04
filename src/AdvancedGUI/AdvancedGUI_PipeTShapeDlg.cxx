//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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
//

#include "AdvancedGUI_PipeTShapeDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_OverrideCursor.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Tools.h>
#include <LightApp_SelectionMgr.h>

// OCCT Includes
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <gp_Pnt.hxx>
#include <BRep_Tool.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// Constructor
//=================================================================================
AdvancedGUI_PipeTShapeDlg::AdvancedGUI_PipeTShapeDlg(GeometryGUI* theGeometryGUI, QWidget* parent) :
	GEOMBase_Skeleton(theGeometryGUI, parent, false) {
	QPixmap imageOp(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PIPETSHAPE")));
	QPixmap imageSel(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));
	imageImp = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICO_PIPETSHAPE_IMPORT"));
	imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE"));

	setWindowTitle(tr("GEOM_PIPE_TSHAPE_TITLE"));

	/***************************************************************/
	mainFrame()->GroupConstructors->setTitle(tr("GEOM_PIPE_TSHAPE"));
	mainFrame()->RadioButton1->setIcon(imageOp);
	mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
	mainFrame()->RadioButton2->close();
	mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
	mainFrame()->RadioButton3->close();

	QGridLayout* myMainLayout = new QGridLayout(centralWidget());
	myMainLayout->setMargin(0);
	myMainLayout->setSpacing(6);

    tshapeScreenShotLabel = new QLabel();
    tshapeScreenShotLabel->setSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Expanding);
    tshapeScreenShotLabel->setAlignment(Qt::AlignCenter);
    tshapeScreenShotLabel->setMinimumSize(100, 100);

	MainTubeGroupParams = new DlgRef_3Spin();
	MainTubeGroupParams->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_MPIPE"));
	MainTubeGroupParams->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_R"));
	MainTubeGroupParams->TextLabel2->setText(tr("GEOM_PIPE_TSHAPE_W"));
	MainTubeGroupParams->TextLabel3->setText(tr("GEOM_PIPE_TSHAPE_L"));

	IncidentTubeGroupParams = new DlgRef_3Spin();
	IncidentTubeGroupParams->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_IPIPE"));
	IncidentTubeGroupParams->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_R"));
	IncidentTubeGroupParams->TextLabel2->setText(tr("GEOM_PIPE_TSHAPE_W"));
	IncidentTubeGroupParams->TextLabel3->setText(tr("GEOM_PIPE_TSHAPE_L"));

	ChamferGroupParams = new DlgRef_2Spin();
	ChamferGroupParams->GroupBox1->setCheckable(true);
	ChamferGroupParams->GroupBox1->setChecked(false);
	ChamferGroupParams->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_CHAMFER"));
	ChamferGroupParams->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_CHAMFER_H"));
	ChamferGroupParams->TextLabel2->setText(tr("GEOM_PIPE_TSHAPE_CHAMFER_W"));

	FilletGroupParams = new DlgRef_1Spin();
	FilletGroupParams->GroupBox1->setCheckable(true);
	FilletGroupParams->GroupBox1->setChecked(false);
	FilletGroupParams->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_FILLET"));
	FilletGroupParams->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_R"));

	HexMeshCheckBox = new QCheckBox();
	HexMeshCheckBox->setText(tr("GEOM_PIPE_TSHAPE_HEX"));
	HexMeshCheckBox->setChecked(true);

	JunctionPointsSel = new DlgRef_6Sel();
	JunctionPointsSel->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_POSITION"));
	JunctionPointsSel->GroupBox1->setCheckable(true);
	JunctionPointsSel->GroupBox1->setChecked(false);
	JunctionPointsSel->PushButton1->setIcon(imageSel);
	JunctionPointsSel->LineEdit1->setReadOnly(true);
	JunctionPointsSel->LineEdit1->setText("");
	JunctionPointsSel->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_POSITION_P1"));

	JunctionPointsSel->PushButton2->setIcon(imageSel);
	JunctionPointsSel->LineEdit2->setReadOnly(true);
	JunctionPointsSel->LineEdit2->setText("");
	JunctionPointsSel->TextLabel2->setText(tr("GEOM_PIPE_TSHAPE_POSITION_P2"));

	JunctionPointsSel->PushButton3->setIcon(imageSel);
	JunctionPointsSel->LineEdit3->setReadOnly(true);
	JunctionPointsSel->LineEdit3->setText("");
	JunctionPointsSel->TextLabel3->setText(tr("GEOM_PIPE_TSHAPE_POSITION_P3"));

    JunctionPointsSel->PushButton4->setIcon(imageImp);
    JunctionPointsSel->LineEdit4->setReadOnly(true);
    JunctionPointsSel->LineEdit4->setText("");
    JunctionPointsSel->TextLabel4->setText(tr("GEOM_PIPE_TSHAPE_POSITION_LBL_L1"));

    JunctionPointsSel->PushButton5->setIcon(imageImp);
    JunctionPointsSel->LineEdit5->setReadOnly(true);
    JunctionPointsSel->LineEdit5->setText("");
    JunctionPointsSel->TextLabel5->setText(tr("GEOM_PIPE_TSHAPE_POSITION_LBL_L2"));

    JunctionPointsSel->PushButton6->setAttribute(Qt::WA_DeleteOnClose);
    JunctionPointsSel->PushButton6->close();
    JunctionPointsSel->LineEdit6->setAttribute(Qt::WA_DeleteOnClose);
    JunctionPointsSel->LineEdit6->close();
    JunctionPointsSel->TextLabel6->setAttribute(Qt::WA_DeleteOnClose);
    JunctionPointsSel->TextLabel6->close();

	// 1st row, height = 1, colspan = 3
	int rowPict = 0, colPict = 0, rowspanPict = 1, colspanPict = 3;
	// 2nd row, height = 4, col 1
	int rowMain = rowspanPict,                           colMain = 0,      rowspanMain = 2,      colspanMain = 1;
	int rowCham = rowspanPict + rowspanMain,             colCham = 0,      rowspanCham = 2,      colspanCham = 1;
	// 2nd row, height = 4, col 2
	int rowInc = rowspanPict,                            colInc  = 1,      rowspanInc = 2,       colspanInc = 1;
	int rowFill = rowspanPict + rowspanInc,              colFill = 1,      rowspanFill = 1,      colspanFill = 1;
	int rowHex = rowspanPict + rowspanInc + rowspanFill, colHex  = 1,      rowspanHex = 1,       colspanHex = 1;
	// 2nd row, height = 4, col 3
	int rowNewPosVal = rowspanPict,                      colNewPosVal = 2, rowspanNewPosVal = 4, colspanNewPosVal = 1;

    myMainLayout->addWidget(tshapeScreenShotLabel, rowPict, colPict, rowspanPict, colspanPict);

	myMainLayout->addWidget(MainTubeGroupParams, rowMain, colMain, rowspanMain, colspanMain);
	myMainLayout->addWidget(FilletGroupParams, rowFill, colFill, rowspanFill, colspanFill);
	myMainLayout->addWidget(HexMeshCheckBox, rowHex, colHex, rowspanHex, colspanHex);

	myMainLayout->addWidget(IncidentTubeGroupParams, rowInc, colInc, rowspanInc, colspanInc);
	myMainLayout->addWidget(ChamferGroupParams, rowCham, colCham, rowspanCham, colspanCham);

	myMainLayout->addWidget(JunctionPointsSel, rowNewPosVal, colNewPosVal, rowspanNewPosVal, colspanNewPosVal);
	/***************************************************************/

	setHelpFileName("create_pipetshape_page.html");

	Init();
}

//=================================================================================
// Destructor
//=================================================================================
AdvancedGUI_PipeTShapeDlg::~AdvancedGUI_PipeTShapeDlg() {
	// no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::Init() {
	// Get setting of step value from file configuration
	SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
	double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

	myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();
	myOkPoint1 = myOkPoint2 = myOkPoint3 = false;

	pipeTShapeGroupObjects.clear();

	// min, max, step and decimals for spin boxes
	initSpinBox(MainTubeGroupParams->SpinBox_DX, Precision::Approximation(), COORD_MAX, step, "length_precision");
	initSpinBox(MainTubeGroupParams->SpinBox_DY, Precision::Approximation(), COORD_MAX, step, "length_precision");
	initSpinBox(MainTubeGroupParams->SpinBox_DZ, Precision::Approximation(), COORD_MAX, step, "length_precision");
	initSpinBox(IncidentTubeGroupParams->SpinBox_DX, Precision::Approximation(), COORD_MAX, step, "length_precision");
	initSpinBox(IncidentTubeGroupParams->SpinBox_DY, Precision::Approximation(), COORD_MAX, step, "length_precision");
	initSpinBox(IncidentTubeGroupParams->SpinBox_DZ, Precision::Approximation(), COORD_MAX, step, "length_precision");
	initSpinBox(ChamferGroupParams->SpinBox_DX, Precision::Approximation(), COORD_MAX, step, "length_precision");
	initSpinBox(ChamferGroupParams->SpinBox_DY, Precision::Approximation(), COORD_MAX, step, "length_precision");
	initSpinBox(FilletGroupParams->SpinBox_DX, Precision::Approximation(), COORD_MAX, step, "length_precision");

	// init variables
	MainTubeGroupParams->SpinBox_DX->setValue(80);
	MainTubeGroupParams->SpinBox_DY->setValue(20);
	MainTubeGroupParams->SpinBox_DZ->setValue(200);
	IncidentTubeGroupParams->SpinBox_DX->setValue(50);
	IncidentTubeGroupParams->SpinBox_DY->setValue(20);
	IncidentTubeGroupParams->SpinBox_DZ->setValue(200);
	ChamferGroupParams->SpinBox_DX->setValue(20);
	ChamferGroupParams->SpinBox_DY->setValue(10);
	FilletGroupParams->SpinBox_DX->setValue(20);

    CssNormal = QString("QDoubleSpinBox {");
    CssNormal.append(MainTubeGroupParams->SpinBox_DZ->styleSheet());
    CssNormal.append("}");
    CssNormal.append("\nQPushButton {");
    CssNormal.append(JunctionPointsSel->PushButton4->styleSheet());
    CssNormal.append("}");
    CssAcceptable = "QDoubleSpinBox, QPushButton {background-color: rgb(85, 170, 127)}";
    CssRefused = "QDoubleSpinBox, QPushButton {background-color: rgb(255, 0, 0)}";

	// Signal/slot connections
	connect(buttonOk(), SIGNAL(clicked()), this, SLOT(ClickOnOk()));
	connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));
	connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));
	// ValueChangedInSpinBox
	connect(MainTubeGroupParams->SpinBox_DX, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
	connect(MainTubeGroupParams->SpinBox_DY, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
	connect(MainTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
	connect(IncidentTubeGroupParams->SpinBox_DX, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
	connect(IncidentTubeGroupParams->SpinBox_DY, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
	connect(IncidentTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
	connect(ChamferGroupParams->SpinBox_DX, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
	connect(ChamferGroupParams->SpinBox_DY, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
	connect(FilletGroupParams->SpinBox_DX, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
	// ChamferOrFillet
	connect(ChamferGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(ChamferOrFillet(bool)));
	connect(FilletGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(ChamferOrFillet(bool)));
	// Preview
//	connect(PreviewPushButton, SIGNAL(clicked()), this, SLOT(DisplayPreview()));
	// Position
	connect(JunctionPointsSel->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(SetPosition(bool)));
	connect(JunctionPointsSel->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
	connect(JunctionPointsSel->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
	connect(JunctionPointsSel->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
    // ApplyNewDimensions
    connect(JunctionPointsSel->PushButton4, SIGNAL(clicked()), this, SLOT(ApplyNewDimensions()));
    connect(JunctionPointsSel->PushButton5, SIGNAL(clicked()), this, SLOT(ApplyNewDimensions()));
	connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(UpdatePicture(QWidget*, QWidget*)));
	//@@ put additional signal/slot connections here @@//

	initName(tr("GEOM_PIPE_TSHAPE"));
    updateTshapeScreenshotLabel();
	DisplayPreview();
}

//=================================================================================
// function : ApplyNewDimensions()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::ApplyNewDimensions() {
    QPushButton* send = (QPushButton*) sender();

    bool ok = false;
    double newVal;
    if (send == JunctionPointsSel->PushButton4) {
        if (!JunctionPointsSel->LineEdit4->text().isEmpty()) {
            newVal = JunctionPointsSel->LineEdit4->text().toDouble(&ok);
            if (ok) {
                disconnect(MainTubeGroupParams->SpinBox_DZ, 0, this, 0);
                MainTubeGroupParams->SpinBox_DZ->setValue(newVal);
                connect(MainTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
                MainTubeGroupParams->SpinBox_DZ->setToolTip("");
                MainTubeGroupParams->SpinBox_DZ->setStyleSheet("background-color: rgb(255, 255, 255);");
                CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
                DisplayPreview();
            }
        }
    }
    else if (send == JunctionPointsSel->PushButton5) {
        if (!JunctionPointsSel->LineEdit5->text().isEmpty()) {
            newVal = JunctionPointsSel->LineEdit5->text().toDouble(&ok);
            if (ok) {
                disconnect(IncidentTubeGroupParams->SpinBox_DZ, 0, this, 0);
                IncidentTubeGroupParams->SpinBox_DZ->setValue(newVal);
                connect(IncidentTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
                IncidentTubeGroupParams->SpinBox_DZ->setToolTip("");
                IncidentTubeGroupParams->SpinBox_DZ->setStyleSheet("background-color: rgb(255, 255, 255);");
                CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
                DisplayPreview();
            }
        }
    }
}

//=================================================================================
// function : UpdatePicture()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::UpdatePicture(QWidget* old, QWidget* now) {

	if (ChamferGroupParams->GroupBox1->isChecked())
		if (now == MainTubeGroupParams->SpinBox_DX)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_R1"));
		else if (now == MainTubeGroupParams->SpinBox_DY)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_W1"));
		else if (now == MainTubeGroupParams->SpinBox_DZ)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_L1"));
		else if (now == IncidentTubeGroupParams->SpinBox_DX)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_R2"));
		else if (now == IncidentTubeGroupParams->SpinBox_DY)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_W2"));
		else if (now == IncidentTubeGroupParams->SpinBox_DZ)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_L2"));
		else if (now == ChamferGroupParams->SpinBox_DX)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_H"));
		else if (now == ChamferGroupParams->SpinBox_DY)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_W"));
		else
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER"));
	else if (FilletGroupParams->GroupBox1->isChecked())
		if (now == MainTubeGroupParams->SpinBox_DX)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_R1"));
		else if (now == MainTubeGroupParams->SpinBox_DY)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_W1"));
		else if (now == MainTubeGroupParams->SpinBox_DZ)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_L1"));
		else if (now == IncidentTubeGroupParams->SpinBox_DX)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_R2"));
		else if (now == IncidentTubeGroupParams->SpinBox_DY)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_W2"));
		else if (now == IncidentTubeGroupParams->SpinBox_DZ)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_L2"));
		else if (now == FilletGroupParams->SpinBox_DX)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_RF"));
		else
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET"));
	else
		if (now == MainTubeGroupParams->SpinBox_DX)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_R1"));
		else if (now == MainTubeGroupParams->SpinBox_DY)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_W1"));
		else if (now == MainTubeGroupParams->SpinBox_DZ)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_L1"));
		else if (now == IncidentTubeGroupParams->SpinBox_DX)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_R2"));
		else if (now == IncidentTubeGroupParams->SpinBox_DY)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_W2"));
		else if (now == IncidentTubeGroupParams->SpinBox_DZ)
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_L2"));
		else
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE"));

    updateTshapeScreenshotLabel();
}

//=================================================================================
// function : SetPosition()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::SetPosition(bool isChecked) {
	if (isChecked) {
		erasePreview();
        JunctionPointsSel->LineEdit4->setText("");
        JunctionPointsSel->LineEdit5->setText("");
// 		connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(
// 				SelectionIntoArgument()));
		JunctionPointsSel->PushButton1->click();
		SelectionIntoArgument();
	} else {
		disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
		JunctionPointsSel->LineEdit4->setText("");
		JunctionPointsSel->LineEdit5->setText("");
		DisplayPreview();
	}
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::ValueChangedInSpinBox(double newValue)
{
    if (JunctionPointsSel->GroupBox1->isChecked() && myOkPoint1 && myOkPoint2 && myOkPoint3)
        CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
    DisplayPreview();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::SelectionIntoArgument() {

	erasePreview();
//	myEditCurrentArgument->setText("");

	LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
	SALOME_ListIO aSelList;
	aSelMgr->selectedObjects(aSelList);

	if (aSelList.Extent() != 1) {
		if (myEditCurrentArgument == JunctionPointsSel->LineEdit1)
			myOkPoint1 = false;
		else if (myEditCurrentArgument == JunctionPointsSel->LineEdit2)
			myOkPoint2 = false;
		else if (myEditCurrentArgument == JunctionPointsSel->LineEdit3)
			myOkPoint3 = false;
		return;
		myEditCurrentArgument->setText("");
	}

	// nbSel == 1
	if (aSelList.Extent() == 1) {
		Standard_Boolean aRes = Standard_False;
		Handle(SALOME_InteractiveObject) anIO = aSelList.First();
		GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(anIO, aRes);
		if (!CORBA::is_nil(aSelectedObject) && aRes) {
			QString aName = GEOMBase::GetName(aSelectedObject);
			TopoDS_Shape aShape;
			if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull()) {

				TColStd_IndexedMapOfInteger aMap;
				aSelMgr->GetIndexes(anIO, aMap);
				if (aMap.Extent() == 1) { // Local Selection
					int anIndex = aMap(1);
					aName += QString(":vertex_%1").arg(anIndex);

					//Find SubShape Object in Father
					GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather(aSelectedObject, aName);

					if (aFindedObject->_is_nil()) { // Object not found in study
						GEOM::GEOM_IShapesOperations_var aShapesOp =
								getGeomEngine()->GetIShapesOperations(getStudyId());
						aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
					} else {
						aSelectedObject = aFindedObject; // get Object from study
					}
					GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE);
				} else { // Global Selection
					if (aShape.ShapeType() != TopAbs_VERTEX) {
						aSelectedObject = GEOM::GEOM_Object::_nil();
						aName = "";
					}
				}
			}

			if (aShape.IsNull() || aShape.ShapeType() != TopAbs_VERTEX) {
				return;
			}
			myEditCurrentArgument->setText(aName);
			if (myEditCurrentArgument == JunctionPointsSel->LineEdit1) {
				myPoint1 = aSelectedObject;
				myOkPoint1 = true;
				if (!myOkPoint2)
					JunctionPointsSel->PushButton2->click();
			} else if (myEditCurrentArgument == JunctionPointsSel->LineEdit2) {
				myPoint2 = aSelectedObject;
				myOkPoint2 = true;
				if (!myOkPoint3)
					JunctionPointsSel->PushButton3->click();
			} else if (myEditCurrentArgument == JunctionPointsSel->LineEdit3) {
				myPoint3 = aSelectedObject;
				myOkPoint3 = true;
				if (!myOkPoint1)
					JunctionPointsSel->PushButton1->click();
			}
		}
	}

	if (myOkPoint1 && myOkPoint2 && myOkPoint3) {
        CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
		DisplayPreview();
    }

//	Standard_Boolean testResult = Standard_False;
//	GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);
//	if (!testResult || aSelectedObject->_is_nil())
//		return;
//
//	myEditCurrentArgument->setText(GEOMBase::GetName(aSelectedObject));
//
//	// clear selection
//	disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
//	myGeomGUI->getApp()->selectionMgr()->clearSelected();
//	connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
//
//	if (myEditCurrentArgument == JunctionPointsSel->LineEdit1) {
//		myPoint1 = aSelectedObject;
//		myOkPoint1 = true;
//		if (!myOkPoint2)
//			JunctionPointsSel->PushButton2->click();
//		//			P2GroupParams->PushButton1->click();
//	} else if (myEditCurrentArgument == JunctionPointsSel->LineEdit2) {
//		//	} else if (myEditCurrentArgument == P2GroupParams->LineEdit1) {
//		myPoint2 = aSelectedObject;
//		myOkPoint2 = true;
//		if (!myOkPoint3)
//			JunctionPointsSel->PushButton3->click();
//		//			P3GroupParams->PushButton1->click();
//	} else if (myEditCurrentArgument == JunctionPointsSel->LineEdit3) {
//		//	} else if (myEditCurrentArgument == P3GroupParams->LineEdit1) {
//		myPoint3 = aSelectedObject;
//		myOkPoint3 = true;
//		if (!myOkPoint1)
//			JunctionPointsSel->PushButton1->click();
//	}
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::SetEditCurrentArgument() {
	QPushButton* send = (QPushButton*) sender();

	if (send == JunctionPointsSel->PushButton1) {
		myEditCurrentArgument = JunctionPointsSel->LineEdit1;
		JunctionPointsSel->PushButton2->setDown(false);
		JunctionPointsSel->LineEdit2->setEnabled(false);
		JunctionPointsSel->PushButton3->setDown(false);
		JunctionPointsSel->LineEdit3->setEnabled(false);
	} else if (send == JunctionPointsSel->PushButton2) {
		myEditCurrentArgument = JunctionPointsSel->LineEdit2;
		JunctionPointsSel->PushButton1->setDown(false);
		JunctionPointsSel->LineEdit1->setEnabled(false);
		JunctionPointsSel->PushButton3->setDown(false);
		JunctionPointsSel->LineEdit3->setEnabled(false);
	} else if (send == JunctionPointsSel->PushButton3) {
		myEditCurrentArgument = JunctionPointsSel->LineEdit3;
		JunctionPointsSel->PushButton1->setDown(false);
		JunctionPointsSel->LineEdit1->setEnabled(false);
		JunctionPointsSel->PushButton2->setDown(false);
		JunctionPointsSel->LineEdit2->setEnabled(false);
	}

	// enable line edit
	myEditCurrentArgument->setEnabled(true);
	myEditCurrentArgument->setFocus();
	myEditCurrentArgument->setText("");
	// after setFocus(), because it will be setDown(false) when loses focus
	send->setDown(true);

    JunctionPointsSel->LineEdit4->setText("");
    JunctionPointsSel->LineEdit5->setText("");

	disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
//	globalSelection(GEOM_POINT);
    globalSelection(); // close local contexts, if any
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
	connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::SetDoubleSpinBoxStep(double step) {
	MainTubeGroupParams->SpinBox_DX->setSingleStep(step);
	MainTubeGroupParams->SpinBox_DY->setSingleStep(step);
	MainTubeGroupParams->SpinBox_DZ->setSingleStep(step);
	IncidentTubeGroupParams->SpinBox_DX->setSingleStep(step);
	IncidentTubeGroupParams->SpinBox_DY->setSingleStep(step);
	IncidentTubeGroupParams->SpinBox_DZ->setSingleStep(step);
	ChamferGroupParams->SpinBox_DX->setSingleStep(step);
	ChamferGroupParams->SpinBox_DY->setSingleStep(step);
	FilletGroupParams->SpinBox_DX->setSingleStep(step);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::ClickOnOk() {
	if (ClickOnApply())
		ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool AdvancedGUI_PipeTShapeDlg::ClickOnApply() {
	if (!onAccept())
		return false;

	initName();

	return true;
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::ActivateThisDialog() {
	GEOMBase_Skeleton::ActivateThisDialog();
	//	globalSelection( GEOM_POINT);
	//	connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(
	//			currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
    if (myOkPoint1 && myOkPoint2 && myOkPoint3)
        CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
	DisplayPreview();
}

//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::enterEvent(QEvent*) {
	if (!mainFrame()->GroupConstructors->isEnabled())
		ActivateThisDialog();
}

//=================================================================================
// function : resizeEvent [REDEFINED]
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::resizeEvent(QResizeEvent */*event*/) {
    QSize scaledSize = imagePipeTShape.size();
    scaledSize.scale(tshapeScreenShotLabel->size(), Qt::KeepAspectRatio);
    if (!tshapeScreenShotLabel->pixmap()
      || scaledSize != tshapeScreenShotLabel->pixmap()->size())
        updateTshapeScreenshotLabel();
}

//=================================================================================
// function : updateTshapeScreenshotLabel
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::updateTshapeScreenshotLabel() {
    tshapeScreenShotLabel->setPixmap(imagePipeTShape.scaled(tshapeScreenShotLabel->size(),
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation));
}

//=================================================================================
// function : ChamferOrFillet()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::ChamferOrFillet(bool) {
	QGroupBox* send = (QGroupBox*) sender();

	if (send == ChamferGroupParams->GroupBox1) {
		if (send->isChecked()) {
			disconnect(FilletGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, 0);
			FilletGroupParams->GroupBox1->setChecked(false);
			connect(FilletGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(ChamferOrFillet(bool)));
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER"));
		}
		else
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE"));
        updateTshapeScreenshotLabel();
        if (myOkPoint1 && myOkPoint2 && myOkPoint3)
            CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
		DisplayPreview();
	} else if (send == FilletGroupParams->GroupBox1) {
		if (send->isChecked()) {
			disconnect(ChamferGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, 0);
			ChamferGroupParams->GroupBox1->setChecked(!send->isChecked());
			connect(ChamferGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(ChamferOrFillet(bool)));
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET"));
		}
		else
			imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE"));
        updateTshapeScreenshotLabel();
        if (myOkPoint1 && myOkPoint2 && myOkPoint3)
            CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
		DisplayPreview();
	}

}

//=================================================================================
// function : DisplayPreview()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::DisplayPreview(const bool activate, const bool update, const bool toRemoveFromEngine,
		const double lineWidth, const int displayMode, const int color) {
	isPreview = true;
	QString msg;
	if (!isValid(msg)) {
		erasePreview(update);
		isPreview = false;
		return;
	}

	erasePreview(false);

	try {
		SUIT_OverrideCursor wc;
		ObjectList objects;
		bool hexMeshState = HexMeshCheckBox->isChecked();
		HexMeshCheckBox->setChecked(false);
		if (!executeNoCheck(objects) || !getOperation()->IsDone()) {
			wc.suspend();
		} else {
			ObjectList::iterator it = objects.begin();
			GEOM::GEOM_Object_var obj = *it;
			displayPreview(obj, true, activate, false, lineWidth, displayMode, color);
			if (toRemoveFromEngine)
				obj->Destroy();
		}
		HexMeshCheckBox->setChecked(hexMeshState);
	} catch (const SALOME::SALOME_Exception& e) {
		SalomeApp_Tools::QtCatchCorbaException(e);
	}

	isPreview = false;

	if (update)
		updateViewer();
}
//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr AdvancedGUI_PipeTShapeDlg::createOperation() {
	return getGeomEngine()->GetIAdvancedOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool AdvancedGUI_PipeTShapeDlg::isValid(QString& msg) {
	bool ok = true;

	ok = MainTubeGroupParams->SpinBox_DX->isValid(msg, !IsPreview()) && ok;
	ok = MainTubeGroupParams->SpinBox_DY->isValid(msg, !IsPreview()) && ok;
	ok = MainTubeGroupParams->SpinBox_DZ->isValid(msg, !IsPreview()) && ok;
	ok = IncidentTubeGroupParams->SpinBox_DX->isValid(msg, !IsPreview()) && ok;
	ok = IncidentTubeGroupParams->SpinBox_DY->isValid(msg, !IsPreview()) && ok;
	ok = IncidentTubeGroupParams->SpinBox_DZ->isValid(msg, !IsPreview()) && ok;
	ok = ChamferGroupParams->SpinBox_DX->isValid(msg, !IsPreview()) && ok;
	ok = ChamferGroupParams->SpinBox_DY->isValid(msg, !IsPreview()) && ok;
	ok = FilletGroupParams->SpinBox_DX->isValid(msg, !IsPreview()) && ok;

	ok = fabs(MainTubeGroupParams->SpinBox_DX->value()) > Precision::Confusion() && ok;
	ok = fabs(MainTubeGroupParams->SpinBox_DY->value()) > Precision::Confusion() && ok;
	ok = fabs(MainTubeGroupParams->SpinBox_DZ->value()) > Precision::Confusion() && ok;
	ok = fabs(IncidentTubeGroupParams->SpinBox_DX->value()) > Precision::Confusion() && ok;
	ok = fabs(IncidentTubeGroupParams->SpinBox_DY->value()) > Precision::Confusion() && ok;
	ok = fabs(IncidentTubeGroupParams->SpinBox_DZ->value()) > Precision::Confusion() && ok;
	ok = fabs(ChamferGroupParams->SpinBox_DX->value()) > Precision::Confusion() && ok;
	ok = fabs(ChamferGroupParams->SpinBox_DY->value()) > Precision::Confusion() && ok;
	ok = fabs(FilletGroupParams->SpinBox_DX->value()) > Precision::Confusion() && ok;

	if (JunctionPointsSel->GroupBox1->isChecked())
		ok = myOkPoint1 && myOkPoint2 && myOkPoint3 && ok;

	return ok;
}

//=================================================================================
// function : CheckCompatiblePosition()
// purpose  :
//=================================================================================
bool AdvancedGUI_PipeTShapeDlg::CheckCompatiblePosition(GEOM::GEOM_Object_var theP1,
		GEOM::GEOM_Object_var theP2, GEOM::GEOM_Object_var theP3, double theTolerance) {

    MainTubeGroupParams->SpinBox_DZ->setStyleSheet(CssNormal);
    IncidentTubeGroupParams->SpinBox_DZ->setStyleSheet(CssNormal);
    JunctionPointsSel->PushButton4->setStyleSheet(CssNormal);
    JunctionPointsSel->PushButton5->setStyleSheet(CssNormal);

    CORBA::Double theL1 = MainTubeGroupParams->SpinBox_DZ->value();
    CORBA::Double theL2 = IncidentTubeGroupParams->SpinBox_DZ->value();

    JunctionPointsSel->LineEdit4->setText("");
    JunctionPointsSel->LineEdit5->setText("");

    MainTubeGroupParams->SpinBox_DZ->setToolTip("");
    IncidentTubeGroupParams->SpinBox_DZ->setToolTip("");

	TopoDS_Shape aShape;
	gp_Pnt P1, P2, P3;
	if ( GEOMBase::GetShape( theP1, aShape ) && !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
		P1 = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
	else
		return false;

	if ( GEOMBase::GetShape( theP2, aShape ) && !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
		P2 = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
	else
		return false;

	if ( GEOMBase::GetShape( theP3, aShape ) && !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
		P3 = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
	else
		return false;

    double d12 = P1.Distance(P2);
    double d13 = P1.Distance(P3);
    double d23 = P2.Distance(P3);

    if (Abs(d12) <= Precision::Confusion()) {
//         SetErrorCode("Junctions points P1 and P2 are identical");
        return false;
    }
    if (Abs(d13) <= Precision::Confusion()) {
//         SetErrorCode("Junctions points P1 and P3 are identical");
        return false;
    }
    if (Abs(d23) <= Precision::Confusion()) {
//         SetErrorCode("Junctions points P2 and P3 are identical");
        return false;
    }

    long double newL1 = 0.5 * d12;
    long double newL2 = sqrt(pow(d13,2)-pow(newL1,2));

    JunctionPointsSel->LineEdit4->setText(QString::number(newL1,'f',7));
    JunctionPointsSel->LineEdit5->setText(QString::number(newL2,'f',7));

    if (fabs(newL1 - theL1) > Precision::Approximation()) {
		if ((newL1 * (1 - theTolerance) - theL1 <= Precision::Approximation()) &&
				(newL1 * (1 + theTolerance) - theL1 >= Precision::Approximation())) {
            disconnect(MainTubeGroupParams->SpinBox_DZ, 0, this, 0);
			MainTubeGroupParams->SpinBox_DZ->setValue(newL1);
            connect(MainTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
			MainTubeGroupParams->SpinBox_DZ->setToolTip("Value was recomputed to fit with position");
            MainTubeGroupParams->SpinBox_DZ->setStyleSheet(CssAcceptable);
            JunctionPointsSel->PushButton4->setStyleSheet(CssAcceptable);
		}
		else {
			MainTubeGroupParams->SpinBox_DZ->setToolTip("Value is incompatible with position");
            MainTubeGroupParams->SpinBox_DZ->setStyleSheet(CssRefused);
            JunctionPointsSel->PushButton4->setStyleSheet(CssRefused);
		}
	}
    else {
        MainTubeGroupParams->SpinBox_DZ->setStyleSheet(CssNormal);
        JunctionPointsSel->PushButton4->setStyleSheet(CssNormal);
    }

	if (fabs(newL2 - theL2) > Precision::Approximation()) {
		if ((newL2 * (1 - theTolerance) - theL2 <= Precision::Approximation()) &&
				(newL2 * (1 + theTolerance) - theL2 >= Precision::Approximation())) {
            disconnect(IncidentTubeGroupParams->SpinBox_DZ, 0, this, 0);
			IncidentTubeGroupParams->SpinBox_DZ->setValue(newL2);
            connect(IncidentTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
			IncidentTubeGroupParams->SpinBox_DZ->setToolTip("Value was recomputed to fit with position");
            IncidentTubeGroupParams->SpinBox_DZ->setStyleSheet(CssAcceptable);
            JunctionPointsSel->PushButton5->setStyleSheet(CssAcceptable);
		}
		else {
			IncidentTubeGroupParams->SpinBox_DZ->setToolTip("Value is incompatible with position");
            IncidentTubeGroupParams->SpinBox_DZ->setStyleSheet(CssRefused);
            JunctionPointsSel->PushButton5->setStyleSheet(CssRefused);
		}
	}
	else {
        IncidentTubeGroupParams->SpinBox_DZ->setStyleSheet(CssNormal);
        JunctionPointsSel->PushButton5->setStyleSheet(CssNormal);
    }

	return true;
}

bool AdvancedGUI_PipeTShapeDlg::execute(ObjectList& objects) {

    if (JunctionPointsSel->GroupBox1->isChecked() && myOkPoint1 && myOkPoint2 && myOkPoint3)
        CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);

    return executeNoCheck(objects);
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool AdvancedGUI_PipeTShapeDlg::executeNoCheck(ObjectList& objects) {
	bool res = false;

	//   GEOM::GEOM_Object_var anObj;
	GEOM::ListOfGO_var anObj;

	GEOM::GEOM_IAdvancedOperations_var anOper = GEOM::GEOM_IAdvancedOperations::_narrow(getOperation());

	//@@ retrieve input values from the widgets here @@//
	CORBA::Double theR1 = MainTubeGroupParams->SpinBox_DX->value();
	CORBA::Double theW1 = MainTubeGroupParams->SpinBox_DY->value();
	CORBA::Double theL1 = MainTubeGroupParams->SpinBox_DZ->value();
	CORBA::Double theR2 = IncidentTubeGroupParams->SpinBox_DX->value();
	CORBA::Double theW2 = IncidentTubeGroupParams->SpinBox_DY->value();
	CORBA::Double theL2 = IncidentTubeGroupParams->SpinBox_DZ->value();
	CORBA::Double theH = ChamferGroupParams->SpinBox_DX->value();
	CORBA::Double theW = ChamferGroupParams->SpinBox_DY->value();
	CORBA::Double theRF = FilletGroupParams->SpinBox_DX->value();
	CORBA::Boolean theHexMesh = HexMeshCheckBox->isChecked();

// 	if (JunctionPointsSel->GroupBox1->isChecked()) {
// 		CheckCompatiblePosition(theL1, theL2, myPoint1, myPoint2, myPoint3, 0.01);
// 		theL1 = MainTubeGroupParams->SpinBox_DZ->value();
// 		theL2 = IncidentTubeGroupParams->SpinBox_DZ->value();
// 	}

	// call engine function
	if (ChamferGroupParams->GroupBox1->isChecked()) {
		if (JunctionPointsSel->GroupBox1->isChecked())
			anObj = anOper->MakePipeTShapeChamferWithPosition(theR1, theW1, theL1, theR2, theW2, theL2, theH, theW,
					theHexMesh, myPoint1, myPoint2, myPoint3);
		else
			anObj = anOper->MakePipeTShapeChamfer(theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, theHexMesh);
	}
	else if (FilletGroupParams->GroupBox1->isChecked()) {
		if (JunctionPointsSel->GroupBox1->isChecked())
			anObj = anOper->MakePipeTShapeFilletWithPosition(theR1, theW1, theL1, theR2, theW2, theL2, theRF,
					theHexMesh, myPoint1, myPoint2, myPoint3);
		else
			anObj = anOper->MakePipeTShapeFillet(theR1, theW1, theL1, theR2, theW2, theL2, theRF, theHexMesh);
	}
	else {
		if (JunctionPointsSel->GroupBox1->isChecked())
		anObj = anOper->MakePipeTShapeWithPosition(theR1, theW1, theL1, theR2, theW2, theL2, theHexMesh, myPoint1,
				myPoint2, myPoint3);
		else
			anObj = anOper->MakePipeTShape(theR1, theW1, theL1, theR2, theW2, theL2, theHexMesh);
	}

	res = anObj->length();
	if (!res)
		return false;

	//   res = !anObj->_is_nil();
	if (res && !IsPreview()) {
		QStringList aParameters;
		//@@ put stringified input parameters to the string list here to store in the data model for notebook @@//
		aParameters << MainTubeGroupParams->SpinBox_DX->text(); // R1 parameter
		aParameters << MainTubeGroupParams->SpinBox_DY->text(); // W1 parameter
		aParameters << MainTubeGroupParams->SpinBox_DZ->text(); // L1 parameter
		aParameters << IncidentTubeGroupParams->SpinBox_DX->text(); // R2 parameter
		aParameters << IncidentTubeGroupParams->SpinBox_DY->text(); // W2 parameter
		aParameters << IncidentTubeGroupParams->SpinBox_DZ->text(); // L2 parameter
		if (ChamferGroupParams->GroupBox1->isChecked()) {// Chamfer parameter
			aParameters << ChamferGroupParams->SpinBox_DX->text(); // H parameter
			aParameters << ChamferGroupParams->SpinBox_DY->text(); // W parameter
		}
		if (FilletGroupParams->GroupBox1->isChecked()) // Fillet parameter
			aParameters << FilletGroupParams->SpinBox_DX->text(); // RF parameter

		if (aParameters.count() > 0)
			anObj[0]->SetParameters(aParameters.join(":").toLatin1().constData());
	}

	objects.push_back(anObj[0]._retn());
	pipeTShapeGroupObjects.clear();
	for (int i = 1, n = anObj->length(); i < n; i++) {
		pipeTShapeGroupObjects.push_back(anObj[i]._retn());
	}

	return objects.size() > 0;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::restoreSubShapes(SALOMEDS::Study_ptr theStudy, SALOMEDS::SObject_ptr theSObject) {
	SALOMEDS::GenericAttribute_var anAttr;
	if (!theSObject->FindAttribute(anAttr, "AttributeIOR"))
		return;

	SALOMEDS::AttributeIOR_var anAttrIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	CORBA::String_var anIORso = anAttrIOR->Value();

	// get Object from SObject
	GEOM::GEOM_Object_var theFather = GEOM::GEOM_Object::_narrow(myGeomGUI->getApp()->orb()->string_to_object(anIORso));
	if (CORBA::is_nil(theFather))
		return;

	ObjectList::iterator it = pipeTShapeGroupObjects.begin();

	for (int i = 0; it != pipeTShapeGroupObjects.end(); it++, i++) {
		getGeomEngine()->AddInStudy(theStudy, (*it), tr((*it)->GetName()).toStdString().c_str(), theFather);
	}

}
