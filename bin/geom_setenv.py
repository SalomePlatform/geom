#! /usr/bin/env python
#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import os, sys, string
from salome_utils import getLogDir, generateFileName, uniteFiles
from setenv import add_path, get_lib_dir, salome_subdir

# -----------------------------------------------------------------------------

def set_env( args ):
    """Add to the PATH-variables modules specific paths"""
    psep = os.pathsep
    python_version="python%d.%d" % sys.version_info[0:2]

    tmp_dir = getLogDir()
    env_dir = generateFileName( tmp_dir, prefix="env", with_port=True )
    res_dir = os.path.join( os.getenv( "GEOM_ROOT_DIR" ), "share", salome_subdir, "resources", "geom" )

    csf_list = ["Plugin", "GEOMDS_Resources", "ShHealing"]

    for csf in csf_list:
        uniteFiles( os.path.join( res_dir, csf ), os.path.join( env_dir, csf ) )
        csf_var = "CSF_%sDefaults" % csf
        if not os.getenv( csf_var ):
            os.environ[ csf_var ] = env_dir
            pass
        pass

    # collect plugins
    plugin_list = []
    resource_path_list = []

    # standard plugins
    plugin_list.append("BREPPlugin")
    plugin_list.append("STEPPlugin")
    plugin_list.append("IGESPlugin")
    plugin_list.append("STLPlugin")
    plugin_list.append("XAOPlugin")
    plugin_list.append("VTKPlugin")
    plugin_list.append("AdvancedGEOM")

    # find additional plugins
    for env_var in os.environ.keys():
        value = os.environ[env_var]
        if env_var[-9:] == "_ROOT_DIR" and value:
            plugin_root = value
            plugin = env_var[:-9] # plugin name may have wrong case

            # look for NAMEOFPlugin.xml file among resource files
            resource_dir = os.path.join(plugin_root, "share", salome_subdir, "resources", plugin.lower())
            if not os.access(resource_dir, os.F_OK): continue

            for resource_file in os.listdir(resource_dir):
                if resource_file.endswith(".xml") and \
                   resource_file.lower() == plugin.lower() + ".xml":
                    # use "name" attribute of "geom-plugin" as name of plugin in a right case
                    try:
                        from xml.dom.minidom import parse
                        xml_doc = parse(os.path.join(resource_dir, resource_file))
                        plugin_nodes = xml_doc.getElementsByTagName("geom-plugin")
                        if not plugin_nodes or not plugin_nodes[0].hasAttribute("name"): continue

                        plugin = plugin_nodes[0].getAttribute("name")
                        if plugin in plugin_list: continue

                        plugin_list.append(plugin)

                        # add paths of plugin
                        if not os.environ.has_key("SALOME_"+plugin+"Resources"):
                            resource_path = os.path.join(plugin_root, "share", salome_subdir, "resources", plugin.lower())
                            os.environ["SALOME_"+plugin+"Resources"] = resource_path
                            resource_path_list.append(resource_path)
                            add_path(os.path.join(plugin_root, get_lib_dir(), python_version, "site-packages", salome_subdir), "PYTHONPATH")
                            add_path(os.path.join(plugin_root, get_lib_dir(), salome_subdir), "PYTHONPATH")
                            if sys.platform == "win32":
                                add_path(os.path.join(plugin_root, get_lib_dir(), salome_subdir), "PATH")
                                add_path(os.path.join(plugin_root, "bin", salome_subdir), "PYTHONPATH")
                            else:
                                add_path(os.path.join(plugin_root, get_lib_dir(), salome_subdir), "LD_LIBRARY_PATH")
                                add_path(os.path.join(plugin_root, "bin", salome_subdir), "PYTHONPATH")
                                add_path(os.path.join(plugin_root, "bin", salome_subdir), "PATH")
                                pass
                            pass
                        pass
                    except:
                        continue
                    pass
                pass
            pass
        pass

    os.environ["GEOM_PluginsList"] = ":".join(plugin_list)
    os.environ["SalomeAppConfig"] = os.environ["SalomeAppConfig"] + psep + psep.join(resource_path_list)
    pass
