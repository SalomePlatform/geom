//  Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <GEOM_Field.hxx>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_OverrideCursor.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Tools.h>

#include <QLabel>
#include <QLineEdit>
#include <QButtonGroup>
#include <QListWidget>
#include <QFileDialog>
#include <QMap>

// OCCT Includes
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>
#include "ImportExportGUI_ImportXAODlg.h"

#include <SALOMEDS_wrap.hxx>

//=================================================================================
// Constructor
//=================================================================================
ImportExportGUI_ImportXAODlg::ImportExportGUI_ImportXAODlg(GeometryGUI* geometryGUI, QWidget* parent)
:
        GEOMBase_Skeleton(geometryGUI, parent, false)
{
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    QPixmap imageOp(resMgr->loadPixmap("GEOM", tr("ICON_DLG_IMPORTXAO")));
    QPixmap iconSelect(resMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

    setWindowTitle(tr("GEOM_IMPORTXAO_TITLE"));

    /***************************************************************/
    mainFrame()->GroupConstructors->setTitle(tr("GEOM_IMPORTXAO_TITLE"));
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
    QLabel* lblFileName = new QLabel(tr("GEOM_IMPORTXAO_FILENAME"), gbxExport);
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
    setHelpFileName("xao_format_page.html");

    Init();
}

//=================================================================================
// Destructor
//=================================================================================
ImportExportGUI_ImportXAODlg::~ImportExportGUI_ImportXAODlg()
{
    // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void ImportExportGUI_ImportXAODlg::Init()
{
    // Signal/slot connections
    connect(buttonOk(), SIGNAL(clicked()), this, SLOT(ClickOnOk()));
    connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

    connect(btnFileSelect, SIGNAL(clicked()), this, SLOT(btnFileSelectClicked()));

    initName(tr("GEOM_IMPORTXAO"));
    //SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void ImportExportGUI_ImportXAODlg::ClickOnOk()
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
bool ImportExportGUI_ImportXAODlg::ClickOnApply()
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
void ImportExportGUI_ImportXAODlg::btnFileSelectClicked()
{
    QString selFile = QFileDialog::getOpenFileName(this, tr("GEOM_SELECT_EXPORT_XAO"),
                                                   QString(), tr("XAO_FILES"));
    if (!selFile.isEmpty())
    {
        ledFileName->setText(selFile);
    }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void ImportExportGUI_ImportXAODlg::ActivateThisDialog()
{
    GEOMBase_Skeleton::ActivateThisDialog();
}

//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void ImportExportGUI_ImportXAODlg::enterEvent(QEvent*)
{
    if (!mainFrame()->GroupConstructors->isEnabled())
        ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr ImportExportGUI_ImportXAODlg::createOperation()
{
    return getGeomEngine()->GetIInsertOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool ImportExportGUI_ImportXAODlg::isValid(QString& msg)
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
bool ImportExportGUI_ImportXAODlg::execute()
{
    bool res = false;

    QString fileName = ledFileName->text();
    GEOM::GEOM_Object_var shape;
    GEOM::ListOfGO_var groups, subShapes;
    GEOM::ListOfFields_var fields;

    GEOM::GEOM_IInsertOperations_var ieOp = GEOM::GEOM_IInsertOperations::_narrow(getOperation());
    res = ieOp->ImportXAO(fileName.toStdString().c_str(), shape, subShapes, groups, fields);

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

        for (int i = 0; i < subShapes->length(); i++)
        {
            addInStudy(subShapes[i].in(), subShapes[i]->GetName());
        }
        for (int i = 0; i < groups->length(); i++)
        {
            addInStudy(groups[i].in(), groups[i]->GetName());
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

QString ImportExportGUI_ImportXAODlg::addFieldInStudy( GEOM::GEOM_Field_ptr theField, GEOM::GEOM_Object_ptr theFather)
{
  if ( !hasCommand() )
    return QString();

  _PTR(Study) aStudy = getStudy()->studyDS();
  if ( !aStudy || theField->_is_nil() )
    return QString();

  SALOMEDS::Study_var aStudyDS = GeometryGUI::ClientStudyToStudy(aStudy);

  SALOMEDS::SObject_var aSO =
    getGeomEngine()->AddInStudy(aStudyDS, theField, theField->GetName(), theFather);

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
    QString stepName = (tr("STEP") + " %1 %2").arg( step->GetID() ).arg( step->GetStamp() );
    SALOMEDS::SObject_wrap aSOField =
      getGeomEngine()->AddInStudy( aStudyDS, step, stepName.toLatin1().constData(), theField );
  }

  aSO->UnRegister();

  return anEntry;
}

GEOM::GEOM_Object_ptr ImportExportGUI_ImportXAODlg::getFather(GEOM::GEOM_Object_ptr object)
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

QString ImportExportGUI_ImportXAODlg::getObjectName(GEOM::GEOM_Object_ptr object) const
{
    if (object->_is_nil())
        return QString::null;
    return object->GetName();
}

