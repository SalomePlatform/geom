#!/usr/bin/env python3
# Copyright (C) 2012-2024  CEA, EDF, OPEN CASCADE
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

#################################################################################
#
# File:   collect_geom_methods.py
# Author: Roman NIKOLAEV, Open CASCADE S.A.S (roman.nikolaev@opencascade.com)
#
#################################################################################
#
# Extraction of the methods dynamically added by the Geometry 
# module plug-in(s) to the geomBuilder class.
# 
# This script is intended for internal usage - only
# for generatation of the extra developer documentation for
# the Geometry module plug-in(s).
# 
# Usage:
#       collect_geom_methods.py <plugin_name>
# where
#   <plugin_name> is a name of the plug-in module
#
# Notes:
# - the script is supposed to be run in correct environment
# i.e. PYTHONPATH, GEOM_PluginsList and other important
# variables are set properly; otherwise the script will fail.
#
################################################################################

import sys
import inspect

def generate(plugin_name, output):
    def get_functions(amodule):
        for attr in dir(amodule):
          if attr.startswith( '_' ): continue # skip an internal methods
          item = getattr(amodule, attr)
          if inspect.isfunction(item):
              yield item
              pass
          pass

    plugin_module_name = plugin_name + "Builder"
    plugin_module = "salome.{}.{}".format(plugin_name, plugin_module_name)
    import_str = "from salome.{} import {}".format(plugin_name, plugin_module_name)
    execLine = "from salome.{} import {}\n" \
               "import {}\n" \
               "mod = {}".format(plugin_name, plugin_module_name, plugin_module, plugin_module)
    print(execLine)
    namespace = {}
    exec(execLine , namespace)

    for function in get_functions(namespace["mod"]):
        comments = inspect.getcomments(function)
        if comments:
            comments = comments.strip().split("\n")
            comments = "\t" + "\n\t".join(comments)
            output.append(comments)
            pass                
        sources = inspect.getsource(function)
        if sources is not None:
            sources_list = sources.split("\n")
            sources_new_list = []
            found = False
            for item in sources_list:
                if '"""' in item:
                    if found == True:
                        found = False                                
                        continue
                    else:
                        found = True
                        continue
                        pass
                    pass                
                if found == False:
                    sources_new_list.append(item)
                    pass
                pass
            sources = "\n".join(sources_new_list)
            sources = "\t" + sources.replace("\n", "\n\t")
            output.append(sources)
            pass
        pass
    pass

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    h  = "Output file (geomBuilder.py by default)"
    parser.add_argument("-o", "--output", dest="output",
                        action="store", default='geomBuilder.py', metavar="file",
                        help=h)
    h  = "If this option is True, dummy help for geomBuiler class is added. "
    h += "This option should be False (default) when building documentation for Geometry module "
    h += "and True when building documentation for Geometry module plug-ins."
    parser.add_argument("-d", "--dummy-geom-help", dest="dummygeomhelp",
                        action="store_true", default=False,
                        help=h)
    parser.add_argument("plugin", nargs='+', help='Name of plugin')
    args = parser.parse_args()

    plugins_names = " ".join(args.plugin) + 'plugin'
    if len(args.plugin) > 1:
        plugins_names += 's'
    output = []
    if args.dummygeomhelp:
        output.append("## @package geomBuilder")
        output.append("#  Documentation of the methods dynamically added by the " + plugins_names + " to the @b %geomBuilder class.")
        # Add dummy Geometry help
        # This is supposed to be done when generating documentation for Geometry module plug-ins
        output.append("#  @note The documentation below does not provide complete description of class @b %geomBuilder")
        output.append("#  from @b geomBuilder package. This documentation provides only information about")
        output.append("#  the methods dynamically added to the %geomBuilder class by the " + plugins_names + ".")
        output.append("#  For more details on the %geomBuilder class, please refer to the SALOME %Geometry module")
        output.append("#  documentation.")
        pass
    else:
        # Extend documentation for geomBuilder class with information about dynamically added methods.
        # This is supposed to be done only when building documentation for Geometry module
        output.append("## @package geomBuilder")
        output.append("#  @note Some methods are dynamically added to the @b %geomBuilder class in runtime by the")
        output.append("#  plug-in modules. If you fail to find help on some methods in the documentation of Geometry module, ")
        output.append("#  try to look into the documentation for the Geometry module plug-ins.")
        pass
    output.append("class geomBuilder():")
    
    for plugin_name in args.plugin:
       generate( plugin_name, output )
       pass

    with open(args.output, "w", encoding='utf8') as f:
        f.write('\n'.join(output))
