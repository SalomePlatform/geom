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

// internal includes
#include "XAOPlugin_ImportDlg.h"
#include "XAOPlugin_IOperations_i.hh"

// KERNEL includes
#include <SALOMEDS_wrap.hxx>

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_FileDlg.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Tools.h>

// GEOM includes
#include "GeometryGUI.h"
#include "GEOMBase.h"
#include "GEOM_Field.hxx"
#include "GEOMImpl_Types.hxx"

// OCCT Includes
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// QT includes
#include <QLabel>
#include <QLineEdit>
#include <QButtonGroup>
#include <QListWidget>
#include <QFileDialog>
#include <QRadioButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QMap>

//=================================================================================
// Constructor
//=================================================================================
XAOPlugin_ImportDlg::XAOPlugin_ImportDlg(GeometryGUI* geometryGUI, QWidget* parent)
:
        GEOMBase_Skeleton(geometryGUI, parent, false)
{
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    QPixmap imageOp(resMgr->loadPixmap("GEOM", tr("XAOPLUGIN_IMPORT_ICON")));
    QPixmap iconSelect(resMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

    setWindowTitle(tr("XAOPLUGIN_IMPORT_TITLE"));

    /***************************************************************/
    mainFrame()->GroupConstructors->setTitle(tr("XAOPLUGIN_IMPORT_TITLE"));
    mainFrame()->RadioButton1->setIcon(imageOp);
    mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
    mainFrame()->RadioButton2->close();
    mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
    mainFrame()->RadioButton3->close();

    // hide name
    mainFrame()->GroupBoxName->hide();

    //****************************
    // Selection Group box
    QGroupBox* gbxExport = new QGroupBox(parent);

    QGridLayout* gridLayoutExport = new QGridLayout(gbxExport);
#ifndef Q_OS_MAC
    gridLayoutExport->setSpacing(6);
    gridLayoutExport->setContentsMargins(9, 9, 9, 9);
#endif
    gridLayoutExport->setObjectName(QString::fromUtf8("gridLayoutExport"));

    int line = 0, col = 0;
    QLabel* lblFileName = new QLabel(tr("XAOPLUGIN_IMPORT_FILENAME"), gbxExport);
    btnFileSelect = new QPushButton(gbxExport);
    ledFileName = new QLineEdit(gbxExport);
    btnFileSelect->setText("...");

    line++; col = 0;
    gridLayoutExport->addWidget(lblFileName, line, col++, 1, 1);
    gridLayoutExport->addWidget(btnFileSelect, line, col++, 1, 1);
    gridLayoutExport->addWidget(ledFileName, line, col++, 1, 1);

    //****************************
    QVBoxLayout* layout = new QVBoxLayout(centralWidget());
    layout->setMargin(0);
    layout->setSpacing(6);
    layout->addWidget(gbxExport);

    // set help
    setHelpFileName("import_export_geom_obj_page.html#io_xao");

    Init();
}

//=================================================================================
// Destructor
//=================================================================================
XAOPlugin_ImportDlg::~XAOPlugin_ImportDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void XAOPlugin_ImportDlg::Init()
{
    // Signal/slot connections
    connect(buttonOk(), SIGNAL(clicked()), this, SLOT(ClickOnOk()));
    connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

    connect(btnFileSelect, SIGNAL(clicked()), this, SLOT(btnFileSelectClicked()));

    initName(tr("XAOPLUGIN_IMPORTXAO"));
    //SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void XAOPlugin_ImportDlg::ClickOnOk()
{
    setIsApplyAndClose(true);
    if (ClickOnApply())
        ClickOnCancel();
    setIsApplyAndClose(false);
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool XAOPlugin_ImportDlg::ClickOnApply()
{
  if(!isApplyAndClose()) {
    setIsDisableBrowsing( true );
    setIsDisplayResult( false );
  }

  QString msg;
  if ( !isValid( msg ) ) {
    showError( msg );
    return false;
  }
  SUIT_OverrideCursor wc;
  SUIT_Session::session()->activeApplication()->putInfo( "" );

  try {
    if ( openCommand() )
      if (!execute (/*isApplyAndClose()*/))
      {
        abortCommand();
        showError();
        return false;
      }
  }
  catch( const SALOME::SALOME_Exception& e ) {
    SalomeApp_Tools::QtCatchCorbaException( e );
    abortCommand();
    return false;
  }
  commitCommand();

  if(!isApplyAndClose()) {
    setIsDisableBrowsing( false );
    setIsDisplayResult( true );
  }

  return true;
}

//=================================================================================
// function : btnFileSelectClicked()
// purpose  :
//=================================================================================
void XAOPlugin_ImportDlg::btnFileSelectClicked()
{
  QString file = SUIT_FileDlg::getFileName(this, ledFileName->text(), 
                                           tr("XAOPLUGIN_FILES"),
                                           tr("XAOPLUGIN_IMPORT_SELECT"));
  if ( !file.isEmpty() )
    ledFileName->setText( file );
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void XAOPlugin_ImportDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
}

//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void XAOPlugin_ImportDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr XAOPlugin_ImportDlg::createOperation()
{
  return getGeomEngine()->GetPluginOperations( getStudyId(), "XAOPluginEngine" );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool XAOPlugin_ImportDlg::isValid(QString& msg)
{
  // check file name
  if (ledFileName->text().isEmpty())
    return false;

  return true;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool XAOPlugin_ImportDlg::execute()
{
  bool res = false;

  QString fileName = ledFileName->text();
  GEOM::GEOM_Object_var shape;
  GEOM::ListOfGO_var groups, subShapes;
  GEOM::ListOfFields_var fields;

  GEOM::IXAOOperations_var ieOp = GEOM::IXAOOperations::_narrow(getOperation());
  res = ieOp->ImportXAO(fileName.toUtf8().constData(), shape, subShapes, groups, fields);

  if (!shape->_is_nil())
  {
    m_mainShape = shape;
  }
  else
  {
    m_mainShape = NULL;
  }

  if (m_mainShape != NULL)
  {
    QStringList anEntryList;
    anEntryList << addInStudy(m_mainShape, m_mainShape->GetName());
    m_mainShape->UnRegister();
    for (int i = 0; i < subShapes->length(); i++)
    {
      addInStudy(subShapes[i].in(), subShapes[i]->GetName());
      subShapes[i]->UnRegister();
    }
    for (int i = 0; i < groups->length(); i++)
    {
      addInStudy(groups[i].in(), groups[i]->GetName());
      groups[i]->UnRegister();
    }
    for (int i = 0; i < fields->length(); i++)
    {
      addFieldInStudy(fields[i].in(), m_mainShape);
    }

    updateObjBrowser();
    if( SUIT_Application* anApp = SUIT_Session::session()->activeApplication() ) {
      LightApp_Application* aLightApp = dynamic_cast<LightApp_Application*>( anApp );
      aLightApp->browseObjects( anEntryList );
    }
  }

  return res;
}

QString XAOPlugin_ImportDlg::addFieldInStudy( GEOM::GEOM_Field_ptr theField, GEOM::GEOM_Object_ptr theFather)
{
  if ( !hasCommand() )
    return QString();

  _PTR(Study) aStudy = getStudy()->studyDS();
  if ( !aStudy || theField->_is_nil() )
    return QString();

  SALOMEDS::Study_var aStudyDS = GeometryGUI::ClientStudyToStudy(aStudy);

  SALOMEDS::SObject_var aSO =
    getGeomEngine()->AddInStudy(aStudyDS, theField, theField->GetName(), theFather);
  theField->UnRegister();

  QString anEntry;
  if ( !aSO->_is_nil() ) {
    CORBA::String_var entry = aSO->GetID();
    anEntry = entry.in();
  }

  // add steps
  GEOM::ListOfLong_var steps = theField->GetSteps();
  for (int i = 0; i < steps->length(); ++i)
  {
    GEOM::GEOM_FieldStep_ptr step = theField->GetStep(steps[i]);
    QString stepName = (tr("XAOPLUGIN_STEP") + " %1 %2").arg( step->GetID() ).arg( step->GetStamp() );
    SALOMEDS::SObject_wrap aSOField =
      getGeomEngine()->AddInStudy( aStudyDS, step, stepName.toLatin1().constData(), theField );
    step->UnRegister();
  }

  aSO->UnRegister();

  return anEntry;
}

GEOM::GEOM_Object_ptr XAOPlugin_ImportDlg::getFather(GEOM::GEOM_Object_ptr object)
{
  GEOM::GEOM_Object_var fatherObj;
  if (object->GetType() != GEOM_IMPORT && m_mainShape != NULL)
  {
    //GEOM::GEOM_IGroupOperations_var groupOper = getGeomEngine()->GetIGroupOperations(getStudyId());
    //fatherObj = groupOper->GetMainShape(object);
    fatherObj = m_mainShape;
  }
  return fatherObj._retn();
}

QString XAOPlugin_ImportDlg::getObjectName(GEOM::GEOM_Object_ptr object) const
{
  if (object->_is_nil())
    return QString::null;
  return object->GetName();
}

