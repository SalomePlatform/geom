# -*- coding: utf-8 -*-
#
#  Copyright (C) 2007-2009	 EDF R&D
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
This package is used to create and visualize structural elements. It contains
three modules:

* This module :mod:`salome.geom.structelem` defines the main classes
  :class:`StructuralElement` and :class:`StructuralElementManager` that can be
  directly used to build structural elements.
* The module :mod:`salome.geom.structelem.parts` defines the classes corresponding to
  the different parts (beams, grids, etc.) that make up a structural element.
  It is used to build the geometric shapes in the structural element.
* The module :mod:`salome.geom.structelem.orientation` defines the classes that are
  used to compute the orientation of the structural element parts and to build
  the corresponding markers.

A structural element is a set of geometric shapes (beams, grids, etc.) that
are built semi-automatically along a set of geometric primitives (edges for
instance). They are visualized with the same color as their base primitives in
the geom viewer.

Structural elements are generally created by the
:class:`StructuralElementManager` class, from a list of commands describing
the element to create.

Example::

    commandList = [('VisuPoutreGenerale', {'Group_Maille': 'Edge_1'}),
                   ('VisuBarreCercle',
                    {'R': 30, 'Group_Maille': 'Edge_1', 'EP': 15}),
                  ]

    structElemManager = StructuralElementManager()
    elem = structElemManager.createElement(commandList)
    elem.display()
    salome.sg.updateObjBrowser(True)

"""

import types

import salome

from salome.kernel.logger import Logger
from salome.kernel import termcolor
logger = Logger("salome.geom.structelem", color = termcolor.RED)
from salome.kernel.studyedit import getStudyEditor

__all__ = ["parts", "orientation"]

from salome.geom.structelem import parts
from salome.geom.structelem.parts import InvalidParameterError

class StructuralElementManager:
    """
    This class manages the structural elements in the study. It is used to
    create a new structural element from a list of commands. The parameter
    `studyId` defines the ID of the study in which the manager will create
    structural elements. If it is :const:`None` or not specified, it will use
    the ID of the current study as defined by
    :func:`salome.kernel.studyedit.getActiveStudyId` function.
    """
    def __init__(self, studyId = None):
        self._studyEditor = getStudyEditor(studyId)

    def createElement(self, commandList):
        """
        Create a structural element from the list of commands `commandList`.
        Each command in this list represent a part of the structural element,
        that is a specific kind of shape (circular beam, grid, etc.)
        associated with one or several geometrical primitives. A command must
        be a tuple. The first element is the structural element part class
        name or alias name. The second element is a dictionary containing the
        parameters describing the part. Valid class names are all the classes
        defined in the module :mod:`~salome.geom.structelem.parts` and inheriting
        class :class:`~parts.StructuralElementPart`. There are also several
        aliases for backward compatibility. Here is the complete list:
        
        * :class:`~parts.GeneralBeam`
        * :class:`~parts.CircularBeam`
        * :class:`~parts.RectangularBeam`
        * :class:`~parts.ThickShell`
        * :class:`~parts.Grid`

        * :func:`~parts.VisuPoutreGenerale` (alias for
          :class:`~parts.GeneralBeam`)
        * :func:`~parts.VisuPoutreCercle` (alias for
          :class:`~parts.CircularBeam`)
        * :func:`~parts.VisuPoutreRectangle` (alias for
          :class:`~parts.RectangularBeam`)
        * :func:`~parts.VisuBarreGenerale` (alias for
          :class:`~parts.GeneralBeam`)
        * :func:`~parts.VisuBarreRectangle` (alias for
          :class:`~parts.RectangularBeam`)
        * :func:`~parts.VisuBarreCercle` (alias for
          :class:`~parts.CircularBeam`)
        * :func:`~parts.VisuCable` (alias for :class:`~parts.CircularBeam`)
        * :func:`~parts.VisuCoque` (alias for :class:`~parts.ThickShell`)
        * :func:`~parts.VisuGrille` (alias for :class:`~parts.Grid`)
        
        * ``Orientation``: This identifier is used to specify the orientation
          of one or several 1D structural element parts (i.e. beams). The
          parameters are described in class
          :class:`~orientation.Orientation1D`.

        The valid parameters in the dictionary depend on the type of the
        structural element part, and are detailed in the documentation of
        the corresponding class. The only parameter that is common to all the
        classes is "MeshGroups" (that can also be named "Group_Maille"). It
        defines the name of the geometrical object(s) in the study that will
        be used as primitives to build the structural element part. This
        parameter can be either a list of strings or a single string with
        comma separated names.
        """
        logger.debug("StructuralElementManager.createElement: START")
        logger.debug("Command list: %s" % commandList)

        element = StructuralElement(self._studyEditor.studyId)
        orientationCmdList = []
        for command in commandList:
            (parttype, parameters) = command
            if parttype == "Orientation":
                orientationCmdList += [command]
            elif parttype not in dir(parts):
                logger.warning('Invalid structural element part name "%s"'
                               ' in command %s, this command will be '
                               'ignored.' % (parttype, command))
            else:
                (meshGroupList, newparams) = self._extractMeshGroups(command)
                for meshGroup in meshGroupList:
                    # Get the geometrical primitive object
                    groupSObj = self._studyEditor.study.FindObject(meshGroup)
                    groupGeomObj = None
                    if groupSObj is not None:
                        groupGeomObj = \
                                self._studyEditor.getOrLoadObject(groupSObj)
                    if groupGeomObj is None:
                        logger.error("Can't get geom object corresponding to "
                                     'mesh group "%s", structural element '
                                     "part %s will not be built." %
                                     (groupName, part))
                        continue
                    
                    # Create the part
                    try:
                        part = parts.__dict__[parttype](
                                        self._studyEditor.studyId, meshGroup,
                                        groupGeomObj, newparams)
                        element.addPart(part)
                    except InvalidParameterError, e:
                        logger.error("Invalid parameter error: %s" % e)
                        raise
                    except:
                        logger.exception("Can't create structural element"
                                         " part with command %s." %
                                         str(command))

        # Orientations are parsed after the parts because they must be
        # associated with existing parts.
        for command in orientationCmdList:
            (parttype, parameters) = command
            (meshGroupList, orientParams) = self._extractMeshGroups(command)
            for meshGroup in meshGroupList:
                element.addOrientation(meshGroup, orientParams)

        element.build()
        logger.debug("StructuralElementManager.createElement: END")
        return element
    
    def _extractMeshGroups(self, command):
        """
        This method extracts the names of the mesh groups (i.e. the
        geometrical objects used to build the structural element part) in the
        command in parameter. It returns a tuple containing the mesh groups as
        a list of strings and the other parameters of the command as a new
        dictionary.
        """
        (parttype, parameters) = command
        newparams = parameters.copy()
        groupMailleParam = newparams.pop("Group_Maille", None)
        meshGroupParam = newparams.pop("MeshGroups", None)
        if groupMailleParam is None and meshGroupParam is None:
            logger.warning("No mesh group specified in command %s, this "
                           "command will be ignored." % command)
            return ([], newparams)
        elif groupMailleParam is not None and meshGroupParam is not None:
            logger.warning('Both "MeshGroups" and "Group_Maille" specified in'
                           ' command %s, only "MeshGroups" will be used.' %
                           command)
        elif groupMailleParam is not None and meshGroupParam is None:
            meshGroupParam = groupMailleParam
        
        meshGroupList = []
        if type(meshGroupParam) == types.StringType:
            meshGroupList = self._getMeshGroupListFromString(meshGroupParam)
        else:
            for item in meshGroupParam:
                meshGroupList += self._getMeshGroupListFromString(item)
        
        if len(meshGroupList) == 0:
            logger.warning("Mesh group list is empty in command %s, this "
                           "command will be ignored." % command)

        return (meshGroupList, newparams)
    
    def _getMeshGroupListFromString(self, meshString):
        """
        This method splits the string in parameter to extract comma separated
        names. Those names are returned as a list of strings.
        """
        meshGroupList = []
        list = meshString.split(",")
        for item in list:
            strippedItem = item.strip()
            if len(strippedItem) > 0:
                meshGroupList.append(strippedItem)
        return meshGroupList


class StructuralElement:
    """
    This class represents a structural element, i.e. a set of geometrical
    objects built along geometrical primitives. The parameter `studyId`
    defines the ID of the study that will contain the structural element. If
    it is :const:`None` or not specified, the constructor will use the ID of
    the active study as defined by :func:`salome.kernel.studyedit.getActiveStudyId`
    function. Structural elements are normally created by the class
    :class:`StructuralElementManager`, so this class should not be
    instantiated directly in the general case.
    """
    _counter = 1
    _mainFolderTag = 14725

    def __init__(self, studyId = None):
        # _parts is the dictionary mapping group name to structural element
        # part. _shapeDict is the dictionary mapping SubShapeID objects to
        # structural element parts. Both are used to avoid duplicate shapes
        # in structural elements.
        self._parts = {}
        self._shapeDict = {}
        self._id = StructuralElement._counter
        StructuralElement._counter += 1
        self._studyEditor = getStudyEditor(studyId)
        logger.debug("Creating structural element in study %s" %
                     self._studyEditor.studyId)
        self._SObject = None

    def _getSObject(self):
        """
        Find or create the study object corresponding to the structural
        element. This object is named "SE_N" where N is a numerical ID. 
        """
        if self._SObject is None:
            geomComponent = self._studyEditor.study.FindComponent("GEOM")
            mainFolder = self._studyEditor.setItemAtTag(geomComponent,
                                            StructuralElement._mainFolderTag,
                                            name = "Structural Elements")
            self._SObject = self._studyEditor.findOrCreateItem(mainFolder,
                                            name = "SE_" + str(self._id))
        return self._SObject

    def addPart(self, newpart):
        """
        Add a part to the structural element.

        :type  newpart: :class:`~parts.StructuralElementPart`
        :param newpart: the part to add to the structural element.

        """
        newshapes = newpart.baseShapesSet

        # Check duplicate groups
        if self._parts.has_key(newpart.groupName):
            logger.warning('Mesh group "%s" is used several times in the '
                           'structural element. Only the last definition '
                           'will be used.' % newpart.groupName)
        else:
            # Check duplicate shapes
            intersect = newshapes.intersection(self._shapeDict.keys())
            while len(intersect) > 0:
                shape, = intersect
                oldpartwithshape = self._shapeDict[shape]
                oldpartshapes = oldpartwithshape.baseShapesSet
                intersectwitholdpart = intersect.intersection(oldpartshapes)
                logger.warning('Some shapes are common to groups "%s" and '
                               '"%s". For those, the parameters defined for '
                               '"%s" will be used.' %
                               (oldpartwithshape.groupName, newpart.groupName,
                                newpart.groupName))
                oldpartwithshape.baseShapesSet = \
                                oldpartshapes.difference(intersectwitholdpart)
                intersect = intersect.difference(intersectwitholdpart)

        # Finally add the new part in the structural element
        self._parts[newpart.groupName] = newpart
        for shape in newshapes:
            self._shapeDict[shape] = newpart

    def addOrientation(self, meshGroup, orientParams):
        """
        Add orientation information to a part in the structural element. This
        information will be used to build the corresponding markers.

        :type  meshGroup: string
        :param meshGroup: the name of a geometrical primitive. The orientation
                          information will apply to the structural element
                          part built along this primitive.

        :type  orientParams: dictionary
        :param orientParams: parameters defining the orientation of the
                             structural element part. Those parameters are
                             detailed in class
                             :class:`~orientation.Orientation1D`.

        """
        if self._parts.has_key(meshGroup):
            self._parts[meshGroup].addOrientation(orientParams)
        else:
            logger.warning('Mesh group "%s" not found in structural element, '
                           'cannot set orientation.' % meshGroup)

    def build(self):
        """
        Build the geometric shapes and the markers corresponding to the
        different parts of the structural element, and add them to the study.
        """
        gg = salome.ImportComponentGUI("GEOM")
        for part in self._parts.itervalues():
            # Build the structural element part
            logger.debug("Building %s" % part)
            try:
                (shape, markers) = part.build()
                if shape is None:
                    logger.error("Part %s has not been built" % part)
                    continue
            except:
                logger.exception("Couldn't build part %s" % part)
                continue
            
            # Add the new objects to the study
            IOR = self._studyEditor.study.ConvertObjectToIOR(shape)
            shapeSObjName = part.name + "_" + part.groupName
            icon = None
            if salome.hasDesktop():
                icon = gg.getShapeTypeIcon(IOR)
            shapeSObj = self._studyEditor.createItem(self._getSObject(),
                                            name = shapeSObjName, IOR = IOR,
                                            icon = icon)
            if markers is not None and len(markers) > 0:
                i = 1
                for marker in markers:
                    markerIOR = \
                            self._studyEditor.study.ConvertObjectToIOR(marker)
                    markerSObjName = "Orient_" + shapeSObjName
                    if len(markers) > 1:
                        markerSObjName += "_%d" % i
                    markerSObj = self._studyEditor.createItem(
                                                self._getSObject(),
                                                name = markerSObjName,
                                                IOR = markerIOR,
                                                icon = "ICON_OBJBROWSER_LCS")
                    i += 1

    def display(self):
        """
        Display the structural element in the geom view.
        """
        StructuralElement.showElement(self._SObject)

    @staticmethod
    def showElement(theSObject):
        """
        Display the structural element corresponding to the study object
        `theSObject`
        """
        if theSObject is not None:
            gg = salome.ImportComponentGUI("GEOM")
            aStudy = theSObject.GetStudy()
            editor = getStudyEditor(aStudy._get_StudyId())
            aIterator = aStudy.NewChildIterator(theSObject)
            aIterator.Init()
            while aIterator.More():
                sobj = aIterator.Value()
                icon = editor.getIcon(sobj)
                if icon != "ICON_OBJBROWSER_LCS":
                    entry = aIterator.Value().GetID()
                    gg.createAndDisplayGO(entry)
                    gg.setDisplayMode(entry, 1)
                aIterator.Next()


def TEST_CreateGeometry():
    import geompy
    import SALOMEDS
    geompy.init_geom(salome.myStudy)
    Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 200)
    edges = geompy.SubShapeAllSorted(Box_1, geompy.ShapeType["EDGE"])
    edges[0].SetColor(SALOMEDS.Color(1.0,0.0,0.0))
    edges[1].SetColor(SALOMEDS.Color(0.0,1.0,0.0))
    edges[2].SetColor(SALOMEDS.Color(0.0,0.0,1.0))
    edges[3].SetColor(SALOMEDS.Color(1.0,0.0,1.0))
    edges[4].SetColor(SALOMEDS.Color(0.0,1.0,1.0))
    edges[5].SetColor(SALOMEDS.Color(0.5,0.0,0.0))
    edges[6].SetColor(SALOMEDS.Color(0.0,0.5,0.0))
    edges[7].SetColor(SALOMEDS.Color(0.0,0.0,0.5))
    geompy.addToStudy(Box_1, "Box_1")
    for i in range(len(edges)):
        geompy.addToStudyInFather(Box_1, edges[i], "Edge_%d" % i)
    faces = geompy.SubShapeAllSorted(Box_1, geompy.ShapeType["FACE"])
    faces[3].SetColor(SALOMEDS.Color(1.0,0.5,0.0))
    faces[4].SetColor(SALOMEDS.Color(0.0,1.0,0.5))
    for i in range(len(faces)):
        geompy.addToStudyInFather(Box_1, faces[i], "Face_%d" % i)
    Cylinder_1 = geompy.MakeCylinderRH(50, 200)
    geompy.TranslateDXDYDZ(Cylinder_1, 300, 300, 0)
    cyl_faces = geompy.SubShapeAllSorted(Cylinder_1, geompy.ShapeType["FACE"])
    geompy.addToStudy(Cylinder_1, "Cylinder_1")
    for i in range(len(cyl_faces)):
        geompy.addToStudyInFather(Cylinder_1, cyl_faces[i], "CylFace_%d" % i)
    Cylinder_2 = geompy.MakeTranslation(Cylinder_1, 100, 100, 0)
    cyl_faces2 = geompy.SubShapeAllSorted(Cylinder_2,
                                          geompy.ShapeType["FACE"])
    geompy.addToStudy(Cylinder_2, "Cylinder_2")
    for i in range(len(cyl_faces2)):
        geompy.addToStudyInFather(Cylinder_2, cyl_faces2[i],
                                  "CylFace2_%d" % i)


def TEST_StructuralElement():
    salome.salome_init()
    TEST_CreateGeometry()
    liste_commandes = [('Orientation', {'MeshGroups': 'Edge_4',
                                        'VECT_Y': (1.0, 0.0, 1.0)}),
                       ('Orientation', {'MeshGroups': 'Edge_5',
                                        'ANGL_VRIL': 45.0}),
                       ('GeneralBeam', {'MeshGroups': 'Edge_1, Edge_7'}),
                       ('VisuPoutreCercle', {'MeshGroups': ['Edge_6'],
                                             'R1': 30, 'R2': 20}),
                       ('CircularBeam', {'MeshGroups': ['Edge_2', 'Edge_3'],
                                         'R': 40, 'EP': 20}),
                       ('RectangularBeam', {'MeshGroups': 'Edge_4, Edge_5',
                                            'HZ1': 60, 'HY1': 40,
                                            'EPZ1': 15, 'EPY1': 10,
                                            'HZ2': 40, 'HY2': 60,
                                            'EPZ2': 10, 'EPY2': 15}),
                       ('VisuCable', {'MeshGroups': 'Edge_7', 'R': 5}),
                       ('VisuCoque', {'MeshGroups': 'Face_4',
                                      'Epais': 10, 'Excentre': 5,
                                      'angleAlpha': 45, 'angleBeta': 60}),
                       ('VisuCoque', {'MeshGroups': 'CylFace_2', 'Epais': 5}),
                       ('VisuGrille', {'MeshGroups': 'Face_5', 'Excentre': 5,
                                       'angleAlpha': 45, 'angleBeta': 60}),
                       ('VisuGrille', {'MeshGroups': 'CylFace2_2',
                                       'Excentre': 5, 'origAxeX': 400,
                                       'origAxeY': 400, 'origAxeZ': 0,
                                       'axeX': 0, 'axeY': 0, 'axeZ': 100}),
                      ]

    structElemManager = StructuralElementManager()
    elem = structElemManager.createElement(liste_commandes)
    if salome.hasDesktop():
        elem.display()
        salome.sg.updateObjBrowser(True)


# Main function only used to test the module
if __name__ == "__main__":
    TEST_StructuralElement()