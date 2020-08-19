// Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GEOM_Swig_LocalSelector.h"
#include <SalomeApp_Study.h>
#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <GeometryGUI.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SALOMEDSClient_definitions.hxx>
#include <vector>
#include <utilities.h>

GEOM_Swig_LocalSelector::GEOM_Swig_LocalSelector(QWidget* parent, SUIT_ViewWindow* wnd, const std::string& shapeEntry, int mode) :
    GEOMBase_Helper(dynamic_cast<SUIT_Desktop*>(parent), wnd )
{
  MESSAGE("GEOM_Swig_LocalSelector " << shapeEntry << " " << mode);
  myObject = GEOM::GEOM_Object::_nil();
  mySubShapesInd = new GEOM::short_array();
  std::string shapeName = "aShape";

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(SUIT_Session::session()->activeApplication()->activeStudy());
  if (study)
  {
    _PTR(Study) studyDS = study->studyDS();
    _PTR(SObject) obj( studyDS->FindObjectID( shapeEntry ) );
    if ( GeometryGUI::IsInGeomComponent( obj ))
    {
      CORBA::Object_var corbaObj = GeometryGUI::ClientSObjectToObject(obj);
      if (!CORBA::is_nil(corbaObj))
      {
        myObject = GEOM::GEOM_Object::_narrow(corbaObj);
        shapeName = myObject->GetName();
        MESSAGE("shapeName: " << shapeName);
      }
    }
  }

  std::list<int> modes;
  modes.push_back(mode);
  localSelection(shapeEntry, shapeName, modes);
}

GEOM_Swig_LocalSelector::~GEOM_Swig_LocalSelector()
{
  MESSAGE("~GEOM_Swig_LocalSelector");
}

std::vector<int> GEOM_Swig_LocalSelector::getSelection()
{
  MESSAGE("getSelection");
  SalomeApp_Application* app = (SalomeApp_Application*)SUIT_Session::session()->activeApplication();
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  TColStd_IndexedMapOfInteger aMap;
  MESSAGE("aSelList.Extent(): " << aSelList.Extent());
  if (aSelList.Extent() > 0)
    {
    Handle(SALOME_InteractiveObject) anIO = aSelList.First();
    aSelMgr->GetIndexes(anIO, aMap);
    }

  const int n = aMap.Extent();
  std::vector<int> ids;
  for (int i = 1; i <= n; i++)
    ids.push_back(aMap(i));
  for (int i =0; i < (int)ids.size(); i++)
    {
      MESSAGE("ids[" << i << "] = " << ids[i]);
    }

  return ids;
}

