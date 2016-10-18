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
// File   : MeasureGUI_PropertiesDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "MeasureGUI_PropertiesDlg.h"
#include "MeasureGUI_Widgets.h"

#include <GEOMImpl_Types.hxx>
#include <GEOMBase.h>
#include <DlgRef.h>

#include <SalomeApp_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <TColStd_MapOfInteger.hxx>

#define DEFAULT_TOLERANCE_VALUE 1.e-6

//=================================================================================
// class    : MeasureGUI_PropertiesDlg()
// purpose  : Constructs a MeasureGUI_PropertiesDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_PropertiesDlg::MeasureGUI_PropertiesDlg( GeometryGUI* GUI, QWidget* parent )
  : MeasureGUI_Skeleton(GUI, parent),
    myTolerance(0),
    myLength(0),
    mySurface(0),
    myVolume(0)
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr( "ICON_DLG_BASICPROPERTIES" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_PROPERTIES_TITLE" ) );

  /***************************************************************/
  
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_PROPERTIES" ) );
  mainFrame()->RadioButton1->setIcon( image0 );

  QGroupBox *aGrpBox  =
          new QGroupBox(tr("GEOM_PROPERTIES_CONSTR"), centralWidget());
  QLabel    *anObjLbl = new QLabel(tr("GEOM_OBJECT"), aGrpBox);
  QLabel    *aTolLbl  = new QLabel(tr("GEOM_TOLERANCE"), aGrpBox);
  QLabel    *aLenLbl  = new QLabel(tr("GEOM_LENGTH"), aGrpBox);
  QLabel    *aSurfLbl = new QLabel(tr("GEOM_PROPERTIES_SURFACE"), aGrpBox);
  QLabel    *aVolLbl  = new QLabel(tr("GEOM_PROPERTIES_VOLUME"), aGrpBox);

  mySelBtn    = new QPushButton(aGrpBox);
  mySelBtn->setIcon(image1);
  mySelEdit   = new QLineEdit(aGrpBox);
  mySelEdit->setReadOnly(true);
  myTolerance = new SalomeApp_DoubleSpinBox(aGrpBox);
  myLength    = new QLineEdit(aGrpBox);
  mySurface   = new QLineEdit(aGrpBox);
  myVolume    = new QLineEdit(aGrpBox);
  myLength->setReadOnly(true);
  mySurface->setReadOnly(true);
  myVolume->setReadOnly(true);

  QGridLayout* aLayout = new QGridLayout(aGrpBox);

  aLayout->setMargin(9);
  aLayout->setSpacing(6);
  aLayout->addWidget(anObjLbl,    0, 0);
  aLayout->addWidget(aTolLbl,     1, 0);
  aLayout->addWidget(aLenLbl,     2, 0);
  aLayout->addWidget(aSurfLbl,    3, 0);
  aLayout->addWidget(aVolLbl,     4, 0);
  aLayout->addWidget(mySelBtn,    0, 1);
  aLayout->addWidget(mySelEdit,   0, 2);
  aLayout->addWidget(myTolerance, 1, 1, 1, 2);
  aLayout->addWidget(myLength,    2, 1, 1, 2);
  aLayout->addWidget(mySurface,   3, 1, 1, 2);
  aLayout->addWidget(myVolume,    4, 1, 1, 2);

  QVBoxLayout* aDlgLayout = new QVBoxLayout( centralWidget() );
  aDlgLayout->setMargin( 0 ); aDlgLayout->setSpacing( 6 );
  aDlgLayout->addWidget(aGrpBox);
  
  /***************************************************************/

  myHelpFileName = "basic_prop_page.html";

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_PropertiesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_PropertiesDlg::~MeasureGUI_PropertiesDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::Init()
{
  mySelEdit->setMinimumSize(100, 0);

  // Obtain precision from preferences
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int aPrecision = resMgr->integerValue( "Geometry", "length_precision", 6 );

  myTolerance->setPrecision(aPrecision);
  // it's necessary to set decimals before the range setting,
  // by default Qt rounds boundaries to 2 decimals at setRange
  myTolerance->setDecimals(qAbs(aPrecision));
  myTolerance->setRange(0., 1.);
  myTolerance->setSingleStep(DEFAULT_TOLERANCE_VALUE);
  myTolerance->setValue(DEFAULT_TOLERANCE_VALUE);

  MeasureGUI_Skeleton::Init();

  connect(myTolerance, SIGNAL(valueChanged(double)),
          this, SLOT(toleranceChanged(double)));
}

//=================================================================================
// function : activateSelection
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::activateSelection()
{
  static TColStd_MapOfInteger aTypes;
  if ( aTypes.IsEmpty() ) {
    aTypes.Add( GEOM_COMPOUND );
    //    aTypes.Add( TopAbs_COMPSOLID );
    aTypes.Add( GEOM_SOLID );
    aTypes.Add( GEOM_SHELL );
    aTypes.Add( GEOM_FACE );
    aTypes.Add( GEOM_WIRE );
    aTypes.Add( GEOM_EDGE );
  }

  globalSelection( aTypes );

  std::list<int> needTypes;
  needTypes.push_back( TopAbs_EDGE ), needTypes.push_back( TopAbs_WIRE ), needTypes.push_back( TopAbs_FACE ), needTypes.push_back( TopAbs_SHELL ), needTypes.push_back( TopAbs_SOLID ), needTypes.push_back( TopAbs_COMPOUND );
  localSelection( needTypes );
}

void MeasureGUI_PropertiesDlg::SelectionIntoArgument()
{
  myObj.nullify();
  QList<TopAbs_ShapeEnum> aTypes;
  aTypes << TopAbs_EDGE << TopAbs_WIRE << TopAbs_FACE << TopAbs_SHELL << TopAbs_SOLID << TopAbs_COMPOUND;
  myObj = getSelected( aTypes );
 
  if (!myObj) {
    mySelEdit->setText("");
    processObject();
    erasePreview();
    return;
  }

  mySelEdit->setText(GEOMBase::GetName(myObj.get()));
  processObject();
  redisplayPreview();
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool MeasureGUI_PropertiesDlg::isValid(QString& msg)
{
  return myTolerance->isValid(msg) && MeasureGUI_Skeleton::isValid(msg);
}

//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::processObject()
{
  double aLength, anArea, aVolume;
  
  if ( !getParameters( aLength, anArea, aVolume ) ) {
    mySelEdit->setText( "" );
    myLength->setText( "" );
    mySurface->setText( "" );
    myVolume->setText( "" );
  }
  else {
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    int aPrecision = resMgr->integerValue( "Geometry", "length_precision", 6 );
    myLength->setText( DlgRef::PrintDoubleValue( aLength, aPrecision ) );
    mySurface->setText( DlgRef::PrintDoubleValue( anArea,  aPrecision ) );
    myVolume->setText( DlgRef::PrintDoubleValue( aVolume, aPrecision ) );
  }
}

//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
bool MeasureGUI_PropertiesDlg::getParameters( double& theLength,
                                              double& theArea,
                                              double& theVolume )
{
  if ( !myObj )
    return false;
  else {
    GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() );
    try {
      anOper->GetBasicProperties( myObj.get(), myTolerance->value(), theLength, theArea, theVolume );
    }
    catch( const SALOME::SALOME_Exception& e ) {
      SalomeApp_Tools::QtCatchCorbaException( e );
      return false;
    }

    return anOper->IsDone();
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
SALOME_Prs* MeasureGUI_PropertiesDlg::buildPrs()
{
  SALOME_Prs* prs = 0;
  TopoDS_Shape shape;

  if ( GEOMBase::GetShape( myObj.get(), shape, TopAbs_EDGE ) &&
       getDisplayer()->IsDisplayed( GEOMBase::GetEntry(myObj.get()) ) ) {
    shape = GEOMBase::CreateArrowForLinearEdge( shape );
    if ( !shape.IsNull() )
      prs = getDisplayer()->BuildPrs( shape );
  }
  return prs;
}

//=================================================================================
// function : toleranceChanged
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::toleranceChanged(double)
{
  processObject();
}
