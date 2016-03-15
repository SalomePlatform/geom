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

// GEOM GEOMGUI : GUI for Geometry component
// File   : RepairGUI_FreeFacesDlg.cxx
// Author : Vladimir KLYACHIN, Open CASCADE S.A.S. (vladimir.klyachin@opencascade.com)
//
#include "RepairGUI_FreeFacesDlg.h"

#include <GEOMBase.h>
#include <GeometryGUI.h>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Constants.h>

#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Tools.h>
#include <SUIT_Desktop.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>

#include <TColStd_MapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>


#define SPACING 6
#define MARGIN  9
#define MIN_WIDTH 200

//=================================================================================
// class    : RepairGUI_FreeFacesDlg()
// purpose  : Constructs a RepairGUI_FreeFacesDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_FreeFacesDlg::RepairGUI_FreeFacesDlg( GeometryGUI* GUI, QWidget* parent,
                                                bool modal)
  : GEOMBase_Skeleton(GUI, parent, modal),
    myDisplayer      (0),
    myEdit           (0)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_FREE_FACES")));
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_FREE_FACES_TITLE" ) );

  /***************************************************************/

  mainFrame()->GroupConstructors->setTitle(tr("GEOM_FREE_FACES"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  mainFrame()->GroupBoxName->hide();

  QGroupBox* aMainGrp = new QGroupBox( tr( "GEOM_SELECTED_SHAPE" ), this );
  
  QLabel* lab = new QLabel( tr( "GEOM_OBJECT" ), aMainGrp );
  QPushButton *aSelBtn = new QPushButton( aMainGrp );
  aSelBtn->setIcon( image1 );
  myEdit = new QLineEdit( aMainGrp );
  myEdit->setReadOnly( true );
  myEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myEdit->setMinimumWidth( MIN_WIDTH );

  QHBoxLayout* aMainLay = new QHBoxLayout( aMainGrp );
  aMainLay->setSpacing( SPACING );
  aMainLay->setMargin( MARGIN );
  aMainLay->addWidget( lab );
  aMainLay->addWidget( aSelBtn );
  aMainLay->addWidget( myEdit );

  QVBoxLayout* aLay = new QVBoxLayout (centralWidget());
  aLay->setSpacing(SPACING);
  aLay->setMargin(MARGIN);
  aLay->addWidget(aMainGrp);

  resize(minimumSizeHint());
  /***************************************************************/

  myHelpFileName = "free_faces_page.html";

  Init();
}


//=================================================================================
// function : ~RepairGUI_FreeFacesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_FreeFacesDlg::~RepairGUI_FreeFacesDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_FreeFacesDlg::Init()
{
  myObj = GEOM::GEOM_Object::_nil();
  myEditCurrentArgument = myEdit;

  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_FREE_FACES_NAME"));
  buttonOk()->setEnabled(false);
  buttonApply()->setEnabled(false);
  activateSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void RepairGUI_FreeFacesDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_FreeFacesDlg::ClickOnApply()
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
void RepairGUI_FreeFacesDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  activateSelection();
  displayPreview(true);
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void RepairGUI_FreeFacesDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  : SLOT. Called when selection changed.
//=================================================================================
void RepairGUI_FreeFacesDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  myObj = GEOM::GEOM_Object::_nil();
  erasePreview();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if ( aSelList.Extent() != 1 ) {
    buttonOk()->setEnabled(false);
    buttonApply()->setEnabled(false);
    return;
  }

  GEOM::GEOM_Object_var anObj =
    GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

  if ( !GEOMBase::IsShape( anObj ) ) {
    buttonOk()->setEnabled(false);
    buttonApply()->setEnabled(false);
    return;
  } else {
    myObj = anObj;
    myEditCurrentArgument->setText(GEOMBase::GetName(myObj));
    displayPreview( true, false, true, true, 3 );
  }
}

//=================================================================================
// function : activateSelection
// purpose  : activate selection of solids
//=================================================================================
void RepairGUI_FreeFacesDlg::activateSelection()
{
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_SOLID );
  aMap.Add( GEOM_COMPOUND );
  globalSelection( aMap );
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_FreeFacesDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_FreeFacesDlg::isValid( QString& )
{
  return !myObj->_is_nil() ;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_FreeFacesDlg::execute( ObjectList& objects )
{
  bool aResult = false;
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );
  GEOM::ListOfLong_var aFaceLst = anOper->GetFreeFacesIDs( myObj );
  TopoDS_Shape aSelShape;
  TopoDS_Shape aFace; 
  TopTools_IndexedMapOfShape anIndices;
  int aNbObj = 0;

  if ( !myObj->_is_nil() && GEOMBase::GetShape( myObj, aSelShape ) ) {
    myEdit->setText( GEOMBase::GetName( myObj ) );
    QString aMess;
    if ( !isValid( aMess ) ) {
      erasePreview( true );
      buttonOk()->setEnabled(false);
      buttonApply()->setEnabled(false);
      return false;
    }
    
    SUIT_OverrideCursor wc;

    TopExp::MapShapes( aSelShape, anIndices);
    SALOME_Prs* aPrs = 0;
    
    //Get object trancparency and set it to preview
    SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
    double transparency = 1.0;
    if( app ) {
      SUIT_ViewWindow* window = app->desktop( )->activeWindow( );
      if( window && window->getViewManager() ) {
	if ( app ) {
	  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
	  if( aStudy ) {
	    int aMgrId = window->getViewManager()->getGlobalId();
	    CORBA::String_var aMainEntry = myObj->GetStudyEntry();
	    QString anEntry = aMainEntry.in();
	    QVariant v = aStudy->getObjectProperty( aMgrId , anEntry , GEOM::propertyName( GEOM::Transparency ) , transparency );
	    if( v.canConvert( QVariant::Double ) ) {
	      transparency = v.toDouble();
	    }
	  }
	}
      }
    }

    int i;
    int n = aFaceLst->length();

    for (i = 0; i < n; i++ ) {
      aFace = anIndices.FindKey( aFaceLst[i] );
      try {
        getDisplayer()->SetColor( Quantity_NOC_RED );
	getDisplayer()->SetTransparency( transparency );
	getDisplayer()->SetWidth( 2 );
	getDisplayer()->SetNbIsos( 1 );
	getDisplayer()->SetIsosWidth( 1 );
	getDisplayer()->SetIsosColor( Quantity_NOC_RED );
	getDisplayer()->SetDisplayMode( 0 );
        getDisplayer()->SetToActivate( false );
        aPrs = !aFace.IsNull() ? getDisplayer()->BuildPrs( aFace ) : 0;
        if ( aPrs )
          displayPreview( aPrs, true );
      }
      catch( const SALOME::SALOME_Exception& e )
      {
        SalomeApp_Tools::QtCatchCorbaException( e );
        buttonOk()->setEnabled(false);
        buttonApply()->setEnabled(false);
        return false;
      }
    }

    // Create sub-objects
    GEOM::ListOfGO_var aList = anOper->MakeSubShapes(myObj, aFaceLst);

    aNbObj = aList->length();

    for (i = 0; i < aNbObj; i++) {
      objects.push_back(GEOM::GEOM_Object::_duplicate(aList[i]));
    }

    aResult = true;
  }

  buttonOk()->setEnabled(aNbObj > 0);
  buttonApply()->setEnabled(aNbObj > 0);

  return aResult;
}

//================================================================
// Function : getDisplayer
// Purpose  :
//================================================================
GEOM_Displayer* RepairGUI_FreeFacesDlg::getDisplayer()
{
  if ( !myDisplayer )
    myDisplayer = new GEOM_Displayer( getStudy() );
  return myDisplayer;
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            (called with addInStudy method)
//================================================================
GEOM::GEOM_Object_ptr RepairGUI_FreeFacesDlg::getFather (GEOM::GEOM_Object_ptr)
{
  return myObj;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_FreeFacesDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myObj);
  res << aGeomObjPtr;
  return res;
}
