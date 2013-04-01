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
// File   : MeasureGUI_CenterMassDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "MeasureGUI_CenterMassDlg.h"
#include "MeasureGUI_Widgets.h"

#include <DlgRef.h>
#include <GEOMBase.h>

#include <GeometryGUI.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Tools.h>

#include <BRep_Tool.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <gp_Pnt.hxx>

//=================================================================================
// class    : MeasureGUI_CenterMassDlg()
// purpose  : Constructs a MeasureGUI_CenterMassDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_CenterMassDlg::MeasureGUI_CenterMassDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent, false )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_CENTERMASS" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_CMASS_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_CMASS" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  myGrp = new MeasureGUI_1Sel3LineEdit( centralWidget() );
  myGrp->GroupBox1->setTitle( tr( "GEOM_CENTER" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->TextLabel2->setText( tr( "GEOM_X" ) );
  myGrp->TextLabel3->setText( tr( "GEOM_Y" ) );
  myGrp->TextLabel4->setText( tr( "GEOM_Z" ) );
  myGrp->LineEdit2->setReadOnly( true );
  myGrp->LineEdit3->setReadOnly( true );
  myGrp->LineEdit4->setReadOnly( true );
  myGrp->PushButton1->setIcon( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp );

  /***************************************************************/

  myHelpFileName = "center_mass_page.html";

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_CenterMassDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_CenterMassDlg::~MeasureGUI_CenterMassDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = myGrp->LineEdit1;

   /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( myGrp->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( myGrp->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_POINT") );
  globalSelection();
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool MeasureGUI_CenterMassDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  return true;

//    myMeasureGUI->MakeCDGAndDisplay( myGeomShape );
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void MeasureGUI_CenterMassDlg::SelectionIntoArgument()
{
  erasePreview();
  myObj = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    processObject();
    return;
  }

  GEOM::GEOM_Object_var aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

  if ( aSelectedObject->_is_nil() ) {
    processObject();
    return;
  }

  myObj = aSelectedObject;
  processObject();
  displayPreview(true);
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::SetEditCurrentArgument()
{
  myGrp->LineEdit1->setFocus();
  myEditCurrentArgument = myGrp->LineEdit1;
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::LineEditReturnPressed()
{
  QLineEdit* send = ( QLineEdit* )sender();
  if ( send == myGrp->LineEdit1 ) {
    myEditCurrentArgument = myGrp->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
           SIGNAL(currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  globalSelection();
  displayPreview(true);
}

//=================================================================================
// function : processObject()
// purpose  : Fill dialog fields in accordance with myObj
//=================================================================================
void MeasureGUI_CenterMassDlg::processObject()
{
  if ( myObj->_is_nil() ) {
    myGrp->LineEdit1->setText( "" );
    myGrp->LineEdit2->setText( "" );
    myGrp->LineEdit3->setText( "" );
    myGrp->LineEdit4->setText( "" );
    erasePreview();
  }
  else {
    double x = 0, y = 0, z = 0;
    
    getParameters( x, y, z );
    
    myGrp->LineEdit1->setText( GEOMBase::GetName( myObj ) );

    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    int aPrecision = resMgr->integerValue( "Geometry", "length_precision", 6 );
    myGrp->LineEdit2->setText( DlgRef::PrintDoubleValue( x, aPrecision ) );
    myGrp->LineEdit3->setText( DlgRef::PrintDoubleValue( y, aPrecision ) );
    myGrp->LineEdit4->setText( DlgRef::PrintDoubleValue( z, aPrecision ) );

    displayPreview(true);
  }
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_CenterMassDlg::createOperation()
{
  return getGeomEngine()->GetIMeasureOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool MeasureGUI_CenterMassDlg::isValid( QString& )
{
  return !myObj->_is_nil();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool MeasureGUI_CenterMassDlg::getParameters( double& theX, double& theY, double& theZ )
{
  if ( myObj->_is_nil() )
    return false;
  else {
    try {
      GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() );
      GEOM::GEOM_Object_var anObj = anOper->GetCentreOfMass( myObj );
      if ( !anOper->IsDone() )
        return false;

      TopoDS_Shape aShape;
      if ( !GEOMBase::GetShape( anObj, aShape ) ||
           aShape.IsNull() ||
           aShape.ShapeType() != TopAbs_VERTEX )
        return false;

      TopoDS_Vertex aVertex = TopoDS::Vertex( aShape );

      gp_Pnt aPnt = BRep_Tool::Pnt( aVertex );

      theX = aPnt.X();
      theY = aPnt.Y();
      theZ = aPnt.Z();

      return true;
    }
    catch( const SALOME::SALOME_Exception& e ) {
      SalomeApp_Tools::QtCatchCorbaException( e );
      return false;
    }
  }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool MeasureGUI_CenterMassDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() );
  GEOM::GEOM_Object_var anObj = anOper->GetCentreOfMass( myObj );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}
