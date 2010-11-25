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
This module defines the different structural element parts. It is used to
build the geometric shapes of the structural elements. It should not be used
directly in the general case. Structural elements should be created by the
class :class:`~salome.geom.structelem.StructuralElementManager`.
"""

import salome

from salome.kernel.logger import Logger
from salome.kernel import termcolor
logger = Logger("salome.geom.structelem.parts", color = termcolor.RED)
from salome.geom.geomtools import getGeompy

import orientation

# Filling for the beams
FULL = "FULL"
HOLLOW = "HOLLOW"

# Minimum dimension for the shapes to extrude
MIN_DIM_FOR_EXTRUDED_SHAPE = 2e-4
MIN_LENGTH_FOR_EXTRUSION = 1e-4
MIN_THICKNESS = 1e-5


class InvalidParameterError(Exception):
    """
    This exception is raised when an invalid parameter is used to build a
    structural element part.
    """
    
    def __init__(self, groupName, expression, minValue, value):
        self.groupName = groupName
        self.expression = expression
        self.minValue = minValue
        self.value = value
        
    def __str__(self):
        return "%s < %g (%s = %g in %s)" % (self.expression, self.minValue,
                                            self.expression, self.value,
                                            self.groupName)


class SubShapeID:
    """
    This class enables the use of subshapes in sets or as dictionary keys.
    It implements __eq__ and __hash__ methods so that subshapes with the same
    CORBA object `mainShape` and the same `id` are considered equal.
    """

    def __init__(self, mainShape, id):
        self._mainShape = mainShape
        self._id = id

    def getObj(self, geom):
        """
        Return the subshape (GEOM object). `geom` is a pseudo-geompy object
        used to find the geometrical object.
        """
        return geom.GetSubShape(self._mainShape, [self._id])
    
    def __eq__(self, other):
        return self._mainShape._is_equivalent(other._mainShape) and \
               self._id == other._id
    
    def __hash__(self):
        return self._mainShape._hash(2147483647) ^ self._id


class StructuralElementPart:
    """
    This class is the base class for all structural element parts. It should
    not be instantiated directly (consider it as an "abstract" class).

    :type  studyId: integer
    :param studyId: the ID of the study in which the part is created.

    :type  groupName: string
    :param groupName: the name of the underlying geometrical primitive in the
                      study.

    :type  groupGeomObj: GEOM object
    :param groupGeomObj: the underlying geometrical primitive.

    :type  parameters: dictionary
    :param parameters: parameters defining the structural element (see
                       subclasses for details).

    :type  name: string
    :param name: name to use for the created object in the study.

    """
    
    DEFAULT_NAME = "StructElemPart"

    def __init__(self, studyId, groupName, groupGeomObj, parameters,
                 name = DEFAULT_NAME):
        self._parameters = parameters
        self.groupName = groupName
        self._groupGeomObj = groupGeomObj
        self._orientation = None
        self._paramUserName = {}
        self.name = name
        self.geom = getGeompy(studyId)
        self.baseShapesSet = set()
        mainShape = self.geom.GetMainShape(groupGeomObj)
        listIDs = self.geom.GetObjectIDs(groupGeomObj)
        if mainShape is not None and listIDs is not None:
            for id in listIDs:
                self.baseShapesSet.add(SubShapeID(mainShape, id))

    def _getParameter(self, nameList, default = None):
        """
        This method finds the value of a parameter in the parameters
        dictionary. The argument is a list because some parameters can have
        several different names.
        """
        if len(nameList) > 0:
            paramName = nameList[0]
        for name in nameList:
            if self._parameters.has_key(name):
                self._paramUserName[paramName] = name
                return self._parameters[name]
        return default

    def _getParamUserName(self, paramName):
        """
        This method finds the user name for a parameter.
        """
        if self._paramUserName.has_key(paramName):
            return self._paramUserName[paramName]
        else:
            return paramName

    def __repr__(self):
        reprdict = self.__dict__.copy()
        del reprdict["_parameters"]
        del reprdict["groupName"]
        del reprdict["_groupGeomObj"]
        del reprdict["_paramUserName"]
        del reprdict["name"]
        del reprdict["geom"]
        del reprdict["baseShapesSet"]
        return '%s("%s", %s)' % (self.__class__.__name__, self.groupName,
                                 reprdict)

    def addOrientation(self, orientParams):
        """
        Add orientation information to the structural element part. See class
        :class:`~salome.geom.structelem.orientation.Orientation1D` for the description
        of the parameters.
        """
        self._orientation.addParams(orientParams)

    def _checkSize(self, value, mindim, expression):
        """
        This method checks that some parameters or some expressions involving
        those parameters are greater than a minimum value.
        """
        if value < mindim:
            raise InvalidParameterError(self.groupName, expression,
                                        mindim, value)

    def build(self):
        """
        Build the geometric shapes and the markers corresponding to the
        structural element part in the study `studyId`.
        """
        shape = self._buildPart()
        markers = self._buildMarkers()
        shape.SetColor(self._groupGeomObj.GetColor())
        for marker in markers:
            marker.SetColor(self._groupGeomObj.GetColor())
        return (shape, markers)

    def _buildPart(self):
        """
        This abstract method must be implemented in subclasses and should
        create the geometrical shape(s) of the structural element part.
        """
        raise NotImplementedError("Method _buildPart not implemented in class"
                                  " %s (it must be implemented in "
                                  "StructuralElementPart subclasses)." %
                                  self.__class__.__name__)

    def _buildMarkers(self):
        """
        This abstract method must be implemented in subclasses and should
        create the markers defining the orientation of the structural element
        part.
        """
        raise NotImplementedError("Method _buildMarker not implemented in "
                                  "class %s (it must be implemented in "
                                  "StructuralElementPart subclasses)." %
                                  self.__class__.__name__)

    def _getSubShapes(self, minDim = MIN_LENGTH_FOR_EXTRUSION):
        """
        Find and return the base subshapes in the structural element part.
        """
        subShapes = []
        for subShapeID in self.baseShapesSet:
            subShape = subShapeID.getObj(self.geom)
            length = self.geom.BasicProperties(subShape)[0]
            if length < minDim:
                logger.warning("Length too short (%s - ID %s, length = %g), "
                               "subshape will not be used in structural "
                               "element" % (self.groupName, subShapeID._id,
                                            length))
            else:
                subShapes.append(subShape)
        return subShapes


class Beam(StructuralElementPart):
    """
    This class is an "abstract" class for all 1D structural element parts. It
    should not be instantiated directly. See class
    :class:`StructuralElementPart` for the description of the parameters.
    """

    DEFAULT_NAME = "Beam"

    def __init__(self, studyId, groupName, groupGeomObj, parameters,
                 name = DEFAULT_NAME):
        StructuralElementPart.__init__(self, studyId, groupName, groupGeomObj,
                                       parameters, name)
        self._orientation = orientation.Orientation1D()

    def _isReversed(self, path):
        """
        This method checks if a 1D object is "reversed", i.e. if its
        orientation is different than the orientation of the underlying OCC
        object.
        """
        fParam = 0.
        lParam = 1.
        fPoint = self.geom.MakeVertexOnCurve(path, fParam)
        lPoint = self.geom.MakeVertexOnCurve(path, lParam)

        fNormal = self.geom.MakeTangentOnCurve(path, fParam)
        lNormal = self.geom.MakeTangentOnCurve(path, lParam)

        fCircle = self.geom.MakeCircle(fPoint, fNormal, 10)
        lCircle = self.geom.MakeCircle(lPoint, lNormal, 10)

        try:
            pipe = self.geom.MakePipeWithDifferentSections([fCircle, lCircle],
                                                           [fPoint, lPoint],
                                                           path, False, False)
        except RuntimeError, e:
            # This dirty trick is needed if the wire is not oriented in the
            # direction corresponding to parameters 0.0 -> 1.0. In this case,
            # we catch the error and invert the ends of the wire. This trick
            # will be removed when the function giving the orientation of an
            # edge will be added in geompy (see issue 1144 in PAL bugtracker).
            if (str(e) == "MakePipeWithDifferentSections : First location "
                          "shapes is not coincided with first vertex of "
                          "aWirePath"):
                return True
            else:
                raise
        return False

    def _getVertexAndTangentOnOrientedWire(self, path, param):
        """
        Get a vertex and the corresponding tangent on a wire by parameter.
        This method takes into account the "real" orientation of the wire
        (i.e. the orientation of the underlying OCC object).
        """
        if self._isReversed(path):
            vertex = self.geom.MakeVertexOnCurve(path, 1.0 - param)
            invtangent = self.geom.MakeTangentOnCurve(path, 1.0 - param)
            tanpoint = self.geom.MakeTranslationVectorDistance(vertex,
                                                               invtangent,
                                                               -1.0)
            tangent = self.geom.MakeVector(vertex, tanpoint)
        else:
            vertex = self.geom.MakeVertexOnCurve(path, param)
            tangent = self.geom.MakeTangentOnCurve(path, param)
        return (vertex, tangent)

    def _makeSolidPipeFromWires(self, wire1, wire2, point1, point2, path):
        """
        Create a solid by the extrusion of section `wire1` to section `wire2`
        along `path`.
        """
        face1 = self.geom.MakeFace(wire1, True)
        face2 = self.geom.MakeFace(wire2, True)
        shell = self.geom.MakePipeWithDifferentSections([wire1, wire2],
                                                        [point1, point2],
                                                        path, False, False)
        closedShell = self.geom.MakeShell([face1, face2, shell])
        solid = self.geom.MakeSolid([closedShell])
        return solid

    def _buildPart(self):
        """
        Build the structural element part.
        """
        # Get all the subshapes in the group (normally only edges and wires)
        paths = self._getSubShapes()
        listPipes = []
        withContact = False
        withCorrection = False
    
        for path in paths:
            # Build the sections (rectangular or circular) at each end of the
            # beam
            (fPoint, fNormal) = self._getVertexAndTangentOnOrientedWire(path,
                                                                        0.0)
            (lPoint, lNormal) = self._getVertexAndTangentOnOrientedWire(path,
                                                                        1.0)
            (outerWire1, innerWire1, outerWire2, innerWire2) = \
                    self._makeSectionWires(fPoint, fNormal, lPoint, lNormal)

            # Create the resulting solid
            outerSolid = self._makeSolidPipeFromWires(outerWire1, outerWire2,
                                                      fPoint, lPoint, path)
            if self.filling == HOLLOW:
                innerSolid = self._makeSolidPipeFromWires(innerWire1,
                                                          innerWire2, fPoint,
                                                          lPoint, path)
                resultSolid = self.geom.MakeCut(outerSolid, innerSolid)
                listPipes.append(resultSolid)
            else:
                listPipes.append(outerSolid)

        if len(listPipes) == 0:
            return None
        elif len(listPipes) == 1:
            return listPipes[0]
        else:
            return self.geom.MakeCompound(listPipes)

    def _buildMarkers(self):
        """
        Build the markers defining the orientation of the structural element
        part.
        """
        param = 0.5
        paths = self._getSubShapes()
        listMarkers = []
        for path in paths:
            (center, vecX) = self._getVertexAndTangentOnOrientedWire(path,
                                                                     param)
            marker = self._orientation.buildMarker(self.geom, center, vecX)
            listMarkers.append(marker)
        return listMarkers


class GeneralBeam(Beam):
    """
    This class defines a beam with a generic section. It is represented only
    as the underlying wire. See class :class:`StructuralElementPart` for the
    description of the parameters.
    """

    def __init__(self, studyId, groupName, groupGeomObj, parameters,
                 name = Beam.DEFAULT_NAME):
        Beam.__init__(self, studyId, groupName, groupGeomObj, parameters,
                      name)
        logger.debug(repr(self))

    def _buildPart(self):
        """
        Create a copy of the underlying wire.
        """
        edges = self._getSubShapes(1e-7)
        wire = None
        if len(edges) > 0:
            wire = self.geom.MakeWire(edges)
        return wire


class CircularBeam(Beam):
    """
    This class defines a beam with a circular section. It can be full or
    hollow, and its radius and thickness can vary from one end of the beam to
    the other. The valid parameters for circular beams are:

    * "R1" or "R": radius at the first end of the beam.
    * "R2" or "R": radius at the other end of the beam.
    * "EP1" or "EP" (optional): thickness at the first end of the beam.
      If not specified or equal to 0, the beam is considered full.
    * "EP2" or "EP" (optional): thickness at the other end of the beam.
      If not specified or equal to 0, the beam is considered full.

    See class :class:`StructuralElementPart` for the description of the
    other parameters.

    """

    def __init__(self, studyId, groupName, groupGeomObj, parameters,
                 name = Beam.DEFAULT_NAME):
        Beam.__init__(self, studyId, groupName, groupGeomObj, parameters,
                      name)

        self.R1 = self._getParameter(["R1", "R"])
        self.R2 = self._getParameter(["R2", "R"])
        self.EP1 = self._getParameter(["EP1", "EP"])
        self.EP2 = self._getParameter(["EP2", "EP"])

        if self.EP1 is None or self.EP2 is None or \
                                self.EP1 == 0 or self.EP2 == 0:
            self.filling = FULL
        else:
            self.filling = HOLLOW

        logger.debug(repr(self))

        # Check parameters
        self._checkSize(self.R1, MIN_DIM_FOR_EXTRUDED_SHAPE / 2.0,
                        self._getParamUserName("R1"))
        self._checkSize(self.R2, MIN_DIM_FOR_EXTRUDED_SHAPE / 2.0,
                        self._getParamUserName("R2"))
        if self.filling == HOLLOW:
            self._checkSize(self.EP1, MIN_THICKNESS,
                            self._getParamUserName("EP1"))
            self._checkSize(self.EP2, MIN_THICKNESS,
                            self._getParamUserName("EP2"))
            self._checkSize(self.R1 - self.EP1,
                            MIN_DIM_FOR_EXTRUDED_SHAPE / 2.0,
                            "%s - %s" % (self._getParamUserName("R1"),
                                         self._getParamUserName("EP1")))
            self._checkSize(self.R2 - self.EP2,
                            MIN_DIM_FOR_EXTRUDED_SHAPE / 2.0,
                            "%s - %s" % (self._getParamUserName("R2"),
                                         self._getParamUserName("EP2")))

    def _makeSectionWires(self, fPoint, fNormal, lPoint, lNormal):
        """
        Create the circular sections used to build the pipe.
        """
        outerCircle1 = self.geom.MakeCircle(fPoint, fNormal, self.R1)
        outerCircle2 = self.geom.MakeCircle(lPoint, lNormal, self.R2)
        if self.filling == HOLLOW:
            innerCircle1 = self.geom.MakeCircle(fPoint, fNormal,
                                                self.R1 - self.EP1)
            innerCircle2 = self.geom.MakeCircle(lPoint, lNormal,
                                                self.R2 - self.EP2)
        else:
            innerCircle1 = None
            innerCircle2 = None

        return (outerCircle1, innerCircle1, outerCircle2, innerCircle2)


class RectangularBeam(Beam):
    """
    This class defines a beam with a rectangular section. It can be full or
    hollow, and its dimensions can vary from one end of the beam to the other.
    The valid parameters for rectangular beams are:

    * "HY1", "HY", "H1" or "H": width at the first end of the beam.
    * "HZ1", "HZ", "H1" or "H": height at the first end of the beam.
    * "HY2", "HY", "H2" or "H": width at the other end of the beam.
    * "HZ2", "HZ", "H2" or "H": height at the other end of the beam.
    * "EPY1", "EPY", "EP1" or "EP" (optional): thickness in the width
      direction at the first end of the beam. If not specified or equal to 0,
      the beam is considered full.
    * "EPZ1", "EPZ", "EP1" or "EP" (optional): thickness in the height
      direction at the first end of the beam. If not specified or equal to 0,
      the beam is considered full.
    * "EPY2", "EPY", "EP2" or "EP" (optional): thickness in the width
      direction at the other end of the beam. If not specified or equal to 0,
      the beam is considered full.
    * "EPZ2", "EPZ", "EP2" or "EP" (optional): thickness in the height
      direction at the other end of the beam. If not specified or equal to 0,
      the beam is considered full.

    See class :class:`StructuralElementPart` for the description of the
    other parameters.

    """

    def __init__(self, studyId, groupName, groupGeomObj, parameters,
                 name = Beam.DEFAULT_NAME):
        Beam.__init__(self, studyId, groupName, groupGeomObj, parameters,
                      name)

        self.HY1 = self._getParameter(["HY1", "HY", "H1", "H"])
        self.HZ1 = self._getParameter(["HZ1", "HZ", "H1", "H"])
        self.HY2 = self._getParameter(["HY2", "HY", "H2", "H"])
        self.HZ2 = self._getParameter(["HZ2", "HZ", "H2", "H"])
        self.EPY1 = self._getParameter(["EPY1", "EPY", "EP1", "EP"])
        self.EPZ1 = self._getParameter(["EPZ1", "EPZ", "EP1", "EP"])
        self.EPY2 = self._getParameter(["EPY2", "EPY", "EP2", "EP"])
        self.EPZ2 = self._getParameter(["EPZ2", "EPZ", "EP2", "EP"])

        if self.EPY1 is None or self.EPZ1 is None or \
           self.EPY2 is None or self.EPZ2 is None or \
           self.EPY1 == 0 or self.EPZ1 == 0 or \
           self.EPY2 == 0 or self.EPZ2 == 0:
            self.filling = FULL
        else:
            self.filling = HOLLOW

        logger.debug(repr(self))

        # Check parameters
        self._checkSize(self.HY1, MIN_DIM_FOR_EXTRUDED_SHAPE,
                        self._getParamUserName("HY1"))
        self._checkSize(self.HZ1, MIN_DIM_FOR_EXTRUDED_SHAPE,
                        self._getParamUserName("HZ1"))
        self._checkSize(self.HY2, MIN_DIM_FOR_EXTRUDED_SHAPE,
                        self._getParamUserName("HY2"))
        self._checkSize(self.HZ2, MIN_DIM_FOR_EXTRUDED_SHAPE,
                        self._getParamUserName("HZ2"))
        if self.filling == HOLLOW:
            self._checkSize(self.EPY1, MIN_THICKNESS,
                            self._getParamUserName("EPY1"))
            self._checkSize(self.EPZ1, MIN_THICKNESS,
                            self._getParamUserName("EPZ1"))
            self._checkSize(self.EPY2, MIN_THICKNESS,
                            self._getParamUserName("EPY2"))
            self._checkSize(self.EPZ2, MIN_THICKNESS,
                            self._getParamUserName("EPZ2"))
            self._checkSize(self.HY1 - 2 * self.EPY1,
                            MIN_DIM_FOR_EXTRUDED_SHAPE,
                            "%s - 2 * %s" % (self._getParamUserName("HY1"),
                                             self._getParamUserName("EPY1")))
            self._checkSize(self.HZ1 - 2 * self.EPZ1,
                            MIN_DIM_FOR_EXTRUDED_SHAPE,
                            "%s - 2 * %s" % (self._getParamUserName("HZ1"),
                                             self._getParamUserName("EPZ1")))
            self._checkSize(self.HY2 - 2 * self.EPY2,
                            MIN_DIM_FOR_EXTRUDED_SHAPE,
                            "%s - 2 * %s" % (self._getParamUserName("HY2"),
                                             self._getParamUserName("EPY2")))
            self._checkSize(self.HZ2 - 2 * self.EPZ2,
                            MIN_DIM_FOR_EXTRUDED_SHAPE,
                            "%s - 2 * %s" % (self._getParamUserName("HZ2"),
                                             self._getParamUserName("EPZ2")))

    def _makeRectangle(self, HY, HZ, planeSect):
        """
        Create a rectangle in the specified plane.
        """
        halfHY = HY / 2.0
        halfHZ = HZ / 2.0
        sketchStr = "Sketcher:F %g" % (-halfHZ) + " %g" % (-halfHY) + ":"
        sketchStr += "TT %g" % (halfHZ) + " %g" % (-halfHY) + ":"
        sketchStr += "TT %g" % (halfHZ) + " %g" % (halfHY) + ":" 
        sketchStr += "TT %g" % (-halfHZ) + " %g" % (halfHY) + ":WW"
        logger.debug('Drawing rectangle: "%s"' % sketchStr)
        sketch = self.geom.MakeSketcherOnPlane(sketchStr, planeSect)
        return sketch

    def _makeSectionWires(self, fPoint, fNormal, lPoint, lNormal):
        """
        Create the rectangular sections used to build the pipe.
        """
        planeSect1 = self.geom.MakePlane(fPoint, fNormal, 1.0)
        outerRect1 = self._makeRectangle(self.HY1, self.HZ1, planeSect1)
        planeSect2 = self.geom.MakePlane(lPoint, lNormal, 1.0)
        outerRect2 = self._makeRectangle(self.HY2, self.HZ2, planeSect2)
        if self.filling == HOLLOW:
            innerRect1 = self._makeRectangle(self.HY1 - 2 * self.EPY1,
                                             self.HZ1 - 2 * self.EPZ1,
                                             planeSect1)
            innerRect2 = self._makeRectangle(self.HY2 - 2 * self.EPY2,
                                             self.HZ2 - 2 * self.EPZ2,
                                             planeSect2)
        else:
            innerRect1 = None
            innerRect2 = None

        return (outerRect1, innerRect1, outerRect2, innerRect2)


class StructuralElementPart2D(StructuralElementPart):
    """
    This class is an "abstract" class for all 2D structural element parts. It
    should not be instantiated directly. See class
    :class:`StructuralElementPart` for the description of the parameters.
    """

    DEFAULT_NAME = "StructuralElementPart2D"

    def __init__(self, studyId, groupName, groupGeomObj, parameters,
                 name = DEFAULT_NAME):
        StructuralElementPart.__init__(self, studyId, groupName, groupGeomObj,
                                       parameters, name)
        self._orientation = orientation.Orientation2D(
                                        self._getParameter(["angleAlpha"]),
                                        self._getParameter(["angleBeta"]),
                                        self._getParameter(["Vecteur"]))
        self.offset = self._getParameter(["Excentre"], 0.0)

    def _makeFaceOffset(self, face, offset, epsilon = 1e-6):
        """
        Create a copy of a face at a given offset.
        """
        if abs(offset) < epsilon:
            return self.geom.MakeCopy(face)
        else:
            offsetObj = self.geom.MakeOffset(face, offset)
            # We have to explode the resulting object into faces because it is
            # created as a polyhedron and not as a single face
            faces = self.geom.SubShapeAll(offsetObj,
                                          self.geom.ShapeType["FACE"])
            return faces[0]

    def _buildMarkersWithOffset(self, offset):
        """
        Build the markers for the structural element part with a given offset
        from the base face.
        """
        uParam = 0.5
        vParam = 0.5
        listMarkers = []
        subShapes = self._getSubShapes()
    
        for subShape in subShapes:
            faces = self.geom.SubShapeAll(subShape,
                                          self.geom.ShapeType["FACE"])
            for face in faces:
                offsetFace = self._makeFaceOffset(face, offset)
                # get tangent plane on surface by parameters
                center = self.geom.MakeVertexOnSurface(offsetFace,
                                                       uParam, vParam)
                tangPlane = self.geom.MakeTangentPlaneOnFace(offsetFace,
                                                             uParam, vParam,
                                                             1.0)
                normal = self.geom.GetNormal(tangPlane)
                marker = self._orientation.buildMarker(self.geom,
                                                       center, normal)
                listMarkers.append(marker)

        return listMarkers


class ThickShell(StructuralElementPart2D):
    """
    This class defines a shell with a given thickness. It can be shifted from
    the base face. The valid parameters for thick shells are:

    * "Epais": thickness of the shell.
    * "Excentre": offset of the shell from the base face.
    * "angleAlpha": angle used to build the markers (see class
      :class:`~salome.geom.structelem.orientation.Orientation2D`)
    * "angleBeta": angle used to build the markers (see class
      :class:`~salome.geom.structelem.orientation.Orientation2D`)
    * "Vecteur": vector used instead of the angles to build the markers (see
      class :class:`~salome.geom.structelem.orientation.Orientation2D`)

    See class :class:`StructuralElementPart` for the description of the
    other parameters.
    """

    DEFAULT_NAME = "ThickShell"

    def __init__(self, studyId, groupName, groupGeomObj, parameters,
                 name = DEFAULT_NAME):
        StructuralElementPart2D.__init__(self, studyId, groupName,
                                         groupGeomObj, parameters, name)
        self.thickness = self._getParameter(["Epais"])
        logger.debug(repr(self))

    def _buildPart(self):
        """
        Create the geometrical shapes corresponding to the thick shell.
        """
        subShapes = self._getSubShapes()
        listSolids = []
    
        for subShape in subShapes:
            faces = self.geom.SubShapeAll(subShape,
                                          self.geom.ShapeType["FACE"])
            for face in faces:
                shape = self._buildThickShellForFace(face)
                listSolids.append(shape)

        if len(listSolids) == 0:
            return None
        elif len(listSolids) == 1:
            return listSolids[0]
        else:
            return self.geom.MakeCompound(listSolids)

    def _buildThickShellForFace(self, face):
        """
        Create the geometrical shapes corresponding to the thick shell for a
        given face.
        """
        epsilon = 1e-6
        if self.thickness < 2 * epsilon:
            return self._makeFaceOffset(face, self.offset, epsilon)

        upperOffset = self.offset + self.thickness / 2.0
        lowerOffset = self.offset - self.thickness / 2.0
        ruledMode = True
        modeSolid = False

        upperFace = self._makeFaceOffset(face, upperOffset, epsilon)
        lowerFace = self._makeFaceOffset(face, lowerOffset, epsilon)
        listShapes = [upperFace, lowerFace]
        upperWires = self.geom.SubShapeAll(upperFace,
                                           self.geom.ShapeType["WIRE"])
        lowerWires = self.geom.SubShapeAll(lowerFace,
                                           self.geom.ShapeType["WIRE"])
        if self.geom.KindOfShape(face)[0] == self.geom.kind.CYLINDER2D:
            # if the face is a cylinder, we remove the extra side edge
            upperWires = self._removeCylinderExtraEdge(upperWires)
            lowerWires = self._removeCylinderExtraEdge(lowerWires)
        for i in range(len(upperWires)):
            resShape = self.geom.MakeThruSections([upperWires[i],
                                                   lowerWires[i]],
                                                  modeSolid, epsilon,
                                                  ruledMode)
            listShapes.append(resShape)
        resultShell = self.geom.MakeShell(listShapes)
        resultSolid = self.geom.MakeSolid([resultShell])
        return resultSolid

    def _removeCylinderExtraEdge(self, wires):
        """
        Remove the side edge in a cylinder.
        """
        result = []
        for wire in wires:
            edges = self.geom.SubShapeAll(wire, self.geom.ShapeType["EDGE"])
            for edge in edges:
                if self.geom.KindOfShape(edge)[0] == self.geom.kind.CIRCLE:
                    result.append(edge)
        return result

    def _buildMarkers(self):
        """
        Build the markers defining the orientation of the thick shell.
        """
        return self._buildMarkersWithOffset(self.offset +
                                            self.thickness / 2.0)


class Grid(StructuralElementPart2D):
    """
    This class defines a grid. A grid is represented by a 2D face patterned
    with small lines in the main direction of the grid frame. The valid
    parameters for grids are:

    * "Excentre": offset of the grid from the base face.
    * "angleAlpha": angle used to build the markers (see class
      :class:`~salome.geom.structelem.orientation.Orientation2D`)
    * "angleBeta": angle used to build the markers (see class
      :class:`~salome.geom.structelem.orientation.Orientation2D`)
    * "Vecteur": vector used instead of the angles to build the markers (see
      class :class:`~salome.geom.structelem.orientation.Orientation2D`)
    * "origAxeX": X coordinate of the origin of the axis used to determine the
      orientation of the frame in the case of a cylindrical grid.
    * "origAxeY": Y coordinate of the origin of the axis used to determine the
      orientation of the frame in the case of a cylindrical grid.
    * "origAxeZ": Z coordinate of the origin of the axis used to determine the
      orientation of the frame in the case of a cylindrical grid.
    * "axeX": X coordinate of the axis used to determine the orientation of
      the frame in the case of a cylindrical grid.
    * "axeY": Y coordinate of the axis used to determine the orientation of
      the frame in the case of a cylindrical grid.
    * "axeZ": Z coordinate of the axis used to determine the orientation of
      the frame in the case of a cylindrical grid.

    See class :class:`StructuralElementPart` for the description of the
    other parameters.
    """

    DEFAULT_NAME = "Grid"

    def __init__(self, studyId, groupName, groupGeomObj, parameters,
                 name = DEFAULT_NAME):
        StructuralElementPart2D.__init__(self, studyId, groupName,
                                         groupGeomObj, parameters, name)
        self.xr = self._getParameter(["origAxeX"])
        self.yr = self._getParameter(["origAxeY"])
        self.zr = self._getParameter(["origAxeZ"])
        self.vx = self._getParameter(["axeX"])
        self.vy = self._getParameter(["axeY"])
        self.vz = self._getParameter(["axeZ"])
        logger.debug(repr(self))

    def _buildPart(self):
        """
        Create the geometrical shapes representing the grid.
        """
        subShapes = self._getSubShapes()
        listGridShapes = []
    
        for subShape in subShapes:
            faces = self.geom.SubShapeAll(subShape,
                                          self.geom.ShapeType["FACE"])
            for face in faces:
                if self.geom.KindOfShape(face)[0] == \
                                        self.geom.kind.CYLINDER2D and \
                        self.xr is not None and self.yr is not None and \
                        self.zr is not None and self.vx is not None and \
                        self.vy is not None and self.vz is not None:
                    shape = self._buildGridForCylinderFace(face)
                else:
                    shape = self._buildGridForNormalFace(face)
                listGridShapes.append(shape)

        if len(listGridShapes) == 0:
            return None
        elif len(listGridShapes) == 1:
            return listGridShapes[0]
        else:
            return self.geom.MakeCompound(listGridShapes)

    def _buildGridForNormalFace(self, face):
        """
        Create the geometrical shapes representing the grid for a given
        non-cylindrical face.
        """
        baseFace = self._makeFaceOffset(face, self.offset)
        gridList = [baseFace]
        
        # Compute display length for grid elements
        p1 = self.geom.MakeVertexOnSurface(baseFace, 0.0, 0.0)
        p2 = self.geom.MakeVertexOnSurface(baseFace, 0.1, 0.1)
        length = self.geom.MinDistance(p1, p2) / 2.0

        for u in range(1, 10):
            uParam = u * 0.1
            for v in range(1, 10):
                vParam = v * 0.1
                # get tangent plane on surface by parameters
                center = self.geom.MakeVertexOnSurface(baseFace,
                                                       uParam, vParam)
                tangPlane = self.geom.MakeTangentPlaneOnFace(baseFace, uParam,
                                                             vParam, 1.0)
                
                # use the marker to get the orientation of the frame
                normal = self.geom.GetNormal(tangPlane)
                marker = self._orientation.buildMarker(self.geom, center,
                                                       normal, False)
                [Ox,Oy,Oz, Zx,Zy,Zz, Xx,Xy,Xz] = self.geom.GetPosition(marker)
                xPoint = self.geom.MakeTranslation(center, Xx * length,
                                                   Xy * length, Xz * length)
                gridLine = self.geom.MakeLineTwoPnt(center, xPoint)
                gridList.append(gridLine)
        grid = self.geom.MakeCompound(gridList)
        return grid

    def _buildGridForCylinderFace(self, face):
        """
        Create the geometrical shapes representing the grid for a given
        cylindrical face.
        """
        baseFace = self._makeFaceOffset(face, self.offset)
        gridList = [baseFace]
        
        # Compute display length for grid elements
        p1 = self.geom.MakeVertexOnSurface(baseFace, 0.0, 0.0)
        p2 = self.geom.MakeVertexOnSurface(baseFace, 0.1, 0.1)
        length = self.geom.MinDistance(p1, p2) / 2.0
        
        # Create reference vector V
        origPoint = self.geom.MakeVertex(self.xr, self.yr, self.zr)
        vPoint = self.geom.MakeTranslation(origPoint,
                                           self.vx, self.vy, self.vz)
        refVec = self.geom.MakeVector(origPoint, vPoint)

        for u in range(10):
            uParam = u * 0.1
            for v in range(1, 10):
                vParam = v * 0.1
                
                # Compute the local orientation of the frame
                center = self.geom.MakeVertexOnSurface(baseFace,
                                                       uParam, vParam)
                locPlaneYZ = self.geom.MakePlaneThreePnt(origPoint, center,
                                                         vPoint, 1.0)
                locOrient = self.geom.GetNormal(locPlaneYZ)
                xPoint = self.geom.MakeTranslationVectorDistance(center,
                                                                 locOrient,
                                                                 length)
                gridLine = self.geom.MakeLineTwoPnt(center, xPoint)
                gridList.append(gridLine)

        grid = self.geom.MakeCompound(gridList)
        return grid

    def _buildMarkers(self):
        """
        Create the markers defining the orientation of the grid.
        """
        return self._buildMarkersWithOffset(self.offset)


def VisuPoutreGenerale(studyId, groupName, groupGeomObj, parameters,
                       name = "POUTRE"):
    """
    Alias for class :class:`GeneralBeam`.
    """
    return GeneralBeam(studyId, groupName, groupGeomObj, parameters, name)

def VisuPoutreCercle(studyId, groupName, groupGeomObj, parameters,
                     name = "POUTRE"):
    """
    Alias for class :class:`CircularBeam`.
    """
    return CircularBeam(studyId, groupName, groupGeomObj, parameters, name)
  
def VisuPoutreRectangle(studyId, groupName, groupGeomObj, parameters,
                        name = "POUTRE"):
    """
    Alias for class :class:`RectangularBeam`.
    """
    return RectangularBeam(studyId, groupName, groupGeomObj, parameters, name)
  
def VisuBarreGenerale(studyId, groupName, groupGeomObj, parameters,
                      name = "BARRE"):
    """
    Alias for class :class:`GeneralBeam`.
    """
    return GeneralBeam(studyId, groupName, groupGeomObj, parameters, name)
      
def VisuBarreRectangle(studyId, groupName, groupGeomObj, parameters,
                       name = "BARRE"):
    """
    Alias for class :class:`RectangularBeam`.
    """
    return RectangularBeam(studyId, groupName, groupGeomObj, parameters, name)

def VisuBarreCercle(studyId, groupName, groupGeomObj, parameters,
                    name = "BARRE"):
    """
    Alias for class :class:`CircularBeam`.
    """
    return CircularBeam(studyId, groupName, groupGeomObj, parameters, name)

def VisuCable(studyId, groupName, groupGeomObj, parameters, name = "CABLE"):
    """
    Alias for class :class:`CircularBeam`.
    """
    return CircularBeam(studyId, groupName, groupGeomObj, parameters, name)

def VisuCoque(studyId, groupName, groupGeomObj, parameters, name = "COQUE"):
    """
    Alias for class :class:`ThickShell`.
    """
    return ThickShell(studyId, groupName, groupGeomObj, parameters, name)
  
def VisuGrille(studyId, groupName, groupGeomObj, parameters, name = "GRILLE"):
    """
    Alias for class :class:`Grid`.
    """
    return Grid(studyId, groupName, groupGeomObj, parameters, name)
