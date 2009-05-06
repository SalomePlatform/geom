#!/usr/bin/env python
#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import os
from salome_utils import getTmpDir, generateFileName, uniteFiles
from setenv import salome_subdir

# -----------------------------------------------------------------------------

def set_env( args ):
    """Add to the PATH-variables modules specific paths"""
    
    tmp_dir = getTmpDir()
    env_dir = generateFileName( tmp_dir, prefix="env", with_port=True )
    res_dir = os.path.join( os.getenv( "GEOM_ROOT_DIR" ), "share", salome_subdir, "resources", "geom" )

    csf_list = ["Plugin", "GEOMDS_Resources", "ShHealing"]

    for csf_file in csf_list:
       uniteFiles( os.path.join( res_dir, csf_file ), os.path.join( env_dir, csf_file ) )
       pass

    for csf_string in csf_list:
        csf_var = "CSF_" + csf_string + "Defaults"
        if not os.getenv( csf_var ):
            os.environ[ csf_var ] = env_dir
            pass
        pass

