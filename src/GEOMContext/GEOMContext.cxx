//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOMContext.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "GEOMContext.h"
#include "QAD_Tools.h"

/* The object itself created in the static method 'GetOrCreateGEOMBase()' */
static GEOMContext* GeomGUI = 0;

//=======================================================================
// function : GEOMContext()
// purpose  : Constructor
//=======================================================================
GEOMContext::GEOMContext() :
  QObject()
{
}


//=======================================================================
// function : ~GEOMContext()
// purpose  : Destructor
//=======================================================================
GEOMContext::~GEOMContext()
{
}


//=======================================================================
// function : GetGEOMContext() static
// purpose  : Returns current 'GeomGUI' a static pointer
//=======================================================================
GEOMContext* GEOMContext::GetGeomGUI()
{
  return GeomGUI;
}


//=======================================================================
// function : GetOrCreateGEOMBase()
// purpose  : Gets or create an object 'GEOMBase' with initialisations
//          : Returns 'GeomGUI' as a pointer
//=======================================================================
GEOMContext* GEOMContext::GetOrCreateGeomGUI(QAD_Desktop* desktop)
{
  if(GeomGUI == 0) {
    GeomGUI = new GEOMContext();
    Engines::Component_var comp = desktop->getEngine("FactoryServer", "GEOM");
    GeomGUI->myComponentGeom = GEOM::GEOM_Gen::_narrow(comp);
    
    GeomGUI->myState = -1;
    GeomGUI->myActiveDialogBox = 0;
    GeomGUI->myFatherior = "";
    GeomGUI->myGUILibrary = OSD_SharedLibrary();

    /* GetCurrentStudy */
    int studyId = desktop->getActiveStudy()->getStudyId();
    GeomGUI->myComponentGeom->GetCurrentStudy(studyId);
    GeomGUI->myNbGeom = GeomGUI->myComponentGeom->NbLabels();
  }

  return GeomGUI;
}


//=======================================================================
// function : LoadLibrary()
// purpose  :
//=======================================================================
bool GEOMContext::LoadLibrary(QString GUILibrary)
{
  QCString libs;
  QFileInfo fileInfo;
  QString GUILib, fileString, dir;

  if(libs = getenv("LD_LIBRARY_PATH")) {
    QStringList dirList = QStringList::split(":", libs, false); // skip empty entries
    for(int i = dirList.count()-1; i >= 0; i--) {
      dir = dirList[i];
      fileString = QAD_Tools::addSlash(dir) + GUILibrary;
      fileInfo.setFile(fileString);
      if(fileInfo.exists()) {
	GUILib = fileInfo.fileName();
	break;
      }
    }
  }

  myGUILibrary.SetName(TCollection_AsciiString((char*)GUILib.latin1()).ToCString());
  bool res = myGUILibrary.DlOpen(OSD_RTLD_LAZY);
  if(!res)
    cout<<"Can't open library : "<<myGUILibrary.DlError()<<endl;
  return res;
}


//=======================================================================
// function : SetActiveDialogBox()
// purpose  : 
//=======================================================================
void GEOMContext::SetActiveDialogBox(QDialog* aDlg)
{
  myActiveDialogBox = (QDialog*)aDlg;
  return;
}


//=======================================================================
// function : EmitSignalDeactivateDialog()
// purpose  : Emit a signal to deactivate the active dialog Box
//=======================================================================
void GEOMContext::EmitSignalDeactivateDialog()
{
  emit SignalDeactivateActiveDialog();
  return;
}


//=======================================================================
// function : EmitSignalCloseAllDialogs()
// purpose  : Emit a signal to deactivate all non modal dialogs box
//=======================================================================
void GEOMContext::EmitSignalCloseAllDialogs()
{
  emit SignalCloseAllDialogs();
  return;
}


//=======================================================================
// function : EmitSignalDefaultStepValueChanged()
// purpose  : 
//=======================================================================
void GEOMContext::EmitSignalDefaultStepValueChanged(double newVal)
{
  emit SignalDefaultStepValueChanged(newVal);
  return;
}
