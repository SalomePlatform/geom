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

#ifndef IMPORTEXPORTGUI_EXPORTXAODLG_H
#define IMPORTEXPORTGUI_EXPORTXAODLG_H

#include <GEOMBase_Skeleton.h>
#include <GEOM_GenericObjPtr.h>

class QLineEdit;
class QButtonGroup;
class QListWidget;

//class ImportExportGUI_1Sel1LineEdit2ListWidget;

//=================================================================================
// class    : ImportExportGUI_ExportXAODlg
// purpose  :
//=================================================================================
class ImportExportGUI_ExportXAODlg: public GEOMBase_Skeleton
{
    Q_OBJECT

public:
    ImportExportGUI_ExportXAODlg(GeometryGUI*, QWidget* = 0);
    ~ImportExportGUI_ExportXAODlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid(QString&);
    virtual bool execute(ObjectList&);

private:
    void Init();
    void enterEvent(QEvent*);
    void processObject();

private:
    GEOM::GEOM_Object_var m_mainObj;
    QList<GEOM::GeomObjPtr> m_groups;
    QList<GEOM::GeomObjPtr> m_fields;
    QLineEdit* ledShape;
    QLineEdit* ledFileName;
    QLineEdit* ledAuthor;
    QListWidget* lstGroups;
    QListWidget* lstFields;
    QPushButton* btnShapeSelect;
    QPushButton* btnFileSelect;

private slots:
    void ClickOnOk();
    bool ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void btnFileSelectClicked();
};

#endif // IMPORTEXPORTGUI_EXPORTXAODLG_H