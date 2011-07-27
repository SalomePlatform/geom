//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : BuildGUI_WireDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "BuildGUI_WireDlg.h"

#include <GEOMImpl_Types.hxx>

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIteratorOfListIO.hxx>

#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <Precision.hxx>

//=================================================================================
// class    : BuildGUI_WireDlg()
// purpose  : Constructs a BuildGUI_WireDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_WireDlg::BuildGUI_WireDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_WIRE" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_WIRE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_WIRE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupType = new DlgRef_3Radio( centralWidget() );
  GroupType->GroupBox1->setTitle( tr( "GEOM_OBJECT_TYPE" ) );
  GroupType->RadioButton1->setText( tr( "GEOM_EDGE" ) );
  GroupType->RadioButton2->setText( tr( "GEOM_WIRE" ) );
  GroupType->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  GroupType->RadioButton3->close();

  GroupArgs = new DlgRef_1Sel1Spin( centralWidget() );
  GroupArgs->GroupBox1->setTitle( tr( "GEOM_WIRE_CONNECT" ) );
  GroupArgs->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupArgs->PushButton1->setIcon( image1 );
  GroupArgs->LineEdit1->setReadOnly( true );

  GroupArgs->TextLabel2->setText( tr( "GEOM_TOLERANCE" ) );
  double SpecificStep = 0.0001;
  double prec = Precision::Confusion();
  initSpinBox(GroupArgs->SpinBox_DX, prec, MAX_NUMBER, SpecificStep, "len_tol_precision" );
  GroupArgs->SpinBox_DX->setValue(prec);

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupType );
  layout->addWidget( GroupArgs );
  /***************************************************************/

  setHelpFileName( "create_wire_page.html" );

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_WireDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_WireDlg::~BuildGUI_WireDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupArgs->LineEdit1;
  GroupArgs->LineEdit1->setReadOnly( true );
  GroupType->RadioButton1->setChecked(true);
  
  myOkEdgesAndWires = false;
  
  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( GroupArgs->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  connect( GroupType->RadioButton1, SIGNAL( clicked() ), this, SLOT( TypeButtonClicked() ) );
  connect( GroupType->RadioButton2, SIGNAL( clicked() ), this, SLOT( TypeButtonClicked() ) );

  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );
  
  initName( tr( "GEOM_WIRE" ) );
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_WireDlg::ClickOnApply()
{
  if ( !onAccept() || !myOkEdgesAndWires )
    return false;

  initName();
  TypeButtonClicked();
  myMapToStudy.clear();
  myEdgesAndWires.length(0);
  myOkEdgesAndWires = false;
  myEditCurrentArgument->setText( "" );
  return true;
}

//=================================================================================
// function : TypeBittonClicked()
// purpose  : Radio button management
//=================================================================================
void BuildGUI_WireDlg::TypeButtonClicked()
{
  if ( GroupType->RadioButton1->isChecked() ) {
    globalSelection(); // close local contexts, if any
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
    GroupArgs->TextLabel1->setText( tr( "GEOM_EDGE" ) );
  }
  else if ( GroupType->RadioButton2->isChecked() ) {
    globalSelection(); // close local contexts, if any
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_WIRE );
    GroupArgs->TextLabel1->setText( tr( "GEOM_WIRE" ) );
  }
  SelectionIntoArgument();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BuildGUI_WireDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  QString aString = ""; /* name of selection */

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  myOkEdgesAndWires = false;
  int nbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aString);

  if ( nbSel == 0 ) {
    myMapToStudy.clear();
    return;
  }

  TopAbs_ShapeEnum aNeedType = TopAbs_EDGE;
  if (GroupType->RadioButton2->isChecked())
    aNeedType = TopAbs_WIRE;

  std::list<GEOM::GEOM_Object_var> aList; // subshapes list
  TopoDS_Shape aShape;
  Standard_Boolean aRes = Standard_False;
  for (SALOME_ListIteratorOfListIO anIt (aSelList); anIt.More(); anIt.Next()) {
    TColStd_IndexedMapOfInteger aMap;
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( anIt.Value(), aRes );
    if ( !CORBA::is_nil(aSelectedObject) && aRes && GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
      aSelMgr->GetIndexes( anIt.Value(), aMap );

      if ( aMap.Extent() > 0 ) { // local selection
        for (int ind = 1; ind <= aMap.Extent(); ind++) {
          aString = aSelectedObject->GetName();
          int anIndex = aMap(ind);
          if ( aNeedType == TopAbs_EDGE )
            aString += QString( ":edge_%1" ).arg( anIndex );
          else
            aString += QString( ":wire_%1" ).arg( anIndex );
          
          //Find SubShape Object in Father
          GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather( aSelectedObject, aString );
          
          if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
            GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
            aList.push_back( aShapesOp->GetSubShape( aSelectedObject, anIndex ) );
            myMapToStudy[aString] = aShapesOp->GetSubShape( aSelectedObject, anIndex );
          }
          else {
            aList.push_back( aFindedObject ); // get Object from study
          }
        }
      } else { // global selection
        if ( aShape.ShapeType() == aNeedType ) {
          GEOMBase::ConvertListOfIOInListOfGO(aSelList,  myEdgesAndWires);
        } else {
          aList.clear();
          myEdgesAndWires.length(0);
        }
      }
    }
  }

  // convert aList in listofgo
  if ( aList.size() ) {
    myEdgesAndWires.length( aList.size()  );
    int k = 0;
    for ( std::list<GEOM::GEOM_Object_var>::iterator j = aList.begin(); j != aList.end(); j++ )
      myEdgesAndWires[k++] = *j;
  }
    
  if ( myEdgesAndWires.length() > 1 )
    aString = tr( "%1_objects" ).arg( myEdgesAndWires.length() );

  if ( !myEdgesAndWires.length() ) {
    aString = "";
    myMapToStudy.clear();
  }

  myEditCurrentArgument->setText( aString );
  myOkEdgesAndWires = true;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if ( send != GroupArgs->PushButton1 )
    return;

  TypeButtonClicked();
  myEditCurrentArgument = GroupArgs->LineEdit1;

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  TypeButtonClicked();
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BuildGUI_WireDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_WireDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_WireDlg::isValid (QString& msg)
{
  bool ok = GroupArgs->SpinBox_DX->isValid(msg, !IsPreview());
  return myOkEdgesAndWires && ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_WireDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );
  GEOM::GEOM_Object_var anObj = anOper->MakeWire(myEdgesAndWires, GroupArgs->SpinBox_DX->value());

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BuildGUI_WireDlg::addSubshapesToStudy()
{
  addSubshapesToFather( myMapToStudy );
}
