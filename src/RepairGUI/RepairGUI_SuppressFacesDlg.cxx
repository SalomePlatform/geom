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
//  File   : RepairGUI_SuppressFacesDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "RepairGUI_SuppressFacesDlg.h"

#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"
#include "SUIT_Session.h"
#include "SALOME_ListIteratorOfListIO.hxx"

#include "GEOMImpl_Types.hxx"

#include <TopAbs.hxx>

#include <qlabel.h>

using namespace std;

//=================================================================================
// class    : RepairGUI_SuppressFacesDlg()
// purpose  : Constructs a RepairGUI_SuppressFacesDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_SuppressFacesDlg::RepairGUI_SuppressFacesDlg(QWidget* parent, const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_SUPRESS_FACE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));
    
  setCaption(tr("GEOM_SUPRESSFACE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_SUPRESSFACE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("Faces to remove"));
  GroupPoints->TextLabel1->setText(tr("Faces"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/

  Init();
}


//=================================================================================
// function : ~RepairGUI_SuppressFacesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_SuppressFacesDlg::~RepairGUI_SuppressFacesDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myObjects = new GEOM::ListOfGO();
  myObjects->length( 0 );

  //myGeomGUI->SetState( 0 );
  initSelection();
	  
  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( tr( "SUPRESS_FACE_NEW_OBJ_NAME" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_SuppressFacesDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}



//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_SuppressFacesDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  myEditCurrentArgument->setText("");
  myObjects->length( 0 );
  myFaces.clear();

  initSelection();
  
  return true;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//          : used only by SelectButtonC1A1 (LineEditC1A1)
//=================================================================================
void RepairGUI_SuppressFacesDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  Standard_Boolean aRes = Standard_False;
  int i = 0;
  int numFaces = 0;
  myObjects->length( IObjectCount() );
  myFaces.clear();
  for ( SALOME_ListIteratorOfListIO anIt( selectedIO() ); anIt.More(); anIt.Next() )
  {
    Handle(SALOME_InteractiveObject) anIO = anIt.Value();
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
    if ( !CORBA::is_nil( aSelectedObject ) && aRes )
    {
      TopoDS_Shape aShape;
      if ( myGeomBase->GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) )
      {
	if ( aShape.ShapeType() <= TopAbs_FACE ) // FACE, SHELL, SOLID, COMPOUND
	{
	  GEOM::short_array anIndexes;

	  TColStd_IndexedMapOfInteger aMap;
	  ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr()->GetIndexes( anIO, aMap );
	  
	  if ( !aMap.IsEmpty() )
	  {
	    Convert( aMap, anIndexes );
	    myObjects[i++] = aSelectedObject; // append the object
	    myFaces.append( anIndexes );   // append faces' indexes
	    numFaces += anIndexes.length();// just for text field output
	  }
	}
      }
    }
  }
  myObjects->length( i ); // this is the right length, smaller of equal to the previously set
  if ( numFaces )
    myEditCurrentArgument->setText( QString::number( numFaces ) + "_" + tr( "GEOM_FACE" ) + tr( "_S_" ) );
}

//=================================================================================
// function : Convert()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::Convert( const TColStd_IndexedMapOfInteger& theMap, GEOM::short_array& theOutSeq )
{
  const int n = theMap.Extent();
  theOutSeq.length( n );
  for ( int i = 0; i < n; i++ )
    theOutSeq[i] = theMap( i+1 );
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::SetEditCurrentArgument()
{
  if( sender() == GroupPoints->PushButton1 )
  {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::LineEditReturnPressed()
{
  if( sender() == GroupPoints->LineEdit1 )
  {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::DeactivateActiveDialog()
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  //myGeomGUI->SetState( 0 );
  initSelection();
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_SuppressFacesDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::closeEvent(QCloseEvent* e)
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_SuppressFacesDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_SuppressFacesDlg::isValid( QString& msg )
{
  const int objL = myObjects->length(), facesL = myFaces.size();
  return ( objL && objL == facesL );
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_SuppressFacesDlg::execute( ObjectList& objects )
{
  QStringList anErrorObjNames;
  for ( int i = 0; i < myObjects->length(); i++ )
  {
    GEOM::GEOM_Object_var obj = myObjects[i];
    GEOM::short_array faces = myFaces[i];
    //MESSAGE(">>>> Dlg, passing faces.. len = " << faces.length());
    GEOM::GEOM_Object_var anObj = GEOM::GEOM_IHealingOperations::_narrow( getOperation() )->SuppressFaces( obj, faces );
    if ( anObj->_is_nil() )
      anErrorObjNames << GEOMBase::GetName( obj );
    else
      objects.push_back( anObj._retn() );
  }

  if ( !anErrorObjNames.empty() )
    MESSAGE("ERRORS occured while processing the following objects: " << anErrorObjNames.join( " " ));

  return anErrorObjNames.size() < myObjects->length(); // true if at least one object was OK, false if ALL objects were nil after Healing.
}

//=================================================================================
// function : initSelection
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::initSelection()
{
  GEOM::GEOM_Object_var aNullGeomObject;
  localSelection( aNullGeomObject, TopAbs_FACE ); // load local selection on ALL objects
}
