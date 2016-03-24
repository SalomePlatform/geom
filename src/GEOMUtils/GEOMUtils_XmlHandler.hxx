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

#ifndef _GEOMUtils_XmlHandler_HXX_
#define _GEOMUtils_XmlHandler_HXX_

#include <Standard_Macro.hxx>

#include <string>
#include <list>

namespace GEOMUtils
{
  //! Plugin action data
  struct Standard_EXPORT ActionData
  {
    std::string label;       //!< unique ID
    std::string icon;        //!< icon
    std::string menuText;    //!< menu text
    std::string toolTip;     //!< tooltip
    std::string statusText;  //!< status bar text
    std::string accel;       //!< shortcut
  };
  
  //! Plugin data
  struct Standard_EXPORT PluginData
  {
    std::string name;              //!< plugin name
    std::string serverLib;         //!< engine library
    std::string clientLib;         //!< GUI library
    std::list<ActionData> actions; //!< actions
  };
  
  //! Plugins information
  typedef std::list<PluginData> PluginInfo;

  Standard_EXPORT PluginInfo ReadPluginInfo();
}

#endif // _GEOMUtils_XmlHandler_HXX_
