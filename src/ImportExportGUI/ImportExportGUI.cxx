// Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <iostream>
#include <QDialog>

#include "GeometryGUI.h"
#include "GeometryGUI_Operations.h"

#include <GEOMBase.h>
#include <SUIT_Desktop.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SalomeApp_Application.h>

#include "ImportExportGUI_ExportXAODlg.h"
#include "ImportExportGUI_ImportXAODlg.h"
#include "ImportExportGUI.h"
//@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@//


//=======================================================================
// function : ImportExportGUI()
// purpose  : Constructor
//=======================================================================
ImportExportGUI::ImportExportGUI(GeometryGUI* parent) :
        GEOMGUI(parent)
{
}

//=======================================================================
// function : ~ImportExportGUI
// purpose  : Destructor
//=======================================================================
ImportExportGUI::~ImportExportGUI()
{
}

//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool ImportExportGUI::OnGUIEvent(int commandId, SUIT_Desktop* parent)
{
    SalomeApp_Application* app = getGeometryGUI()->getApp();
    if (!app) return false;

    getGeometryGUI()->EmitSignalDeactivateDialog();

    QDialog* dialog = NULL;
    switch (commandId)
    {
        case GEOMOp::OpExportXAO:
            dialog = new ImportExportGUI_ExportXAODlg(getGeometryGUI(), parent);
            break;
        case GEOMOp::OpImportXAO:
            dialog = new ImportExportGUI_ImportXAODlg(getGeometryGUI(), parent);
            break;
            //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@//
        default:
            app->putInfo(tr("GEOM_PRP_COMMAND").arg(commandId));
            break;
    }

    if (dialog != NULL)
        dialog->show();

    return true;
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WIN32
    __declspec( dllexport )
#endif
    GEOMGUI* GetLibGUI(GeometryGUI* parent)
    {
        return new ImportExportGUI(parent);
    }
}
