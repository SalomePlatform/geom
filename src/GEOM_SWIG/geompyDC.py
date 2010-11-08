#  -*- coding: iso-8859-1 -*-
#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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
#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#  File   : geompy.py
#  Author : Paul RASCLE, EDF
#  Module : GEOM

"""
    \namespace geompy
    \brief Module geompy
"""

## @defgroup l1_geompy_auxiliary Auxiliary data structures and methods

## @defgroup l1_geompy_purpose   All package methods, grouped by their purpose
## @{
##   @defgroup l2_import_export Importing/exporting geometrical objects
##   @defgroup l2_creating      Creating geometrical objects
##   @{
##     @defgroup l3_basic_go      Creating Basic Geometric Objects
##     @{
##       @defgroup l4_curves        Creating Curves

##     @}
##     @defgroup l3_3d_primitives Creating 3D Primitives
##     @defgroup l3_complex       Creating Complex Objects
##     @defgroup l3_groups        Working with groups
##     @defgroup l3_blocks        Building by blocks
##     @{
##       @defgroup l4_blocks_measure Check and Improve

##     @}
##     @defgroup l3_sketcher      Sketcher
##     @defgroup l3_advanced      Creating Advanced Geometrical Objects
##     @{
##       @defgroup l4_decompose     Decompose objects
##       @defgroup l4_access        Access to sub-shapes by their unique IDs inside the main shape
##       @defgroup l4_obtain        Access to subshapes by a criteria
##       @defgroup l4_advanced      Advanced objects creation functions

##     @}

##   @}
##   @defgroup l2_transforming  Transforming geometrical objects
##   @{
##     @defgroup l3_basic_op      Basic Operations
##     @defgroup l3_boolean       Boolean Operations
##     @defgroup l3_transform     Transformation Operations
##     @defgroup l3_local         Local Operations (Fillet and Chamfer)
##     @defgroup l3_blocks_op     Blocks Operations
##     @defgroup l3_healing       Repairing Operations
##     @defgroup l3_restore_ss    Restore presentation parameters and a tree of subshapes

##   @}
##   @defgroup l2_measure       Using measurement tools

## @}

import salome
salome.salome_init()
from salome import *

from salome_notebook import *

import GEOM
import math

## Enumeration ShapeType as a dictionary
#  @ingroup l1_geompy_auxiliary
ShapeType = {"COMPOUND":0, "COMPSOLID":1, "SOLID":2, "SHELL":3, "FACE":4, "WIRE":5, "EDGE":6, "VERTEX":7, "SHAPE":8}

## Raise an Error, containing the Method_name, if Operation is Failed
## @ingroup l1_geompy_auxiliary
def RaiseIfFailed (Method_name, Operation):
    if Operation.IsDone() == 0 and Operation.GetErrorCode() != "NOT_FOUND_ANY":
        raise RuntimeError, Method_name + " : " + Operation.GetErrorCode()

## Return list of variables value from salome notebook
## @ingroup l1_geompy_auxiliary
def ParseParameters(*parameters):
    Result = []
    StringResult = []
    for parameter in parameters:
        if isinstance(parameter, list):
            lResults = ParseParameters(*parameter)
            if len(lResults) > 0:
                Result.append(lResults[:-1])
                StringResult += lResults[-1].split(":")
                pass
            pass
        else:
            if isinstance(parameter,str):
                if notebook.isVariable(parameter):
                    Result.append(notebook.get(parameter))
                else:
                    raise RuntimeError, "Variable with name '" + parameter + "' doesn't exist!!!"
                pass
            else:
                Result.append(parameter)
                pass
            StringResult.append(str(parameter))
            pass
        pass
    if Result:
        Result.append(":".join(StringResult))
    else:
        Result = ":".join(StringResult)
    return Result

## Return list of variables value from salome notebook
## @ingroup l1_geompy_auxiliary
def ParseList(list):
    Result = []
    StringResult = ""
    for parameter in list:
        if isinstance(parameter,str) and notebook.isVariable(parameter):
            Result.append(str(notebook.get(parameter)))
            pass
        else:
            Result.append(str(parameter))
            pass

        StringResult = StringResult + str(parameter)
        StringResult = StringResult + ":"
        pass
    StringResult = StringResult[:len(StringResult)-1]
    return Result, StringResult

## Return list of variables value from salome notebook
## @ingroup l1_geompy_auxiliary
def ParseSketcherCommand(command):
    Result = ""
    StringResult = ""
    sections = command.split(":")
    for section in sections:
        parameters = section.split(" ")
        paramIndex = 1
        for parameter in parameters:
            if paramIndex > 1 and parameter.find("'") != -1:
                parameter = parameter.replace("'","")
                if notebook.isVariable(parameter):
                    Result = Result + str(notebook.get(parameter)) + " "
                    pass
                else:
                    raise RuntimeError, "Variable with name '" + parameter + "' doesn't exist!!!"
                    pass
                pass
            else:
                Result = Result + str(parameter) + " "
                pass
            if paramIndex > 1:
                StringResult = StringResult + parameter
                StringResult = StringResult + ":"
                pass
            paramIndex = paramIndex + 1
            pass
        Result = Result[:len(Result)-1] + ":"
        pass
    Result = Result[:len(Result)-1]
    return Result, StringResult

## Helper function which can be used to pack the passed string to the byte data.
## Only '1' an '0' symbols are valid for the string. The missing bits are replaced by zeroes.
## If the string contains invalid symbol (neither '1' nor '0'), the function raises an exception.
## For example,
## \code
## val = PackData("10001110") # val = 0xAE
## val = PackData("1")        # val = 0x80
## \endcode
## @param data unpacked data - a string containing '1' and '0' symbols
## @return data packed to the byte stream
## @ingroup l1_geompy_auxiliary
def PackData(data):
    bytes = len(data)/8
    if len(data)%8: bytes += 1
    res = ""
    for b in range(bytes):
        d = data[b*8:(b+1)*8]
        val = 0
        for i in range(8):
            val *= 2
            if i < len(d):
                if d[i] == "1": val += 1
                elif d[i] != "0":
                    raise "Invalid symbol %s" % d[i]
                pass
            pass
        res += chr(val)
        pass
    return res

## Read bitmap texture from the text file.
## In that file, any non-zero symbol represents '1' opaque pixel of the bitmap.
## A zero symbol ('0') represents transparent pixel of the texture bitmap.
## The function returns width and height of the pixmap in pixels and byte stream representing
## texture bitmap itself.
##
## This function can be used to read the texture to the byte stream in order to pass it to
## the AddTexture() function of geompy class.
## For example,
## \code
## import geompy
## geompy.init_geom(salome.myStudy)
## texture = geompy.readtexture('mytexture.dat')
## texture = geompy.AddTexture(*texture)
## obj.SetMarkerTexture(texture)
## \endcode
## @param fname texture file name
## @return sequence of tree values: texture's width, height in pixels and its byte stream
## @ingroup l1_geompy_auxiliary
def ReadTexture(fname):
    try:
        f = open(fname)
        lines = [ l.strip() for l in f.readlines()]
        f.close()
        maxlen = 0
        if lines: maxlen = max([len(x) for x in lines])
        lenbytes = maxlen/8
        if maxlen%8: lenbytes += 1
        bytedata=""
        for line in lines:
            if len(line)%8:
                lenline = (len(line)/8+1)*8
                pass
            else:
                lenline = (len(line)/8)*8
                pass
            for i in range(lenline/8):
                byte=""
                for j in range(8):
                    if i*8+j < len(line) and line[i*8+j] != "0": byte += "1"
                    else: byte += "0"
                    pass
                bytedata += PackData(byte)
                pass
            for i in range(lenline/8, lenbytes):
                bytedata += PackData("0")
            pass
        return lenbytes*8, len(lines), bytedata
    except:
        pass
    return 0, 0, ""

## Kinds of shape enumeration
#  @ingroup l1_geompy_auxiliary
kind = GEOM.GEOM_IKindOfShape

## Information about closed/unclosed state of shell or wire
#  @ingroup l1_geompy_auxiliary
class info:
    UNKNOWN  = 0
    CLOSED   = 1
    UNCLOSED = 2

class geompyDC(GEOM._objref_GEOM_Gen):

        def __init__(self):
            GEOM._objref_GEOM_Gen.__init__(self)
            self.myBuilder = None
            self.myStudyId = 0
            self.father    = None

            self.BasicOp  = None
            self.CurvesOp = None
            self.PrimOp   = None
            self.ShapesOp = None
            self.HealOp   = None
            self.InsertOp = None
            self.BoolOp   = None
            self.TrsfOp   = None
            self.LocalOp  = None
            self.MeasuOp  = None
            self.BlocksOp = None
            self.GroupOp  = None
            self.AdvOp    = None
            pass

        ## @addtogroup l1_geompy_auxiliary
        ## @{
        def init_geom(self,theStudy):
            self.myStudy = theStudy
            self.myStudyId = self.myStudy._get_StudyId()
            self.myBuilder = self.myStudy.NewBuilder()
            self.father = self.myStudy.FindComponent("GEOM")
            if self.father is None:
                self.father = self.myBuilder.NewComponent("GEOM")
                A1 = self.myBuilder.FindOrCreateAttribute(self.father, "AttributeName")
                FName = A1._narrow(SALOMEDS.AttributeName)
                FName.SetValue("Geometry")
                A2 = self.myBuilder.FindOrCreateAttribute(self.father, "AttributePixMap")
                aPixmap = A2._narrow(SALOMEDS.AttributePixMap)
                aPixmap.SetPixMap("ICON_OBJBROWSER_Geometry")
                self.myBuilder.DefineComponentInstance(self.father,self)
                pass
            self.BasicOp  = self.GetIBasicOperations    (self.myStudyId)
            self.CurvesOp = self.GetICurvesOperations   (self.myStudyId)
            self.PrimOp   = self.GetI3DPrimOperations   (self.myStudyId)
            self.ShapesOp = self.GetIShapesOperations   (self.myStudyId)
            self.HealOp   = self.GetIHealingOperations  (self.myStudyId)
            self.InsertOp = self.GetIInsertOperations   (self.myStudyId)
            self.BoolOp   = self.GetIBooleanOperations  (self.myStudyId)
            self.TrsfOp   = self.GetITransformOperations(self.myStudyId)
            self.LocalOp  = self.GetILocalOperations    (self.myStudyId)
            self.MeasuOp  = self.GetIMeasureOperations  (self.myStudyId)
            self.BlocksOp = self.GetIBlocksOperations   (self.myStudyId)
            self.GroupOp  = self.GetIGroupOperations    (self.myStudyId)
            self.AdvOp    = self.GetIAdvancedOperations (self.myStudyId)
            pass

        ## Get name for sub-shape aSubObj of shape aMainObj
        #
        # @ref swig_SubShapeAllSorted "Example"
        def SubShapeName(self,aSubObj, aMainObj):
            # Example: see GEOM_TestAll.py

            #aSubId  = orb.object_to_string(aSubObj)
            #aMainId = orb.object_to_string(aMainObj)
            #index = gg.getIndexTopology(aSubId, aMainId)
            #name = gg.getShapeTypeString(aSubId) + "_%d"%(index)
            index = self.ShapesOp.GetTopologyIndex(aMainObj, aSubObj)
            name = self.ShapesOp.GetShapeTypeString(aSubObj) + "_%d"%(index)
            return name

        ## Publish in study aShape with name aName
        #
        #  \param aShape the shape to be published
        #  \param aName  the name for the shape
        #  \param doRestoreSubShapes if True, finds and publishes also
        #         sub-shapes of <VAR>aShape</VAR>, corresponding to its arguments
        #         and published sub-shapes of arguments
        #  \param theArgs,theFindMethod,theInheritFirstArg see geompy.RestoreSubShapes for
        #                                                  these arguments description
        #  \return study entry of the published shape in form of string
        #
        #  @ref swig_MakeQuad4Vertices "Example"
        def addToStudy(self, aShape, aName, doRestoreSubShapes=False,
                       theArgs=[], theFindMethod=GEOM.FSM_GetInPlace, theInheritFirstArg=False):
            # Example: see GEOM_TestAll.py
            try:
                aSObject = self.AddInStudy(self.myStudy, aShape, aName, None)
                if doRestoreSubShapes:
                    self.RestoreSubShapesSO(self.myStudy, aSObject, theArgs,
                                            theFindMethod, theInheritFirstArg, True )
            except:
                print "addToStudy() failed"
                return ""
            return aShape.GetStudyEntry()

        ## Publish in study aShape with name aName as sub-object of previously published aFather
        #
        #  @ref swig_SubShapeAllSorted "Example"
        def addToStudyInFather(self, aFather, aShape, aName):
            # Example: see GEOM_TestAll.py
            try:
                aSObject = self.AddInStudy(self.myStudy, aShape, aName, aFather)
            except:
                print "addToStudyInFather() failed"
                return ""
            return aShape.GetStudyEntry()

        # end of l1_geompy_auxiliary
        ## @}

        ## @addtogroup l3_restore_ss
        ## @{

        ## Publish sub-shapes, standing for arguments and sub-shapes of arguments
        #  To be used from python scripts out of geompy.addToStudy (non-default usage)
        #  \param theObject published GEOM object, arguments of which will be published
        #  \param theArgs   list of GEOM_Object, operation arguments to be published.
        #                   If this list is empty, all operation arguments will be published
        #  \param theFindMethod method to search subshapes, corresponding to arguments and
        #                       their subshapes. Value from enumeration GEOM::find_shape_method.
        #  \param theInheritFirstArg set properties of the first argument for <VAR>theObject</VAR>.
        #                            Do not publish subshapes in place of arguments, but only
        #                            in place of subshapes of the first argument,
        #                            because the whole shape corresponds to the first argument.
        #                            Mainly to be used after transformations, but it also can be
        #                            usefull after partition with one object shape, and some other
        #                            operations, where only the first argument has to be considered.
        #                            If theObject has only one argument shape, this flag is automatically
        #                            considered as True, not regarding really passed value.
        #  \param theAddPrefix add prefix "from_" to names of restored sub-shapes,
        #                      and prefix "from_subshapes_of_" to names of partially restored subshapes.
        #  \return list of published sub-shapes
        #
        #  @ref tui_restore_prs_params "Example"
        def RestoreSubShapes (self, theObject, theArgs=[], theFindMethod=GEOM.FSM_GetInPlace,
                              theInheritFirstArg=False, theAddPrefix=True):
            # Example: see GEOM_TestAll.py
            return self.RestoreSubShapesO(self.myStudy, theObject, theArgs,
                                          theFindMethod, theInheritFirstArg, theAddPrefix)

        ## Publish sub-shapes, standing for arguments and sub-shapes of arguments
        #  To be used from python scripts out of geompy.addToStudy (non-default usage)
        #  \param theObject published GEOM object, arguments of which will be published
        #  \param theArgs   list of GEOM_Object, operation arguments to be published.
        #                   If this list is empty, all operation arguments will be published
        #  \param theFindMethod method to search subshapes, corresponding to arguments and
        #                       their subshapes. Value from enumeration GEOM::find_shape_method.
        #  \param theInheritFirstArg set properties of the first argument for <VAR>theObject</VAR>.
        #                            Do not publish subshapes in place of arguments, but only
        #                            in place of subshapes of the first argument,
        #                            because the whole shape corresponds to the first argument.
        #                            Mainly to be used after transformations, but it also can be
        #                            usefull after partition with one object shape, and some other
        #                            operations, where only the first argument has to be considered.
        #                            If theObject has only one argument shape, this flag is automatically
        #                            considered as True, not regarding really passed value.
        #  \param theAddPrefix add prefix "from_" to names of restored sub-shapes,
        #                      and prefix "from_subshapes_of_" to names of partially restored subshapes.
        #  \return list of published sub-shapes
        #
        #  @ref tui_restore_prs_params "Example"
        def RestoreGivenSubShapes (self, theObject, theArgs=[], theFindMethod=GEOM.FSM_GetInPlace,
                                   theInheritFirstArg=False, theAddPrefix=True):
            # Example: see GEOM_TestAll.py
            return self.RestoreGivenSubShapesO(self.myStudy, theObject, theArgs,
                                               theFindMethod, theInheritFirstArg, theAddPrefix)

        # end of l3_restore_ss
        ## @}

        ## @addtogroup l3_basic_go
        ## @{

        ## Create point by three coordinates.
        #  @param theX The X coordinate of the point.
        #  @param theY The Y coordinate of the point.
        #  @param theZ The Z coordinate of the point.
        #  @return New GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        def MakeVertex(self, theX, theY, theZ):
            # Example: see GEOM_TestAll.py
            theX,theY,theZ,Parameters = ParseParameters(theX, theY, theZ)
            anObj = self.BasicOp.MakePointXYZ(theX, theY, theZ)
            RaiseIfFailed("MakePointXYZ", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a point, distant from the referenced point
        #  on the given distances along the coordinate axes.
        #  @param theReference The referenced point.
        #  @param theX Displacement from the referenced point along OX axis.
        #  @param theY Displacement from the referenced point along OY axis.
        #  @param theZ Displacement from the referenced point along OZ axis.
        #  @return New GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        def MakeVertexWithRef(self,theReference, theX, theY, theZ):
            # Example: see GEOM_TestAll.py
            theX,theY,theZ,Parameters = ParseParameters(theX, theY, theZ)
            anObj = self.BasicOp.MakePointWithReference(theReference, theX, theY, theZ)
            RaiseIfFailed("MakePointWithReference", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a point, corresponding to the given parameter on the given curve.
        #  @param theRefCurve The referenced curve.
        #  @param theParameter Value of parameter on the referenced curve.
        #  @return New GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        def MakeVertexOnCurve(self,theRefCurve, theParameter):
            # Example: see GEOM_TestAll.py
            theParameter, Parameters = ParseParameters(theParameter)
            anObj = self.BasicOp.MakePointOnCurve(theRefCurve, theParameter)
            RaiseIfFailed("MakePointOnCurve", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a point by projection give coordinates on the given curve
        #  @param theRefCurve The referenced curve.
        #  @param theX X-coordinate in 3D space
        #  @param theY Y-coordinate in 3D space
        #  @param theZ Z-coordinate in 3D space
        #  @return New GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        def MakeVertexOnCurveByCoord(self,theRefCurve, theX, theY, theZ):
            # Example: see GEOM_TestAll.py
            theX, theY, theZ, Parameters = ParseParameters(theX, theY, theZ)
            anObj = self.BasicOp.MakePointOnCurveByCoord(theRefCurve, theX, theY, theZ)
            RaiseIfFailed("MakeVertexOnCurveByCoord", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a point, corresponding to the given parameters on the
        #    given surface.
        #  @param theRefSurf The referenced surface.
        #  @param theUParameter Value of U-parameter on the referenced surface.
        #  @param theVParameter Value of V-parameter on the referenced surface.
        #  @return New GEOM_Object, containing the created point.
        #
        #  @ref swig_MakeVertexOnSurface "Example"
        def MakeVertexOnSurface(self, theRefSurf, theUParameter, theVParameter):
            theUParameter, theVParameter, Parameters = ParseParameters(theUParameter, theVParameter)
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakePointOnSurface(theRefSurf, theUParameter, theVParameter)
            RaiseIfFailed("MakePointOnSurface", self.BasicOp)
            anObj.SetParameters(Parameters);
            return anObj

        ## Create a point by projection give coordinates on the given surface
        #  @param theRefSurf The referenced surface.
        #  @param theX X-coordinate in 3D space
        #  @param theY Y-coordinate in 3D space
        #  @param theZ Z-coordinate in 3D space
        #  @return New GEOM_Object, containing the created point.
        #
        #  @ref swig_MakeVertexOnSurfaceByCoord "Example"
        def MakeVertexOnSurfaceByCoord(self, theRefSurf, theX, theY, theZ):
            theX, theY, theZ, Parameters = ParseParameters(theX, theY, theZ)
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakePointOnSurfaceByCoord(theRefSurf, theX, theY, theZ)
            RaiseIfFailed("MakeVertexOnSurfaceByCoord", self.BasicOp)
            anObj.SetParameters(Parameters);
            return anObj

        ## Create a point on intersection of two lines.
        #  @param theRefLine1, theRefLine2 The referenced lines.
        #  @return New GEOM_Object, containing the created point.
        #
        #  @ref swig_MakeVertexOnLinesIntersection "Example"
        def MakeVertexOnLinesIntersection(self, theRefLine1, theRefLine2):
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakePointOnLinesIntersection(theRefLine1, theRefLine2)
            RaiseIfFailed("MakePointOnLinesIntersection", self.BasicOp)
            return anObj

        ## Create a tangent, corresponding to the given parameter on the given curve.
        #  @param theRefCurve The referenced curve.
        #  @param theParameter Value of parameter on the referenced curve.
        #  @return New GEOM_Object, containing the created tangent.
        #
        #  @ref swig_MakeTangentOnCurve "Example"
        def MakeTangentOnCurve(self, theRefCurve, theParameter):
            anObj = self.BasicOp.MakeTangentOnCurve(theRefCurve, theParameter)
            RaiseIfFailed("MakeTangentOnCurve", self.BasicOp)
            return anObj

        ## Create a tangent plane, corresponding to the given parameter on the given face.
        #  @param theFace The face for which tangent plane should be built.
        #  @param theParameterV vertical value of the center point (0.0 - 1.0).
        #  @param theParameterU horisontal value of the center point (0.0 - 1.0).
        #  @param theTrimSize the size of plane.
        #  @return New GEOM_Object, containing the created tangent.
        #
        #  @ref swig_MakeTangentPlaneOnFace "Example"
        def MakeTangentPlaneOnFace(self, theFace, theParameterU, theParameterV, theTrimSize):
            anObj = self.BasicOp.MakeTangentPlaneOnFace(theFace, theParameterU, theParameterV, theTrimSize)
            RaiseIfFailed("MakeTangentPlaneOnFace", self.BasicOp)
            return anObj

        ## Create a vector with the given components.
        #  @param theDX X component of the vector.
        #  @param theDY Y component of the vector.
        #  @param theDZ Z component of the vector.
        #  @return New GEOM_Object, containing the created vector.
        #
        #  @ref tui_creation_vector "Example"
        def MakeVectorDXDYDZ(self,theDX, theDY, theDZ):
            # Example: see GEOM_TestAll.py
            theDX,theDY,theDZ,Parameters = ParseParameters(theDX, theDY, theDZ)
            anObj = self.BasicOp.MakeVectorDXDYDZ(theDX, theDY, theDZ)
            RaiseIfFailed("MakeVectorDXDYDZ", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a vector between two points.
        #  @param thePnt1 Start point for the vector.
        #  @param thePnt2 End point for the vector.
        #  @return New GEOM_Object, containing the created vector.
        #
        #  @ref tui_creation_vector "Example"
        def MakeVector(self,thePnt1, thePnt2):
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeVectorTwoPnt(thePnt1, thePnt2)
            RaiseIfFailed("MakeVectorTwoPnt", self.BasicOp)
            return anObj

        ## Create a line, passing through the given point
        #  and parrallel to the given direction
        #  @param thePnt Point. The resulting line will pass through it.
        #  @param theDir Direction. The resulting line will be parallel to it.
        #  @return New GEOM_Object, containing the created line.
        #
        #  @ref tui_creation_line "Example"
        def MakeLine(self,thePnt, theDir):
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeLine(thePnt, theDir)
            RaiseIfFailed("MakeLine", self.BasicOp)
            return anObj

        ## Create a line, passing through the given points
        #  @param thePnt1 First of two points, defining the line.
        #  @param thePnt2 Second of two points, defining the line.
        #  @return New GEOM_Object, containing the created line.
        #
        #  @ref tui_creation_line "Example"
        def MakeLineTwoPnt(self,thePnt1, thePnt2):
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeLineTwoPnt(thePnt1, thePnt2)
            RaiseIfFailed("MakeLineTwoPnt", self.BasicOp)
            return anObj

        ## Create a line on two faces intersection.
        #  @param theFace1 First of two faces, defining the line.
        #  @param theFace2 Second of two faces, defining the line.
        #  @return New GEOM_Object, containing the created line.
        #
        #  @ref swig_MakeLineTwoFaces "Example"
        def MakeLineTwoFaces(self, theFace1, theFace2):
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeLineTwoFaces(theFace1, theFace2)
            RaiseIfFailed("MakeLineTwoFaces", self.BasicOp)
            return anObj

        ## Create a plane, passing through the given point
        #  and normal to the given vector.
        #  @param thePnt Point, the plane has to pass through.
        #  @param theVec Vector, defining the plane normal direction.
        #  @param theTrimSize Half size of a side of quadrangle face, representing the plane.
        #  @return New GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        def MakePlane(self,thePnt, theVec, theTrimSize):
            # Example: see GEOM_TestAll.py
            theTrimSize, Parameters = ParseParameters(theTrimSize);
            anObj = self.BasicOp.MakePlanePntVec(thePnt, theVec, theTrimSize)
            RaiseIfFailed("MakePlanePntVec", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a plane, passing through the three given points
        #  @param thePnt1 First of three points, defining the plane.
        #  @param thePnt2 Second of three points, defining the plane.
        #  @param thePnt3 Fird of three points, defining the plane.
        #  @param theTrimSize Half size of a side of quadrangle face, representing the plane.
        #  @return New GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        def MakePlaneThreePnt(self,thePnt1, thePnt2, thePnt3, theTrimSize):
            # Example: see GEOM_TestAll.py
            theTrimSize, Parameters = ParseParameters(theTrimSize);
            anObj = self.BasicOp.MakePlaneThreePnt(thePnt1, thePnt2, thePnt3, theTrimSize)
            RaiseIfFailed("MakePlaneThreePnt", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a plane, similar to the existing one, but with another size of representing face.
        #  @param theFace Referenced plane or LCS(Marker).
        #  @param theTrimSize New half size of a side of quadrangle face, representing the plane.
        #  @return New GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        def MakePlaneFace(self,theFace, theTrimSize):
            # Example: see GEOM_TestAll.py
            theTrimSize, Parameters = ParseParameters(theTrimSize);
            anObj = self.BasicOp.MakePlaneFace(theFace, theTrimSize)
            RaiseIfFailed("MakePlaneFace", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a plane, passing through the 2 vectors
        #  with center in a start point of the first vector.
        #  @param theVec1 Vector, defining center point and plane direction.
        #  @param theVec2 Vector, defining the plane normal direction.
        #  @param theTrimSize Half size of a side of quadrangle face, representing the plane.
        #  @return New GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        def MakePlane2Vec(self,theVec1, theVec2, theTrimSize):
            # Example: see GEOM_TestAll.py
            theTrimSize, Parameters = ParseParameters(theTrimSize);
            anObj = self.BasicOp.MakePlane2Vec(theVec1, theVec2, theTrimSize)
            RaiseIfFailed("MakePlane2Vec", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a plane, based on a Local coordinate system.
        #  @param theLCS  coordinate system, defining plane.
        #  @param theTrimSize Half size of a side of quadrangle face, representing the plane.
        #  @param theOrientation OXY, OYZ or OZX orientation - (1, 2 or 3)
        #  @return New GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        def MakePlaneLCS(self,theLCS, theTrimSize, theOrientation):
            # Example: see GEOM_TestAll.py
            theTrimSize, Parameters = ParseParameters(theTrimSize);
            anObj = self.BasicOp.MakePlaneLCS(theLCS, theTrimSize, theOrientation)
            RaiseIfFailed("MakePlaneLCS", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a local coordinate system.
        #  @param OX,OY,OZ Three coordinates of coordinate system origin.
        #  @param XDX,XDY,XDZ Three components of OX direction
        #  @param YDX,YDY,YDZ Three components of OY direction
        #  @return New GEOM_Object, containing the created coordinate system.
        #
        #  @ref swig_MakeMarker "Example"
        def MakeMarker(self, OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ):
            # Example: see GEOM_TestAll.py
            OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ, Parameters = ParseParameters(OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ);
            anObj = self.BasicOp.MakeMarker(OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ)
            RaiseIfFailed("MakeMarker", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a local coordinate system from shape.
        #  @param theShape The initial shape to detect the coordinate system.
        #  @return New GEOM_Object, containing the created coordinate system.
        #
        #  @ref tui_creation_lcs "Example"
        def MakeMarkerFromShape(self, theShape):
            anObj = self.BasicOp.MakeMarkerFromShape(theShape)
            RaiseIfFailed("MakeMarkerFromShape", self.BasicOp)
            return anObj

        ## Create a local coordinate system from point and two vectors.
        #  @param theOrigin Point of coordinate system origin.
        #  @param theXVec Vector of X direction
        #  @param theYVec Vector of Y direction
        #  @return New GEOM_Object, containing the created coordinate system.
        #
        #  @ref tui_creation_lcs "Example"
        def MakeMarkerPntTwoVec(self, theOrigin, theXVec, theYVec):
            anObj = self.BasicOp.MakeMarkerPntTwoVec(theOrigin, theXVec, theYVec)
            RaiseIfFailed("MakeMarkerPntTwoVec", self.BasicOp)
            return anObj

        # end of l3_basic_go
        ## @}

        ## @addtogroup l4_curves
        ## @{

        ##  Create an arc of circle, passing through three given points.
        #  @param thePnt1 Start point of the arc.
        #  @param thePnt2 Middle point of the arc.
        #  @param thePnt3 End point of the arc.
        #  @return New GEOM_Object, containing the created arc.
        #
        #  @ref swig_MakeArc "Example"
        def MakeArc(self,thePnt1, thePnt2, thePnt3):
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeArc(thePnt1, thePnt2, thePnt3)
            RaiseIfFailed("MakeArc", self.CurvesOp)
            return anObj

        ##  Create an arc of circle from a center and 2 points.
        #  @param thePnt1 Center of the arc
        #  @param thePnt2 Start point of the arc. (Gives also the radius of the arc)
        #  @param thePnt3 End point of the arc (Gives also a direction)
        #  @param theSense Orientation of the arc
        #  @return New GEOM_Object, containing the created arc.
        #
        #  @ref swig_MakeArc "Example"
        def MakeArcCenter(self, thePnt1, thePnt2, thePnt3, theSense=False):
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeArcCenter(thePnt1, thePnt2, thePnt3, theSense)
            RaiseIfFailed("MakeArcCenter", self.CurvesOp)
            return anObj

        ##  Create an arc of ellipse, of center and two points.
        #  @param theCenter Center of the arc.
        #  @param thePnt1 defines major radius of the arc by distance from Pnt1 to Pnt2.
        #  @param thePnt2 defines plane of ellipse and minor radius as distance from Pnt3 to line from Pnt1 to Pnt2.
        #  @return New GEOM_Object, containing the created arc.
        #
        #  @ref swig_MakeArc "Example"
        def MakeArcOfEllipse(self,theCenter, thePnt1, thePnt2):
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeArcOfEllipse(theCenter, thePnt1, thePnt2)
            RaiseIfFailed("MakeArcOfEllipse", self.CurvesOp)
            return anObj

        ## Create a circle with given center, normal vector and radius.
        #  @param thePnt Circle center.
        #  @param theVec Vector, normal to the plane of the circle.
        #  @param theR Circle radius.
        #  @return New GEOM_Object, containing the created circle.
        #
        #  @ref tui_creation_circle "Example"
        def MakeCircle(self, thePnt, theVec, theR):
            # Example: see GEOM_TestAll.py
            theR, Parameters = ParseParameters(theR)
            anObj = self.CurvesOp.MakeCirclePntVecR(thePnt, theVec, theR)
            RaiseIfFailed("MakeCirclePntVecR", self.CurvesOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a circle with given radius.
        #  Center of the circle will be in the origin of global
        #  coordinate system and normal vector will be codirected with Z axis
        #  @param theR Circle radius.
        #  @return New GEOM_Object, containing the created circle.
        def MakeCircleR(self, theR):
            anObj = self.CurvesOp.MakeCirclePntVecR(None, None, theR)
            RaiseIfFailed("MakeCirclePntVecR", self.CurvesOp)
            return anObj

        ## Create a circle, passing through three given points
        #  @param thePnt1,thePnt2,thePnt3 Points, defining the circle.
        #  @return New GEOM_Object, containing the created circle.
        #
        #  @ref tui_creation_circle "Example"
        def MakeCircleThreePnt(self,thePnt1, thePnt2, thePnt3):
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeCircleThreePnt(thePnt1, thePnt2, thePnt3)
            RaiseIfFailed("MakeCircleThreePnt", self.CurvesOp)
            return anObj

        ## Create a circle, with given point1 as center,
        #  passing through the point2 as radius and laying in the plane,
        #  defined by all three given points.
        #  @param thePnt1,thePnt2,thePnt3 Points, defining the circle.
        #  @return New GEOM_Object, containing the created circle.
        #
        #  @ref swig_MakeCircle "Example"
        def MakeCircleCenter2Pnt(self,thePnt1, thePnt2, thePnt3):
            # Example: see GEOM_example6.py
            anObj = self.CurvesOp.MakeCircleCenter2Pnt(thePnt1, thePnt2, thePnt3)
            RaiseIfFailed("MakeCircleCenter2Pnt", self.CurvesOp)
            return anObj

        ## Create an ellipse with given center, normal vector and radiuses.
        #  @param thePnt Ellipse center.
        #  @param theVec Vector, normal to the plane of the ellipse.
        #  @param theRMajor Major ellipse radius.
        #  @param theRMinor Minor ellipse radius.
        #  @param theVecMaj Vector, direction of the ellipse's main axis.
        #  @return New GEOM_Object, containing the created ellipse.
        #
        #  @ref tui_creation_ellipse "Example"
        def MakeEllipse(self, thePnt, theVec, theRMajor, theRMinor, theVecMaj=None):
            # Example: see GEOM_TestAll.py
            theRMajor, theRMinor, Parameters = ParseParameters(theRMajor, theRMinor)
            if theVecMaj is not None:
                anObj = self.CurvesOp.MakeEllipseVec(thePnt, theVec, theRMajor, theRMinor, theVecMaj)
            else:
                anObj = self.CurvesOp.MakeEllipse(thePnt, theVec, theRMajor, theRMinor)
                pass
            RaiseIfFailed("MakeEllipse", self.CurvesOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create an ellipse with given radiuses.
        #  Center of the ellipse will be in the origin of global
        #  coordinate system and normal vector will be codirected with Z axis
        #  @param theRMajor Major ellipse radius.
        #  @param theRMinor Minor ellipse radius.
        #  @return New GEOM_Object, containing the created ellipse.
        def MakeEllipseRR(self, theRMajor, theRMinor):
            anObj = self.CurvesOp.MakeEllipse(None, None, theRMajor, theRMinor)
            RaiseIfFailed("MakeEllipse", self.CurvesOp)
            return anObj

        ## Create a polyline on the set of points.
        #  @param thePoints Sequence of points for the polyline.
        #  @return New GEOM_Object, containing the created polyline.
        #
        #  @ref tui_creation_curve "Example"
        def MakePolyline(self,thePoints):
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakePolyline(thePoints)
            RaiseIfFailed("MakePolyline", self.CurvesOp)
            return anObj

        ## Create bezier curve on the set of points.
        #  @param thePoints Sequence of points for the bezier curve.
        #  @return New GEOM_Object, containing the created bezier curve.
        #
        #  @ref tui_creation_curve "Example"
        def MakeBezier(self,thePoints):
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeSplineBezier(thePoints)
            RaiseIfFailed("MakeSplineBezier", self.CurvesOp)
            return anObj

        ## Create B-Spline curve on the set of points.
        #  @param thePoints Sequence of points for the B-Spline curve.
        #  @param theIsClosed If True, build a closed curve.
        #  @return New GEOM_Object, containing the created B-Spline curve.
        #
        #  @ref tui_creation_curve "Example"
        def MakeInterpol(self, thePoints, theIsClosed=False):
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeSplineInterpolation(thePoints, theIsClosed)
            RaiseIfFailed("MakeSplineInterpolation", self.CurvesOp)
            return anObj

        # end of l4_curves
        ## @}

        ## @addtogroup l3_sketcher
        ## @{

        ## Create a sketcher (wire or face), following the textual description,
        #  passed through <VAR>theCommand</VAR> argument. \n
        #  Edges of the resulting wire or face will be arcs of circles and/or linear segments. \n
        #  Format of the description string have to be the following:
        #
        #  "Sketcher[:F x1 y1]:CMD[:CMD[:CMD...]]"
        #
        #  Where:
        #  - x1, y1 are coordinates of the first sketcher point (zero by default),
        #  - CMD is one of
        #     - "R angle" : Set the direction by angle
        #     - "D dx dy" : Set the direction by DX & DY
        #     .
        #       \n
        #     - "TT x y" : Create segment by point at X & Y
        #     - "T dx dy" : Create segment by point with DX & DY
        #     - "L length" : Create segment by direction & Length
        #     - "IX x" : Create segment by direction & Intersect. X
        #     - "IY y" : Create segment by direction & Intersect. Y
        #     .
        #       \n
        #     - "C radius length" : Create arc by direction, radius and length(in degree)
        #     .
        #       \n
        #     - "WW" : Close Wire (to finish)
        #     - "WF" : Close Wire and build face (to finish)
        #
        #  @param theCommand String, defining the sketcher in local
        #                    coordinates of the working plane.
        #  @param theWorkingPlane Nine double values, defining origin,
        #                         OZ and OX directions of the working plane.
        #  @return New GEOM_Object, containing the created wire.
        #
        #  @ref tui_sketcher_page "Example"
        def MakeSketcher(self, theCommand, theWorkingPlane = [0,0,0, 0,0,1, 1,0,0]):
            # Example: see GEOM_TestAll.py
            theCommand,Parameters = ParseSketcherCommand(theCommand)
            anObj = self.CurvesOp.MakeSketcher(theCommand, theWorkingPlane)
            RaiseIfFailed("MakeSketcher", self.CurvesOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a sketcher (wire or face), following the textual description,
        #  passed through <VAR>theCommand</VAR> argument. \n
        #  For format of the description string see the previous method.\n
        #  @param theCommand String, defining the sketcher in local
        #                    coordinates of the working plane.
        #  @param theWorkingPlane Planar Face or LCS(Marker) of the working plane.
        #  @return New GEOM_Object, containing the created wire.
        #
        #  @ref tui_sketcher_page "Example"
        def MakeSketcherOnPlane(self, theCommand, theWorkingPlane):
            anObj = self.CurvesOp.MakeSketcherOnPlane(theCommand, theWorkingPlane)
            RaiseIfFailed("MakeSketcherOnPlane", self.CurvesOp)
            return anObj

        ## Create a sketcher wire, following the numerical description,
        #  passed through <VAR>theCoordinates</VAR> argument. \n
        #  @param theCoordinates double values, defining points to create a wire,
        #                                                      passing from it.
        #  @return New GEOM_Object, containing the created wire.
        #
        #  @ref tui_sketcher_page "Example"
        def Make3DSketcher(self, theCoordinates):
            theCoordinates,Parameters = ParseParameters(theCoordinates)
            anObj = self.CurvesOp.Make3DSketcher(theCoordinates)
            RaiseIfFailed("Make3DSketcher", self.CurvesOp)
            anObj.SetParameters(Parameters)
            return anObj

        # end of l3_sketcher
        ## @}

        ## @addtogroup l3_3d_primitives
        ## @{

        ## Create a box by coordinates of two opposite vertices.
        #
        #  @ref tui_creation_box "Example"
        def MakeBox(self,x1,y1,z1,x2,y2,z2):
            # Example: see GEOM_TestAll.py
            pnt1 = self.MakeVertex(x1,y1,z1)
            pnt2 = self.MakeVertex(x2,y2,z2)
            return self.MakeBoxTwoPnt(pnt1,pnt2)

        ## Create a box with specified dimensions along the coordinate axes
        #  and with edges, parallel to the coordinate axes.
        #  Center of the box will be at point (DX/2, DY/2, DZ/2).
        #  @param theDX Length of Box edges, parallel to OX axis.
        #  @param theDY Length of Box edges, parallel to OY axis.
        #  @param theDZ Length of Box edges, parallel to OZ axis.
        #  @return New GEOM_Object, containing the created box.
        #
        #  @ref tui_creation_box "Example"
        def MakeBoxDXDYDZ(self,theDX, theDY, theDZ):
            # Example: see GEOM_TestAll.py
            theDX,theDY,theDZ,Parameters = ParseParameters(theDX, theDY, theDZ)
            anObj = self.PrimOp.MakeBoxDXDYDZ(theDX, theDY, theDZ)
            RaiseIfFailed("MakeBoxDXDYDZ", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a box with two specified opposite vertices,
        #  and with edges, parallel to the coordinate axes
        #  @param thePnt1 First of two opposite vertices.
        #  @param thePnt2 Second of two opposite vertices.
        #  @return New GEOM_Object, containing the created box.
        #
        #  @ref tui_creation_box "Example"
        def MakeBoxTwoPnt(self,thePnt1, thePnt2):
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakeBoxTwoPnt(thePnt1, thePnt2)
            RaiseIfFailed("MakeBoxTwoPnt", self.PrimOp)
            return anObj

        ## Create a face with specified dimensions along OX-OY coordinate axes,
        #  with edges, parallel to this coordinate axes.
        #  @param theH height of Face.
        #  @param theW width of Face.
        #  @param theOrientation orientation belong axis OXY OYZ OZX
        #  @return New GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_face "Example"
        def MakeFaceHW(self,theH, theW, theOrientation):
            # Example: see GEOM_TestAll.py
            theH,theW,Parameters = ParseParameters(theH, theW)
            anObj = self.PrimOp.MakeFaceHW(theH, theW, theOrientation)
            RaiseIfFailed("MakeFaceHW", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a face from another plane and two sizes,
        #  vertical size and horisontal size.
        #  @param theObj   Normale vector to the creating face or
        #  the face object.
        #  @param theH     Height (vertical size).
        #  @param theW     Width (horisontal size).
        #  @return New GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_face "Example"
        def MakeFaceObjHW(self, theObj, theH, theW):
            # Example: see GEOM_TestAll.py
            theH,theW,Parameters = ParseParameters(theH, theW)
            anObj = self.PrimOp.MakeFaceObjHW(theObj, theH, theW)
            RaiseIfFailed("MakeFaceObjHW", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a disk with given center, normal vector and radius.
        #  @param thePnt Disk center.
        #  @param theVec Vector, normal to the plane of the disk.
        #  @param theR Disk radius.
        #  @return New GEOM_Object, containing the created disk.
        #
        #  @ref tui_creation_disk "Example"
        def MakeDiskPntVecR(self,thePnt, theVec, theR):
            # Example: see GEOM_TestAll.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.PrimOp.MakeDiskPntVecR(thePnt, theVec, theR)
            RaiseIfFailed("MakeDiskPntVecR", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a disk, passing through three given points
        #  @param thePnt1,thePnt2,thePnt3 Points, defining the disk.
        #  @return New GEOM_Object, containing the created disk.
        #
        #  @ref tui_creation_disk "Example"
        def MakeDiskThreePnt(self,thePnt1, thePnt2, thePnt3):
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakeDiskThreePnt(thePnt1, thePnt2, thePnt3)
            RaiseIfFailed("MakeDiskThreePnt", self.PrimOp)
            return anObj

        ## Create a disk with specified dimensions along OX-OY coordinate axes.
        #  @param theR Radius of Face.
        #  @param theOrientation set the orientation belong axis OXY or OYZ or OZX
        #  @return New GEOM_Object, containing the created disk.
        #
        #  @ref tui_creation_face "Example"
        def MakeDiskR(self,theR, theOrientation):
            # Example: see GEOM_TestAll.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.PrimOp.MakeDiskR(theR, theOrientation)
            RaiseIfFailed("MakeDiskR", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a cylinder with given base point, axis, radius and height.
        #  @param thePnt Central point of cylinder base.
        #  @param theAxis Cylinder axis.
        #  @param theR Cylinder radius.
        #  @param theH Cylinder height.
        #  @return New GEOM_Object, containing the created cylinder.
        #
        #  @ref tui_creation_cylinder "Example"
        def MakeCylinder(self,thePnt, theAxis, theR, theH):
            # Example: see GEOM_TestAll.py
            theR,theH,Parameters = ParseParameters(theR, theH)
            anObj = self.PrimOp.MakeCylinderPntVecRH(thePnt, theAxis, theR, theH)
            RaiseIfFailed("MakeCylinderPntVecRH", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a cylinder with given radius and height at
        #  the origin of coordinate system. Axis of the cylinder
        #  will be collinear to the OZ axis of the coordinate system.
        #  @param theR Cylinder radius.
        #  @param theH Cylinder height.
        #  @return New GEOM_Object, containing the created cylinder.
        #
        #  @ref tui_creation_cylinder "Example"
        def MakeCylinderRH(self,theR, theH):
            # Example: see GEOM_TestAll.py
            theR,theH,Parameters = ParseParameters(theR, theH)
            anObj = self.PrimOp.MakeCylinderRH(theR, theH)
            RaiseIfFailed("MakeCylinderRH", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a sphere with given center and radius.
        #  @param thePnt Sphere center.
        #  @param theR Sphere radius.
        #  @return New GEOM_Object, containing the created sphere.
        #
        #  @ref tui_creation_sphere "Example"
        def MakeSpherePntR(self, thePnt, theR):
            # Example: see GEOM_TestAll.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.PrimOp.MakeSpherePntR(thePnt, theR)
            RaiseIfFailed("MakeSpherePntR", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a sphere with given center and radius.
        #  @param x,y,z Coordinates of sphere center.
        #  @param theR Sphere radius.
        #  @return New GEOM_Object, containing the created sphere.
        #
        #  @ref tui_creation_sphere "Example"
        def MakeSphere(self, x, y, z, theR):
            # Example: see GEOM_TestAll.py
            point = self.MakeVertex(x, y, z)
            anObj = self.MakeSpherePntR(point, theR)
            return anObj

        ## Create a sphere with given radius at the origin of coordinate system.
        #  @param theR Sphere radius.
        #  @return New GEOM_Object, containing the created sphere.
        #
        #  @ref tui_creation_sphere "Example"
        def MakeSphereR(self, theR):
            # Example: see GEOM_TestAll.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.PrimOp.MakeSphereR(theR)
            RaiseIfFailed("MakeSphereR", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a cone with given base point, axis, height and radiuses.
        #  @param thePnt Central point of the first cone base.
        #  @param theAxis Cone axis.
        #  @param theR1 Radius of the first cone base.
        #  @param theR2 Radius of the second cone base.
        #    \note If both radiuses are non-zero, the cone will be truncated.
        #    \note If the radiuses are equal, a cylinder will be created instead.
        #  @param theH Cone height.
        #  @return New GEOM_Object, containing the created cone.
        #
        #  @ref tui_creation_cone "Example"
        def MakeCone(self,thePnt, theAxis, theR1, theR2, theH):
            # Example: see GEOM_TestAll.py
            theR1,theR2,theH,Parameters = ParseParameters(theR1,theR2,theH)
            anObj = self.PrimOp.MakeConePntVecR1R2H(thePnt, theAxis, theR1, theR2, theH)
            RaiseIfFailed("MakeConePntVecR1R2H", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a cone with given height and radiuses at
        #  the origin of coordinate system. Axis of the cone will
        #  be collinear to the OZ axis of the coordinate system.
        #  @param theR1 Radius of the first cone base.
        #  @param theR2 Radius of the second cone base.
        #    \note If both radiuses are non-zero, the cone will be truncated.
        #    \note If the radiuses are equal, a cylinder will be created instead.
        #  @param theH Cone height.
        #  @return New GEOM_Object, containing the created cone.
        #
        #  @ref tui_creation_cone "Example"
        def MakeConeR1R2H(self,theR1, theR2, theH):
            # Example: see GEOM_TestAll.py
            theR1,theR2,theH,Parameters = ParseParameters(theR1,theR2,theH)
            anObj = self.PrimOp.MakeConeR1R2H(theR1, theR2, theH)
            RaiseIfFailed("MakeConeR1R2H", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a torus with given center, normal vector and radiuses.
        #  @param thePnt Torus central point.
        #  @param theVec Torus axis of symmetry.
        #  @param theRMajor Torus major radius.
        #  @param theRMinor Torus minor radius.
        #  @return New GEOM_Object, containing the created torus.
        #
        #  @ref tui_creation_torus "Example"
        def MakeTorus(self, thePnt, theVec, theRMajor, theRMinor):
            # Example: see GEOM_TestAll.py
            theRMajor,theRMinor,Parameters = ParseParameters(theRMajor,theRMinor)
            anObj = self.PrimOp.MakeTorusPntVecRR(thePnt, theVec, theRMajor, theRMinor)
            RaiseIfFailed("MakeTorusPntVecRR", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a torus with given radiuses at the origin of coordinate system.
        #  @param theRMajor Torus major radius.
        #  @param theRMinor Torus minor radius.
        #  @return New GEOM_Object, containing the created torus.
        #
        #  @ref tui_creation_torus "Example"
        def MakeTorusRR(self, theRMajor, theRMinor):
            # Example: see GEOM_TestAll.py
            theRMajor,theRMinor,Parameters = ParseParameters(theRMajor,theRMinor)
            anObj = self.PrimOp.MakeTorusRR(theRMajor, theRMinor)
            RaiseIfFailed("MakeTorusRR", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        # end of l3_3d_primitives
        ## @}

        ## @addtogroup l3_complex
        ## @{

        ## Create a shape by extrusion of the base shape along a vector, defined by two points.
        #  @param theBase Base shape to be extruded.
        #  @param thePoint1 First end of extrusion vector.
        #  @param thePoint2 Second end of extrusion vector.
        #  @return New GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        def MakePrism(self, theBase, thePoint1, thePoint2):
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakePrismTwoPnt(theBase, thePoint1, thePoint2)
            RaiseIfFailed("MakePrismTwoPnt", self.PrimOp)
            return anObj

        ## Create a shape by extrusion of the base shape along the vector,
        #  i.e. all the space, transfixed by the base shape during its translation
        #  along the vector on the given distance.
        #  @param theBase Base shape to be extruded.
        #  @param theVec Direction of extrusion.
        #  @param theH Prism dimension along theVec.
        #  @return New GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        def MakePrismVecH(self, theBase, theVec, theH):
            # Example: see GEOM_TestAll.py
            theH,Parameters = ParseParameters(theH)
            anObj = self.PrimOp.MakePrismVecH(theBase, theVec, theH)
            RaiseIfFailed("MakePrismVecH", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a shape by extrusion of the base shape along the vector,
        #  i.e. all the space, transfixed by the base shape during its translation
        #  along the vector on the given distance in 2 Ways (forward/backward) .
        #  @param theBase Base shape to be extruded.
        #  @param theVec Direction of extrusion.
        #  @param theH Prism dimension along theVec in forward direction.
        #  @return New GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        def MakePrismVecH2Ways(self, theBase, theVec, theH):
            # Example: see GEOM_TestAll.py
            theH,Parameters = ParseParameters(theH)
            anObj = self.PrimOp.MakePrismVecH2Ways(theBase, theVec, theH)
            RaiseIfFailed("MakePrismVecH2Ways", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a shape by extrusion of the base shape along the dx, dy, dz direction
        #  @param theBase Base shape to be extruded.
        #  @param theDX, theDY, theDZ Directions of extrusion.
        #  @return New GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        def MakePrismDXDYDZ(self, theBase, theDX, theDY, theDZ):
            # Example: see GEOM_TestAll.py
            theDX,theDY,theDZ,Parameters = ParseParameters(theDX, theDY, theDZ)
            anObj = self.PrimOp.MakePrismDXDYDZ(theBase, theDX, theDY, theDZ)
            RaiseIfFailed("MakePrismDXDYDZ", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a shape by extrusion of the base shape along the dx, dy, dz direction
        #  i.e. all the space, transfixed by the base shape during its translation
        #  along the vector on the given distance in 2 Ways (forward/backward) .
        #  @param theBase Base shape to be extruded.
        #  @param theDX, theDY, theDZ Directions of extrusion.
        #  @return New GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        def MakePrismDXDYDZ2Ways(self, theBase, theDX, theDY, theDZ):
            # Example: see GEOM_TestAll.py
            theDX,theDY,theDZ,Parameters = ParseParameters(theDX, theDY, theDZ)
            anObj = self.PrimOp.MakePrismDXDYDZ2Ways(theBase, theDX, theDY, theDZ)
            RaiseIfFailed("MakePrismDXDYDZ2Ways", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a shape by revolution of the base shape around the axis
        #  on the given angle, i.e. all the space, transfixed by the base
        #  shape during its rotation around the axis on the given angle.
        #  @param theBase Base shape to be rotated.
        #  @param theAxis Rotation axis.
        #  @param theAngle Rotation angle in radians.
        #  @return New GEOM_Object, containing the created revolution.
        #
        #  @ref tui_creation_revolution "Example"
        def MakeRevolution(self, theBase, theAxis, theAngle):
            # Example: see GEOM_TestAll.py
            theAngle,Parameters = ParseParameters(theAngle)
            anObj = self.PrimOp.MakeRevolutionAxisAngle(theBase, theAxis, theAngle)
            RaiseIfFailed("MakeRevolutionAxisAngle", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## The Same Revolution but in both ways forward&backward.
        def MakeRevolution2Ways(self, theBase, theAxis, theAngle):
            theAngle,Parameters = ParseParameters(theAngle)
            anObj = self.PrimOp.MakeRevolutionAxisAngle2Ways(theBase, theAxis, theAngle)
            RaiseIfFailed("MakeRevolutionAxisAngle2Ways", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a filling from the given compound of contours.
        #  @param theShape the compound of contours
        #  @param theMinDeg a minimal degree of BSpline surface to create
        #  @param theMaxDeg a maximal degree of BSpline surface to create
        #  @param theTol2D a 2d tolerance to be reached
        #  @param theTol3D a 3d tolerance to be reached
        #  @param theNbIter a number of iteration of approximation algorithm
        #  @param theMethod Kind of method to perform filling operation:
        #                   GEOM.FOM_Default - Default - standard behaviour
        #                   /GEOM.FOM_UseOri - Use edges orientation - orientation of edges is
        #                       used: if the edge is reversed, the curve from this edge
        #                       is reversed before using it in the filling algorithm.
        #                   /GEOM.FOM_AutoCorrect - Auto-correct orientation - changes the orientation
        #                       of the curves using minimization of sum of distances
        #                       between the end points of the edges.
        #  @param isApprox if True, BSpline curves are generated in the process
        #                  of surface construction. By default it is False, that means
        #                  the surface is created using Besier curves. The usage of
        #                  Approximation makes the algorithm work slower, but allows
        #                  building the surface for rather complex cases
        #  @return New GEOM_Object, containing the created filling surface.
        #
        #  @ref tui_creation_filling "Example"
        def MakeFilling(self, theShape, theMinDeg, theMaxDeg, theTol2D,
                        theTol3D, theNbIter, theMethod=GEOM.FOM_Default, isApprox=0):
            # Example: see GEOM_TestAll.py
            theMinDeg,theMaxDeg,theTol2D,theTol3D,theNbIter,Parameters = ParseParameters(theMinDeg, theMaxDeg, theTol2D, theTol3D, theNbIter)
            anObj = self.PrimOp.MakeFilling(theShape, theMinDeg, theMaxDeg,
                                            theTol2D, theTol3D, theNbIter,
                                            theMethod, isApprox)
            RaiseIfFailed("MakeFilling", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a shell or solid passing through set of sections.Sections should be wires,edges or vertices.
        #  @param theSeqSections - set of specified sections.
        #  @param theModeSolid - mode defining building solid or shell
        #  @param thePreci - precision 3D used for smoothing by default 1.e-6
        #  @param theRuled - mode defining type of the result surfaces (ruled or smoothed).
        #  @return New GEOM_Object, containing the created shell or solid.
        #
        #  @ref swig_todo "Example"
        def MakeThruSections(self,theSeqSections,theModeSolid,thePreci,theRuled):
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakeThruSections(theSeqSections,theModeSolid,thePreci,theRuled)
            RaiseIfFailed("MakeThruSections", self.PrimOp)
            return anObj

        ## Create a shape by extrusion of the base shape along
        #  the path shape. The path shape can be a wire or an edge.
        #  @param theBase Base shape to be extruded.
        #  @param thePath Path shape to extrude the base shape along it.
        #  @return New GEOM_Object, containing the created pipe.
        #
        #  @ref tui_creation_pipe "Example"
        def MakePipe(self,theBase, thePath):
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakePipe(theBase, thePath)
            RaiseIfFailed("MakePipe", self.PrimOp)
            return anObj

        ## Create a shape by extrusion of the profile shape along
        #  the path shape. The path shape can be a wire or an edge.
        #  the several profiles can be specified in the several locations of path.
        #  @param theSeqBases - list of  Bases shape to be extruded.
        #  @param theLocations - list of locations on the path corresponding
        #                        specified list of the Bases shapes. Number of locations
        #                        should be equal to number of bases or list of locations can be empty.
        #  @param thePath - Path shape to extrude the base shape along it.
        #  @param theWithContact - the mode defining that the section is translated to be in
        #                          contact with the spine.
        #  @param theWithCorrection - defining that the section is rotated to be
        #                             orthogonal to the spine tangent in the correspondent point
        #  @return New GEOM_Object, containing the created pipe.
        #
        #  @ref tui_creation_pipe_with_diff_sec "Example"
        def MakePipeWithDifferentSections(self, theSeqBases,
                                          theLocations, thePath,
                                          theWithContact, theWithCorrection):
            anObj = self.PrimOp.MakePipeWithDifferentSections(theSeqBases,
                                                              theLocations, thePath,
                                                              theWithContact, theWithCorrection)
            RaiseIfFailed("MakePipeWithDifferentSections", self.PrimOp)
            return anObj

        ## Create a shape by extrusion of the profile shape along
        #  the path shape. The path shape can be a wire or a edge.
        #  the several profiles can be specified in the several locations of path.
        #  @param theSeqBases - list of  Bases shape to be extruded. Base shape must be
        #                       shell or face. If number of faces in neighbour sections
        #                       aren't coincided result solid between such sections will
        #                       be created using external boundaries of this shells.
        #  @param theSeqSubBases - list of corresponding subshapes of section shapes.
        #                          This list is used for searching correspondences between
        #                          faces in the sections. Size of this list must be equal
        #                          to size of list of base shapes.
        #  @param theLocations - list of locations on the path corresponding
        #                        specified list of the Bases shapes. Number of locations
        #                        should be equal to number of bases. First and last
        #                        locations must be coincided with first and last vertexes
        #                        of path correspondingly.
        #  @param thePath - Path shape to extrude the base shape along it.
        #  @param theWithContact - the mode defining that the section is translated to be in
        #                          contact with the spine.
        #  @param theWithCorrection - defining that the section is rotated to be
        #                             orthogonal to the spine tangent in the correspondent point
        #  @return New GEOM_Object, containing the created solids.
        #
        #  @ref tui_creation_pipe_with_shell_sec "Example"
        def MakePipeWithShellSections(self,theSeqBases, theSeqSubBases,
                                      theLocations, thePath,
                                      theWithContact, theWithCorrection):
            anObj = self.PrimOp.MakePipeWithShellSections(theSeqBases, theSeqSubBases,
                                                          theLocations, thePath,
                                                          theWithContact, theWithCorrection)
            RaiseIfFailed("MakePipeWithShellSections", self.PrimOp)
            return anObj

        ## Create a shape by extrusion of the profile shape along
        #  the path shape. This function is used only for debug pipe
        #  functionality - it is a version of previous function
        #  (MakePipeWithShellSections(...)) which give a possibility to
        #  recieve information about creating pipe between each pair of
        #  sections step by step.
        def MakePipeWithShellSectionsBySteps(self, theSeqBases, theSeqSubBases,
                                             theLocations, thePath,
                                             theWithContact, theWithCorrection):
            res = []
            nbsect = len(theSeqBases)
            nbsubsect = len(theSeqSubBases)
            #print "nbsect = ",nbsect
            for i in range(1,nbsect):
                #print "  i = ",i
                tmpSeqBases = [ theSeqBases[i-1], theSeqBases[i] ]
                tmpLocations = [ theLocations[i-1], theLocations[i] ]
                tmpSeqSubBases = []
                if nbsubsect>0: tmpSeqSubBases = [ theSeqSubBases[i-1], theSeqSubBases[i] ]
                anObj = self.PrimOp.MakePipeWithShellSections(tmpSeqBases, tmpSeqSubBases,
                                                              tmpLocations, thePath,
                                                              theWithContact, theWithCorrection)
                if self.PrimOp.IsDone() == 0:
                    print "Problems with pipe creation between ",i," and ",i+1," sections"
                    RaiseIfFailed("MakePipeWithShellSections", self.PrimOp)
                    break
                else:
                    print "Pipe between ",i," and ",i+1," sections is OK"
                    res.append(anObj)
                    pass
                pass

            resc = self.MakeCompound(res)
            #resc = self.MakeSewing(res, 0.001)
            #print "resc: ",resc
            return resc

        ## Create solids between given sections
        #  @param theSeqBases - list of sections (shell or face).
        #  @param theLocations - list of corresponding vertexes
        #  @return New GEOM_Object, containing the created solids.
        #
        #  @ref tui_creation_pipe_without_path "Example"
        def MakePipeShellsWithoutPath(self, theSeqBases, theLocations):
            anObj = self.PrimOp.MakePipeShellsWithoutPath(theSeqBases, theLocations)
            RaiseIfFailed("MakePipeShellsWithoutPath", self.PrimOp)
            return anObj

        ## Create a shape by extrusion of the base shape along
        #  the path shape with constant bi-normal direction along the given vector.
        #  The path shape can be a wire or an edge.
        #  @param theBase Base shape to be extruded.
        #  @param thePath Path shape to extrude the base shape along it.
        #  @param theVec Vector defines a constant binormal direction to keep the
        #                same angle beetween the direction and the sections
        #                along the sweep surface.
        #  @return New GEOM_Object, containing the created pipe.
        #
        #  @ref tui_creation_pipe "Example"
        def MakePipeBiNormalAlongVector(self,theBase, thePath, theVec):
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakePipeBiNormalAlongVector(theBase, thePath, theVec)
            RaiseIfFailed("MakePipeBiNormalAlongVector", self.PrimOp)
            return anObj

        # end of l3_complex
        ## @}

        ## @addtogroup l3_advanced
        ## @{

        ## Create a linear edge with specified ends.
        #  @param thePnt1 Point for the first end of edge.
        #  @param thePnt2 Point for the second end of edge.
        #  @return New GEOM_Object, containing the created edge.
        #
        #  @ref tui_creation_edge "Example"
        def MakeEdge(self,thePnt1, thePnt2):
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeEdge(thePnt1, thePnt2)
            RaiseIfFailed("MakeEdge", self.ShapesOp)
            return anObj

        ## Create an edge from specified wire.
        #  @param theWire source Wire.
        #  @param theLinearTolerance linear tolerance value.
        #  @param theAngularTolerance angular tolerance value.
        #  @return New GEOM_Object, containing the created edge.
        #
        #  @ref tui_creation_edge "Example"
        def MakeEdgeWire(self, theWire, theLinearTolerance = 1e-07, theAngularTolerance = 1e-12):
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeEdgeWire(theWire, theLinearTolerance, theAngularTolerance)
            RaiseIfFailed("MakeEdgeWire", self.ShapesOp)
            return anObj

        ## Create a wire from the set of edges and wires.
        #  @param theEdgesAndWires List of edges and/or wires.
        #  @param theTolerance Maximum distance between vertices, that will be merged.
        #                      Values less than 1e-07 are equivalent to 1e-07 (Precision::Confusion()).
        #  @return New GEOM_Object, containing the created wire.
        #
        #  @ref tui_creation_wire "Example"
        def MakeWire(self, theEdgesAndWires, theTolerance = 1e-07):
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeWire(theEdgesAndWires, theTolerance)
            RaiseIfFailed("MakeWire", self.ShapesOp)
            return anObj

        ## Create a face on the given wire.
        #  @param theWire closed Wire or Edge to build the face on.
        #  @param isPlanarWanted If TRUE, only planar face will be built.
        #                        If impossible, NULL object will be returned.
        #  @return New GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_face "Example"
        def MakeFace(self,theWire, isPlanarWanted):
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeFace(theWire, isPlanarWanted)
            RaiseIfFailed("MakeFace", self.ShapesOp)
            return anObj

        ## Create a face on the given wires set.
        #  @param theWires List of closed wires or edges to build the face on.
        #  @param isPlanarWanted If TRUE, only planar face will be built.
        #                        If impossible, NULL object will be returned.
        #  @return New GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_face "Example"
        def MakeFaceWires(self,theWires, isPlanarWanted):
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeFaceWires(theWires, isPlanarWanted)
            RaiseIfFailed("MakeFaceWires", self.ShapesOp)
            return anObj

        ## Shortcut to MakeFaceWires()
        #
        #  @ref tui_creation_face "Example 1"
        #  \n @ref swig_MakeFaces  "Example 2"
        def MakeFaces(self,theWires, isPlanarWanted):
            # Example: see GEOM_TestOthers.py
            anObj = self.MakeFaceWires(theWires, isPlanarWanted)
            return anObj

        ## Create a shell from the set of faces and shells.
        #  @param theFacesAndShells List of faces and/or shells.
        #  @return New GEOM_Object, containing the created shell.
        #
        #  @ref tui_creation_shell "Example"
        def MakeShell(self,theFacesAndShells):
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeShell(theFacesAndShells)
            RaiseIfFailed("MakeShell", self.ShapesOp)
            return anObj

        ## Create a solid, bounded by the given shells.
        #  @param theShells Sequence of bounding shells.
        #  @return New GEOM_Object, containing the created solid.
        #
        #  @ref tui_creation_solid "Example"
        def MakeSolid(self,theShells):
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeSolidShells(theShells)
            RaiseIfFailed("MakeSolidShells", self.ShapesOp)
            return anObj

        ## Create a compound of the given shapes.
        #  @param theShapes List of shapes to put in compound.
        #  @return New GEOM_Object, containing the created compound.
        #
        #  @ref tui_creation_compound "Example"
        def MakeCompound(self,theShapes):
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeCompound(theShapes)
            RaiseIfFailed("MakeCompound", self.ShapesOp)
            return anObj

        # end of l3_advanced
        ## @}

        ## @addtogroup l2_measure
        ## @{

        ## Gives quantity of faces in the given shape.
        #  @param theShape Shape to count faces of.
        #  @return Quantity of faces.
        #
        #  @ref swig_NumberOf "Example"
        def NumberOfFaces(self, theShape):
            # Example: see GEOM_TestOthers.py
            nb_faces = self.ShapesOp.NumberOfFaces(theShape)
            RaiseIfFailed("NumberOfFaces", self.ShapesOp)
            return nb_faces

        ## Gives quantity of edges in the given shape.
        #  @param theShape Shape to count edges of.
        #  @return Quantity of edges.
        #
        #  @ref swig_NumberOf "Example"
        def NumberOfEdges(self, theShape):
            # Example: see GEOM_TestOthers.py
            nb_edges = self.ShapesOp.NumberOfEdges(theShape)
            RaiseIfFailed("NumberOfEdges", self.ShapesOp)
            return nb_edges

        ## Gives quantity of subshapes of type theShapeType in the given shape.
        #  @param theShape Shape to count subshapes of.
        #  @param theShapeType Type of subshapes to count.
        #  @return Quantity of subshapes of given type.
        #
        #  @ref swig_NumberOf "Example"
        def NumberOfSubShapes(self, theShape, theShapeType):
            # Example: see GEOM_TestOthers.py
            nb_ss = self.ShapesOp.NumberOfSubShapes(theShape, theShapeType)
            RaiseIfFailed("NumberOfSubShapes", self.ShapesOp)
            return nb_ss

        ## Gives quantity of solids in the given shape.
        #  @param theShape Shape to count solids in.
        #  @return Quantity of solids.
        #
        #  @ref swig_NumberOf "Example"
        def NumberOfSolids(self, theShape):
            # Example: see GEOM_TestOthers.py
            nb_solids = self.ShapesOp.NumberOfSubShapes(theShape, ShapeType["SOLID"])
            RaiseIfFailed("NumberOfSolids", self.ShapesOp)
            return nb_solids

        # end of l2_measure
        ## @}

        ## @addtogroup l3_healing
        ## @{

        ## Reverses an orientation the given shape.
        #  @param theShape Shape to be reversed.
        #  @return The reversed copy of theShape.
        #
        #  @ref swig_ChangeOrientation "Example"
        def ChangeOrientation(self,theShape):
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.ChangeOrientation(theShape)
            RaiseIfFailed("ChangeOrientation", self.ShapesOp)
            return anObj

        ## Shortcut to ChangeOrientation()
        #
        #  @ref swig_OrientationChange "Example"
        def OrientationChange(self,theShape):
            # Example: see GEOM_TestOthers.py
            anObj = self.ChangeOrientation(theShape)
            return anObj

        # end of l3_healing
        ## @}

        ## @addtogroup l4_obtain
        ## @{

        ## Retrieve all free faces from the given shape.
        #  Free face is a face, which is not shared between two shells of the shape.
        #  @param theShape Shape to find free faces in.
        #  @return List of IDs of all free faces, contained in theShape.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetFreeFacesIDs(self,theShape):
            # Example: see GEOM_TestOthers.py
            anIDs = self.ShapesOp.GetFreeFacesIDs(theShape)
            RaiseIfFailed("GetFreeFacesIDs", self.ShapesOp)
            return anIDs

        ## Get all sub-shapes of theShape1 of the given type, shared with theShape2.
        #  @param theShape1 Shape to find sub-shapes in.
        #  @param theShape2 Shape to find shared sub-shapes with.
        #  @param theShapeType Type of sub-shapes to be retrieved.
        #  @return List of sub-shapes of theShape1, shared with theShape2.
        #
        #  @ref swig_GetSharedShapes "Example"
        def GetSharedShapes(self,theShape1, theShape2, theShapeType):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetSharedShapes(theShape1, theShape2, theShapeType)
            RaiseIfFailed("GetSharedShapes", self.ShapesOp)
            return aList

        ## Find in <VAR>theShape</VAR> all sub-shapes of type <VAR>theShapeType</VAR>,
        #  situated relatively the specified plane by the certain way,
        #  defined through <VAR>theState</VAR> parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved.
        #  @param theAx1 Vector (or line, or linear edge), specifying normal
        #                direction and location of the plane to find shapes on.
        #  @param theState The state of the subshapes to find. It can be one of
        #   ST_ON, ST_OUT, ST_ONOUT, ST_IN, ST_ONIN.
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnPlane "Example"
        def GetShapesOnPlane(self,theShape, theShapeType, theAx1, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnPlane(theShape, theShapeType, theAx1, theState)
            RaiseIfFailed("GetShapesOnPlane", self.ShapesOp)
            return aList

        ## Works like the above method, but returns list of sub-shapes indices
        #
        #  @ref swig_GetShapesOnPlaneIDs "Example"
        def GetShapesOnPlaneIDs(self,theShape, theShapeType, theAx1, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnPlaneIDs(theShape, theShapeType, theAx1, theState)
            RaiseIfFailed("GetShapesOnPlaneIDs", self.ShapesOp)
            return aList

        ## Find in <VAR>theShape</VAR> all sub-shapes of type <VAR>theShapeType</VAR>,
        #  situated relatively the specified plane by the certain way,
        #  defined through <VAR>theState</VAR> parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved.
        #  @param theAx1 Vector (or line, or linear edge), specifying normal
        #                direction of the plane to find shapes on.
        #  @param thePnt Point specifying location of the plane to find shapes on.
        #  @param theState The state of the subshapes to find. It can be one of
        #                  ST_ON, ST_OUT, ST_ONOUT, ST_IN, ST_ONIN.
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnPlaneWithLocation "Example"
        def GetShapesOnPlaneWithLocation(self, theShape, theShapeType, theAx1, thePnt, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnPlaneWithLocation(theShape, theShapeType,
                                                               theAx1, thePnt, theState)
            RaiseIfFailed("GetShapesOnPlaneWithLocation", self.ShapesOp)
            return aList

        ## Works like the above method, but returns list of sub-shapes indices
        #
        #  @ref swig_GetShapesOnPlaneWithLocationIDs "Example"
        def GetShapesOnPlaneWithLocationIDs(self, theShape, theShapeType, theAx1, thePnt, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnPlaneWithLocationIDs(theShape, theShapeType,
                                                                  theAx1, thePnt, theState)
            RaiseIfFailed("GetShapesOnPlaneWithLocationIDs", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified cylinder by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved.
        #  @param theAxis Vector (or line, or linear edge), specifying
        #                 axis of the cylinder to find shapes on.
        #  @param theRadius Radius of the cylinder to find shapes on.
        #  @param theState The state of the subshapes to find. It can be one of
        #   ST_ON, ST_OUT, ST_ONOUT, ST_IN, ST_ONIN.
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnCylinder "Example"
        def GetShapesOnCylinder(self, theShape, theShapeType, theAxis, theRadius, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnCylinder(theShape, theShapeType, theAxis, theRadius, theState)
            RaiseIfFailed("GetShapesOnCylinder", self.ShapesOp)
            return aList

        ## Works like the above method, but returns list of sub-shapes indices
        #
        #  @ref swig_GetShapesOnCylinderIDs "Example"
        def GetShapesOnCylinderIDs(self, theShape, theShapeType, theAxis, theRadius, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnCylinderIDs(theShape, theShapeType, theAxis, theRadius, theState)
            RaiseIfFailed("GetShapesOnCylinderIDs", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified cylinder by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved.
        #  @param theAxis Vector (or line, or linear edge), specifying
        #                 axis of the cylinder to find shapes on.
        #  @param thePnt Point specifying location of the bottom of the cylinder.
        #  @param theRadius Radius of the cylinder to find shapes on.
        #  @param theState The state of the subshapes to find. It can be one of
        #   ST_ON, ST_OUT, ST_ONOUT, ST_IN, ST_ONIN.
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnCylinderWithLocation "Example"
        def GetShapesOnCylinderWithLocation(self, theShape, theShapeType, theAxis, thePnt, theRadius, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnCylinderWithLocation(theShape, theShapeType, theAxis, thePnt, theRadius, theState)
            RaiseIfFailed("GetShapesOnCylinderWithLocation", self.ShapesOp)
            return aList

        ## Works like the above method, but returns list of sub-shapes indices
        #
        #  @ref swig_GetShapesOnCylinderWithLocationIDs "Example"
        def GetShapesOnCylinderWithLocationIDs(self, theShape, theShapeType, theAxis, thePnt, theRadius, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnCylinderWithLocationIDs(theShape, theShapeType, theAxis, thePnt, theRadius, theState)
            RaiseIfFailed("GetShapesOnCylinderWithLocationIDs", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified sphere by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved.
        #  @param theCenter Point, specifying center of the sphere to find shapes on.
        #  @param theRadius Radius of the sphere to find shapes on.
        #  @param theState The state of the subshapes to find. It can be one of
        #   ST_ON, ST_OUT, ST_ONOUT, ST_IN, ST_ONIN.
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnSphere "Example"
        def GetShapesOnSphere(self,theShape, theShapeType, theCenter, theRadius, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnSphere(theShape, theShapeType, theCenter, theRadius, theState)
            RaiseIfFailed("GetShapesOnSphere", self.ShapesOp)
            return aList

        ## Works like the above method, but returns list of sub-shapes indices
        #
        #  @ref swig_GetShapesOnSphereIDs "Example"
        def GetShapesOnSphereIDs(self,theShape, theShapeType, theCenter, theRadius, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnSphereIDs(theShape, theShapeType, theCenter, theRadius, theState)
            RaiseIfFailed("GetShapesOnSphereIDs", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified quadrangle by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved.
        #  @param theTopLeftPoint Point, specifying top left corner of a quadrangle
        #  @param theTopRigthPoint Point, specifying top right corner of a quadrangle
        #  @param theBottomLeftPoint Point, specifying bottom left corner of a quadrangle
        #  @param theBottomRigthPoint Point, specifying bottom right corner of a quadrangle
        #  @param theState The state of the subshapes to find. It can be one of
        #                  ST_ON, ST_OUT, ST_ONOUT, ST_IN, ST_ONIN.
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnQuadrangle "Example"
        def GetShapesOnQuadrangle(self, theShape, theShapeType,
                                  theTopLeftPoint, theTopRigthPoint,
                                  theBottomLeftPoint, theBottomRigthPoint, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnQuadrangle(theShape, theShapeType,
                                                        theTopLeftPoint, theTopRigthPoint,
                                                        theBottomLeftPoint, theBottomRigthPoint, theState)
            RaiseIfFailed("GetShapesOnQuadrangle", self.ShapesOp)
            return aList

        ## Works like the above method, but returns list of sub-shapes indices
        #
        #  @ref swig_GetShapesOnQuadrangleIDs "Example"
        def GetShapesOnQuadrangleIDs(self, theShape, theShapeType,
                                     theTopLeftPoint, theTopRigthPoint,
                                     theBottomLeftPoint, theBottomRigthPoint, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnQuadrangleIDs(theShape, theShapeType,
                                                           theTopLeftPoint, theTopRigthPoint,
                                                           theBottomLeftPoint, theBottomRigthPoint, theState)
            RaiseIfFailed("GetShapesOnQuadrangleIDs", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified \a theBox by the certain way, defined through \a theState parameter.
        #  @param theBox Shape for relative comparing.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved.
        #  @param theState The state of the subshapes to find. It can be one of
        #                  ST_ON, ST_OUT, ST_ONOUT, ST_IN, ST_ONIN.
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnBox "Example"
        def GetShapesOnBox(self, theBox, theShape, theShapeType, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnBox(theBox, theShape, theShapeType, theState)
            RaiseIfFailed("GetShapesOnBox", self.ShapesOp)
            return aList

        ## Works like the above method, but returns list of sub-shapes indices
        #
        #  @ref swig_GetShapesOnBoxIDs "Example"
        def GetShapesOnBoxIDs(self, theBox, theShape, theShapeType, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnBoxIDs(theBox, theShape, theShapeType, theState)
            RaiseIfFailed("GetShapesOnBoxIDs", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType,
        #  situated relatively the specified \a theCheckShape by the
        #  certain way, defined through \a theState parameter.
        #  @param theCheckShape Shape for relative comparing. It must be a solid.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved.
        #  @param theState The state of the subshapes to find. It can be one of
        #                  ST_ON, ST_OUT, ST_ONOUT, ST_IN, ST_ONIN.
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnShape "Example"
        def GetShapesOnShape(self, theCheckShape, theShape, theShapeType, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnShape(theCheckShape, theShape,
                                                   theShapeType, theState)
            RaiseIfFailed("GetShapesOnShape", self.ShapesOp)
            return aList

        ## Works like the above method, but returns result as compound
        #
        #  @ref swig_GetShapesOnShapeAsCompound "Example"
        def GetShapesOnShapeAsCompound(self, theCheckShape, theShape, theShapeType, theState):
            # Example: see GEOM_TestOthers.py
            anObj = self.ShapesOp.GetShapesOnShapeAsCompound(theCheckShape, theShape,
                                                             theShapeType, theState)
            RaiseIfFailed("GetShapesOnShapeAsCompound", self.ShapesOp)
            return anObj

        ## Works like the above method, but returns list of sub-shapes indices
        #
        #  @ref swig_GetShapesOnShapeIDs "Example"
        def GetShapesOnShapeIDs(self, theCheckShape, theShape, theShapeType, theState):
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnShapeIDs(theCheckShape, theShape,
                                                      theShapeType, theState)
            RaiseIfFailed("GetShapesOnShapeIDs", self.ShapesOp)
            return aList

        ## Get sub-shape(s) of theShapeWhere, which are
        #  coincident with \a theShapeWhat or could be a part of it.
        #  @param theShapeWhere Shape to find sub-shapes of.
        #  @param theShapeWhat Shape, specifying what to find.
        #  @return Group of all found sub-shapes or a single found sub-shape.
        #
        #  @ref swig_GetInPlace "Example"
        def GetInPlace(self,theShapeWhere, theShapeWhat):
            # Example: see GEOM_TestOthers.py
            anObj = self.ShapesOp.GetInPlace(theShapeWhere, theShapeWhat)
            RaiseIfFailed("GetInPlace", self.ShapesOp)
            return anObj

        ## Get sub-shape(s) of \a theShapeWhere, which are
        #  coincident with \a theShapeWhat or could be a part of it.
        #
        #  Implementation of this method is based on a saved history of an operation,
        #  produced \a theShapeWhere. The \a theShapeWhat must be among this operation's
        #  arguments (an argument shape or a sub-shape of an argument shape).
        #  The operation could be the Partition or one of boolean operations,
        #  performed on simple shapes (not on compounds).
        #
        #  @param theShapeWhere Shape to find sub-shapes of.
        #  @param theShapeWhat Shape, specifying what to find (must be in the
        #                      building history of the ShapeWhere).
        #  @return Group of all found sub-shapes or a single found sub-shape.
        #
        #  @ref swig_GetInPlace "Example"
        def GetInPlaceByHistory(self, theShapeWhere, theShapeWhat):
            # Example: see GEOM_TestOthers.py
            anObj = self.ShapesOp.GetInPlaceByHistory(theShapeWhere, theShapeWhat)
            RaiseIfFailed("GetInPlaceByHistory", self.ShapesOp)
            return anObj

        ## Get sub-shape of theShapeWhere, which is
        #  equal to \a theShapeWhat.
        #  @param theShapeWhere Shape to find sub-shape of.
        #  @param theShapeWhat Shape, specifying what to find.
        #  @return New GEOM_Object for found sub-shape.
        #
        #  @ref swig_GetSame "Example"
        def GetSame(self,theShapeWhere, theShapeWhat):
            anObj = self.ShapesOp.GetSame(theShapeWhere, theShapeWhat)
            RaiseIfFailed("GetSame", self.ShapesOp)
            return anObj

        # end of l4_obtain
        ## @}

        ## @addtogroup l4_access
        ## @{

        ## Obtain a composite sub-shape of <VAR>aShape</VAR>, composed from sub-shapes
        #  of aShape, selected by their unique IDs inside <VAR>aShape</VAR>
        #
        #  @ref swig_all_decompose "Example"
        def GetSubShape(self, aShape, ListOfID):
            # Example: see GEOM_TestAll.py
            anObj = self.AddSubShape(aShape,ListOfID)
            return anObj

        ## Obtain unique ID of sub-shape <VAR>aSubShape</VAR> inside <VAR>aShape</VAR>
        #
        #  @ref swig_all_decompose "Example"
        def GetSubShapeID(self, aShape, aSubShape):
            # Example: see GEOM_TestAll.py
            anID = self.LocalOp.GetSubShapeIndex(aShape, aSubShape)
            RaiseIfFailed("GetSubShapeIndex", self.LocalOp)
            return anID

        # end of l4_access
        ## @}

        ## @addtogroup l4_decompose
        ## @{

        ## Get all sub-shapes and groups of \a theShape,
        #  that were created already by any other methods.
        #  @param theShape Any shape.
        #  @param theGroupsOnly If this parameter is TRUE, only groups will be
        #                       returned, else all found sub-shapes and groups.
        #  @return List of existing sub-objects of \a theShape.
        #
        #  @ref swig_all_decompose "Example"
        def GetExistingSubObjects(self, theShape, theGroupsOnly = False):
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.GetExistingSubObjects(theShape, theGroupsOnly)
            RaiseIfFailed("GetExistingSubObjects", self.ShapesOp)
            return ListObj

        ## Get all groups of \a theShape,
        #  that were created already by any other methods.
        #  @param theShape Any shape.
        #  @return List of existing groups of \a theShape.
        #
        #  @ref swig_all_decompose "Example"
        def GetGroups(self, theShape):
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.GetExistingSubObjects(theShape, True)
            RaiseIfFailed("GetExistingSubObjects", self.ShapesOp)
            return ListObj

        ## Explode a shape on subshapes of a given type.
        #  @param aShape Shape to be exploded.
        #  @param aType Type of sub-shapes to be retrieved.
        #  @return List of sub-shapes of type theShapeType, contained in theShape.
        #
        #  @ref swig_all_decompose "Example"
        def SubShapeAll(self, aShape, aType):
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.MakeExplode(aShape,aType,0)
            RaiseIfFailed("MakeExplode", self.ShapesOp)
            return ListObj

        ## Explode a shape on subshapes of a given type.
        #  @param aShape Shape to be exploded.
        #  @param aType Type of sub-shapes to be retrieved.
        #  @return List of IDs of sub-shapes.
        #
        #  @ref swig_all_decompose "Example"
        def SubShapeAllIDs(self, aShape, aType):
            ListObj = self.ShapesOp.SubShapeAllIDs(aShape,aType,0)
            RaiseIfFailed("SubShapeAllIDs", self.ShapesOp)
            return ListObj

        ## Explode a shape on subshapes of a given type.
        #  Sub-shapes will be sorted by coordinates of their gravity centers.
        #  @param aShape Shape to be exploded.
        #  @param aType Type of sub-shapes to be retrieved.
        #  @return List of sub-shapes of type theShapeType, contained in theShape.
        #
        #  @ref swig_SubShapeAllSorted "Example"
        def SubShapeAllSorted(self, aShape, aType):
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.MakeExplode(aShape,aType,1)
            RaiseIfFailed("MakeExplode", self.ShapesOp)
            return ListObj

        ## Explode a shape on subshapes of a given type.
        #  Sub-shapes will be sorted by coordinates of their gravity centers.
        #  @param aShape Shape to be exploded.
        #  @param aType Type of sub-shapes to be retrieved.
        #  @return List of IDs of sub-shapes.
        #
        #  @ref swig_all_decompose "Example"
        def SubShapeAllSortedIDs(self, aShape, aType):
            ListIDs = self.ShapesOp.SubShapeAllIDs(aShape,aType,1)
            RaiseIfFailed("SubShapeAllIDs", self.ShapesOp)
            return ListIDs

        ## Obtain a compound of sub-shapes of <VAR>aShape</VAR>,
        #  selected by they indices in list of all sub-shapes of type <VAR>aType</VAR>.
        #  Each index is in range [1, Nb_Sub-Shapes_Of_Given_Type]
        #
        #  @ref swig_all_decompose "Example"
        def SubShape(self, aShape, aType, ListOfInd):
            # Example: see GEOM_TestAll.py
            ListOfIDs = []
            AllShapeList = self.SubShapeAll(aShape, aType)
            for ind in ListOfInd:
                ListOfIDs.append(self.GetSubShapeID(aShape, AllShapeList[ind - 1]))
            anObj = self.GetSubShape(aShape, ListOfIDs)
            return anObj

        ## Obtain a compound of sub-shapes of <VAR>aShape</VAR>,
        #  selected by they indices in sorted list of all sub-shapes of type <VAR>aType</VAR>.
        #  Each index is in range [1, Nb_Sub-Shapes_Of_Given_Type]
        #
        #  @ref swig_all_decompose "Example"
        def SubShapeSorted(self,aShape, aType, ListOfInd):
            # Example: see GEOM_TestAll.py
            ListOfIDs = []
            AllShapeList = self.SubShapeAllSorted(aShape, aType)
            for ind in ListOfInd:
                ListOfIDs.append(self.GetSubShapeID(aShape, AllShapeList[ind - 1]))
            anObj = self.GetSubShape(aShape, ListOfIDs)
            return anObj

        # end of l4_decompose
        ## @}

        ## @addtogroup l3_healing
        ## @{

        ## Apply a sequence of Shape Healing operators to the given object.
        #  @param theShape Shape to be processed.
        #  @param theOperators List of names of operators ("FixShape", "SplitClosedFaces", etc.).
        #  @param theParameters List of names of parameters
        #                    ("FixShape.Tolerance3d", "SplitClosedFaces.NbSplitPoints", etc.).
        #  @param theValues List of values of parameters, in the same order
        #                    as parameters are listed in <VAR>theParameters</VAR> list.
        #  @return New GEOM_Object, containing processed shape.
        #
        #  @ref tui_shape_processing "Example"
        def ProcessShape(self, theShape, theOperators, theParameters, theValues):
            # Example: see GEOM_TestHealing.py
            theValues,Parameters = ParseList(theValues)
            anObj = self.HealOp.ProcessShape(theShape, theOperators, theParameters, theValues)
            # To avoid script failure in case of good argument shape
            if self.HealOp.GetErrorCode() == "ShHealOper_NotError_msg":
                return theShape
            RaiseIfFailed("ProcessShape", self.HealOp)
            for string in (theOperators + theParameters):
                Parameters = ":" + Parameters
                pass
            anObj.SetParameters(Parameters)
            return anObj

        ## Remove faces from the given object (shape).
        #  @param theObject Shape to be processed.
        #  @param theFaces Indices of faces to be removed, if EMPTY then the method
        #                  removes ALL faces of the given object.
        #  @return New GEOM_Object, containing processed shape.
        #
        #  @ref tui_suppress_faces "Example"
        def SuppressFaces(self,theObject, theFaces):
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.SuppressFaces(theObject, theFaces)
            RaiseIfFailed("SuppressFaces", self.HealOp)
            return anObj

        ## Sewing of some shapes into single shape.
        #
        #  @ref tui_sewing "Example"
        def MakeSewing(self, ListShape, theTolerance):
            # Example: see GEOM_TestHealing.py
            comp = self.MakeCompound(ListShape)
            anObj = self.Sew(comp, theTolerance)
            return anObj

        ## Sewing of the given object.
        #  @param theObject Shape to be processed.
        #  @param theTolerance Required tolerance value.
        #  @return New GEOM_Object, containing processed shape.
        def Sew(self, theObject, theTolerance):
            # Example: see MakeSewing() above
            theTolerance,Parameters = ParseParameters(theTolerance)
            anObj = self.HealOp.Sew(theObject, theTolerance)
            RaiseIfFailed("Sew", self.HealOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Remove internal wires and edges from the given object (face).
        #  @param theObject Shape to be processed.
        #  @param theWires Indices of wires to be removed, if EMPTY then the method
        #                  removes ALL internal wires of the given object.
        #  @return New GEOM_Object, containing processed shape.
        #
        #  @ref tui_suppress_internal_wires "Example"
        def SuppressInternalWires(self,theObject, theWires):
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.RemoveIntWires(theObject, theWires)
            RaiseIfFailed("RemoveIntWires", self.HealOp)
            return anObj

        ## Remove internal closed contours (holes) from the given object.
        #  @param theObject Shape to be processed.
        #  @param theWires Indices of wires to be removed, if EMPTY then the method
        #                  removes ALL internal holes of the given object
        #  @return New GEOM_Object, containing processed shape.
        #
        #  @ref tui_suppress_holes "Example"
        def SuppressHoles(self,theObject, theWires):
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.FillHoles(theObject, theWires)
            RaiseIfFailed("FillHoles", self.HealOp)
            return anObj

        ## Close an open wire.
        #  @param theObject Shape to be processed.
        #  @param theWires Indexes of edge(s) and wire(s) to be closed within <VAR>theObject</VAR>'s shape,
        #                  if -1, then <VAR>theObject</VAR> itself is a wire.
        #  @param isCommonVertex If TRUE : closure by creation of a common vertex,
        #                        If FALS : closure by creation of an edge between ends.
        #  @return New GEOM_Object, containing processed shape.
        #
        #  @ref tui_close_contour "Example"
        def CloseContour(self,theObject, theWires, isCommonVertex):
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.CloseContour(theObject, theWires, isCommonVertex)
            RaiseIfFailed("CloseContour", self.HealOp)
            return anObj

        ## Addition of a point to a given edge object.
        #  @param theObject Shape to be processed.
        #  @param theEdgeIndex Index of edge to be divided within theObject's shape,
        #                      if -1, then theObject itself is the edge.
        #  @param theValue Value of parameter on edge or length parameter,
        #                  depending on \a isByParameter.
        #  @param isByParameter If TRUE : \a theValue is treated as a curve parameter [0..1],
        #                       if FALSE : \a theValue is treated as a length parameter [0..1]
        #  @return New GEOM_Object, containing processed shape.
        #
        #  @ref tui_add_point_on_edge "Example"
        def DivideEdge(self,theObject, theEdgeIndex, theValue, isByParameter):
            # Example: see GEOM_TestHealing.py
            theEdgeIndex,theValue,isByParameter,Parameters = ParseParameters(theEdgeIndex,theValue,isByParameter)
            anObj = self.HealOp.DivideEdge(theObject, theEdgeIndex, theValue, isByParameter)
            RaiseIfFailed("DivideEdge", self.HealOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Change orientation of the given object. Updates given shape.
        #  @param theObject Shape to be processed.
        #
        #  @ref swig_todo "Example"
        def ChangeOrientationShell(self,theObject):
            theObject = self.HealOp.ChangeOrientation(theObject)
            RaiseIfFailed("ChangeOrientation", self.HealOp)
            pass

        ## Change orientation of the given object.
        #  @param theObject Shape to be processed.
        #  @return New GEOM_Object, containing processed shape.
        #
        #  @ref swig_todo "Example"
        def ChangeOrientationShellCopy(self, theObject):
            anObj = self.HealOp.ChangeOrientationCopy(theObject)
            RaiseIfFailed("ChangeOrientationCopy", self.HealOp)
            return anObj

        ## Try to limit tolerance of the given object by value \a theTolerance.
        #  @param theObject Shape to be processed.
        #  @param theTolerance Required tolerance value.
        #  @return New GEOM_Object, containing processed shape.
        #
        #  @ref tui_limit_tolerance "Example"
        def LimitTolerance(self, theObject, theTolerance = 1e-07):
            anObj = self.HealOp.LimitTolerance(theObject, theTolerance)
            RaiseIfFailed("LimitTolerance", self.HealOp)
            return anObj

        ## Get a list of wires (wrapped in GEOM_Object-s),
        #  that constitute a free boundary of the given shape.
        #  @param theObject Shape to get free boundary of.
        #  @return [status, theClosedWires, theOpenWires]
        #  status: FALSE, if an error(s) occured during the method execution.
        #  theClosedWires: Closed wires on the free boundary of the given shape.
        #  theOpenWires: Open wires on the free boundary of the given shape.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetFreeBoundary(self, theObject):
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.GetFreeBoundary(theObject)
            RaiseIfFailed("GetFreeBoundary", self.HealOp)
            return anObj

        ## Replace coincident faces in theShape by one face.
        #  @param theShape Initial shape.
        #  @param theTolerance Maximum distance between faces, which can be considered as coincident.
        #  @param doKeepNonSolids If FALSE, only solids will present in the result,
        #                         otherwise all initial shapes.
        #  @return New GEOM_Object, containing a copy of theShape without coincident faces.
        #
        #  @ref tui_glue_faces "Example"
        def MakeGlueFaces(self, theShape, theTolerance, doKeepNonSolids=True):
            # Example: see GEOM_Spanner.py
            theTolerance,Parameters = ParseParameters(theTolerance)
            anObj = self.ShapesOp.MakeGlueFaces(theShape, theTolerance, doKeepNonSolids)
            if anObj is None:
                raise RuntimeError, "MakeGlueFaces : " + self.ShapesOp.GetErrorCode()
            anObj.SetParameters(Parameters)
            return anObj

        ## Find coincident faces in theShape for possible gluing.
        #  @param theShape Initial shape.
        #  @param theTolerance Maximum distance between faces,
        #                      which can be considered as coincident.
        #  @return ListOfGO.
        #
        #  @ref swig_todo "Example"
        def GetGlueFaces(self, theShape, theTolerance):
            # Example: see GEOM_Spanner.py
            anObj = self.ShapesOp.GetGlueFaces(theShape, theTolerance)
            RaiseIfFailed("GetGlueFaces", self.ShapesOp)
            return anObj

        ## Replace coincident faces in theShape by one face
        #  in compliance with given list of faces
        #  @param theShape Initial shape.
        #  @param theTolerance Maximum distance between faces,
        #                      which can be considered as coincident.
        #  @param theFaces List of faces for gluing.
        #  @param doKeepNonSolids If FALSE, only solids will present in the result,
        #                         otherwise all initial shapes.
        #  @return New GEOM_Object, containing a copy of theShape
        #          without some faces.
        #
        #  @ref swig_todo "Example"
        def MakeGlueFacesByList(self, theShape, theTolerance, theFaces, doKeepNonSolids=True):
            # Example: see GEOM_Spanner.py
            anObj = self.ShapesOp.MakeGlueFacesByList(theShape, theTolerance, theFaces, doKeepNonSolids)
            if anObj is None:
                raise RuntimeError, "MakeGlueFacesByList : " + self.ShapesOp.GetErrorCode()
            return anObj

        # end of l3_healing
        ## @}

        ## @addtogroup l3_boolean Boolean Operations
        ## @{

        # -----------------------------------------------------------------------------
        # Boolean (Common, Cut, Fuse, Section)
        # -----------------------------------------------------------------------------

        ## Perform one of boolean operations on two given shapes.
        #  @param theShape1 First argument for boolean operation.
        #  @param theShape2 Second argument for boolean operation.
        #  @param theOperation Indicates the operation to be done:
        #                      1 - Common, 2 - Cut, 3 - Fuse, 4 - Section.
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref tui_fuse "Example"
        def MakeBoolean(self,theShape1, theShape2, theOperation):
            # Example: see GEOM_TestAll.py
            anObj = self.BoolOp.MakeBoolean(theShape1, theShape2, theOperation)
            RaiseIfFailed("MakeBoolean", self.BoolOp)
            return anObj

        ## Shortcut to MakeBoolean(s1, s2, 1)
        #
        #  @ref tui_common "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        def MakeCommon(self, s1, s2):
            # Example: see GEOM_TestOthers.py
            return self.MakeBoolean(s1, s2, 1)

        ## Shortcut to MakeBoolean(s1, s2, 2)
        #
        #  @ref tui_cut "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        def MakeCut(self, s1, s2):
            # Example: see GEOM_TestOthers.py
            return self.MakeBoolean(s1, s2, 2)

        ## Shortcut to MakeBoolean(s1, s2, 3)
        #
        #  @ref tui_fuse "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        def MakeFuse(self, s1, s2):
            # Example: see GEOM_TestOthers.py
            return self.MakeBoolean(s1, s2, 3)

        ## Shortcut to MakeBoolean(s1, s2, 4)
        #
        #  @ref tui_section "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        def MakeSection(self, s1, s2):
            # Example: see GEOM_TestOthers.py
            return self.MakeBoolean(s1, s2, 4)

        # end of l3_boolean
        ## @}

        ## @addtogroup l3_basic_op
        ## @{

        ## Perform partition operation.
        #  @param ListShapes Shapes to be intersected.
        #  @param ListTools Shapes to intersect theShapes.
        #  !!!NOTE: Each compound from ListShapes and ListTools will be exploded
        #           in order to avoid possible intersection between shapes from
        #           this compound.
        #  @param Limit Type of resulting shapes (corresponding to TopAbs_ShapeEnum).
        #  @param KeepNonlimitShapes: if this parameter == 0, then only shapes of
        #                             target type (equal to Limit) are kept in the result,
        #                             else standalone shapes of lower dimension
        #                             are kept also (if they exist).
        #
        #  After implementation new version of PartitionAlgo (October 2006)
        #  other parameters are ignored by current functionality. They are kept
        #  in this function only for support old versions.
        #  Ignored parameters:
        #      @param ListKeepInside Shapes, outside which the results will be deleted.
        #         Each shape from theKeepInside must belong to theShapes also.
        #      @param ListRemoveInside Shapes, inside which the results will be deleted.
        #         Each shape from theRemoveInside must belong to theShapes also.
        #      @param RemoveWebs If TRUE, perform Glue 3D algorithm.
        #      @param ListMaterials Material indices for each shape. Make sence,
        #         only if theRemoveWebs is TRUE.
        #
        #  @return New GEOM_Object, containing the result shapes.
        #
        #  @ref tui_partition "Example"
        def MakePartition(self, ListShapes, ListTools=[], ListKeepInside=[], ListRemoveInside=[],
                          Limit=ShapeType["SHAPE"], RemoveWebs=0, ListMaterials=[],
                          KeepNonlimitShapes=0):
            # Example: see GEOM_TestAll.py
            anObj = self.BoolOp.MakePartition(ListShapes, ListTools,
                                              ListKeepInside, ListRemoveInside,
                                              Limit, RemoveWebs, ListMaterials,
                                              KeepNonlimitShapes);
            RaiseIfFailed("MakePartition", self.BoolOp)
            return anObj

        ## Perform partition operation.
        #  This method may be useful if it is needed to make a partition for
        #  compound contains nonintersected shapes. Performance will be better
        #  since intersection between shapes from compound is not performed.
        #
        #  Description of all parameters as in previous method MakePartition()
        #
        #  !!!NOTE: Passed compounds (via ListShapes or via ListTools)
        #           have to consist of nonintersecting shapes.
        #
        #  @return New GEOM_Object, containing the result shapes.
        #
        #  @ref swig_todo "Example"
        def MakePartitionNonSelfIntersectedShape(self, ListShapes, ListTools=[],
                                                 ListKeepInside=[], ListRemoveInside=[],
                                                 Limit=ShapeType["SHAPE"], RemoveWebs=0,
                                                 ListMaterials=[], KeepNonlimitShapes=0):
            anObj = self.BoolOp.MakePartitionNonSelfIntersectedShape(ListShapes, ListTools,
                                                                     ListKeepInside, ListRemoveInside,
                                                                     Limit, RemoveWebs, ListMaterials,
                                                                     KeepNonlimitShapes);
            RaiseIfFailed("MakePartitionNonSelfIntersectedShape", self.BoolOp)
            return anObj

        ## Shortcut to MakePartition()
        #
        #  @ref tui_partition "Example 1"
        #  \n @ref swig_Partition "Example 2"
        def Partition(self, ListShapes, ListTools=[], ListKeepInside=[], ListRemoveInside=[],
                      Limit=ShapeType["SHAPE"], RemoveWebs=0, ListMaterials=[],
                      KeepNonlimitShapes=0):
            # Example: see GEOM_TestOthers.py
            anObj = self.MakePartition(ListShapes, ListTools,
                                       ListKeepInside, ListRemoveInside,
                                       Limit, RemoveWebs, ListMaterials,
                                       KeepNonlimitShapes);
            return anObj

        ## Perform partition of the Shape with the Plane
        #  @param theShape Shape to be intersected.
        #  @param thePlane Tool shape, to intersect theShape.
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref tui_partition "Example"
        def MakeHalfPartition(self,theShape, thePlane):
            # Example: see GEOM_TestAll.py
            anObj = self.BoolOp.MakeHalfPartition(theShape, thePlane)
            RaiseIfFailed("MakeHalfPartition", self.BoolOp)
            return anObj

        # end of l3_basic_op
        ## @}

        ## @addtogroup l3_transform
        ## @{

        ## Translate the given object along the vector, specified
        #  by its end points, creating its copy before the translation.
        #  @param theObject The object to be translated.
        #  @param thePoint1 Start point of translation vector.
        #  @param thePoint2 End point of translation vector.
        #  @return New GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example 1"
        #  \n @ref swig_MakeTranslationTwoPoints "Example 2"
        def MakeTranslationTwoPoints(self,theObject, thePoint1, thePoint2):
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.TranslateTwoPointsCopy(theObject, thePoint1, thePoint2)
            RaiseIfFailed("TranslateTwoPointsCopy", self.TrsfOp)
            return anObj

        ## Translate the given object along the vector, specified by its components.
        #  @param theObject The object to be translated.
        #  @param theDX,theDY,theDZ Components of translation vector.
        #  @return Translated GEOM_Object.
        #
        #  @ref tui_translation "Example"
        def TranslateDXDYDZ(self,theObject, theDX, theDY, theDZ):
            # Example: see GEOM_TestAll.py
            theDX, theDY, theDZ, Parameters = ParseParameters(theDX, theDY, theDZ)
            anObj = self.TrsfOp.TranslateDXDYDZ(theObject, theDX, theDY, theDZ)
            anObj.SetParameters(Parameters)
            RaiseIfFailed("TranslateDXDYDZ", self.TrsfOp)
            return anObj

        ## Translate the given object along the vector, specified
        #  by its components, creating its copy before the translation.
        #  @param theObject The object to be translated.
        #  @param theDX,theDY,theDZ Components of translation vector.
        #  @return New GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example"
        def MakeTranslation(self,theObject, theDX, theDY, theDZ):
            # Example: see GEOM_TestAll.py
            theDX, theDY, theDZ, Parameters = ParseParameters(theDX, theDY, theDZ)
            anObj = self.TrsfOp.TranslateDXDYDZCopy(theObject, theDX, theDY, theDZ)
            anObj.SetParameters(Parameters)
            RaiseIfFailed("TranslateDXDYDZ", self.TrsfOp)
            return anObj

        ## Translate the given object along the given vector,
        #  creating its copy before the translation.
        #  @param theObject The object to be translated.
        #  @param theVector The translation vector.
        #  @return New GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example"
        def MakeTranslationVector(self,theObject, theVector):
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.TranslateVectorCopy(theObject, theVector)
            RaiseIfFailed("TranslateVectorCopy", self.TrsfOp)
            return anObj

        ## Translate the given object along the given vector on given distance.
        #  @param theObject The object to be translated.
        #  @param theVector The translation vector.
        #  @param theDistance The translation distance.
        #  @param theCopy Flag used to translate object itself or create a copy.
        #  @return Translated GEOM_Object.
        #
        #  @ref tui_translation "Example"
        def TranslateVectorDistance(self, theObject, theVector, theDistance, theCopy):
            # Example: see GEOM_TestAll.py
            theDistance,Parameters = ParseParameters(theDistance)
            anObj = self.TrsfOp.TranslateVectorDistance(theObject, theVector, theDistance, theCopy)
            RaiseIfFailed("TranslateVectorDistance", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Translate the given object along the given vector on given distance,
        #  creating its copy before the translation.
        #  @param theObject The object to be translated.
        #  @param theVector The translation vector.
        #  @param theDistance The translation distance.
        #  @return New GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example"
        def MakeTranslationVectorDistance(self, theObject, theVector, theDistance):
            # Example: see GEOM_TestAll.py
            theDistance,Parameters = ParseParameters(theDistance)
            anObj = self.TrsfOp.TranslateVectorDistance(theObject, theVector, theDistance, 1)
            RaiseIfFailed("TranslateVectorDistance", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Rotate the given object around the given axis on the given angle.
        #  @param theObject The object to be rotated.
        #  @param theAxis Rotation axis.
        #  @param theAngle Rotation angle in radians.
        #  @return Rotated GEOM_Object.
        #
        #  @ref tui_rotation "Example"
        def Rotate(self,theObject, theAxis, theAngle):
            # Example: see GEOM_TestAll.py
            flag = False
            if isinstance(theAngle,str):
                flag = True
            theAngle, Parameters = ParseParameters(theAngle)
            if flag:
                theAngle = theAngle*math.pi/180.0
            anObj = self.TrsfOp.Rotate(theObject, theAxis, theAngle)
            RaiseIfFailed("RotateCopy", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Rotate the given object around the given axis
        #  on the given angle, creating its copy before the rotatation.
        #  @param theObject The object to be rotated.
        #  @param theAxis Rotation axis.
        #  @param theAngle Rotation angle in radians.
        #  @return New GEOM_Object, containing the rotated object.
        #
        #  @ref tui_rotation "Example"
        def MakeRotation(self,theObject, theAxis, theAngle):
            # Example: see GEOM_TestAll.py
            flag = False
            if isinstance(theAngle,str):
                flag = True
            theAngle, Parameters = ParseParameters(theAngle)
            if flag:
                theAngle = theAngle*math.pi/180.0
            anObj = self.TrsfOp.RotateCopy(theObject, theAxis, theAngle)
            RaiseIfFailed("RotateCopy", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Rotate given object around vector perpendicular to plane
        #  containing three points, creating its copy before the rotatation.
        #  @param theObject The object to be rotated.
        #  @param theCentPoint central point - the axis is the vector perpendicular to the plane
        #  containing the three points.
        #  @param thePoint1,thePoint2 - in a perpendicular plane of the axis.
        #  @return New GEOM_Object, containing the rotated object.
        #
        #  @ref tui_rotation "Example"
        def MakeRotationThreePoints(self,theObject, theCentPoint, thePoint1, thePoint2):
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.RotateThreePointsCopy(theObject, theCentPoint, thePoint1, thePoint2)
            RaiseIfFailed("RotateThreePointsCopy", self.TrsfOp)
            return anObj

        ## Scale the given object by the factor, creating its copy before the scaling.
        #  @param theObject The object to be scaled.
        #  @param thePoint Center point for scaling.
        #                  Passing None for it means scaling relatively the origin of global CS.
        #  @param theFactor Scaling factor value.
        #  @return New GEOM_Object, containing the scaled shape.
        #
        #  @ref tui_scale "Example"
        def MakeScaleTransform(self, theObject, thePoint, theFactor):
            # Example: see GEOM_TestAll.py
            theFactor, Parameters = ParseParameters(theFactor)
            anObj = self.TrsfOp.ScaleShapeCopy(theObject, thePoint, theFactor)
            RaiseIfFailed("ScaleShapeCopy", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Scale the given object by different factors along coordinate axes,
        #  creating its copy before the scaling.
        #  @param theObject The object to be scaled.
        #  @param thePoint Center point for scaling.
        #                  Passing None for it means scaling relatively the origin of global CS.
        #  @param theFactorX,theFactorY,theFactorZ Scaling factors along each axis.
        #  @return New GEOM_Object, containing the scaled shape.
        #
        #  @ref swig_scale "Example"
        def MakeScaleAlongAxes(self, theObject, thePoint, theFactorX, theFactorY, theFactorZ):
            # Example: see GEOM_TestAll.py
            theFactorX, theFactorY, theFactorZ, Parameters = ParseParameters(theFactorX, theFactorY, theFactorZ)
            anObj = self.TrsfOp.ScaleShapeAlongAxesCopy(theObject, thePoint,
                                                        theFactorX, theFactorY, theFactorZ)
            RaiseIfFailed("MakeScaleAlongAxes", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create an object, symmetrical
        #  to the given one relatively the given plane.
        #  @param theObject The object to be mirrored.
        #  @param thePlane Plane of symmetry.
        #  @return New GEOM_Object, containing the mirrored shape.
        #
        #  @ref tui_mirror "Example"
        def MakeMirrorByPlane(self,theObject, thePlane):
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.MirrorPlaneCopy(theObject, thePlane)
            RaiseIfFailed("MirrorPlaneCopy", self.TrsfOp)
            return anObj

        ## Create an object, symmetrical
        #  to the given one relatively the given axis.
        #  @param theObject The object to be mirrored.
        #  @param theAxis Axis of symmetry.
        #  @return New GEOM_Object, containing the mirrored shape.
        #
        #  @ref tui_mirror "Example"
        def MakeMirrorByAxis(self,theObject, theAxis):
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.MirrorAxisCopy(theObject, theAxis)
            RaiseIfFailed("MirrorAxisCopy", self.TrsfOp)
            return anObj

        ## Create an object, symmetrical
        #  to the given one relatively the given point.
        #  @param theObject The object to be mirrored.
        #  @param thePoint Point of symmetry.
        #  @return New GEOM_Object, containing the mirrored shape.
        #
        #  @ref tui_mirror "Example"
        def MakeMirrorByPoint(self,theObject, thePoint):
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.MirrorPointCopy(theObject, thePoint)
            RaiseIfFailed("MirrorPointCopy", self.TrsfOp)
            return anObj

        ## Modify the Location of the given object by LCS,
        #  creating its copy before the setting.
        #  @param theObject The object to be displaced.
        #  @param theStartLCS Coordinate system to perform displacement from it.
        #                     If \a theStartLCS is NULL, displacement
        #                     will be performed from global CS.
        #                     If \a theObject itself is used as \a theStartLCS,
        #                     its location will be changed to \a theEndLCS.
        #  @param theEndLCS Coordinate system to perform displacement to it.
        #  @return New GEOM_Object, containing the displaced shape.
        #
        #  @ref tui_modify_location "Example"
        def MakePosition(self,theObject, theStartLCS, theEndLCS):
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.PositionShapeCopy(theObject, theStartLCS, theEndLCS)
            RaiseIfFailed("PositionShapeCopy", self.TrsfOp)
            return anObj

        ## Modify the Location of the given object by Path,
        #  @param  theObject The object to be displaced.
        #  @param  thePath Wire or Edge along that the object will be translated.
        #  @param  theDistance progress of Path (0 = start location, 1 = end of path location).
        #  @param  theCopy is to create a copy objects if true.
        #  @param  theReverse - 0 for usual direction, 1 to reverse path direction.
        #  @return New GEOM_Object, containing the displaced shape.
        #
        #  @ref tui_modify_location "Example"
        def PositionAlongPath(self,theObject, thePath, theDistance, theCopy, theReverse):
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.PositionAlongPath(theObject, thePath, theDistance, theCopy, theReverse)
            RaiseIfFailed("PositionAlongPath", self.TrsfOp)
            return anObj

        ## Create new object as offset of the given one.
        #  @param theObject The base object for the offset.
        #  @param theOffset Offset value.
        #  @return New GEOM_Object, containing the offset object.
        #
        #  @ref tui_offset "Example"
        def MakeOffset(self,theObject, theOffset):
            # Example: see GEOM_TestAll.py
            theOffset, Parameters = ParseParameters(theOffset)
            anObj = self.TrsfOp.OffsetShapeCopy(theObject, theOffset)
            RaiseIfFailed("OffsetShapeCopy", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        # -----------------------------------------------------------------------------
        # Patterns
        # -----------------------------------------------------------------------------

        ## Translate the given object along the given vector a given number times
        #  @param theObject The object to be translated.
        #  @param theVector Direction of the translation.
        #  @param theStep Distance to translate on.
        #  @param theNbTimes Quantity of translations to be done.
        #  @return New GEOM_Object, containing compound of all
        #          the shapes, obtained after each translation.
        #
        #  @ref tui_multi_translation "Example"
        def MakeMultiTranslation1D(self,theObject, theVector, theStep, theNbTimes):
            # Example: see GEOM_TestAll.py
            theStep, theNbTimes, Parameters = ParseParameters(theStep, theNbTimes)
            anObj = self.TrsfOp.MultiTranslate1D(theObject, theVector, theStep, theNbTimes)
            RaiseIfFailed("MultiTranslate1D", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Conseqently apply two specified translations to theObject specified number of times.
        #  @param theObject The object to be translated.
        #  @param theVector1 Direction of the first translation.
        #  @param theStep1 Step of the first translation.
        #  @param theNbTimes1 Quantity of translations to be done along theVector1.
        #  @param theVector2 Direction of the second translation.
        #  @param theStep2 Step of the second translation.
        #  @param theNbTimes2 Quantity of translations to be done along theVector2.
        #  @return New GEOM_Object, containing compound of all
        #          the shapes, obtained after each translation.
        #
        #  @ref tui_multi_translation "Example"
        def MakeMultiTranslation2D(self,theObject, theVector1, theStep1, theNbTimes1,
                                   theVector2, theStep2, theNbTimes2):
            # Example: see GEOM_TestAll.py
            theStep1,theNbTimes1,theStep2,theNbTimes2, Parameters = ParseParameters(theStep1,theNbTimes1,theStep2,theNbTimes2)
            anObj = self.TrsfOp.MultiTranslate2D(theObject, theVector1, theStep1, theNbTimes1,
                                                 theVector2, theStep2, theNbTimes2)
            RaiseIfFailed("MultiTranslate2D", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Rotate the given object around the given axis a given number times.
        #  Rotation angle will be 2*PI/theNbTimes.
        #  @param theObject The object to be rotated.
        #  @param theAxis The rotation axis.
        #  @param theNbTimes Quantity of rotations to be done.
        #  @return New GEOM_Object, containing compound of all the
        #          shapes, obtained after each rotation.
        #
        #  @ref tui_multi_rotation "Example"
        def MultiRotate1D(self,theObject, theAxis, theNbTimes):
            # Example: see GEOM_TestAll.py
            theAxis, theNbTimes, Parameters = ParseParameters(theAxis, theNbTimes)
            anObj = self.TrsfOp.MultiRotate1D(theObject, theAxis, theNbTimes)
            RaiseIfFailed("MultiRotate1D", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Rotate the given object around the
        #  given axis on the given angle a given number
        #  times and multi-translate each rotation result.
        #  Translation direction passes through center of gravity
        #  of rotated shape and its projection on the rotation axis.
        #  @param theObject The object to be rotated.
        #  @param theAxis Rotation axis.
        #  @param theAngle Rotation angle in graduces.
        #  @param theNbTimes1 Quantity of rotations to be done.
        #  @param theStep Translation distance.
        #  @param theNbTimes2 Quantity of translations to be done.
        #  @return New GEOM_Object, containing compound of all the
        #          shapes, obtained after each transformation.
        #
        #  @ref tui_multi_rotation "Example"
        def MultiRotate2D(self,theObject, theAxis, theAngle, theNbTimes1, theStep, theNbTimes2):
            # Example: see GEOM_TestAll.py
            theAngle, theNbTimes1, theStep, theNbTimes2, Parameters = ParseParameters(theAngle, theNbTimes1, theStep, theNbTimes2)
            anObj = self.TrsfOp.MultiRotate2D(theObject, theAxis, theAngle, theNbTimes1, theStep, theNbTimes2)
            RaiseIfFailed("MultiRotate2D", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## The same, as MultiRotate1D(), but axis is given by direction and point
        #  @ref swig_MakeMultiRotation "Example"
        def MakeMultiRotation1D(self,aShape,aDir,aPoint,aNbTimes):
            # Example: see GEOM_TestOthers.py
            aVec = self.MakeLine(aPoint,aDir)
            anObj = self.MultiRotate1D(aShape,aVec,aNbTimes)
            return anObj

        ## The same, as MultiRotate2D(), but axis is given by direction and point
        #  @ref swig_MakeMultiRotation "Example"
        def MakeMultiRotation2D(self,aShape,aDir,aPoint,anAngle,nbtimes1,aStep,nbtimes2):
            # Example: see GEOM_TestOthers.py
            aVec = self.MakeLine(aPoint,aDir)
            anObj = self.MultiRotate2D(aShape,aVec,anAngle,nbtimes1,aStep,nbtimes2)
            return anObj

        # end of l3_transform
        ## @}

        ## @addtogroup l3_local
        ## @{

        ## Perform a fillet on all edges of the given shape.
        #  @param theShape Shape, to perform fillet on.
        #  @param theR Fillet radius.
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet "Example 1"
        #  \n @ref swig_MakeFilletAll "Example 2"
        def MakeFilletAll(self,theShape, theR):
            # Example: see GEOM_TestOthers.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.LocalOp.MakeFilletAll(theShape, theR)
            RaiseIfFailed("MakeFilletAll", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Perform a fillet on the specified edges/faces of the given shape
        #  @param theShape Shape, to perform fillet on.
        #  @param theR Fillet radius.
        #  @param theShapeType Type of shapes in <VAR>theListShapes</VAR>.
        #  @param theListShapes Global indices of edges/faces to perform fillet on.
        #    \note Global index of sub-shape can be obtained, using method geompy.GetSubShapeID().
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet "Example"
        def MakeFillet(self,theShape, theR, theShapeType, theListShapes):
            # Example: see GEOM_TestAll.py
            theR,Parameters = ParseParameters(theR)
            anObj = None
            if theShapeType == ShapeType["EDGE"]:
                anObj = self.LocalOp.MakeFilletEdges(theShape, theR, theListShapes)
                RaiseIfFailed("MakeFilletEdges", self.LocalOp)
            else:
                anObj = self.LocalOp.MakeFilletFaces(theShape, theR, theListShapes)
                RaiseIfFailed("MakeFilletFaces", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## The same that MakeFillet but with two Fillet Radius R1 and R2
        def MakeFilletR1R2(self, theShape, theR1, theR2, theShapeType, theListShapes):
            theR1,theR2,Parameters = ParseParameters(theR1,theR2)
            anObj = None
            if theShapeType == ShapeType["EDGE"]:
                anObj = self.LocalOp.MakeFilletEdgesR1R2(theShape, theR1, theR2, theListShapes)
                RaiseIfFailed("MakeFilletEdgesR1R2", self.LocalOp)
            else:
                anObj = self.LocalOp.MakeFilletFacesR1R2(theShape, theR1, theR2, theListShapes)
                RaiseIfFailed("MakeFilletFacesR1R2", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Perform a fillet on the specified edges of the given shape
        #  @param theShape - Wire Shape to perform fillet on.
        #  @param theR - Fillet radius.
        #  @param theListOfVertexes Global indices of vertexes to perform fillet on.
        #    \note Global index of sub-shape can be obtained, using method geompy.GetSubShapeID().
        #    \note The list of vertices could be empty,
        #          in this case fillet will done done at all vertices in wire
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet2d "Example"
        def MakeFillet1D(self,theShape, theR, theListOfVertexes):
            # Example: see GEOM_TestAll.py
            anObj = self.LocalOp.MakeFillet1D(theShape, theR, theListOfVertexes)
            RaiseIfFailed("MakeFillet1D", self.LocalOp)
            return anObj

        ## Perform a fillet on the specified edges/faces of the given shape
        #  @param theShape - Face Shape to perform fillet on.
        #  @param theR - Fillet radius.
        #  @param theListOfVertexes Global indices of vertexes to perform fillet on.
        #    \note Global index of sub-shape can be obtained, using method geompy.GetSubShapeID().
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet2d "Example"
        def MakeFillet2D(self,theShape, theR, theListOfVertexes):
            # Example: see GEOM_TestAll.py
            anObj = self.LocalOp.MakeFillet2D(theShape, theR, theListOfVertexes)
            RaiseIfFailed("MakeFillet2D", self.LocalOp)
            return anObj

        ## Perform a symmetric chamfer on all edges of the given shape.
        #  @param theShape Shape, to perform chamfer on.
        #  @param theD Chamfer size along each face.
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref tui_chamfer "Example 1"
        #  \n @ref swig_MakeChamferAll "Example 2"
        def MakeChamferAll(self,theShape, theD):
            # Example: see GEOM_TestOthers.py
            theD,Parameters = ParseParameters(theD)
            anObj = self.LocalOp.MakeChamferAll(theShape, theD)
            RaiseIfFailed("MakeChamferAll", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Perform a chamfer on edges, common to the specified faces,
        #  with distance D1 on the Face1
        #  @param theShape Shape, to perform chamfer on.
        #  @param theD1 Chamfer size along \a theFace1.
        #  @param theD2 Chamfer size along \a theFace2.
        #  @param theFace1,theFace2 Global indices of two faces of \a theShape.
        #    \note Global index of sub-shape can be obtained, using method geompy.GetSubShapeID().
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref tui_chamfer "Example"
        def MakeChamferEdge(self,theShape, theD1, theD2, theFace1, theFace2):
            # Example: see GEOM_TestAll.py
            theD1,theD2,Parameters = ParseParameters(theD1,theD2)
            anObj = self.LocalOp.MakeChamferEdge(theShape, theD1, theD2, theFace1, theFace2)
            RaiseIfFailed("MakeChamferEdge", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## The Same that MakeChamferEdge but with params theD is chamfer length and
        #  theAngle is Angle of chamfer (angle in radians or a name of variable which defines angle in degrees)
        def MakeChamferEdgeAD(self, theShape, theD, theAngle, theFace1, theFace2):
            flag = False
            if isinstance(theAngle,str):
                flag = True
            theD,theAngle,Parameters = ParseParameters(theD,theAngle)
            if flag:
                theAngle = theAngle*math.pi/180.0
            anObj = self.LocalOp.MakeChamferEdgeAD(theShape, theD, theAngle, theFace1, theFace2)
            RaiseIfFailed("MakeChamferEdgeAD", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Perform a chamfer on all edges of the specified faces,
        #  with distance D1 on the first specified face (if several for one edge)
        #  @param theShape Shape, to perform chamfer on.
        #  @param theD1 Chamfer size along face from \a theFaces. If both faces,
        #               connected to the edge, are in \a theFaces, \a theD1
        #               will be get along face, which is nearer to \a theFaces beginning.
        #  @param theD2 Chamfer size along another of two faces, connected to the edge.
        #  @param theFaces Sequence of global indices of faces of \a theShape.
        #    \note Global index of sub-shape can be obtained, using method geompy.GetSubShapeID().
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref tui_chamfer "Example"
        def MakeChamferFaces(self,theShape, theD1, theD2, theFaces):
            # Example: see GEOM_TestAll.py
            theD1,theD2,Parameters = ParseParameters(theD1,theD2)
            anObj = self.LocalOp.MakeChamferFaces(theShape, theD1, theD2, theFaces)
            RaiseIfFailed("MakeChamferFaces", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## The Same that MakeChamferFaces but with params theD is chamfer lenght and
        #  theAngle is Angle of chamfer (angle in radians or a name of variable which defines angle in degrees)
        #
        #  @ref swig_FilletChamfer "Example"
        def MakeChamferFacesAD(self, theShape, theD, theAngle, theFaces):
            flag = False
            if isinstance(theAngle,str):
                flag = True
            theD,theAngle,Parameters = ParseParameters(theD,theAngle)
            if flag:
                theAngle = theAngle*math.pi/180.0
            anObj = self.LocalOp.MakeChamferFacesAD(theShape, theD, theAngle, theFaces)
            RaiseIfFailed("MakeChamferFacesAD", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Perform a chamfer on edges,
        #  with distance D1 on the first specified face (if several for one edge)
        #  @param theShape Shape, to perform chamfer on.
        #  @param theD1,theD2 Chamfer size
        #  @param theEdges Sequence of edges of \a theShape.
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref swig_FilletChamfer "Example"
        def MakeChamferEdges(self, theShape, theD1, theD2, theEdges):
            theD1,theD2,Parameters = ParseParameters(theD1,theD2)
            anObj = self.LocalOp.MakeChamferEdges(theShape, theD1, theD2, theEdges)
            RaiseIfFailed("MakeChamferEdges", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## The Same that MakeChamferEdges but with params theD is chamfer lenght and
        #  theAngle is Angle of chamfer (angle in radians or a name of variable which defines angle in degrees)
        def MakeChamferEdgesAD(self, theShape, theD, theAngle, theEdges):
            flag = False
            if isinstance(theAngle,str):
                flag = True
            theD,theAngle,Parameters = ParseParameters(theD,theAngle)
            if flag:
                theAngle = theAngle*math.pi/180.0
            anObj = self.LocalOp.MakeChamferEdgesAD(theShape, theD, theAngle, theEdges)
            RaiseIfFailed("MakeChamferEdgesAD", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Shortcut to MakeChamferEdge() and MakeChamferFaces()
        #
        #  @ref swig_MakeChamfer "Example"
        def MakeChamfer(self,aShape,d1,d2,aShapeType,ListShape):
            # Example: see GEOM_TestOthers.py
            anObj = None
            if aShapeType == ShapeType["EDGE"]:
                anObj = self.MakeChamferEdge(aShape,d1,d2,ListShape[0],ListShape[1])
            else:
                anObj = self.MakeChamferFaces(aShape,d1,d2,ListShape)
            return anObj

        # end of l3_local
        ## @}

        ## @addtogroup l3_basic_op
        ## @{

        ## Perform an Archimde operation on the given shape with given parameters.
        #  The object presenting the resulting face is returned.
        #  @param theShape Shape to be put in water.
        #  @param theWeight Weight og the shape.
        #  @param theWaterDensity Density of the water.
        #  @param theMeshDeflection Deflection of the mesh, using to compute the section.
        #  @return New GEOM_Object, containing a section of \a theShape
        #          by a plane, corresponding to water level.
        #
        #  @ref tui_archimede "Example"
        def Archimede(self,theShape, theWeight, theWaterDensity, theMeshDeflection):
            # Example: see GEOM_TestAll.py
            theWeight,theWaterDensity,theMeshDeflection,Parameters = ParseParameters(
              theWeight,theWaterDensity,theMeshDeflection)
            anObj = self.LocalOp.MakeArchimede(theShape, theWeight, theWaterDensity, theMeshDeflection)
            RaiseIfFailed("MakeArchimede", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        # end of l3_basic_op
        ## @}

        ## @addtogroup l2_measure
        ## @{

        ## Get point coordinates
        #  @return [x, y, z]
        #
        #  @ref tui_measurement_tools_page "Example"
        def PointCoordinates(self,Point):
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.PointCoordinates(Point)
            RaiseIfFailed("PointCoordinates", self.MeasuOp)
            return aTuple

        ## Get summarized length of all wires,
        #  area of surface and volume of the given shape.
        #  @param theShape Shape to define properties of.
        #  @return [theLength, theSurfArea, theVolume]
        #  theLength:   Summarized length of all wires of the given shape.
        #  theSurfArea: Area of surface of the given shape.
        #  theVolume:   Volume of the given shape.
        #
        #  @ref tui_measurement_tools_page "Example"
        def BasicProperties(self,theShape):
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetBasicProperties(theShape)
            RaiseIfFailed("GetBasicProperties", self.MeasuOp)
            return aTuple

        ## Get parameters of bounding box of the given shape
        #  @param theShape Shape to obtain bounding box of.
        #  @return [Xmin,Xmax, Ymin,Ymax, Zmin,Zmax]
        #  Xmin,Xmax: Limits of shape along OX axis.
        #  Ymin,Ymax: Limits of shape along OY axis.
        #  Zmin,Zmax: Limits of shape along OZ axis.
        #
        #  @ref tui_measurement_tools_page "Example"
        def BoundingBox(self,theShape):
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetBoundingBox(theShape)
            RaiseIfFailed("GetBoundingBox", self.MeasuOp)
            return aTuple

        ## Get inertia matrix and moments of inertia of theShape.
        #  @param theShape Shape to calculate inertia of.
        #  @return [I11,I12,I13, I21,I22,I23, I31,I32,I33, Ix,Iy,Iz]
        #  I(1-3)(1-3): Components of the inertia matrix of the given shape.
        #  Ix,Iy,Iz:    Moments of inertia of the given shape.
        #
        #  @ref tui_measurement_tools_page "Example"
        def Inertia(self,theShape):
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetInertia(theShape)
            RaiseIfFailed("GetInertia", self.MeasuOp)
            return aTuple

        ## Get minimal distance between the given shapes.
        #  @param theShape1,theShape2 Shapes to find minimal distance between.
        #  @return Value of the minimal distance between the given shapes.
        #
        #  @ref tui_measurement_tools_page "Example"
        def MinDistance(self, theShape1, theShape2):
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetMinDistance(theShape1, theShape2)
            RaiseIfFailed("GetMinDistance", self.MeasuOp)
            return aTuple[0]

        ## Get minimal distance between the given shapes.
        #  @param theShape1,theShape2 Shapes to find minimal distance between.
        #  @return Value of the minimal distance between the given shapes.
        #
        #  @ref swig_all_measure "Example"
        def MinDistanceComponents(self, theShape1, theShape2):
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetMinDistance(theShape1, theShape2)
            RaiseIfFailed("GetMinDistance", self.MeasuOp)
            aRes = [aTuple[0], aTuple[4] - aTuple[1], aTuple[5] - aTuple[2], aTuple[6] - aTuple[3]]
            return aRes

        ## Get angle between the given shapes in degrees.
        #  @param theShape1,theShape2 Lines or linear edges to find angle between.
        #  @return Value of the angle between the given shapes in degrees.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetAngle(self, theShape1, theShape2):
            # Example: see GEOM_TestMeasures.py
            anAngle = self.MeasuOp.GetAngle(theShape1, theShape2)
            RaiseIfFailed("GetAngle", self.MeasuOp)
            return anAngle
        ## Get angle between the given shapes in radians.
        #  @param theShape1,theShape2 Lines or linear edges to find angle between.
        #  @return Value of the angle between the given shapes in radians.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetAngleRadians(self, theShape1, theShape2):
            # Example: see GEOM_TestMeasures.py
            anAngle = self.MeasuOp.GetAngle(theShape1, theShape2)*math.pi/180.
            RaiseIfFailed("GetAngle", self.MeasuOp)
            return anAngle

        ## @name Curve Curvature Measurement
        #  Methods for receiving radius of curvature of curves
        #  in the given point
        ## @{

        ## Measure curvature of a curve at a point, set by parameter.
        #  @ref swig_todo "Example"
        def CurveCurvatureByParam(self, theCurve, theParam):
            # Example: see GEOM_TestMeasures.py
            aCurv = self.MeasuOp.CurveCurvatureByParam(theCurve,theParam)
            RaiseIfFailed("CurveCurvatureByParam", self.MeasuOp)
            return aCurv

        ## @details
        #  @ref swig_todo "Example"
        def CurveCurvatureByPoint(self, theCurve, thePoint):
            aCurv = self.MeasuOp.CurveCurvatureByPoint(theCurve,thePoint)
            RaiseIfFailed("CurveCurvatureByPoint", self.MeasuOp)
            return aCurv
        ## @}

        ## @name Surface Curvature Measurement
        #  Methods for receiving max and min radius of curvature of surfaces
        #  in the given point
        ## @{

        ## @details
        ## @ref swig_todo "Example"
        def MaxSurfaceCurvatureByParam(self, theSurf, theUParam, theVParam):
            # Example: see GEOM_TestMeasures.py
            aSurf = self.MeasuOp.MaxSurfaceCurvatureByParam(theSurf,theUParam,theVParam)
            RaiseIfFailed("MaxSurfaceCurvatureByParam", self.MeasuOp)
            return aSurf

        ## @details
        ## @ref swig_todo "Example"
        def MaxSurfaceCurvatureByPoint(self, theSurf, thePoint):
            aSurf = self.MeasuOp.MaxSurfaceCurvatureByPoint(theSurf,thePoint)
            RaiseIfFailed("MaxSurfaceCurvatureByPoint", self.MeasuOp)
            return aSurf

        ## @details
        ## @ref swig_todo "Example"
        def MinSurfaceCurvatureByParam(self, theSurf, theUParam, theVParam):
            aSurf = self.MeasuOp.MinSurfaceCurvatureByParam(theSurf,theUParam,theVParam)
            RaiseIfFailed("MinSurfaceCurvatureByParam", self.MeasuOp)
            return aSurf

        ## @details
        ## @ref swig_todo "Example"
        def MinSurfaceCurvatureByPoint(self, theSurf, thePoint):
            aSurf = self.MeasuOp.MinSurfaceCurvatureByPoint(theSurf,thePoint)
            RaiseIfFailed("MinSurfaceCurvatureByPoint", self.MeasuOp)
            return aSurf
        ## @}

        ## Get min and max tolerances of sub-shapes of theShape
        #  @param theShape Shape, to get tolerances of.
        #  @return [FaceMin,FaceMax, EdgeMin,EdgeMax, VertMin,VertMax]
        #  FaceMin,FaceMax: Min and max tolerances of the faces.
        #  EdgeMin,EdgeMax: Min and max tolerances of the edges.
        #  VertMin,VertMax: Min and max tolerances of the vertices.
        #
        #  @ref tui_measurement_tools_page "Example"
        def Tolerance(self,theShape):
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetTolerance(theShape)
            RaiseIfFailed("GetTolerance", self.MeasuOp)
            return aTuple

        ## Obtain description of the given shape (number of sub-shapes of each type)
        #  @param theShape Shape to be described.
        #  @return Description of the given shape.
        #
        #  @ref tui_measurement_tools_page "Example"
        def WhatIs(self,theShape):
            # Example: see GEOM_TestMeasures.py
            aDescr = self.MeasuOp.WhatIs(theShape)
            RaiseIfFailed("WhatIs", self.MeasuOp)
            return aDescr

        ## Get a point, situated at the centre of mass of theShape.
        #  @param theShape Shape to define centre of mass of.
        #  @return New GEOM_Object, containing the created point.
        #
        #  @ref tui_measurement_tools_page "Example"
        def MakeCDG(self,theShape):
            # Example: see GEOM_TestMeasures.py
            anObj = self.MeasuOp.GetCentreOfMass(theShape)
            RaiseIfFailed("GetCentreOfMass", self.MeasuOp)
            return anObj

        ## Get a vertex subshape by index depended with orientation.
        #  @param theShape Shape to find subshape.
        #  @param theIndex Index to find vertex by this index.
        #  @return New GEOM_Object, containing the created vertex.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetVertexByIndex(self,theShape, theIndex):
            # Example: see GEOM_TestMeasures.py
            anObj = self.MeasuOp.GetVertexByIndex(theShape, theIndex)
            RaiseIfFailed("GetVertexByIndex", self.MeasuOp)
            return anObj

        ## Get the first vertex of wire/edge depended orientation.
        #  @param theShape Shape to find first vertex.
        #  @return New GEOM_Object, containing the created vertex.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetFirstVertex(self,theShape):
            # Example: see GEOM_TestMeasures.py
            anObj = self.GetVertexByIndex(theShape, 0)
            RaiseIfFailed("GetFirstVertex", self.MeasuOp)
            return anObj

        ## Get the last vertex of wire/edge depended orientation.
        #  @param theShape Shape to find last vertex.
        #  @return New GEOM_Object, containing the created vertex.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetLastVertex(self,theShape):
            # Example: see GEOM_TestMeasures.py
            nb_vert =  self.ShapesOp.NumberOfSubShapes(theShape, ShapeType["VERTEX"])
            anObj = self.GetVertexByIndex(theShape, (nb_vert-1))
            RaiseIfFailed("GetLastVertex", self.MeasuOp)
            return anObj

        ## Get a normale to the given face. If the point is not given,
        #  the normale is calculated at the center of mass.
        #  @param theFace Face to define normale of.
        #  @param theOptionalPoint Point to compute the normale at.
        #  @return New GEOM_Object, containing the created vector.
        #
        #  @ref swig_todo "Example"
        def GetNormal(self, theFace, theOptionalPoint = None):
            # Example: see GEOM_TestMeasures.py
            anObj = self.MeasuOp.GetNormal(theFace, theOptionalPoint)
            RaiseIfFailed("GetNormal", self.MeasuOp)
            return anObj

        ## Check a topology of the given shape.
        #  @param theShape Shape to check validity of.
        #  @param theIsCheckGeom If FALSE, only the shape's topology will be checked,
        #                        if TRUE, the shape's geometry will be checked also.
        #  @return TRUE, if the shape "seems to be valid".
        #  If theShape is invalid, prints a description of problem.
        #
        #  @ref tui_measurement_tools_page "Example"
        def CheckShape(self,theShape, theIsCheckGeom = 0):
            # Example: see GEOM_TestMeasures.py
            if theIsCheckGeom:
                (IsValid, Status) = self.MeasuOp.CheckShapeWithGeometry(theShape)
                RaiseIfFailed("CheckShapeWithGeometry", self.MeasuOp)
            else:
                (IsValid, Status) = self.MeasuOp.CheckShape(theShape)
                RaiseIfFailed("CheckShape", self.MeasuOp)
            if IsValid == 0:
                print Status
            return IsValid

        ## Get position (LCS) of theShape.
        #
        #  Origin of the LCS is situated at the shape's center of mass.
        #  Axes of the LCS are obtained from shape's location or,
        #  if the shape is a planar face, from position of its plane.
        #
        #  @param theShape Shape to calculate position of.
        #  @return [Ox,Oy,Oz, Zx,Zy,Zz, Xx,Xy,Xz].
        #          Ox,Oy,Oz: Coordinates of shape's LCS origin.
        #          Zx,Zy,Zz: Coordinates of shape's LCS normal(main) direction.
        #          Xx,Xy,Xz: Coordinates of shape's LCS X direction.
        #
        #  @ref swig_todo "Example"
        def GetPosition(self,theShape):
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetPosition(theShape)
            RaiseIfFailed("GetPosition", self.MeasuOp)
            return aTuple

        ## Get kind of theShape.
        #
        #  @param theShape Shape to get a kind of.
        #  @return Returns a kind of shape in terms of <VAR>GEOM_IKindOfShape.shape_kind</VAR> enumeration
        #          and a list of parameters, describing the shape.
        #  @note  Concrete meaning of each value, returned via \a theIntegers
        #         or \a theDoubles list depends on the kind of the shape.
        #         The full list of possible outputs is:
        #
        #  - geompy.kind.COMPOUND              nb_solids  nb_faces  nb_edges  nb_vertices
        #  - geompy.kind.COMPSOLID             nb_solids  nb_faces  nb_edges  nb_vertices
        #
        #  - geompy.kind.SHELL       geompy.info.CLOSED   nb_faces  nb_edges  nb_vertices
        #  - geompy.kind.SHELL       geompy.info.UNCLOSED nb_faces  nb_edges  nb_vertices
        #
        #  - geompy.kind.WIRE        geompy.info.CLOSED             nb_edges  nb_vertices
        #  - geompy.kind.WIRE        geompy.info.UNCLOSED           nb_edges  nb_vertices
        #
        #  - geompy.kind.SPHERE       xc yc zc            R
        #  - geompy.kind.CYLINDER     xb yb zb  dx dy dz  R         H
        #  - geompy.kind.BOX          xc yc zc                      ax ay az
        #  - geompy.kind.ROTATED_BOX  xc yc zc  zx zy zz  xx xy xz  ax ay az
        #  - geompy.kind.TORUS        xc yc zc  dx dy dz  R_1  R_2
        #  - geompy.kind.CONE         xb yb zb  dx dy dz  R_1  R_2  H
        #  - geompy.kind.POLYHEDRON                       nb_faces  nb_edges  nb_vertices
        #  - geompy.kind.SOLID                            nb_faces  nb_edges  nb_vertices
        #
        #  - geompy.kind.SPHERE2D     xc yc zc            R
        #  - geompy.kind.CYLINDER2D   xb yb zb  dx dy dz  R         H
        #  - geompy.kind.TORUS2D      xc yc zc  dx dy dz  R_1  R_2
        #  - geompy.kind.CONE2D       xc yc zc  dx dy dz  R_1  R_2  H
        #  - geompy.kind.DISK_CIRCLE  xc yc zc  dx dy dz  R
        #  - geompy.kind.DISK_ELLIPSE xc yc zc  dx dy dz  R_1  R_2
        #  - geompy.kind.POLYGON      xo yo zo  dx dy dz            nb_edges  nb_vertices
        #  - geompy.kind.PLANE        xo yo zo  dx dy dz
        #  - geompy.kind.PLANAR       xo yo zo  dx dy dz            nb_edges  nb_vertices
        #  - geompy.kind.FACE                                       nb_edges  nb_vertices
        #
        #  - geompy.kind.CIRCLE       xc yc zc  dx dy dz  R
        #  - geompy.kind.ARC_CIRCLE   xc yc zc  dx dy dz  R         x1 y1 z1  x2 y2 z2
        #  - geompy.kind.ELLIPSE      xc yc zc  dx dy dz  R_1  R_2
        #  - geompy.kind.ARC_ELLIPSE  xc yc zc  dx dy dz  R_1  R_2  x1 y1 z1  x2 y2 z2
        #  - geompy.kind.LINE         xo yo zo  dx dy dz
        #  - geompy.kind.SEGMENT      x1 y1 z1  x2 y2 z2
        #  - geompy.kind.EDGE                                                 nb_vertices
        #
        #  - geompy.kind.VERTEX       x  y  z
        #
        #  @ref swig_todo "Example"
        def KindOfShape(self,theShape):
            # Example: see GEOM_TestMeasures.py
            aRoughTuple = self.MeasuOp.KindOfShape(theShape)
            RaiseIfFailed("KindOfShape", self.MeasuOp)

            aKind  = aRoughTuple[0]
            anInts = aRoughTuple[1]
            aDbls  = aRoughTuple[2]

            # Now there is no exception from this rule:
            aKindTuple = [aKind] + aDbls + anInts

            # If they are we will regroup parameters for such kind of shape.
            # For example:
            #if aKind == kind.SOME_KIND:
            #    #  SOME_KIND     int int double int double double
            #    aKindTuple = [aKind, anInts[0], anInts[1], aDbls[0], anInts[2], aDbls[1], aDbls[2]]

            return aKindTuple

        # end of l2_measure
        ## @}

        ## @addtogroup l2_import_export
        ## @{

        ## Import a shape from the BREP or IGES or STEP file
        #  (depends on given format) with given name.
        #  @param theFileName The file, containing the shape.
        #  @param theFormatName Specify format for the file reading.
        #         Available formats can be obtained with InsertOp.ImportTranslators() method.
        #         If format 'IGES_SCALE' is used instead 'IGES' length unit will be
        #         set to 'meter' and result model will be scaled.
        #  @return New GEOM_Object, containing the imported shape.
        #
        #  @ref swig_Import_Export "Example"
        def Import(self,theFileName, theFormatName):
            # Example: see GEOM_TestOthers.py
            anObj = self.InsertOp.Import(theFileName, theFormatName)
            RaiseIfFailed("Import", self.InsertOp)
            return anObj

        ## Shortcut to Import() for BREP format
        #
        #  @ref swig_Import_Export "Example"
        def ImportBREP(self,theFileName):
            # Example: see GEOM_TestOthers.py
            return self.Import(theFileName, "BREP")

        ## Shortcut to Import() for IGES format
        #
        #  @ref swig_Import_Export "Example"
        def ImportIGES(self,theFileName):
            # Example: see GEOM_TestOthers.py
            return self.Import(theFileName, "IGES")

        ## Return length unit from given IGES file
        #
        #  @ref swig_Import_Export "Example"
        def GetIGESUnit(self,theFileName):
            # Example: see GEOM_TestOthers.py
            anObj = self.InsertOp.Import(theFileName, "IGES_UNIT")
            #RaiseIfFailed("Import", self.InsertOp)
            # recieve name using returned vertex
            UnitName = "M"
            vertices = self.SubShapeAll(anObj,ShapeType["VERTEX"])
            if len(vertices)>0:
                p = self.PointCoordinates(vertices[0])
                if abs(p[0]-0.01) < 1.e-6:
                    UnitName = "CM"
                elif abs(p[0]-0.001) < 1.e-6:
                    UnitName = "MM"
            return UnitName

        ## Shortcut to Import() for STEP format
        #
        #  @ref swig_Import_Export "Example"
        def ImportSTEP(self,theFileName):
            # Example: see GEOM_TestOthers.py
            return self.Import(theFileName, "STEP")

        ## Export the given shape into a file with given name.
        #  @param theObject Shape to be stored in the file.
        #  @param theFileName Name of the file to store the given shape in.
        #  @param theFormatName Specify format for the shape storage.
        #         Available formats can be obtained with InsertOp.ImportTranslators() method.
        #
        #  @ref swig_Import_Export "Example"
        def Export(self,theObject, theFileName, theFormatName):
            # Example: see GEOM_TestOthers.py
            self.InsertOp.Export(theObject, theFileName, theFormatName)
            if self.InsertOp.IsDone() == 0:
                raise RuntimeError,  "Export : " + self.InsertOp.GetErrorCode()
                pass
            pass

        ## Shortcut to Export() for BREP format
        #
        #  @ref swig_Import_Export "Example"
        def ExportBREP(self,theObject, theFileName):
            # Example: see GEOM_TestOthers.py
            return self.Export(theObject, theFileName, "BREP")

        ## Shortcut to Export() for IGES format
        #
        #  @ref swig_Import_Export "Example"
        def ExportIGES(self,theObject, theFileName):
            # Example: see GEOM_TestOthers.py
            return self.Export(theObject, theFileName, "IGES")

        ## Shortcut to Export() for STEP format
        #
        #  @ref swig_Import_Export "Example"
        def ExportSTEP(self,theObject, theFileName):
            # Example: see GEOM_TestOthers.py
            return self.Export(theObject, theFileName, "STEP")

        # end of l2_import_export
        ## @}

        ## @addtogroup l3_blocks
        ## @{

        ## Create a quadrangle face from four edges. Order of Edges is not
        #  important. It is  not necessary that edges share the same vertex.
        #  @param E1,E2,E3,E4 Edges for the face bound.
        #  @return New GEOM_Object, containing the created face.
        #
        #  @ref tui_building_by_blocks_page "Example"
        def MakeQuad(self,E1, E2, E3, E4):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeQuad(E1, E2, E3, E4)
            RaiseIfFailed("MakeQuad", self.BlocksOp)
            return anObj

        ## Create a quadrangle face on two edges.
        #  The missing edges will be built by creating the shortest ones.
        #  @param E1,E2 Two opposite edges for the face.
        #  @return New GEOM_Object, containing the created face.
        #
        #  @ref tui_building_by_blocks_page "Example"
        def MakeQuad2Edges(self,E1, E2):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeQuad2Edges(E1, E2)
            RaiseIfFailed("MakeQuad2Edges", self.BlocksOp)
            return anObj

        ## Create a quadrangle face with specified corners.
        #  The missing edges will be built by creating the shortest ones.
        #  @param V1,V2,V3,V4 Corner vertices for the face.
        #  @return New GEOM_Object, containing the created face.
        #
        #  @ref tui_building_by_blocks_page "Example 1"
        #  \n @ref swig_MakeQuad4Vertices "Example 2"
        def MakeQuad4Vertices(self,V1, V2, V3, V4):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeQuad4Vertices(V1, V2, V3, V4)
            RaiseIfFailed("MakeQuad4Vertices", self.BlocksOp)
            return anObj

        ## Create a hexahedral solid, bounded by the six given faces. Order of
        #  faces is not important. It is  not necessary that Faces share the same edge.
        #  @param F1,F2,F3,F4,F5,F6 Faces for the hexahedral solid.
        #  @return New GEOM_Object, containing the created solid.
        #
        #  @ref tui_building_by_blocks_page "Example 1"
        #  \n @ref swig_MakeHexa "Example 2"
        def MakeHexa(self,F1, F2, F3, F4, F5, F6):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeHexa(F1, F2, F3, F4, F5, F6)
            RaiseIfFailed("MakeHexa", self.BlocksOp)
            return anObj

        ## Create a hexahedral solid between two given faces.
        #  The missing faces will be built by creating the smallest ones.
        #  @param F1,F2 Two opposite faces for the hexahedral solid.
        #  @return New GEOM_Object, containing the created solid.
        #
        #  @ref tui_building_by_blocks_page "Example 1"
        #  \n @ref swig_MakeHexa2Faces "Example 2"
        def MakeHexa2Faces(self,F1, F2):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeHexa2Faces(F1, F2)
            RaiseIfFailed("MakeHexa2Faces", self.BlocksOp)
            return anObj

        # end of l3_blocks
        ## @}

        ## @addtogroup l3_blocks_op
        ## @{

        ## Get a vertex, found in the given shape by its coordinates.
        #  @param theShape Block or a compound of blocks.
        #  @param theX,theY,theZ Coordinates of the sought vertex.
        #  @param theEpsilon Maximum allowed distance between the resulting
        #                    vertex and point with the given coordinates.
        #  @return New GEOM_Object, containing the found vertex.
        #
        #  @ref swig_GetPoint "Example"
        def GetPoint(self, theShape, theX, theY, theZ, theEpsilon):
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetPoint(theShape, theX, theY, theZ, theEpsilon)
            RaiseIfFailed("GetPoint", self.BlocksOp)
            return anObj

        ## Find a vertex of the given shape, which has minimal distance to the given point.
        #  @param theShape Any shape.
        #  @param thePoint Point, close to the desired vertex.
        #  @return New GEOM_Object, containing the found vertex.
        #
        #  @ref swig_GetVertexNearPoint "Example"
        def GetVertexNearPoint(self, theShape, thePoint):
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetVertexNearPoint(theShape, thePoint)
            RaiseIfFailed("GetVertexNearPoint", self.BlocksOp)
            return anObj

        ## Get an edge, found in the given shape by two given vertices.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint1,thePoint2 Points, close to the ends of the desired edge.
        #  @return New GEOM_Object, containing the found edge.
        #
        #  @ref swig_GetEdge "Example"
        def GetEdge(self, theShape, thePoint1, thePoint2):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetEdge(theShape, thePoint1, thePoint2)
            RaiseIfFailed("GetEdge", self.BlocksOp)
            return anObj

        ## Find an edge of the given shape, which has minimal distance to the given point.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint Point, close to the desired edge.
        #  @return New GEOM_Object, containing the found edge.
        #
        #  @ref swig_GetEdgeNearPoint "Example"
        def GetEdgeNearPoint(self, theShape, thePoint):
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetEdgeNearPoint(theShape, thePoint)
            RaiseIfFailed("GetEdgeNearPoint", self.BlocksOp)
            return anObj

        ## Returns a face, found in the given shape by four given corner vertices.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint1,thePoint2,thePoint3,thePoint4 Points, close to the corners of the desired face.
        #  @return New GEOM_Object, containing the found face.
        #
        #  @ref swig_todo "Example"
        def GetFaceByPoints(self,theShape, thePoint1, thePoint2, thePoint3, thePoint4):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceByPoints(theShape, thePoint1, thePoint2, thePoint3, thePoint4)
            RaiseIfFailed("GetFaceByPoints", self.BlocksOp)
            return anObj

        ## Get a face of block, found in the given shape by two given edges.
        #  @param theShape Block or a compound of blocks.
        #  @param theEdge1,theEdge2 Edges, close to the edges of the desired face.
        #  @return New GEOM_Object, containing the found face.
        #
        #  @ref swig_todo "Example"
        def GetFaceByEdges(self,theShape, theEdge1, theEdge2):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceByEdges(theShape, theEdge1, theEdge2)
            RaiseIfFailed("GetFaceByEdges", self.BlocksOp)
            return anObj

        ## Find a face, opposite to the given one in the given block.
        #  @param theBlock Must be a hexahedral solid.
        #  @param theFace Face of \a theBlock, opposite to the desired face.
        #  @return New GEOM_Object, containing the found face.
        #
        #  @ref swig_GetOppositeFace "Example"
        def GetOppositeFace(self,theBlock, theFace):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetOppositeFace(theBlock, theFace)
            RaiseIfFailed("GetOppositeFace", self.BlocksOp)
            return anObj

        ## Find a face of the given shape, which has minimal distance to the given point.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint Point, close to the desired face.
        #  @return New GEOM_Object, containing the found face.
        #
        #  @ref swig_GetFaceNearPoint "Example"
        def GetFaceNearPoint(self, theShape, thePoint):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceNearPoint(theShape, thePoint)
            RaiseIfFailed("GetFaceNearPoint", self.BlocksOp)
            return anObj

        ## Find a face of block, whose outside normale has minimal angle with the given vector.
        #  @param theBlock Block or a compound of blocks.
        #  @param theVector Vector, close to the normale of the desired face.
        #  @return New GEOM_Object, containing the found face.
        #
        #  @ref swig_todo "Example"
        def GetFaceByNormale(self, theBlock, theVector):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceByNormale(theBlock, theVector)
            RaiseIfFailed("GetFaceByNormale", self.BlocksOp)
            return anObj

        ## Find all subshapes of type \a theShapeType of the given shape,
        #  which have minimal distance to the given point.
        #  @param theShape Any shape.
        #  @param thePoint Point, close to the desired shape.
        #  @param theShapeType Defines what kind of subshapes is searched.
        #  @param theTolerance The tolerance for distances comparison. All shapes
        #                      with distances to the given point in interval
        #                      [minimal_distance, minimal_distance + theTolerance] will be gathered.
        #  @return New GEOM_Object, containing a group of all found shapes.
        #
        #  @ref swig_GetShapesNearPoint "Example"
        def GetShapesNearPoint(self, theShape, thePoint, theShapeType, theTolerance = 1e-07):
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetShapesNearPoint(theShape, thePoint, theShapeType, theTolerance)
            RaiseIfFailed("GetShapesNearPoint", self.BlocksOp)
            return anObj

        # end of l3_blocks_op
        ## @}

        ## @addtogroup l4_blocks_measure
        ## @{

        ## Check, if the compound of blocks is given.
        #  To be considered as a compound of blocks, the
        #  given shape must satisfy the following conditions:
        #  - Each element of the compound should be a Block (6 faces and 12 edges).
        #  - A connection between two Blocks should be an entire quadrangle face or an entire edge.
        #  - The compound should be connexe.
        #  - The glue between two quadrangle faces should be applied.
        #  @param theCompound The compound to check.
        #  @return TRUE, if the given shape is a compound of blocks.
        #  If theCompound is not valid, prints all discovered errors.
        #
        #  @ref tui_measurement_tools_page "Example 1"
        #  \n @ref swig_CheckCompoundOfBlocks "Example 2"
        def CheckCompoundOfBlocks(self,theCompound):
            # Example: see GEOM_Spanner.py
            (IsValid, BCErrors) = self.BlocksOp.CheckCompoundOfBlocks(theCompound)
            RaiseIfFailed("CheckCompoundOfBlocks", self.BlocksOp)
            if IsValid == 0:
                Descr = self.BlocksOp.PrintBCErrors(theCompound, BCErrors)
                print Descr
            return IsValid

        ## Remove all seam and degenerated edges from \a theShape.
        #  Unite faces and edges, sharing one surface. It means that
        #  this faces must have references to one C++ surface object (handle).
        #  @param theShape The compound or single solid to remove irregular edges from.
        #  @param doUnionFaces If True, then unite faces. If False (the default value),
        #         do not unite faces.
        #  @return Improved shape.
        #
        #  @ref swig_RemoveExtraEdges "Example"
        def RemoveExtraEdges(self, theShape, doUnionFaces=False):
            # Example: see GEOM_TestOthers.py
            nbFacesOptimum = -1 # -1 means do not unite faces
            if doUnionFaces is True: nbFacesOptimum = 0 # 0 means unite faces
            anObj = self.BlocksOp.RemoveExtraEdges(theShape, nbFacesOptimum)
            RaiseIfFailed("RemoveExtraEdges", self.BlocksOp)
            return anObj

        ## Check, if the given shape is a blocks compound.
        #  Fix all detected errors.
        #    \note Single block can be also fixed by this method.
        #  @param theShape The compound to check and improve.
        #  @return Improved compound.
        #
        #  @ref swig_CheckAndImprove "Example"
        def CheckAndImprove(self,theShape):
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.CheckAndImprove(theShape)
            RaiseIfFailed("CheckAndImprove", self.BlocksOp)
            return anObj

        # end of l4_blocks_measure
        ## @}

        ## @addtogroup l3_blocks_op
        ## @{

        ## Get all the blocks, contained in the given compound.
        #  @param theCompound The compound to explode.
        #  @param theMinNbFaces If solid has lower number of faces, it is not a block.
        #  @param theMaxNbFaces If solid has higher number of faces, it is not a block.
        #    \note If theMaxNbFaces = 0, the maximum number of faces is not restricted.
        #  @return List of GEOM_Objects, containing the retrieved blocks.
        #
        #  @ref tui_explode_on_blocks "Example 1"
        #  \n @ref swig_MakeBlockExplode "Example 2"
        def MakeBlockExplode(self,theCompound, theMinNbFaces, theMaxNbFaces):
            # Example: see GEOM_TestOthers.py
            theMinNbFaces,theMaxNbFaces,Parameters = ParseParameters(theMinNbFaces,theMaxNbFaces)
            aList = self.BlocksOp.ExplodeCompoundOfBlocks(theCompound, theMinNbFaces, theMaxNbFaces)
            RaiseIfFailed("ExplodeCompoundOfBlocks", self.BlocksOp)
            for anObj in aList:
                anObj.SetParameters(Parameters)
                pass
            return aList

        ## Find block, containing the given point inside its volume or on boundary.
        #  @param theCompound Compound, to find block in.
        #  @param thePoint Point, close to the desired block. If the point lays on
        #         boundary between some blocks, we return block with nearest center.
        #  @return New GEOM_Object, containing the found block.
        #
        #  @ref swig_todo "Example"
        def GetBlockNearPoint(self,theCompound, thePoint):
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetBlockNearPoint(theCompound, thePoint)
            RaiseIfFailed("GetBlockNearPoint", self.BlocksOp)
            return anObj

        ## Find block, containing all the elements, passed as the parts, or maximum quantity of them.
        #  @param theCompound Compound, to find block in.
        #  @param theParts List of faces and/or edges and/or vertices to be parts of the found block.
        #  @return New GEOM_Object, containing the found block.
        #
        #  @ref swig_GetBlockByParts "Example"
        def GetBlockByParts(self,theCompound, theParts):
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetBlockByParts(theCompound, theParts)
            RaiseIfFailed("GetBlockByParts", self.BlocksOp)
            return anObj

        ## Return all blocks, containing all the elements, passed as the parts.
        #  @param theCompound Compound, to find blocks in.
        #  @param theParts List of faces and/or edges and/or vertices to be parts of the found blocks.
        #  @return List of GEOM_Objects, containing the found blocks.
        #
        #  @ref swig_todo "Example"
        def GetBlocksByParts(self,theCompound, theParts):
            # Example: see GEOM_Spanner.py
            aList = self.BlocksOp.GetBlocksByParts(theCompound, theParts)
            RaiseIfFailed("GetBlocksByParts", self.BlocksOp)
            return aList

        ## Multi-transformate block and glue the result.
        #  Transformation is defined so, as to superpose direction faces.
        #  @param Block Hexahedral solid to be multi-transformed.
        #  @param DirFace1 ID of First direction face.
        #  @param DirFace2 ID of Second direction face.
        #  @param NbTimes Quantity of transformations to be done.
        #    \note Unique ID of sub-shape can be obtained, using method GetSubShapeID().
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref tui_multi_transformation "Example"
        def MakeMultiTransformation1D(self,Block, DirFace1, DirFace2, NbTimes):
            # Example: see GEOM_Spanner.py
            DirFace1,DirFace2,NbTimes,Parameters = ParseParameters(DirFace1,DirFace2,NbTimes)
            anObj = self.BlocksOp.MakeMultiTransformation1D(Block, DirFace1, DirFace2, NbTimes)
            RaiseIfFailed("MakeMultiTransformation1D", self.BlocksOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Multi-transformate block and glue the result.
        #  @param Block Hexahedral solid to be multi-transformed.
        #  @param DirFace1U,DirFace2U IDs of Direction faces for the first transformation.
        #  @param DirFace1V,DirFace2V IDs of Direction faces for the second transformation.
        #  @param NbTimesU,NbTimesV Quantity of transformations to be done.
        #  @return New GEOM_Object, containing the result shape.
        #
        #  @ref tui_multi_transformation "Example"
        def MakeMultiTransformation2D(self,Block, DirFace1U, DirFace2U, NbTimesU,
                                      DirFace1V, DirFace2V, NbTimesV):
            # Example: see GEOM_Spanner.py
            DirFace1U,DirFace2U,NbTimesU,DirFace1V,DirFace2V,NbTimesV,Parameters = ParseParameters(
              DirFace1U,DirFace2U,NbTimesU,DirFace1V,DirFace2V,NbTimesV)
            anObj = self.BlocksOp.MakeMultiTransformation2D(Block, DirFace1U, DirFace2U, NbTimesU,
                                                            DirFace1V, DirFace2V, NbTimesV)
            RaiseIfFailed("MakeMultiTransformation2D", self.BlocksOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Build all possible propagation groups.
        #  Propagation group is a set of all edges, opposite to one (main)
        #  edge of this group directly or through other opposite edges.
        #  Notion of Opposite Edge make sence only on quadrangle face.
        #  @param theShape Shape to build propagation groups on.
        #  @return List of GEOM_Objects, each of them is a propagation group.
        #
        #  @ref swig_Propagate "Example"
        def Propagate(self,theShape):
            # Example: see GEOM_TestOthers.py
            listChains = self.BlocksOp.Propagate(theShape)
            RaiseIfFailed("Propagate", self.BlocksOp)
            return listChains

        # end of l3_blocks_op
        ## @}

        ## @addtogroup l3_groups
        ## @{

        ## Creates a new group which will store sub shapes of theMainShape
        #  @param theMainShape is a GEOM object on which the group is selected
        #  @param theShapeType defines a shape type of the group
        #  @return a newly created GEOM group
        #
        #  @ref tui_working_with_groups_page "Example 1"
        #  \n @ref swig_CreateGroup "Example 2"
        def CreateGroup(self,theMainShape, theShapeType):
            # Example: see GEOM_TestOthers.py
            anObj = self.GroupOp.CreateGroup(theMainShape, theShapeType)
            RaiseIfFailed("CreateGroup", self.GroupOp)
            return anObj

        ## Adds a sub object with ID theSubShapeId to the group
        #  @param theGroup is a GEOM group to which the new sub shape is added
        #  @param theSubShapeID is a sub shape ID in the main object.
        #  \note Use method GetSubShapeID() to get an unique ID of the sub shape
        #
        #  @ref tui_working_with_groups_page "Example"
        def AddObject(self,theGroup, theSubShapeID):
            # Example: see GEOM_TestOthers.py
            self.GroupOp.AddObject(theGroup, theSubShapeID)
            if self.GroupOp.GetErrorCode() != "PAL_ELEMENT_ALREADY_PRESENT":
                RaiseIfFailed("AddObject", self.GroupOp)
                pass
            pass

        ## Removes a sub object with ID \a theSubShapeId from the group
        #  @param theGroup is a GEOM group from which the new sub shape is removed
        #  @param theSubShapeID is a sub shape ID in the main object.
        #  \note Use method GetSubShapeID() to get an unique ID of the sub shape
        #
        #  @ref tui_working_with_groups_page "Example"
        def RemoveObject(self,theGroup, theSubShapeID):
            # Example: see GEOM_TestOthers.py
            self.GroupOp.RemoveObject(theGroup, theSubShapeID)
            RaiseIfFailed("RemoveObject", self.GroupOp)
            pass

        ## Adds to the group all the given shapes. No errors, if some shapes are alredy included.
        #  @param theGroup is a GEOM group to which the new sub shapes are added.
        #  @param theSubShapes is a list of sub shapes to be added.
        #
        #  @ref tui_working_with_groups_page "Example"
        def UnionList (self,theGroup, theSubShapes):
            # Example: see GEOM_TestOthers.py
            self.GroupOp.UnionList(theGroup, theSubShapes)
            RaiseIfFailed("UnionList", self.GroupOp)
            pass

        ## Works like the above method, but argument
        #  theSubShapes here is a list of sub-shapes indices
        #
        #  @ref swig_UnionIDs "Example"
        def UnionIDs(self,theGroup, theSubShapes):
            # Example: see GEOM_TestOthers.py
            self.GroupOp.UnionIDs(theGroup, theSubShapes)
            RaiseIfFailed("UnionIDs", self.GroupOp)
            pass

        ## Removes from the group all the given shapes. No errors, if some shapes are not included.
        #  @param theGroup is a GEOM group from which the sub-shapes are removed.
        #  @param theSubShapes is a list of sub-shapes to be removed.
        #
        #  @ref tui_working_with_groups_page "Example"
        def DifferenceList (self,theGroup, theSubShapes):
            # Example: see GEOM_TestOthers.py
            self.GroupOp.DifferenceList(theGroup, theSubShapes)
            RaiseIfFailed("DifferenceList", self.GroupOp)
            pass

        ## Works like the above method, but argument
        #  theSubShapes here is a list of sub-shapes indices
        #
        #  @ref swig_DifferenceIDs "Example"
        def DifferenceIDs(self,theGroup, theSubShapes):
            # Example: see GEOM_TestOthers.py
            self.GroupOp.DifferenceIDs(theGroup, theSubShapes)
            RaiseIfFailed("DifferenceIDs", self.GroupOp)
            pass

        ## Returns a list of sub objects ID stored in the group
        #  @param theGroup is a GEOM group for which a list of IDs is requested
        #
        #  @ref swig_GetObjectIDs "Example"
        def GetObjectIDs(self,theGroup):
            # Example: see GEOM_TestOthers.py
            ListIDs = self.GroupOp.GetObjects(theGroup)
            RaiseIfFailed("GetObjects", self.GroupOp)
            return ListIDs

        ## Returns a type of sub objects stored in the group
        #  @param theGroup is a GEOM group which type is returned.
        #
        #  @ref swig_GetType "Example"
        def GetType(self,theGroup):
            # Example: see GEOM_TestOthers.py
            aType = self.GroupOp.GetType(theGroup)
            RaiseIfFailed("GetType", self.GroupOp)
            return aType

        ## Convert a type of geom object from id to string value
        #  @param theId is a GEOM obect type id.
        #
        #  @ref swig_GetType "Example"
        def ShapeIdToType(self, theId):
            if theId == 0:
                return "COPY"
            if theId == 1:
                return "IMPORT"
            if theId == 2:
                return "POINT"
            if theId == 3:
                return "VECTOR"
            if theId == 4:
                return "PLANE"
            if theId == 5:
                return "LINE"
            if theId == 6:
                return "TORUS"
            if theId == 7:
                return "BOX"
            if theId == 8:
                return "CYLINDER"
            if theId == 9:
                return "CONE"
            if theId == 10:
                return "SPHERE"
            if theId == 11:
                return "PRISM"
            if theId == 12:
                return "REVOLUTION"
            if theId == 13:
                return "BOOLEAN"
            if theId == 14:
                return "PARTITION"
            if theId == 15:
                return "POLYLINE"
            if theId == 16:
                return "CIRCLE"
            if theId == 17:
                return "SPLINE"
            if theId == 18:
                return "ELLIPSE"
            if theId == 19:
                return "CIRC_ARC"
            if theId == 20:
                return "FILLET"
            if theId == 21:
                return "CHAMFER"
            if theId == 22:
                return "EDGE"
            if theId == 23:
                return "WIRE"
            if theId == 24:
                return "FACE"
            if theId == 25:
                return "SHELL"
            if theId == 26:
                return "SOLID"
            if theId == 27:
                return "COMPOUND"
            if theId == 28:
                return "SUBSHAPE"
            if theId == 29:
                return "PIPE"
            if theId == 30:
                return "ARCHIMEDE"
            if theId == 31:
                return "FILLING"
            if theId == 32:
                return "EXPLODE"
            if theId == 33:
                return "GLUED"
            if theId == 34:
                return "SKETCHER"
            if theId == 35:
                return "CDG"
            if theId == 36:
                return "FREE_BOUNDS"
            if theId == 37:
                return "GROUP"
            if theId == 38:
                return "BLOCK"
            if theId == 39:
                return "MARKER"
            if theId == 40:
                return "THRUSECTIONS"
            if theId == 41:
                return "COMPOUNDFILTER"
            if theId == 42:
                return "SHAPES_ON_SHAPE"
            if theId == 43:
                return "ELLIPSE_ARC"
            if theId == 44:
                return "3DSKETCHER"
            if theId == 45:
                return "FILLET_2D"
            if theId == 46:
                return "FILLET_1D"
            return "Shape Id not exist."

        ## Returns a main shape associated with the group
        #  @param theGroup is a GEOM group for which a main shape object is requested
        #  @return a GEOM object which is a main shape for theGroup
        #
        #  @ref swig_GetMainShape "Example"
        def GetMainShape(self,theGroup):
            # Example: see GEOM_TestOthers.py
            anObj = self.GroupOp.GetMainShape(theGroup)
            RaiseIfFailed("GetMainShape", self.GroupOp)
            return anObj

        ## Create group of edges of theShape, whose length is in range [min_length, max_length].
        #  If include_min/max == 0, edges with length == min/max_length will not be included in result.
        #
        #  @ref swig_todo "Example"
        def GetEdgesByLength (self, theShape, min_length, max_length, include_min = 1, include_max = 1):
            edges = self.SubShapeAll(theShape, ShapeType["EDGE"])
            edges_in_range = []
            for edge in edges:
                Props = self.BasicProperties(edge)
                if min_length <= Props[0] and Props[0] <= max_length:
                    if (not include_min) and (min_length == Props[0]):
                        skip = 1
                    else:
                        if (not include_max) and (Props[0] == max_length):
                            skip = 1
                        else:
                            edges_in_range.append(edge)

            if len(edges_in_range) <= 0:
                print "No edges found by given criteria"
                return 0

            group_edges = self.CreateGroup(theShape, ShapeType["EDGE"])
            self.UnionList(group_edges, edges_in_range)

            return group_edges

        ## Create group of edges of selected shape, whose length is in range [min_length, max_length].
        #  If include_min/max == 0, edges with length == min/max_length will not be included in result.
        #
        #  @ref swig_todo "Example"
        def SelectEdges (self, min_length, max_length, include_min = 1, include_max = 1):
            nb_selected = sg.SelectedCount()
            if nb_selected < 1:
                print "Select a shape before calling this function, please."
                return 0
            if nb_selected > 1:
                print "Only one shape must be selected"
                return 0

            id_shape = sg.getSelected(0)
            shape = IDToObject( id_shape )

            group_edges = self.GetEdgesByLength(shape, min_length, max_length, include_min, include_max)

            left_str  = " < "
            right_str = " < "
            if include_min: left_str  = " <= "
            if include_max: right_str  = " <= "

            self.addToStudyInFather(shape, group_edges, "Group of edges with " + `min_length`
                                    + left_str + "length" + right_str + `max_length`)

            sg.updateObjBrowser(1)

            return group_edges

        # end of l3_groups
        ## @}

        ## @addtogroup l4_advanced
        ## @{

        ## Create a T-shape object with specified caracteristics for the main
        #  and the incident pipes (radius, width, half-length).
        #  The extremities of the main pipe are located on junctions points P1 and P2.
        #  The extremity of the incident pipe is located on junction point P3.
        #  If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
        #  the main plane of the T-shape is XOY.
        #  @param theR1 Internal radius of main pipe
        #  @param theW1 Width of main pipe
        #  @param theL1 Half-length of main pipe
        #  @param theR2 Internal radius of incident pipe (R2 < R1)
        #  @param theW2 Width of incident pipe (R2+W2 < R1+W1)
        #  @param theL2 Half-length of incident pipe
        #  @param theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
        #  @param theP1 1st junction point of main pipe
        #  @param theP2 2nd junction point of main pipe
        #  @param theP3 Junction point of incident pipe
        #  @return List of GEOM_Objects, containing the created shape and propagation groups.
        #
        #  @ref tui_creation_pipetshape "Example"
        def MakePipeTShape(self, theR1, theW1, theL1, theR2, theW2, theL2, theHexMesh=True, theP1=None, theP2=None, theP3=None):
            theR1, theW1, theL1, theR2, theW2, theL2, Parameters = ParseParameters(theR1, theW1, theL1, theR2, theW2, theL2)
            if (theP1 and theP2 and theP3):
                anObj = self.AdvOp.MakePipeTShapeWithPosition(theR1, theW1, theL1, theR2, theW2, theL2, theHexMesh, theP1, theP2, theP3)
            else:
                anObj = self.AdvOp.MakePipeTShape(theR1, theW1, theL1, theR2, theW2, theL2, theHexMesh)
            RaiseIfFailed("MakePipeTShape", self.AdvOp)
            if Parameters: anObj[0].SetParameters(Parameters)
            return anObj

        ## Create a T-shape object with chamfer and with specified caracteristics for the main
        #  and the incident pipes (radius, width, half-length). The chamfer is
        #  created on the junction of the pipes.
        #  The extremities of the main pipe are located on junctions points P1 and P2.
        #  The extremity of the incident pipe is located on junction point P3.
        #  If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
        #  the main plane of the T-shape is XOY.
        #  @param theR1 Internal radius of main pipe
        #  @param theW1 Width of main pipe
        #  @param theL1 Half-length of main pipe
        #  @param theR2 Internal radius of incident pipe (R2 < R1)
        #  @param theW2 Width of incident pipe (R2+W2 < R1+W1)
        #  @param theL2 Half-length of incident pipe
        #  @param theH Height of the chamfer.
        #  @param theW Width of the chamfer.
        #  @param theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
        #  @param theP1 1st junction point of main pipe
        #  @param theP2 2nd junction point of main pipe
        #  @param theP3 Junction point of incident pipe
        #  @return List of GEOM_Objects, containing the created shape and propagation groups.
        #
        #  @ref tui_creation_pipetshape "Example"
        def MakePipeTShapeChamfer(self, theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, theHexMesh=True, theP1=None, theP2=None, theP3=None):
            theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, Parameters = ParseParameters(theR1, theW1, theL1, theR2, theW2, theL2, theH, theW)
            if (theP1 and theP2 and theP3):
              anObj = self.AdvOp.MakePipeTShapeChamferWithPosition(theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, theHexMesh, theP1, theP2, theP3)
            else:
              anObj = self.AdvOp.MakePipeTShapeChamfer(theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, theHexMesh)
            RaiseIfFailed("MakePipeTShapeChamfer", self.AdvOp)
            if Parameters: anObj[0].SetParameters(Parameters)
            return anObj

        ## Create a T-shape object with fillet and with specified caracteristics for the main
        #  and the incident pipes (radius, width, half-length). The fillet is
        #  created on the junction of the pipes.
        #  The extremities of the main pipe are located on junctions points P1 and P2.
        #  The extremity of the incident pipe is located on junction point P3.
        #  If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
        #  the main plane of the T-shape is XOY.
        #  @param theR1 Internal radius of main pipe
        #  @param theW1 Width of main pipe
        #  @param theL1 Half-length of main pipe
        #  @param theR2 Internal radius of incident pipe (R2 < R1)
        #  @param theW2 Width of incident pipe (R2+W2 < R1+W1)
        #  @param theL2 Half-length of incident pipe
        #  @param theRF Radius of curvature of fillet.
        #  @param theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
        #  @param theP1 1st junction point of main pipe
        #  @param theP2 2nd junction point of main pipe
        #  @param theP3 Junction point of incident pipe
        #  @return List of GEOM_Objects, containing the created shape and propagation groups.
        #
        #  @ref tui_creation_pipetshape "Example"
        def MakePipeTShapeFillet(self, theR1, theW1, theL1, theR2, theW2, theL2, theRF, theHexMesh=True, theP1=None, theP2=None, theP3=None):
            theR1, theW1, theL1, theR2, theW2, theL2, theRF, Parameters = ParseParameters(theR1, theW1, theL1, theR2, theW2, theL2, theRF)
            if (theP1 and theP2 and theP3):
              anObj = self.AdvOp.MakePipeTShapeFilletWithPosition(theR1, theW1, theL1, theR2, theW2, theL2, theRF, theHexMesh, theP1, theP2, theP3)
            else:
              anObj = self.AdvOp.MakePipeTShapeFillet(theR1, theW1, theL1, theR2, theW2, theL2, theRF, theHexMesh)
            RaiseIfFailed("MakePipeTShapeFillet", self.AdvOp)
            if Parameters: anObj[0].SetParameters(Parameters)
            return anObj

        #@@ insert new functions before this line @@ do not remove this line @@#

        # end of l4_advanced
        ## @}

        ## Create a copy of the given object
        #  @ingroup l1_geompy_auxiliary
        #
        #  @ref swig_all_advanced "Example"
        def MakeCopy(self,theOriginal):
            # Example: see GEOM_TestAll.py
            anObj = self.InsertOp.MakeCopy(theOriginal)
            RaiseIfFailed("MakeCopy", self.InsertOp)
            return anObj

        ## Add Path to load python scripts from
        #  @ingroup l1_geompy_auxiliary
        def addPath(self,Path):
            if (sys.path.count(Path) < 1):
                sys.path.append(Path)
                pass
            pass

        ## Load marker texture from the file
        #  @param Path a path to the texture file
        #  @return unique texture identifier
        #  @ingroup l1_geompy_auxiliary
        def LoadTexture(self, Path):
            # Example: see GEOM_TestAll.py
            ID = self.InsertOp.LoadTexture(Path)
            RaiseIfFailed("LoadTexture", self.InsertOp)
            return ID

        ## Add marker texture. @a Width and @a Height parameters
        #  specify width and height of the texture in pixels.
        #  If @a RowData is @c True, @a Texture parameter should represent texture data
        #  packed into the byte array. If @a RowData is @c False (default), @a Texture
        #  parameter should be unpacked string, in which '1' symbols represent opaque
        #  pixels and '0' represent transparent pixels of the texture bitmap.
        #
        #  @param Width texture width in pixels
        #  @param Height texture height in pixels
        #  @param Texture texture data
        #  @param RowData if @c True, @a Texture data are packed in the byte stream
        #  @ingroup l1_geompy_auxiliary
        def AddTexture(self, Width, Height, Texture, RowData=False):
            # Example: see GEOM_TestAll.py
            if not RowData: Texture = PackData(Texture)
            ID = self.InsertOp.AddTexture(Width, Height, Texture)
            RaiseIfFailed("AddTexture", self.InsertOp)
            return ID

import omniORB
#Register the new proxy for GEOM_Gen
omniORB.registerObjref(GEOM._objref_GEOM_Gen._NP_RepositoryId, geompyDC)
