#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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
##       @defgroup l4_decompose_d   Decompose objects deprecated methods
##       @defgroup l4_access        Access to sub-shapes by their unique IDs inside the main shape
##       @defgroup l4_obtain        Access to sub-shapes by a criteria
##       @defgroup l4_advanced      Advanced objects creation functions

##     @}

##   @}
##   @defgroup l2_transforming  Transforming geometrical objects
##   @{
##     @defgroup l3_basic_op      Basic Operations
##     @defgroup l3_boolean       Boolean Operations
##     @defgroup l3_transform     Transformation Operations
##     @defgroup l3_local         Local Operations (Fillet, Chamfer and other Features)
##     @defgroup l3_blocks_op     Blocks Operations
##     @defgroup l3_healing       Repairing Operations
##     @defgroup l3_restore_ss    Restore presentation parameters and a tree of sub-shapes

##   @}
##   @defgroup l2_measure       Using measurement tools

## @}

# initialize SALOME session in try/except block
# to avoid problems in some cases, e.g. when generating documentation
try:
    import salome
    salome.salome_init()
    from salome import *
except:
    pass

from salome_notebook import *

import GEOM
import math
import os

from gsketcher import Sketcher3D

## Enumeration ShapeType as a dictionary. \n
## Topological types of shapes (like Open Cascade types). See GEOM::shape_type for details.
#  @ingroup l1_geompy_auxiliary
ShapeType = {"AUTO":-1, "COMPOUND":0, "COMPSOLID":1, "SOLID":2, "SHELL":3, "FACE":4, "WIRE":5, "EDGE":6, "VERTEX":7, "SHAPE":8}

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
    """
    Helper function which can be used to pack the passed string to the byte data.
    Only '1' an '0' symbols are valid for the string. The missing bits are replaced by zeroes.
    If the string contains invalid symbol (neither '1' nor '0'), the function raises an exception.

    Parameters:
        data unpacked data - a string containing '1' and '0' symbols

    Returns:
        data packed to the byte stream
        
    Example of usage:
        val = PackData("10001110") # val = 0xAE
        val = PackData("1")        # val = 0x80
    """
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
    """
    Read bitmap texture from the text file.
    In that file, any non-zero symbol represents '1' opaque pixel of the bitmap.
    A zero symbol ('0') represents transparent pixel of the texture bitmap.
    The function returns width and height of the pixmap in pixels and byte stream representing
    texture bitmap itself.
    This function can be used to read the texture to the byte stream in order to pass it to
    the AddTexture() function of geompy class.
    
    Parameters:
        fname texture file name

    Returns:
        sequence of tree values: texture's width, height in pixels and its byte stream
    
    Example of usage:
        import geompy
        geompy.init_geom(salome.myStudy)
        texture = geompy.readtexture('mytexture.dat')
        texture = geompy.AddTexture(*texture)
        obj.SetMarkerTexture(texture)
    """
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

## Returns a long value from enumeration type
#  Can be used for CORBA enumerator types like GEOM.shape_type
#  @param theItem enumeration type
#  @ingroup l1_geompy_auxiliary
def EnumToLong(theItem):
    """
    Returns a long value from enumeration type
    Can be used for CORBA enumerator types like geompy.ShapeType

    Parameters:
        theItem enumeration type
    """
    ret = theItem
    if hasattr(theItem, "_v"): ret = theItem._v
    return ret

## Kinds of shape in terms of <VAR>GEOM.GEOM_IKindOfShape.shape_kind</VAR> enumeration
#  and a list of parameters, describing the shape.
#  List of parameters, describing the shape:
#  - COMPOUND:            [nb_solids  nb_faces  nb_edges  nb_vertices]
#  - COMPSOLID:           [nb_solids  nb_faces  nb_edges  nb_vertices]
#
#  - SHELL:       [info.CLOSED / info.UNCLOSED  nb_faces  nb_edges  nb_vertices]
#
#  - WIRE:        [info.CLOSED / info.UNCLOSED nb_edges  nb_vertices]
#
#  - SPHERE:       [xc yc zc            R]
#  - CYLINDER:     [xb yb zb  dx dy dz  R         H]
#  - BOX:          [xc yc zc                      ax ay az]
#  - ROTATED_BOX:  [xc yc zc  zx zy zz  xx xy xz  ax ay az]
#  - TORUS:        [xc yc zc  dx dy dz  R_1  R_2]
#  - CONE:         [xb yb zb  dx dy dz  R_1  R_2  H]
#  - POLYHEDRON:                       [nb_faces  nb_edges  nb_vertices]
#  - SOLID:                            [nb_faces  nb_edges  nb_vertices]
#
#  - SPHERE2D:     [xc yc zc            R]
#  - CYLINDER2D:   [xb yb zb  dx dy dz  R         H]
#  - TORUS2D:      [xc yc zc  dx dy dz  R_1  R_2]
#  - CONE2D:       [xc yc zc  dx dy dz  R_1  R_2  H]
#  - DISK_CIRCLE:  [xc yc zc  dx dy dz  R]
#  - DISK_ELLIPSE: [xc yc zc  dx dy dz  R_1  R_2]
#  - POLYGON:      [xo yo zo  dx dy dz            nb_edges  nb_vertices]
#  - PLANE:        [xo yo zo  dx dy dz]
#  - PLANAR:       [xo yo zo  dx dy dz            nb_edges  nb_vertices]
#  - FACE:                                       [nb_edges  nb_vertices]
#
#  - CIRCLE:       [xc yc zc  dx dy dz  R]
#  - ARC_CIRCLE:   [xc yc zc  dx dy dz  R         x1 y1 z1  x2 y2 z2]
#  - ELLIPSE:      [xc yc zc  dx dy dz  R_1  R_2]
#  - ARC_ELLIPSE:  [xc yc zc  dx dy dz  R_1  R_2  x1 y1 z1  x2 y2 z2]
#  - LINE:         [xo yo zo  dx dy dz]
#  - SEGMENT:      [x1 y1 z1  x2 y2 z2]
#  - EDGE:                                                 [nb_vertices]
#
#  - VERTEX:       [x  y  z]
#  @ingroup l1_geompy_auxiliary
kind = GEOM.GEOM_IKindOfShape

## Information about closed/unclosed state of shell or wire
#  @ingroup l1_geompy_auxiliary
class info:
    """
    Information about closed/unclosed state of shell or wire
    """
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

        ## Dump component to the Python script
        #  This method overrides IDL function to allow default values for the parameters.
        def DumpPython(self, theStudy, theIsPublished=True, theIsMultiFile=True):
            """
            Dump component to the Python script
            This method overrides IDL function to allow default values for the parameters.
            """
            return GEOM._objref_GEOM_Gen.DumpPython(self, theStudy, theIsPublished, theIsMultiFile)

        ## Get name for sub-shape aSubObj of shape aMainObj
        #
        # @ref swig_SubShapeName "Example"
        def SubShapeName(self,aSubObj, aMainObj):
            """
            Get name for sub-shape aSubObj of shape aMainObj
            """
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
        #  \param theArgs,theFindMethod,theInheritFirstArg see RestoreSubShapes() for
        #                                                  these arguments description
        #  \return study entry of the published shape in form of string
        #
        #  @ref swig_all_addtostudy "Example"
        def addToStudy(self, aShape, aName, doRestoreSubShapes=False,
                       theArgs=[], theFindMethod=GEOM.FSM_GetInPlace, theInheritFirstArg=False):
            """
            Publish in study aShape with name aName

            Parameters:
                aShape the shape to be published
                aName  the name for the shape
                doRestoreSubShapes if True, finds and publishes also
                                   sub-shapes of aShape, corresponding to its arguments
                                   and published sub-shapes of arguments
                theArgs,theFindMethod,theInheritFirstArg see geompy.RestoreSubShapes() for
                                                         these arguments description

            Returns:
                study entry of the published shape in form of string

            Example of usage:
                id_block1 = geompy.addToStudy(Block1, "Block 1")
            """
            # Example: see GEOM_TestAll.py
            try:
                aSObject = self.AddInStudy(self.myStudy, aShape, aName, None)
                if aSObject and aName: aSObject.SetAttrString("AttributeName", aName)
                if doRestoreSubShapes:
                    self.RestoreSubShapesSO(self.myStudy, aSObject, theArgs,
                                            theFindMethod, theInheritFirstArg, True )
            except:
                print "addToStudy() failed"
                return ""
            return aShape.GetStudyEntry()

        ## Publish in study aShape with name aName as sub-object of previously published aFather
        #  \param aFather previously published object
        #  \param aShape the shape to be published as sub-object of <VAR>aFather</VAR>
        #  \param aName  the name for the shape
        #
        #  \return study entry of the published shape in form of string
        #  @ref swig_all_addtostudyInFather "Example"
        def addToStudyInFather(self, aFather, aShape, aName):
            """
            Publish in study aShape with name aName as sub-object of previously published aFather

            Parameters:
                aFather previously published object
                aShape the shape to be published as sub-object of aFather
                aName  the name for the shape

            Returns:
                study entry of the published shape in form of string
            """
            # Example: see GEOM_TestAll.py
            try:
                aSObject = self.AddInStudy(self.myStudy, aShape, aName, aFather)
                if aSObject and aName: aSObject.SetAttrString("AttributeName", aName)
            except:
                print "addToStudyInFather() failed"
                return ""
            return aShape.GetStudyEntry()

        ## Unpublish object in study
        #
        #  \param obj the object to be unpublished
        def hideInStudy(self, obj):
            """
            Unpublish object in study

            Parameters:
                obj the object to be unpublished
            """
            ior = salome.orb.object_to_string(obj)
            aSObject = self.myStudy.FindObjectIOR(ior)
            if aSObject is not None:
                genericAttribute = self.myBuilder.FindOrCreateAttribute(aSObject, "AttributeDrawable")
                drwAttribute = genericAttribute._narrow(SALOMEDS.AttributeDrawable)
                drwAttribute.SetDrawable(False)
                pass

        # end of l1_geompy_auxiliary
        ## @}

        ## @addtogroup l3_restore_ss
        ## @{

        ## Publish sub-shapes, standing for arguments and sub-shapes of arguments
        #  To be used from python scripts out of addToStudy() (non-default usage)
        #  \param theObject published GEOM.GEOM_Object, arguments of which will be published
        #  \param theArgs   list of GEOM.GEOM_Object, operation arguments to be published.
        #                   If this list is empty, all operation arguments will be published
        #  \param theFindMethod method to search sub-shapes, corresponding to arguments and
        #                       their sub-shapes. Value from enumeration GEOM.find_shape_method.
        #  \param theInheritFirstArg set properties of the first argument for <VAR>theObject</VAR>.
        #                            Do not publish sub-shapes in place of arguments, but only
        #                            in place of sub-shapes of the first argument,
        #                            because the whole shape corresponds to the first argument.
        #                            Mainly to be used after transformations, but it also can be
        #                            usefull after partition with one object shape, and some other
        #                            operations, where only the first argument has to be considered.
        #                            If theObject has only one argument shape, this flag is automatically
        #                            considered as True, not regarding really passed value.
        #  \param theAddPrefix add prefix "from_" to names of restored sub-shapes,
        #                      and prefix "from_subshapes_of_" to names of partially restored sub-shapes.
        #  \return list of published sub-shapes
        #
        #  @ref tui_restore_prs_params "Example"
        def RestoreSubShapes (self, theObject, theArgs=[], theFindMethod=GEOM.FSM_GetInPlace,
                              theInheritFirstArg=False, theAddPrefix=True):
            """
            Publish sub-shapes, standing for arguments and sub-shapes of arguments
            To be used from python scripts out of geompy.addToStudy (non-default usage)

            Parameters:
                theObject published GEOM.GEOM_Object, arguments of which will be published
                theArgs   list of GEOM.GEOM_Object, operation arguments to be published.
                          If this list is empty, all operation arguments will be published
                theFindMethod method to search sub-shapes, corresponding to arguments and
                              their sub-shapes. Value from enumeration GEOM.find_shape_method.
                theInheritFirstArg set properties of the first argument for theObject.
                                   Do not publish sub-shapes in place of arguments, but only
                                   in place of sub-shapes of the first argument,
                                   because the whole shape corresponds to the first argument.
                                   Mainly to be used after transformations, but it also can be
                                   usefull after partition with one object shape, and some other
                                   operations, where only the first argument has to be considered.
                                   If theObject has only one argument shape, this flag is automatically
                                   considered as True, not regarding really passed value.
                theAddPrefix add prefix "from_" to names of restored sub-shapes,
                             and prefix "from_subshapes_of_" to names of partially restored sub-shapes.
            Returns:
                list of published sub-shapes
            """
            # Example: see GEOM_TestAll.py
            return self.RestoreSubShapesO(self.myStudy, theObject, theArgs,
                                          theFindMethod, theInheritFirstArg, theAddPrefix)

        ## Publish sub-shapes, standing for arguments and sub-shapes of arguments
        #  To be used from python scripts out of addToStudy() (non-default usage)
        #  \param theObject published GEOM.GEOM_Object, arguments of which will be published
        #  \param theArgs   list of GEOM.GEOM_Object, operation arguments to be published.
        #                   If this list is empty, all operation arguments will be published
        #  \param theFindMethod method to search sub-shapes, corresponding to arguments and
        #                       their sub-shapes. Value from enumeration GEOM::find_shape_method.
        #  \param theInheritFirstArg set properties of the first argument for <VAR>theObject</VAR>.
        #                            Do not publish sub-shapes in place of arguments, but only
        #                            in place of sub-shapes of the first argument,
        #                            because the whole shape corresponds to the first argument.
        #                            Mainly to be used after transformations, but it also can be
        #                            usefull after partition with one object shape, and some other
        #                            operations, where only the first argument has to be considered.
        #                            If theObject has only one argument shape, this flag is automatically
        #                            considered as True, not regarding really passed value.
        #  \param theAddPrefix add prefix "from_" to names of restored sub-shapes,
        #                      and prefix "from_subshapes_of_" to names of partially restored sub-shapes.
        #  \return list of published sub-shapes
        #
        #  @ref tui_restore_prs_params "Example"
        def RestoreGivenSubShapes (self, theObject, theArgs=[], theFindMethod=GEOM.FSM_GetInPlace,
                                   theInheritFirstArg=False, theAddPrefix=True):
            """
            Publish sub-shapes, standing for arguments and sub-shapes of arguments
            To be used from python scripts out of geompy.addToStudy() (non-default usage)

            Parameters:
                theObject published GEOM.GEOM_Object, arguments of which will be published
                theArgs   list of GEOM.GEOM_Object, operation arguments to be published.
                          If this list is empty, all operation arguments will be published
                theFindMethod method to search sub-shapes, corresponding to arguments and
                              their sub-shapes. Value from enumeration GEOM::find_shape_method.
                theInheritFirstArg set properties of the first argument for theObject.
                                   Do not publish sub-shapes in place of arguments, but only
                                   in place of sub-shapes of the first argument,
                                   because the whole shape corresponds to the first argument.
                                   Mainly to be used after transformations, but it also can be
                                   usefull after partition with one object shape, and some other
                                   operations, where only the first argument has to be considered.
                                   If theObject has only one argument shape, this flag is automatically
                                   considered as True, not regarding really passed value.
                theAddPrefix add prefix "from_" to names of restored sub-shapes,
                             and prefix "from_subshapes_of_" to names of partially restored sub-shapes.

            Returns: 
                list of published sub-shapes
            """
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
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        def MakeVertex(self, theX, theY, theZ):
            """
            Create point by three coordinates.

            Parameters:
                theX The X coordinate of the point.
                theY The Y coordinate of the point.
                theZ The Z coordinate of the point.
                
            Returns: 
                New GEOM.GEOM_Object, containing the created point.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        def MakeVertexWithRef(self,theReference, theX, theY, theZ):
            """
            Create a point, distant from the referenced point
            on the given distances along the coordinate axes.

            Parameters:
                theReference The referenced point.
                theX Displacement from the referenced point along OX axis.
                theY Displacement from the referenced point along OY axis.
                theZ Displacement from the referenced point along OZ axis.

            Returns:
                New GEOM.GEOM_Object, containing the created point.
            """
            # Example: see GEOM_TestAll.py
            theX,theY,theZ,Parameters = ParseParameters(theX, theY, theZ)
            anObj = self.BasicOp.MakePointWithReference(theReference, theX, theY, theZ)
            RaiseIfFailed("MakePointWithReference", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a point, corresponding to the given parameter on the given curve.
        #  @param theRefCurve The referenced curve.
        #  @param theParameter Value of parameter on the referenced curve.
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        def MakeVertexOnCurve(self,theRefCurve, theParameter):
            """
            Create a point, corresponding to the given parameter on the given curve.

            Parameters:
                theRefCurve The referenced curve.
                theParameter Value of parameter on the referenced curve.

            Returns:
                New GEOM.GEOM_Object, containing the created point.

            Example of usage:
                p_on_arc = geompy.MakeVertexOnCurve(Arc, 0.25)
            """
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
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        def MakeVertexOnCurveByCoord(self,theRefCurve, theX, theY, theZ):
            """
            Create a point by projection give coordinates on the given curve
            
            Parameters:
                theRefCurve The referenced curve.
                theX X-coordinate in 3D space
                theY Y-coordinate in 3D space
                theZ Z-coordinate in 3D space

            Returns:
                New GEOM.GEOM_Object, containing the created point.

            Example of usage:
                p_on_arc3 = geompy.MakeVertexOnCurveByCoord(Arc, 100, -10, 10)
            """
            # Example: see GEOM_TestAll.py
            theX, theY, theZ, Parameters = ParseParameters(theX, theY, theZ)
            anObj = self.BasicOp.MakePointOnCurveByCoord(theRefCurve, theX, theY, theZ)
            RaiseIfFailed("MakeVertexOnCurveByCoord", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a point, corresponding to the given length on the given curve.
        #  @param theRefCurve The referenced curve.
        #  @param theLength Length on the referenced curve. It can be negative.
        #  @param theStartPoint Point allowing to choose the direction for the calculation
        #                       of the length. If None, start from the first point of theRefCurve.
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        def MakeVertexOnCurveByLength(self, theRefCurve, theLength, theStartPoint = None):
            """
            Create a point, corresponding to the given length on the given curve.

            Parameters:
                theRefCurve The referenced curve.
                theLength Length on the referenced curve. It can be negative.
                theStartPoint Point allowing to choose the direction for the calculation
                              of the length. If None, start from the first point of theRefCurve.

            Returns:
                New GEOM.GEOM_Object, containing the created point.
            """
            # Example: see GEOM_TestAll.py
            theLength, Parameters = ParseParameters(theLength)
            anObj = self.BasicOp.MakePointOnCurveByLength(theRefCurve, theLength, theStartPoint)
            RaiseIfFailed("MakePointOnCurveByLength", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a point, corresponding to the given parameters on the
        #    given surface.
        #  @param theRefSurf The referenced surface.
        #  @param theUParameter Value of U-parameter on the referenced surface.
        #  @param theVParameter Value of V-parameter on the referenced surface.
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref swig_MakeVertexOnSurface "Example"
        def MakeVertexOnSurface(self, theRefSurf, theUParameter, theVParameter):
            """
            Create a point, corresponding to the given parameters on the
            given surface.

            Parameters:
                theRefSurf The referenced surface.
                theUParameter Value of U-parameter on the referenced surface.
                theVParameter Value of V-parameter on the referenced surface.

            Returns:
                New GEOM.GEOM_Object, containing the created point.

            Example of usage:
                p_on_face = geompy.MakeVertexOnSurface(Face, 0.1, 0.8)
            """
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
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref swig_MakeVertexOnSurfaceByCoord "Example"
        def MakeVertexOnSurfaceByCoord(self, theRefSurf, theX, theY, theZ):
            """
            Create a point by projection give coordinates on the given surface

            Parameters:
                theRefSurf The referenced surface.
                theX X-coordinate in 3D space
                theY Y-coordinate in 3D space
                theZ Z-coordinate in 3D space

            Returns:
                New GEOM.GEOM_Object, containing the created point.

            Example of usage:
                p_on_face2 = geompy.MakeVertexOnSurfaceByCoord(Face, 0., 0., 0.)
            """
            theX, theY, theZ, Parameters = ParseParameters(theX, theY, theZ)
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakePointOnSurfaceByCoord(theRefSurf, theX, theY, theZ)
            RaiseIfFailed("MakeVertexOnSurfaceByCoord", self.BasicOp)
            anObj.SetParameters(Parameters);
            return anObj

        ## Create a point, which lays on the given face.
        #  The point will lay in arbitrary place of the face.
        #  The only condition on it is a non-zero distance to the face boundary.
        #  Such point can be used to uniquely identify the face inside any
        #  shape in case, when the shape does not contain overlapped faces.
        #  @param theFace The referenced face.
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref swig_MakeVertexInsideFace "Example"
        def MakeVertexInsideFace (self, theFace):
            """
            Create a point, which lays on the given face.
            The point will lay in arbitrary place of the face.
            The only condition on it is a non-zero distance to the face boundary.
            Such point can be used to uniquely identify the face inside any
            shape in case, when the shape does not contain overlapped faces.

            Parameters:
                theFace The referenced face.

            Returns:
                New GEOM.GEOM_Object, containing the created point.

            Example of usage:
                p_on_face = geompy.MakeVertexInsideFace(Face)
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakePointOnFace(theFace)
            RaiseIfFailed("MakeVertexInsideFace", self.BasicOp)
            return anObj

        ## Create a point on intersection of two lines.
        #  @param theRefLine1, theRefLine2 The referenced lines.
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref swig_MakeVertexOnLinesIntersection "Example"
        def MakeVertexOnLinesIntersection(self, theRefLine1, theRefLine2):
            """
            Create a point on intersection of two lines.

            Parameters:
                theRefLine1, theRefLine2 The referenced lines.

            Returns:
                New GEOM.GEOM_Object, containing the created point.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakePointOnLinesIntersection(theRefLine1, theRefLine2)
            RaiseIfFailed("MakePointOnLinesIntersection", self.BasicOp)
            return anObj

        ## Create a tangent, corresponding to the given parameter on the given curve.
        #  @param theRefCurve The referenced curve.
        #  @param theParameter Value of parameter on the referenced curve.
        #  @return New GEOM.GEOM_Object, containing the created tangent.
        #
        #  @ref swig_MakeTangentOnCurve "Example"
        def MakeTangentOnCurve(self, theRefCurve, theParameter):
            """
            Create a tangent, corresponding to the given parameter on the given curve.

            Parameters:
                theRefCurve The referenced curve.
                theParameter Value of parameter on the referenced curve.

            Returns:
                New GEOM.GEOM_Object, containing the created tangent.

            Example of usage:
                tan_on_arc = geompy.MakeTangentOnCurve(Arc, 0.7)
            """
            anObj = self.BasicOp.MakeTangentOnCurve(theRefCurve, theParameter)
            RaiseIfFailed("MakeTangentOnCurve", self.BasicOp)
            return anObj

        ## Create a tangent plane, corresponding to the given parameter on the given face.
        #  @param theFace The face for which tangent plane should be built.
        #  @param theParameterV vertical value of the center point (0.0 - 1.0).
        #  @param theParameterU horisontal value of the center point (0.0 - 1.0).
        #  @param theTrimSize the size of plane.
        #  @return New GEOM.GEOM_Object, containing the created tangent.
        #
        #  @ref swig_MakeTangentPlaneOnFace "Example"
        def MakeTangentPlaneOnFace(self, theFace, theParameterU, theParameterV, theTrimSize):
            """
            Create a tangent plane, corresponding to the given parameter on the given face.

            Parameters:
                theFace The face for which tangent plane should be built.
                theParameterV vertical value of the center point (0.0 - 1.0).
                theParameterU horisontal value of the center point (0.0 - 1.0).
                theTrimSize the size of plane.

           Returns: 
                New GEOM.GEOM_Object, containing the created tangent.

           Example of usage:
                an_on_face = geompy.MakeTangentPlaneOnFace(tan_extrusion, 0.7, 0.5, 150)
            """
            anObj = self.BasicOp.MakeTangentPlaneOnFace(theFace, theParameterU, theParameterV, theTrimSize)
            RaiseIfFailed("MakeTangentPlaneOnFace", self.BasicOp)
            return anObj

        ## Create a vector with the given components.
        #  @param theDX X component of the vector.
        #  @param theDY Y component of the vector.
        #  @param theDZ Z component of the vector.
        #  @return New GEOM.GEOM_Object, containing the created vector.
        #
        #  @ref tui_creation_vector "Example"
        def MakeVectorDXDYDZ(self,theDX, theDY, theDZ):
            """
            Create a vector with the given components.

            Parameters:
                theDX X component of the vector.
                theDY Y component of the vector.
                theDZ Z component of the vector.

            Returns:     
                New GEOM.GEOM_Object, containing the created vector.
            """
            # Example: see GEOM_TestAll.py
            theDX,theDY,theDZ,Parameters = ParseParameters(theDX, theDY, theDZ)
            anObj = self.BasicOp.MakeVectorDXDYDZ(theDX, theDY, theDZ)
            RaiseIfFailed("MakeVectorDXDYDZ", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a vector between two points.
        #  @param thePnt1 Start point for the vector.
        #  @param thePnt2 End point for the vector.
        #  @return New GEOM.GEOM_Object, containing the created vector.
        #
        #  @ref tui_creation_vector "Example"
        def MakeVector(self,thePnt1, thePnt2):
            """
            Create a vector between two points.

            Parameters:
                thePnt1 Start point for the vector.
                thePnt2 End point for the vector.

            Returns:        
                New GEOM.GEOM_Object, containing the created vector.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeVectorTwoPnt(thePnt1, thePnt2)
            RaiseIfFailed("MakeVectorTwoPnt", self.BasicOp)
            return anObj

        ## Create a line, passing through the given point
        #  and parrallel to the given direction
        #  @param thePnt Point. The resulting line will pass through it.
        #  @param theDir Direction. The resulting line will be parallel to it.
        #  @return New GEOM.GEOM_Object, containing the created line.
        #
        #  @ref tui_creation_line "Example"
        def MakeLine(self,thePnt, theDir):
            """
            Create a line, passing through the given point
            and parrallel to the given direction

            Parameters:
                thePnt Point. The resulting line will pass through it.
                theDir Direction. The resulting line will be parallel to it.

            Returns:
                New GEOM.GEOM_Object, containing the created line.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeLine(thePnt, theDir)
            RaiseIfFailed("MakeLine", self.BasicOp)
            return anObj

        ## Create a line, passing through the given points
        #  @param thePnt1 First of two points, defining the line.
        #  @param thePnt2 Second of two points, defining the line.
        #  @return New GEOM.GEOM_Object, containing the created line.
        #
        #  @ref tui_creation_line "Example"
        def MakeLineTwoPnt(self,thePnt1, thePnt2):
            """
            Create a line, passing through the given points

            Parameters:
                thePnt1 First of two points, defining the line.
                thePnt2 Second of two points, defining the line.

            Returns:
                New GEOM.GEOM_Object, containing the created line.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeLineTwoPnt(thePnt1, thePnt2)
            RaiseIfFailed("MakeLineTwoPnt", self.BasicOp)
            return anObj

        ## Create a line on two faces intersection.
        #  @param theFace1 First of two faces, defining the line.
        #  @param theFace2 Second of two faces, defining the line.
        #  @return New GEOM.GEOM_Object, containing the created line.
        #
        #  @ref swig_MakeLineTwoFaces "Example"
        def MakeLineTwoFaces(self, theFace1, theFace2):
            """
            Create a line on two faces intersection.

            Parameters:
                theFace1 First of two faces, defining the line.
                theFace2 Second of two faces, defining the line.

            Returns:
                New GEOM.GEOM_Object, containing the created line.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeLineTwoFaces(theFace1, theFace2)
            RaiseIfFailed("MakeLineTwoFaces", self.BasicOp)
            return anObj

        ## Create a plane, passing through the given point
        #  and normal to the given vector.
        #  @param thePnt Point, the plane has to pass through.
        #  @param theVec Vector, defining the plane normal direction.
        #  @param theTrimSize Half size of a side of quadrangle face, representing the plane.
        #  @return New GEOM.GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        def MakePlane(self,thePnt, theVec, theTrimSize):
            """
            Create a plane, passing through the given point
            and normal to the given vector.

            Parameters:
                thePnt Point, the plane has to pass through.
                theVec Vector, defining the plane normal direction.
                theTrimSize Half size of a side of quadrangle face, representing the plane.

            Returns:    
                New GEOM.GEOM_Object, containing the created plane.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        def MakePlaneThreePnt(self,thePnt1, thePnt2, thePnt3, theTrimSize):
            """
            Create a plane, passing through the three given points

            Parameters:
                thePnt1 First of three points, defining the plane.
                thePnt2 Second of three points, defining the plane.
                thePnt3 Fird of three points, defining the plane.
                theTrimSize Half size of a side of quadrangle face, representing the plane.

            Returns:
                New GEOM.GEOM_Object, containing the created plane.
            """
            # Example: see GEOM_TestAll.py
            theTrimSize, Parameters = ParseParameters(theTrimSize);
            anObj = self.BasicOp.MakePlaneThreePnt(thePnt1, thePnt2, thePnt3, theTrimSize)
            RaiseIfFailed("MakePlaneThreePnt", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a plane, similar to the existing one, but with another size of representing face.
        #  @param theFace Referenced plane or LCS(Marker).
        #  @param theTrimSize New half size of a side of quadrangle face, representing the plane.
        #  @return New GEOM.GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        def MakePlaneFace(self,theFace, theTrimSize):
            """
            Create a plane, similar to the existing one, but with another size of representing face.

            Parameters:
                theFace Referenced plane or LCS(Marker).
                theTrimSize New half size of a side of quadrangle face, representing the plane.

            Returns:
                New GEOM.GEOM_Object, containing the created plane.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        def MakePlane2Vec(self,theVec1, theVec2, theTrimSize):
            """
            Create a plane, passing through the 2 vectors
            with center in a start point of the first vector.

            Parameters:
                theVec1 Vector, defining center point and plane direction.
                theVec2 Vector, defining the plane normal direction.
                theTrimSize Half size of a side of quadrangle face, representing the plane.

            Returns: 
                New GEOM.GEOM_Object, containing the created plane.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        def MakePlaneLCS(self,theLCS, theTrimSize, theOrientation):
            """
            Create a plane, based on a Local coordinate system.

           Parameters: 
                theLCS  coordinate system, defining plane.
                theTrimSize Half size of a side of quadrangle face, representing the plane.
                theOrientation OXY, OYZ or OZX orientation - (1, 2 or 3)

            Returns: 
                New GEOM.GEOM_Object, containing the created plane.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created coordinate system.
        #
        #  @ref swig_MakeMarker "Example"
        def MakeMarker(self, OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ):
            """
            Create a local coordinate system.

            Parameters: 
                OX,OY,OZ Three coordinates of coordinate system origin.
                XDX,XDY,XDZ Three components of OX direction
                YDX,YDY,YDZ Three components of OY direction

            Returns: 
                New GEOM.GEOM_Object, containing the created coordinate system.
            """
            # Example: see GEOM_TestAll.py
            OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ, Parameters = ParseParameters(OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ);
            anObj = self.BasicOp.MakeMarker(OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ)
            RaiseIfFailed("MakeMarker", self.BasicOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a local coordinate system from shape.
        #  @param theShape The initial shape to detect the coordinate system.
        #  @return New GEOM.GEOM_Object, containing the created coordinate system.
        #
        #  @ref tui_creation_lcs "Example"
        def MakeMarkerFromShape(self, theShape):
            """
            Create a local coordinate system from shape.

            Parameters:
                theShape The initial shape to detect the coordinate system.
                
            Returns: 
                New GEOM.GEOM_Object, containing the created coordinate system.
            """
            anObj = self.BasicOp.MakeMarkerFromShape(theShape)
            RaiseIfFailed("MakeMarkerFromShape", self.BasicOp)
            return anObj

        ## Create a local coordinate system from point and two vectors.
        #  @param theOrigin Point of coordinate system origin.
        #  @param theXVec Vector of X direction
        #  @param theYVec Vector of Y direction
        #  @return New GEOM.GEOM_Object, containing the created coordinate system.
        #
        #  @ref tui_creation_lcs "Example"
        def MakeMarkerPntTwoVec(self, theOrigin, theXVec, theYVec):
            """
            Create a local coordinate system from point and two vectors.

            Parameters:
                theOrigin Point of coordinate system origin.
                theXVec Vector of X direction
                theYVec Vector of Y direction

            Returns: 
                New GEOM.GEOM_Object, containing the created coordinate system.

            """
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
        #  @return New GEOM.GEOM_Object, containing the created arc.
        #
        #  @ref swig_MakeArc "Example"
        def MakeArc(self,thePnt1, thePnt2, thePnt3):
            """
            Create an arc of circle, passing through three given points.

            Parameters:
                thePnt1 Start point of the arc.
                thePnt2 Middle point of the arc.
                thePnt3 End point of the arc.

            Returns: 
                New GEOM.GEOM_Object, containing the created arc.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeArc(thePnt1, thePnt2, thePnt3)
            RaiseIfFailed("MakeArc", self.CurvesOp)
            return anObj

        ##  Create an arc of circle from a center and 2 points.
        #  @param thePnt1 Center of the arc
        #  @param thePnt2 Start point of the arc. (Gives also the radius of the arc)
        #  @param thePnt3 End point of the arc (Gives also a direction)
        #  @param theSense Orientation of the arc
        #  @return New GEOM.GEOM_Object, containing the created arc.
        #
        #  @ref swig_MakeArc "Example"
        def MakeArcCenter(self, thePnt1, thePnt2, thePnt3, theSense=False):
            """
            Create an arc of circle from a center and 2 points.

            Parameters:
                thePnt1 Center of the arc
                thePnt2 Start point of the arc. (Gives also the radius of the arc)
                thePnt3 End point of the arc (Gives also a direction)
                theSense Orientation of the arc

            Returns:
                New GEOM.GEOM_Object, containing the created arc.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeArcCenter(thePnt1, thePnt2, thePnt3, theSense)
            RaiseIfFailed("MakeArcCenter", self.CurvesOp)
            return anObj

        ##  Create an arc of ellipse, of center and two points.
        #  @param theCenter Center of the arc.
        #  @param thePnt1 defines major radius of the arc by distance from Pnt1 to Pnt2.
        #  @param thePnt2 defines plane of ellipse and minor radius as distance from Pnt3 to line from Pnt1 to Pnt2.
        #  @return New GEOM.GEOM_Object, containing the created arc.
        #
        #  @ref swig_MakeArc "Example"
        def MakeArcOfEllipse(self,theCenter, thePnt1, thePnt2):
            """
            Create an arc of ellipse, of center and two points.

            Parameters:
                theCenter Center of the arc.
                thePnt1 defines major radius of the arc by distance from Pnt1 to Pnt2.
                thePnt2 defines plane of ellipse and minor radius as distance from Pnt3 to line from Pnt1 to Pnt2.

            Returns:
                New GEOM.GEOM_Object, containing the created arc.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeArcOfEllipse(theCenter, thePnt1, thePnt2)
            RaiseIfFailed("MakeArcOfEllipse", self.CurvesOp)
            return anObj

        ## Create a circle with given center, normal vector and radius.
        #  @param thePnt Circle center.
        #  @param theVec Vector, normal to the plane of the circle.
        #  @param theR Circle radius.
        #  @return New GEOM.GEOM_Object, containing the created circle.
        #
        #  @ref tui_creation_circle "Example"
        def MakeCircle(self, thePnt, theVec, theR):
            """
            Create a circle with given center, normal vector and radius.

            Parameters:
                thePnt Circle center.
                theVec Vector, normal to the plane of the circle.
                theR Circle radius.

            Returns:
                New GEOM.GEOM_Object, containing the created circle.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created circle.
        def MakeCircleR(self, theR):
            """
            Create a circle with given radius.
            Center of the circle will be in the origin of global
            coordinate system and normal vector will be codirected with Z axis

            Parameters:
                theR Circle radius.

            Returns:
                New GEOM.GEOM_Object, containing the created circle.
            """
            anObj = self.CurvesOp.MakeCirclePntVecR(None, None, theR)
            RaiseIfFailed("MakeCirclePntVecR", self.CurvesOp)
            return anObj

        ## Create a circle, passing through three given points
        #  @param thePnt1,thePnt2,thePnt3 Points, defining the circle.
        #  @return New GEOM.GEOM_Object, containing the created circle.
        #
        #  @ref tui_creation_circle "Example"
        def MakeCircleThreePnt(self,thePnt1, thePnt2, thePnt3):
            """
            Create a circle, passing through three given points

            Parameters:
                thePnt1,thePnt2,thePnt3 Points, defining the circle.

            Returns:
                New GEOM.GEOM_Object, containing the created circle.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeCircleThreePnt(thePnt1, thePnt2, thePnt3)
            RaiseIfFailed("MakeCircleThreePnt", self.CurvesOp)
            return anObj

        ## Create a circle, with given point1 as center,
        #  passing through the point2 as radius and laying in the plane,
        #  defined by all three given points.
        #  @param thePnt1,thePnt2,thePnt3 Points, defining the circle.
        #  @return New GEOM.GEOM_Object, containing the created circle.
        #
        #  @ref swig_MakeCircle "Example"
        def MakeCircleCenter2Pnt(self,thePnt1, thePnt2, thePnt3):
            """
            Create a circle, with given point1 as center,
            passing through the point2 as radius and laying in the plane,
            defined by all three given points.

            Parameters:
                thePnt1,thePnt2,thePnt3 Points, defining the circle.

            Returns:
                New GEOM.GEOM_Object, containing the created circle.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created ellipse.
        #
        #  @ref tui_creation_ellipse "Example"
        def MakeEllipse(self, thePnt, theVec, theRMajor, theRMinor, theVecMaj=None):
            """
            Create an ellipse with given center, normal vector and radiuses.

            Parameters:
                thePnt Ellipse center.
                theVec Vector, normal to the plane of the ellipse.
                theRMajor Major ellipse radius.
                theRMinor Minor ellipse radius.
                theVecMaj Vector, direction of the ellipse's main axis.

            Returns:    
                New GEOM.GEOM_Object, containing the created ellipse.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created ellipse.
        def MakeEllipseRR(self, theRMajor, theRMinor):
            """
            Create an ellipse with given radiuses.
            Center of the ellipse will be in the origin of global
            coordinate system and normal vector will be codirected with Z axis

            Parameters:
                theRMajor Major ellipse radius.
                theRMinor Minor ellipse radius.

            Returns:
                New GEOM.GEOM_Object, containing the created ellipse.
            """
            anObj = self.CurvesOp.MakeEllipse(None, None, theRMajor, theRMinor)
            RaiseIfFailed("MakeEllipse", self.CurvesOp)
            return anObj

        ## Create a polyline on the set of points.
        #  @param thePoints Sequence of points for the polyline.
        #  @param theIsClosed If True, build a closed wire.
        #  @return New GEOM.GEOM_Object, containing the created polyline.
        #
        #  @ref tui_creation_curve "Example"
        def MakePolyline(self, thePoints, theIsClosed=False):
            """
            Create a polyline on the set of points.

            Parameters:
                thePoints Sequence of points for the polyline.
                theIsClosed If True, build a closed wire.

            Returns:
                New GEOM.GEOM_Object, containing the created polyline.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakePolyline(thePoints, theIsClosed)
            RaiseIfFailed("MakePolyline", self.CurvesOp)
            return anObj

        ## Create bezier curve on the set of points.
        #  @param thePoints Sequence of points for the bezier curve.
        #  @param theIsClosed If True, build a closed curve.
        #  @return New GEOM.GEOM_Object, containing the created bezier curve.
        #
        #  @ref tui_creation_curve "Example"
        def MakeBezier(self, thePoints, theIsClosed=False):
            """
            Create bezier curve on the set of points.

            Parameters:
                thePoints Sequence of points for the bezier curve.
                theIsClosed If True, build a closed curve.

            Returns:
                New GEOM.GEOM_Object, containing the created bezier curve.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeSplineBezier(thePoints, theIsClosed)
            RaiseIfFailed("MakeSplineBezier", self.CurvesOp)
            return anObj

        ## Create B-Spline curve on the set of points.
        #  @param thePoints Sequence of points for the B-Spline curve.
        #  @param theIsClosed If True, build a closed curve.
        #  @param theDoReordering If TRUE, the algo does not follow the order of
        #                         \a thePoints but searches for the closest vertex.
        #  @return New GEOM.GEOM_Object, containing the created B-Spline curve.
        #
        #  @ref tui_creation_curve "Example"
        def MakeInterpol(self, thePoints, theIsClosed=False, theDoReordering=False):
            """
            Create B-Spline curve on the set of points.

            Parameters:
                thePoints Sequence of points for the B-Spline curve.
                theIsClosed If True, build a closed curve.
                theDoReordering If True, the algo does not follow the order of
                                thePoints but searches for the closest vertex.

            Returns:                     
                New GEOM.GEOM_Object, containing the created B-Spline curve.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeSplineInterpolation(thePoints, theIsClosed, theDoReordering)
            RaiseIfFailed("MakeSplineInterpolation", self.CurvesOp)
            return anObj


        ## Creates a curve using the parametric definition of the basic points.
        #  @param thexExpr parametric equation of the coordinates X.
        #  @param theyExpr parametric equation of the coordinates Y.
        #  @param thezExpr parametric equation of the coordinates Z.
        #  @param theParamMin the minimal value of the parameter.
        #  @param theParamMax the maximum value of the parameter.
        #  @param theParamStep the number of steps if theNewMethod = True, else step value of the parameter.
        #  @param theCurveType the type of the curve.
        #  @param theNewMethod flag for switching to the new method if the flag is set to false a deprecated method is used which can lead to a bug.
        #  @return New GEOM.GEOM_Object, containing the created curve.
        #
        #  @ref tui_creation_curve "Example"
        def MakeCurveParametric(self, thexExpr, theyExpr, thezExpr,
                                theParamMin, theParamMax, theParamStep, theCurveType, theNewMethod=False ):
            """
            Creates a curve using the parametric definition of the basic points.

            Parameters:
                thexExpr parametric equation of the coordinates X.
                theyExpr parametric equation of the coordinates Y.
                thezExpr parametric equation of the coordinates Z.
                theParamMin the minimal value of the parameter.
                theParamMax the maximum value of the parameter.
                theParamStep the number of steps if theNewMethod = True, else step value of the parameter.
                theCurveType the type of the curve.
                theNewMethod flag for switching to the new method if the flag is set to false a deprecated
                             method is used which can lead to a bug.

            Returns:
                New GEOM.GEOM_Object, containing the created curve.
            """
            theParamMin,theParamMax,theParamStep,Parameters = ParseParameters(theParamMin,theParamMax,theParamStep)
            if theNewMethod:
              anObj = self.CurvesOp.MakeCurveParametricNew(thexExpr,theyExpr,thezExpr,theParamMin,theParamMax,theParamStep,theCurveType)
            else:
              anObj = self.CurvesOp.MakeCurveParametric(thexExpr,theyExpr,thezExpr,theParamMin,theParamMax,theParamStep,theCurveType)   
            RaiseIfFailed("MakeSplineInterpolation", self.CurvesOp)
            anObj.SetParameters(Parameters)
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
        #     - "AA x y": Create arc by point at X & Y
        #     - "A dx dy" : Create arc by point with DX & DY
        #     - "UU x y radius flag1": Create arc by point at X & Y with given radiUs
        #     - "U dx dy radius flag1" : Create arc by point with DX & DY with given radiUs
        #     - "EE x y xc yc flag1 flag2": Create arc by point at X & Y with given cEnter coordinates
        #     - "E dx dy dxc dyc radius flag1 flag2" : Create arc by point with DX & DY with given cEnter coordinates
        #     .
        #       \n
        #     - "WW" : Close Wire (to finish)
        #     - "WF" : Close Wire and build face (to finish)
        #     .
        #        \n
        #  - Flag1 (= reverse) is 0 or 2 ...
        #     - if 0 the drawn arc is the one of lower angle (< Pi)
        #     - if 2 the drawn arc ius the one of greater angle (> Pi)
        #     .
        #        \n
        #  - Flag2 (= control tolerance) is 0 or 1 ...
        #     - if 0 the specified end point can be at a distance of the arc greater than the tolerance (10^-7)
        #     - if 1 the wire is built only if the end point is on the arc
        #       with a tolerance of 10^-7 on the distance else the creation fails
        #
        #  @param theCommand String, defining the sketcher in local
        #                    coordinates of the working plane.
        #  @param theWorkingPlane Nine double values, defining origin,
        #                         OZ and OX directions of the working plane.
        #  @return New GEOM.GEOM_Object, containing the created wire.
        #
        #  @ref tui_sketcher_page "Example"
        def MakeSketcher(self, theCommand, theWorkingPlane = [0,0,0, 0,0,1, 1,0,0]):
            """
            Create a sketcher (wire or face), following the textual description, passed
            through theCommand argument.
            Edges of the resulting wire or face will be arcs of circles and/or linear segments.
            Format of the description string have to be the following:
                "Sketcher[:F x1 y1]:CMD[:CMD[:CMD...]]"
            Where:
            - x1, y1 are coordinates of the first sketcher point (zero by default),
            - CMD is one of
               - "R angle" : Set the direction by angle
               - "D dx dy" : Set the direction by DX & DY
               
               - "TT x y" : Create segment by point at X & Y
               - "T dx dy" : Create segment by point with DX & DY
               - "L length" : Create segment by direction & Length
               - "IX x" : Create segment by direction & Intersect. X
               - "IY y" : Create segment by direction & Intersect. Y

               - "C radius length" : Create arc by direction, radius and length(in degree)
               - "AA x y": Create arc by point at X & Y
               - "A dx dy" : Create arc by point with DX & DY
               - "UU x y radius flag1": Create arc by point at X & Y with given radiUs
               - "U dx dy radius flag1" : Create arc by point with DX & DY with given radiUs
               - "EE x y xc yc flag1 flag2": Create arc by point at X & Y with given cEnter coordinates
               - "E dx dy dxc dyc radius flag1 flag2" : Create arc by point with DX & DY with given cEnter coordinates

               - "WW" : Close Wire (to finish)
               - "WF" : Close Wire and build face (to finish)
            
            - Flag1 (= reverse) is 0 or 2 ...
               - if 0 the drawn arc is the one of lower angle (< Pi)
               - if 2 the drawn arc ius the one of greater angle (> Pi)
        
            - Flag2 (= control tolerance) is 0 or 1 ...
               - if 0 the specified end point can be at a distance of the arc greater than the tolerance (10^-7)
               - if 1 the wire is built only if the end point is on the arc
                 with a tolerance of 10^-7 on the distance else the creation fails

            Parameters:
                theCommand String, defining the sketcher in local
                           coordinates of the working plane.
                theWorkingPlane Nine double values, defining origin,
                                OZ and OX directions of the working plane.

            Returns:
                New GEOM.GEOM_Object, containing the created wire.
            """
            # Example: see GEOM_TestAll.py
            theCommand,Parameters = ParseSketcherCommand(theCommand)
            anObj = self.CurvesOp.MakeSketcher(theCommand, theWorkingPlane)
            RaiseIfFailed("MakeSketcher", self.CurvesOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a sketcher (wire or face), following the textual description,
        #  passed through <VAR>theCommand</VAR> argument. \n
        #  For format of the description string see MakeSketcher() method.\n
        #  @param theCommand String, defining the sketcher in local
        #                    coordinates of the working plane.
        #  @param theWorkingPlane Planar Face or LCS(Marker) of the working plane.
        #  @return New GEOM.GEOM_Object, containing the created wire.
        #
        #  @ref tui_sketcher_page "Example"
        def MakeSketcherOnPlane(self, theCommand, theWorkingPlane):
            """
            Create a sketcher (wire or face), following the textual description,
            passed through theCommand argument.
            For format of the description string see geompy.MakeSketcher() method.

            Parameters:
                theCommand String, defining the sketcher in local
                           coordinates of the working plane.
                theWorkingPlane Planar Face or LCS(Marker) of the working plane.

            Returns:
                New GEOM.GEOM_Object, containing the created wire.
            """
            theCommand,Parameters = ParseSketcherCommand(theCommand)
            anObj = self.CurvesOp.MakeSketcherOnPlane(theCommand, theWorkingPlane)
            RaiseIfFailed("MakeSketcherOnPlane", self.CurvesOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a sketcher wire, following the numerical description,
        #  passed through <VAR>theCoordinates</VAR> argument. \n
        #  @param theCoordinates double values, defining points to create a wire,
        #                                                      passing from it.
        #  @return New GEOM.GEOM_Object, containing the created wire.
        #
        #  @ref tui_3dsketcher_page "Example"
        def Make3DSketcher(self, theCoordinates):
            """
            Create a sketcher wire, following the numerical description,
            passed through theCoordinates argument.

            Parameters:
                theCoordinates double values, defining points to create a wire,
                               passing from it.

            Returns:
                New GEOM_Object, containing the created wire.
            """
            theCoordinates,Parameters = ParseParameters(theCoordinates)
            anObj = self.CurvesOp.Make3DSketcher(theCoordinates)
            RaiseIfFailed("Make3DSketcher", self.CurvesOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Obtain a 3D sketcher interface
        #  @return An instance of @ref gsketcher.Sketcher3D "Sketcher3D" interface
        #
        #  @ref tui_3dsketcher_page "Example"
        def Sketcher3D (self):
            """
            Obtain a 3D sketcher interface.

            Example of usage:
                sk = geompy.Sketcher3D()
                sk.addPointsAbsolute(0,0,0, 70,0,0)
                sk.addPointsRelative(0, 0, 130)
                sk.addPointAnglesLength("OXY", 50, 0, 100)
                sk.addPointAnglesLength("OXZ", 30, 80, 130)
                sk.close()
                a3D_Sketcher_1 = sk.wire()
            """
            sk = Sketcher3D (self)
            return sk

        # end of l3_sketcher
        ## @}

        ## @addtogroup l3_3d_primitives
        ## @{

        ## Create a box by coordinates of two opposite vertices.
        #
        #  @param x1,y1,z1 double values, defining first point it.
        #  @param x2,y2,z2 double values, defining first point it.
        #
        #  @return New GEOM.GEOM_Object, containing the created box.
        #  @ref tui_creation_box "Example"
        def MakeBox (self, x1,y1,z1, x2,y2,z2):
            """
            Create a box by coordinates of two opposite vertices.
            
            Parameters:
                x1,y1,z1 double values, defining first point.
                x2,y2,z2 double values, defining second point.
                
            Returns:
                New GEOM.GEOM_Object, containing the created box.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created box.
        #
        #  @ref tui_creation_box "Example"
        def MakeBoxDXDYDZ(self,theDX, theDY, theDZ):
            """
            Create a box with specified dimensions along the coordinate axes
            and with edges, parallel to the coordinate axes.
            Center of the box will be at point (DX/2, DY/2, DZ/2).

            Parameters:
                theDX Length of Box edges, parallel to OX axis.
                theDY Length of Box edges, parallel to OY axis.
                theDZ Length of Box edges, parallel to OZ axis.

            Returns:   
                New GEOM.GEOM_Object, containing the created box.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created box.
        #
        #  @ref tui_creation_box "Example"
        def MakeBoxTwoPnt(self,thePnt1, thePnt2):
            """
            Create a box with two specified opposite vertices,
            and with edges, parallel to the coordinate axes

            Parameters:
                thePnt1 First of two opposite vertices.
                thePnt2 Second of two opposite vertices.

            Returns:
                New GEOM.GEOM_Object, containing the created box.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakeBoxTwoPnt(thePnt1, thePnt2)
            RaiseIfFailed("MakeBoxTwoPnt", self.PrimOp)
            return anObj

        ## Create a face with specified dimensions with edges parallel to coordinate axes.
        #  @param theH height of Face.
        #  @param theW width of Face.
        #  @param theOrientation face orientation: 1-OXY, 2-OYZ, 3-OZX
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_face "Example"
        def MakeFaceHW(self,theH, theW, theOrientation):
            """
            Create a face with specified dimensions with edges parallel to coordinate axes.

            Parameters:
                theH height of Face.
                theW width of Face.
                theOrientation face orientation: 1-OXY, 2-OYZ, 3-OZX

            Returns:
                New GEOM.GEOM_Object, containing the created face.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_face "Example"
        def MakeFaceObjHW(self, theObj, theH, theW):
            """
            Create a face from another plane and two sizes,
            vertical size and horisontal size.

            Parameters:
                theObj   Normale vector to the creating face or
                         the face object.
                theH     Height (vertical size).
                theW     Width (horisontal size).

            Returns:
                New GEOM_Object, containing the created face.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created disk.
        #
        #  @ref tui_creation_disk "Example"
        def MakeDiskPntVecR(self,thePnt, theVec, theR):
            """
            Create a disk with given center, normal vector and radius.

            Parameters:
                thePnt Disk center.
                theVec Vector, normal to the plane of the disk.
                theR Disk radius.

            Returns:    
                New GEOM.GEOM_Object, containing the created disk.
            """
            # Example: see GEOM_TestAll.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.PrimOp.MakeDiskPntVecR(thePnt, theVec, theR)
            RaiseIfFailed("MakeDiskPntVecR", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a disk, passing through three given points
        #  @param thePnt1,thePnt2,thePnt3 Points, defining the disk.
        #  @return New GEOM.GEOM_Object, containing the created disk.
        #
        #  @ref tui_creation_disk "Example"
        def MakeDiskThreePnt(self,thePnt1, thePnt2, thePnt3):
            """
            Create a disk, passing through three given points

            Parameters:
                thePnt1,thePnt2,thePnt3 Points, defining the disk.

            Returns:    
                New GEOM.GEOM_Object, containing the created disk.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakeDiskThreePnt(thePnt1, thePnt2, thePnt3)
            RaiseIfFailed("MakeDiskThreePnt", self.PrimOp)
            return anObj

        ## Create a disk with specified dimensions along OX-OY coordinate axes.
        #  @param theR Radius of Face.
        #  @param theOrientation set the orientation belong axis OXY or OYZ or OZX
        #  @return New GEOM.GEOM_Object, containing the created disk.
        #
        #  @ref tui_creation_face "Example"
        def MakeDiskR(self,theR, theOrientation):
            """
            Create a disk with specified dimensions along OX-OY coordinate axes.

            Parameters:
                theR Radius of Face.
                theOrientation set the orientation belong axis OXY or OYZ or OZX

            Returns: 
                New GEOM.GEOM_Object, containing the created disk.

            Example of usage:
                Disk3 = geompy.MakeDiskR(100., 1)
            """
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
        #  @return New GEOM.GEOM_Object, containing the created cylinder.
        #
        #  @ref tui_creation_cylinder "Example"
        def MakeCylinder(self,thePnt, theAxis, theR, theH):
            """
            Create a cylinder with given base point, axis, radius and height.

            Parameters:
                thePnt Central point of cylinder base.
                theAxis Cylinder axis.
                theR Cylinder radius.
                theH Cylinder height.

            Returns: 
                New GEOM.GEOM_Object, containing the created cylinder.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created cylinder.
        #
        #  @ref tui_creation_cylinder "Example"
        def MakeCylinderRH(self,theR, theH):
            """
            Create a cylinder with given radius and height at
            the origin of coordinate system. Axis of the cylinder
            will be collinear to the OZ axis of the coordinate system.

            Parameters:
                theR Cylinder radius.
                theH Cylinder height.

            Returns:    
                New GEOM.GEOM_Object, containing the created cylinder.
            """
            # Example: see GEOM_TestAll.py
            theR,theH,Parameters = ParseParameters(theR, theH)
            anObj = self.PrimOp.MakeCylinderRH(theR, theH)
            RaiseIfFailed("MakeCylinderRH", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a sphere with given center and radius.
        #  @param thePnt Sphere center.
        #  @param theR Sphere radius.
        #  @return New GEOM.GEOM_Object, containing the created sphere.
        #
        #  @ref tui_creation_sphere "Example"
        def MakeSpherePntR(self, thePnt, theR):
            """
            Create a sphere with given center and radius.

            Parameters:
                thePnt Sphere center.
                theR Sphere radius.

            Returns:    
                New GEOM.GEOM_Object, containing the created sphere.            
            """
            # Example: see GEOM_TestAll.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.PrimOp.MakeSpherePntR(thePnt, theR)
            RaiseIfFailed("MakeSpherePntR", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a sphere with given center and radius.
        #  @param x,y,z Coordinates of sphere center.
        #  @param theR Sphere radius.
        #  @return New GEOM.GEOM_Object, containing the created sphere.
        #
        #  @ref tui_creation_sphere "Example"
        def MakeSphere(self, x, y, z, theR):
            """
            Create a sphere with given center and radius.

            Parameters: 
                x,y,z Coordinates of sphere center.
                theR Sphere radius.

            Returns:
                New GEOM.GEOM_Object, containing the created sphere.
            """
            # Example: see GEOM_TestAll.py
            point = self.MakeVertex(x, y, z)
            anObj = self.MakeSpherePntR(point, theR)
            return anObj

        ## Create a sphere with given radius at the origin of coordinate system.
        #  @param theR Sphere radius.
        #  @return New GEOM.GEOM_Object, containing the created sphere.
        #
        #  @ref tui_creation_sphere "Example"
        def MakeSphereR(self, theR):
            """
            Create a sphere with given radius at the origin of coordinate system.

            Parameters: 
                theR Sphere radius.

            Returns:
                New GEOM.GEOM_Object, containing the created sphere.            
            """
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
        #  @return New GEOM.GEOM_Object, containing the created cone.
        #
        #  @ref tui_creation_cone "Example"
        def MakeCone(self,thePnt, theAxis, theR1, theR2, theH):
            """
            Create a cone with given base point, axis, height and radiuses.

            Parameters: 
                thePnt Central point of the first cone base.
                theAxis Cone axis.
                theR1 Radius of the first cone base.
                theR2 Radius of the second cone base.
                theH Cone height.

            Note:
                If both radiuses are non-zero, the cone will be truncated.
                If the radiuses are equal, a cylinder will be created instead.

            Returns:
                New GEOM.GEOM_Object, containing the created cone.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created cone.
        #
        #  @ref tui_creation_cone "Example"
        def MakeConeR1R2H(self,theR1, theR2, theH):
            """
            Create a cone with given height and radiuses at
            the origin of coordinate system. Axis of the cone will
            be collinear to the OZ axis of the coordinate system.

            Parameters: 
                theR1 Radius of the first cone base.
                theR2 Radius of the second cone base.
                theH Cone height.

            Note:
                If both radiuses are non-zero, the cone will be truncated.
                If the radiuses are equal, a cylinder will be created instead.

            Returns:
                New GEOM.GEOM_Object, containing the created cone.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created torus.
        #
        #  @ref tui_creation_torus "Example"
        def MakeTorus(self, thePnt, theVec, theRMajor, theRMinor):
            """
            Create a torus with given center, normal vector and radiuses.

            Parameters: 
                thePnt Torus central point.
                theVec Torus axis of symmetry.
                theRMajor Torus major radius.
                theRMinor Torus minor radius.

           Returns:
                New GEOM.GEOM_Object, containing the created torus.
            """
            # Example: see GEOM_TestAll.py
            theRMajor,theRMinor,Parameters = ParseParameters(theRMajor,theRMinor)
            anObj = self.PrimOp.MakeTorusPntVecRR(thePnt, theVec, theRMajor, theRMinor)
            RaiseIfFailed("MakeTorusPntVecRR", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a torus with given radiuses at the origin of coordinate system.
        #  @param theRMajor Torus major radius.
        #  @param theRMinor Torus minor radius.
        #  @return New GEOM.GEOM_Object, containing the created torus.
        #
        #  @ref tui_creation_torus "Example"
        def MakeTorusRR(self, theRMajor, theRMinor):
            """
           Create a torus with given radiuses at the origin of coordinate system.

           Parameters: 
                theRMajor Torus major radius.
                theRMinor Torus minor radius.

           Returns:
                New GEOM.GEOM_Object, containing the created torus.            
            """
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
        #  @param theScaleFactor Use it to make prism with scaled second base.
        #                        Nagative value means not scaled second base.
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        def MakePrism(self, theBase, thePoint1, thePoint2, theScaleFactor = -1.0):
            """
            Create a shape by extrusion of the base shape along a vector, defined by two points.

            Parameters: 
                theBase Base shape to be extruded.
                thePoint1 First end of extrusion vector.
                thePoint2 Second end of extrusion vector.
                theScaleFactor Use it to make prism with scaled second base.
                               Nagative value means not scaled second base.

            Returns:
                New GEOM.GEOM_Object, containing the created prism.
            """
            # Example: see GEOM_TestAll.py
            anObj = None
            Parameters = ""
            if theScaleFactor > 0:
                theScaleFactor,Parameters = ParseParameters(theScaleFactor)
                anObj = self.PrimOp.MakePrismTwoPntWithScaling(theBase, thePoint1, thePoint2, theScaleFactor)
            else:
                anObj = self.PrimOp.MakePrismTwoPnt(theBase, thePoint1, thePoint2)
            RaiseIfFailed("MakePrismTwoPnt", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a shape by extrusion of the base shape along a
        #  vector, defined by two points, in 2 Ways (forward/backward).
        #  @param theBase Base shape to be extruded.
        #  @param thePoint1 First end of extrusion vector.
        #  @param thePoint2 Second end of extrusion vector.
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        def MakePrism2Ways(self, theBase, thePoint1, thePoint2):
            """
            Create a shape by extrusion of the base shape along a
            vector, defined by two points, in 2 Ways (forward/backward).

            Parameters: 
                theBase Base shape to be extruded.
                thePoint1 First end of extrusion vector.
                thePoint2 Second end of extrusion vector.

            Returns:
                New GEOM.GEOM_Object, containing the created prism.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakePrismTwoPnt2Ways(theBase, thePoint1, thePoint2)
            RaiseIfFailed("MakePrismTwoPnt", self.PrimOp)
            return anObj

        ## Create a shape by extrusion of the base shape along the vector,
        #  i.e. all the space, transfixed by the base shape during its translation
        #  along the vector on the given distance.
        #  @param theBase Base shape to be extruded.
        #  @param theVec Direction of extrusion.
        #  @param theH Prism dimension along theVec.
        #  @param theScaleFactor Use it to make prism with scaled second base.
        #                        Negative value means not scaled second base.
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        def MakePrismVecH(self, theBase, theVec, theH, theScaleFactor = -1.0):
            """
            Create a shape by extrusion of the base shape along the vector,
            i.e. all the space, transfixed by the base shape during its translation
            along the vector on the given distance.

            Parameters: 
                theBase Base shape to be extruded.
                theVec Direction of extrusion.
                theH Prism dimension along theVec.
                theScaleFactor Use it to make prism with scaled second base.
                               Negative value means not scaled second base.

            Returns:
                New GEOM.GEOM_Object, containing the created prism.
            """
            # Example: see GEOM_TestAll.py
            anObj = None
            Parameters = ""
            if theScaleFactor > 0:
                theH,theScaleFactor,Parameters = ParseParameters(theH,theScaleFactor)
                anObj = self.PrimOp.MakePrismVecHWithScaling(theBase, theVec, theH, theScaleFactor)
            else:
                theH,Parameters = ParseParameters(theH)
                anObj = self.PrimOp.MakePrismVecH(theBase, theVec, theH)
            RaiseIfFailed("MakePrismVecH", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a shape by extrusion of the base shape along the vector,
        #  i.e. all the space, transfixed by the base shape during its translation
        #  along the vector on the given distance in 2 Ways (forward/backward).
        #  @param theBase Base shape to be extruded.
        #  @param theVec Direction of extrusion.
        #  @param theH Prism dimension along theVec in forward direction.
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        def MakePrismVecH2Ways(self, theBase, theVec, theH):
            """
            Create a shape by extrusion of the base shape along the vector,
            i.e. all the space, transfixed by the base shape during its translation
            along the vector on the given distance in 2 Ways (forward/backward).

            Parameters:
                theBase Base shape to be extruded.
                theVec Direction of extrusion.
                theH Prism dimension along theVec in forward direction.

            Returns:
                New GEOM.GEOM_Object, containing the created prism.
            """
            # Example: see GEOM_TestAll.py
            theH,Parameters = ParseParameters(theH)
            anObj = self.PrimOp.MakePrismVecH2Ways(theBase, theVec, theH)
            RaiseIfFailed("MakePrismVecH2Ways", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a shape by extrusion of the base shape along the dx, dy, dz direction
        #  @param theBase Base shape to be extruded.
        #  @param theDX, theDY, theDZ Directions of extrusion.
        #  @param theScaleFactor Use it to make prism with scaled second base.
        #                        Nagative value means not scaled second base.
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        def MakePrismDXDYDZ(self, theBase, theDX, theDY, theDZ, theScaleFactor = -1.0):
            """
            Create a shape by extrusion of the base shape along the dx, dy, dz direction

            Parameters:
                theBase Base shape to be extruded.
                theDX, theDY, theDZ Directions of extrusion.
                theScaleFactor Use it to make prism with scaled second base.
                               Nagative value means not scaled second base.

            Returns: 
                New GEOM.GEOM_Object, containing the created prism.
            """
            # Example: see GEOM_TestAll.py
            anObj = None
            Parameters = ""
            if theScaleFactor > 0:
                theDX,theDY,theDZ,theScaleFactor,Parameters = ParseParameters(theDX, theDY, theDZ, theScaleFactor)
                anObj = self.PrimOp.MakePrismDXDYDZWithScaling(theBase, theDX, theDY, theDZ, theScaleFactor)
            else:
                theDX,theDY,theDZ,Parameters = ParseParameters(theDX, theDY, theDZ)
                anObj = self.PrimOp.MakePrismDXDYDZ(theBase, theDX, theDY, theDZ)
            RaiseIfFailed("MakePrismDXDYDZ", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a shape by extrusion of the base shape along the dx, dy, dz direction
        #  i.e. all the space, transfixed by the base shape during its translation
        #  along the vector on the given distance in 2 Ways (forward/backward).
        #  @param theBase Base shape to be extruded.
        #  @param theDX, theDY, theDZ Directions of extrusion.
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        def MakePrismDXDYDZ2Ways(self, theBase, theDX, theDY, theDZ):
            """
            Create a shape by extrusion of the base shape along the dx, dy, dz direction
            i.e. all the space, transfixed by the base shape during its translation
            along the vector on the given distance in 2 Ways (forward/backward).

            Parameters:
                theBase Base shape to be extruded.
                theDX, theDY, theDZ Directions of extrusion.

            Returns:
                New GEOM.GEOM_Object, containing the created prism.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created revolution.
        #
        #  @ref tui_creation_revolution "Example"
        def MakeRevolution(self, theBase, theAxis, theAngle):
            """
            Create a shape by revolution of the base shape around the axis
            on the given angle, i.e. all the space, transfixed by the base
            shape during its rotation around the axis on the given angle.

            Parameters:
                theBase Base shape to be rotated.
                theAxis Rotation axis.
                theAngle Rotation angle in radians.

            Returns: 
                New GEOM.GEOM_Object, containing the created revolution.
            """
            # Example: see GEOM_TestAll.py
            theAngle,Parameters = ParseParameters(theAngle)
            anObj = self.PrimOp.MakeRevolutionAxisAngle(theBase, theAxis, theAngle)
            RaiseIfFailed("MakeRevolutionAxisAngle", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a shape by revolution of the base shape around the axis
        #  on the given angle, i.e. all the space, transfixed by the base
        #  shape during its rotation around the axis on the given angle in
        #  both directions (forward/backward)
        #  @param theBase Base shape to be rotated.
        #  @param theAxis Rotation axis.
        #  @param theAngle Rotation angle in radians.
        #  @return New GEOM.GEOM_Object, containing the created revolution.
        #
        #  @ref tui_creation_revolution "Example"
        def MakeRevolution2Ways(self, theBase, theAxis, theAngle):
            """
            Create a shape by revolution of the base shape around the axis
            on the given angle, i.e. all the space, transfixed by the base
            shape during its rotation around the axis on the given angle in
            both directions (forward/backward).

            Parameters:
                theBase Base shape to be rotated.
                theAxis Rotation axis.
                theAngle Rotation angle in radians.

            Returns: 
                New GEOM.GEOM_Object, containing the created revolution.
            """
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
        #  @param theMethod Kind of method to perform filling operation(see GEOM::filling_oper_method())
        #  @param isApprox if True, BSpline curves are generated in the process
        #                  of surface construction. By default it is False, that means
        #                  the surface is created using given curves. The usage of
        #                  Approximation makes the algorithm work slower, but allows
        #                  building the surface for rather complex cases.
        #  @return New GEOM.GEOM_Object, containing the created filling surface.
        #
        #  @ref tui_creation_filling "Example"
        def MakeFilling(self, theShape, theMinDeg=2, theMaxDeg=5, theTol2D=0.0001,
                        theTol3D=0.0001, theNbIter=0, theMethod=GEOM.FOM_Default, isApprox=0):
            """
            Create a filling from the given compound of contours.

            Parameters:
                theShape the compound of contours
                theMinDeg a minimal degree of BSpline surface to create
                theMaxDeg a maximal degree of BSpline surface to create
                theTol2D a 2d tolerance to be reached
                theTol3D a 3d tolerance to be reached
                theNbIter a number of iteration of approximation algorithm
                theMethod Kind of method to perform filling operation(see GEOM::filling_oper_method())
                isApprox if True, BSpline curves are generated in the process
                         of surface construction. By default it is False, that means
                         the surface is created using given curves. The usage of
                         Approximation makes the algorithm work slower, but allows
                         building the surface for rather complex cases

            Returns: 
                New GEOM.GEOM_Object, containing the created filling surface.

            Example of usage:
                filling = geompy.MakeFilling(compound, 2, 5, 0.0001, 0.0001, 5)
            """
            # Example: see GEOM_TestAll.py
            theMinDeg,theMaxDeg,theTol2D,theTol3D,theNbIter,Parameters = ParseParameters(theMinDeg, theMaxDeg, theTol2D, theTol3D, theNbIter)
            anObj = self.PrimOp.MakeFilling(theShape, theMinDeg, theMaxDeg,
                                            theTol2D, theTol3D, theNbIter,
                                            theMethod, isApprox)
            RaiseIfFailed("MakeFilling", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj


        ## Create a filling from the given compound of contours.
        #  This method corresponds to MakeFilling with isApprox=True
        #  @param theShape the compound of contours
        #  @param theMinDeg a minimal degree of BSpline surface to create
        #  @param theMaxDeg a maximal degree of BSpline surface to create
        #  @param theTol3D a 3d tolerance to be reached
        #  @return New GEOM.GEOM_Object, containing the created filling surface.
        #
        #  @ref tui_creation_filling "Example"
        def MakeFillingNew(self, theShape, theMinDeg=2, theMaxDeg=5, theTol3D=0.0001):
            """
            Create a filling from the given compound of contours.
            This method corresponds to MakeFilling with isApprox=True

            Parameters:
                theShape the compound of contours
                theMinDeg a minimal degree of BSpline surface to create
                theMaxDeg a maximal degree of BSpline surface to create
                theTol3D a 3d tolerance to be reached

            Returns: 
                New GEOM.GEOM_Object, containing the created filling surface.

            Example of usage:
                filling = geompy.MakeFillingNew(compound, 2, 5, 0.0001)
            """
            # Example: see GEOM_TestAll.py
            theMinDeg,theMaxDeg,theTol3D,Parameters = ParseParameters(theMinDeg, theMaxDeg, theTol3D)
            anObj = self.PrimOp.MakeFilling(theShape, theMinDeg, theMaxDeg,
                                            0, theTol3D, 0, GEOM.FOM_Default, True)
            RaiseIfFailed("MakeFillingNew", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create a shell or solid passing through set of sections.Sections should be wires,edges or vertices.
        #  @param theSeqSections - set of specified sections.
        #  @param theModeSolid - mode defining building solid or shell
        #  @param thePreci - precision 3D used for smoothing
        #  @param theRuled - mode defining type of the result surfaces (ruled or smoothed).
        #  @return New GEOM.GEOM_Object, containing the created shell or solid.
        #
        #  @ref swig_todo "Example"
        def MakeThruSections(self,theSeqSections,theModeSolid,thePreci,theRuled):
            """
            Create a shell or solid passing through set of sections.Sections should be wires,edges or vertices.

            Parameters:
                theSeqSections - set of specified sections.
                theModeSolid - mode defining building solid or shell
                thePreci - precision 3D used for smoothing
                theRuled - mode defining type of the result surfaces (ruled or smoothed).

            Returns:
                New GEOM.GEOM_Object, containing the created shell or solid.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakeThruSections(theSeqSections,theModeSolid,thePreci,theRuled)
            RaiseIfFailed("MakeThruSections", self.PrimOp)
            return anObj

        ## Create a shape by extrusion of the base shape along
        #  the path shape. The path shape can be a wire or an edge.
        #  @param theBase Base shape to be extruded.
        #  @param thePath Path shape to extrude the base shape along it.
        #  @return New GEOM.GEOM_Object, containing the created pipe.
        #
        #  @ref tui_creation_pipe "Example"
        def MakePipe(self,theBase, thePath):
            """
            Create a shape by extrusion of the base shape along
            the path shape. The path shape can be a wire or an edge.

            Parameters:
                theBase Base shape to be extruded.
                thePath Path shape to extrude the base shape along it.

            Returns:
                New GEOM.GEOM_Object, containing the created pipe.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created pipe.
        #
        #  @ref tui_creation_pipe_with_diff_sec "Example"
        def MakePipeWithDifferentSections(self, theSeqBases,
                                          theLocations, thePath,
                                          theWithContact, theWithCorrection):
            """
            Create a shape by extrusion of the profile shape along
            the path shape. The path shape can be a wire or an edge.
            the several profiles can be specified in the several locations of path.

            Parameters:
                theSeqBases - list of  Bases shape to be extruded.
                theLocations - list of locations on the path corresponding
                               specified list of the Bases shapes. Number of locations
                               should be equal to number of bases or list of locations can be empty.
                thePath - Path shape to extrude the base shape along it.
                theWithContact - the mode defining that the section is translated to be in
                                 contact with the spine(0/1)
                theWithCorrection - defining that the section is rotated to be
                                    orthogonal to the spine tangent in the correspondent point (0/1)

            Returns:
                New GEOM.GEOM_Object, containing the created pipe.
            """
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
        #  @param theSeqSubBases - list of corresponding sub-shapes of section shapes.
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
        #  @return New GEOM.GEOM_Object, containing the created solids.
        #
        #  @ref tui_creation_pipe_with_shell_sec "Example"
        def MakePipeWithShellSections(self,theSeqBases, theSeqSubBases,
                                      theLocations, thePath,
                                      theWithContact, theWithCorrection):
            """
            Create a shape by extrusion of the profile shape along
            the path shape. The path shape can be a wire or a edge.
            the several profiles can be specified in the several locations of path.

            Parameters:
                theSeqBases - list of  Bases shape to be extruded. Base shape must be
                              shell or face. If number of faces in neighbour sections
                              aren't coincided result solid between such sections will
                              be created using external boundaries of this shells.
                theSeqSubBases - list of corresponding sub-shapes of section shapes.
                                 This list is used for searching correspondences between
                                 faces in the sections. Size of this list must be equal
                                 to size of list of base shapes.
                theLocations - list of locations on the path corresponding
                               specified list of the Bases shapes. Number of locations
                               should be equal to number of bases. First and last
                               locations must be coincided with first and last vertexes
                               of path correspondingly.
                thePath - Path shape to extrude the base shape along it.
                theWithContact - the mode defining that the section is translated to be in
                                 contact with the spine (0/1)
                theWithCorrection - defining that the section is rotated to be
                                    orthogonal to the spine tangent in the correspondent point (0/1)

            Returns:                           
                New GEOM.GEOM_Object, containing the created solids.
            """
            anObj = self.PrimOp.MakePipeWithShellSections(theSeqBases, theSeqSubBases,
                                                          theLocations, thePath,
                                                          theWithContact, theWithCorrection)
            RaiseIfFailed("MakePipeWithShellSections", self.PrimOp)
            return anObj

        ## Create a shape by extrusion of the profile shape along
        #  the path shape. This function is used only for debug pipe
        #  functionality - it is a version of function MakePipeWithShellSections()
        #  which give a possibility to recieve information about
        #  creating pipe between each pair of sections step by step.
        def MakePipeWithShellSectionsBySteps(self, theSeqBases, theSeqSubBases,
                                             theLocations, thePath,
                                             theWithContact, theWithCorrection):
            """
            Create a shape by extrusion of the profile shape along
            the path shape. This function is used only for debug pipe
            functionality - it is a version of previous function
            geompy.MakePipeWithShellSections() which give a possibility to
            recieve information about creating pipe between each pair of
            sections step by step.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created solids.
        #
        #  @ref tui_creation_pipe_without_path "Example"
        def MakePipeShellsWithoutPath(self, theSeqBases, theLocations):
            """
            Create solids between given sections

            Parameters:
                theSeqBases - list of sections (shell or face).
                theLocations - list of corresponding vertexes

            Returns:
                New GEOM.GEOM_Object, containing the created solids.
            """
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
        #  @return New GEOM.GEOM_Object, containing the created pipe.
        #
        #  @ref tui_creation_pipe "Example"
        def MakePipeBiNormalAlongVector(self,theBase, thePath, theVec):
            """
            Create a shape by extrusion of the base shape along
            the path shape with constant bi-normal direction along the given vector.
            The path shape can be a wire or an edge.

            Parameters:
                theBase Base shape to be extruded.
                thePath Path shape to extrude the base shape along it.
                theVec Vector defines a constant binormal direction to keep the
                       same angle beetween the direction and the sections
                       along the sweep surface.

            Returns:              
                New GEOM.GEOM_Object, containing the created pipe.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakePipeBiNormalAlongVector(theBase, thePath, theVec)
            RaiseIfFailed("MakePipeBiNormalAlongVector", self.PrimOp)
            return anObj

        ## Build a middle path of a pipe-like shape.
        #  The path shape can be a wire or an edge.
        #  @param theShape It can be closed or unclosed pipe-like shell
        #                  or a pipe-like solid.
        #  @param theBase1, theBase2 Two bases of the supposed pipe. This
        #                            should be wires or faces of theShape.
        #  @note It is not assumed that exact or approximate copy of theShape
        #        can be obtained by applying existing Pipe operation on the
        #        resulting "Path" wire taking theBase1 as the base - it is not
        #        always possible; though in some particular cases it might work
        #        it is not guaranteed. Thus, RestorePath function should not be
        #        considered as an exact reverse operation of the Pipe.
        #  @return New GEOM.GEOM_Object, containing an edge or wire that represent
        #                                source pipe's "path".
        #
        #  @ref tui_creation_pipe_path "Example"
        def RestorePath (self, theShape, theBase1, theBase2):
            """
            Build a middle path of a pipe-like shape.
            The path shape can be a wire or an edge.

            Parameters:
                theShape It can be closed or unclosed pipe-like shell
                         or a pipe-like solid.
                theBase1, theBase2 Two bases of the supposed pipe. This
                                   should be wires or faces of theShape.

            Returns:
                New GEOM_Object, containing an edge or wire that represent
                                 source pipe's path.
            """
            anObj = self.PrimOp.RestorePath(theShape, theBase1, theBase2)
            RaiseIfFailed("RestorePath", self.PrimOp)
            return anObj

        ## Build a middle path of a pipe-like shape.
        #  The path shape can be a wire or an edge.
        #  @param theShape It can be closed or unclosed pipe-like shell
        #                  or a pipe-like solid.
        #  @param listEdges1, listEdges2 Two bases of the supposed pipe. This
        #                                should be lists of edges of theShape.
        #  @note It is not assumed that exact or approximate copy of theShape
        #        can be obtained by applying existing Pipe operation on the
        #        resulting "Path" wire taking theBase1 as the base - it is not
        #        always possible; though in some particular cases it might work
        #        it is not guaranteed. Thus, RestorePath function should not be
        #        considered as an exact reverse operation of the Pipe.
        #  @return New GEOM.GEOM_Object, containing an edge or wire that represent
        #                                source pipe's "path".
        #
        #  @ref tui_creation_pipe_path "Example"
        def RestorePathEdges (self, theShape, listEdges1, listEdges2):
            """
            Build a middle path of a pipe-like shape.
            The path shape can be a wire or an edge.

            Parameters:
                theShape It can be closed or unclosed pipe-like shell
                         or a pipe-like solid.
                listEdges1, listEdges2 Two bases of the supposed pipe. This
                                       should be lists of edges of theShape.

            Returns:
                New GEOM_Object, containing an edge or wire that represent
                                 source pipe's path.
            """
            anObj = self.PrimOp.RestorePathEdges(theShape, listEdges1, listEdges2)
            RaiseIfFailed("RestorePath", self.PrimOp)
            return anObj

        # end of l3_complex
        ## @}

        ## @addtogroup l3_advanced
        ## @{

        ## Create a linear edge with specified ends.
        #  @param thePnt1 Point for the first end of edge.
        #  @param thePnt2 Point for the second end of edge.
        #  @return New GEOM.GEOM_Object, containing the created edge.
        #
        #  @ref tui_creation_edge "Example"
        def MakeEdge(self,thePnt1, thePnt2):
            """
            Create a linear edge with specified ends.

            Parameters:
                thePnt1 Point for the first end of edge.
                thePnt2 Point for the second end of edge.

            Returns:           
                New GEOM.GEOM_Object, containing the created edge.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeEdge(thePnt1, thePnt2)
            RaiseIfFailed("MakeEdge", self.ShapesOp)
            return anObj

        ## Create a new edge, corresponding to the given length on the given curve.
        #  @param theRefCurve The referenced curve (edge).
        #  @param theLength Length on the referenced curve. It can be negative.
        #  @param theStartPoint Any point can be selected for it, the new edge will begin
        #                       at the end of \a theRefCurve, close to the selected point.
        #                       If None, start from the first point of \a theRefCurve.
        #  @return New GEOM.GEOM_Object, containing the created edge.
        #
        #  @ref tui_creation_edge "Example"
        def MakeEdgeOnCurveByLength(self, theRefCurve, theLength, theStartPoint = None):
            """
            Create a new edge, corresponding to the given length on the given curve.

            Parameters:
                theRefCurve The referenced curve (edge).
                theLength Length on the referenced curve. It can be negative.
                theStartPoint Any point can be selected for it, the new edge will begin
                              at the end of theRefCurve, close to the selected point.
                              If None, start from the first point of theRefCurve.

            Returns:              
                New GEOM.GEOM_Object, containing the created edge.
            """
            # Example: see GEOM_TestAll.py
            theLength, Parameters = ParseParameters(theLength)
            anObj = self.ShapesOp.MakeEdgeOnCurveByLength(theRefCurve, theLength, theStartPoint)
            RaiseIfFailed("MakeEdgeOnCurveByLength", self.ShapesOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create an edge from specified wire.
        #  @param theWire source Wire
        #  @param theLinearTolerance linear tolerance value (default = 1e-07)
        #  @param theAngularTolerance angular tolerance value (default = 1e-12)
        #  @return New GEOM.GEOM_Object, containing the created edge.
        #
        #  @ref tui_creation_edge "Example"
        def MakeEdgeWire(self, theWire, theLinearTolerance = 1e-07, theAngularTolerance = 1e-12):
            """
            Create an edge from specified wire.

            Parameters:
                theWire source Wire
                theLinearTolerance linear tolerance value (default = 1e-07)
                theAngularTolerance angular tolerance value (default = 1e-12)

            Returns:
                New GEOM.GEOM_Object, containing the created edge.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeEdgeWire(theWire, theLinearTolerance, theAngularTolerance)
            RaiseIfFailed("MakeEdgeWire", self.ShapesOp)
            return anObj

        ## Create a wire from the set of edges and wires.
        #  @param theEdgesAndWires List of edges and/or wires.
        #  @param theTolerance Maximum distance between vertices, that will be merged.
        #                      Values less than 1e-07 are equivalent to 1e-07 (Precision::Confusion())
        #  @return New GEOM.GEOM_Object, containing the created wire.
        #
        #  @ref tui_creation_wire "Example"
        def MakeWire(self, theEdgesAndWires, theTolerance = 1e-07):
            """
            Create a wire from the set of edges and wires.

            Parameters:
                theEdgesAndWires List of edges and/or wires.
                theTolerance Maximum distance between vertices, that will be merged.
                             Values less than 1e-07 are equivalent to 1e-07 (Precision::Confusion()).

            Returns:                    
                New GEOM.GEOM_Object, containing the created wire.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeWire(theEdgesAndWires, theTolerance)
            RaiseIfFailed("MakeWire", self.ShapesOp)
            return anObj

        ## Create a face on the given wire.
        #  @param theWire closed Wire or Edge to build the face on.
        #  @param isPlanarWanted If TRUE, the algorithm tries to build a planar face.
        #                        If the tolerance of the obtained planar face is less
        #                        than 1e-06, this face will be returned, otherwise the
        #                        algorithm tries to build any suitable face on the given
        #                        wire and prints a warning message.
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_face "Example"
        def MakeFace(self, theWire, isPlanarWanted):
            """
            Create a face on the given wire.

            Parameters:
                theWire closed Wire or Edge to build the face on.
                isPlanarWanted If TRUE, the algorithm tries to build a planar face.
                               If the tolerance of the obtained planar face is less
                               than 1e-06, this face will be returned, otherwise the
                               algorithm tries to build any suitable face on the given
                               wire and prints a warning message.

            Returns:
                New GEOM.GEOM_Object, containing the created face.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeFace(theWire, isPlanarWanted)
            if isPlanarWanted and anObj is not None and self.ShapesOp.GetErrorCode() == "MAKE_FACE_TOLERANCE_TOO_BIG":
                print "WARNING: Cannot build a planar face: required tolerance is too big. Non-planar face is built."
            else:
                RaiseIfFailed("MakeFace", self.ShapesOp)
            return anObj

        ## Create a face on the given wires set.
        #  @param theWires List of closed wires or edges to build the face on.
        #  @param isPlanarWanted If TRUE, the algorithm tries to build a planar face.
        #                        If the tolerance of the obtained planar face is less
        #                        than 1e-06, this face will be returned, otherwise the
        #                        algorithm tries to build any suitable face on the given
        #                        wire and prints a warning message.
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_face "Example"
        def MakeFaceWires(self, theWires, isPlanarWanted):
            """
            Create a face on the given wires set.

            Parameters:
                theWires List of closed wires or edges to build the face on.
                isPlanarWanted If TRUE, the algorithm tries to build a planar face.
                               If the tolerance of the obtained planar face is less
                               than 1e-06, this face will be returned, otherwise the
                               algorithm tries to build any suitable face on the given
                               wire and prints a warning message.

            Returns: 
                New GEOM.GEOM_Object, containing the created face.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeFaceWires(theWires, isPlanarWanted)
            if isPlanarWanted and anObj is not None and self.ShapesOp.GetErrorCode() == "MAKE_FACE_TOLERANCE_TOO_BIG":
                print "WARNING: Cannot build a planar face: required tolerance is too big. Non-planar face is built."
            else:
                RaiseIfFailed("MakeFaceWires", self.ShapesOp)
            return anObj

        ## See MakeFaceWires() method for details.
        #
        #  @ref tui_creation_face "Example 1"
        #  \n @ref swig_MakeFaces  "Example 2"
        def MakeFaces(self, theWires, isPlanarWanted):
            """
            See geompy.MakeFaceWires() method for details.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.MakeFaceWires(theWires, isPlanarWanted)
            return anObj

        ## Create a shell from the set of faces and shells.
        #  @param theFacesAndShells List of faces and/or shells.
        #  @return New GEOM.GEOM_Object, containing the created shell.
        #
        #  @ref tui_creation_shell "Example"
        def MakeShell(self,theFacesAndShells):
            """
            Create a shell from the set of faces and shells.

            Parameters:
                theFacesAndShells List of faces and/or shells.

            Returns:
                New GEOM.GEOM_Object, containing the created shell.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeShell(theFacesAndShells)
            RaiseIfFailed("MakeShell", self.ShapesOp)
            return anObj

        ## Create a solid, bounded by the given shells.
        #  @param theShells Sequence of bounding shells.
        #  @return New GEOM.GEOM_Object, containing the created solid.
        #
        #  @ref tui_creation_solid "Example"
        def MakeSolid(self, theShells):
            """
            Create a solid, bounded by the given shells.

            Parameters:
                theShells Sequence of bounding shells.

            Returns:
                New GEOM.GEOM_Object, containing the created solid.
            """
            # Example: see GEOM_TestAll.py
            if len(theShells) == 1:
                descr = self.MeasuOp.IsGoodForSolid(theShells[0])
                #if len(descr) > 0:
                #    raise RuntimeError, "MakeSolidShells : " + descr
                if descr == "WRN_SHAPE_UNCLOSED":
                    raise RuntimeError, "MakeSolidShells : Unable to create solid from unclosed shape"
            anObj = self.ShapesOp.MakeSolidShells(theShells)
            RaiseIfFailed("MakeSolidShells", self.ShapesOp)
            return anObj

        ## Create a compound of the given shapes.
        #  @param theShapes List of shapes to put in compound.
        #  @return New GEOM.GEOM_Object, containing the created compound.
        #
        #  @ref tui_creation_compound "Example"
        def MakeCompound(self,theShapes):
            """
            Create a compound of the given shapes.

            Parameters:
                theShapes List of shapes to put in compound.

            Returns:
                New GEOM.GEOM_Object, containing the created compound.
            """
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
            """
            Gives quantity of faces in the given shape.

            Parameters:
                theShape Shape to count faces of.

            Returns:    
                Quantity of faces.
            """
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
            """
            Gives quantity of edges in the given shape.

            Parameters:
                theShape Shape to count edges of.

            Returns:    
                Quantity of edges.
            """
            # Example: see GEOM_TestOthers.py
            nb_edges = self.ShapesOp.NumberOfEdges(theShape)
            RaiseIfFailed("NumberOfEdges", self.ShapesOp)
            return nb_edges

        ## Gives quantity of sub-shapes of type theShapeType in the given shape.
        #  @param theShape Shape to count sub-shapes of.
        #  @param theShapeType Type of sub-shapes to count (see ShapeType())
        #  @return Quantity of sub-shapes of given type.
        #
        #  @ref swig_NumberOf "Example"
        def NumberOfSubShapes(self, theShape, theShapeType):
            """
            Gives quantity of sub-shapes of type theShapeType in the given shape.

            Parameters:
                theShape Shape to count sub-shapes of.
                theShapeType Type of sub-shapes to count (see geompy.ShapeType)

            Returns:
                Quantity of sub-shapes of given type.
            """
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
            """
            Gives quantity of solids in the given shape.

            Parameters:
                theShape Shape to count solids in.

            Returns:
                Quantity of solids.
            """
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
            """
            Reverses an orientation the given shape.

            Parameters:
                theShape Shape to be reversed.

            Returns:   
                The reversed copy of theShape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.ChangeOrientation(theShape)
            RaiseIfFailed("ChangeOrientation", self.ShapesOp)
            return anObj

        ## See ChangeOrientation() method for details.
        #
        #  @ref swig_OrientationChange "Example"
        def OrientationChange(self,theShape):
            """
            See geompy.ChangeOrientation method for details.
            """
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
            """
            Retrieve all free faces from the given shape.
            Free face is a face, which is not shared between two shells of the shape.

            Parameters:
                theShape Shape to find free faces in.

            Returns:
                List of IDs of all free faces, contained in theShape.
            """
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
            """
            Get all sub-shapes of theShape1 of the given type, shared with theShape2.

            Parameters:
                theShape1 Shape to find sub-shapes in.
                theShape2 Shape to find shared sub-shapes with.
                theShapeType Type of sub-shapes to be retrieved.

            Returns:
                List of sub-shapes of theShape1, shared with theShape2.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetSharedShapes(theShape1, theShape2, theShapeType)
            RaiseIfFailed("GetSharedShapes", self.ShapesOp)
            return aList

        ## Get all sub-shapes, shared by all shapes in the list <VAR>theShapes</VAR>.
        #  @param theShapes Shapes to find common sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @return List of objects, that are sub-shapes of all given shapes.
        #
        #  @ref swig_GetSharedShapes "Example"
        def GetSharedShapesMulti(self, theShapes, theShapeType):
            """
            Get all sub-shapes, shared by all shapes in the list theShapes.

            Parameters:
                theShapes Shapes to find common sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)

            Returns:    
                List of GEOM.GEOM_Object, that are sub-shapes of all given shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetSharedShapesMulti(theShapes, theShapeType)
            RaiseIfFailed("GetSharedShapesMulti", self.ShapesOp)
            return aList

        ## Find in <VAR>theShape</VAR> all sub-shapes of type <VAR>theShapeType</VAR>,
        #  situated relatively the specified plane by the certain way,
        #  defined through <VAR>theState</VAR> parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theAx1 Vector (or line, or linear edge), specifying normal
        #                direction and location of the plane to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnPlane "Example"
        def GetShapesOnPlane(self,theShape, theShapeType, theAx1, theState):
            """
            Find in theShape all sub-shapes of type theShapeType,
            situated relatively the specified plane by the certain way,
            defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theAx1 Vector (or line, or linear edge), specifying normal
                       direction and location of the plane to find shapes on.
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnPlane(theShape, theShapeType, theAx1, theState)
            RaiseIfFailed("GetShapesOnPlane", self.ShapesOp)
            return aList

        ## Find in <VAR>theShape</VAR> all sub-shapes of type <VAR>theShapeType</VAR>,
        #  situated relatively the specified plane by the certain way,
        #  defined through <VAR>theState</VAR> parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theAx1 Vector (or line, or linear edge), specifying normal
        #                direction and location of the plane to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnPlaneIDs "Example"
        def GetShapesOnPlaneIDs(self,theShape, theShapeType, theAx1, theState):
            """
            Find in theShape all sub-shapes of type theShapeType,
            situated relatively the specified plane by the certain way,
            defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theAx1 Vector (or line, or linear edge), specifying normal
                       direction and location of the plane to find shapes on.
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes indices.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnPlaneIDs(theShape, theShapeType, theAx1, theState)
            RaiseIfFailed("GetShapesOnPlaneIDs", self.ShapesOp)
            return aList

        ## Find in <VAR>theShape</VAR> all sub-shapes of type <VAR>theShapeType</VAR>,
        #  situated relatively the specified plane by the certain way,
        #  defined through <VAR>theState</VAR> parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theAx1 Vector (or line, or linear edge), specifying normal
        #                direction of the plane to find shapes on.
        #  @param thePnt Point specifying location of the plane to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnPlaneWithLocation "Example"
        def GetShapesOnPlaneWithLocation(self, theShape, theShapeType, theAx1, thePnt, theState):
            """
            Find in theShape all sub-shapes of type theShapeType,
            situated relatively the specified plane by the certain way,
            defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theAx1 Vector (or line, or linear edge), specifying normal
                       direction and location of the plane to find shapes on.
                thePnt Point specifying location of the plane to find shapes on.
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnPlaneWithLocation(theShape, theShapeType,
                                                               theAx1, thePnt, theState)
            RaiseIfFailed("GetShapesOnPlaneWithLocation", self.ShapesOp)
            return aList

        ## Find in <VAR>theShape</VAR> all sub-shapes of type <VAR>theShapeType</VAR>,
        #  situated relatively the specified plane by the certain way,
        #  defined through <VAR>theState</VAR> parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theAx1 Vector (or line, or linear edge), specifying normal
        #                direction of the plane to find shapes on.
        #  @param thePnt Point specifying location of the plane to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnPlaneWithLocationIDs "Example"
        def GetShapesOnPlaneWithLocationIDs(self, theShape, theShapeType, theAx1, thePnt, theState):
            """
            Find in theShape all sub-shapes of type theShapeType,
            situated relatively the specified plane by the certain way,
            defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theAx1 Vector (or line, or linear edge), specifying normal
                       direction and location of the plane to find shapes on.
                thePnt Point specifying location of the plane to find shapes on.
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes indices.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnPlaneWithLocationIDs(theShape, theShapeType,
                                                                  theAx1, thePnt, theState)
            RaiseIfFailed("GetShapesOnPlaneWithLocationIDs", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified cylinder by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theAxis Vector (or line, or linear edge), specifying
        #                 axis of the cylinder to find shapes on.
        #  @param theRadius Radius of the cylinder to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnCylinder "Example"
        def GetShapesOnCylinder(self, theShape, theShapeType, theAxis, theRadius, theState):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified cylinder by the certain way, defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theAxis Vector (or line, or linear edge), specifying
                        axis of the cylinder to find shapes on.
                theRadius Radius of the cylinder to find shapes on.
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnCylinder(theShape, theShapeType, theAxis, theRadius, theState)
            RaiseIfFailed("GetShapesOnCylinder", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified cylinder by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theAxis Vector (or line, or linear edge), specifying
        #                 axis of the cylinder to find shapes on.
        #  @param theRadius Radius of the cylinder to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnCylinderIDs "Example"
        def GetShapesOnCylinderIDs(self, theShape, theShapeType, theAxis, theRadius, theState):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified cylinder by the certain way, defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theAxis Vector (or line, or linear edge), specifying
                        axis of the cylinder to find shapes on.
                theRadius Radius of the cylinder to find shapes on.
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes indices.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnCylinderIDs(theShape, theShapeType, theAxis, theRadius, theState)
            RaiseIfFailed("GetShapesOnCylinderIDs", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified cylinder by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theAxis Vector (or line, or linear edge), specifying
        #                 axis of the cylinder to find shapes on.
        #  @param thePnt Point specifying location of the bottom of the cylinder.
        #  @param theRadius Radius of the cylinder to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnCylinderWithLocation "Example"
        def GetShapesOnCylinderWithLocation(self, theShape, theShapeType, theAxis, thePnt, theRadius, theState):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified cylinder by the certain way, defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theAxis Vector (or line, or linear edge), specifying
                        axis of the cylinder to find shapes on.
                theRadius Radius of the cylinder to find shapes on.
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnCylinderWithLocation(theShape, theShapeType, theAxis, thePnt, theRadius, theState)
            RaiseIfFailed("GetShapesOnCylinderWithLocation", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified cylinder by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theAxis Vector (or line, or linear edge), specifying
        #                 axis of the cylinder to find shapes on.
        #  @param thePnt Point specifying location of the bottom of the cylinder.
        #  @param theRadius Radius of the cylinder to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes indices
        #
        #  @ref swig_GetShapesOnCylinderWithLocationIDs "Example"
        def GetShapesOnCylinderWithLocationIDs(self, theShape, theShapeType, theAxis, thePnt, theRadius, theState):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified cylinder by the certain way, defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theAxis Vector (or line, or linear edge), specifying
                        axis of the cylinder to find shapes on.
                theRadius Radius of the cylinder to find shapes on.
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes indices.            
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnCylinderWithLocationIDs(theShape, theShapeType, theAxis, thePnt, theRadius, theState)
            RaiseIfFailed("GetShapesOnCylinderWithLocationIDs", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified sphere by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theCenter Point, specifying center of the sphere to find shapes on.
        #  @param theRadius Radius of the sphere to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnSphere "Example"
        def GetShapesOnSphere(self,theShape, theShapeType, theCenter, theRadius, theState):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified sphere by the certain way, defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theCenter Point, specifying center of the sphere to find shapes on.
                theRadius Radius of the sphere to find shapes on.
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnSphere(theShape, theShapeType, theCenter, theRadius, theState)
            RaiseIfFailed("GetShapesOnSphere", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified sphere by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theCenter Point, specifying center of the sphere to find shapes on.
        #  @param theRadius Radius of the sphere to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnSphereIDs "Example"
        def GetShapesOnSphereIDs(self,theShape, theShapeType, theCenter, theRadius, theState):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified sphere by the certain way, defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theCenter Point, specifying center of the sphere to find shapes on.
                theRadius Radius of the sphere to find shapes on.
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes indices.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnSphereIDs(theShape, theShapeType, theCenter, theRadius, theState)
            RaiseIfFailed("GetShapesOnSphereIDs", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified quadrangle by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theTopLeftPoint Point, specifying top left corner of a quadrangle
        #  @param theTopRigthPoint Point, specifying top right corner of a quadrangle
        #  @param theBottomLeftPoint Point, specifying bottom left corner of a quadrangle
        #  @param theBottomRigthPoint Point, specifying bottom right corner of a quadrangle
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnQuadrangle "Example"
        def GetShapesOnQuadrangle(self, theShape, theShapeType,
                                  theTopLeftPoint, theTopRigthPoint,
                                  theBottomLeftPoint, theBottomRigthPoint, theState):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified quadrangle by the certain way, defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theTopLeftPoint Point, specifying top left corner of a quadrangle
                theTopRigthPoint Point, specifying top right corner of a quadrangle
                theBottomLeftPoint Point, specifying bottom left corner of a quadrangle
                theBottomRigthPoint Point, specifying bottom right corner of a quadrangle
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnQuadrangle(theShape, theShapeType,
                                                        theTopLeftPoint, theTopRigthPoint,
                                                        theBottomLeftPoint, theBottomRigthPoint, theState)
            RaiseIfFailed("GetShapesOnQuadrangle", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified quadrangle by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theTopLeftPoint Point, specifying top left corner of a quadrangle
        #  @param theTopRigthPoint Point, specifying top right corner of a quadrangle
        #  @param theBottomLeftPoint Point, specifying bottom left corner of a quadrangle
        #  @param theBottomRigthPoint Point, specifying bottom right corner of a quadrangle
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnQuadrangleIDs "Example"
        def GetShapesOnQuadrangleIDs(self, theShape, theShapeType,
                                     theTopLeftPoint, theTopRigthPoint,
                                     theBottomLeftPoint, theBottomRigthPoint, theState):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified quadrangle by the certain way, defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theTopLeftPoint Point, specifying top left corner of a quadrangle
                theTopRigthPoint Point, specifying top right corner of a quadrangle
                theBottomLeftPoint Point, specifying bottom left corner of a quadrangle
                theBottomRigthPoint Point, specifying bottom right corner of a quadrangle
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes indices.
            """

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
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnBox "Example"
        def GetShapesOnBox(self, theBox, theShape, theShapeType, theState):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified theBox by the certain way, defined through theState parameter.

            Parameters:
                theBox Shape for relative comparing.
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnBox(theBox, theShape, theShapeType, theState)
            RaiseIfFailed("GetShapesOnBox", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified \a theBox by the certain way, defined through \a theState parameter.
        #  @param theBox Shape for relative comparing.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnBoxIDs "Example"
        def GetShapesOnBoxIDs(self, theBox, theShape, theShapeType, theState):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified theBox by the certain way, defined through theState parameter.

            Parameters:
                theBox Shape for relative comparing.
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes indices.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnBoxIDs(theBox, theShape, theShapeType, theState)
            RaiseIfFailed("GetShapesOnBoxIDs", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType,
        #  situated relatively the specified \a theCheckShape by the
        #  certain way, defined through \a theState parameter.
        #  @param theCheckShape Shape for relative comparing. It must be a solid.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType()) 
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnShape "Example"
        def GetShapesOnShape(self, theCheckShape, theShape, theShapeType, theState):
            """
            Find in theShape all sub-shapes of type theShapeType,
            situated relatively the specified theCheckShape by the
            certain way, defined through theState parameter.

            Parameters:
                theCheckShape Shape for relative comparing. It must be a solid.
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnShape(theCheckShape, theShape,
                                                   theShapeType, theState)
            RaiseIfFailed("GetShapesOnShape", self.ShapesOp)
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType,
        #  situated relatively the specified \a theCheckShape by the
        #  certain way, defined through \a theState parameter.
        #  @param theCheckShape Shape for relative comparing. It must be a solid.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return All found sub-shapes as compound.
        #
        #  @ref swig_GetShapesOnShapeAsCompound "Example"
        def GetShapesOnShapeAsCompound(self, theCheckShape, theShape, theShapeType, theState):
            """
            Find in theShape all sub-shapes of type theShapeType,
            situated relatively the specified theCheckShape by the
            certain way, defined through theState parameter.

            Parameters:
                theCheckShape Shape for relative comparing. It must be a solid.
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                All found sub-shapes as compound.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.ShapesOp.GetShapesOnShapeAsCompound(theCheckShape, theShape,
                                                             theShapeType, theState)
            RaiseIfFailed("GetShapesOnShapeAsCompound", self.ShapesOp)
            return anObj

        ## Find in \a theShape all sub-shapes of type \a theShapeType,
        #  situated relatively the specified \a theCheckShape by the
        #  certain way, defined through \a theState parameter.
        #  @param theCheckShape Shape for relative comparing. It must be a solid.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnShapeIDs "Example"
        def GetShapesOnShapeIDs(self, theCheckShape, theShape, theShapeType, theState):
            """
            Find in theShape all sub-shapes of type theShapeType,
            situated relatively the specified theCheckShape by the
            certain way, defined through theState parameter.

            Parameters:
                theCheckShape Shape for relative comparing. It must be a solid.
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theState The state of the sub-shapes to find (see GEOM::shape_state)

            Returns:
                List of all found sub-shapes indices.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnShapeIDs(theCheckShape, theShape,
                                                      theShapeType, theState)
            RaiseIfFailed("GetShapesOnShapeIDs", self.ShapesOp)
            return aList

        ## Get sub-shape(s) of theShapeWhere, which are
        #  coincident with \a theShapeWhat or could be a part of it.
        #  @param theShapeWhere Shape to find sub-shapes of.
        #  @param theShapeWhat Shape, specifying what to find.
        #  @param isNewImplementation implementation of GetInPlace functionality
        #             (default = False, old alghorithm based on shape properties)
        #  @return Group of all found sub-shapes or a single found sub-shape.
        #
        #  @note This function has a restriction on argument shapes.
        #        If \a theShapeWhere has curved parts with significantly
        #        outstanding centres (i.e. the mass centre of a part is closer to
        #        \a theShapeWhat than to the part), such parts will not be found.
        #        @image html get_in_place_lost_part.png
        #
        #  @ref swig_GetInPlace "Example"
        def GetInPlace(self, theShapeWhere, theShapeWhat, isNewImplementation = False):
            """
            Get sub-shape(s) of theShapeWhere, which are
            coincident with  theShapeWhat or could be a part of it.

            Parameters:
                theShapeWhere Shape to find sub-shapes of.
                theShapeWhat Shape, specifying what to find.
                isNewImplementation Implementation of GetInPlace functionality
                                    (default = False, old alghorithm based on shape properties)

            Returns:
                Group of all found sub-shapes or a single found sub-shape.

                
            Note:
                This function has a restriction on argument shapes.
                If theShapeWhere has curved parts with significantly
                outstanding centres (i.e. the mass centre of a part is closer to
                theShapeWhat than to the part), such parts will not be found.
            """
            # Example: see GEOM_TestOthers.py
            anObj = None
            if isNewImplementation:
                anObj = self.ShapesOp.GetInPlace(theShapeWhere, theShapeWhat)
            else:
                anObj = self.ShapesOp.GetInPlaceOld(theShapeWhere, theShapeWhat)
                pass
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
            """
            Implementation of this method is based on a saved history of an operation,
            produced theShapeWhere. The theShapeWhat must be among this operation's
            arguments (an argument shape or a sub-shape of an argument shape).
            The operation could be the Partition or one of boolean operations,
            performed on simple shapes (not on compounds).

            Parameters:
                theShapeWhere Shape to find sub-shapes of.
                theShapeWhat Shape, specifying what to find (must be in the
                                building history of the ShapeWhere).

            Returns:
                Group of all found sub-shapes or a single found sub-shape.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.ShapesOp.GetInPlaceByHistory(theShapeWhere, theShapeWhat)
            RaiseIfFailed("GetInPlaceByHistory", self.ShapesOp)
            return anObj

        ## Get sub-shape of theShapeWhere, which is
        #  equal to \a theShapeWhat.
        #  @param theShapeWhere Shape to find sub-shape of.
        #  @param theShapeWhat Shape, specifying what to find.
        #  @return New GEOM.GEOM_Object for found sub-shape.
        #
        #  @ref swig_GetSame "Example"
        def GetSame(self,theShapeWhere, theShapeWhat):
            """
            Get sub-shape of theShapeWhere, which is
            equal to theShapeWhat.

            Parameters:
                theShapeWhere Shape to find sub-shape of.
                theShapeWhat Shape, specifying what to find.

            Returns:
                New GEOM.GEOM_Object for found sub-shape.
            """
            anObj = self.ShapesOp.GetSame(theShapeWhere, theShapeWhat)
            RaiseIfFailed("GetSame", self.ShapesOp)
            return anObj


        ## Get sub-shape indices of theShapeWhere, which is
        #  equal to \a theShapeWhat.
        #  @param theShapeWhere Shape to find sub-shape of.
        #  @param theShapeWhat Shape, specifying what to find.
        #  @return List of all found sub-shapes indices. 
        #
        #  @ref swig_GetSame "Example"
        def GetSameIDs(self,theShapeWhere, theShapeWhat):
            """
            Get sub-shape indices of theShapeWhere, which is
            equal to theShapeWhat.

            Parameters:
                theShapeWhere Shape to find sub-shape of.
                theShapeWhat Shape, specifying what to find.

            Returns:
                List of all found sub-shapes indices.
            """
            anObj = self.ShapesOp.GetSameIDs(theShapeWhere, theShapeWhat)
            RaiseIfFailed("GetSameIDs", self.ShapesOp)
            return anObj


        # end of l4_obtain
        ## @}

        ## @addtogroup l4_access
        ## @{

        ## Obtain a composite sub-shape of <VAR>aShape</VAR>, composed from sub-shapes
        #  of aShape, selected by their unique IDs inside <VAR>aShape</VAR>
        #  @param aShape Shape to get sub-shape of.
        #  @param ListOfID List of sub-shapes indices.
        #  @return Found sub-shape.
        #
        #  @ref swig_all_decompose "Example"
        def GetSubShape(self, aShape, ListOfID):
            """
            Obtain a composite sub-shape of aShape, composed from sub-shapes
            of aShape, selected by their unique IDs inside aShape

            Parameters:
               aShape Shape to get sub-shape of.
               ListOfID List of sub-shapes indices.

            Returns:
                Found sub-shape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.AddSubShape(aShape,ListOfID)
            return anObj

        ## Obtain unique ID of sub-shape <VAR>aSubShape</VAR> inside <VAR>aShape</VAR>
        #  of aShape, selected by their unique IDs inside <VAR>aShape</VAR>
        #  @param aShape Shape to get sub-shape of.
        #  @param aSubShape Sub-shapes of aShape.
        #  @return ID of found sub-shape.
        #
        #  @ref swig_all_decompose "Example"
        def GetSubShapeID(self, aShape, aSubShape):
            """
            Obtain unique ID of sub-shape aSubShape inside aShape
            of aShape, selected by their unique IDs inside aShape

            Parameters:
               aShape Shape to get sub-shape of.
               aSubShape Sub-shapes of aShape.

            Returns:
               ID of found sub-shape.
            """
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
            """
            Get all sub-shapes and groups of theShape,
            that were created already by any other methods.

            Parameters:
                theShape Any shape.
                theGroupsOnly If this parameter is TRUE, only groups will be
                                 returned, else all found sub-shapes and groups.

            Returns:
                List of existing sub-objects of theShape.
            """
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
            """
            Get all groups of theShape,
            that were created already by any other methods.

            Parameters:
                theShape Any shape.

            Returns:
                List of existing groups of theShape.
            """
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.GetExistingSubObjects(theShape, True)
            RaiseIfFailed("GetExistingSubObjects", self.ShapesOp)
            return ListObj

        ## Explode a shape on sub-shapes of a given type.
        #  If the shape itself matches the type, it is also returned.
        #  @param aShape Shape to be exploded.
        #  @param aType Type of sub-shapes to be retrieved (see ShapeType()) 
        #  @return List of sub-shapes of type theShapeType, contained in theShape.
        #
        #  @ref swig_all_decompose "Example"
        def SubShapeAll(self, aShape, aType):
            """
            Explode a shape on sub-shapes of a given type.
            If the shape itself matches the type, it is also returned.

            Parameters:
                aShape Shape to be exploded.
                aType Type of sub-shapes to be retrieved (see geompy.ShapeType) 

            Returns:
                List of sub-shapes of type theShapeType, contained in theShape.
            """
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.MakeAllSubShapes(aShape, EnumToLong( aType ), False)
            RaiseIfFailed("SubShapeAll", self.ShapesOp)
            return ListObj

        ## Explode a shape on sub-shapes of a given type.
        #  @param aShape Shape to be exploded.
        #  @param aType Type of sub-shapes to be retrieved (see ShapeType())
        #  @return List of IDs of sub-shapes.
        #
        #  @ref swig_all_decompose "Example"
        def SubShapeAllIDs(self, aShape, aType):
            """
            Explode a shape on sub-shapes of a given type.

            Parameters:
                aShape Shape to be exploded (see geompy.ShapeType) 
                aType Type of sub-shapes to be retrieved (see geompy.ShapeType)

            Returns:
                List of IDs of sub-shapes.
            """
            ListObj = self.ShapesOp.GetAllSubShapesIDs(aShape, EnumToLong( aType ), False)
            RaiseIfFailed("SubShapeAllIDs", self.ShapesOp)
            return ListObj

        ## Obtain a compound of sub-shapes of <VAR>aShape</VAR>,
        #  selected by they indices in list of all sub-shapes of type <VAR>aType</VAR>.
        #  Each index is in range [1, Nb_Sub-Shapes_Of_Given_Type]
        #  @param aShape Shape to get sub-shape of.
        #  @param ListOfInd List of sub-shapes indices.
        #  @param aType Type of sub-shapes to be retrieved (see ShapeType())
        #  @return A compound of sub-shapes of aShape.
        #
        #  @ref swig_all_decompose "Example"
        def SubShape(self, aShape, aType, ListOfInd):
            """
            Obtain a compound of sub-shapes of aShape,
            selected by they indices in list of all sub-shapes of type aType.
            Each index is in range [1, Nb_Sub-Shapes_Of_Given_Type]
            
            Parameters:
                aShape Shape to get sub-shape of.
                ListOfID List of sub-shapes indices.
                aType Type of sub-shapes to be retrieved (see geompy.ShapeType)

            Returns:
                A compound of sub-shapes of aShape.
            """
            # Example: see GEOM_TestAll.py
            ListOfIDs = []
            AllShapeIDsList = self.SubShapeAllIDs(aShape, EnumToLong( aType ))
            for ind in ListOfInd:
                ListOfIDs.append(AllShapeIDsList[ind - 1])
            anObj = self.GetSubShape(aShape, ListOfIDs)
            return anObj

        ## Explode a shape on sub-shapes of a given type.
        #  Sub-shapes will be sorted by coordinates of their gravity centers.
        #  If the shape itself matches the type, it is also returned.
        #  @param aShape Shape to be exploded.
        #  @param aType Type of sub-shapes to be retrieved (see ShapeType())
        #  @return List of sub-shapes of type theShapeType, contained in theShape.
        #
        #  @ref swig_SubShapeAllSorted "Example"
        def SubShapeAllSortedCentres(self, aShape, aType):
            """
            Explode a shape on sub-shapes of a given type.
            Sub-shapes will be sorted by coordinates of their gravity centers.
            If the shape itself matches the type, it is also returned.

            Parameters: 
                aShape Shape to be exploded.
                aType Type of sub-shapes to be retrieved (see geompy.ShapeType)

            Returns: 
                List of sub-shapes of type theShapeType, contained in theShape.
            """
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.MakeAllSubShapes(aShape, EnumToLong( aType ), True)
            RaiseIfFailed("SubShapeAllSortedCentres", self.ShapesOp)
            return ListObj

        ## Explode a shape on sub-shapes of a given type.
        #  Sub-shapes will be sorted by coordinates of their gravity centers.
        #  @param aShape Shape to be exploded.
        #  @param aType Type of sub-shapes to be retrieved (see ShapeType())
        #  @return List of IDs of sub-shapes.
        #
        #  @ref swig_all_decompose "Example"
        def SubShapeAllSortedCentresIDs(self, aShape, aType):
            """
            Explode a shape on sub-shapes of a given type.
            Sub-shapes will be sorted by coordinates of their gravity centers.

            Parameters: 
                aShape Shape to be exploded.
                aType Type of sub-shapes to be retrieved (see geompy.ShapeType)

            Returns: 
                List of IDs of sub-shapes.
            """
            ListIDs = self.ShapesOp.GetAllSubShapesIDs(aShape, EnumToLong( aType ), True)
            RaiseIfFailed("SubShapeAllIDs", self.ShapesOp)
            return ListIDs

        ## Obtain a compound of sub-shapes of <VAR>aShape</VAR>,
        #  selected by they indices in sorted list of all sub-shapes of type <VAR>aType</VAR>.
        #  Each index is in range [1, Nb_Sub-Shapes_Of_Given_Type]
        #  @param aShape Shape to get sub-shape of.
        #  @param ListOfInd List of sub-shapes indices.
        #  @param aType Type of sub-shapes to be retrieved (see ShapeType())
        #  @return A compound of sub-shapes of aShape.
        #
        #  @ref swig_all_decompose "Example"
        def SubShapeSortedCentres(self, aShape, aType, ListOfInd):
            """
            Obtain a compound of sub-shapes of aShape,
            selected by they indices in sorted list of all sub-shapes of type aType.
            Each index is in range [1, Nb_Sub-Shapes_Of_Given_Type]

            Parameters:
                aShape Shape to get sub-shape of.
                ListOfID List of sub-shapes indices.
                aType Type of sub-shapes to be retrieved (see geompy.ShapeType)

            Returns:
                A compound of sub-shapes of aShape.
            """
            # Example: see GEOM_TestAll.py
            ListOfIDs = []
            AllShapeIDsList = self.SubShapeAllSortedCentresIDs(aShape, EnumToLong( aType ))
            for ind in ListOfInd:
                ListOfIDs.append(AllShapeIDsList[ind - 1])
            anObj = self.GetSubShape(aShape, ListOfIDs)
            return anObj

        ## Extract shapes (excluding the main shape) of given type.
        #  @param aShape The shape.
        #  @param aType  The shape type (see ShapeType())
        #  @param isSorted Boolean flag to switch sorting on/off.
        #  @return List of sub-shapes of type aType, contained in aShape.
        #
        #  @ref swig_FilletChamfer "Example"
        def ExtractShapes(self, aShape, aType, isSorted = False):
            """
            Extract shapes (excluding the main shape) of given type.

            Parameters:
                aShape The shape.
                aType  The shape type (see geompy.ShapeType)
                isSorted Boolean flag to switch sorting on/off.

            Returns:     
                List of sub-shapes of type aType, contained in aShape.
            """
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.ExtractSubShapes(aShape, EnumToLong( aType ), isSorted)
            RaiseIfFailed("ExtractSubShapes", self.ShapesOp)
            return ListObj

        ## Get a set of sub-shapes defined by their unique IDs inside <VAR>aShape</VAR>
        #  @param aShape Main shape.
        #  @param anIDs List of unique IDs of sub-shapes inside <VAR>aShape</VAR>.
        #  @return List of GEOM.GEOM_Object, corresponding to found sub-shapes.
        #
        #  @ref swig_all_decompose "Example"
        def SubShapes(self, aShape, anIDs):
            """
            Get a set of sub-shapes defined by their unique IDs inside theMainShape

            Parameters:
                aShape Main shape.
                anIDs List of unique IDs of sub-shapes inside theMainShape.

            Returns:      
                List of GEOM.GEOM_Object, corresponding to found sub-shapes.
            """
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.MakeSubShapes(aShape, anIDs)
            RaiseIfFailed("SubShapes", self.ShapesOp)
            return ListObj

        # end of l4_decompose
        ## @}

        ## @addtogroup l4_decompose_d
        ## @{

        ## Deprecated method
        #  It works like SubShapeAllSortedCentres(), but wrongly
        #  defines centres of faces, shells and solids.
        def SubShapeAllSorted(self, aShape, aType):
            """
            Deprecated method
            It works like geompy.SubShapeAllSortedCentres, but wrongly
            defines centres of faces, shells and solids.
            """
            ListObj = self.ShapesOp.MakeExplode(aShape, EnumToLong( aType ), True)
            RaiseIfFailed("MakeExplode", self.ShapesOp)
            return ListObj

        ## Deprecated method
        #  It works like SubShapeAllSortedCentresIDs(), but wrongly
        #  defines centres of faces, shells and solids.
        def SubShapeAllSortedIDs(self, aShape, aType):
            """
            Deprecated method
            It works like geompy.SubShapeAllSortedCentresIDs, but wrongly
            defines centres of faces, shells and solids.
            """
            ListIDs = self.ShapesOp.SubShapeAllIDs(aShape, EnumToLong( aType ), True)
            RaiseIfFailed("SubShapeAllIDs", self.ShapesOp)
            return ListIDs

        ## Deprecated method
        #  It works like SubShapeSortedCentres(), but has a bug
        #  (wrongly defines centres of faces, shells and solids).
        def SubShapeSorted(self, aShape, aType, ListOfInd):
            """
            Deprecated method
            It works like geompy.SubShapeSortedCentres, but has a bug
            (wrongly defines centres of faces, shells and solids).
            """
            ListOfIDs = []
            AllShapeIDsList = self.SubShapeAllSortedIDs(aShape, EnumToLong( aType ))
            for ind in ListOfInd:
                ListOfIDs.append(AllShapeIDsList[ind - 1])
            anObj = self.GetSubShape(aShape, ListOfIDs)
            return anObj

        # end of l4_decompose_d
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
        #
        #
        #  <b> Operators and Parameters: </b> \n
        #
        #  * \b FixShape - corrects invalid shapes. \n
        #  - \b FixShape.Tolerance3d - work tolerance for detection of the problems and correction of them. \n
        #  - \b FixShape.MaxTolerance3d - maximal possible tolerance of the shape after correction. \n
        #
        #  * \b FixFaceSize - removes small faces, such as spots and strips.\n
        #  - \b FixFaceSize.Tolerance - defines minimum possible face size. \n
        #  - \b DropSmallEdges - removes edges, which merge with neighbouring edges. \n
        #  - \b DropSmallEdges.Tolerance3d - defines minimum possible distance between two parallel edges.\n
        #
        #  * \b SplitAngle - splits faces based on conical surfaces, surfaces of revolution and cylindrical
        #    surfaces in segments using a certain angle. \n
        #  - \b SplitAngle.Angle - the central angle of the resulting segments (i.e. we obtain two segments
        #    if Angle=180, four if Angle=90, etc). \n
        #  - \b SplitAngle.MaxTolerance - maximum possible tolerance among the resulting segments.\n
        #
        #  * \b SplitClosedFaces - splits closed faces in segments.
        #    The number of segments depends on the number of splitting points.\n
        #  - \b SplitClosedFaces.NbSplitPoints - the number of splitting points.\n
        #
        #  * \b SplitContinuity - splits shapes to reduce continuities of curves and surfaces.\n
        #  - \b SplitContinuity.Tolerance3d - 3D tolerance for correction of geometry.\n
        #  - \b SplitContinuity.SurfaceContinuity - required continuity for surfaces.\n
        #  - \b SplitContinuity.CurveContinuity - required continuity for curves.\n
        #   This and the previous parameters can take the following values:\n
        #   \b Parametric \b Continuity \n
        #   \b C0 (Positional Continuity): curves are joined (the end positions of curves or surfaces
        #   are coincidental. The curves or surfaces may still meet at an angle, giving rise to a sharp corner or edge).\n
        #   \b C1 (Tangential Continuity): first derivatives are equal (the end vectors of curves or surfaces are parallel,
        #    ruling out sharp edges).\n
        #   \b C2 (Curvature Continuity): first and second derivatives are equal (the end vectors of curves or surfaces 
        #       are of the same magnitude).\n
        #   \b CN N-th derivatives are equal (both the direction and the magnitude of the Nth derivatives of curves
        #    or surfaces (d/du C(u)) are the same at junction. \n
        #   \b Geometric \b Continuity \n
        #   \b G1: first derivatives are proportional at junction.\n
        #   The curve tangents thus have the same direction, but not necessarily the same magnitude.
        #      i.e., C1'(1) = (a,b,c) and C2'(0) = (k*a, k*b, k*c).\n
        #   \b G2: first and second derivatives are proportional at junction.
        #   As the names imply, geometric continuity requires the geometry to be continuous, while parametric
        #    continuity requires that the underlying parameterization was continuous as well.
        #   Parametric continuity of order n implies geometric continuity of order n, but not vice-versa.\n
        #
        #  * \b BsplineRestriction - converts curves and surfaces to Bsplines and processes them with the following parameters:\n
        #  - \b BSplineRestriction.SurfaceMode - approximation of surfaces if restriction is necessary.\n
        #  - \b BSplineRestriction.Curve3dMode - conversion of any 3D curve to BSpline and approximation.\n
        #  - \b BSplineRestriction.Curve2dMode - conversion of any 2D curve to BSpline and approximation.\n
        #  - \b BSplineRestriction.Tolerance3d - defines the possibility of surfaces and 3D curves approximation
        #       with the specified parameters.\n
        #  - \b BSplineRestriction.Tolerance2d - defines the possibility of surfaces and 2D curves approximation
        #       with the specified parameters.\n
        #  - \b BSplineRestriction.RequiredDegree - required degree of the resulting BSplines.\n
        #  - \b BSplineRestriction.RequiredNbSegments - required maximum number of segments of resultant BSplines.\n
        #  - \b BSplineRestriction.Continuity3d - continuity of the resulting surfaces and 3D curves.\n
        #  - \b BSplineRestriction.Continuity2d - continuity of the resulting 2D curves.\n
        #
        #  * \b ToBezier - converts curves and surfaces of any type to Bezier curves and surfaces.\n
        #  - \b ToBezier.SurfaceMode - if checked in, allows conversion of surfaces.\n
        #  - \b ToBezier.Curve3dMode - if checked in, allows conversion of 3D curves.\n
        #  - \b ToBezier.Curve2dMode - if checked in, allows conversion of 2D curves.\n
        #  - \b ToBezier.MaxTolerance - defines tolerance for detection and correction of problems.\n
        #
        #  * \b SameParameter - fixes edges of 2D and 3D curves not having the same parameter.\n
        #  - \b SameParameter.Tolerance3d - defines tolerance for fixing of edges.\n
        #
        #
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  \n @ref tui_shape_processing "Example"
        def ProcessShape(self, theShape, theOperators, theParameters, theValues):
            """
            Apply a sequence of Shape Healing operators to the given object.

            Parameters:
                theShape Shape to be processed.
                theValues List of values of parameters, in the same order
                          as parameters are listed in theParameters list.
                theOperators List of names of operators ("FixShape", "SplitClosedFaces", etc.).
                theParameters List of names of parameters
                              ("FixShape.Tolerance3d", "SplitClosedFaces.NbSplitPoints", etc.).
                 Operators and Parameters:

                 * FixShape - corrects invalid shapes.
                     * FixShape.Tolerance3d - work tolerance for detection of the problems and correction of them.
                     * FixShape.MaxTolerance3d - maximal possible tolerance of the shape after correction.
                 * FixFaceSize - removes small faces, such as spots and strips.
                     * FixFaceSize.Tolerance - defines minimum possible face size.
                     * DropSmallEdges - removes edges, which merge with neighbouring edges.
                     * DropSmallEdges.Tolerance3d - defines minimum possible distance between two parallel edges.
                 * SplitAngle - splits faces based on conical surfaces, surfaces of revolution and cylindrical surfaces
                                in segments using a certain angle.
                     * SplitAngle.Angle - the central angle of the resulting segments (i.e. we obtain two segments
                                          if Angle=180, four if Angle=90, etc).
                     * SplitAngle.MaxTolerance - maximum possible tolerance among the resulting segments.
                 * SplitClosedFaces - splits closed faces in segments. The number of segments depends on the number of
                                      splitting points.
                     * SplitClosedFaces.NbSplitPoints - the number of splitting points.
                 * SplitContinuity - splits shapes to reduce continuities of curves and surfaces.
                     * SplitContinuity.Tolerance3d - 3D tolerance for correction of geometry.
                     * SplitContinuity.SurfaceContinuity - required continuity for surfaces.
                     * SplitContinuity.CurveContinuity - required continuity for curves.
                       This and the previous parameters can take the following values:
                       
                       Parametric Continuity:
                       C0 (Positional Continuity): curves are joined (the end positions of curves or surfaces are
                                                   coincidental. The curves or surfaces may still meet at an angle,
                                                   giving rise to a sharp corner or edge).
                       C1 (Tangential Continuity): first derivatives are equal (the end vectors of curves or surfaces
                                                   are parallel, ruling out sharp edges).
                       C2 (Curvature Continuity): first and second derivatives are equal (the end vectors of curves
                                                  or surfaces are of the same magnitude).
                       CN N-th derivatives are equal (both the direction and the magnitude of the Nth derivatives of
                          curves or surfaces (d/du C(u)) are the same at junction.
                          
                       Geometric Continuity:
                       G1: first derivatives are proportional at junction.
                           The curve tangents thus have the same direction, but not necessarily the same magnitude.
                           i.e., C1'(1) = (a,b,c) and C2'(0) = (k*a, k*b, k*c).
                       G2: first and second derivatives are proportional at junction. As the names imply,
                           geometric continuity requires the geometry to be continuous, while parametric continuity requires
                           that the underlying parameterization was continuous as well. Parametric continuity of order n implies
                           geometric continuity of order n, but not vice-versa.
                 * BsplineRestriction - converts curves and surfaces to Bsplines and processes them with the following parameters:
                     * BSplineRestriction.SurfaceMode - approximation of surfaces if restriction is necessary.
                     * BSplineRestriction.Curve3dMode - conversion of any 3D curve to BSpline and approximation.
                     * BSplineRestriction.Curve2dMode - conversion of any 2D curve to BSpline and approximation.
                     * BSplineRestriction.Tolerance3d - defines the possibility of surfaces and 3D curves approximation with
                                                        the specified parameters.
                     * BSplineRestriction.Tolerance2d - defines the possibility of surfaces and 2D curves approximation with
                                                        the specified parameters.
                     * BSplineRestriction.RequiredDegree - required degree of the resulting BSplines.
                     * BSplineRestriction.RequiredNbSegments - required maximum number of segments of resultant BSplines.
                     * BSplineRestriction.Continuity3d - continuity of the resulting surfaces and 3D curves.
                     * BSplineRestriction.Continuity2d - continuity of the resulting 2D curves.
                 * ToBezier - converts curves and surfaces of any type to Bezier curves and surfaces.
                     * ToBezier.SurfaceMode - if checked in, allows conversion of surfaces.
                     * ToBezier.Curve3dMode - if checked in, allows conversion of 3D curves.
                     * ToBezier.Curve2dMode - if checked in, allows conversion of 2D curves.
                     * ToBezier.MaxTolerance - defines tolerance for detection and correction of problems.
                 * SameParameter - fixes edges of 2D and 3D curves not having the same parameter.
                     * SameParameter.Tolerance3d - defines tolerance for fixing of edges.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.

            Note: For more information look through SALOME Geometry User's Guide->
                  -> Introduction to Geometry-> Repairing Operations-> Shape Processing
            """
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
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_suppress_faces "Example"
        def SuppressFaces(self,theObject, theFaces):
            """
            Remove faces from the given object (shape).

            Parameters:
                theObject Shape to be processed.
                theFaces Indices of faces to be removed, if EMPTY then the method
                         removes ALL faces of the given object.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.SuppressFaces(theObject, theFaces)
            RaiseIfFailed("SuppressFaces", self.HealOp)
            return anObj

        ## Sewing of some shapes into single shape.
        #  @param ListShape Shapes to be processed.
        #  @param theTolerance Required tolerance value.
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_sewing "Example"
        def MakeSewing(self, ListShape, theTolerance):
            """
            Sewing of some shapes into single shape.

            Parameters:
                ListShape Shapes to be processed.
                theTolerance Required tolerance value.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            comp = self.MakeCompound(ListShape)
            anObj = self.Sew(comp, theTolerance)
            return anObj

        ## Sewing of the given object.
        #  @param theObject Shape to be processed.
        #  @param theTolerance Required tolerance value.
        #  @return New GEOM.GEOM_Object, containing processed shape.
        def Sew(self, theObject, theTolerance):
            """
            Sewing of the given object.

            Parameters:
                theObject Shape to be processed.
                theTolerance Required tolerance value.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
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
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_suppress_internal_wires "Example"
        def SuppressInternalWires(self,theObject, theWires):
            """
            Remove internal wires and edges from the given object (face).

            Parameters:
                theObject Shape to be processed.
                theWires Indices of wires to be removed, if EMPTY then the method
                         removes ALL internal wires of the given object.

            Returns:                
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.RemoveIntWires(theObject, theWires)
            RaiseIfFailed("RemoveIntWires", self.HealOp)
            return anObj

        ## Remove internal closed contours (holes) from the given object.
        #  @param theObject Shape to be processed.
        #  @param theWires Indices of wires to be removed, if EMPTY then the method
        #                  removes ALL internal holes of the given object
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_suppress_holes "Example"
        def SuppressHoles(self,theObject, theWires):
            """
            Remove internal closed contours (holes) from the given object.

            Parameters:
                theObject Shape to be processed.
                theWires Indices of wires to be removed, if EMPTY then the method
                         removes ALL internal holes of the given object

            Returns:    
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.FillHoles(theObject, theWires)
            RaiseIfFailed("FillHoles", self.HealOp)
            return anObj

        ## Close an open wire.
        #  @param theObject Shape to be processed.
        #  @param theWires Indexes of edge(s) and wire(s) to be closed within <VAR>theObject</VAR>'s shape,
        #                  if [ ], then <VAR>theObject</VAR> itself is a wire.
        #  @param isCommonVertex If True  : closure by creation of a common vertex,
        #                        If False : closure by creation of an edge between ends.
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_close_contour "Example"
        def CloseContour(self,theObject, theWires, isCommonVertex):
            """
            Close an open wire.

            Parameters: 
                theObject Shape to be processed.
                theWires Indexes of edge(s) and wire(s) to be closed within theObject's shape,
                         if [ ], then theObject itself is a wire.
                isCommonVertex If True  : closure by creation of a common vertex,
                               If False : closure by creation of an edge between ends.

            Returns:                      
                New GEOM.GEOM_Object, containing processed shape. 
            """
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
        #  @param isByParameter If TRUE : \a theValue is treated as a curve parameter [0..1], \n
        #                       if FALSE : \a theValue is treated as a length parameter [0..1]
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_add_point_on_edge "Example"
        def DivideEdge(self,theObject, theEdgeIndex, theValue, isByParameter):
            """
            Addition of a point to a given edge object.

            Parameters: 
                theObject Shape to be processed.
                theEdgeIndex Index of edge to be divided within theObject's shape,
                             if -1, then theObject itself is the edge.
                theValue Value of parameter on edge or length parameter,
                         depending on isByParameter.
                isByParameter If TRUE :  theValue is treated as a curve parameter [0..1],
                              if FALSE : theValue is treated as a length parameter [0..1]

            Returns:  
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            theEdgeIndex,theValue,isByParameter,Parameters = ParseParameters(theEdgeIndex,theValue,isByParameter)
            anObj = self.HealOp.DivideEdge(theObject, theEdgeIndex, theValue, isByParameter)
            RaiseIfFailed("DivideEdge", self.HealOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Suppress the vertices in the wire in case if adjacent edges are C1 continuous.
        #  @param theWire Wire to minimize the number of C1 continuous edges in.
        #  @param theVertices A list of vertices to suppress. If the list
        #                     is empty, all vertices in a wire will be assumed.
        #  @return New GEOM.GEOM_Object with modified wire.
        #
        #  @ref tui_fuse_collinear_edges "Example"
        def FuseCollinearEdgesWithinWire(self, theWire, theVertices = []):
            """
            Suppress the vertices in the wire in case if adjacent edges are C1 continuous.

            Parameters: 
                theWire Wire to minimize the number of C1 continuous edges in.
                theVertices A list of vertices to suppress. If the list
                            is empty, all vertices in a wire will be assumed.

            Returns:  
                New GEOM.GEOM_Object with modified wire.
            """
            anObj = self.HealOp.FuseCollinearEdgesWithinWire(theWire, theVertices)
            RaiseIfFailed("FuseCollinearEdgesWithinWire", self.HealOp)
            return anObj

        ## Change orientation of the given object. Updates given shape.
        #  @param theObject Shape to be processed.
        #  @return Updated <var>theObject</var>
        #
        #  @ref swig_todo "Example"
        def ChangeOrientationShell(self,theObject):
            """
            Change orientation of the given object. Updates given shape.

            Parameters: 
                theObject Shape to be processed.

            Returns:  
                Updated theObject
            """
            theObject = self.HealOp.ChangeOrientation(theObject)
            RaiseIfFailed("ChangeOrientation", self.HealOp)
            pass

        ## Change orientation of the given object.
        #  @param theObject Shape to be processed.
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref swig_todo "Example"
        def ChangeOrientationShellCopy(self, theObject):
            """
            Change orientation of the given object.

            Parameters:
                theObject Shape to be processed.

            Returns:   
                New GEOM.GEOM_Object, containing processed shape.
            """
            anObj = self.HealOp.ChangeOrientationCopy(theObject)
            RaiseIfFailed("ChangeOrientationCopy", self.HealOp)
            return anObj

        ## Try to limit tolerance of the given object by value \a theTolerance.
        #  @param theObject Shape to be processed.
        #  @param theTolerance Required tolerance value.
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_limit_tolerance "Example"
        def LimitTolerance(self, theObject, theTolerance = 1e-07):
            """
            Try to limit tolerance of the given object by value theTolerance.

            Parameters:
                theObject Shape to be processed.
                theTolerance Required tolerance value.

            Returns:   
                New GEOM.GEOM_Object, containing processed shape.
            """
            anObj = self.HealOp.LimitTolerance(theObject, theTolerance)
            RaiseIfFailed("LimitTolerance", self.HealOp)
            return anObj

        ## Get a list of wires (wrapped in GEOM.GEOM_Object-s),
        #  that constitute a free boundary of the given shape.
        #  @param theObject Shape to get free boundary of.
        #  @return [\a status, \a theClosedWires, \a theOpenWires]
        #  \n \a status: FALSE, if an error(s) occured during the method execution.
        #  \n \a theClosedWires: Closed wires on the free boundary of the given shape.
        #  \n \a theOpenWires: Open wires on the free boundary of the given shape.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetFreeBoundary(self, theObject):
            """
            Get a list of wires (wrapped in GEOM.GEOM_Object-s),
            that constitute a free boundary of the given shape.

            Parameters:
                theObject Shape to get free boundary of.

            Returns: 
                [status, theClosedWires, theOpenWires]
                 status: FALSE, if an error(s) occured during the method execution.
                 theClosedWires: Closed wires on the free boundary of the given shape.
                 theOpenWires: Open wires on the free boundary of the given shape.
            """
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.GetFreeBoundary(theObject)
            RaiseIfFailed("GetFreeBoundary", self.HealOp)
            return anObj

        ## Replace coincident faces in theShape by one face.
        #  @param theShape Initial shape.
        #  @param theTolerance Maximum distance between faces, which can be considered as coincident.
        #  @param doKeepNonSolids If FALSE, only solids will present in the result,
        #                         otherwise all initial shapes.
        #  @return New GEOM.GEOM_Object, containing a copy of theShape without coincident faces.
        #
        #  @ref tui_glue_faces "Example"
        def MakeGlueFaces(self, theShape, theTolerance, doKeepNonSolids=True):
            """
            Replace coincident faces in theShape by one face.

            Parameters:
                theShape Initial shape.
                theTolerance Maximum distance between faces, which can be considered as coincident.
                doKeepNonSolids If FALSE, only solids will present in the result,
                                otherwise all initial shapes.

            Returns:
                New GEOM.GEOM_Object, containing a copy of theShape without coincident faces.
            """
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
        #  @return GEOM.ListOfGO
        #
        #  @ref tui_glue_faces "Example"
        def GetGlueFaces(self, theShape, theTolerance):
            """
            Find coincident faces in theShape for possible gluing.

            Parameters:
                theShape Initial shape.
                theTolerance Maximum distance between faces,
                             which can be considered as coincident.

            Returns:                    
                GEOM.ListOfGO
            """
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
        #  @param doGlueAllEdges If TRUE, all coincident edges of <VAR>theShape</VAR>
        #                        will be glued, otherwise only the edges,
        #                        belonging to <VAR>theFaces</VAR>.
        #  @return New GEOM.GEOM_Object, containing a copy of theShape
        #          without some faces.
        #
        #  @ref tui_glue_faces "Example"
        def MakeGlueFacesByList(self, theShape, theTolerance, theFaces,
                                doKeepNonSolids=True, doGlueAllEdges=True):
            """
            Replace coincident faces in theShape by one face
            in compliance with given list of faces

            Parameters:
                theShape Initial shape.
                theTolerance Maximum distance between faces,
                             which can be considered as coincident.
                theFaces List of faces for gluing.
                doKeepNonSolids If FALSE, only solids will present in the result,
                                otherwise all initial shapes.
                doGlueAllEdges If TRUE, all coincident edges of theShape
                               will be glued, otherwise only the edges,
                               belonging to theFaces.

            Returns:
                New GEOM.GEOM_Object, containing a copy of theShape
                    without some faces.
            """
            anObj = self.ShapesOp.MakeGlueFacesByList(theShape, theTolerance, theFaces,
                                                      doKeepNonSolids, doGlueAllEdges)
            if anObj is None:
                raise RuntimeError, "MakeGlueFacesByList : " + self.ShapesOp.GetErrorCode()
            return anObj

        ## Replace coincident edges in theShape by one edge.
        #  @param theShape Initial shape.
        #  @param theTolerance Maximum distance between edges, which can be considered as coincident.
        #  @return New GEOM.GEOM_Object, containing a copy of theShape without coincident edges.
        #
        #  @ref tui_glue_edges "Example"
        def MakeGlueEdges(self, theShape, theTolerance):
            """
            Replace coincident edges in theShape by one edge.

            Parameters:
                theShape Initial shape.
                theTolerance Maximum distance between edges, which can be considered as coincident.

            Returns:    
                New GEOM.GEOM_Object, containing a copy of theShape without coincident edges.
            """
            theTolerance,Parameters = ParseParameters(theTolerance)
            anObj = self.ShapesOp.MakeGlueEdges(theShape, theTolerance)
            if anObj is None:
                raise RuntimeError, "MakeGlueEdges : " + self.ShapesOp.GetErrorCode()
            anObj.SetParameters(Parameters)
            return anObj

        ## Find coincident edges in theShape for possible gluing.
        #  @param theShape Initial shape.
        #  @param theTolerance Maximum distance between edges,
        #                      which can be considered as coincident.
        #  @return GEOM.ListOfGO
        #
        #  @ref tui_glue_edges "Example"
        def GetGlueEdges(self, theShape, theTolerance):
            """
            Find coincident edges in theShape for possible gluing.

            Parameters:
                theShape Initial shape.
                theTolerance Maximum distance between edges,
                             which can be considered as coincident.

            Returns:                         
                GEOM.ListOfGO
            """
            anObj = self.ShapesOp.GetGlueEdges(theShape, theTolerance)
            RaiseIfFailed("GetGlueEdges", self.ShapesOp)
            return anObj

        ## Replace coincident edges in theShape by one edge
        #  in compliance with given list of edges.
        #  @param theShape Initial shape.
        #  @param theTolerance Maximum distance between edges,
        #                      which can be considered as coincident.
        #  @param theEdges List of edges for gluing.
        #  @return New GEOM.GEOM_Object, containing a copy of theShape
        #          without some edges.
        #
        #  @ref tui_glue_edges "Example"
        def MakeGlueEdgesByList(self, theShape, theTolerance, theEdges):
            """
            Replace coincident edges in theShape by one edge
            in compliance with given list of edges.

            Parameters:
                theShape Initial shape.
                theTolerance Maximum distance between edges,
                             which can be considered as coincident.
                theEdges List of edges for gluing.

            Returns:  
                New GEOM.GEOM_Object, containing a copy of theShape
                without some edges.
            """
            anObj = self.ShapesOp.MakeGlueEdgesByList(theShape, theTolerance, theEdges)
            if anObj is None:
                raise RuntimeError, "MakeGlueEdgesByList : " + self.ShapesOp.GetErrorCode()
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
        #  @param theOperation Indicates the operation to be done:\n
        #                      1 - Common, 2 - Cut, 3 - Fuse, 4 - Section.
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fuse "Example"
        def MakeBoolean(self,theShape1, theShape2, theOperation):
            """
            Perform one of boolean operations on two given shapes.

            Parameters: 
                theShape1 First argument for boolean operation.
                theShape2 Second argument for boolean operation.
                theOperation Indicates the operation to be done:
                             1 - Common, 2 - Cut, 3 - Fuse, 4 - Section.

            Returns:   
                New GEOM.GEOM_Object, containing the result shape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BoolOp.MakeBoolean(theShape1, theShape2, theOperation)
            RaiseIfFailed("MakeBoolean", self.BoolOp)
            return anObj

        ## Perform Common boolean operation on two given shapes.
        #  @param theShape1 First argument for boolean operation.
        #  @param theShape2 Second argument for boolean operation.
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_common "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        def MakeCommon(self, theShape1, theShape2):
            """
            Perform Common boolean operation on two given shapes.

            Parameters: 
                theShape1 First argument for boolean operation.
                theShape2 Second argument for boolean operation.
 
            Returns:   
                New GEOM.GEOM_Object, containing the result shape.
            """
            # Example: see GEOM_TestOthers.py
            return self.MakeBoolean(theShape1, theShape2, 1)

        ## Perform Cut boolean operation on two given shapes.
        #  @param theShape1 First argument for boolean operation.
        #  @param theShape2 Second argument for boolean operation.
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_cut "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        def MakeCut(self, theShape1, theShape2):
            """
            Perform Cut boolean operation on two given shapes.

            Parameters: 
                theShape1 First argument for boolean operation.
                theShape2 Second argument for boolean operation.
 
            Returns:   
                New GEOM.GEOM_Object, containing the result shape.
            
            """
            # Example: see GEOM_TestOthers.py
            return self.MakeBoolean(theShape1, theShape2, 2)

        ## Perform Fuse boolean operation on two given shapes.
        #  @param theShape1 First argument for boolean operation.
        #  @param theShape2 Second argument for boolean operation.
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fuse "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        def MakeFuse(self, theShape1, theShape2):
            """
            Perform Fuse boolean operation on two given shapes.

            Parameters: 
                theShape1 First argument for boolean operation.
                theShape2 Second argument for boolean operation.
 
            Returns:   
                New GEOM.GEOM_Object, containing the result shape.
            
            """
            # Example: see GEOM_TestOthers.py
            return self.MakeBoolean(theShape1, theShape2, 3)

        ## Perform Section boolean operation on two given shapes.
        #  @param theShape1 First argument for boolean operation.
        #  @param theShape2 Second argument for boolean operation.
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_section "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        def MakeSection(self, theShape1, theShape2):
            """
            Perform Section boolean operation on two given shapes.

            Parameters: 
                theShape1 First argument for boolean operation.
                theShape2 Second argument for boolean operation.
 
            Returns:   
                New GEOM.GEOM_Object, containing the result shape.
            
            """
            # Example: see GEOM_TestOthers.py
            return self.MakeBoolean(theShape1, theShape2, 4)

        # end of l3_boolean
        ## @}

        ## @addtogroup l3_basic_op
        ## @{

        ## Perform partition operation.
        #  @param ListShapes Shapes to be intersected.
        #  @param ListTools Shapes to intersect theShapes.
        #  @param Limit Type of resulting shapes (see ShapeType()).\n
        #         If this parameter is set to -1 ("Auto"), most appropriate shape limit
        #         type will be detected automatically.
        #  @param KeepNonlimitShapes if this parameter == 0, then only shapes of
        #                             target type (equal to Limit) are kept in the result,
        #                             else standalone shapes of lower dimension
        #                             are kept also (if they exist).
        #  @note Each compound from ListShapes and ListTools will be exploded
        #        in order to avoid possible intersection between shapes from this compound.
        #
        #  After implementation new version of PartitionAlgo (October 2006)
        #  other parameters are ignored by current functionality. They are kept
        #  in this function only for support old versions.
        #      @param ListKeepInside Shapes, outside which the results will be deleted.
        #         Each shape from theKeepInside must belong to theShapes also.
        #      @param ListRemoveInside Shapes, inside which the results will be deleted.
        #         Each shape from theRemoveInside must belong to theShapes also.
        #      @param RemoveWebs If TRUE, perform Glue 3D algorithm.
        #      @param ListMaterials Material indices for each shape. Make sence,
        #         only if theRemoveWebs is TRUE.
        #
        #  @return New GEOM.GEOM_Object, containing the result shapes.
        #
        #  @ref tui_partition "Example"
        def MakePartition(self, ListShapes, ListTools=[], ListKeepInside=[], ListRemoveInside=[],
                          Limit=ShapeType["AUTO"], RemoveWebs=0, ListMaterials=[],
                          KeepNonlimitShapes=0):
            """
            Perform partition operation.

            Parameters: 
                ListShapes Shapes to be intersected.
                ListTools Shapes to intersect theShapes.
                Limit Type of resulting shapes (see geompy.ShapeType)
                      If this parameter is set to -1 ("Auto"), most appropriate shape limit
                      type will be detected automatically.
                KeepNonlimitShapes if this parameter == 0, then only shapes of
                                    target type (equal to Limit) are kept in the result,
                                    else standalone shapes of lower dimension
                                    are kept also (if they exist).
            Note:
                    Each compound from ListShapes and ListTools will be exploded
                    in order to avoid possible intersection between shapes from
                    this compound.
                    
            After implementation new version of PartitionAlgo (October 2006) other
            parameters are ignored by current functionality. They are kept in this
            function only for support old versions.
            
            Ignored parameters:
                ListKeepInside Shapes, outside which the results will be deleted.
                               Each shape from theKeepInside must belong to theShapes also.
                ListRemoveInside Shapes, inside which the results will be deleted.
                                 Each shape from theRemoveInside must belong to theShapes also.
                RemoveWebs If TRUE, perform Glue 3D algorithm.
                ListMaterials Material indices for each shape. Make sence, only if theRemoveWebs is TRUE.

            Returns:   
                New GEOM.GEOM_Object, containing the result shapes.
            """
            # Example: see GEOM_TestAll.py
            if Limit == ShapeType["AUTO"]:
                # automatic detection of the most appropriate shape limit type
                lim = GEOM.SHAPE
                for s in ListShapes: lim = min( lim, s.GetMaxShapeType() )
                Limit = EnumToLong(lim)
                pass
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
        #  @note Passed compounds (via ListShapes or via ListTools)
        #           have to consist of nonintersecting shapes.
        #
        #  @return New GEOM.GEOM_Object, containing the result shapes.
        #
        #  @ref swig_todo "Example"
        def MakePartitionNonSelfIntersectedShape(self, ListShapes, ListTools=[],
                                                 ListKeepInside=[], ListRemoveInside=[],
                                                 Limit=ShapeType["AUTO"], RemoveWebs=0,
                                                 ListMaterials=[], KeepNonlimitShapes=0):
            """
            Perform partition operation.
            This method may be useful if it is needed to make a partition for
            compound contains nonintersected shapes. Performance will be better
            since intersection between shapes from compound is not performed.

            Parameters: 
                Description of all parameters as in method geompy.MakePartition
        
            NOTE:
                Passed compounds (via ListShapes or via ListTools)
                have to consist of nonintersecting shapes.

            Returns:   
                New GEOM.GEOM_Object, containing the result shapes.
            """
            if Limit == ShapeType["AUTO"]:
                # automatic detection of the most appropriate shape limit type
                lim = GEOM.SHAPE
                for s in ListShapes: lim = min( lim, s.GetMaxShapeType() )
                Limit = EnumToLong(lim)
                pass
            anObj = self.BoolOp.MakePartitionNonSelfIntersectedShape(ListShapes, ListTools,
                                                                     ListKeepInside, ListRemoveInside,
                                                                     Limit, RemoveWebs, ListMaterials,
                                                                     KeepNonlimitShapes);
            RaiseIfFailed("MakePartitionNonSelfIntersectedShape", self.BoolOp)
            return anObj

        ## See method MakePartition() for more information.
        #
        #  @ref tui_partition "Example 1"
        #  \n @ref swig_Partition "Example 2"
        def Partition(self, ListShapes, ListTools=[], ListKeepInside=[], ListRemoveInside=[],
                      Limit=ShapeType["AUTO"], RemoveWebs=0, ListMaterials=[],
                      KeepNonlimitShapes=0):
            """
            See method geompy.MakePartition for more information.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.MakePartition(ListShapes, ListTools,
                                       ListKeepInside, ListRemoveInside,
                                       Limit, RemoveWebs, ListMaterials,
                                       KeepNonlimitShapes);
            return anObj

        ## Perform partition of the Shape with the Plane
        #  @param theShape Shape to be intersected.
        #  @param thePlane Tool shape, to intersect theShape.
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_partition "Example"
        def MakeHalfPartition(self,theShape, thePlane):
            """
            Perform partition of the Shape with the Plane

            Parameters: 
                theShape Shape to be intersected.
                thePlane Tool shape, to intersect theShape.

            Returns:  
                New GEOM.GEOM_Object, containing the result shape.
            """
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
        #  @return New GEOM.GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example 1"
        #  \n @ref swig_MakeTranslationTwoPoints "Example 2"
        def MakeTranslationTwoPoints(self,theObject, thePoint1, thePoint2):
            """
            Translate the given object along the vector, specified
            by its end points, creating its copy before the translation.

            Parameters: 
                theObject The object to be translated.
                thePoint1 Start point of translation vector.
                thePoint2 End point of translation vector.

            Returns:  
                New GEOM.GEOM_Object, containing the translated object.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.TranslateTwoPointsCopy(theObject, thePoint1, thePoint2)
            RaiseIfFailed("TranslateTwoPointsCopy", self.TrsfOp)
            return anObj

        ## Translate the given object along the vector, specified by its components.
        #  @param theObject The object to be translated.
        #  @param theDX,theDY,theDZ Components of translation vector.
        #  @return Translated GEOM.GEOM_Object.
        #
        #  @ref tui_translation "Example"
        def TranslateDXDYDZ(self,theObject, theDX, theDY, theDZ):
            """
            Translate the given object along the vector, specified by its components.

            Parameters: 
                theObject The object to be translated.
                theDX,theDY,theDZ Components of translation vector.

            Returns: 
                Translated GEOM.GEOM_Object.
            """
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
        #  @return New GEOM.GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example"
        def MakeTranslation(self,theObject, theDX, theDY, theDZ):
            """
            Translate the given object along the vector, specified
            by its components, creating its copy before the translation.

            Parameters: 
                theObject The object to be translated.
                theDX,theDY,theDZ Components of translation vector.

            Returns: 
                New GEOM.GEOM_Object, containing the translated object.
            """
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
        #  @return New GEOM.GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example"
        def MakeTranslationVector(self,theObject, theVector):
            """
            Translate the given object along the given vector,
            creating its copy before the translation.

            Parameters: 
                theObject The object to be translated.
                theVector The translation vector.

            Returns: 
                New GEOM.GEOM_Object, containing the translated object.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.TranslateVectorCopy(theObject, theVector)
            RaiseIfFailed("TranslateVectorCopy", self.TrsfOp)
            return anObj

        ## Translate the given object along the given vector on given distance.
        #  @param theObject The object to be translated.
        #  @param theVector The translation vector.
        #  @param theDistance The translation distance.
        #  @param theCopy Flag used to translate object itself or create a copy.
        #  @return New GEOM.GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example"
        def TranslateVectorDistance(self, theObject, theVector, theDistance, theCopy):
            """
            Translate the given object along the given vector on given distance.

            Parameters: 
                theObject The object to be translated.
                theVector The translation vector.
                theDistance The translation distance.
                theCopy Flag used to translate object itself or create a copy.

            Returns: 
                New GEOM.GEOM_Object, containing the translated object.
            """
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
        #  @return New GEOM.GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example"
        def MakeTranslationVectorDistance(self, theObject, theVector, theDistance):
            """
            Translate the given object along the given vector on given distance,
            creating its copy before the translation.

            Parameters:
                theObject The object to be translated.
                theVector The translation vector.
                theDistance The translation distance.

            Returns: 
                New GEOM.GEOM_Object, containing the translated object.
            """
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
        #  @return New GEOM.GEOM_Object, containing the rotated object.
        #
        #  @ref tui_rotation "Example"
        def Rotate(self,theObject, theAxis, theAngle):
            """
            Rotate the given object around the given axis on the given angle.

            Parameters:
                theObject The object to be rotated.
                theAxis Rotation axis.
                theAngle Rotation angle in radians.

            Returns: 
                New GEOM.GEOM_Object, containing the rotated object.
            """
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
        #  @return New GEOM.GEOM_Object, containing the rotated object.
        #
        #  @ref tui_rotation "Example"
        def MakeRotation(self,theObject, theAxis, theAngle):
            """
            Rotate the given object around the given axis
            on the given angle, creating its copy before the rotatation.

            Parameters:
                theObject The object to be rotated.
                theAxis Rotation axis.
                theAngle Rotation angle in radians.

            Returns:
                New GEOM.GEOM_Object, containing the rotated object.
            """
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
        #  @param theCentPoint central point the axis is the vector perpendicular to the plane
        #  containing the three points.
        #  @param thePoint1,thePoint2 in a perpendicular plane of the axis.
        #  @return New GEOM.GEOM_Object, containing the rotated object.
        #
        #  @ref tui_rotation "Example"
        def MakeRotationThreePoints(self,theObject, theCentPoint, thePoint1, thePoint2):
            """
            Rotate given object around vector perpendicular to plane
            containing three points, creating its copy before the rotatation.

            Parameters:
                theObject The object to be rotated.
                theCentPoint central point  the axis is the vector perpendicular to the plane
                             containing the three points.
                thePoint1,thePoint2  in a perpendicular plane of the axis.

            Returns:
                New GEOM.GEOM_Object, containing the rotated object.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.RotateThreePointsCopy(theObject, theCentPoint, thePoint1, thePoint2)
            RaiseIfFailed("RotateThreePointsCopy", self.TrsfOp)
            return anObj

        ## Scale the given object by the factor, creating its copy before the scaling.
        #  @param theObject The object to be scaled.
        #  @param thePoint Center point for scaling.
        #                  Passing None for it means scaling relatively the origin of global CS.
        #  @param theFactor Scaling factor value.
        #  @return New GEOM.GEOM_Object, containing the scaled shape.
        #
        #  @ref tui_scale "Example"
        def MakeScaleTransform(self, theObject, thePoint, theFactor):
            """
            Scale the given object by the factor, creating its copy before the scaling.

            Parameters:
                theObject The object to be scaled.
                thePoint Center point for scaling.
                         Passing None for it means scaling relatively the origin of global CS.
                theFactor Scaling factor value.

            Returns:    
                New GEOM.GEOM_Object, containing the scaled shape.
            """
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
        #  @return New GEOM.GEOM_Object, containing the scaled shape.
        #
        #  @ref swig_scale "Example"
        def MakeScaleAlongAxes(self, theObject, thePoint, theFactorX, theFactorY, theFactorZ):
            """
            Scale the given object by different factors along coordinate axes,
            creating its copy before the scaling.

            Parameters:
                theObject The object to be scaled.
                thePoint Center point for scaling.
                            Passing None for it means scaling relatively the origin of global CS.
                theFactorX,theFactorY,theFactorZ Scaling factors along each axis.

            Returns:
                New GEOM.GEOM_Object, containing the scaled shape.
            """
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
        #  @return New GEOM.GEOM_Object, containing the mirrored shape.
        #
        #  @ref tui_mirror "Example"
        def MakeMirrorByPlane(self,theObject, thePlane):
            """
            Create an object, symmetrical to the given one relatively the given plane.

            Parameters:
                theObject The object to be mirrored.
                thePlane Plane of symmetry.

            Returns:
                New GEOM.GEOM_Object, containing the mirrored shape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.MirrorPlaneCopy(theObject, thePlane)
            RaiseIfFailed("MirrorPlaneCopy", self.TrsfOp)
            return anObj

        ## Create an object, symmetrical
        #  to the given one relatively the given axis.
        #  @param theObject The object to be mirrored.
        #  @param theAxis Axis of symmetry.
        #  @return New GEOM.GEOM_Object, containing the mirrored shape.
        #
        #  @ref tui_mirror "Example"
        def MakeMirrorByAxis(self,theObject, theAxis):
            """
            Create an object, symmetrical to the given one relatively the given axis.

            Parameters:
                theObject The object to be mirrored.
                theAxis Axis of symmetry.

            Returns: 
                New GEOM.GEOM_Object, containing the mirrored shape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.MirrorAxisCopy(theObject, theAxis)
            RaiseIfFailed("MirrorAxisCopy", self.TrsfOp)
            return anObj

        ## Create an object, symmetrical
        #  to the given one relatively the given point.
        #  @param theObject The object to be mirrored.
        #  @param thePoint Point of symmetry.
        #  @return New GEOM.GEOM_Object, containing the mirrored shape.
        #
        #  @ref tui_mirror "Example"
        def MakeMirrorByPoint(self,theObject, thePoint):
            """
            Create an object, symmetrical
            to the given one relatively the given point.

            Parameters:
                theObject The object to be mirrored.
                thePoint Point of symmetry.

            Returns:  
                New GEOM.GEOM_Object, containing the mirrored shape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.MirrorPointCopy(theObject, thePoint)
            RaiseIfFailed("MirrorPointCopy", self.TrsfOp)
            return anObj

        ## Modify the Location of the given object by LCS,
        #  creating its copy before the setting.
        #  @param theObject The object to be displaced.
        #  @param theStartLCS Coordinate system to perform displacement from it.\n
        #                     If \a theStartLCS is NULL, displacement
        #                     will be performed from global CS.\n
        #                     If \a theObject itself is used as \a theStartLCS,
        #                     its location will be changed to \a theEndLCS.
        #  @param theEndLCS Coordinate system to perform displacement to it.
        #  @return New GEOM.GEOM_Object, containing the displaced shape.
        #
        #  @ref tui_modify_location "Example"
        def MakePosition(self,theObject, theStartLCS, theEndLCS):
            """
            Modify the Location of the given object by LCS, creating its copy before the setting.

            Parameters:
                theObject The object to be displaced.
                theStartLCS Coordinate system to perform displacement from it.
                            If theStartLCS is NULL, displacement
                            will be performed from global CS.
                            If theObject itself is used as theStartLCS,
                            its location will be changed to theEndLCS.
                theEndLCS Coordinate system to perform displacement to it.

            Returns:  
                New GEOM.GEOM_Object, containing the displaced shape.

            Example of usage:
                # create local coordinate systems
                cs1 = geompy.MakeMarker( 0, 0, 0, 1,0,0, 0,1,0)
                cs2 = geompy.MakeMarker(30,40,40, 1,0,0, 0,1,0)
                # modify the location of the given object
                position = geompy.MakePosition(cylinder, cs1, cs2)
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.PositionShapeCopy(theObject, theStartLCS, theEndLCS)
            RaiseIfFailed("PositionShapeCopy", self.TrsfOp)
            return anObj

        ## Modify the Location of the given object by Path,
        #  @param  theObject The object to be displaced.
        #  @param  thePath Wire or Edge along that the object will be translated.
        #  @param  theDistance progress of Path (0 = start location, 1 = end of path location).
        #  @param  theCopy is to create a copy objects if true.
        #  @param  theReverse  0 - for usual direction, 1 - to reverse path direction.
        #  @return New GEOM.GEOM_Object, containing the displaced shape.
        #
        #  @ref tui_modify_location "Example"
        def PositionAlongPath(self,theObject, thePath, theDistance, theCopy, theReverse):
            """
            Modify the Location of the given object by Path

            Parameters:
                 theObject The object to be displaced.
                 thePath Wire or Edge along that the object will be translated.
                 theDistance progress of Path (0 = start location, 1 = end of path location).
                 theCopy is to create a copy objects if true.
                 theReverse  0 - for usual direction, 1 - to reverse path direction.

            Returns:  
                New GEOM.GEOM_Object, containing the displaced shape.

            Example of usage:
                position = geompy.PositionAlongPath(cylinder, circle, 0.75, 1, 1)
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.PositionAlongPath(theObject, thePath, theDistance, theCopy, theReverse)
            RaiseIfFailed("PositionAlongPath", self.TrsfOp)
            return anObj

        ## Create new object as offset of the given one.
        #  @param theObject The base object for the offset.
        #  @param theOffset Offset value.
        #  @return New GEOM.GEOM_Object, containing the offset object.
        #
        #  @ref tui_offset "Example"
        def MakeOffset(self,theObject, theOffset):
            """
            Create new object as offset of the given one.

            Parameters:
                theObject The base object for the offset.
                theOffset Offset value.

            Returns:  
                New GEOM.GEOM_Object, containing the offset object.

            Example of usage:
                 box = geompy.MakeBox(20, 20, 20, 200, 200, 200)
                 # create a new object as offset of the given object
                 offset = geompy.MakeOffset(box, 70.)
            """
            # Example: see GEOM_TestAll.py
            theOffset, Parameters = ParseParameters(theOffset)
            anObj = self.TrsfOp.OffsetShapeCopy(theObject, theOffset)
            RaiseIfFailed("OffsetShapeCopy", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create new object as projection of the given one on a 2D surface.
        #  @param theSource The source object for the projection. It can be a point, edge or wire.
        #  @param theTarget The target object. It can be planar or cylindrical face.
        #  @return New GEOM.GEOM_Object, containing the projection.
        #
        #  @ref tui_projection "Example"
        def MakeProjection(self, theSource, theTarget):
            """
            Create new object as projection of the given one on a 2D surface.

            Parameters:
                theSource The source object for the projection. It can be a point, edge or wire.
                theTarget The target object. It can be planar or cylindrical face.

            Returns:  
                New GEOM.GEOM_Object, containing the projection.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.ProjectShapeCopy(theSource, theTarget)
            RaiseIfFailed("ProjectShapeCopy", self.TrsfOp)
            return anObj

        # -----------------------------------------------------------------------------
        # Patterns
        # -----------------------------------------------------------------------------

        ## Translate the given object along the given vector a given number times
        #  @param theObject The object to be translated.
        #  @param theVector Direction of the translation.
        #  @param theStep Distance to translate on.
        #  @param theNbTimes Quantity of translations to be done.
        #  @return New GEOM.GEOM_Object, containing compound of all
        #          the shapes, obtained after each translation.
        #
        #  @ref tui_multi_translation "Example"
        def MakeMultiTranslation1D(self,theObject, theVector, theStep, theNbTimes):
            """
            Translate the given object along the given vector a given number times

            Parameters:
                theObject The object to be translated.
                theVector Direction of the translation.
                theStep Distance to translate on.
                theNbTimes Quantity of translations to be done.

            Returns:     
                New GEOM.GEOM_Object, containing compound of all
                the shapes, obtained after each translation.

            Example of usage:
                r1d = geompy.MakeMultiTranslation1D(prism, vect, 20, 4)
            """
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
        #  @return New GEOM.GEOM_Object, containing compound of all
        #          the shapes, obtained after each translation.
        #
        #  @ref tui_multi_translation "Example"
        def MakeMultiTranslation2D(self,theObject, theVector1, theStep1, theNbTimes1,
                                   theVector2, theStep2, theNbTimes2):
            """
            Conseqently apply two specified translations to theObject specified number of times.

            Parameters:
                theObject The object to be translated.
                theVector1 Direction of the first translation.
                theStep1 Step of the first translation.
                theNbTimes1 Quantity of translations to be done along theVector1.
                theVector2 Direction of the second translation.
                theStep2 Step of the second translation.
                theNbTimes2 Quantity of translations to be done along theVector2.

            Returns:
                New GEOM.GEOM_Object, containing compound of all
                the shapes, obtained after each translation.

            Example of usage:
                tr2d = geompy.MakeMultiTranslation2D(prism, vect1, 20, 4, vect2, 80, 3)
            """
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
        #  @return New GEOM.GEOM_Object, containing compound of all the
        #          shapes, obtained after each rotation.
        #
        #  @ref tui_multi_rotation "Example"
        def MultiRotate1D(self,theObject, theAxis, theNbTimes):
            """
            Rotate the given object around the given axis a given number times.
            Rotation angle will be 2*PI/theNbTimes.

            Parameters:
                theObject The object to be rotated.
                theAxis The rotation axis.
                theNbTimes Quantity of rotations to be done.

            Returns:     
                New GEOM.GEOM_Object, containing compound of all the
                shapes, obtained after each rotation.

            Example of usage:
                rot1d = geompy.MultiRotate1D(prism, vect, 4)
            """
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
        #  @return New GEOM.GEOM_Object, containing compound of all the
        #          shapes, obtained after each transformation.
        #
        #  @ref tui_multi_rotation "Example"
        def MultiRotate2D(self,theObject, theAxis, theAngle, theNbTimes1, theStep, theNbTimes2):
            """
            Rotate the given object around the
            given axis on the given angle a given number
            times and multi-translate each rotation result.
            Translation direction passes through center of gravity
            of rotated shape and its projection on the rotation axis.

            Parameters:
                theObject The object to be rotated.
                theAxis Rotation axis.
                theAngle Rotation angle in graduces.
                theNbTimes1 Quantity of rotations to be done.
                theStep Translation distance.
                theNbTimes2 Quantity of translations to be done.

            Returns:    
                New GEOM.GEOM_Object, containing compound of all the
                shapes, obtained after each transformation.

            Example of usage:
                rot2d = geompy.MultiRotate2D(prism, vect, 60, 4, 50, 5)
            """
            # Example: see GEOM_TestAll.py
            theAngle, theNbTimes1, theStep, theNbTimes2, Parameters = ParseParameters(theAngle, theNbTimes1, theStep, theNbTimes2)
            anObj = self.TrsfOp.MultiRotate2D(theObject, theAxis, theAngle, theNbTimes1, theStep, theNbTimes2)
            RaiseIfFailed("MultiRotate2D", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## The same, as MultiRotate1D(), but axis is given by direction and point
        #
        #  @ref swig_MakeMultiRotation "Example"
        def MakeMultiRotation1D(self,aShape,aDir,aPoint,aNbTimes):
            """
            The same, as geompy.MultiRotate1D, but axis is given by direction and point

            Example of usage:
                pz = geompy.MakeVertex(0, 0, 100)
                vy = geompy.MakeVectorDXDYDZ(0, 100, 0)
                MultiRot1D = geompy.MakeMultiRotation1D(prism, vy, pz, 6)
            """
            # Example: see GEOM_TestOthers.py
            aVec = self.MakeLine(aPoint,aDir)
            anObj = self.MultiRotate1D(aShape,aVec,aNbTimes)
            return anObj

        ## The same, as MultiRotate2D(), but axis is given by direction and point
        #
        #  @ref swig_MakeMultiRotation "Example"
        def MakeMultiRotation2D(self,aShape,aDir,aPoint,anAngle,nbtimes1,aStep,nbtimes2):
            """
            The same, as MultiRotate2D(), but axis is given by direction and point
            
            Example of usage:
                pz = geompy.MakeVertex(0, 0, 100)
                vy = geompy.MakeVectorDXDYDZ(0, 100, 0)
                MultiRot2D = geompy.MakeMultiRotation2D(f12, vy, pz, 45, 6, 30, 3)
            """
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
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet "Example 1"
        #  \n @ref swig_MakeFilletAll "Example 2"
        def MakeFilletAll(self,theShape, theR):
            """
            Perform a fillet on all edges of the given shape.

            Parameters:
                theShape Shape, to perform fillet on.
                theR Fillet radius.

            Returns: 
                New GEOM.GEOM_Object, containing the result shape.

            Example of usage: 
               filletall = geompy.MakeFilletAll(prism, 10.) 
            """
            # Example: see GEOM_TestOthers.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.LocalOp.MakeFilletAll(theShape, theR)
            RaiseIfFailed("MakeFilletAll", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Perform a fillet on the specified edges/faces of the given shape
        #  @param theShape Shape, to perform fillet on.
        #  @param theR Fillet radius.
        #  @param theShapeType Type of shapes in <VAR>theListShapes</VAR> (see ShapeType())
        #  @param theListShapes Global indices of edges/faces to perform fillet on.
        #    \note Global index of sub-shape can be obtained, using method GetSubShapeID().
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet "Example"
        def MakeFillet(self,theShape, theR, theShapeType, theListShapes):
            """
            Perform a fillet on the specified edges/faces of the given shape

            Parameters:
                theShape Shape, to perform fillet on.
                theR Fillet radius.
                theShapeType Type of shapes in theListShapes (see geompy.ShapeTypes)
                theListShapes Global indices of edges/faces to perform fillet on.

            Note:
                Global index of sub-shape can be obtained, using method geompy.GetSubShapeID

            Returns: 
                New GEOM.GEOM_Object, containing the result shape.

            Example of usage:
                # get the list of IDs (IDList) for the fillet
                prism_edges = geompy.SubShapeAllSortedCentres(prism, geompy.ShapeType["EDGE"])
                IDlist_e = []
                IDlist_e.append(geompy.GetSubShapeID(prism, prism_edges[0]))
                IDlist_e.append(geompy.GetSubShapeID(prism, prism_edges[1]))
                IDlist_e.append(geompy.GetSubShapeID(prism, prism_edges[2]))
                # make a fillet on the specified edges of the given shape
                fillet = geompy.MakeFillet(prism, 10., geompy.ShapeType["EDGE"], IDlist_e)
            """
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

        ## The same that MakeFillet() but with two Fillet Radius R1 and R2
        def MakeFilletR1R2(self, theShape, theR1, theR2, theShapeType, theListShapes):
            """
            The same that geompy.MakeFillet but with two Fillet Radius R1 and R2

            Example of usage:
                # get the list of IDs (IDList) for the fillet
                prism_edges = geompy.SubShapeAllSortedCentres(prism, geompy.ShapeType["EDGE"])
                IDlist_e = []
                IDlist_e.append(geompy.GetSubShapeID(prism, prism_edges[0]))
                IDlist_e.append(geompy.GetSubShapeID(prism, prism_edges[1]))
                IDlist_e.append(geompy.GetSubShapeID(prism, prism_edges[2]))
                # make a fillet on the specified edges of the given shape
                fillet = geompy.MakeFillet(prism, 10., 15., geompy.ShapeType["EDGE"], IDlist_e)
            """
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
        #  @param theShape  Wire Shape to perform fillet on.
        #  @param theR  Fillet radius.
        #  @param theListOfVertexes Global indices of vertexes to perform fillet on.
        #    \note Global index of sub-shape can be obtained, using method GetSubShapeID()
        #    \note The list of vertices could be empty,
        #          in this case fillet will done done at all vertices in wire
        #  @param doIgnoreSecantVertices If FALSE, fillet radius is always limited
        #         by the length of the edges, nearest to the fillet vertex.
        #         But sometimes the next edge is C1 continuous with the one, nearest to
        #         the fillet point, and such two (or more) edges can be united to allow
        #         bigger radius. Set this flag to TRUE to allow collinear edges union,
        #         thus ignoring the secant vertex (vertices).
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet2d "Example"
        def MakeFillet1D(self,theShape, theR, theListOfVertexes, doIgnoreSecantVertices = True):
            """
            Perform a fillet on the specified edges of the given shape

            Parameters:
                theShape  Wire Shape to perform fillet on.
                theR  Fillet radius.
                theListOfVertexes Global indices of vertexes to perform fillet on.
                doIgnoreSecantVertices If FALSE, fillet radius is always limited
                    by the length of the edges, nearest to the fillet vertex.
                    But sometimes the next edge is C1 continuous with the one, nearest to
                    the fillet point, and such two (or more) edges can be united to allow
                    bigger radius. Set this flag to TRUE to allow collinear edges union,
                    thus ignoring the secant vertex (vertices).
            Note:
                Global index of sub-shape can be obtained, using method geompy.GetSubShapeID

                The list of vertices could be empty,in this case fillet will done done at all vertices in wire

            Returns: 
                New GEOM.GEOM_Object, containing the result shape.

            Example of usage:  
                # create wire
                Wire_1 = geompy.MakeWire([Edge_12, Edge_7, Edge_11, Edge_6, Edge_1,Edge_4])
                # make fillet at given wire vertices with giver radius
                Fillet_1D_1 = geompy.MakeFillet1D(Wire_1, 55, [3, 4, 6, 8, 10])
            """
            # Example: see GEOM_TestAll.py
            theR,doIgnoreSecantVertices,Parameters = ParseParameters(theR,doIgnoreSecantVertices)
            anObj = self.LocalOp.MakeFillet1D(theShape, theR, theListOfVertexes, doIgnoreSecantVertices)
            RaiseIfFailed("MakeFillet1D", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Perform a fillet at the specified vertices of the given face/shell.
        #  @param theShape Face or Shell shape to perform fillet on.
        #  @param theR Fillet radius.
        #  @param theListOfVertexes Global indices of vertexes to perform fillet on.
        #    \note Global index of sub-shape can be obtained, using method GetSubShapeID().
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet2d "Example"
        def MakeFillet2D(self, theShape, theR, theListOfVertexes):
            """
            Perform a fillet at the specified vertices of the given face/shell.

            Parameters:
                theShape  Face or Shell shape to perform fillet on.
                theR  Fillet radius.
                theListOfVertexes Global indices of vertexes to perform fillet on.
            Note:
                Global index of sub-shape can be obtained, using method geompy.GetSubShapeID

            Returns: 
                New GEOM.GEOM_Object, containing the result shape.

            Example of usage:
                face = geompy.MakeFaceHW(100, 100, 1)
                fillet2d = geompy.MakeFillet2D(face, 30, [7, 9])
            """
            # Example: see GEOM_TestAll.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.LocalOp.MakeFillet2D(theShape, theR, theListOfVertexes)
            RaiseIfFailed("MakeFillet2D", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Perform a symmetric chamfer on all edges of the given shape.
        #  @param theShape Shape, to perform chamfer on.
        #  @param theD Chamfer size along each face.
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_chamfer "Example 1"
        #  \n @ref swig_MakeChamferAll "Example 2"
        def MakeChamferAll(self,theShape, theD):
            """
            Perform a symmetric chamfer on all edges of the given shape.

            Parameters:
                theShape Shape, to perform chamfer on.
                theD Chamfer size along each face.

            Returns:     
                New GEOM.GEOM_Object, containing the result shape.

            Example of usage:
                chamfer_all = geompy.MakeChamferAll(prism, 10.)
            """
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
        #    \note Global index of sub-shape can be obtained, using method GetSubShapeID().
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_chamfer "Example"
        def MakeChamferEdge(self,theShape, theD1, theD2, theFace1, theFace2):
            """
            Perform a chamfer on edges, common to the specified faces,
            with distance D1 on the Face1

            Parameters:
                theShape Shape, to perform chamfer on.
                theD1 Chamfer size along theFace1.
                theD2 Chamfer size along theFace2.
                theFace1,theFace2 Global indices of two faces of theShape.

            Note:
                Global index of sub-shape can be obtained, using method geompy.GetSubShapeID

            Returns:      
                New GEOM.GEOM_Object, containing the result shape.

            Example of usage:
                prism_faces = geompy.SubShapeAllSortedCentres(prism, geompy.ShapeType["FACE"])
                f_ind_1 = geompy.GetSubShapeID(prism, prism_faces[0])
                f_ind_2 = geompy.GetSubShapeID(prism, prism_faces[1])
                chamfer_e = geompy.MakeChamferEdge(prism, 10., 10., f_ind_1, f_ind_2)
            """
            # Example: see GEOM_TestAll.py
            theD1,theD2,Parameters = ParseParameters(theD1,theD2)
            anObj = self.LocalOp.MakeChamferEdge(theShape, theD1, theD2, theFace1, theFace2)
            RaiseIfFailed("MakeChamferEdge", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Perform a chamfer on edges
        #  @param theShape Shape, to perform chamfer on.
        #  @param theD Chamfer length
        #  @param theAngle Angle of chamfer (angle in radians or a name of variable which defines angle in degrees)
        #  @param theFace1,theFace2 Global indices of two faces of \a theShape.
        #    \note Global index of sub-shape can be obtained, using method GetSubShapeID().
        #  @return New GEOM.GEOM_Object, containing the result shape.
        def MakeChamferEdgeAD(self, theShape, theD, theAngle, theFace1, theFace2):
            """
            Perform a chamfer on edges

            Parameters:
                theShape Shape, to perform chamfer on.
                theD1 Chamfer size along theFace1.
                theAngle Angle of chamfer (angle in radians or a name of variable which defines angle in degrees).
                theFace1,theFace2 Global indices of two faces of theShape.

            Note:
                Global index of sub-shape can be obtained, using method geompy.GetSubShapeID

            Returns:      
                New GEOM.GEOM_Object, containing the result shape.

            Example of usage:
                prism_faces = geompy.SubShapeAllSortedCentres(prism, geompy.ShapeType["FACE"])
                f_ind_1 = geompy.GetSubShapeID(prism, prism_faces[0])
                f_ind_2 = geompy.GetSubShapeID(prism, prism_faces[1])
                ang = 30
                chamfer_e = geompy.MakeChamferEdge(prism, 10., ang, f_ind_1, f_ind_2)
            """
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
        #    \note Global index of sub-shape can be obtained, using method GetSubShapeID().
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_chamfer "Example"
        def MakeChamferFaces(self,theShape, theD1, theD2, theFaces):
            """
            Perform a chamfer on all edges of the specified faces,
            with distance D1 on the first specified face (if several for one edge)

            Parameters:
                theShape Shape, to perform chamfer on.
                theD1 Chamfer size along face from  theFaces. If both faces,
                      connected to the edge, are in theFaces, theD1
                      will be get along face, which is nearer to theFaces beginning.
                theD2 Chamfer size along another of two faces, connected to the edge.
                theFaces Sequence of global indices of faces of theShape.

                
            Note: Global index of sub-shape can be obtained, using method geompy.GetSubShapeID().

            Returns:  
                New GEOM.GEOM_Object, containing the result shape.
            """
            # Example: see GEOM_TestAll.py
            theD1,theD2,Parameters = ParseParameters(theD1,theD2)
            anObj = self.LocalOp.MakeChamferFaces(theShape, theD1, theD2, theFaces)
            RaiseIfFailed("MakeChamferFaces", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## The Same that MakeChamferFaces() but with params theD is chamfer lenght and
        #  theAngle is Angle of chamfer (angle in radians or a name of variable which defines angle in degrees)
        #
        #  @ref swig_FilletChamfer "Example"
        def MakeChamferFacesAD(self, theShape, theD, theAngle, theFaces):
            """
            The Same that geompy.MakeChamferFaces but with params theD is chamfer lenght and
            theAngle is Angle of chamfer (angle in radians or a name of variable which defines angle in degrees)
            """
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
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref swig_FilletChamfer "Example"
        def MakeChamferEdges(self, theShape, theD1, theD2, theEdges):
            """
            Perform a chamfer on edges,
            with distance D1 on the first specified face (if several for one edge)
            
            Parameters:
                theShape Shape, to perform chamfer on.
                theD1,theD2 Chamfer size
                theEdges Sequence of edges of theShape.

            Returns:
                New GEOM.GEOM_Object, containing the result shape.
            """
            theD1,theD2,Parameters = ParseParameters(theD1,theD2)
            anObj = self.LocalOp.MakeChamferEdges(theShape, theD1, theD2, theEdges)
            RaiseIfFailed("MakeChamferEdges", self.LocalOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## The Same that MakeChamferEdges() but with params theD is chamfer lenght and
        #  theAngle is Angle of chamfer (angle in radians or a name of variable which defines angle in degrees)
        def MakeChamferEdgesAD(self, theShape, theD, theAngle, theEdges):
            """
            The Same that geompy.MakeChamferEdges but with params theD is chamfer lenght and
            theAngle is Angle of chamfer (angle in radians or a name of variable which defines angle in degrees)
            """
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

        ## /sa MakeChamferEdge() and MakeChamferFaces()
        #
        #  @ref swig_MakeChamfer "Example"
        def MakeChamfer(self,aShape,d1,d2,aShapeType,ListShape):
            """
            See geompy.MakeChamferEdge() and geompy.MakeChamferFaces() functions for more information.
            """
            # Example: see GEOM_TestOthers.py
            anObj = None
            if aShapeType == ShapeType["EDGE"]:
                anObj = self.MakeChamferEdge(aShape,d1,d2,ListShape[0],ListShape[1])
            else:
                anObj = self.MakeChamferFaces(aShape,d1,d2,ListShape)
            return anObj
            
        ## Remove material from a solid by extrusion of the base shape on the given distance.
        #  @param theInit Shape to remove material from. It must be a solid or 
        #  a compound made of a single solid.
        #  @param theBase Closed edge or wire defining the base shape to be extruded.
        #  @param theH Prism dimension along the normal to theBase
        #  @param theAngle Draft angle in degrees.
        #  @return New GEOM.GEOM_Object, containing the initial shape with removed material 
        #
        #  @ref tui_creation_prism "Example"
        def MakeExtrudedCut(self, theInit, theBase, theH, theAngle):
            """
            Add material to a solid by extrusion of the base shape on the given distance.

            Parameters:
                theInit Shape to remove material from. It must be a solid or a compound made of a single solid.
                theBase Closed edge or wire defining the base shape to be extruded.
                theH Prism dimension along the normal  to theBase
                theAngle Draft angle in degrees.

            Returns:
                New GEOM.GEOM_Object,  containing the initial shape with removed material.
            """
            # Example: see GEOM_TestAll.py
            #theH,Parameters = ParseParameters(theH)
            anObj = self.PrimOp.MakeDraftPrism(theInit, theBase, theH, theAngle, False)
            RaiseIfFailed("MakeExtrudedBoss", self.PrimOp)
            #anObj.SetParameters(Parameters)
            return anObj   
            
        ## Add material to a solid by extrusion of the base shape on the given distance.
        #  @param theInit Shape to add material to. It must be a solid or 
        #  a compound made of a single solid.
        #  @param theBase Closed edge or wire defining the base shape to be extruded.
        #  @param theH Prism dimension along the normal to theBase
        #  @param theAngle Draft angle in degrees.
        #  @return New GEOM.GEOM_Object, containing the initial shape with added material 
        #
        #  @ref tui_creation_prism "Example"
        def MakeExtrudedBoss(self, theInit, theBase, theH, theAngle):
            """
            Add material to a solid by extrusion of the base shape on the given distance.

            Parameters:
                theInit Shape to add material to. It must be a solid or a compound made of a single solid.
                theBase Closed edge or wire defining the base shape to be extruded.
                theH Prism dimension along the normal  to theBase
                theAngle Draft angle in degrees.

            Returns:
                New GEOM.GEOM_Object,  containing the initial shape with added material.
            """
            # Example: see GEOM_TestAll.py
            #theH,Parameters = ParseParameters(theH)
            anObj = self.PrimOp.MakeDraftPrism(theInit, theBase, theH, theAngle, True)
            RaiseIfFailed("MakeExtrudedBoss", self.PrimOp)
            #anObj.SetParameters(Parameters)
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
        #  @return New GEOM.GEOM_Object, containing a section of \a theShape
        #          by a plane, corresponding to water level.
        #
        #  @ref tui_archimede "Example"
        def Archimede(self,theShape, theWeight, theWaterDensity, theMeshDeflection):
            """
            Perform an Archimde operation on the given shape with given parameters.
            The object presenting the resulting face is returned.

            Parameters: 
                theShape Shape to be put in water.
                theWeight Weight og the shape.
                theWaterDensity Density of the water.
                theMeshDeflection Deflection of the mesh, using to compute the section.

            Returns: 
                New GEOM.GEOM_Object, containing a section of theShape
                by a plane, corresponding to water level.
            """
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
            """
            Get point coordinates

            Returns:
                [x, y, z]
            """
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.PointCoordinates(Point)
            RaiseIfFailed("PointCoordinates", self.MeasuOp)
            return aTuple

        ## Get summarized length of all wires,
        #  area of surface and volume of the given shape.
        #  @param theShape Shape to define properties of.
        #  @return [theLength, theSurfArea, theVolume]\n
        #  theLength:   Summarized length of all wires of the given shape.\n
        #  theSurfArea: Area of surface of the given shape.\n
        #  theVolume:   Volume of the given shape.
        #
        #  @ref tui_measurement_tools_page "Example"
        def BasicProperties(self,theShape):
            """
            Get summarized length of all wires,
            area of surface and volume of the given shape.

            Parameters: 
                theShape Shape to define properties of.

            Returns:
                [theLength, theSurfArea, theVolume]
                 theLength:   Summarized length of all wires of the given shape.
                 theSurfArea: Area of surface of the given shape.
                 theVolume:   Volume of the given shape.
            """
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
            """
            Get parameters of bounding box of the given shape

            Parameters: 
                theShape Shape to obtain bounding box of.

            Returns:
                [Xmin,Xmax, Ymin,Ymax, Zmin,Zmax]
                 Xmin,Xmax: Limits of shape along OX axis.
                 Ymin,Ymax: Limits of shape along OY axis.
                 Zmin,Zmax: Limits of shape along OZ axis.
            """
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
            """
            Get inertia matrix and moments of inertia of theShape.

            Parameters: 
                theShape Shape to calculate inertia of.

            Returns:
                [I11,I12,I13, I21,I22,I23, I31,I32,I33, Ix,Iy,Iz]
                 I(1-3)(1-3): Components of the inertia matrix of the given shape.
                 Ix,Iy,Iz:    Moments of inertia of the given shape.
            """
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetInertia(theShape)
            RaiseIfFailed("GetInertia", self.MeasuOp)
            return aTuple

        ## Get if coords are included in the shape (ST_IN or ST_ON)
        #  @param theShape Shape
        #  @param coords list of points coordinates [x1, y1, z1, x2, y2, z2, ...]
        #  @param tolerance to be used (default is 1.0e-7)
        #  @return list_of_boolean = [res1, res2, ...]
        def AreCoordsInside(self, theShape, coords, tolerance=1.e-7):
            """
            Get if coords are included in the shape (ST_IN or ST_ON)
            
            Parameters: 
                theShape Shape
                coords list of points coordinates [x1, y1, z1, x2, y2, z2, ...]
                tolerance to be used (default is 1.0e-7)

            Returns:
                list_of_boolean = [res1, res2, ...]
            """
            return self.MeasuOp.AreCoordsInside(theShape, coords, tolerance)

        ## Get minimal distance between the given shapes.
        #  @param theShape1,theShape2 Shapes to find minimal distance between.
        #  @return Value of the minimal distance between the given shapes.
        #
        #  @ref tui_measurement_tools_page "Example"
        def MinDistance(self, theShape1, theShape2):
            """
            Get minimal distance between the given shapes.
            
            Parameters: 
                theShape1,theShape2 Shapes to find minimal distance between.

            Returns:    
                Value of the minimal distance between the given shapes.
            """
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
            """
            Get minimal distance between the given shapes.

            Parameters: 
                theShape1,theShape2 Shapes to find minimal distance between.

            Returns:  
                Value of the minimal distance between the given shapes.
            """
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetMinDistance(theShape1, theShape2)
            RaiseIfFailed("GetMinDistance", self.MeasuOp)
            aRes = [aTuple[0], aTuple[4] - aTuple[1], aTuple[5] - aTuple[2], aTuple[6] - aTuple[3]]
            return aRes

        ## Get angle between the given shapes in degrees.
        #  @param theShape1,theShape2 Lines or linear edges to find angle between.
        #  @note If both arguments are vectors, the angle is computed in accordance
        #        with their orientations, otherwise the minimum angle is computed.
        #  @return Value of the angle between the given shapes in degrees.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetAngle(self, theShape1, theShape2):
            """
            Get angle between the given shapes in degrees.

            Parameters: 
                theShape1,theShape2 Lines or linear edges to find angle between.

            Note:
                If both arguments are vectors, the angle is computed in accordance
                with their orientations, otherwise the minimum angle is computed.

            Returns:  
                Value of the angle between the given shapes in degrees.
            """
            # Example: see GEOM_TestMeasures.py
            anAngle = self.MeasuOp.GetAngle(theShape1, theShape2)
            RaiseIfFailed("GetAngle", self.MeasuOp)
            return anAngle

        ## Get angle between the given shapes in radians.
        #  @param theShape1,theShape2 Lines or linear edges to find angle between.
        #  @note If both arguments are vectors, the angle is computed in accordance
        #        with their orientations, otherwise the minimum angle is computed.
        #  @return Value of the angle between the given shapes in radians.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetAngleRadians(self, theShape1, theShape2):
            """
            Get angle between the given shapes in radians.

            Parameters: 
                theShape1,theShape2 Lines or linear edges to find angle between.

                
            Note:
                If both arguments are vectors, the angle is computed in accordance
                with their orientations, otherwise the minimum angle is computed.

            Returns:  
                Value of the angle between the given shapes in radians.
            """
            # Example: see GEOM_TestMeasures.py
            anAngle = self.MeasuOp.GetAngle(theShape1, theShape2)*math.pi/180.
            RaiseIfFailed("GetAngle", self.MeasuOp)
            return anAngle

        ## Get angle between the given vectors in degrees.
        #  @param theShape1,theShape2 Vectors to find angle between.
        #  @param theFlag If True, the normal vector is defined by the two vectors cross,
        #                 if False, the opposite vector to the normal vector is used.
        #  @return Value of the angle between the given vectors in degrees.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetAngleVectors(self, theShape1, theShape2, theFlag = True):
            """
            Get angle between the given vectors in degrees.

            Parameters: 
                theShape1,theShape2 Vectors to find angle between.
                theFlag If True, the normal vector is defined by the two vectors cross,
                        if False, the opposite vector to the normal vector is used.

            Returns:  
                Value of the angle between the given vectors in degrees.
            """
            anAngle = self.MeasuOp.GetAngleBtwVectors(theShape1, theShape2)
            if not theFlag:
                anAngle = 360. - anAngle
            RaiseIfFailed("GetAngleVectors", self.MeasuOp)
            return anAngle

        ## The same as GetAngleVectors, but the result is in radians.
        def GetAngleRadiansVectors(self, theShape1, theShape2, theFlag = True):
            """
            Get angle between the given vectors in radians.

            Parameters: 
                theShape1,theShape2 Vectors to find angle between.
                theFlag If True, the normal vector is defined by the two vectors cross,
                        if False, the opposite vector to the normal vector is used.

            Returns:  
                Value of the angle between the given vectors in radians.
            """
            anAngle = self.GetAngleVectors(theShape1, theShape2, theFlag)*math.pi/180.
            return anAngle

        ## @name Curve Curvature Measurement
        #  Methods for receiving radius of curvature of curves
        #  in the given point
        ## @{

        ## Measure curvature of a curve at a point, set by parameter.
        #  @param theCurve a curve.
        #  @param theParam parameter.
        #  @return radius of curvature of \a theCurve.
        #
        #  @ref swig_todo "Example"
        def CurveCurvatureByParam(self, theCurve, theParam):
            """
            Measure curvature of a curve at a point, set by parameter.

            Parameters: 
                theCurve a curve.
                theParam parameter.

            Returns: 
                radius of curvature of theCurve.
            """
            # Example: see GEOM_TestMeasures.py
            aCurv = self.MeasuOp.CurveCurvatureByParam(theCurve,theParam)
            RaiseIfFailed("CurveCurvatureByParam", self.MeasuOp)
            return aCurv

        ## Measure curvature of a curve at a point.
        #  @param theCurve a curve.
        #  @param thePoint given point.
        #  @return radius of curvature of \a theCurve.
        #
        #  @ref swig_todo "Example"
        def CurveCurvatureByPoint(self, theCurve, thePoint):
            """
            Measure curvature of a curve at a point.

            Parameters: 
                theCurve a curve.
                thePoint given point.

            Returns: 
                radius of curvature of theCurve.           
            """
            aCurv = self.MeasuOp.CurveCurvatureByPoint(theCurve,thePoint)
            RaiseIfFailed("CurveCurvatureByPoint", self.MeasuOp)
            return aCurv
        ## @}

        ## @name Surface Curvature Measurement
        #  Methods for receiving max and min radius of curvature of surfaces
        #  in the given point
        ## @{

        ## Measure max radius of curvature of surface.
        #  @param theSurf the given surface.
        #  @param theUParam Value of U-parameter on the referenced surface.
        #  @param theVParam Value of V-parameter on the referenced surface.
        #  @return max radius of curvature of theSurf.
        #
        ## @ref swig_todo "Example"
        def MaxSurfaceCurvatureByParam(self, theSurf, theUParam, theVParam):
            """
            Measure max radius of curvature of surface.

            Parameters: 
                theSurf the given surface.
                theUParam Value of U-parameter on the referenced surface.
                theVParam Value of V-parameter on the referenced surface.
                
            Returns:     
                max radius of curvature of theSurf.
            """
            # Example: see GEOM_TestMeasures.py
            aSurf = self.MeasuOp.MaxSurfaceCurvatureByParam(theSurf,theUParam,theVParam)
            RaiseIfFailed("MaxSurfaceCurvatureByParam", self.MeasuOp)
            return aSurf

        ## Measure max radius of curvature of surface in the given point
        #  @param theSurf the given surface.
        #  @param thePoint given point.
        #  @return max radius of curvature of theSurf.
        #
        ## @ref swig_todo "Example"
        def MaxSurfaceCurvatureByPoint(self, theSurf, thePoint):
            """
            Measure max radius of curvature of surface in the given point.

            Parameters: 
                theSurf the given surface.
                thePoint given point.
                
            Returns:     
                max radius of curvature of theSurf.          
            """
            aSurf = self.MeasuOp.MaxSurfaceCurvatureByPoint(theSurf,thePoint)
            RaiseIfFailed("MaxSurfaceCurvatureByPoint", self.MeasuOp)
            return aSurf

        ## Measure min radius of curvature of surface.
        #  @param theSurf the given surface.
        #  @param theUParam Value of U-parameter on the referenced surface.
        #  @param theVParam Value of V-parameter on the referenced surface.
        #  @return min radius of curvature of theSurf.
        #   
        ## @ref swig_todo "Example"
        def MinSurfaceCurvatureByParam(self, theSurf, theUParam, theVParam):
            """
            Measure min radius of curvature of surface.

            Parameters: 
                theSurf the given surface.
                theUParam Value of U-parameter on the referenced surface.
                theVParam Value of V-parameter on the referenced surface.
                
            Returns:     
                Min radius of curvature of theSurf.
            """
            aSurf = self.MeasuOp.MinSurfaceCurvatureByParam(theSurf,theUParam,theVParam)
            RaiseIfFailed("MinSurfaceCurvatureByParam", self.MeasuOp)
            return aSurf

        ## Measure min radius of curvature of surface in the given point
        #  @param theSurf the given surface.
        #  @param thePoint given point.
        #  @return min radius of curvature of theSurf.
        #
        ## @ref swig_todo "Example"
        def MinSurfaceCurvatureByPoint(self, theSurf, thePoint):
            """
            Measure min radius of curvature of surface in the given point.

            Parameters: 
                theSurf the given surface.
                thePoint given point.
                
            Returns:     
                Min radius of curvature of theSurf.          
            """
            aSurf = self.MeasuOp.MinSurfaceCurvatureByPoint(theSurf,thePoint)
            RaiseIfFailed("MinSurfaceCurvatureByPoint", self.MeasuOp)
            return aSurf
        ## @}

        ## Get min and max tolerances of sub-shapes of theShape
        #  @param theShape Shape, to get tolerances of.
        #  @return [FaceMin,FaceMax, EdgeMin,EdgeMax, VertMin,VertMax]\n
        #  FaceMin,FaceMax: Min and max tolerances of the faces.\n
        #  EdgeMin,EdgeMax: Min and max tolerances of the edges.\n
        #  VertMin,VertMax: Min and max tolerances of the vertices.
        #
        #  @ref tui_measurement_tools_page "Example"
        def Tolerance(self,theShape):
            """
            Get min and max tolerances of sub-shapes of theShape

            Parameters: 
                theShape Shape, to get tolerances of.

            Returns:    
                [FaceMin,FaceMax, EdgeMin,EdgeMax, VertMin,VertMax]
                 FaceMin,FaceMax: Min and max tolerances of the faces.
                 EdgeMin,EdgeMax: Min and max tolerances of the edges.
                 VertMin,VertMax: Min and max tolerances of the vertices.
            """
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
            """
            Obtain description of the given shape (number of sub-shapes of each type)

            Parameters:
                theShape Shape to be described.

            Returns:
                Description of the given shape.
            """
            # Example: see GEOM_TestMeasures.py
            aDescr = self.MeasuOp.WhatIs(theShape)
            RaiseIfFailed("WhatIs", self.MeasuOp)
            return aDescr

        ## Obtain quantity of shapes of the given type in \a theShape.
        #  If \a theShape is of type \a theType, it is also counted.
        #  @param theShape Shape to be described.
        #  @param theType the given ShapeType().
        #  @return Quantity of shapes of type \a theType in \a theShape.
        #
        #  @ref tui_measurement_tools_page "Example"
        def NbShapes (self, theShape, theType):
            """
            Obtain quantity of shapes of the given type in theShape.
            If theShape is of type theType, it is also counted.

            Parameters:
                theShape Shape to be described.
                theType the given geompy.ShapeType

            Returns:
                Quantity of shapes of type theType in theShape.
            """
            # Example: see GEOM_TestMeasures.py
            listSh = self.SubShapeAllIDs(theShape, theType)
            Nb = len(listSh)
            t       = EnumToLong(theShape.GetShapeType())
            theType = EnumToLong(theType)
            if t == theType:
                Nb = Nb + 1
                pass
            return Nb

        ## Obtain quantity of shapes of each type in \a theShape.
        #  The \a theShape is also counted.
        #  @param theShape Shape to be described.
        #  @return Dictionary of ShapeType() with bound quantities of shapes.
        #
        #  @ref tui_measurement_tools_page "Example"
        def ShapeInfo (self, theShape):
            """
            Obtain quantity of shapes of each type in theShape.
            The theShape is also counted.

            Parameters:
                theShape Shape to be described.

            Returns:
                Dictionary of geompy.ShapeType with bound quantities of shapes.
            """
            # Example: see GEOM_TestMeasures.py
            aDict = {}
            for typeSh in ShapeType:
                if typeSh in ( "AUTO", "SHAPE" ): continue
                listSh = self.SubShapeAllIDs(theShape, ShapeType[typeSh])
                Nb = len(listSh)
                if EnumToLong(theShape.GetShapeType()) == ShapeType[typeSh]:
                    Nb = Nb + 1
                    pass
                aDict[typeSh] = Nb
                pass
            return aDict

        ## Get a point, situated at the centre of mass of theShape.
        #  @param theShape Shape to define centre of mass of.
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_measurement_tools_page "Example"
        def MakeCDG(self,theShape):
            """
            Get a point, situated at the centre of mass of theShape.

            Parameters:
                theShape Shape to define centre of mass of.

            Returns:
                New GEOM.GEOM_Object, containing the created point.
            """
            # Example: see GEOM_TestMeasures.py
            anObj = self.MeasuOp.GetCentreOfMass(theShape)
            RaiseIfFailed("GetCentreOfMass", self.MeasuOp)
            return anObj

        ## Get a vertex sub-shape by index depended with orientation.
        #  @param theShape Shape to find sub-shape.
        #  @param theIndex Index to find vertex by this index (starting from zero)
        #  @return New GEOM.GEOM_Object, containing the created vertex.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetVertexByIndex(self,theShape, theIndex):
            """
            Get a vertex sub-shape by index depended with orientation.

            Parameters:
                theShape Shape to find sub-shape.
                theIndex Index to find vertex by this index (starting from zero)

            Returns:
                New GEOM.GEOM_Object, containing the created vertex.
            """
            # Example: see GEOM_TestMeasures.py
            anObj = self.MeasuOp.GetVertexByIndex(theShape, theIndex)
            RaiseIfFailed("GetVertexByIndex", self.MeasuOp)
            return anObj

        ## Get the first vertex of wire/edge depended orientation.
        #  @param theShape Shape to find first vertex.
        #  @return New GEOM.GEOM_Object, containing the created vertex.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetFirstVertex(self,theShape):
            """
            Get the first vertex of wire/edge depended orientation.

            Parameters:
                theShape Shape to find first vertex.

            Returns:    
                New GEOM.GEOM_Object, containing the created vertex.
            """
            # Example: see GEOM_TestMeasures.py
            anObj = self.GetVertexByIndex(theShape, 0)
            RaiseIfFailed("GetFirstVertex", self.MeasuOp)
            return anObj

        ## Get the last vertex of wire/edge depended orientation.
        #  @param theShape Shape to find last vertex.
        #  @return New GEOM.GEOM_Object, containing the created vertex.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetLastVertex(self,theShape):
            """
            Get the last vertex of wire/edge depended orientation.

            Parameters: 
                theShape Shape to find last vertex.

            Returns:   
                New GEOM.GEOM_Object, containing the created vertex.
            """
            # Example: see GEOM_TestMeasures.py
            nb_vert =  self.ShapesOp.NumberOfSubShapes(theShape, ShapeType["VERTEX"])
            anObj = self.GetVertexByIndex(theShape, (nb_vert-1))
            RaiseIfFailed("GetLastVertex", self.MeasuOp)
            return anObj

        ## Get a normale to the given face. If the point is not given,
        #  the normale is calculated at the center of mass.
        #  @param theFace Face to define normale of.
        #  @param theOptionalPoint Point to compute the normale at.
        #  @return New GEOM.GEOM_Object, containing the created vector.
        #
        #  @ref swig_todo "Example"
        def GetNormal(self, theFace, theOptionalPoint = None):
            """
            Get a normale to the given face. If the point is not given,
            the normale is calculated at the center of mass.
            
            Parameters: 
                theFace Face to define normale of.
                theOptionalPoint Point to compute the normale at.

            Returns:   
                New GEOM.GEOM_Object, containing the created vector.
            """
            # Example: see GEOM_TestMeasures.py
            anObj = self.MeasuOp.GetNormal(theFace, theOptionalPoint)
            RaiseIfFailed("GetNormal", self.MeasuOp)
            return anObj

        ## Check a topology of the given shape.
        #  @param theShape Shape to check validity of.
        #  @param theIsCheckGeom If FALSE, only the shape's topology will be checked, \n
        #                        if TRUE, the shape's geometry will be checked also.
        #  @param theReturnStatus If FALSE and if theShape is invalid, a description \n
        #                        of problem is printed.
        #                        if TRUE and if theShape is invalid, the description 
        #                        of problem is also returned.
        #  @return TRUE, if the shape "seems to be valid".
        #
        #  @ref tui_measurement_tools_page "Example"
        def CheckShape(self,theShape, theIsCheckGeom = 0, theReturnStatus = 0):
            """
            Check a topology of the given shape.

            Parameters: 
                theShape Shape to check validity of.
                theIsCheckGeom If FALSE, only the shape's topology will be checked,
                               if TRUE, the shape's geometry will be checked also.
                theReturnStatus If FALSE and if theShape is invalid, a description
                                of problem is printed.
                                if TRUE and if theShape is invalid, the description 
                                of problem is returned.

            Returns:   
                TRUE, if the shape "seems to be valid".
                If theShape is invalid, prints a description of problem.
                This description can also be returned.
            """
            # Example: see GEOM_TestMeasures.py
            if theIsCheckGeom:
                (IsValid, Status) = self.MeasuOp.CheckShapeWithGeometry(theShape)
                RaiseIfFailed("CheckShapeWithGeometry", self.MeasuOp)
            else:
                (IsValid, Status) = self.MeasuOp.CheckShape(theShape)
                RaiseIfFailed("CheckShape", self.MeasuOp)
            if IsValid == 0:
                if theReturnStatus == 0:
                    print Status
            if theReturnStatus == 1:
              return (IsValid, Status)
            return IsValid

        ## Detect self-intersections in the given shape.
        #  @param theShape Shape to check.
        #  @return TRUE, if the shape contains no self-intersections.
        #
        #  @ref tui_measurement_tools_page "Example"
        def CheckSelfIntersections(self, theShape):
            """
            Detect self-intersections in the given shape.

            Parameters: 
                theShape Shape to check.

            Returns:   
                TRUE, if the shape contains no self-intersections.
            """
            # Example: see GEOM_TestMeasures.py
            (IsValid, Pairs) = self.MeasuOp.CheckSelfIntersections(theShape)
            RaiseIfFailed("CheckSelfIntersections", self.MeasuOp)
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
            """
            Get position (LCS) of theShape.
            Origin of the LCS is situated at the shape's center of mass.
            Axes of the LCS are obtained from shape's location or,
            if the shape is a planar face, from position of its plane.

            Parameters: 
                theShape Shape to calculate position of.

            Returns:  
                [Ox,Oy,Oz, Zx,Zy,Zz, Xx,Xy,Xz].
                 Ox,Oy,Oz: Coordinates of shape's LCS origin.
                 Zx,Zy,Zz: Coordinates of shape's LCS normal(main) direction.
                 Xx,Xy,Xz: Coordinates of shape's LCS X direction.
            """
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetPosition(theShape)
            RaiseIfFailed("GetPosition", self.MeasuOp)
            return aTuple

        ## Get kind of theShape.
        #
        #  @param theShape Shape to get a kind of.
        #  @return Returns a kind of shape in terms of <VAR>GEOM.GEOM_IKindOfShape.shape_kind</VAR> enumeration
        #          and a list of parameters, describing the shape.
        #  @note  Concrete meaning of each value, returned via \a theIntegers
        #         or \a theDoubles list depends on the kind() of the shape.
        #
        #  @ref swig_todo "Example"
        def KindOfShape(self,theShape):
            """
            Get kind of theShape.
         
            Parameters: 
                theShape Shape to get a kind of.

            Returns:
                a kind of shape in terms of GEOM_IKindOfShape.shape_kind enumeration
                    and a list of parameters, describing the shape.
            Note:
                Concrete meaning of each value, returned via theIntegers
                or theDoubles list depends on the geompy.kind of the shape
            """
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
        #         If format 'IGES_SCALE' is used instead of 'IGES' or
        #            format 'STEP_SCALE' is used instead of 'STEP',
        #            length unit will be set to 'meter' and result model will be scaled.
        #  @return New GEOM.GEOM_Object, containing the imported shape.
        #
        #  @ref swig_Import_Export "Example"
        def ImportFile(self, theFileName, theFormatName):
            """
            Import a shape from the BREP or IGES or STEP file
            (depends on given format) with given name.

            Parameters: 
                theFileName The file, containing the shape.
                theFormatName Specify format for the file reading.
                    Available formats can be obtained with geompy.InsertOp.ImportTranslators() method.
                    If format 'IGES_SCALE' is used instead of 'IGES' or
                       format 'STEP_SCALE' is used instead of 'STEP',
                       length unit will be set to 'meter' and result model will be scaled.

            Returns:
                New GEOM.GEOM_Object, containing the imported shape.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.InsertOp.ImportFile(theFileName, theFormatName)
            RaiseIfFailed("Import", self.InsertOp)
            return anObj

        ## Deprecated analog of ImportFile()
        def Import(self, theFileName, theFormatName):
            """
            Deprecated analog of geompy.ImportFile
            """
            print "WARNING: Function Import is deprecated, use ImportFile instead"
            anObj = self.InsertOp.ImportFile(theFileName, theFormatName)
            RaiseIfFailed("Import", self.InsertOp)
            return anObj

        ## Shortcut to ImportFile() for BREP format.
        #  Import a shape from the BREP file with given name.
        #  @param theFileName The file, containing the shape.
        #  @return New GEOM.GEOM_Object, containing the imported shape.
        #
        #  @ref swig_Import_Export "Example"
        def ImportBREP(self, theFileName):
            """
            geompy.ImportFile(...) function for BREP format
            Import a shape from the BREP file with given name.

            Parameters: 
                theFileName The file, containing the shape.

            Returns:
                New GEOM.GEOM_Object, containing the imported shape.
            """
            # Example: see GEOM_TestOthers.py
            return self.ImportFile(theFileName, "BREP")

        ## Shortcut to ImportFile() for IGES format
        #  Import a shape from the IGES file with given name.
        #  @param theFileName The file, containing the shape.
        #  @param ignoreUnits If True, file length units will be ignored (set to 'meter')
        #                     and result model will be scaled, if its units are not meters.
        #                     If False (default), file length units will be taken into account.
        #  @return New GEOM.GEOM_Object, containing the imported shape.
        #
        #  @ref swig_Import_Export "Example"
        def ImportIGES(self, theFileName, ignoreUnits = False):
            """
            geompy.ImportFile(...) function for IGES format

            Parameters:
                theFileName The file, containing the shape.
                ignoreUnits If True, file length units will be ignored (set to 'meter')
                            and result model will be scaled, if its units are not meters.
                            If False (default), file length units will be taken into account.

            Returns:
                New GEOM.GEOM_Object, containing the imported shape.
            """
            # Example: see GEOM_TestOthers.py
            if ignoreUnits:
                return self.ImportFile(theFileName, "IGES_SCALE")
            return self.ImportFile(theFileName, "IGES")

        ## Return length unit from given IGES file
        #  @param theFileName The file, containing the shape.
        #  @return String, containing the units name.
        #
        #  @ref swig_Import_Export "Example"
        def GetIGESUnit(self, theFileName):
            """
            Return length units from given IGES file

            Parameters:
                theFileName The file, containing the shape.

            Returns:
                String, containing the units name.
            """
            # Example: see GEOM_TestOthers.py
            aUnitName = self.InsertOp.ReadValue(theFileName, "IGES", "LEN_UNITS")
            return aUnitName

        ## Shortcut to ImportFile() for STEP format
        #  Import a shape from the STEP file with given name.
        #  @param theFileName The file, containing the shape.
        #  @param ignoreUnits If True, file length units will be ignored (set to 'meter')
        #                     and result model will be scaled, if its units are not meters.
        #                     If False (default), file length units will be taken into account.
        #  @return New GEOM.GEOM_Object, containing the imported shape.
        #
        #  @ref swig_Import_Export "Example"
        def ImportSTEP(self, theFileName, ignoreUnits = False):
            """
            geompy.ImportFile(...) function for STEP format

            Parameters:
                theFileName The file, containing the shape.
                ignoreUnits If True, file length units will be ignored (set to 'meter')
                            and result model will be scaled, if its units are not meters.
                            If False (default), file length units will be taken into account.

            Returns:
                New GEOM.GEOM_Object, containing the imported shape.
            """
            # Example: see GEOM_TestOthers.py
            if ignoreUnits:
                return self.ImportFile(theFileName, "STEP_SCALE")
            return self.ImportFile(theFileName, "STEP")

        ## Return length unit from given IGES or STEP file
        #  @param theFileName The file, containing the shape.
        #  @return String, containing the units name.
        #
        #  @ref swig_Import_Export "Example"
        def GetSTEPUnit(self, theFileName):
            """
            Return length units from given STEP file

            Parameters:
                theFileName The file, containing the shape.

            Returns:
                String, containing the units name.
            """
            # Example: see GEOM_TestOthers.py
            aUnitName = self.InsertOp.ReadValue(theFileName, "STEP", "LEN_UNITS")
            return aUnitName

        ## Read a shape from the binary stream, containing its bounding representation (BRep).
        #  @note This method will not be dumped to the python script by DumpStudy functionality.
        #  @note GEOM.GEOM_Object.GetShapeStream() method can be used to obtain the shape's BRep stream.
        #  @param theStream The BRep binary stream.
        #  @return New GEOM_Object, containing the shape, read from theStream.
        #
        #  @ref swig_Import_Export "Example"
        def RestoreShape (self, theStream):
            """
            Read a shape from the binary stream, containing its bounding representation (BRep).

            Note:
                shape.GetShapeStream() method can be used to obtain the shape's BRep stream.

            Parameters: 
                theStream The BRep binary stream.

            Returns:
                New GEOM_Object, containing the shape, read from theStream.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.InsertOp.RestoreShape(theStream)
            RaiseIfFailed("RestoreShape", self.InsertOp)
            return anObj

        ## Export the given shape into a file with given name.
        #  @param theObject Shape to be stored in the file.
        #  @param theFileName Name of the file to store the given shape in.
        #  @param theFormatName Specify format for the shape storage.
        #         Available formats can be obtained with InsertOp.ImportTranslators() method.
        #
        #  @ref swig_Import_Export "Example"
        def Export(self, theObject, theFileName, theFormatName):
            """
            Export the given shape into a file with given name.

            Parameters: 
                theObject Shape to be stored in the file.
                theFileName Name of the file to store the given shape in.
                theFormatName Specify format for the shape storage.
                              Available formats can be obtained with geompy.InsertOp.ImportTranslators() method.
            """
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
            """
            geompy.Export(...) function for BREP format
            """
            # Example: see GEOM_TestOthers.py
            return self.Export(theObject, theFileName, "BREP")

        ## Shortcut to Export() for IGES format
        #
        #  @ref swig_Import_Export "Example"
        def ExportIGES(self,theObject, theFileName):
            """
            geompy.Export(...) function for IGES format
            """
            # Example: see GEOM_TestOthers.py
            return self.Export(theObject, theFileName, "IGES")

        ## Shortcut to Export() for STEP format
        #
        #  @ref swig_Import_Export "Example"
        def ExportSTEP(self,theObject, theFileName):
            """
            geompy.Export(...) function for STEP format
            """
            # Example: see GEOM_TestOthers.py
            return self.Export(theObject, theFileName, "STEP")

        # end of l2_import_export
        ## @}

        ## @addtogroup l3_blocks
        ## @{

        ## Create a quadrangle face from four edges. Order of Edges is not
        #  important. It is  not necessary that edges share the same vertex.
        #  @param E1,E2,E3,E4 Edges for the face bound.
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_building_by_blocks_page "Example"
        def MakeQuad(self,E1, E2, E3, E4):
            """
            Create a quadrangle face from four edges. Order of Edges is not
            important. It is  not necessary that edges share the same vertex.

            Parameters: 
                E1,E2,E3,E4 Edges for the face bound.

            Returns: 
                New GEOM.GEOM_Object, containing the created face.

            Example of usage:               
                qface1 = geompy.MakeQuad(edge1, edge2, edge3, edge4)
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeQuad(E1, E2, E3, E4)
            RaiseIfFailed("MakeQuad", self.BlocksOp)
            return anObj

        ## Create a quadrangle face on two edges.
        #  The missing edges will be built by creating the shortest ones.
        #  @param E1,E2 Two opposite edges for the face.
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_building_by_blocks_page "Example"
        def MakeQuad2Edges(self,E1, E2):
            """
            Create a quadrangle face on two edges.
            The missing edges will be built by creating the shortest ones.

            Parameters: 
                E1,E2 Two opposite edges for the face.

            Returns: 
                New GEOM.GEOM_Object, containing the created face.
            
            Example of usage:
                # create vertices
                p1 = geompy.MakeVertex(  0.,   0.,   0.)
                p2 = geompy.MakeVertex(150.,  30.,   0.)
                p3 = geompy.MakeVertex(  0., 120.,  50.)
                p4 = geompy.MakeVertex(  0.,  40.,  70.)
                # create edges
                edge1 = geompy.MakeEdge(p1, p2)
                edge2 = geompy.MakeEdge(p3, p4)
                # create a quadrangle face from two edges
                qface2 = geompy.MakeQuad2Edges(edge1, edge2)
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeQuad2Edges(E1, E2)
            RaiseIfFailed("MakeQuad2Edges", self.BlocksOp)
            return anObj

        ## Create a quadrangle face with specified corners.
        #  The missing edges will be built by creating the shortest ones.
        #  @param V1,V2,V3,V4 Corner vertices for the face.
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_building_by_blocks_page "Example 1"
        #  \n @ref swig_MakeQuad4Vertices "Example 2"
        def MakeQuad4Vertices(self,V1, V2, V3, V4):
            """
            Create a quadrangle face with specified corners.
            The missing edges will be built by creating the shortest ones.

            Parameters: 
                V1,V2,V3,V4 Corner vertices for the face.

            Returns: 
                New GEOM.GEOM_Object, containing the created face.

            Example of usage:
                # create vertices
                p1 = geompy.MakeVertex(  0.,   0.,   0.)
                p2 = geompy.MakeVertex(150.,  30.,   0.)
                p3 = geompy.MakeVertex(  0., 120.,  50.)
                p4 = geompy.MakeVertex(  0.,  40.,  70.)
                # create a quadrangle from four points in its corners
                qface3 = geompy.MakeQuad4Vertices(p1, p2, p3, p4)
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeQuad4Vertices(V1, V2, V3, V4)
            RaiseIfFailed("MakeQuad4Vertices", self.BlocksOp)
            return anObj

        ## Create a hexahedral solid, bounded by the six given faces. Order of
        #  faces is not important. It is  not necessary that Faces share the same edge.
        #  @param F1,F2,F3,F4,F5,F6 Faces for the hexahedral solid.
        #  @return New GEOM.GEOM_Object, containing the created solid.
        #
        #  @ref tui_building_by_blocks_page "Example 1"
        #  \n @ref swig_MakeHexa "Example 2"
        def MakeHexa(self,F1, F2, F3, F4, F5, F6):
            """
            Create a hexahedral solid, bounded by the six given faces. Order of
            faces is not important. It is  not necessary that Faces share the same edge.

            Parameters: 
                F1,F2,F3,F4,F5,F6 Faces for the hexahedral solid.

            Returns:    
                New GEOM.GEOM_Object, containing the created solid.

            Example of usage:
                solid = geompy.MakeHexa(qface1, qface2, qface3, qface4, qface5, qface6)
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeHexa(F1, F2, F3, F4, F5, F6)
            RaiseIfFailed("MakeHexa", self.BlocksOp)
            return anObj

        ## Create a hexahedral solid between two given faces.
        #  The missing faces will be built by creating the smallest ones.
        #  @param F1,F2 Two opposite faces for the hexahedral solid.
        #  @return New GEOM.GEOM_Object, containing the created solid.
        #
        #  @ref tui_building_by_blocks_page "Example 1"
        #  \n @ref swig_MakeHexa2Faces "Example 2"
        def MakeHexa2Faces(self,F1, F2):
            """
            Create a hexahedral solid between two given faces.
            The missing faces will be built by creating the smallest ones.

            Parameters: 
                F1,F2 Two opposite faces for the hexahedral solid.

            Returns:
                New GEOM.GEOM_Object, containing the created solid.

            Example of usage:
                solid1 = geompy.MakeHexa2Faces(qface1, qface2)
            """
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
        #  @return New GEOM.GEOM_Object, containing the found vertex.
        #
        #  @ref swig_GetPoint "Example"
        def GetPoint(self, theShape, theX, theY, theZ, theEpsilon):
            """
            Get a vertex, found in the given shape by its coordinates.

            Parameters: 
                theShape Block or a compound of blocks.
                theX,theY,theZ Coordinates of the sought vertex.
                theEpsilon Maximum allowed distance between the resulting
                           vertex and point with the given coordinates.

            Returns:                  
                New GEOM.GEOM_Object, containing the found vertex.

            Example of usage:
                pnt = geompy.GetPoint(shape, -50,  50,  50, 0.01)
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetPoint(theShape, theX, theY, theZ, theEpsilon)
            RaiseIfFailed("GetPoint", self.BlocksOp)
            return anObj

        ## Find a vertex of the given shape, which has minimal distance to the given point.
        #  @param theShape Any shape.
        #  @param thePoint Point, close to the desired vertex.
        #  @return New GEOM.GEOM_Object, containing the found vertex.
        #
        #  @ref swig_GetVertexNearPoint "Example"
        def GetVertexNearPoint(self, theShape, thePoint):
            """
            Find a vertex of the given shape, which has minimal distance to the given point.

            Parameters: 
                theShape Any shape.
                thePoint Point, close to the desired vertex.

            Returns:
                New GEOM.GEOM_Object, containing the found vertex.

            Example of usage:
                pmidle = geompy.MakeVertex(50, 0, 50)
                edge1 = geompy.GetEdgeNearPoint(blocksComp, pmidle)
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetVertexNearPoint(theShape, thePoint)
            RaiseIfFailed("GetVertexNearPoint", self.BlocksOp)
            return anObj

        ## Get an edge, found in the given shape by two given vertices.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint1,thePoint2 Points, close to the ends of the desired edge.
        #  @return New GEOM.GEOM_Object, containing the found edge.
        #
        #  @ref swig_GetEdge "Example"
        def GetEdge(self, theShape, thePoint1, thePoint2):
            """
            Get an edge, found in the given shape by two given vertices.

            Parameters: 
                theShape Block or a compound of blocks.
                thePoint1,thePoint2 Points, close to the ends of the desired edge.

            Returns:
                New GEOM.GEOM_Object, containing the found edge.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetEdge(theShape, thePoint1, thePoint2)
            RaiseIfFailed("GetEdge", self.BlocksOp)
            return anObj

        ## Find an edge of the given shape, which has minimal distance to the given point.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint Point, close to the desired edge.
        #  @return New GEOM.GEOM_Object, containing the found edge.
        #
        #  @ref swig_GetEdgeNearPoint "Example"
        def GetEdgeNearPoint(self, theShape, thePoint):
            """
            Find an edge of the given shape, which has minimal distance to the given point.

            Parameters: 
                theShape Block or a compound of blocks.
                thePoint Point, close to the desired edge.

            Returns:
                New GEOM.GEOM_Object, containing the found edge.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetEdgeNearPoint(theShape, thePoint)
            RaiseIfFailed("GetEdgeNearPoint", self.BlocksOp)
            return anObj

        ## Returns a face, found in the given shape by four given corner vertices.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint1,thePoint2,thePoint3,thePoint4 Points, close to the corners of the desired face.
        #  @return New GEOM.GEOM_Object, containing the found face.
        #
        #  @ref swig_todo "Example"
        def GetFaceByPoints(self,theShape, thePoint1, thePoint2, thePoint3, thePoint4):
            """
            Returns a face, found in the given shape by four given corner vertices.

            Parameters:
                theShape Block or a compound of blocks.
                thePoint1,thePoint2,thePoint3,thePoint4 Points, close to the corners of the desired face.

            Returns:
                New GEOM.GEOM_Object, containing the found face.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceByPoints(theShape, thePoint1, thePoint2, thePoint3, thePoint4)
            RaiseIfFailed("GetFaceByPoints", self.BlocksOp)
            return anObj

        ## Get a face of block, found in the given shape by two given edges.
        #  @param theShape Block or a compound of blocks.
        #  @param theEdge1,theEdge2 Edges, close to the edges of the desired face.
        #  @return New GEOM.GEOM_Object, containing the found face.
        #
        #  @ref swig_todo "Example"
        def GetFaceByEdges(self,theShape, theEdge1, theEdge2):
            """
            Get a face of block, found in the given shape by two given edges.

            Parameters:
                theShape Block or a compound of blocks.
                theEdge1,theEdge2 Edges, close to the edges of the desired face.

            Returns:
                New GEOM.GEOM_Object, containing the found face.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceByEdges(theShape, theEdge1, theEdge2)
            RaiseIfFailed("GetFaceByEdges", self.BlocksOp)
            return anObj

        ## Find a face, opposite to the given one in the given block.
        #  @param theBlock Must be a hexahedral solid.
        #  @param theFace Face of \a theBlock, opposite to the desired face.
        #  @return New GEOM.GEOM_Object, containing the found face.
        #
        #  @ref swig_GetOppositeFace "Example"
        def GetOppositeFace(self,theBlock, theFace):
            """
            Find a face, opposite to the given one in the given block.

            Parameters:
                theBlock Must be a hexahedral solid.
                theFace Face of theBlock, opposite to the desired face.

            Returns: 
                New GEOM.GEOM_Object, containing the found face.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetOppositeFace(theBlock, theFace)
            RaiseIfFailed("GetOppositeFace", self.BlocksOp)
            return anObj

        ## Find a face of the given shape, which has minimal distance to the given point.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint Point, close to the desired face.
        #  @return New GEOM.GEOM_Object, containing the found face.
        #
        #  @ref swig_GetFaceNearPoint "Example"
        def GetFaceNearPoint(self, theShape, thePoint):
            """
            Find a face of the given shape, which has minimal distance to the given point.

            Parameters:
                theShape Block or a compound of blocks.
                thePoint Point, close to the desired face.

            Returns:
                New GEOM.GEOM_Object, containing the found face.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceNearPoint(theShape, thePoint)
            RaiseIfFailed("GetFaceNearPoint", self.BlocksOp)
            return anObj

        ## Find a face of block, whose outside normale has minimal angle with the given vector.
        #  @param theBlock Block or a compound of blocks.
        #  @param theVector Vector, close to the normale of the desired face.
        #  @return New GEOM.GEOM_Object, containing the found face.
        #
        #  @ref swig_todo "Example"
        def GetFaceByNormale(self, theBlock, theVector):
            """
            Find a face of block, whose outside normale has minimal angle with the given vector.

            Parameters:
                theBlock Block or a compound of blocks.
                theVector Vector, close to the normale of the desired face.

            Returns:
                New GEOM.GEOM_Object, containing the found face.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceByNormale(theBlock, theVector)
            RaiseIfFailed("GetFaceByNormale", self.BlocksOp)
            return anObj

        ## Find all sub-shapes of type \a theShapeType of the given shape,
        #  which have minimal distance to the given point.
        #  @param theShape Any shape.
        #  @param thePoint Point, close to the desired shape.
        #  @param theShapeType Defines what kind of sub-shapes is searched GEOM::shape_type
        #  @param theTolerance The tolerance for distances comparison. All shapes
        #                      with distances to the given point in interval
        #                      [minimal_distance, minimal_distance + theTolerance] will be gathered.
        #  @return New GEOM_Object, containing a group of all found shapes.
        #
        #  @ref swig_GetShapesNearPoint "Example"
        def GetShapesNearPoint(self, theShape, thePoint, theShapeType, theTolerance = 1e-07):
            """
            Find all sub-shapes of type theShapeType of the given shape,
            which have minimal distance to the given point.

            Parameters:
                theShape Any shape.
                thePoint Point, close to the desired shape.
                theShapeType Defines what kind of sub-shapes is searched (see GEOM::shape_type)
                theTolerance The tolerance for distances comparison. All shapes
                                with distances to the given point in interval
                                [minimal_distance, minimal_distance + theTolerance] will be gathered.

            Returns:
                New GEOM_Object, containing a group of all found shapes.
            """
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
            """
            Check, if the compound of blocks is given.
            To be considered as a compound of blocks, the
            given shape must satisfy the following conditions:
            - Each element of the compound should be a Block (6 faces and 12 edges).
            - A connection between two Blocks should be an entire quadrangle face or an entire edge.
            - The compound should be connexe.
            - The glue between two quadrangle faces should be applied.

            Parameters:
                theCompound The compound to check.

            Returns:
                TRUE, if the given shape is a compound of blocks.
                If theCompound is not valid, prints all discovered errors.            
            """
            # Example: see GEOM_Spanner.py
            (IsValid, BCErrors) = self.BlocksOp.CheckCompoundOfBlocks(theCompound)
            RaiseIfFailed("CheckCompoundOfBlocks", self.BlocksOp)
            if IsValid == 0:
                Descr = self.BlocksOp.PrintBCErrors(theCompound, BCErrors)
                print Descr
            return IsValid

        ## Retrieve all non blocks solids and faces from \a theShape.
        #  @param theShape The shape to explore.
        #  @return A tuple of two GEOM_Objects. The first object is a group of all
        #          non block solids (= not 6 faces, or with 6 faces, but with the
        #          presence of non-quadrangular faces). The second object is a
        #          group of all non quadrangular faces.
        #
        #  @ref tui_measurement_tools_page "Example 1"
        #  \n @ref swig_GetNonBlocks "Example 2"
        def GetNonBlocks (self, theShape):
            """
            Retrieve all non blocks solids and faces from theShape.

            Parameters:
                theShape The shape to explore.

            Returns:
                A tuple of two GEOM_Objects. The first object is a group of all
                non block solids (= not 6 faces, or with 6 faces, but with the
                presence of non-quadrangular faces). The second object is a
                group of all non quadrangular faces.

            Usage:
                (res_sols, res_faces) = geompy.GetNonBlocks(myShape1)
            """
            # Example: see GEOM_Spanner.py
            aTuple = self.BlocksOp.GetNonBlocks(theShape)
            RaiseIfFailed("GetNonBlocks", self.BlocksOp)
            return aTuple

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
            """
            Remove all seam and degenerated edges from theShape.
            Unite faces and edges, sharing one surface. It means that
            this faces must have references to one C++ surface object (handle).

            Parameters:
                theShape The compound or single solid to remove irregular edges from.
                doUnionFaces If True, then unite faces. If False (the default value),
                             do not unite faces.

            Returns: 
                Improved shape.
            """
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
            """
            Check, if the given shape is a blocks compound.
            Fix all detected errors.

            Note:
                Single block can be also fixed by this method.

            Parameters:
                theShape The compound to check and improve.

            Returns: 
                Improved compound.
            """
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
        #  @return List of GEOM.GEOM_Object, containing the retrieved blocks.
        #
        #  @ref tui_explode_on_blocks "Example 1"
        #  \n @ref swig_MakeBlockExplode "Example 2"
        def MakeBlockExplode(self,theCompound, theMinNbFaces, theMaxNbFaces):
            """
            Get all the blocks, contained in the given compound.

            Parameters:
                theCompound The compound to explode.
                theMinNbFaces If solid has lower number of faces, it is not a block.
                theMaxNbFaces If solid has higher number of faces, it is not a block.

            Note:
                If theMaxNbFaces = 0, the maximum number of faces is not restricted.

            Returns:  
                List of GEOM.GEOM_Object, containing the retrieved blocks.
            """
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
        #  @return New GEOM.GEOM_Object, containing the found block.
        #
        #  @ref swig_todo "Example"
        def GetBlockNearPoint(self,theCompound, thePoint):
            """
            Find block, containing the given point inside its volume or on boundary.

            Parameters:
                theCompound Compound, to find block in.
                thePoint Point, close to the desired block. If the point lays on
                         boundary between some blocks, we return block with nearest center.

            Returns:
                New GEOM.GEOM_Object, containing the found block.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetBlockNearPoint(theCompound, thePoint)
            RaiseIfFailed("GetBlockNearPoint", self.BlocksOp)
            return anObj

        ## Find block, containing all the elements, passed as the parts, or maximum quantity of them.
        #  @param theCompound Compound, to find block in.
        #  @param theParts List of faces and/or edges and/or vertices to be parts of the found block.
        #  @return New GEOM.GEOM_Object, containing the found block.
        #
        #  @ref swig_GetBlockByParts "Example"
        def GetBlockByParts(self,theCompound, theParts):
            """
             Find block, containing all the elements, passed as the parts, or maximum quantity of them.

             Parameters:
                theCompound Compound, to find block in.
                theParts List of faces and/or edges and/or vertices to be parts of the found block.

            Returns: 
                New GEOM_Object, containing the found block.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetBlockByParts(theCompound, theParts)
            RaiseIfFailed("GetBlockByParts", self.BlocksOp)
            return anObj

        ## Return all blocks, containing all the elements, passed as the parts.
        #  @param theCompound Compound, to find blocks in.
        #  @param theParts List of faces and/or edges and/or vertices to be parts of the found blocks.
        #  @return List of GEOM.GEOM_Object, containing the found blocks.
        #
        #  @ref swig_todo "Example"
        def GetBlocksByParts(self,theCompound, theParts):
            """
            Return all blocks, containing all the elements, passed as the parts.

            Parameters:
                theCompound Compound, to find blocks in.
                theParts List of faces and/or edges and/or vertices to be parts of the found blocks.

            Returns:
                List of GEOM.GEOM_Object, containing the found blocks.
            """
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
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_multi_transformation "Example"
        def MakeMultiTransformation1D(self,Block, DirFace1, DirFace2, NbTimes):
            """
            Multi-transformate block and glue the result.
            Transformation is defined so, as to superpose direction faces.

            Parameters:
                Block Hexahedral solid to be multi-transformed.
                DirFace1 ID of First direction face.
                DirFace2 ID of Second direction face.
                NbTimes Quantity of transformations to be done.

            Note:
                Unique ID of sub-shape can be obtained, using method GetSubShapeID().

            Returns:
                New GEOM.GEOM_Object, containing the result shape.
            """
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
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_multi_transformation "Example"
        def MakeMultiTransformation2D(self,Block, DirFace1U, DirFace2U, NbTimesU,
                                      DirFace1V, DirFace2V, NbTimesV):
            """
            Multi-transformate block and glue the result.

            Parameters:
                Block Hexahedral solid to be multi-transformed.
                DirFace1U,DirFace2U IDs of Direction faces for the first transformation.
                DirFace1V,DirFace2V IDs of Direction faces for the second transformation.
                NbTimesU,NbTimesV Quantity of transformations to be done.

            Returns:
                New GEOM.GEOM_Object, containing the result shape.
            """
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
        #  @return List of GEOM.GEOM_Object, each of them is a propagation group.
        #
        #  @ref swig_Propagate "Example"
        def Propagate(self,theShape):
            """
            Build all possible propagation groups.
            Propagation group is a set of all edges, opposite to one (main)
            edge of this group directly or through other opposite edges.
            Notion of Opposite Edge make sence only on quadrangle face.

            Parameters:
                theShape Shape to build propagation groups on.

            Returns:
                List of GEOM.GEOM_Object, each of them is a propagation group.
            """
            # Example: see GEOM_TestOthers.py
            listChains = self.BlocksOp.Propagate(theShape)
            RaiseIfFailed("Propagate", self.BlocksOp)
            return listChains

        # end of l3_blocks_op
        ## @}

        ## @addtogroup l3_groups
        ## @{

        ## Creates a new group which will store sub-shapes of theMainShape
        #  @param theMainShape is a GEOM object on which the group is selected
        #  @param theShapeType defines a shape type of the group (see GEOM::shape_type)
        #  @return a newly created GEOM group
        #
        #  @ref tui_working_with_groups_page "Example 1"
        #  \n @ref swig_CreateGroup "Example 2"
        def CreateGroup(self,theMainShape, theShapeType):
            """
            Creates a new group which will store sub-shapes of theMainShape

            Parameters:
               theMainShape is a GEOM object on which the group is selected
               theShapeType defines a shape type of the group:"COMPOUND", "COMPSOLID",
                            "SOLID", "SHELL", "FACE", "WIRE", "EDGE", "VERTEX", "SHAPE".

            Returns:
               a newly created GEOM group

            Example of usage:
                group = geompy.CreateGroup(Box, geompy.ShapeType["FACE"])
                
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.GroupOp.CreateGroup(theMainShape, theShapeType)
            RaiseIfFailed("CreateGroup", self.GroupOp)
            return anObj

        ## Adds a sub-object with ID theSubShapeId to the group
        #  @param theGroup is a GEOM group to which the new sub-shape is added
        #  @param theSubShapeID is a sub-shape ID in the main object.
        #  \note Use method GetSubShapeID() to get an unique ID of the sub-shape
        #
        #  @ref tui_working_with_groups_page "Example"
        def AddObject(self,theGroup, theSubShapeID):
            """
            Adds a sub-object with ID theSubShapeId to the group

            Parameters:
                theGroup       is a GEOM group to which the new sub-shape is added
                theSubShapeID  is a sub-shape ID in the main object.

            Note:
                Use method GetSubShapeID() to get an unique ID of the sub-shape 
            """
            # Example: see GEOM_TestOthers.py
            self.GroupOp.AddObject(theGroup, theSubShapeID)
            if self.GroupOp.GetErrorCode() != "PAL_ELEMENT_ALREADY_PRESENT":
                RaiseIfFailed("AddObject", self.GroupOp)
                pass
            pass

        ## Removes a sub-object with ID \a theSubShapeId from the group
        #  @param theGroup is a GEOM group from which the new sub-shape is removed
        #  @param theSubShapeID is a sub-shape ID in the main object.
        #  \note Use method GetSubShapeID() to get an unique ID of the sub-shape
        #
        #  @ref tui_working_with_groups_page "Example"
        def RemoveObject(self,theGroup, theSubShapeID):
            """
            Removes a sub-object with ID theSubShapeId from the group

            Parameters:
                theGroup is a GEOM group from which the new sub-shape is removed
                theSubShapeID is a sub-shape ID in the main object.

            Note:
                Use method GetSubShapeID() to get an unique ID of the sub-shape
            """
            # Example: see GEOM_TestOthers.py
            self.GroupOp.RemoveObject(theGroup, theSubShapeID)
            RaiseIfFailed("RemoveObject", self.GroupOp)
            pass

        ## Adds to the group all the given shapes. No errors, if some shapes are alredy included.
        #  @param theGroup is a GEOM group to which the new sub-shapes are added.
        #  @param theSubShapes is a list of sub-shapes to be added.
        #
        #  @ref tui_working_with_groups_page "Example"
        def UnionList (self,theGroup, theSubShapes):
            """
            Adds to the group all the given shapes. No errors, if some shapes are alredy included.

            Parameters:
                theGroup is a GEOM group to which the new sub-shapes are added.
                theSubShapes is a list of sub-shapes to be added.
            """
            # Example: see GEOM_TestOthers.py
            self.GroupOp.UnionList(theGroup, theSubShapes)
            RaiseIfFailed("UnionList", self.GroupOp)
            pass

        ## Adds to the group all the given shapes. No errors, if some shapes are alredy included.
        #  @param theGroup is a GEOM group to which the new sub-shapes are added.
        #  @param theSubShapes is a list of indices of sub-shapes to be added.
        #
        #  @ref swig_UnionIDs "Example"
        def UnionIDs(self,theGroup, theSubShapes):
            """
            Adds to the group all the given shapes. No errors, if some shapes are alredy included.

            Parameters:
                theGroup is a GEOM group to which the new sub-shapes are added.
                theSubShapes is a list of indices of sub-shapes to be added.
            """
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
            """
            Removes from the group all the given shapes. No errors, if some shapes are not included.

            Parameters:
                theGroup is a GEOM group from which the sub-shapes are removed.
                theSubShapes is a list of sub-shapes to be removed.
            """
            # Example: see GEOM_TestOthers.py
            self.GroupOp.DifferenceList(theGroup, theSubShapes)
            RaiseIfFailed("DifferenceList", self.GroupOp)
            pass

        ## Removes from the group all the given shapes. No errors, if some shapes are not included.
        #  @param theGroup is a GEOM group from which the sub-shapes are removed.
        #  @param theSubShapes is a list of indices of sub-shapes to be removed.
        #
        #  @ref swig_DifferenceIDs "Example"
        def DifferenceIDs(self,theGroup, theSubShapes):
            """
            Removes from the group all the given shapes. No errors, if some shapes are not included.

            Parameters:
                theGroup is a GEOM group from which the sub-shapes are removed.
                theSubShapes is a list of indices of sub-shapes to be removed.
            """            
            # Example: see GEOM_TestOthers.py
            self.GroupOp.DifferenceIDs(theGroup, theSubShapes)
            RaiseIfFailed("DifferenceIDs", self.GroupOp)
            pass

        ## Union of two groups.
        #  New group is created. It will contain all entities
        #  which are present in groups theGroup1 and theGroup2.
        #  @param theGroup1, theGroup2 are the initial GEOM groups
        #                              to create the united group from.
        #  @return a newly created GEOM group.
        #  @ref tui_union_groups_anchor "Example"
        def UnionGroups (self, theGroup1, theGroup2):
            """
            Union of two groups.
            New group is created. It will contain all entities
            which are present in groups theGroup1 and theGroup2.

            Parameters:
                theGroup1, theGroup2 are the initial GEOM groups
                                     to create the united group from.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.UnionGroups(theGroup1, theGroup2)
            RaiseIfFailed("UnionGroups", self.GroupOp)
            return aGroup

        ## Intersection of two groups.
        #  New group is created. It will contain only those entities
        #  which are present in both groups theGroup1 and theGroup2.
        #  @param theGroup1, theGroup2 are the initial GEOM groups to get common part of.
        #  @return a newly created GEOM group.
        #  @ref tui_intersect_groups_anchor "Example"
        def IntersectGroups (self, theGroup1, theGroup2):
            """
            Intersection of two groups.
            New group is created. It will contain only those entities
            which are present in both groups theGroup1 and theGroup2.

            Parameters:
                theGroup1, theGroup2 are the initial GEOM groups to get common part of.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.IntersectGroups(theGroup1, theGroup2)
            RaiseIfFailed("IntersectGroups", self.GroupOp)
            return aGroup

        ## Cut of two groups.
        #  New group is created. It will contain entities which are
        #  present in group theGroup1 but are not present in group theGroup2.
        #  @param theGroup1 is a GEOM group to include elements of.
        #  @param theGroup2 is a GEOM group to exclude elements of.
        #  @return a newly created GEOM group.
        #  @ref tui_cut_groups_anchor "Example"
        def CutGroups (self, theGroup1, theGroup2):
            """
            Cut of two groups.
            New group is created. It will contain entities which are
            present in group theGroup1 but are not present in group theGroup2.

            Parameters:
                theGroup1 is a GEOM group to include elements of.
                theGroup2 is a GEOM group to exclude elements of.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.CutGroups(theGroup1, theGroup2)
            RaiseIfFailed("CutGroups", self.GroupOp)
            return aGroup

        ## Union of list of groups.
        #  New group is created. It will contain all entities that are
        #  present in groups listed in theGList.
        #  @param theGList is a list of GEOM groups to create the united group from.
        #  @return a newly created GEOM group.
        #  @ref tui_union_groups_anchor "Example"
        def UnionListOfGroups (self, theGList):
            """
            Union of list of groups.
            New group is created. It will contain all entities that are
            present in groups listed in theGList.

            Parameters:
                theGList is a list of GEOM groups to create the united group from.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.UnionListOfGroups(theGList)
            RaiseIfFailed("UnionListOfGroups", self.GroupOp)
            return aGroup

        ## Cut of lists of groups.
        #  New group is created. It will contain only entities
        #  which are present in groups listed in theGList1 but 
        #  are not present in groups from theGList2.
        #  @param theGList1 is a list of GEOM groups to include elements of.
        #  @param theGList2 is a list of GEOM groups to exclude elements of.
        #  @return a newly created GEOM group.
        #  @ref tui_intersect_groups_anchor "Example"
        def IntersectListOfGroups (self, theGList):
            """
            Cut of lists of groups.
            New group is created. It will contain only entities
            which are present in groups listed in theGList1 but 
            are not present in groups from theGList2.

            Parameters:
                theGList1 is a list of GEOM groups to include elements of.
                theGList2 is a list of GEOM groups to exclude elements of.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.IntersectListOfGroups(theGList)
            RaiseIfFailed("IntersectListOfGroups", self.GroupOp)
            return aGroup

        ## Cut of lists of groups.
        #  New group is created. It will contain only entities
        #  which are present in groups listed in theGList1 but 
        #  are not present in groups from theGList2.
        #  @param theGList1 is a list of GEOM groups to include elements of.
        #  @param theGList2 is a list of GEOM groups to exclude elements of.
        #  @return a newly created GEOM group.
        #  @ref tui_cut_groups_anchor "Example"
        def CutListOfGroups (self, theGList1, theGList2):
            """
            Cut of lists of groups.
            New group is created. It will contain only entities
            which are present in groups listed in theGList1 but 
            are not present in groups from theGList2.

            Parameters:
                theGList1 is a list of GEOM groups to include elements of.
                theGList2 is a list of GEOM groups to exclude elements of.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.CutListOfGroups(theGList1, theGList2)
            RaiseIfFailed("CutListOfGroups", self.GroupOp)
            return aGroup

        ## Returns a list of sub-objects ID stored in the group
        #  @param theGroup is a GEOM group for which a list of IDs is requested
        #
        #  @ref swig_GetObjectIDs "Example"
        def GetObjectIDs(self,theGroup):
            """
            Returns a list of sub-objects ID stored in the group

            Parameters:
                theGroup is a GEOM group for which a list of IDs is requested
            """
            # Example: see GEOM_TestOthers.py
            ListIDs = self.GroupOp.GetObjects(theGroup)
            RaiseIfFailed("GetObjects", self.GroupOp)
            return ListIDs

        ## Returns a type of sub-objects stored in the group
        #  @param theGroup is a GEOM group which type is returned.
        #
        #  @ref swig_GetType "Example"
        def GetType(self,theGroup):
            """
            Returns a type of sub-objects stored in the group

            Parameters:
                theGroup is a GEOM group which type is returned.
            """
            # Example: see GEOM_TestOthers.py
            aType = self.GroupOp.GetType(theGroup)
            RaiseIfFailed("GetType", self.GroupOp)
            return aType

        ## Convert a type of geom object from id to string value
        #  @param theId is a GEOM obect type id.
        #  @return type of geom object (POINT, VECTOR, PLANE, LINE, TORUS, ... )
        #  @ref swig_GetType "Example"
        def ShapeIdToType(self, theId):
            """
            Convert a type of geom object from id to string value

            Parameters:
                theId is a GEOM obect type id.
                
            Returns:
                type of geom object (POINT, VECTOR, PLANE, LINE, TORUS, ... )
            """
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
            if theId == 201:
                return "PIPETSHAPE"
            return "Shape Id not exist."

        ## Returns a main shape associated with the group
        #  @param theGroup is a GEOM group for which a main shape object is requested
        #  @return a GEOM object which is a main shape for theGroup
        #
        #  @ref swig_GetMainShape "Example"
        def GetMainShape(self,theGroup):
            """
            Returns a main shape associated with the group

            Parameters:
                theGroup is a GEOM group for which a main shape object is requested

            Returns:
                a GEOM object which is a main shape for theGroup

            Example of usage: BoxCopy = geompy.GetMainShape(CreateGroup)
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.GroupOp.GetMainShape(theGroup)
            RaiseIfFailed("GetMainShape", self.GroupOp)
            return anObj

        ## Create group of edges of theShape, whose length is in range [min_length, max_length].
        #  If include_min/max == 0, edges with length == min/max_length will not be included in result.
        #  @param theShape given shape (see GEOM.GEOM_Object)
        #  @param min_length minimum length of edges of theShape
        #  @param max_length maximum length of edges of theShape
        #  @param include_max indicating if edges with length == max_length should be included in result, 1-yes, 0-no (default=1)
        #  @param include_min indicating if edges with length == min_length should be included in result, 1-yes, 0-no (default=1)
        #  @return a newly created GEOM group of edges
        #  @@ref swig_todo "Example"
        def GetEdgesByLength (self, theShape, min_length, max_length, include_min = 1, include_max = 1):
            """
            Create group of edges of theShape, whose length is in range [min_length, max_length].
            If include_min/max == 0, edges with length == min/max_length will not be included in result.

            Parameters:
                theShape given shape
                min_length minimum length of edges of theShape
                max_length maximum length of edges of theShape
                include_max indicating if edges with length == max_length should be included in result, 1-yes, 0-no (default=1)
                include_min indicating if edges with length == min_length should be included in result, 1-yes, 0-no (default=1)

             Returns:
                a newly created GEOM group of edges.
            """
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
        #  @param min_length minimum length of edges of selected shape
        #  @param max_length maximum length of edges of selected shape
        #  @param include_max indicating if edges with length == max_length should be included in result, 1-yes, 0-no (default=1)
        #  @param include_min indicating if edges with length == min_length should be included in result, 1-yes, 0-no (default=1)
        #  @return a newly created GEOM group of edges
        #  @ref swig_todo "Example"
        def SelectEdges (self, min_length, max_length, include_min = 1, include_max = 1):
            """
            Create group of edges of selected shape, whose length is in range [min_length, max_length].
            If include_min/max == 0, edges with length == min/max_length will not be included in result.

            Parameters:
                min_length minimum length of edges of selected shape
                max_length maximum length of edges of selected shape
                include_max indicating if edges with length == max_length should be included in result, 1-yes, 0-no (default=1)
                include_min indicating if edges with length == min_length should be included in result, 1-yes, 0-no (default=1)

             Returns:
                a newly created GEOM group of edges.
            """
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
        #  @return List of GEOM.GEOM_Object, containing the created shape and propagation groups.
        #
        #  @ref tui_creation_pipetshape "Example"
        def MakePipeTShape(self, theR1, theW1, theL1, theR2, theW2, theL2, theHexMesh=True, theP1=None, theP2=None, theP3=None):
            """
            Create a T-shape object with specified caracteristics for the main
            and the incident pipes (radius, width, half-length).
            The extremities of the main pipe are located on junctions points P1 and P2.
            The extremity of the incident pipe is located on junction point P3.
            If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
            the main plane of the T-shape is XOY.

            Paremeters:
                theR1 Internal radius of main pipe
                theW1 Width of main pipe
                theL1 Half-length of main pipe
                theR2 Internal radius of incident pipe (R2 < R1)
                theW2 Width of incident pipe (R2+W2 < R1+W1)
                theL2 Half-length of incident pipe
                theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
                theP1 1st junction point of main pipe
                theP2 2nd junction point of main pipe
                theP3 Junction point of incident pipe

            Returns:
                List of GEOM_Object, containing the created shape and propagation groups.

            Example of usage:
                # create PipeTShape object
                pipetshape = geompy.MakePipeTShape(80.0, 20.0, 200.0, 50.0, 20.0, 200.0)
                # create PipeTShape object with position
                pipetshape_position = geompy.MakePipeTShape(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, True, P1, P2, P3)
            """
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
        #  @return List of GEOM.GEOM_Object, containing the created shape and propagation groups.
        #
        #  @ref tui_creation_pipetshape "Example"
        def MakePipeTShapeChamfer(self, theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, theHexMesh=True, theP1=None, theP2=None, theP3=None):
            """
            Create a T-shape object with chamfer and with specified caracteristics for the main
            and the incident pipes (radius, width, half-length). The chamfer is
            created on the junction of the pipes.
            The extremities of the main pipe are located on junctions points P1 and P2.
            The extremity of the incident pipe is located on junction point P3.
            If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
            the main plane of the T-shape is XOY.

            Paremeters:
                theR1 Internal radius of main pipe
                theW1 Width of main pipe
                theL1 Half-length of main pipe
                theR2 Internal radius of incident pipe (R2 < R1)
                theW2 Width of incident pipe (R2+W2 < R1+W1)
                theL2 Half-length of incident pipe
                theH Height of the chamfer.
                theW Width of the chamfer.
                theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
                theP1 1st junction point of main pipe
                theP2 2nd junction point of main pipe
                theP3 Junction point of incident pipe

            Returns:
                List of GEOM_Object, containing the created shape and propagation groups.

            Example of usage:
                # create PipeTShape with chamfer object
                pipetshapechamfer = geompy.MakePipeTShapeChamfer(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 20.0, 20.0)
                # create PipeTShape with chamfer object with position
                pipetshapechamfer_position = geompy.MakePipeTShapeChamfer(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 20.0, 20.0, True, P1, P2, P3)
            """
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
        #  @return List of GEOM.GEOM_Object, containing the created shape and propagation groups.
        #
        #  @ref tui_creation_pipetshape "Example"
        def MakePipeTShapeFillet(self, theR1, theW1, theL1, theR2, theW2, theL2, theRF, theHexMesh=True, theP1=None, theP2=None, theP3=None):
            """
            Create a T-shape object with fillet and with specified caracteristics for the main
            and the incident pipes (radius, width, half-length). The fillet is
            created on the junction of the pipes.
            The extremities of the main pipe are located on junctions points P1 and P2.
            The extremity of the incident pipe is located on junction point P3.

            Paremeters:
                If P1, P2 and P3 are not given, the center of the shape is (0,0,0) and
                the main plane of the T-shape is XOY.
                theR1 Internal radius of main pipe
                theW1 Width of main pipe
                heL1 Half-length of main pipe
                theR2 Internal radius of incident pipe (R2 < R1)
                theW2 Width of incident pipe (R2+W2 < R1+W1)
                theL2 Half-length of incident pipe
                theRF Radius of curvature of fillet.
                theHexMesh Boolean indicating if shape is prepared for hex mesh (default=True)
                theP1 1st junction point of main pipe
                theP2 2nd junction point of main pipe
                theP3 Junction point of incident pipe
                
            Returns:
                List of GEOM_Object, containing the created shape and propagation groups.
                
            Example of usage:
                # create PipeTShape with fillet object
                pipetshapefillet = geompy.MakePipeTShapeFillet(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 5.0)
                # create PipeTShape with fillet object with position
                pipetshapefillet_position = geompy.MakePipeTShapeFillet(80.0, 20.0, 200.0, 50.0, 20.0, 200.0, 5.0, True, P1, P2, P3)
        
            """
            theR1, theW1, theL1, theR2, theW2, theL2, theRF, Parameters = ParseParameters(theR1, theW1, theL1, theR2, theW2, theL2, theRF)
            if (theP1 and theP2 and theP3):
              anObj = self.AdvOp.MakePipeTShapeFilletWithPosition(theR1, theW1, theL1, theR2, theW2, theL2, theRF, theHexMesh, theP1, theP2, theP3)
            else:
              anObj = self.AdvOp.MakePipeTShapeFillet(theR1, theW1, theL1, theR2, theW2, theL2, theRF, theHexMesh)
            RaiseIfFailed("MakePipeTShapeFillet", self.AdvOp)
            if Parameters: anObj[0].SetParameters(Parameters)
            return anObj

        ## This function allows creating a disk already divided into blocks. It
        #  can be used to create divided pipes for later meshing in hexaedra.
        #  @param theR Radius of the disk
        #  @param theOrientation Orientation of the plane on which the disk will be built
        #         1 = XOY, 2 = OYZ, 3 = OZX
        #  @param thePattern Division pattern. It can be GEOM.SQUARE or GEOM.HEXAGON
        #  @return New GEOM_Object, containing the created shape.
        #
        #  @ref tui_creation_divideddisk "Example"
        def MakeDividedDisk(self, theR, theOrientation, thePattern ):
            theR, Parameters = ParseParameters(theR)
            anObj = self.AdvOp.MakeDividedDisk(theR, 67.0, theOrientation, thePattern)
            RaiseIfFailed("MakeDividedDisk", self.AdvOp)
            if Parameters: anObj.SetParameters(Parameters)
            return anObj
            
        ## This function allows creating a disk already divided into blocks. It
        #  can be used to create divided pipes for later meshing in hexaedra.
        #  @param theCenter Center of the disk
        #  @param theVector Normal vector to the plane of the created disk
        #  @param theRadius Radius of the disk
        #  @param thePattern Division pattern. It can be GEOM.SQUARE or GEOM.HEXAGON
        #  @return New GEOM_Object, containing the created shape.
        #
        #  @ref tui_creation_divideddisk "Example"
        def MakeDividedDiskPntVecR(self, theCenter, theVector, theRadius, thePattern):
            theRadius, Parameters = ParseParameters(theRadius)
            anObj = self.AdvOp.MakeDividedDiskPntVecR(theCenter, theVector, theRadius, 67.0, thePattern)
            RaiseIfFailed("MakeDividedDiskPntVecR", self.AdvOp)
            if Parameters: anObj.SetParameters(Parameters)
            return anObj

        ## Builds a cylinder prepared for hexa meshes
        #  @param theR Radius of the cylinder
        #  @param theH Height of the cylinder
        #  @param thePattern Division pattern. It can be GEOM.SQUARE or GEOM.HEXAGON
        #  @return New GEOM_Object, containing the created shape.
        #
        #  @ref tui_creation_dividedcylinder "Example"
        def MakeDividedCylinder(self, theR, theH, thePattern):
            theR, theH, Parameters = ParseParameters(theR, theH)
            anObj = self.AdvOp.MakeDividedCylinder(theR, theH, thePattern)
            RaiseIfFailed("MakeDividedCylinder", self.AdvOp)
            if Parameters: anObj.SetParameters(Parameters)
            return anObj

        #@@ insert new functions before this line @@ do not remove this line @@#

        # end of l4_advanced
        ## @}

        ## Create a copy of the given object
        #
        #  @param theOriginal geometry object for copy
        #  @return unique object identifier
        #  @ingroup l1_geompy_auxiliary
        #  @ref swig_MakeCopy "Example"
        def MakeCopy(self,theOriginal):
            """
            Create a copy of the given object

            Paremeters:
                theOriginal geometry object for copy

            Returns:
                unique object identifier

            Example of usage: Copy = geompy.MakeCopy(Box)
            """
            # Example: see GEOM_TestAll.py
            anObj = self.InsertOp.MakeCopy(theOriginal)
            RaiseIfFailed("MakeCopy", self.InsertOp)
            return anObj

        ## Add Path to load python scripts from
        #  @param Path a path to load python scripts from
        #  @ingroup l1_geompy_auxiliary
        def addPath(self,Path):
            """
            Add Path to load python scripts from

            Parameters:
                Path a path to load python scripts from
            """
            if (sys.path.count(Path) < 1):
                sys.path.append(Path)
                pass
            pass

        ## Load marker texture from the file
        #  @param Path a path to the texture file
        #  @return unique texture identifier
        #  @ingroup l1_geompy_auxiliary
        def LoadTexture(self, Path):
            """
            Load marker texture from the file
            
            Parameters:
                Path a path to the texture file
                
            Returns:
                unique texture identifier
            """
            # Example: see GEOM_TestAll.py
            ID = self.InsertOp.LoadTexture(Path)
            RaiseIfFailed("LoadTexture", self.InsertOp)
            return ID

        ## Get internal name of the object based on its study entry
        #  @note This method does not provide an unique identifier of the geometry object.
        #  @note This is internal function of GEOM component, though it can be used outside it for 
        #  appropriate reason (e.g. for identification of geometry object).
        #  @param obj geometry object
        #  @return unique object identifier
        #  @ingroup l1_geompy_auxiliary
        def getObjectID(self, obj):
            """
            Get internal name of the object based on its study entry.
            Note: this method does not provide an unique identifier of the geometry object.
            It is an internal function of GEOM component, though it can be used outside GEOM for 
            appropriate reason (e.g. for identification of geometry object).

            Parameters:
                obj geometry object

            Returns:
                unique object identifier
            """
            ID = ""
            entry = salome.ObjectToID(obj)
            if entry is not None:
                lst = entry.split(":")
                if len(lst) > 0:
                    ID = lst[-1] # -1 means last item in the list            
                    return "GEOM_" + ID
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
        #  @return unique texture identifier
        #  @ingroup l1_geompy_auxiliary
        def AddTexture(self, Width, Height, Texture, RowData=False):
            """
            Add marker texture. Width and Height parameters
            specify width and height of the texture in pixels.
            If RowData is True, Texture parameter should represent texture data
            packed into the byte array. If RowData is False (default), Texture
            parameter should be unpacked string, in which '1' symbols represent opaque
            pixels and '0' represent transparent pixels of the texture bitmap.

            Parameters:
                Width texture width in pixels
                Height texture height in pixels
                Texture texture data
                RowData if True, Texture data are packed in the byte stream

            Returns:
                return unique texture identifier
            """
            if not RowData: Texture = PackData(Texture)
            ID = self.InsertOp.AddTexture(Width, Height, Texture)
            RaiseIfFailed("AddTexture", self.InsertOp)
            return ID

import omniORB
#Register the new proxy for GEOM_Gen
omniORB.registerObjref(GEOM._objref_GEOM_Gen._NP_RepositoryId, geompyDC)
