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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : MeasureGUI_WhatisDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

using namespace std;
#include "MeasureGUI_WhatisDlg.h"

#include <TopTools_MapOfShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS_Iterator.hxx>

#include <qtextedit.h>

//=================================================================================
// class    : MeasureGUI_WhatisDlg()
// purpose  : Constructs a MeasureGUI_WhatisDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_WhatisDlg::MeasureGUI_WhatisDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :MeasureGUI_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_WHATIS")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_WHATIS_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_WHATIS"));
  RadioButton1->setPixmap(image0);

  GroupC1 = new MeasureGUI_1Sel1TextView_QTD(this, "GroupC1");
  GroupC1->GroupBox1->setTitle(tr("GEOM_WHATIS_OBJECT"));
  GroupC1->TextLabel1->setText(tr("GEOM_OBJECT"));
  GroupC1->TextEdit1->setReadOnly(TRUE);
  GroupC1->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupC1, 1, 0);
  /***************************************************************/

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_WhatisDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_WhatisDlg::~MeasureGUI_WhatisDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupC1->LineEdit1;

   /* signals and slots connections */
  connect(GroupC1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupC1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupC1->show();
  this->show();

  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void MeasureGUI_WhatisDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  myEditCurrentArgument->setText("");
  SelectedName = ""; /* future the name of selection */
  GroupC1->TextEdit1->setText("");

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, SelectedName);
  if(nbSel != 1)
    return;

  /*  nbSel == 1  */
  TopoDS_Shape S;
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  if(S.IsNull())
    return;

   /* Try to display of a cone simulation shape to show direction of a linear edge only in OCC viewer */
  if(myGeomBase->CreateArrowForLinearEdge(S, mySimulationTopoDs))
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);

  GroupC1->LineEdit1->setText(SelectedName);
  this->CalculateWhatis(S);

  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupC1->PushButton1) {
    GroupC1->LineEdit1->setFocus();
    myEditCurrentArgument = GroupC1->LineEdit1;
  }

  this->SelectionIntoArgument();
  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupC1->LineEdit1)
    myEditCurrentArgument = GroupC1->LineEdit1;
  else
    return;

  MeasureGUI_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::ActivateThisDialog()
{
  MeasureGUI_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : CalculateWhatis()
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::CalculateWhatis(const TopoDS_Shape& S)
{
  if(S.IsNull()) 
    return;

  TCollection_AsciiString Astr; 
  Astr = Astr + " Number of shapes in " + strdup(SelectedName.latin1()) + " : \n";

  try {
    int iType, nbTypes [TopAbs_SHAPE];
    for(iType = 0; iType < TopAbs_SHAPE; ++iType)
      nbTypes[iType] = 0;
    nbTypes[S.ShapeType()]++;

    TopTools_MapOfShape aMapOfShape;
    aMapOfShape.Add(S);
    TopTools_ListOfShape aListOfShape;
    aListOfShape.Append(S);

    TopTools_ListIteratorOfListOfShape itL(aListOfShape);
    for(; itL.More(); itL.Next()) {
      TopoDS_Iterator it(itL.Value());
      for(; it.More(); it.Next()) {
        TopoDS_Shape s = it.Value();
        if(aMapOfShape.Add(s)) {
          aListOfShape.Append(s);
          nbTypes[s.ShapeType()]++;
        }
      }
    }

    Astr = Astr + " VERTEX : " + TCollection_AsciiString(nbTypes[TopAbs_VERTEX]) + "\n";
    Astr = Astr + " EDGE : " + TCollection_AsciiString(nbTypes[TopAbs_EDGE]) + "\n";
    Astr = Astr + " WIRE : " + TCollection_AsciiString(nbTypes[TopAbs_WIRE]) + "\n";
    Astr = Astr + " FACE : " + TCollection_AsciiString(nbTypes[TopAbs_FACE]) + "\n";
    Astr = Astr + " SHELL : " + TCollection_AsciiString(nbTypes[TopAbs_SHELL]) + "\n";
    Astr = Astr + " SOLID : " + TCollection_AsciiString(nbTypes[TopAbs_SOLID]) + "\n";
    Astr = Astr + " COMPSOLID : " + TCollection_AsciiString(nbTypes[TopAbs_COMPSOLID]) + "\n";
    Astr = Astr + " COMPOUND : " + TCollection_AsciiString(nbTypes[TopAbs_COMPOUND]) + "\n";
    Astr = Astr + " SHAPE : " + TCollection_AsciiString(aMapOfShape.Extent());

    GroupC1->TextEdit1->setText(Astr.ToCString());
  }
  catch(Standard_Failure) {
    MESSAGE("Catch intercepted in CalculateWhatis()");
  }
  return;
}
