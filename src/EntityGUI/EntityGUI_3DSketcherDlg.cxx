// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// File   : EntityGUI_3DSketcherDlg.cxx
// Author : DMV, OCN
//

#include "EntityGUI_3DSketcherDlg.h"
#include "EntityGUI_Widgets.h"
#include <QtxDoubleSpinBox.h>

#include <GEOMBase.h>
#include <GeometryGUI.h>
#include <GEOMImpl_Types.hxx>
#include <Precision.hxx>  

#include <DlgRef.h>
#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

class Locker
{
public:
  Locker( bool& l ) : myLock( l ) { myLock = true;  }
  ~Locker()                       { myLock = false; }
private:
  bool& myLock;
};

//=================================================================================
// class    : EntityGUI_3DSketcherDlg()
// purpose  : Constructs a EntityGUI_3DSketcherDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
EntityGUI_3DSketcherDlg::EntityGUI_3DSketcherDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                                  bool modal, Qt::WindowFlags fl,
                                                  const double lineWidth )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl ),
    myMode( -1 ),
    myOK( false ),
    myLineWidth( lineWidth ),
    myGeometryGUI( theGeometryGUI )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_UNDO" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_REDO" ) ) );
  QPixmap image3( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICO_3DSKETCH" ) ) );

  setWindowTitle( tr( "GEOM_3DSKETCHER_TITLE" ) );

  /***************************************************************/

  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_3DSKETCHER" ) );
  mainFrame()->RadioButton1->setIcon( image3 );;
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->close();

  GroupType = new DlgRef_3Radio(centralWidget());
  GroupType->GroupBox1->setTitle(tr("GEOM_COORDINATES_TYPE"));
  GroupType->RadioButton1->setText(tr("GEOM_SKETCHER_ABS"));
  GroupType->RadioButton2->setText(tr("GEOM_SKETCHER_REL"));
  GroupType->RadioButton3->close();
  myTypeGroup = new QButtonGroup( this );
  myTypeGroup->addButton( GroupType->RadioButton1, 0 );
  myTypeGroup->addButton( GroupType->RadioButton2, 1 );

  Group3Spin = new EntityGUI_3Spin( centralWidget() );
  Group3Spin->GroupBox1->setTitle( tr( "GEOM_SKETCHER_VALUES" ) );
  Group3Spin->buttonApply->setText( tr( "GEOM_SKETCHER_APPLY" ) );
  Group3Spin->buttonUndo->setIcon( image1 );
  Group3Spin->buttonRedo->setIcon( image2 );
  Group3Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_X2" ) );
  Group3Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_Y2" ) );
  Group3Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_Z2" ) );

  buttonOk()->setText( tr( "GEOM_BUT_END_SKETCH" ) );
  buttonApply()->setText( tr( "GEOM_BUT_CLOSE_SKETCH" ) );
  
  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupType );
  layout->addWidget( Group3Spin );

  setHelpFileName( "create_3dsketcher_page.html" );

  resize(100,100);
  Init();
}  



//=================================================================================
// function : ~EntityGUI_3DSketcherDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
EntityGUI_3DSketcherDlg::~EntityGUI_3DSketcherDlg()
{
  myGeomGUI->SetActiveDialogBox( 0 );
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::Init()
{
  myOK = false;

  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );

  /* Get setting of step value from file configuration */
  double step = SUIT_Session::session()->resourceMgr()->doubleValue( "Geometry", "SettingsGeomStep", 100.0 );

  /* min, max, step and decimals for spin boxes */
  initSpinBox( Group3Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( Group3Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( Group3Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision" );

  Group3Spin->SpinBox_DX->setValue(0.0);
  Group3Spin->SpinBox_DY->setValue(0.0);
  Group3Spin->SpinBox_DZ->setValue(0.0);

  /* signals and slots connections */
  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  connect( Group3Spin->buttonApply,  SIGNAL( clicked() ), this, SLOT( ClickOnAddPoint() ) );
  connect( Group3Spin->buttonUndo,   SIGNAL( clicked() ), this, SLOT( ClickOnUndo() ) );
  connect( Group3Spin->buttonRedo,   SIGNAL( clicked() ), this, SLOT( ClickOnRedo() ) ) ;

  connect( myTypeGroup, SIGNAL( buttonClicked( int ) ),  this, SLOT( TypeClicked( int ) ) );

  connect( Group3Spin->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group3Spin->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group3Spin->SpinBox_DZ, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  initName( tr( "GEOM_3DSKETCHER" ) );

  UpdateButtonsState();
  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
}

//=================================================================================
// function : TypeClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_3DSketcherDlg::TypeClicked( int mode )
{
  if ( mode == myMode ) return;

  bool blocked = Group3Spin->SpinBox_DX->signalsBlocked();
  Group3Spin->SpinBox_DX->blockSignals(true);
  Group3Spin->SpinBox_DY->blockSignals(true);
  Group3Spin->SpinBox_DZ->blockSignals(true);
  // Get setting of step value from file configuration
  XYZ xyz = getLastPoint();
  bool okx, oky, okz;
  Group3Spin->SpinBox_DX->text().toDouble( &okx );
  Group3Spin->SpinBox_DY->text().toDouble( &oky );
  Group3Spin->SpinBox_DZ->text().toDouble( &okz );
  if ( mode == 0 ) {  // XY
    Group3Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_X2" ) );
    Group3Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_Y2" ) );
    Group3Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_Z2" ) );
    if ( okx ) Group3Spin->SpinBox_DX->setValue( xyz.x + Group3Spin->SpinBox_DX->value() );
    if ( oky ) Group3Spin->SpinBox_DY->setValue( xyz.y + Group3Spin->SpinBox_DY->value() );
    if ( okz ) Group3Spin->SpinBox_DZ->setValue( xyz.z + Group3Spin->SpinBox_DZ->value() );
    Group3Spin->buttonApply->setFocus();
  }
  else { // DXDY
    Group3Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_DX2" ) );
    Group3Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_DY2" ) );
    Group3Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_DZ2" ) );
    if ( okx ) Group3Spin->SpinBox_DX->setValue( Group3Spin->SpinBox_DX->value() - xyz.x );
    if ( oky ) Group3Spin->SpinBox_DY->setValue( Group3Spin->SpinBox_DY->value() - xyz.y );
    if ( okz ) Group3Spin->SpinBox_DZ->setValue( Group3Spin->SpinBox_DZ->value() - xyz.z );
    Group3Spin->buttonApply->setFocus();
  }
  Group3Spin->SpinBox_DX->blockSignals(blocked);
  Group3Spin->SpinBox_DY->blockSignals(blocked);
  Group3Spin->SpinBox_DZ->blockSignals(blocked);

  myMode = mode;
}

//=================================================================================
// function : ClickOnAddPoint()
// purpose  : called when the point coordinates is Applyed
//=================================================================================
void EntityGUI_3DSketcherDlg::ClickOnAddPoint()
{
  QString msg;
  if ( !isValid( msg ) ) {
    showError( msg );
    return;
  }

  myPointsList.append( getCurrentPoint() );
  myRedoList.clear();

  if ( myMode == 1 ) {
    Group3Spin->SpinBox_DX->setValue( 0.0 );
    Group3Spin->SpinBox_DY->setValue( 0.0 );
    Group3Spin->SpinBox_DZ->setValue( 0.0 );
  }
  UpdateButtonsState();
  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
}

//=================================================================================
// function : UpdateButtonsState()
// purpose  : 
//=================================================================================
void EntityGUI_3DSketcherDlg::UpdateButtonsState()
{
  if ( myPointsList.count() == 0 ) GroupType->RadioButton1->click();
  GroupType->RadioButton2->setEnabled( myPointsList.count() > 0 );
  Group3Spin->buttonUndo->setEnabled( myPointsList.count() > 0 );
  Group3Spin->buttonRedo->setEnabled( myRedoList.count() > 0 );
}

//=================================================================================
// function : ClickOnUndo()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ClickOnUndo()
{
  if (myPointsList.count() > 0) {
    myRedoList.append( myPointsList.takeLast() );
    UpdateButtonsState();
    GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
  }
}

//=================================================================================
// function : ClickOnRedo()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ClickOnRedo()
{
  if ( myRedoList.count() > 0) {
    myPointsList.append( myRedoList.takeLast() );
    UpdateButtonsState();
    GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
  }
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed
//=================================================================================
void EntityGUI_3DSketcherDlg::SelectionIntoArgument()
{
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  int nbSel = aSelList.Extent();
  if ( nbSel == 1 ) {
    Standard_Boolean aRes = Standard_False;
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), aRes);
    if (!CORBA::is_nil(aSelectedObject) && aRes) {
      TopoDS_Shape aShape;
      if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE)) { // Explore the shape if its a local selection
        TColStd_IndexedMapOfInteger aMap;
        aSelMgr->GetIndexes(aSelList.First(), aMap);
        if (aMap.Extent() == 1)
          {
            int anIndex = aMap(1);
            GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
            aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
          }
      }
      bool isOk = true;
      if ( aShape.ShapeType() != TopAbs_VERTEX )
        isOk = GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_VERTEX);
      if (isOk) {
        gp_Pnt aPnt;
        if ( GEOMBase::VertexToPoint( aShape, aPnt ) ) {
          // set coordinates to the Spin Boxes
          double aX, aY, aZ;
          aX = aPnt.X();
          aY = aPnt.Y();
          aZ = aPnt.Z();
          bool blocked = Group3Spin->SpinBox_DX->signalsBlocked();
          Group3Spin->SpinBox_DX->blockSignals(true);
          Group3Spin->SpinBox_DY->blockSignals(true);
          Group3Spin->SpinBox_DZ->blockSignals(true);
          if ( GroupType->RadioButton1->isChecked() ) {
            Group3Spin->SpinBox_DX->setValue( aX );
            Group3Spin->SpinBox_DY->setValue( aY );
            Group3Spin->SpinBox_DZ->setValue( aZ );
          } else if ( GroupType->RadioButton2->isChecked() ) {
            XYZ xyz = getLastPoint();
            Group3Spin->SpinBox_DX->setValue( aX - xyz.x );
            Group3Spin->SpinBox_DY->setValue( aY - xyz.y );
            Group3Spin->SpinBox_DZ->setValue( aZ - xyz.z );
          }
          Group3Spin->SpinBox_DX->blockSignals(blocked);
          Group3Spin->SpinBox_DY->blockSignals(blocked);
          Group3Spin->SpinBox_DZ->blockSignals(blocked);
        }
      }
    }
  }
  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::DeactivateActiveDialog()
{
  setEnabled( false );
  globalSelection();
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  myGeomGUI->SetActiveDialogBox( 0 );
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ActivateThisDialog()
{
  myGeomGUI->EmitSignalDeactivateDialog();
  setEnabled( true );
  myGeomGUI->SetActiveDialogBox( this );

  connect( myGeomGUI->getApp()->selectionMgr(),
          SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ValueChangedInSpinBox( double newValue )
{
  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr EntityGUI_3DSketcherDlg::createOperation()
{
  return getGeomEngine()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool EntityGUI_3DSketcherDlg::isValid( QString& msg )
{
  bool ok = true;
  ok = Group3Spin->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
  ok = Group3Spin->SpinBox_DY->isValid( msg, !IsPreview() ) && ok;
  ok = Group3Spin->SpinBox_DZ->isValid( msg, !IsPreview() ) && ok;
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool EntityGUI_3DSketcherDlg::execute( ObjectList& objects )
{
  GEOM::ListOfDouble_var aCoordsArray = new GEOM::ListOfDouble;
  if (!myOK || myPointsList.size() == 0)
    aCoordsArray->length((myPointsList.size()+1)*3);
  else
    aCoordsArray->length(myPointsList.size()*3);

  QStringList aParameters;

  int i = 0;
  QList<XYZ>::const_iterator it;
  for(it = myPointsList.begin(); it != myPointsList.end(); ++it ) {
    aCoordsArray[i++] = (*it).x;
    aCoordsArray[i++] = (*it).y;
    aCoordsArray[i++] = (*it).z;
    aParameters << (*it).xt;
    aParameters << (*it).yt;
    aParameters << (*it).zt;
  }

  if (!myOK || myPointsList.size() == 0) {
    XYZ xyz = getCurrentPoint();
    aCoordsArray[i++] = xyz.x;
    aCoordsArray[i++] = xyz.y;
    aCoordsArray[i++] = xyz.z;
    aParameters << xyz.xt;
    aParameters << xyz.yt;
    aParameters << xyz.zt;
  } 

  GEOM::GEOM_ICurvesOperations_var anOper = GEOM::GEOM_ICurvesOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->Make3DSketcher( aCoordsArray );

  if ( !anObj->_is_nil() ) {
    if ( !IsPreview() ) anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    objects.push_back( anObj._retn() );
  }

  return true;
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void EntityGUI_3DSketcherDlg::SetDoubleSpinBoxStep( double step )
{
  Group3Spin->SpinBox_DX->setSingleStep(step);
  Group3Spin->SpinBox_DY->setSingleStep(step);
  Group3Spin->SpinBox_DZ->setSingleStep(step);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ClickOnOk()
{
  Locker lock( myOK );

  if ( !onAccept() )
    return;

  ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool EntityGUI_3DSketcherDlg::ClickOnApply()
{
  QString msg;
  if ( !isValid( msg ) ) {
    showError( msg );
    return false;
  }

  if (myPointsList.count() > 0)
    myPointsList.append(myPointsList[0]);

  Locker lock( myOK );

  if ( !onAccept() )
    return false;

  ClickOnCancel();
  return true;
}

//=================================================================================
// function : getLastPoint()
// purpose  : return last points from list
//=================================================================================
EntityGUI_3DSketcherDlg::XYZ EntityGUI_3DSketcherDlg::getLastPoint() const
{
  return myPointsList.count() > 0 ? myPointsList.last() : XYZ();
}

//=================================================================================
// function : getCurrentPoint()
// purpose  : returns current points
//=================================================================================
EntityGUI_3DSketcherDlg::XYZ EntityGUI_3DSketcherDlg::getCurrentPoint() const
{
  XYZ xyz;
  if ( myMode == 0 ) {
    xyz.x = Group3Spin->SpinBox_DX->value();
    xyz.y = Group3Spin->SpinBox_DY->value();
    xyz.z = Group3Spin->SpinBox_DZ->value();
  } 
  else {
    xyz = getLastPoint();
    xyz.x += Group3Spin->SpinBox_DX->value();
    xyz.y += Group3Spin->SpinBox_DY->value();
    xyz.z += Group3Spin->SpinBox_DZ->value();
  }
  xyz.xt = Group3Spin->SpinBox_DX->text();
  xyz.yt = Group3Spin->SpinBox_DY->text();
  xyz.zt = Group3Spin->SpinBox_DZ->text();
  return xyz;
}

//================================================================
// Function : displayPreview
// Purpose  : Method for displaying preview of resulting shape
//            Redefined from GEOMBase_Helper.
//================================================================
void EntityGUI_3DSketcherDlg::displayPreview( GEOM::GEOM_Object_ptr object,
                                              const bool            append,
                                              const bool            activate,
                                              const bool            update,
                                              const double          lineWidth,
                                              const int             displayMode,
                                              const int             color )
{
  // Set color for preview shape
  getDisplayer()->SetColor( Quantity_NOC_RED );

  // set width of displayed shape
  getDisplayer()->SetWidth( (lineWidth == -1)?myLineWidth:lineWidth ); 

  // Disable activation of selection
  getDisplayer()->SetToActivate( activate );

  // Make a reference to GEOM_Object
  CORBA::String_var objStr = myGeometryGUI->getApp()->orb()->object_to_string( object );
  getDisplayer()->SetName( objStr.in() );

  // Create wire from applied object
  TopoDS_Shape anApplyedWire, aLastSegment;
  if ( !createShapes( object, anApplyedWire, aLastSegment ) )
    return;

  // Build prs
  SALOME_Prs* aPrs = getDisplayer()->BuildPrs( anApplyedWire );
  if ( aPrs != 0 && !aPrs->IsNull() )
    GEOMBase_Helper::displayPreview( aPrs, append, update );

  getDisplayer()->SetColor( Quantity_NOC_VIOLET );
  aPrs = getDisplayer()->BuildPrs( aLastSegment );
  if ( aPrs != 0 && !aPrs->IsNull() )
    GEOMBase_Helper::displayPreview( aPrs, append, update );

  getDisplayer()->UnsetName();

  // Enable activation of displayed objects
  getDisplayer()->SetToActivate( true );
}

//================================================================
// Function : createShapes
// Purpose  : Create applyed wire, and last segment from entry object
//================================================================
bool EntityGUI_3DSketcherDlg::createShapes( GEOM::GEOM_Object_ptr theObject,
                                            TopoDS_Shape&         theApplyedWire,
                                            TopoDS_Shape&         theLastSegment )
{
  TopoDS_Shape aShape;
  if ( !GEOMBase::GetShape( theObject, aShape ) ||
       aShape.ShapeType() != TopAbs_WIRE && aShape.ShapeType() != TopAbs_VERTEX )
    return false;

  theApplyedWire = aShape;
  if ( myOK  )
     return true;

  BRepBuilderAPI_MakeWire aBuilder;
  TopExp_Explorer edgeExp( aShape, TopAbs_EDGE );
  while ( 1 ) {
    TopoDS_Shape anEdge = edgeExp.Current();
    edgeExp.Next();
    if ( edgeExp.More() ) // i.e. non-last edge
      aBuilder.Add( TopoDS::Edge( anEdge ) );
    else {
      theLastSegment = anEdge;
      break;
    }
  }

  if ( aBuilder.IsDone() ) {
    theApplyedWire = aBuilder.Shape();
  }
  else if ( !theLastSegment.IsNull() ) {
    TopExp_Explorer vertexExp( theLastSegment, TopAbs_VERTEX );
    theApplyedWire = vertexExp.Current();
  }

  return true;
}
