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

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_OverrideCursor.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Tools.h>
#include <LightApp_SelectionMgr.h>

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
#include "ImportExportGUI_ExportXAODlg.h"

//=================================================================================
// Constructor
//=================================================================================
ImportExportGUI_ExportXAODlg::ImportExportGUI_ExportXAODlg(GeometryGUI* geometryGUI, QWidget* parent)
:
        GEOMBase_Skeleton(geometryGUI, parent, false)
{
    m_mainObj = GEOM::GEOM_Object::_nil();

    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    QPixmap imageOp(resMgr->loadPixmap("GEOM", tr("ICON_DLG_EXPORTXAO")));
    QPixmap iconSelect(resMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

    setWindowTitle(tr("GEOM_EXPORTXAO_TITLE"));

    /***************************************************************/
    mainFrame()->GroupConstructors->setTitle(tr("GEOM_EXPORTXAO_TITLE"));
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
    QLabel* lblShape = new QLabel(tr("GEOM_EXPORTXAO_EXPORTINGSHAPE"), gbxExport);
    btnShapeSelect = new QPushButton(gbxExport);
    btnShapeSelect->setIcon(iconSelect);
    ledShape = new QLineEdit(gbxExport);
    ledShape->setMinimumSize(QSize(100, 0));

    int line = 0, col = 0;
    gridLayoutExport->addWidget(lblShape, line, col++, 1, 1);
    gridLayoutExport->addWidget(btnShapeSelect, line, col++, 1, 1);
    gridLayoutExport->addWidget(ledShape, line, col++, 1, 1);

    // Line 1
    QLabel* lblFileName = new QLabel(tr("GEOM_EXPORTXAO_FILENAME"), gbxExport);
    btnFileSelect = new QPushButton(gbxExport);
    ledFileName = new QLineEdit(gbxExport);
    btnFileSelect->setText("...");

    line++; col = 0;
    gridLayoutExport->addWidget(lblFileName, line, col++, 1, 1);
    gridLayoutExport->addWidget(btnFileSelect, line, col++, 1, 1);
    gridLayoutExport->addWidget(ledFileName, line, col++, 1, 1);

    // Line 2
    QLabel* lblAuthor = new QLabel(tr("GEOM_EXPORTXAO_AUTHOR"), gbxExport);
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
    QLabel* lblGroups = new QLabel(tr("GEOM_EXPORTXAO_LGROUPS"), gbxFilter);
    QLabel* lblFields = new QLabel(tr("GEOM_EXPORTXAO_LFIELDS"), gbxFilter);

    line = 0, col = 0;
    gridLayoutFilter->addWidget(lblGroups, line, col++, 1, 1);
    gridLayoutFilter->addWidget(lblFields, line, col++, 1, 1);

    // Line 1
    lstGroups = new QListWidget(gbxFilter);
    lstGroups->setSelectionMode(QAbstractItemView::ExtendedSelection);
    lstFields = new QListWidget(gbxFilter);
    lstFields->setSelectionMode(QAbstractItemView::ExtendedSelection);

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
    setHelpFileName("create_exportxao_page.html");

    Init();
}

//=================================================================================
// Destructor
//=================================================================================
ImportExportGUI_ExportXAODlg::~ImportExportGUI_ExportXAODlg()
{
    // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void ImportExportGUI_ExportXAODlg::Init()
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

    initName(tr("GEOM_EXPORTXAO"));
    SelectionIntoArgument();
}

//=================================================================================
// function : processObject()
// purpose  : Fill dialog fields in accordance with myObj
//=================================================================================
void ImportExportGUI_ExportXAODlg::processObject()
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
            lstGroups->addItem(item);
            item->setSelected(true);
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
            lstFields->addItem(item);
            item->setSelected(true);
            m_fields.append(GEOM::GeomFieldPtr(fields[i].in()));
        }
        lstFields->sortItems(Qt::AscendingOrder);
    }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void ImportExportGUI_ExportXAODlg::ClickOnOk()
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
bool ImportExportGUI_ExportXAODlg::ClickOnApply()
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
            if (!execute(/*isApplyAndClose()*/))
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
void ImportExportGUI_ExportXAODlg::SelectionIntoArgument()
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
void ImportExportGUI_ExportXAODlg::SetEditCurrentArgument()
{
    ledShape->setFocus();
    myEditCurrentArgument = ledShape;
    SelectionIntoArgument();
}

//=================================================================================
// function : btnFileSelectClicked()
// purpose  :
//=================================================================================
void ImportExportGUI_ExportXAODlg::btnFileSelectClicked()
{
    QFileDialog dlg(this, tr("GEOM_SELECT_EXPORT_XAO") );
    dlg.setLabelText( QFileDialog::Accept, tr("EXPORT_DLG_ACCEPT") );
    dlg.setNameFilter( tr("XAO_FILES") );
    if (dlg.exec())
    {
        QStringList selFiles = dlg.selectedFiles();
        if (selFiles.length() > 0 && !selFiles[0].isEmpty())
        {
            ledFileName->setText(selFiles[0]);
        }
    }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void ImportExportGUI_ExportXAODlg::ActivateThisDialog()
{
    GEOMBase_Skeleton::ActivateThisDialog();
}

//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void ImportExportGUI_ExportXAODlg::enterEvent(QEvent*)
{
    if (!mainFrame()->GroupConstructors->isEnabled())
        ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr ImportExportGUI_ExportXAODlg::createOperation()
{
    return getGeomEngine()->GetIInsertOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool ImportExportGUI_ExportXAODlg::isValid(QString& msg)
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
bool ImportExportGUI_ExportXAODlg::execute()
{
    bool res = false;

    QString author = ledAuthor->text();
    QString fileName = ledFileName->text();

    // get selected groups
    QList<QListWidgetItem*> selGroups = lstGroups->selectedItems();
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
    QList<QListWidgetItem*> selFields = lstFields->selectedItems();
    GEOM::ListOfFields_var fields = new GEOM::ListOfFields();
    fields->length(m_fields.count());
    i = 0;
    for (QList<QListWidgetItem*>::iterator it = selFields.begin(); it != selFields.end(); ++it)
    {
        QListWidgetItem* item = (*it);
        int index = item->data(Qt::UserRole).toInt();
        fields[i++] = m_fields[index].copy();
    }

    // call engine function
    GEOM::GEOM_IInsertOperations_var ieOp = GEOM::GEOM_IInsertOperations::_narrow(getOperation());
    res = ieOp->ExportXAO(m_mainObj, groups, fields,
                          author.toStdString().c_str(),
                          fileName.toStdString().c_str());

    return res;
}
