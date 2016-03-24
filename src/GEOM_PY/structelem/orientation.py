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

## \defgroup orientation orientation
#  \{ 
#  \details
#  This module is used to compute the orientation of the different parts in a
#  structural element and to build the corresponding markers (trihedrons).
#  \}

"""
This module is used to compute the orientation of the different parts in a
structural element and to build the corresponding markers (trihedrons).
"""

import math

from salome.kernel.logger import Logger
from salome.kernel import termcolor
logger = Logger("salome.geom.structelem.orientation", color = termcolor.RED)

## This class is used to compute the orientation of 1D elements and to build
#  the corresponding markers.
#  \ingroup orientation
class Orientation1D:
    """
    This class is used to compute the orientation of 1D elements and to build
    the corresponding markers.
    """
    
    def __init__(self):
        self.geom = None
        self._vectorYCoords = None
        self._angle = 0.0

    def __repr__(self):
        reprdict = self.__dict__.copy()
        del reprdict["geom"]
        return '%s(%s)' % (self.__class__.__name__, reprdict)

    ## Add orientation parameters. \em params is a dictionary containing one or
    #  several orientation parameters. The valid parameters are:
    #  - "VECT_Y": Triplet defining the local Y axis (the X axis is the
    #  main direction of the 1D element).
    #  - "ANGL_VRIL": Angle of rotation along the X axis to define the local
    #  coordinate system.
    #  The parameters can be specified several times. In this case, only the
    #  last "VECT_Y" or "ANGL_VRIL" is taken into account.
    def addParams(self, params):
        """
        Add orientation parameters. `params` is a dictionary containing one or
        several orientation parameters. The valid parameters are:

        * "VECT_Y": Triplet defining the local Y axis (the X axis is the
          main direction of the 1D element).
        * "ANGL_VRIL": Angle of rotation along the X axis to define the local
          coordinate system.
        
        The parameters can be specified several times. In this case, only the
        last "VECT_Y" or "ANGL_VRIL" is taken into account.
        """
        if self._vectorYCoords is not None or self._angle != 0.0:
            logger.warning('Orientation parameters are specified several '
                           'times for the same mesh group, only the last '
                           'parameter will be used')
        mydict = params.copy()
        if mydict.has_key("VECT_Y"):
            newVecCoords = mydict.pop("VECT_Y")
            logger.debug("Setting orientation vector Y to %s" %
                             str(newVecCoords))
            self._vectorYCoords = newVecCoords
            self._angle = 0.0
        if mydict.has_key("ANGL_VRIL"):
            newAngle = mydict.pop("ANGL_VRIL")
            logger.debug("Setting orientation angle to %f" % newAngle)
            self._angle = newAngle
            self._vectorYCoords = None
        if len(mydict) > 0:
            logger.warning("Invalid orientation parameter(s) (ignored): %s" %
                           str(mydict))
    ## Get the vectors Y and Z for the default LCS, that use the main
    #  direction of the 1D object as the local X axis and the global Z axis
    #  to determine the local Z axis.
    def _getDefaultVecYZ(self, center, vecX):
        """
        Get the vectors Y and Z for the default LCS, that use the main
        direction of the 1D object as the local X axis and the global Z axis
        to determine the local Z axis.
        """
        xPoint = self.geom.MakeTranslationVector(center, vecX)
        givenVecZ = self.geom.MakeVectorDXDYDZ(0.0, 0.0, 1.0)
        angle = self.geom.GetAngleRadians(vecX, givenVecZ)
        if abs(angle) < 1e-7 or abs(angle - math.pi) < 1e-7:
            logger.warning("Beam X axis is colinear to absolute Z axis. "
                           "Absolute X axis will be used to determine "
                           "local Z axis.")
            givenVecZ = self.geom.MakeVectorDXDYDZ(1.0, 0.0, 0.0)
        zPoint = self.geom.MakeTranslationVector(center, givenVecZ)
        locPlaneZX = self.geom.MakePlaneThreePnt(center, zPoint, xPoint, 1.0)
        locY = self.geom.GetNormal(locPlaneZX)
        yPoint = self.geom.MakeTranslationVector(center, locY)
        locPlaneXY = self.geom.MakePlaneThreePnt(center, xPoint, yPoint, 1.0)
        locZ = self.geom.GetNormal(locPlaneXY)
        return (locY, locZ)
    
    ## Create a marker with origin \em center and X axis \em vecX. 
    #  \em geom is the pseudo-geompy object used to build the geometric shapes.
    def buildMarker(self, geom, center, vecX):
        """
        Create a marker with origin `center` and X axis `vecX`. `geom` is the
        pseudo-geompy object used to build the geometric shapes.
        """
        (locY, locZ) = self.getVecYZ(geom, center, vecX)
        marker = geom.MakeMarkerPntTwoVec(center, vecX, locY)
        return marker

    ## Get the vectors Y and Z for the LCS with origin \em center and X axis
    #  \em vecX. \em geom is the pseudo-geompy object used to build the 
    #  geometric shapes.
    def getVecYZ(self, geom, center, vecX):
        """
        Get the vectors Y and Z for the LCS with origin `center` and X axis
        `vecX`. `geom` is the pseudo-geompy object used to build the geometric
        shapes.
        """
        self.geom = geom
        locY = None
        locZ = None
        if self._vectorYCoords is None:
            (locY, locZ) = self._getDefaultVecYZ(center, vecX)
        else:
            xPoint = self.geom.MakeTranslationVector(center, vecX)
            givenLocY = self.geom.MakeVectorDXDYDZ(self._vectorYCoords[0],
                                                   self._vectorYCoords[1],
                                                   self._vectorYCoords[2])
            angle = self.geom.GetAngleRadians(vecX, givenLocY)
            if abs(angle) < 1e-7 or abs(angle - math.pi) < 1e-7:
                logger.warning("Vector Y is colinear to the beam X axis, "
                               "using default LCS.")
                (locY, locZ) = self._getDefaultVecYZ(center, vecX)
            else:
                yPoint = self.geom.MakeTranslationVector(center, givenLocY)
                locPlaneXY = self.geom.MakePlaneThreePnt(center, xPoint,
                                                         yPoint, 1.0)
                locZ = self.geom.GetNormal(locPlaneXY)
                zPoint = self.geom.MakeTranslationVector(center, locZ)
                locPlaneZX = self.geom.MakePlaneThreePnt(center, zPoint,
                                                         xPoint, 1.0)
                locY = self.geom.GetNormal(locPlaneZX)

        if self._angle != 0.0:
            angleRad = math.radians(self._angle)
            locY = self.geom.Rotate(locY, vecX, angleRad)
            locZ = self.geom.Rotate(locZ, vecX, angleRad)

        return (locY, locZ)

## This class is used to compute the orientation of 2D elements and to build
#  the corresponding markers. Angles \em alpha and \em beta are used to determine
#  the local coordinate system for the 2D element. If \em vect is not
#  \b None, it is used instead of \em alpha and \em beta.
#  \ingroup orientation
class Orientation2D:
    """
    This class is used to compute the orientation of 2D elements and to build
    the corresponding markers. Angles `alpha` and `beta` are used to determine
    the local coordinate system for the 2D element. If `vect` is not
    :const:`None`, it is used instead of `alpha` and `beta`.
    """

    def __init__(self, alpha, beta, vect):
        self.geom = None
        self._alpha = alpha
        self._beta = beta
        self._vect = vect

    def __repr__(self):
        reprdict = self.__dict__.copy()
        del reprdict["geom"]
        return '%s(%s)' % (self.__class__.__name__, reprdict)

    ## Create the default marker, that use the normal vector of the 2D object
    #  as the local Z axis and the global X axis to determine the local X
    #  axis. \em warnings can be used to enable or disable the logging of
    # warning messages.
    def _buildDefaultMarker(self, center, normal, warnings = True):
        """
        Create the default marker, that use the normal vector of the 2D object
        as the local Z axis and the global X axis to determine the local X
        axis. `warnings` can be used to enable or disable the logging of
        warning messages.
        """
        marker = None
        globalVecX = self.geom.MakeVectorDXDYDZ(1.0, 0.0, 0.0)
        angle = self.geom.GetAngleRadians(normal, globalVecX)
        if abs(angle) < 1e-7 or abs(angle - math.pi) < 1e-7:
            if warnings:
                logger.warning("Face normal is colinear to absolute X axis. "
                               "Absolute Y axis will be used to determine "
                               "local X axis.")
            globalVecY = self.geom.MakeVectorDXDYDZ(0.0, 1.0, 0.0)
            marker = self._buildMarkerRefVecX(center, normal, globalVecY)
        else:
            marker = self._buildMarkerRefVecX(center, normal, globalVecX)
        return marker

    ## Create a marker using \em normal as Z axis and \em refVecX 
    #  to determine the X axis.
    def _buildMarkerRefVecX(self, center, normal, refVecX):
        """
        Create a marker using `normal` as Z axis and `refVecX` to determine
        the X axis.
        """
        xPoint = self.geom.MakeTranslationVector(center, refVecX)
        zPoint = self.geom.MakeTranslationVector(center, normal)
        locPlaneZX = self.geom.MakePlaneThreePnt(center, zPoint, xPoint, 1.0)
        locY = self.geom.GetNormal(locPlaneZX)
        yPoint = self.geom.MakeTranslationVector(center, locY)
        locPlaneYZ = self.geom.MakePlaneThreePnt(center, yPoint, zPoint, 1.0)
        locX = self.geom.GetNormal(locPlaneYZ)
        marker = self.geom.MakeMarkerPntTwoVec(center, locX, locY)
        return marker

    ## Create a marker with origin \em center and \em normal as Z axis. 
    #  The other axes are computed using the parameters alpha and beta of the
    #  Orientation2D instance. \em geom is the pseudo-geompy object used to
    #  build the geometric shapes. \em warnings can be used to enable or
    #  disable the logging of warning messages.
    def buildMarker(self, geom, center, normal, warnings = True):
        """
        Create a marker with origin `center` and `normal` as Z axis. The other
        axes are computed using the parameters alpha and beta of the
        Orientation2D instance. `geom` is the pseudo-geompy object used to
        build the geometric shapes. `warnings` can be used to enable or
        disable the logging of warning messages.
        """
        self.geom = geom
        marker = None
        refVecX = None
        if self._vect is not None:
            # Using vector parameter
            if abs(self._vect[0]) <= 1e-7 and abs(self._vect[1]) <= 1e-7 and \
                                              abs(self._vect[2]) <= 1e-7:
                if warnings:
                    logger.warning("Vector too small: %s, using default LCS" %
                                   self._vect)
            else:
                refVecX = self.geom.MakeVectorDXDYDZ(self._vect[0],
                                                     self._vect[1],
                                                     self._vect[2])
        elif self._alpha is not None and self._beta is not None:
            # Using alpha and beta angles
            alphaRad = math.radians(self._alpha)
            betaRad = math.radians(self._beta)
            if abs(alphaRad) <= 1e-7 and abs(betaRad) <= 1e-7:
                if warnings:
                    logger.warning("Angles too small: (%g, %g), using "
                                   "default LCS" % (self._alpha, self._beta))
            else:
                # rotate global CS with angles alpha and beta
                refVecX = self.geom.MakeVectorDXDYDZ(1.0, 0.0, 0.0)
                refVecY = self.geom.MakeVectorDXDYDZ(0.0, 1.0, 0.0)
                globalVecZ = self.geom.MakeVectorDXDYDZ(0.0, 0.0, 1.0)
                if abs(alphaRad) > 1e-7:
                    refVecX = self.geom.Rotate(refVecX, globalVecZ, alphaRad)
                    refVecY = self.geom.Rotate(refVecY, globalVecZ, alphaRad)
                if abs(betaRad) > 1e-7:
                    refVecX = self.geom.Rotate(refVecX, refVecY, betaRad)
    
        if refVecX is not None:
            # build local coordinate system
            angle = self.geom.GetAngleRadians(normal, refVecX)
            if abs(angle) < 1e-7 or abs(angle - math.pi) < 1e-7:
                if warnings:
                    logger.warning("Face normal is colinear to the reference "
                                   "X axis, using default LCS.")
            else:
                marker = self._buildMarkerRefVecX(center, normal, refVecX)

        if marker is None:
            marker = self._buildDefaultMarker(center, normal, warnings)

        return marker
