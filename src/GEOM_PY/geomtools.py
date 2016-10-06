# -*- coding: utf-8 -*-
#
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

## \defgroup geomtools geomtools - Tools to access GEOM engine and objects 
#  \{ 
#  \details
#  This module provides tools to facilitate the use of geom engine and geom
#  objects in Salome.
#  \}

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
from salome.kernel.services import IDToObject, IDToSObject
from salome.kernel.deprecation import is_called_by_sphinx

try:
    if not is_called_by_sphinx():
        from salome.gui import helper as guihelper
        pass
    pass
except:
    pass

_geompys = {}

## Return an object behaving exactly like geompy module, except that it is
#  associated with the study \em studyId. If \em studyId is \b None, return
#  a pseudo geompy object for the current study.
#  \ingroup geomtools
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
        from salome.geom import geomBuilder
        study = salome.myStudyManager.GetStudyByID(studyId)
        _geompys[studyId] = geomBuilder.New(study)
    return _geompys[studyId]


ModeWireFrame = 0
ModeShading = 1
DisplayMode=ModeShading
PreviewColor=[236,163,255]

## This class provides several methods to manipulate geom objects in Salome study. 
#  The parameter <em>studyEditor</em> defines a \b StudyEditor
#  object used to access the study. If \b None, the method returns a 
#  \b StudyEditor object on the current study.
#
#  \b editor  
# 
#  This instance attribute contains the underlying \b StudyEditor object. 
#  It can be used to access the study but the attribute itself should not be modified.
#  \ingroup geomtools
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

    # ======================================================================
    # Helper functions to add/remove a geometrical shape in/from the study
    # ======================================================================
    ## Add a GEOM shape in the study. It returns the associated entry
    #  that corresponds to the identifier of the entry in the study. This
    #  entry can be used to retrieve an object in the study. A folderName
    #  can be specified. In this case, a folder with this name is first
    #  created in the Geometry part of the study, and the shape study
    #  object is stored in this folder of the study. 
    #
    #    \param  shape (GEOM object) the GEOM object defining the shape
    #    \param  shapeName (string) the name for this shape in the study 
    #    \param  folderName (string) the name of a folder in the GEOM part of the study
    def addShapeToStudy(self, shape,shapeName,folderName=None):
        """
        Add a GEOM shape in the study. It returns the associated entry
        that corresponds to the identifier of the entry in the study. This
        entry can be used to retrieve an object in the study. A folderName
        can be specified. In this case, a folder with this name is first
        created in the Geometry part of the study, and the shape study
        object is stored in this folder of the study. 

        :type   shape: GEOM object
        :param  shape: the GEOM object defining the shape

        :type   shapeName: string
        :param  shapeName: the name for this shape in the study 

        :type   folderName: string
        :param  folderName: the name of a folder in the GEOM part of the study
        """
        study   = self.editor.study
        studyId = study._get_StudyId()
        geompy  = getGeompy(studyId)

        if folderName is None:
            # Insert the shape in the study by the standard way
            entry = geompy.addToStudy( shape, shapeName )
        else:
            # A folder name has been specified to embed this shape. Find
            # or create a folder with this name in the Geometry study, and
            # then store the shape in this folder.
            geomStudyFolder = self.editor.findOrCreateComponent("GEOM")
            shapeStudyFolder = self.editor.findOrCreateItem(geomStudyFolder,folderName)
            
            shapeIor = salome.orb.object_to_string(shape)
            geomgui = salome.ImportComponentGUI("GEOM")
            shapeIcon = geomgui.getShapeTypeIcon(shapeIor)
            
            shapeStudyObject = self.editor.createItem(shapeStudyFolder,
                                                      name=shapeName,
                                                      IOR=shapeIor,
                                                      icon=shapeIcon)
            entry = shapeStudyObject.GetID()

        return entry
    
    ## This removes the specified entry from the study. Note that this
    #  operation does not destroy the underlying GEOM object, neither
    #  erase the drawing in the viewer.
    #  The underlying GEOM object is returned (so that it can be destroyed)
    def removeFromStudy(self, shapeStudyEntry):
        """
        This removes the specified entry from the study. Note that this
        operation does not destroy the underlying GEOM object, neither
        erase the drawing in the viewer.
        The underlying GEOM object is returned (so that it can be destroyed)
        """
        study = self.editor.study
        studyId = study._get_StudyId()
        shape = self.getGeomObjectFromEntry(shapeStudyEntry)    
        studyObject = IDToSObject(shapeStudyEntry)
        self.editor.removeItem(studyObject,True)
        return shape

    # ======================================================================
    # Helper functions to display/erase a shape in/from the viewer. The
    # shape must be previously put in the study and the study entry must
    # be known. Note also that these function works implicitly on the
    # active study (WARN: it does not ensure consistency with the
    # study associated to the studyEditor used to initiate this
    # object. It's up to you to be self-consistent (or to improve this
    # python source code). 
    # ======================================================================

    ## Display the geometrical shape whose name in the study is <em>shapeName</em>. 
    #
    #  \param shapeName (string) name of the geometrical shape
    #  \param color (tuple) RGB components of the color of the shape
    #  \return True if the shape was found, False otherwise
    def displayShapeByName(self, shapeName, color = None, fit=True):
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
                    return self.displayShapeByEntry(entry,color,fit)
        return False

    ## Display the geometrical shape whose entry is given by \em entry. 
    #  You should prefer use this function instead of the
    #  displayShapeByName() which can have an unpredictible behavior in 
    #  the case where several objects exist with the same name in the study.
    def displayShapeByEntry(self, shapeStudyEntry, color = None, fit=True):
        """
        Display the geometrical shape whose entry is given by
        `entry`. You should prefer use this function instead of the
        displayShapeByName which can have an unpredictible behavior in
        the case where several objects exist with the same name in the
        study.
        """
        geomgui = salome.ImportComponentGUI("GEOM")
        if fit:
            geomgui.createAndDisplayFitAllGO(shapeStudyEntry)
        else:
            geomgui.createAndDisplayGO(shapeStudyEntry)
        geomgui.setDisplayMode(shapeStudyEntry, DisplayMode)
        if color is not None:
            geomgui.setColor(shapeStudyEntry, color[0], color[1], color[2])
        return True

    ## Erase the geometrical shape whose entry is given by \em entry. 
    #  Please note that the shape is just erased from the
    #  viewer. The associated study object still exists in the study,
    #  and the geom object still exists in the GEOM engine.
    def eraseShapeByEntry(self, shapeStudyEntry):
        """
        Erase the geometrical shape whose entry is given by
        `entry`. Please note that the shape is just erased from the
        viewer. The associated study object still exists in the study,
        and the geom object still exists in the GEOM engine.
        """
        geomgui = salome.ImportComponentGUI("GEOM")
        eraseFromAllWindows=True
        geomgui.eraseGO(shapeStudyEntry,eraseFromAllWindows)
        return True


    # ======================================================================
    # Helper functions for a complete suppression of a shape from the
    # SALOME session.
    # ======================================================================
    ## This completly deletes a geom shape.
    #  \warning Please be aware that to delete a geom object, 
    #  you have three operations to perform:
    #    
    #  1. erase the shape from the viewers
    #  2. remove the entry from the study
    #  3. destroy the underlying geom object
    def deleteShape(self,shapeStudyEntry):
        """
        This completly deletes a geom shape.
        
        WARNING: please be aware that to delete a geom object, you have
        three operations to perform:
        
        1. erase the shape from the viewers
        2. remove the entry from the study
        3. destroy the underlying geom object
        """
        self.eraseShapeByEntry(shapeStudyEntry)
        shape = self.removeFromStudy(shapeStudyEntry)
        shape.Destroy()

    # ======================================================================
    # Helper functions for interactivity with the object browser
    # ======================================================================
    ## Returns the GEOM object currently selected in the objects browser.
    def getGeomObjectSelected(self):
        """
        Returns the GEOM object currently selected in the objects browser.
        """
        sobject, entry = guihelper.getSObjectSelected()
        geomObject = self.getGeomObjectFromEntry(entry)
        return geomObject

    ## Returns the GEOM object associated to the specified entry,
    #  (the entry is the identifier of an item in the active study)
    def getGeomObjectFromEntry(self,entry):
        """
        Returns the GEOM object associated to the specified entry,
        (the entry is the identifier of an item in the active study)
        """
        if entry is None:
            return None
        geomObject=IDToObject(entry, self.editor.study)
        return geomObject._narrow(GEOM.GEOM_Object)

#
# ==================================================================
# Use cases and demo functions
# ==================================================================
#

# How to test?
# 1. Run a SALOME application including GEOM, and create a new study
# 2. In the console, enter:
#    >>> from salome.geom import geomtools
#    >>> geomtools.TEST_createBox()
# 3. Select the object named "box" in the browser
# 4. In the console, enter:
#    >>> geomtools.TEST_getGeomObjectSelected()

def TEST_createBox():
    geompy = getGeompy()
    box = geompy.MakeBoxDXDYDZ(200, 200, 200)
    geompy.addToStudy( box, 'box' )    
    if salome.sg.hasDesktop():
        salome.sg.updateObjBrowser(True)


def TEST_getGeomObjectSelected():
    tool = GeomStudyTools()
    myGeomObject = tool.getGeomObjectSelected()
    print myGeomObject

## This test is a simple use case that illustrates how to create a
#  GEOM shape in a SALOME session (create the GEOM object, put in in
#  the study, and display the shape in a viewer) and delete a shape
#  from a SALOME session (erase the shape from the viewer, delete the
#  entry from the study, and finally destroy the underlying GEOM
#  object).
#  \ingroup geomtools
def TEST_createAndDeleteShape():
    """
    This test is a simple use case that illustrates how to create a
    GEOM shape in a SALOME session (create the GEOM object, put in in
    the study, and display the shape in a viewer) and delete a shape
    from a SALOME session (erase the shape from the viewer, delete the
    entry from the study, and finally destroy the underlying GEOM
    object).
    """
    import salome
    salome.salome_init()
    study   = salome.myStudy
    studyId = salome.myStudyId

    from salome.geom import geomtools
    geompy = geomtools.getGeompy(studyId)
    
    from salome.kernel.studyedit import getStudyEditor
    studyEditor = getStudyEditor(studyId)

    gst = geomtools.GeomStudyTools(studyEditor)

    # --------------------------------------------------
    # Create a first shape (GEOM object)
    radius = 5
    length = 100
    cylinder = geompy.MakeCylinderRH(radius, length)

    # Register the shape in the study, at the root of the GEOM
    # folder. A name must be specified. The register operation
    # (addShapeToStudy) returns an identifier of the entry in the study.
    cylinderName = "cyl.r%s.l%s"%(radius,length)
    cylinderStudyEntry = gst.addShapeToStudy(cylinder, cylinderName)

    # Display the registered shape in a viewer
    gst.displayShapeByEntry(cylinderStudyEntry)

    # --------------------------------------------------
    # A second shape
    radius = 10
    sphere = geompy.MakeSphereR(radius)
    sphereName = "sph.r%s"%radius
    sphereStudyEntry = gst.addShapeToStudy(sphere, sphereName)
    gst.displayShapeByEntry(sphereStudyEntry)

    # --------------------------------------------------
    # This new shape is stored in the study, but in a specific
    # sub-folder, and is displayed in the viewer with a specific
    # color.
    length = 20
    box = geompy.MakeBoxDXDYDZ(length,length,length)
    boxName = "box.l%s"%length
    folderName = "boxset" 
    boxStudyEntry = gst.addShapeToStudy(box, boxName, folderName)
    gst.displayShapeByEntry(boxStudyEntry,PreviewColor)

    # --------------------------------------------------
    # In this example, we illustrate how to erase a shape (the sphere)
    # from the viewer. After this operation, the sphere is no longer
    # displayed but still exists in the study. You can then redisplay
    # it using the context menu of the SALOME object browser.
    gst.eraseShapeByEntry(sphereStudyEntry)

    # --------------------------------------------------
    # In this last example, we completly delete an object from the
    # SALOME session (erase from viewer, remove from study and finnaly
    # destroy the object). This is done by a simple call to
    # deleteShape().
    gst.deleteShape(cylinderStudyEntry)

    # --------------------------------------------------
    # At the end of the executioon of this test, you should have in
    # the SALOME session:
    # - the box, in a dedicated folder of the study, and displayed in the viewer
    # - the sphere, in the standard place of the study, and not displayed 

    # If you comment the deleteShape line, you should see the cylinder
    # in the study and displayed in the viewer. 

if __name__ == "__main__":
    #TEST_getGeomObjectSelected()
    TEST_createAndDeleteShape()


    
