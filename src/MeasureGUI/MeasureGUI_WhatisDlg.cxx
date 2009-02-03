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
//
//  GEOM GEOMGUI : GUI for Geometry component
//  File   : MeasureGUI_WhatisDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$
//
#include "MeasureGUI_WhatisDlg.h"
//#include "MeasureGUI_1Sel1TextView_QTD.h"
#include "GEOMBase.h"

#include "DlgRef_SpinBox.h"

#include <TopTools_MapOfShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

#include <BRep_Tool.hxx>

#include "utilities.h"
#include "SUIT_Session.h"
#include "SalomeApp_Tools.h"

#include <qtextedit.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>

//=================================================================================
// Constructs a MeasureGUI_Whatis_QTD which is a child of 'parent', with the
// name 'name' and widget flags set to 'f'
//=================================================================================
MeasureGUI_Whatis_QTD::MeasureGUI_Whatis_QTD (QWidget* parent, const char* name, WFlags fl)
  : QWidget(parent, name, fl)
{
  if (!name)
    setName("MeasureGUI_Whatis_QTD");
  //resize(130, 160); 
  MeasureGUI_Whatis_QTDLayout = new QGridLayout (this, 1, 1, 0, 6, "MeasureGUI_Whatis_QTDLayout"); 

  GroupBox1 = new QGroupBox(this, "GroupBox1");
  GroupBox1->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7,
                                       0, 0, GroupBox1->sizePolicy().hasHeightForWidth()));
  GroupBox1->setColumnLayout(0, Qt::Vertical);
  GroupBox1->layout()->setSpacing(6);
  GroupBox1->layout()->setMargin(11);
  GroupBox1Layout = new QGridLayout(GroupBox1->layout());
  GroupBox1Layout->setAlignment(Qt::AlignTop);

  Layout1 = new QGridLayout(0, 1, 1, 0, 6, "Layout1"); 

  TextLabel1 = new QLabel (GroupBox1, "TextLabel1");
  TextLabel1->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0,
                                        0, 0, TextLabel1->sizePolicy().hasHeightForWidth()));

  PushButton1 = new QPushButton (GroupBox1, "PushButton1");
  PushButton1->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0,
                                         0, 0, PushButton1->sizePolicy().hasHeightForWidth()));

  LineEdit1 = new QLineEdit (GroupBox1, "LineEdit1");

  Layout1->addWidget(TextLabel1 , 0, 0);
  Layout1->addWidget(PushButton1, 0, 1);
  Layout1->addWidget(LineEdit1  , 0, 2);

  TextEdit1 = new QTextEdit (GroupBox1, "TextEdit1");

  Layout1->addMultiCellWidget(TextEdit1, 1, 1, 0, 2);

  TextLabel2 = new QLabel (GroupBox1, "TextLabel2");
  TextLabel2->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0,
                                        0, 0, TextLabel1->sizePolicy().hasHeightForWidth()));

  LineEdit2 = new QLineEdit (GroupBox1, "LineEdit2");

  PushButton2 = new QPushButton (GroupBox1, "PushButton2");
  PushButton2->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0,
                                         0, 0, PushButton1->sizePolicy().hasHeightForWidth()));

  Layout1->addWidget(TextLabel2 , 2, 0);
  Layout1->addWidget(LineEdit2  , 2, 1);
  Layout1->addWidget(PushButton2, 2, 2);

  GroupBox1Layout->addLayout(Layout1, 0, 0);

  MeasureGUI_Whatis_QTDLayout->addWidget(GroupBox1, 0, 0);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
MeasureGUI_Whatis_QTD::~MeasureGUI_Whatis_QTD()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// class    : MeasureGUI_WhatisDlg( )
// purpose  : Constructs a MeasureGUI_WhatisDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_WhatisDlg::MeasureGUI_WhatisDlg (GeometryGUI* GUI, QWidget* parent)
  : MeasureGUI_Skeleton(GUI, parent, "MeasureGUI_WhatisDlg")
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM",tr( "ICON_DLG_WHATIS" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM",tr( "ICON_SELECT" ) ) );

  setCaption( tr( "GEOM_WHATIS_TITLE" ) );

  /***************************************************************/

  GroupConstructors->setTitle( tr( "GEOM_WHATIS" ) );
  RadioButton1->setPixmap( image0 );

  //myGrp = new MeasureGUI_1Sel1TextView_QTD( this, "myGrp" );
  myGrp = new MeasureGUI_Whatis_QTD( this, "myGrp" );
  myGrp->GroupBox1->setTitle( tr( "GEOM_WHATIS_OBJECT" ) );

  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->PushButton1->setPixmap( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  myGrp->TextEdit1->setReadOnly( TRUE );
  // fix height to fit all text
  myGrp->TextEdit1->setWordWrap(QTextEdit::NoWrap);
  myGrp->TextEdit1->setTextFormat(Qt::PlainText);
  QString allLines ("\n\n\n\n\n\n\n\n\n"); // 10 lines
  myGrp->TextEdit1->setText(allLines);
  int sbHeight = myGrp->TextEdit1->horizontalScrollBar()->height();
  myGrp->TextEdit1->setFixedHeight(myGrp->TextEdit1->contentsHeight() + sbHeight);

  myGrp->TextLabel2->setText(tr("GEOM_KIND_OF_SHAPE"));
  myGrp->LineEdit2->setReadOnly(true);
  myGrp->PushButton2->setText(tr("GEOM_PROPERTIES"));

  Layout1->addWidget( myGrp, 1, 0 );

  // Properties dialog
  //myParamsDlg = new MeasureGUI_Params_Dlg;
  myParamsDlg = new QMessageBox ("", "", QMessageBox::Information,
                                 QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, 
				 this, 0, FALSE);

  /***************************************************************/

  myHelpFileName = "using_measurement_tools_page.html#whatis_anchor";

  /* Initialisation */
  Init();
}

//=================================================================================
// function : ~MeasureGUI_WhatisDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_WhatisDlg::~MeasureGUI_WhatisDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::Init()
{
  mySelBtn = myGrp->PushButton1;
  mySelEdit = myGrp->LineEdit1;

  connect(myGrp->PushButton2, SIGNAL(clicked()), this, SLOT(ClickOnProperties()));

  MeasureGUI_Skeleton::Init();
}

//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::processObject()
{
  QString aText;
  myGrp->TextEdit1->setText( getParameters( aText ) ? aText : QString("") );

  QString aParameters;
  QString aKind = getKindOfShape(aParameters);
  myGrp->LineEdit2->setText(aKind);
  myGrp->PushButton2->setEnabled(!aParameters.isEmpty());
  myParamsDlg->setCaption(aKind + " " + tr("GEOM_PROPERTIES"));
  myParamsDlg->setText(aParameters);
  myParamsDlg->adjustSize();
}

//=================================================================================
// function : ClickOnProperties()
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::ClickOnProperties()
{
  myParamsDlg->show();
}

//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
bool MeasureGUI_WhatisDlg::getParameters( QString& theText )
{
  if ( myObj->_is_nil() )
    return false;

  try
  {
    theText = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() )->WhatIs( myObj );
  }
  catch( const SALOME::SALOME_Exception& e )
  {
    SalomeApp_Tools::QtCatchCorbaException( e );
    return false;
  }

  return getOperation()->IsDone();
}

//=================================================================================
// function : getKindOfShape
// purpose  :
//=================================================================================
QString MeasureGUI_WhatisDlg::getKindOfShape (QString& theParameters)
{
  QString aKindStr ("");
  theParameters = "";

  if ( myObj->_is_nil() )
    return aKindStr;

  GEOM::GEOM_IKindOfShape::shape_kind aKind;
  GEOM::ListOfLong_var anInts;
  GEOM::ListOfDouble_var aDbls;

  GEOM::GEOM_IMeasureOperations_var anOp = GEOM::GEOM_IMeasureOperations::_narrow(getOperation());

  try
  {
    aKind = anOp->KindOfShape(myObj, anInts, aDbls);
  }
  catch( const SALOME::SALOME_Exception& e )
  {
    SalomeApp_Tools::QtCatchCorbaException( e );
    return false;
  }

  if (!anOp->IsDone())
    return aKindStr;

  switch (aKind)
  {
  case GEOM::GEOM_IKindOfShape::COMPOUND:
    aKindStr = tr("GEOM_COMPOUND");
    break;
  case GEOM::GEOM_IKindOfShape::COMPSOLID:
    aKindStr = tr("GEOM_COMPOUNDSOLID");
    break;
  case GEOM::GEOM_IKindOfShape::SHELL:
    aKindStr = tr("GEOM_SHELL");
    if (anInts[0] == 1)
      theParameters = tr("GEOM_CLOSED");
    else if (anInts[0] == 2)
      theParameters = tr("GEOM_UNCLOSED");
    else
      theParameters = tr("GEOM_CLOSEDUNCLOSED");
    break;
  case GEOM::GEOM_IKindOfShape::WIRE:
    aKindStr = tr("GEOM_WIRE");
    if (anInts[0] == 1)
      theParameters = tr("GEOM_CLOSED");
    else if (anInts[0] == 2)
      theParameters = tr("GEOM_UNCLOSED");
    else
      theParameters = tr("GEOM_CLOSEDUNCLOSED");
    break;
  // SOLIDs
  case GEOM::GEOM_IKindOfShape::SPHERE:
    aKindStr = tr("GEOM_SPHERE");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS")     + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]);
    break;
  case GEOM::GEOM_IKindOfShape::CYLINDER:
    aKindStr = tr("GEOM_CYLINDER");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_AXIS") +
      "\n" +        tr("GEOM_DX")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS")     + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]) +
      "\n" +        tr("GEOM_HEIGHT")     + DlgRef_SpinBox::PrintDoubleValue(aDbls[7]);
    break;
  case GEOM::GEOM_IKindOfShape::BOX:
    aKindStr = tr("GEOM_BOX");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        "Ax :"                + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        "Ay :"                + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        "Az :"                + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]);
    break;
  case GEOM::GEOM_IKindOfShape::ROTATED_BOX:
    aKindStr = tr("GEOM_BOX");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\nZ Axis:" +
      "\n" +        "Zx :"                  + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        "Zy :"                  + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        "Zz :"                  + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\nX Axis:" +
      "\n" +        tr("GEOM_X_I").arg("x") + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]) +
      "\n" +        tr("GEOM_X_I").arg("y") + DlgRef_SpinBox::PrintDoubleValue(aDbls[7]) +
      "\n" +        tr("GEOM_X_I").arg("z") + DlgRef_SpinBox::PrintDoubleValue(aDbls[8]) +
      "\nDimensions along local axes:" +
      "\n" +        "Ax :"                  + DlgRef_SpinBox::PrintDoubleValue(aDbls[9]) +
      "\n" +        "Ay :"                  + DlgRef_SpinBox::PrintDoubleValue(aDbls[10]) +
      "\n" +        "Az :"                  + DlgRef_SpinBox::PrintDoubleValue(aDbls[11]);
    break;
  case GEOM::GEOM_IKindOfShape::TORUS:
    aKindStr = tr("GEOM_TORUS");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_AXIS") +
      "\n" +        tr("GEOM_DX")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS_I").arg(1) + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]) +
      "\n" +        tr("GEOM_RADIUS_I").arg(2) + DlgRef_SpinBox::PrintDoubleValue(aDbls[7]);
    break;
  case GEOM::GEOM_IKindOfShape::CONE:
    aKindStr = tr("GEOM_CONE");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_AXIS") +
      "\n" +        tr("GEOM_DX")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS_I").arg(1) + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]) +
      "\n" +        tr("GEOM_RADIUS_I").arg(2) + DlgRef_SpinBox::PrintDoubleValue(aDbls[7]) +
      "\n" +        tr("GEOM_HEIGHT")          + DlgRef_SpinBox::PrintDoubleValue(aDbls[8]);
    break;
  case GEOM::GEOM_IKindOfShape::POLYHEDRON:
    aKindStr = tr("GEOM_POLYHEDRON");
    break;
  case GEOM::GEOM_IKindOfShape::SOLID:
    aKindStr = tr("GEOM_SOLID");
    break;
  // FACEs
  case GEOM::GEOM_IKindOfShape::SPHERE2D:
    aKindStr = tr("GEOM_SURFSPHERE");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS")     + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]);
    break;
  case GEOM::GEOM_IKindOfShape::CYLINDER2D:
    aKindStr = tr("GEOM_SURFCYLINDER");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_AXIS") +
      "\n" +        tr("GEOM_DX")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS")     + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]) +
      "\n" +        tr("GEOM_HEIGHT")     + DlgRef_SpinBox::PrintDoubleValue(aDbls[7]);
    break;
  case GEOM::GEOM_IKindOfShape::TORUS2D:
    aKindStr = tr("GEOM_SURFTORUS");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_AXIS") +
      "\n" +        tr("GEOM_DX")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS_I").arg(1) + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]) +
      "\n" +        tr("GEOM_RADIUS_I").arg(2) + DlgRef_SpinBox::PrintDoubleValue(aDbls[7]);
    break;
  case GEOM::GEOM_IKindOfShape::CONE2D:
    aKindStr = tr("GEOM_SURFCONE");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0)      + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_AXIS") +
      "\n" +        tr("GEOM_DX")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")              + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS_I").arg(1) + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]) +
      "\n" +        tr("GEOM_RADIUS_I").arg(2) + DlgRef_SpinBox::PrintDoubleValue(aDbls[7]) +
      "\n" +        tr("GEOM_HEIGHT")          + DlgRef_SpinBox::PrintDoubleValue(aDbls[8]);
    break;
  case GEOM::GEOM_IKindOfShape::DISK_CIRCLE:
    aKindStr = tr("GEOM_DISK_CIRCLE");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_NORMAL") +
      "\n" +        tr("GEOM_DX")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS")     + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]);
    break;
  case GEOM::GEOM_IKindOfShape::DISK_ELLIPSE:
    aKindStr = tr("GEOM_DISK_ELLIPSE");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_NORMAL") +
      "\n" +        tr("GEOM_DX")           + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")           + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")           + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS_MAJOR") + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]) +
      "\n" +        tr("GEOM_RADIUS_MINOR") + DlgRef_SpinBox::PrintDoubleValue(aDbls[7]);
    break;
  case GEOM::GEOM_IKindOfShape::POLYGON:
    aKindStr = tr("GEOM_POLYGON");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_NORMAL") +
      "\n" +        tr("GEOM_DX")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]);
    break;
  case GEOM::GEOM_IKindOfShape::PLANE:
    aKindStr = tr("GEOM_PLANE");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_NORMAL") +
      "\n" +        tr("GEOM_DX")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]);
    break;
  case GEOM::GEOM_IKindOfShape::PLANAR:
    aKindStr = tr("GEOM_PLANAR_FACE");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_NORMAL") +
      "\n" +        tr("GEOM_DX")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]);
    break;
  case GEOM::GEOM_IKindOfShape::FACE:
    aKindStr = tr("GEOM_FACE");
    break;
  // EDGEs
  case GEOM::GEOM_IKindOfShape::CIRCLE:
    aKindStr = tr("GEOM_CIRCLE");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_NORMAL") +
      "\n" +        tr("GEOM_DX")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS")     + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]);
    break;
  case GEOM::GEOM_IKindOfShape::ARC_CIRCLE:
    aKindStr = tr("GEOM_ARC");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_NORMAL") +
      "\n" +        tr("GEOM_DX")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS")     + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]) +
      "\n" +        tr("GEOM_POINT_I").arg(1) +
      "\n" +        tr("GEOM_X_I").arg(1) + DlgRef_SpinBox::PrintDoubleValue(aDbls[7]) +
      "\n" +        tr("GEOM_Y_I").arg(1) + DlgRef_SpinBox::PrintDoubleValue(aDbls[8]) +
      "\n" +        tr("GEOM_Z_I").arg(1) + DlgRef_SpinBox::PrintDoubleValue(aDbls[9]) +
      "\n" +        tr("GEOM_POINT_I").arg(2) +
      "\n" +        tr("GEOM_X_I").arg(2) + DlgRef_SpinBox::PrintDoubleValue(aDbls[10]) +
      "\n" +        tr("GEOM_Y_I").arg(2) + DlgRef_SpinBox::PrintDoubleValue(aDbls[11]) +
      "\n" +        tr("GEOM_Z_I").arg(2) + DlgRef_SpinBox::PrintDoubleValue(aDbls[12]);
    break;
  case GEOM::GEOM_IKindOfShape::ELLIPSE:
    aKindStr = tr("GEOM_ELLIPSE");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_NORMAL") +
      "\n" +        tr("GEOM_DX")           + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")           + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")           + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS_MAJOR") + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]) +
      "\n" +        tr("GEOM_RADIUS_MINOR") + DlgRef_SpinBox::PrintDoubleValue(aDbls[7]);
    break;
  case GEOM::GEOM_IKindOfShape::ARC_ELLIPSE:
    aKindStr = tr("GEOM_ARC_ELLIPSE");
    theParameters = tr("GEOM_CENTER") +
      "\n" +        tr("GEOM_X_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_NORMAL") +
      "\n" +        tr("GEOM_DX")           + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")           + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")           + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]) +
      "\n" +        tr("GEOM_DIMENSIONS") +
      "\n" +        tr("GEOM_RADIUS_MAJOR") + DlgRef_SpinBox::PrintDoubleValue(aDbls[6]) +
      "\n" +        tr("GEOM_RADIUS_MINOR") + DlgRef_SpinBox::PrintDoubleValue(aDbls[7]) +
      "\n" +        tr("GEOM_POINT_I").arg(1) +
      "\n" +        tr("GEOM_X_I").arg(1)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[8]) +
      "\n" +        tr("GEOM_Y_I").arg(1)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[9]) +
      "\n" +        tr("GEOM_Z_I").arg(1)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[10]) +
      "\n" +        tr("GEOM_POINT_I").arg(2) +
      "\n" +        tr("GEOM_X_I").arg(2)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[11]) +
      "\n" +        tr("GEOM_Y_I").arg(2)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[12]) +
      "\n" +        tr("GEOM_Z_I").arg(2)   + DlgRef_SpinBox::PrintDoubleValue(aDbls[13]);
    break;
  case GEOM::GEOM_IKindOfShape::LINE:
    aKindStr = tr("GEOM_LINE");
    theParameters = tr("GEOM_POSITION") +
      "\n" +        tr("GEOM_X_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(0) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_DIRECTION") +
      "\n" +        tr("GEOM_DX")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_DY")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_DZ")         + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]);
    break;
  case GEOM::GEOM_IKindOfShape::SEGMENT:
    aKindStr = tr("GEOM_SEGMENT");
    theParameters = tr("GEOM_POINT_I").arg(1) +
      "\n" +        tr("GEOM_X_I").arg(1) + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y_I").arg(1) + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z_I").arg(1) + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]) +
      "\n" +        tr("GEOM_POINT_I").arg(2) +
      "\n" +        tr("GEOM_X_I").arg(2) + DlgRef_SpinBox::PrintDoubleValue(aDbls[3]) +
      "\n" +        tr("GEOM_Y_I").arg(2) + DlgRef_SpinBox::PrintDoubleValue(aDbls[4]) +
      "\n" +        tr("GEOM_Z_I").arg(2) + DlgRef_SpinBox::PrintDoubleValue(aDbls[5]);
    break;
  case GEOM::GEOM_IKindOfShape::EDGE:
    aKindStr = tr("GEOM_EDGE");
    break;
  case GEOM::GEOM_IKindOfShape::VERTEX:
    aKindStr = tr("GEOM_VERTEX");
    theParameters = tr("GEOM_COORDINATES") +
      "\n" +        tr("GEOM_X") + DlgRef_SpinBox::PrintDoubleValue(aDbls[0]) +
      "\n" +        tr("GEOM_Y") + DlgRef_SpinBox::PrintDoubleValue(aDbls[1]) +
      "\n" +        tr("GEOM_Z") + DlgRef_SpinBox::PrintDoubleValue(aDbls[2]);
    break;
  default:
    ;
  }

  return aKindStr;
}
