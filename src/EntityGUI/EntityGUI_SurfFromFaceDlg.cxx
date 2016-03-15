// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : EntityGUI_SurfFromFaceDlg.cxx

#include "EntityGUI_SurfFromFaceDlg.h"
#include <GeometryGUI.h>
#include <DlgRef.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>


//=================================================================================
// class    : EntityGUI_SurfFromFaceDlg
// purpose  : 
//=================================================================================
EntityGUI_SurfFromFaceDlg::EntityGUI_SurfFromFaceDlg
                                           (GeometryGUI     *theGeometryGUI,
                                            QWidget         *parent,
                                            bool             modal,
                                            Qt::WindowFlags  fl)
  : GEOMBase_Skeleton (theGeometryGUI, parent, modal, fl),
    myGroup           (0)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SURFACE_FROM_FACE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_SURF_FROM_FACE_TITLE"));

  /***************************************************************/

  mainFrame()->GroupConstructors->setTitle(tr("GEOM_SURF_FROM_FACE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->close();

  // Construct a group.
  myGroup = new DlgRef_1Sel(centralWidget());
  myGroup->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  myGroup->TextLabel1->setText(tr("GEOM_FACE"));
  myGroup->PushButton1->setIcon(image1);
  myGroup->LineEdit1->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGroup);

  setHelpFileName("create_surface_from_face_page.html");

  Init();
}

//=================================================================================
// function : ~EntityGUI_SurfFromFaceDlg()
// purpose  : 
//=================================================================================
EntityGUI_SurfFromFaceDlg::~EntityGUI_SurfFromFaceDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void EntityGUI_SurfFromFaceDlg::Init()
{
  initName(tr("GEOM_SURF_FROM_FACE_NAME"));
  showOnlyPreviewControl();

  /* signals and slots connections */
  connect(myGroup->PushButton1, SIGNAL(clicked()),
          this,                 SLOT(SetEditCurrentArgument()));
  connect(myGeomGUI->getApp()->selectionMgr(),
                                SIGNAL(currentSelectionChanged()),
          this,                 SLOT(SelectionIntoArgument()));
  connect(myGeomGUI,            SIGNAL(SignalDeactivateActiveDialog()),
          this,                 SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI,            SIGNAL(SignalCloseAllDialogs()),
          this,                 SLOT(ClickOnCancel()));
  connect(buttonOk(),           SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(),        SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  myEditCurrentArgument = myGroup->LineEdit1;
  myGroup->LineEdit1->setReadOnly(true);
  SelectionIntoArgument();
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  : Called when selection is changed
//=================================================================================
void EntityGUI_SurfFromFaceDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");
  myFace.nullify();

  GEOM::GeomObjPtr aSelectedObject = getSelected(TopAbs_FACE);

  if (aSelectedObject) {
    myEditCurrentArgument->setText(GEOMBase::GetName(aSelectedObject.get()));
    myFace = aSelectedObject;
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void EntityGUI_SurfFromFaceDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == myGroup->PushButton1) {
    myEditCurrentArgument->setFocus();
    SelectionIntoArgument();
  }
}

//=================================================================================
// function : ActivateThisDialog
// purpose  :
//=================================================================================
void EntityGUI_SurfFromFaceDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(myGeomGUI->getApp()->selectionMgr(),
                       SIGNAL(currentSelectionChanged()),
          this,        SLOT(SelectionIntoArgument()));
  SelectionIntoArgument();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void EntityGUI_SurfFromFaceDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr EntityGUI_SurfFromFaceDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetIShapesOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool EntityGUI_SurfFromFaceDlg::isValid (QString& msg)
{
  return myFace;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool EntityGUI_SurfFromFaceDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IShapesOperations_var anOper =
    GEOM::GEOM_IShapesOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->MakeSurfaceFromFace(myFace.get());

  if (!anObj->_is_nil()) {
    objects.push_back(anObj._retn());
  }

  return true;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void EntityGUI_SurfFromFaceDlg::ClickOnOk()
{
  setIsApplyAndClose(true);

  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool EntityGUI_SurfFromFaceDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();

  return true;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> EntityGUI_SurfFromFaceDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myFace;
  return res;
}
