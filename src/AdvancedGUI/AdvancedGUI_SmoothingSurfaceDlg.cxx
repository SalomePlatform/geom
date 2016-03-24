// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "AdvancedGUI_SmoothingSurfaceDlg.h"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(AdvancedGEOM)

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// OCCT Includes
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// Constructor
//=================================================================================
AdvancedGUI_SmoothingSurfaceDlg::AdvancedGUI_SmoothingSurfaceDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false),
    myNbMaxSpin(0),
    myDegMaxSpin(0),
    myDMaxSpin(0)
{
  QPixmap imageOp  (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_SMOOTHINGSURFACE_LPOINTS")));
  QPixmap imageSel (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle(tr("GEOM_SMOOTHINGSURFACE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_SMOOTHINGSURFACE"));
  mainFrame()->RadioButton1->setIcon(imageOp);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1Sel( centralWidget() );

  GroupPoints->GroupBox1->setTitle( tr( "GEOM_SMOOTHINGSURFACE_ARG" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_SMOOTHINGSURFACE_ARG_POINTS" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->LineEdit1->setReadOnly( true );

  QLabel *aNbMax  = new QLabel(tr("GEOM_SMOOTHINGSURFACE_ARG_NB_MAX"));
  QLabel *aDegMax = new QLabel(tr("GEOM_SMOOTHINGSURFACE_ARG_DEG_MAX"));
  QLabel *aDMax   = new QLabel(tr("GEOM_SMOOTHINGSURFACE_ARG_D_MAX"));

  myNbMaxSpin  = new SalomeApp_IntSpinBox(0, 1000, 1, 0, true, true);
  myDegMaxSpin = new SalomeApp_IntSpinBox(0, 1000, 1, 0, true, true);
  myDMaxSpin   = new SalomeApp_DoubleSpinBox;
  GroupPoints->gridLayout1->addWidget(aNbMax,       1, 0);
  GroupPoints->gridLayout1->addWidget(aDegMax,      2, 0);
  GroupPoints->gridLayout1->addWidget(aDMax,        3, 0);
  GroupPoints->gridLayout1->addWidget(myNbMaxSpin,  1, 1, 1, 2);
  GroupPoints->gridLayout1->addWidget(myDegMaxSpin, 2, 1, 1, 2);
  GroupPoints->gridLayout1->addWidget(myDMaxSpin,   3, 1, 1, 2);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("create_smoothingsurface_page.html");

  Init();
}

//=================================================================================
// Destructor
//=================================================================================
AdvancedGUI_SmoothingSurfaceDlg::~AdvancedGUI_SmoothingSurfaceDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void AdvancedGUI_SmoothingSurfaceDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  initSpinBox(myDMaxSpin, 0., COORD_MAX, 0.00001, "parametric_precision" );

  myNbMaxSpin->setValue(2);
  myDegMaxSpin->setValue(8);
  myDMaxSpin->setValue(0.);

  showOnlyPreviewControl();

  globalSelection();
  localSelection( TopAbs_VERTEX );
  //@@ initialize dialog box widgets here @@//

  // Signal/slot connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(myGeomGUI,     SIGNAL(SignalDefaultStepValueChanged(double)),
          this,          SLOT(SetDoubleSpinBoxStep(double)));
  connect( myGeomGUI->getApp()->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  connect(myNbMaxSpin, SIGNAL(valueChanged(const QString&)),
          this, SLOT(processPreview()) );
  connect(myDegMaxSpin, SIGNAL(valueChanged(const QString&)),
          this, SLOT(processPreview()) );
  connect(myDMaxSpin, SIGNAL(valueChanged(const QString&)),
          this, SLOT(processPreview()) );
 
  initName(tr("GEOM_SMOOTHINGSURFACE"));
  //displayPreview();
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void AdvancedGUI_SmoothingSurfaceDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool AdvancedGUI_SmoothingSurfaceDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  globalSelection();
  localSelection( TopAbs_VERTEX );
  return true;
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void AdvancedGUI_SmoothingSurfaceDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection();
  localSelection( TopAbs_VERTEX );
  //displayPreview();
}

//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void AdvancedGUI_SmoothingSurfaceDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr AdvancedGUI_SmoothingSurfaceDlg::createOperation()
{
  return getGeomEngine()->GetPluginOperations(getStudyId(), "AdvancedEngine");
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool AdvancedGUI_SmoothingSurfaceDlg::isValid (QString& msg)
{
  if (myPoints.empty()) {
    msg += tr("GEOM_SMOOTHINGSURFACE_NO_POINTS");
    return false;
  }

  bool ok = myNbMaxSpin->isValid (msg, !IsPreview()) &&
            myDegMaxSpin->isValid(msg, !IsPreview()) &&
            myDMaxSpin->isValid  (msg, !IsPreview());

  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool AdvancedGUI_SmoothingSurfaceDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::IAdvancedOperations_var anOper = GEOM::IAdvancedOperations::_narrow(getOperation());

  //@@ retrieve input values from the widgets here @@//
  GEOM::ListOfGO_var points = new GEOM::ListOfGO();
  points->length( myPoints.count() );
  for ( int i = 0; i < myPoints.count(); i++ )
    points[i] = myPoints[i].copy();

  const int    aNbMax  = myNbMaxSpin->value();
  const int    aDegMax = myDegMaxSpin->value();
  const double aDMax   = myDMaxSpin->value();

  // call engine function
  anObj = anOper->MakeSmoothingSurface(points, aNbMax, aDegMax, aDMax);
  res = !anObj->_is_nil();
  if (res && !IsPreview())
  {
    QStringList aParameters;
    //@@ put stringified input parameters to the string list here to store in the data model for notebook @@//
    if ( aParameters.count() > 0 ) anObj->SetParameters(aParameters.join(":").toLatin1().constData());
  }
  
  if (res)
    objects.push_back(anObj._retn());

  return res;
}

//=================================================================================
// function : addSubshapesToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void AdvancedGUI_SmoothingSurfaceDlg::addSubshapesToStudy()
{
  for ( int i = 0; i < myPoints.count(); i++ )
    GEOMBase::PublishSubObject( myPoints[i].get() );
}


//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> AdvancedGUI_SmoothingSurfaceDlg::getSourceObjects()
{
  return myPoints;
}

//=================================================================================
// function : getNbPoints()
// purpose  : Returns the number of points in myPoints list.
//=================================================================================
int AdvancedGUI_SmoothingSurfaceDlg::getNbPoints() const
{
  TopTools_IndexedMapOfShape aMap;

  foreach (GEOM::GeomObjPtr anObj, myPoints) {
    TopoDS_Shape aShape;

    if(anObj && GEOMBase::GetShape(anObj.get(), aShape) && !aShape.IsNull()) {
      if (aShape.ShapeType() == TopAbs_VERTEX) {
        aMap.Add(aShape);
      } else {
        // Compound.
        TopExp::MapShapes(aShape, TopAbs_VERTEX, aMap);
      }
    }
  }

  const int aNbPoints = aMap.Extent();

  return aNbPoints;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void AdvancedGUI_SmoothingSurfaceDlg::SelectionIntoArgument()
{
  QList<TopAbs_ShapeEnum> aTypes;

  aTypes << TopAbs_VERTEX << TopAbs_COMPOUND;

  QList<GEOM::GeomObjPtr> points = getSelected( aTypes, -1 );

  // Check the selected compounds if they consist of points only.
  foreach (GEOM::GeomObjPtr anObj, points) {
    TopoDS_Shape aShape;

    if(anObj && GEOMBase::GetShape(anObj.get(), aShape) && !aShape.IsNull()) {
      if (aShape.ShapeType() == TopAbs_COMPOUND) {
        // Check if the compound contains vertices only.
        TopoDS_Iterator anIter(aShape);
        Standard_Boolean isValid = Standard_False;

        for (; anIter.More(); anIter.Next()) {
          const TopoDS_Shape &aSubShape = anIter.Value();

          if (aSubShape.ShapeType() == TopAbs_VERTEX) {
            isValid = Standard_True;
          } else {
            isValid = Standard_False;
            break;
          }
        }

        if (!isValid) {
          points.clear();
          break;
        }
      }
    } else {
      // NEVERREACHED
      points.clear();
      break;
    }
  }

  GEOMBase::Synchronize( myPoints, points );
  if ( !myPoints.isEmpty()  )
    GroupPoints->LineEdit1->setText( QString::number( getNbPoints() ) + "_" + tr( "GEOM_POINT" ) + tr( "_S_" ) );
  else
    GroupPoints->LineEdit1->setText( "" );
  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void AdvancedGUI_SmoothingSurfaceDlg::SetEditCurrentArgument()
{
  if ( sender() == GroupPoints->PushButton1 )
    myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setFocus();
  globalSelection();
  localSelection( TopAbs_VERTEX );
  SelectionIntoArgument();
}
