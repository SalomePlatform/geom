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

#include "GEOMUtils_XmlHandler.hxx"

#include <libxml/parser.h>
#include <algorithm>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

//#define MYDEBUG

namespace
{
  const char*    env_var     = "GEOM_PluginsList";
  
  const xmlChar* root_tag    = (xmlChar*)"geom-plugins";
  const xmlChar* plugin_tag  = (xmlChar*)"geom-plugin";
  const xmlChar* name_tag    = (xmlChar*)"name";
  const xmlChar* server_tag  = (xmlChar*)"server-lib";
  const xmlChar* gui_tag     = (xmlChar*)"gui-lib";
  const xmlChar* actions_tag = (xmlChar*)"actions";
  const xmlChar* action_tag  = (xmlChar*)"action";
  const xmlChar* label_tag   = (xmlChar*)"label";
  const xmlChar* icon_tag    = (xmlChar*)"icon";
  const xmlChar* menu_tag    = (xmlChar*)"menu";
  const xmlChar* tooltip_tag = (xmlChar*)"tooltip";
  const xmlChar* status_tag  = (xmlChar*)"status-bar";
  const xmlChar* accel_tag   = (xmlChar*)"accel";

  std::string toUpper( const std::string& s )
  {
    std::string r = s;
    std::transform( r.begin(), r.end(), r.begin(), toupper );
    return r;
  }

  std::string toLower( const std::string& s )
  {
    std::string r = s;
    std::transform( r.begin(), r.end(), r.begin(), tolower );
    return r;
  }

  std::string readXmlAttribute(xmlNodePtr node, const xmlChar* attribute)
  {
    std::string result = "";
    xmlChar* strAttr = xmlGetProp(node, attribute);
    if (strAttr != NULL) {
      result = (char*)strAttr;
      xmlFree(strAttr);
    }
    return result;
  }

  std::list<std::string> getPluginXMLFiles()
  {
    std::list<std::string> xmlPaths;

#ifdef WIN32
    std::string sep = "\\";
#else
    std::string sep = "/";
#endif

    if ( const char* var = getenv( env_var ) )
    {
      std::string plugins = var;

      std::string::size_type from = 0, pos;
      while ( from < plugins.size() )
      {
	pos = plugins.find( ':', from );
	std::string plugin;
	if ( pos != std::string::npos )
	  plugin = plugins.substr( from, pos-from );
	else
	  plugin = plugins.substr( from ), pos = plugins.size();
	from = pos + 1;
	
	if ( plugin.size() == 0 ) continue;
	
	std::string pluginRoot    = toUpper( plugin+"_ROOT_DIR" );

	const char* rootDirGeom   = getenv( "GEOM_ROOT_DIR" );
	const char* rootDirPlugin = getenv( pluginRoot.c_str() );

	bool fileOK = false;
	if ( rootDirGeom ) {
	  std::string xmlPath = rootDirGeom;
	  if ( xmlPath[ xmlPath.size()-1 ] != sep[0] )
	    xmlPath += sep;
	  xmlPath += "share" + sep + "salome" + sep + "resources" + sep + "geom" + sep + plugin + ".xml";
#ifdef WIN32
	  fileOK = (GetFileAttributes(xmlPath.c_str()) != INVALID_FILE_ATTRIBUTES);
#else
	  fileOK = (access(xmlPath.c_str(), F_OK) == 0);
#endif
	  if ( fileOK )
	    xmlPaths.push_back( xmlPath );
	}
	if ( !fileOK && rootDirPlugin ) {
	  std::string xmlPath = rootDirPlugin;
	  if ( xmlPath[ xmlPath.size()-1 ] != sep[0] )
	    xmlPath += sep;
	  xmlPath += "share" + sep + "salome" + sep + "resources" + sep + toLower(plugin) + sep + plugin + ".xml";
#ifdef WIN32
	  fileOK = (GetFileAttributes(xmlPath.c_str()) != INVALID_FILE_ATTRIBUTES);
#else
	  fileOK = (access(xmlPath.c_str(), F_OK) == 0);
#endif
	  if ( fileOK )
	    xmlPaths.push_back( xmlPath );
	}
      }
    }
    return xmlPaths;
  }

  void dumpinfo(const GEOMUtils::PluginInfo& info)
  {
    printf("DUMPING PLUGIN INFO\n");
    GEOMUtils::PluginInfo::const_iterator it;
    for (it = info.begin(); it != info.end(); ++it) {
      GEOMUtils::PluginData pdata = *it;
      printf("Plugin: %s\n", pdata.name.c_str());
      printf("  serverLib = %s\n", pdata.serverLib.c_str());
      printf("  clientLib = %s\n", pdata.clientLib.c_str());
      printf("  actions:\n");
      std::list<GEOMUtils::ActionData>::const_iterator ait;
      for (ait = pdata.actions.begin(); ait != pdata.actions.end(); ++ait) {
	GEOMUtils::ActionData adata = *ait;
	printf("     label      = %s\n", adata.label.c_str());
	printf("     icon       = %s\n", adata.icon.c_str());
	printf("     menuText   = %s\n", adata.menuText.c_str());
	printf("     toolTip    = %s\n", adata.toolTip.c_str());
	printf("     statusText = %s\n", adata.statusText.c_str());
	printf("\n");
      }
      printf("-----\n");
    }
  }
}

namespace GEOMUtils
{
  PluginInfo ReadPluginInfo()
  {
    PluginInfo info;

    std::list<std::string> xmlPaths = getPluginXMLFiles();

    std::list<std::string>::const_iterator fit;

    for ( fit = xmlPaths.begin(); fit != xmlPaths.end(); ++fit )
    {
      std::string fileName = *fit;
      
      int options = XML_PARSE_HUGE | XML_PARSE_NOCDATA;
      xmlDocPtr doc = xmlReadFile( fileName.c_str(), NULL, options );
      
      if ( doc )
      {
	// get root node
	xmlNodePtr root = xmlDocGetRootElement(doc);
	
	// check if it is plugins container node
	if (xmlStrcmp(root->name, root_tag) == 0)
	{
	  // iterate through children, to get plugins data
	  for (xmlNodePtr node = root->children; node; node = node->next)
	  {
	    if (xmlStrcmp(node->name, plugin_tag) == 0)
	    {
	      // plugin node
	      PluginData data;
	      data.name      = readXmlAttribute(node, name_tag);
	      data.serverLib = readXmlAttribute(node, server_tag);
	      data.clientLib = readXmlAttribute(node, gui_tag);
	      // iterate through children, to find actions container node
	      for (xmlNodePtr subnode = node->children; subnode; subnode = subnode->next)
	      {
		if (xmlStrcmp(subnode->name, actions_tag) == 0)
		{
		  // actions container node
		  // iterate through children, to get actions data
		  for (xmlNodePtr subsubnode = subnode->children; subsubnode; subsubnode = subsubnode->next)
		  {
		    if (xmlStrcmp(subsubnode->name, action_tag) == 0)
		    {
		      // action node
		      ActionData action;
		      action.label      = readXmlAttribute(subsubnode, label_tag);
		      action.icon       = readXmlAttribute(subsubnode, icon_tag);
		      action.menuText   = readXmlAttribute(subsubnode, menu_tag);
		      action.toolTip    = readXmlAttribute(subsubnode, tooltip_tag);
		      action.statusText = readXmlAttribute(subsubnode, status_tag);
		      action.accel      = readXmlAttribute(subsubnode, accel_tag);
		      if (action.label != "")
			data.actions.push_back(action);
		    } // end action node
		  } // end iteration through actions container node children
		} // end actions container node
	      } // end iterations through plugin node children
	      
	      if (data.name != "")
		info.push_back(data);
	    } // end plugin node
	  } // end iterations through plugins container node children
	} // end root node
	
	xmlFreeDoc(doc);
	//xmlCleanupParser();//vsr: xmlCleanupParser should not be called from the application
      } // end xml doc
    }
#ifdef MYDEBUG
    dumpinfo(info);
#endif
    return info;
  }
}
