# -*- coding: utf-8 -*-
#
#  Copyright (C) 2007-2009     EDF R&D
# 
#    This file is part of PAL_SRC.
#
#    PAL_SRC is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    PAL_SRC is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with PAL_SRC; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
#
"""
This module provides tools to facilitate the use of geom engine and geom
objects in Salome.
"""

import salome
GEOM = None    # GEOM module is loaded only when needed

from salome.kernel.logger import Logger
from salome.kernel import termcolor
logger = Logger("salome.geom.geomtools", color = termcolor.RED)

from salome.kernel.studyedit import getActiveStudyId, getStudyEditor

_geompys = {}

def getGeompy(studyId = None):
    """
    Return an object behaving exactly like geompy module, except that it is
    associated with the study `studyId`. If `studyId` is :const:`None`, return
    a pseudo geompy object for the current study.
    """
    # We can't use geompy module because it initializes GEOM with
    # salome.myStudy, which may not exist. So we use this trick to create
    # a pseudo geompy module. 
    salome.salome_init()
    if studyId is None:
        studyId = getActiveStudyId()
    if not _geompys.has_key(studyId):
        import geompyDC
        _geompys[studyId] = salome.lcc.FindOrLoadComponent("FactoryServer",
                                                           "GEOM")
        _geompys[studyId].ShapeType = geompyDC.ShapeType
        _geompys[studyId].GEOM = geompyDC.GEOM
        _geompys[studyId].kind = geompyDC.kind
        _geompys[studyId].info = geompyDC.info
        _geompys[studyId].PackData = geompyDC.PackData
        _geompys[studyId].ReadTexture = geompyDC.ReadTexture
        study = salome.myStudyManager.GetStudyByID(studyId)
        _geompys[studyId].init_geom(study)
    return _geompys[studyId]


class GeomStudyTools:
    """
    This class provides several methods to manipulate geom objects in Salome
    study. The parameter `studyEditor` defines a
    :class:`~salome.kernel.studyedit.StudyEditor` object used to access the study. If
    :const:`None`, the method returns a :class:`~salome.kernel.studyedit.StudyEditor`
    object on the current study.

    .. attribute:: editor
    
       This instance attribute contains the underlying
       :class:`~salome.kernel.studyedit.StudyEditor` object. It can be used to access
       the study but the attribute itself should not be modified.

    """

    def __init__(self, studyEditor = None):
        global GEOM
        if GEOM is None:
            GEOM = __import__("GEOM")
        if studyEditor is None:
            studyEditor = getStudyEditor()
        self.editor = studyEditor

    def displayShapeByName(self, shapeName, color = None):
        """
        Display the geometrical shape whose name in the study is `shapeName`.
        
        :type   shapeName: string
        :param  shapeName: name of the geometrical shape
        
        :type   color: tuple (triplet)
        :param  color: RGB components of the color of the shape
        
        :return: True if the shape was found, False otherwise
        """
        logger.debug("displayShapeByName in PAL: %s with color %s" %
                     (shapeName, color))
        listSO = self.editor.study.FindObjectByName(shapeName, "GEOM")
        for sObj in listSO:
            entry = sObj.GetID()
            geomObj = self.editor.getOrLoadObject(sObj)
            if geomObj:
                shape = geomObj._narrow(GEOM.GEOM_Object)
                if shape:                
                    geomgui = salome.ImportComponentGUI("GEOM")            
                    geomgui.createAndDisplayGO(entry)
                    geomgui.setDisplayMode(entry, 1)
                    if color is not None:
                        geomgui.setColor(entry, color[0], color[1], color[2])
                    return True
        return False
