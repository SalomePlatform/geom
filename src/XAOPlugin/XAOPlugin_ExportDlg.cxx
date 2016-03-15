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
#include "XAOPlugin_ExportDlg.h"
#include "XAOPlugin_IOperations_i.hh"

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_FileDlg.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Tools.h>
#include <LightApp_SelectionMgr.h>

// GEOM includes
#include "GeometryGUI.h"
#include "GEOMBase.h"
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
#include <QGroupBox>
#include <QRadioButton>
#include <QGridLayout>
#include <QPushButton>
#include <QMap>

//=================================================================================
// Constructor
//=================================================================================
XAOPlugin_ExportDlg::XAOPlugin_ExportDlg(GeometryGUI* geometryGUI, QWidget* parent)
:
        GEOMBase_Skeleton(geometryGUI, parent, false)
{
  m_mainObj = GEOM::GEOM_Object::_nil();

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  QPixmap imageOp(resMgr->loadPixmap("GEOM", tr("XAOPLUGIN_EXPORT_ICON")));
  QPixmap iconSelect(resMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("XAOPLUGIN_EXPORT_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("XAOPLUGIN_EXPORT_TITLE"));
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

  // Line 0
  QLabel* lblShape = new QLabel(tr("XAOPLUGIN_EXPORT_INGSHAPE"), gbxExport);
  btnShapeSelect = new QPushButton(gbxExport);
  btnShapeSelect->setIcon(iconSelect);
  ledShape = new QLineEdit(gbxExport);
  ledShape->setMinimumSize(QSize(100, 0));

  int line = 0, col = 0;
  gridLayoutExport->addWidget(lblShape, line, col++, 1, 1);
  gridLayoutExport->addWidget(btnShapeSelect, line, col++, 1, 1);
  gridLayoutExport->addWidget(ledShape, line, col++, 1, 1);

  // Line 1
  QLabel* lblFileName = new QLabel(tr("XAOPLUGIN_EXPORT_FILENAME"), gbxExport);
  btnFileSelect = new QPushButton(gbxExport);
  ledFileName = new QLineEdit(gbxExport);
  btnFileSelect->setText("...");

  line++; col = 0;
  gridLayoutExport->addWidget(lblFileName, line, col++, 1, 1);
  gridLayoutExport->addWidget(btnFileSelect, line, col++, 1, 1);
  gridLayoutExport->addWidget(ledFileName, line, col++, 1, 1);

  // Line 2
  QLabel* lblAuthor = new QLabel(tr("XAOPLUGIN_EXPORT_AUTHOR"), gbxExport);
  ledAuthor = new QLineEdit(gbxExport);

  line++; col = 0;
  gridLayoutExport->addWidget(lblAuthor, line, col++, 2, 1);
  col++; // span
  gridLayoutExport->addWidget(ledAuthor, line, col++, 1, 1);

  //****************************
  // Filter Group box
  QGroupBox* gbxFilter = new QGroupBox(parent);

  QGridLayout* gridLayoutFilter = new QGridLayout(gbxFilter);
#ifndef Q_OS_MAC
  gridLayoutFilter->setSpacing(6);
  gridLayoutFilter->setContentsMargins(9, 9, 9, 9);
#endif
  gridLayoutFilter->setObjectName(QString::fromUtf8("gbxFilter"));

  // Line 0
  QLabel* lblGroups = new QLabel(tr("XAOPLUGIN_EXPORT_LGROUPS"), gbxFilter);
  QLabel* lblFields = new QLabel(tr("XAOPLUGIN_EXPORT_LFIELDS"), gbxFilter);

  line = 0, col = 0;
  gridLayoutFilter->addWidget(lblGroups, line, col++, 1, 1);
  gridLayoutFilter->addWidget(lblFields, line, col++, 1, 1);

  // Line 1
  lstGroups = new QListWidget(gbxFilter);
  lstGroups->setSelectionMode(QAbstractItemView::NoSelection);
  lstFields = new QListWidget(gbxFilter);
  lstFields->setSelectionMode(QAbstractItemView::NoSelection);

  line++; col = 0;
  gridLayoutFilter->addWidget(lstGroups, line, col++, 1, 1);
  gridLayoutFilter->addWidget(lstFields, line, col++, 1, 1);

  //****************************
  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0);
  layout->setSpacing(6);
  layout->addWidget(gbxExport);
  layout->addWidget(gbxFilter);

  // set help
  setHelpFileName("import_export_geom_obj_page.html#io_xao");

  Init();
}

//=================================================================================
// Destructor
//=================================================================================
XAOPlugin_ExportDlg::~XAOPlugin_ExportDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void XAOPlugin_ExportDlg::Init()
{
  // Get setting of step value from file configuration
  m_groups.clear();
  m_fields.clear();

  // Signal/slot connections
  connect(buttonOk(), SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(btnShapeSelect, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(((SalomeApp_Application*) (SUIT_Session::session()->activeApplication()))->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  connect(btnFileSelect, SIGNAL(clicked()), this, SLOT(btnFileSelectClicked()));

  initName(tr("XAOPLUGIN_EXPORTXAO"));
  SelectionIntoArgument();
}

//=================================================================================
// function : processObject()
// purpose  : Fill dialog fields in accordance with myObj
//=================================================================================
void XAOPlugin_ExportDlg::processObject()
{
  lstGroups->clear();
  lstFields->clear();
  m_groups.clear();
  m_fields.clear();

  if (m_mainObj->_is_nil())
  {
    ledShape->setText("");
  }
  else
  {
    ledShape->setText(GEOMBase::GetName(m_mainObj));
    GEOM::GEOM_IShapesOperations_var shapeOp = getGeomEngine()->GetIShapesOperations(getStudyId());

    // add groups names
    GEOM::ListOfGO_var groups = shapeOp->GetExistingSubObjects(m_mainObj, true);
    for (int i = 0, n = groups->length(); i < n; i++)
    {
      QListWidgetItem* item = new QListWidgetItem();
      item->setData(Qt::UserRole, QVariant(i));
      item->setText(GEOMBase::GetName(groups[i]));
      item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
      item->setCheckState(Qt::Checked);
      lstGroups->addItem(item);
      m_groups.append(GEOM::GeomObjPtr(groups[i].in()));
    }
    lstGroups->sortItems(Qt::AscendingOrder);

    // add fields
    GEOM::GEOM_IFieldOperations_var fieldOp = getGeomEngine()->GetIFieldOperations(getStudyId());

    GEOM::ListOfFields_var fields = fieldOp->GetFields(m_mainObj);
    for (int i = 0, n = fields->length(); i < n; i++)
    {
      QListWidgetItem* item = new QListWidgetItem();
      item->setData(Qt::UserRole, QVariant(i));
      item->setText(fields[i]->GetName());
      item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
      item->setCheckState(Qt::Checked);
      lstFields->addItem(item);
      m_fields.append(GEOM::GeomFieldPtr(fields[i].in()));
    }
    lstFields->sortItems(Qt::AscendingOrder);
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void XAOPlugin_ExportDlg::ClickOnOk()
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
bool XAOPlugin_ExportDlg::ClickOnApply()
{
  if (!isApplyAndClose())
  {
    setIsDisableBrowsing(true);
    setIsDisplayResult(false);
  }

  QString msg;
  if (!isValid(msg))
  {
    showError(msg);
    return false;
  }
  SUIT_OverrideCursor wc;
  SUIT_Session::session()->activeApplication()->putInfo("");

  try
  {
    if (openCommand())
      if (!execute())
      {
        abortCommand();
        showError();
        return false;
      }
  }
  catch (const SALOME::SALOME_Exception& e)
  {
    SalomeApp_Tools::QtCatchCorbaException(e);
    abortCommand();
    return false;
  }
  commitCommand();

  if (!isApplyAndClose())
  {
    setIsDisableBrowsing(false);
    setIsDisplayResult(true);
  }

  processObject();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void XAOPlugin_ExportDlg::SelectionIntoArgument()
{
  m_mainObj = GEOM::GEOM_Object::_nil();
  LightApp_SelectionMgr* selMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO selList;
  selMgr->selectedObjects(selList);

  if (selList.Extent() == 1)
  {
    m_mainObj = GEOMBase::ConvertIOinGEOMObject(selList.First());
  }

  processObject();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void XAOPlugin_ExportDlg::SetEditCurrentArgument()
{
  ledShape->setFocus();
  myEditCurrentArgument = ledShape;
  SelectionIntoArgument();
}

//=================================================================================
// function : btnFileSelectClicked()
// purpose  :
//=================================================================================
void XAOPlugin_ExportDlg::btnFileSelectClicked()
{
  QString file = SUIT_FileDlg::getFileName(this, ledFileName->text(), 
                                           tr("XAOPLUGIN_FILES"),
                                           tr("XAOPLUGIN_EXPORT_SELECT"), false);
  if ( !file.isEmpty() )
    ledFileName->setText( file );
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void XAOPlugin_ExportDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
}

//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void XAOPlugin_ExportDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr XAOPlugin_ExportDlg::createOperation()
{
  return getGeomEngine()->GetPluginOperations( getStudyId(), "XAOPluginEngine" );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool XAOPlugin_ExportDlg::isValid(QString& msg)
{
  // check shape
  if (ledShape->text().isEmpty())
    return false;

  // check file name
  if (ledFileName->text().isEmpty())
    return false;

  return true;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool XAOPlugin_ExportDlg::execute()
{
  bool res = false;

  QString author = ledAuthor->text();
  QString fileName = ledFileName->text();

  // get selected groups
  QList<QListWidgetItem*> selGroups;
  for (int j = 0; j < lstGroups->count(); ++j)
  {
    if (lstGroups->item(j)->checkState() == Qt::Checked)
      selGroups.append(lstGroups->item(j));
  }

  GEOM::ListOfGO_var groups = new GEOM::ListOfGO();
  groups->length(selGroups.count());
  int i = 0;
  for (QList<QListWidgetItem*>::iterator it = selGroups.begin(); it != selGroups.end(); ++it)
  {
    QListWidgetItem* item = (*it);
    int index = item->data(Qt::UserRole).toInt();
    groups[i++] = m_groups[index].copy();
  }

  // get selected fields
  QList<QListWidgetItem*> selFields;
  for (int j = 0; j < lstFields->count(); ++j)
  {
    if (lstFields->item(j)->checkState() == Qt::Checked)
      selFields.append(lstFields->item(j));
  }

  GEOM::ListOfFields_var fields = new GEOM::ListOfFields();
  fields->length(selFields.count());
  i = 0;
  for (QList<QListWidgetItem*>::iterator it = selFields.begin(); it != selFields.end(); ++it)
  {
    QListWidgetItem* item = (*it);
    int index = item->data(Qt::UserRole).toInt();
    fields[i++] = m_fields[index].copy();
  }

  // call engine function
  GEOM::IXAOOperations_var aXAOOp = GEOM::IXAOOperations::_narrow( getOperation() );
  res = aXAOOp->ExportXAO(m_mainObj, groups, fields,
                          author.toUtf8().constData(),
                          fileName.toUtf8().constData());
  return res;
}
