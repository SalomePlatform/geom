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
// File   : GEOMBase_Skeleton.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.

#include "GEOMBase_Skeleton.h"
#include "GEOMBase_DlgSkeleton.h"
#include "GEOMBase.h"

#include <DlgRef.h>
#include <GeometryGUI.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_DoubleSpinBox.h>
#include <SalomeApp_Study.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>

#include <QKeyEvent>
#include <QSpinBox>
#include <QDoubleSpinBox>

//=================================================================================
// class    : GEOMBase_Skeleton()
// purpose  : Constructs a GEOMBase_Skeleton which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
GEOMBase_Skeleton::GEOMBase_Skeleton( GeometryGUI* theGeometryGUI, QWidget* parent,
                                      bool modal, Qt::WindowFlags fl )
  : QDialog( parent, fl ), 
    GEOMBase_Helper( dynamic_cast<SUIT_Desktop*>( parent ) ),
    myGeomGUI( theGeometryGUI ),
    myRBGroup( 0 )
{
  setAttribute( Qt::WA_DeleteOnClose );

  setModal( modal );

  myMainFrame = new GEOMBase_DlgSkeleton( this );
  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 0 ); topLayout->setSpacing( 0 );
  topLayout->addWidget( myMainFrame );

  myMainFrame->GroupBoxName->setTitle( tr( "GEOM_RESULT_NAME_GRP" ) );
  myMainFrame->NameLabel->setText( tr( "GEOM_RESULT_NAME_LBL" ) );

  myMainFrame->GroupBoxPublish->setTitle( tr( "GEOM_PUBLISH_RESULT_GRP" ) );
  myMainFrame->CheckBoxRestoreSS->setText( tr( "GEOM_RESTORE_SUB_SHAPES" ) );
  myMainFrame->CheckBoxAddPrefix->setText( tr( "GEOM_RSS_ADD_FREFIX" ) );
  myMainFrame->CheckBoxPreview->setText( tr("GEOM_PREVIEW") );

  buttonCancel()->setText( tr( "GEOM_BUT_CLOSE" ) );
  buttonOk()->setText( tr( "GEOM_BUT_APPLY_AND_CLOSE" ) );
  buttonApply()->setText( tr( "GEOM_BUT_APPLY" ) );
  buttonHelp()->setText( tr( "GEOM_BUT_HELP" ) );

  myRBGroup = new QButtonGroup( this );
  myRBGroup->addButton( myMainFrame->RadioButton1, 0 );
  myRBGroup->addButton( myMainFrame->RadioButton2, 1 );
  myRBGroup->addButton( myMainFrame->RadioButton3, 2 );
  myRBGroup->addButton( myMainFrame->RadioButton4, 3 );
  myRBGroup->addButton( myMainFrame->RadioButton5, 4 );

  connect( myRBGroup, SIGNAL( buttonClicked( int ) ), this, SIGNAL( constructorsClicked( int ) ) );

  Init();
}

//=================================================================================
// function : ~GEOMBase_Skeleton()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GEOMBase_Skeleton::~GEOMBase_Skeleton()
{
  if ( myGeomGUI )
    myGeomGUI->SetActiveDialogBox( 0 );
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::Init()
{
  SalomeApp_Application* app = (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() );
  if ( !myGeomGUI && app )
    myGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );

  /* init variables */
  if ( myGeomGUI )
    myGeomGUI->SetActiveDialogBox( this );

  /* signals and slots connections */
  connect( buttonCancel(), SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) );
  if ( myGeomGUI ) {
    connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
    connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );
  }

  // connect help button on a private slot that displays help information
  connect( buttonHelp(), SIGNAL( clicked() ), this, SLOT( ClickOnHelp() ) );

  connect( myMainFrame->CheckBoxPreview, SIGNAL(toggled(bool)), this, SLOT(processPreview()) );

  /* displays Dialog */
  myMainFrame->RadioButton1->setChecked( true );
  myMainFrame->RadioButton4->hide();
  myMainFrame->RadioButton5->hide();

  myMainFrame->CheckBoxRestoreSS->setChecked( false );
  myMainFrame->CheckBoxAddPrefix->setChecked( true );

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  bool aPrv = (resMgr == 0) ? false : resMgr->booleanValue( "Geometry", "geom_preview", false );
  
  myMainFrame->CheckBoxPreview->setChecked( aPrv );
  myMainFrame->GroupBoxPublish->hide();
}

//=================================================================================
// function : initSpinBox()
// purpose  : 
//=================================================================================
void GEOMBase_Skeleton::initSpinBox( QSpinBox* spinBox, 
                                     int min,  int max, int step )
{
  spinBox->setRange( min, max );
  spinBox->setSingleStep( step );
}

//=================================================================================
// function : initSpinBox()
// purpose  : 
//=================================================================================
void GEOMBase_Skeleton::initSpinBox( SalomeApp_DoubleSpinBox* spinBox, 
                                     double min,  double max, 
                                     double step, const char* quantity )
{
  // Obtain precision from preferences
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int aPrecision = resMgr->integerValue( "Geometry", quantity, 6 );
  
  spinBox->setPrecision( aPrecision );
  spinBox->setDecimals( qAbs( aPrecision ) ); // it's necessary to set decimals before the range setting,
                                    // by default Qt rounds boundaries to 2 decimals at setRange
  spinBox->setRange( min, max );
  spinBox->setSingleStep( step );
  
  // Add a hint for the user saying how to tune precision
  QString userPropName = QObject::tr( QString( "GEOM_PREF_%1" ).arg( quantity ).toLatin1().constData() );
  spinBox->setProperty( "validity_tune_hint", 
                        QVariant( QObject::tr( "GEOM_PRECISION_HINT" ).arg( userPropName ) ) );
}

//=================================================================================
// function : updateAttributes()
// purpose  : Workaround for Translation and Rotation operations with unchecked option "Create a copy".
//            In this case PublishInStudy isn't called, so we need to update object's attributes manually
//=================================================================================
void GEOMBase_Skeleton::updateAttributes( GEOM::GEOM_Object_ptr theObj,
                                          const QStringList& theParameters)
{
  SALOMEDS::Study_var aStudy = GeometryGUI::ClientStudyToStudy(getStudy()->studyDS());
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
  SALOMEDS::SObject_var aSObject = aStudy->FindObjectID(theObj->GetStudyEntry());
  SALOMEDS::GenericAttribute_var anAttr = aStudyBuilder->FindOrCreateAttribute(aSObject, "AttributeString");
  SALOMEDS::AttributeString_var aStringAttrib = SALOMEDS::AttributeString::_narrow(anAttr);

  std::string aValue = aStringAttrib->Value();
  if( aValue != "" )
    aValue += "|";
  for( int i = 0, n = theParameters.count(); i < n; i++ ) {
    std::string aParameter = theParameters[i].toStdString();
    if(aStudy->IsVariable(aParameter.c_str()))
      aValue += aParameter;
    if(i != n-1)
      aValue += ":";
  }
  aStringAttrib->SetValue(aValue.c_str());
}

//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::ClickOnCancel()
{
  close();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::LineEditReturnPressed()
{
  if (!myEditCurrentArgument)
    return;

  /* User name of object input management                          */
  /* If successfull the selection is changed and signal emitted... */
  /* so SelectionIntoArgument() is automatically called.           */
  const QString objectUserName = myEditCurrentArgument->text();
  QWidget* thisWidget = (QWidget*)this;

  SALOME_ListIO aList;
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  if (aSelMgr)
    aSelMgr->selectedObjects(aList);
  if (GEOMBase::SelectionByNameInDialogs(thisWidget, objectUserName, aList))
    myEditCurrentArgument->setText(objectUserName);
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::DeactivateActiveDialog()
{
  this->setEnabled( false );
  globalSelection();
  if ( myGeomGUI ) {
    myGeomGUI->SetActiveDialogBox( 0 );
    disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  }
  erasePreview();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  if ( myGeomGUI ) myGeomGUI->EmitSignalDeactivateDialog();
  this->setEnabled( true );
  if ( myGeomGUI ) myGeomGUI->SetActiveDialogBox( (QDialog*)this );
  return;
}

//=================================================================================
// function : closeEvent()
// purpose  : same than click on cancel button
//=================================================================================
void GEOMBase_Skeleton::closeEvent( QCloseEvent* e )
{
  if ( myGeomGUI ) {
    disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
    myGeomGUI->getApp()->updateActions();
  }
  QDialog::closeEvent( e );
}

//=================================================================================
// function : initName()
// purpose  : initialize the Name field with a string "thePrefix_X" (Vertex_3)
//=================================================================================
void GEOMBase_Skeleton::initName( const QString& thePrefix )
{
  if ( !thePrefix.isNull() )
    setPrefix( thePrefix );
  myMainFrame->ResultName->setText( GEOMBase::GetDefaultName( getPrefix() ) );
}

//=================================================================================
// function : getNewObjectName()
// purpose  : returns contents of Name field
//=================================================================================
QString GEOMBase_Skeleton::getNewObjectName (int) const
{
  return myMainFrame->ResultName->text();
}

//=================================================================================
// function : getConstructorId()
// purpose  :
//=================================================================================
int GEOMBase_Skeleton::getConstructorId() const
{
  if ( myRBGroup )
    return myRBGroup->checkedId();
  return -1;
}

//=================================================================================
// function : setConstructorId( id )
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::setConstructorId( const int id )
{
  if ( myRBGroup && myRBGroup->button( id ) )
    myRBGroup->button( id )->setChecked( true );
}

//=================================================================================
// function : unsetConstructorId
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::unsetConstructorId()
{
  // 0020428: EDF 906 GEOM : Performance for Group creation in GEOM
  // uncheck all buttons
  // workaround, because setChecked( false ) does not result in Qt4
  bool isExclusive = myRBGroup->exclusive();
  myRBGroup->setExclusive( false );
  QList<QAbstractButton*> btnList = myRBGroup->buttons();
  for ( int j = 0; j < 2; j++ )
  {
    QList<QAbstractButton*>::const_iterator it = btnList.constBegin();
    for ( ; it != btnList.constEnd(); ++it )
      (*it)->setCheckable( j == 1 );
  }
  myRBGroup->setExclusive( isExclusive );
}

//=================================================================================
// function : ClickOnHelp()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::ClickOnHelp()
{
  LightApp_Application* app = (LightApp_Application*)( SUIT_Session::session()->activeApplication() );

  QString context;
  if(myHelpContext.isEmpty()) {
    context = myGeomGUI ? app->moduleName( myGeomGUI->moduleName() ) : QString("");
  } else {
    context = myHelpContext;
  }
  if ( app ) 
    app->onHelpContextModule( context , myHelpFileName );
  else {
    QString platform;
#ifdef WIN32
    platform = "winapplication";
#else
    platform = "application";
#endif
    SUIT_MessageBox::warning( 0, QObject::tr( "WRN_WARNING" ),
                              QObject::tr( "EXTERNAL_BROWSER_CANNOT_SHOW_PAGE" ).
                              arg( app->resourceMgr()->stringValue( "ExternalBrowser", platform ) ).arg( myHelpFileName ),
                              QObject::tr( "BUT_OK" ) );
  }
}

//=================================================================================
//  function : setHelpFileName()
//  purpose  : set name for help file html
//=================================================================================
void GEOMBase_Skeleton::setHelpFileName( const QString& theName )
{
  myHelpFileName = theName;
}

GEOMBase_DlgSkeleton* GEOMBase_Skeleton::mainFrame()
{
  return myMainFrame;
}

QWidget* GEOMBase_Skeleton::centralWidget()
{
  return myMainFrame->GroupMedium;
}

QPushButton* GEOMBase_Skeleton::buttonCancel() const
{
  return myMainFrame->buttonCancel;
}

QPushButton* GEOMBase_Skeleton::buttonOk() const
{
  return myMainFrame->buttonOk;
}

QPushButton* GEOMBase_Skeleton::buttonApply() const
{
  return myMainFrame->buttonApply;
}

QPushButton* GEOMBase_Skeleton::buttonHelp() const
{
  return myMainFrame->buttonHelp;
}

//=================================================================================
// function : keyPressEvent()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::keyPressEvent( QKeyEvent* e )
{
  QDialog::keyPressEvent( e );
  if ( e->isAccepted() )
    return;

  if ( e->key() == Qt::Key_F1 ) {
    e->accept();
    ClickOnHelp();
  }
}

//=================================================================================
// function : showOnlyPreviewControl()
// purpose  : display only CheckBoxPreview check box,
//            hide CheckBoxRestoreSS and CheckBoxAddPrefix 
//=================================================================================
void GEOMBase_Skeleton::showOnlyPreviewControl(){
  mainFrame()->GroupBoxPublish->show();
  mainFrame()->CheckBoxRestoreSS->hide();
  mainFrame()->CheckBoxAddPrefix->hide();
}

//=================================================================================
// function : processPreview()
// purpose  : Display preview if CheckBoxPreview is checked
//=================================================================================
void GEOMBase_Skeleton::processPreview() {
  displayPreview(mainFrame()->CheckBoxPreview->isChecked());
}
