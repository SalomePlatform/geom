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

#ifndef IMPORTEXPORTGUI_IMPORTXAODLG_H
#define IMPORTEXPORTGUI_IMPORTXAODLG_H

#include <GEOMBase_Skeleton.h>
#include <GEOM_GenericObjPtr.h>

class QLineEdit;
class QButtonGroup;
class QListWidget;

//=================================================================================
// class    : ImportExportGUI_ImportXAODlg
// purpose  :
//=================================================================================
class ImportExportGUI_ImportXAODlg: public GEOMBase_Skeleton
{
    Q_OBJECT

public:
    ImportExportGUI_ImportXAODlg(GeometryGUI*, QWidget* = 0);
    ~ImportExportGUI_ImportXAODlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid(QString&);
    virtual bool execute(ObjectList&);
    virtual GEOM::GEOM_Object_ptr getFather(GEOM::GEOM_Object_ptr object);
    virtual QString getObjectName(GEOM::GEOM_Object_ptr object) const;

private:
    void Init();
    void enterEvent(QEvent*);

private:
    QLineEdit* ledFileName;
    QPushButton* btnFileSelect;
    GEOM::GEOM_Object_var m_mainShape;

private slots:
    void ClickOnOk();
    bool ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void btnFileSelectClicked();
};

#endif // IMPORTEXPORTGUI_EXPORTXAODLG_H
