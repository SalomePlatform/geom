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
//  File   : MeasureGUI_MaxToleranceDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#include "MeasureGUI_MaxToleranceDlg.h"

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : MeasureGUI_MaxToleranceDlg()
// purpose  : Constructs a MeasureGUI_MaxToleranceDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_MaxToleranceDlg::MeasureGUI_MaxToleranceDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :MeasureGUI_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_TOLERANCE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_TOLERANCE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_TOLERANCE"));
  RadioButton1->setPixmap(image0);

  GroupC1 = new MeasureGUI_1Sel6LineEdit_QTD(this, "GroupC1");
  GroupC1->GroupBox1->setTitle(tr("GEOM_TOLERANCE_CONSTR"));
  GroupC1->TextLabel1->setText(tr("GEOM_OBJECT"));
  GroupC1->TextLabel2->setText(tr("GEOM_MIN"));
  GroupC1->TextLabel3->setText(tr("GEOM_MAX"));
  GroupC1->TextLabel4->setText(tr("GEOM_TOLERANCE_FACE"));
  GroupC1->TextLabel5->setText(tr("GEOM_TOLERANCE_EDGE"));
  GroupC1->TextLabel6->setText(tr("GEOM_TOLERANCE_VERTEX"));
  GroupC1->LineEdit11->setReadOnly(TRUE);
  GroupC1->LineEdit12->setReadOnly(TRUE);
  GroupC1->LineEdit21->setReadOnly(TRUE);
  GroupC1->LineEdit22->setReadOnly(TRUE);
  GroupC1->LineEdit31->setReadOnly(TRUE);
  GroupC1->LineEdit32->setReadOnly(TRUE);
  GroupC1->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupC1, 1, 0);
  /***************************************************************/

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_MaxToleranceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_MaxToleranceDlg::~MeasureGUI_MaxToleranceDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_MaxToleranceDlg::Init()
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
void MeasureGUI_MaxToleranceDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = "";

  GroupC1->LineEdit11->setText("");
  GroupC1->LineEdit12->setText("");
  GroupC1->LineEdit21->setText("");
  GroupC1->LineEdit22->setText("");
  GroupC1->LineEdit31->setText("");
  GroupC1->LineEdit32->setText("");

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1)
    return;

  /*  nbSel == 1  */
  TopoDS_Shape S;
  if(!myGeomBase->GetTopoFromSelection(mySelection, S) || S.IsNull())
    return;

  if(S.IsNull())
    return;

  GroupC1->LineEdit1->setText(aString);

  this->CalculateMaxTolerance(S);
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_MaxToleranceDlg::SetEditCurrentArgument()
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
void MeasureGUI_MaxToleranceDlg::LineEditReturnPressed()
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
void MeasureGUI_MaxToleranceDlg::ActivateThisDialog()
{
  MeasureGUI_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_MaxToleranceDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : CalculateMaxTolerance()
// purpose  :
//=================================================================================
void MeasureGUI_MaxToleranceDlg::CalculateMaxTolerance(const TopoDS_Shape& S)
{
  GroupC1->LineEdit11->setText("");
  GroupC1->LineEdit12->setText("");
  GroupC1->LineEdit21->setText("");
  GroupC1->LineEdit22->setText("");
  GroupC1->LineEdit31->setText("");
  GroupC1->LineEdit32->setText("");

  if(S.IsNull()) 
    return;

  Standard_Real T, TMF, TME, TMV, TmF, TmE, TmV;
  Standard_Integer nbF, nbE, nbV;
  bool m_isFace, m_isEdge, m_isVertex;

  TMF = TME = TMV = -RealLast();
  TmF = TmE = TmV = RealLast();
  nbF = nbE = nbV = 0;
  m_isFace = m_isEdge = m_isVertex = false;

  try {
    for(TopExp_Explorer ExF(S,TopAbs_FACE); ExF.More(); ExF.Next()) { 
      m_isFace = true;
      TopoDS_Face Face=TopoDS::Face(ExF.Current());
      T=BRep_Tool::Tolerance(Face);
      if(T>TMF)
	TMF=T;
      if(T<TmF)
	TmF=T;
      nbF++;
    }
    for(TopExp_Explorer ExE(S,TopAbs_EDGE); ExE.More(); ExE.Next()) { 
      m_isEdge = true;
      TopoDS_Edge Edge=TopoDS::Edge(ExE.Current());
      T=BRep_Tool::Tolerance(Edge);
      if(T>TME)
	TME=T;
      if(T<TmE)
	TmE=T;
      nbE++;
    }
    for(TopExp_Explorer ExV(S,TopAbs_VERTEX); ExV.More(); ExV.Next()) { 
      m_isVertex = true;
      TopoDS_Vertex Vertex=TopoDS::Vertex(ExV.Current());
      T=BRep_Tool::Tolerance(Vertex);
      if(T>TMV)
	TMV=T;
      if(T<TmV)
	TmV=T;
      nbV++;
    }

    if(m_isFace) {
      GroupC1->LineEdit11->setText(tr("%1").arg(TmF, 5, 'e', 8));
      GroupC1->LineEdit12->setText(tr("%1").arg(TMF, 5, 'e', 8));
    }
    if(m_isEdge) {
      GroupC1->LineEdit21->setText(tr("%1").arg(TmE, 5, 'e', 8));
      GroupC1->LineEdit22->setText(tr("%1").arg(TME, 5, 'e', 8));
      }
    if(m_isVertex) {
      GroupC1->LineEdit31->setText(tr("%1").arg(TmV, 5, 'e', 8));
      GroupC1->LineEdit32->setText(tr("%1").arg(TMV, 5, 'e', 8));
    }
  }
  catch(Standard_Failure) 
    {
      MESSAGE("Catch intercepted in CalculateMaxTolerance()");
    }
  return;
}
