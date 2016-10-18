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

#  GEOM GEOM_SWIG : binding of C++ implementation with Python
#  File   : geomBuilder.py
#  Author : Paul RASCLE, EDF
#  Module : GEOM

"""
    \namespace geomBuilder
    \brief Module geomBuilder
"""

##
## @defgroup geomBuilder geomBuilder Python module
## @{
##
## @details
##
## By default, all functions of geomBuilder Python module do not publish
## resulting geometrical objects. This can be done in the Python script
## by means of \ref geomBuilder.geomBuilder.addToStudy() "addToStudy()"
## or \ref geomBuilder.geomBuilder.addToStudyInFather() "addToStudyInFather()"
## functions.
##
## However, it is possible to publish result data in the study
## automatically. For this, almost each function of
## \ref geomBuilder.geomBuilder "geomBuilder" class has
## an additional @a theName parameter (@c None by default).
## As soon as non-empty string value is passed to this parameter,
## the result object is published in the study automatically.
##
## For example, consider the following Python script:
##
## @code
## import salome
## from salome.geom import geomBuilder
## geompy = geomBuilder.New(salome.myStudy)
## box = geompy.MakeBoxDXDYDZ(100, 100, 100) # box is not published in the study yet
## geompy.addToStudy(box, "box")             # explicit publishing
## @endcode
##
## Last two lines can be replaced by one-line instruction:
##
## @code
## box = geompy.MakeBoxDXDYDZ(100, 100, 100, theName="box") # box is published in the study with "box" name
## @endcode
##
## ... or simply
##
## @code
## box = geompy.MakeBoxDXDYDZ(100, 100, 100, "box") # box is published in the study with "box" name
## @endcode
##
## Note, that some functions produce more than one geometrical objects. For example,
## \ref geomBuilder.geomBuilder.GetNonBlocks() "GetNonBlocks()" function returns two objects:
## group of all non-hexa solids and group of all non-quad faces.
## For such functions it is possible to specify separate names for results.
##
## For example
##
## @code
## # create and publish cylinder
## cyl = geompy.MakeCylinderRH(100, 100, "cylinder")
## # get non blocks from cylinder
## g1, g2 = geompy.GetNonBlocks(cyl, theName="nonblock")
## @endcode
##
## Above example will publish both result compounds (first with non-hexa solids and
## second with non-quad faces) as two items, both named "nonblock".
## However, if second command is invoked as
##
## @code
## g1, g2 = geompy.GetNonBlocks(cyl, theName=("nonhexa", "nonquad"))
## @endcode
##
## ... the first compound will be published with "nonhexa" name, and second will be named "nonquad".
##
## Automatic publication of all results can be also enabled/disabled by means of the function
## \ref geomBuilder.geomBuilder.addToStudyAuto() "addToStudyAuto()". The automatic publishing
## is managed by the numeric parameter passed to this function:
## - if @a maxNbSubShapes = 0, automatic publishing is disabled.
## - if @a maxNbSubShapes = -1 (default), automatic publishing is enabled and
##   maximum number of sub-shapes allowed for publishing is unlimited; any negative
##   value passed as parameter has the same effect.
## - if @a maxNbSubShapes is any positive value, automatic publishing is enabled and
##   maximum number of sub-shapes allowed for publishing is set to specified value.
##
## When automatic publishing is enabled, you even do not need to pass @a theName parameter
## to the functions creating objects, instead default names will be used. However, you
## can always change the behavior, by passing explicit name to the @a theName parameter
## and it will be used instead default one.
## The publishing of the collections of objects will be done according to the above
## mentioned rules (maximum allowed number of sub-shapes).
##
## For example:
##
## @code
## import salome
## from salome.geom import geomBuilder
## geompy = geomBuilder.New(salome.myStudy)
## geompy.addToStudyAuto() # enable automatic publication
## box = geompy.MakeBoxDXDYDZ(100, 100, 100)
## # the box is created and published in the study with default name
## geompy.addToStudyAuto(5) # set max allowed number of sub-shapes to 5
## vertices = geompy.SubShapeAll(box, geomBuilder.ShapeType['VERTEX'])
## # only 5 first vertices will be published, with default names
## print len(vertices)
## # note, that result value still containes all 8 vertices
## geompy.addToStudyAuto(-1) # disable automatic publication
## @endcode
##
## This feature can be used, for example, for debugging purposes.
##
## @note
## - Use automatic publication feature with caution. When it is enabled, any function of
##   \ref geomBuilder.geomBuilder "geomBuilder" class publishes the results in the study,
##   that can lead to the huge size of the study data tree.
##   For example, repeating call of \ref geomBuilder.geomBuilder.SubShapeAll() "SubShapeAll()"
##   command on the same main shape each time will publish all child objects, that will lead
##   to a lot of duplicated items in the study.
## - Sub-shapes are automatically published as child items of the parent main shape in the study if main
##   shape was also published before. Otherwise, sub-shapes are published as top-level objects.
## - Some functions of \ref geomBuilder.geomBuilder "geomBuilder" class do not have
##   \a theName parameter (and, thus, do not support automatic publication).
##   For example, some transformation operations like
##   \ref geomBuilder.geomBuilder.TranslateDXDYDZ() "TranslateDXDYDZ()".
##   Refer to the documentation to check if some function has such possibility.
##
## It is possible to customize the representation of the geometrical
## data in the data tree; this can be done by using folders. A folder can
## be created in the study tree using function
## \ref geomBuilder.geomBuilder.NewFolder() "NewFolder()"
## (by default it is created under the "Geometry" root object).
## As soon as folder is created, any published geometry object
## can be moved into it.
##
## For example:
##
## @code
## import salome
## from salome.geom import geomBuilder
## geompy = geomBuilder.New(salome.myStudy)
## box = geompy.MakeBoxDXDYDZ(100, 100, 100, "Box")
## # the box was created and published in the study
## folder = geompy.NewFolder("Primitives")
## # an empty "Primitives" folder was created under default "Geometry" root object
## geompy.PutToFolder(box, folder)
## # the box was moved into "Primitives" folder
## @endcode
##
## Subfolders are also can be created by specifying another folder as a parent:
##
## @code
## subfolder = geompy.NewFolder("3D", folder)
## # "3D" folder was created under "Primitives" folder
## @endcode
##
## @note
## - Folder container is just a representation layer object that
## deals with already published objects only. So, any geometry object
## should be published in the study (for example, with
## \ref geomBuilder.geomBuilder.PutToFolder() "addToStudy()" function)
## BEFORE moving it into any existing folder.
## - \ref geomBuilder.geomBuilder.PutToFolder() "PutToFolder()" function
## does not change physical position of geometry object in the study tree,
## it only affects on the representation of the data tree.
## - It is impossible to publish geometry object using any folder as father.
##
##  \defgroup l1_publish_data
##  \defgroup l1_geomBuilder_auxiliary
##  \defgroup l1_geomBuilder_purpose
## @}

## @defgroup l1_publish_data Publishing results in SALOME study

## @defgroup l1_geomBuilder_auxiliary Auxiliary data structures and methods

## @defgroup l1_geomBuilder_purpose   All package methods, grouped by their purpose
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
##     @defgroup l3_transform_d   Transformation Operations deprecated methods
##     @defgroup l3_local         Local Operations (Fillet, Chamfer and other Features)
##     @defgroup l3_blocks_op     Blocks Operations
##     @defgroup l3_healing       Repairing Operations
##     @defgroup l3_restore_ss    Restore presentation parameters and a tree of sub-shapes

##   @}
##   @defgroup l2_measure       Using measurement tools
##   @defgroup l2_field         Field on Geometry

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
import functools

from salome.geom.gsketcher import Sketcher3D, Sketcher2D, Polyline2D

# service function
def _toListOfNames(_names, _size=-1):
    l = []
    import types
    if type(_names) in [types.ListType, types.TupleType]:
        for i in _names: l.append(i)
    elif _names:
        l.append(_names)
    if l and len(l) < _size:
        for i in range(len(l), _size): l.append("%s_%d"%(l[0],i))
    return l

# Decorator function to manage transactions for all geometric operations.
def ManageTransactions(theOpeName):
    def MTDecorator(theFunction):
        # To keep the original function name an documentation.
        @functools.wraps(theFunction)
        def OpenCallClose(self, *args, **kwargs):
            # Open transaction
            anOperation = getattr(self, theOpeName)
            anOperation.StartOperation()
            try:
                # Call the function
                res = theFunction(self, *args, **kwargs)
                # Commit transaction
                anOperation.FinishOperation()
                return res
            except:
                # Abort transaction
                anOperation.AbortOperation()
                raise
        return OpenCallClose
    return MTDecorator

## Raise an Error, containing the Method_name, if Operation is Failed
## @ingroup l1_geomBuilder_auxiliary
def RaiseIfFailed (Method_name, Operation):
    if not Operation.IsDone() and Operation.GetErrorCode() != "NOT_FOUND_ANY":
        raise RuntimeError, Method_name + " : " + Operation.GetErrorCode()

## Return list of variables value from salome notebook
## @ingroup l1_geomBuilder_auxiliary
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
## @ingroup l1_geomBuilder_auxiliary
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
## @ingroup l1_geomBuilder_auxiliary
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
## @ingroup l1_geomBuilder_auxiliary
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
## the AddTexture() function of geomBuilder class.
## For example,
## \code
## from salome.geom import geomBuilder
## geompy = geomBuilder.New(salome.myStudy)
## texture = geompy.readtexture('mytexture.dat')
## texture = geompy.AddTexture(*texture)
## obj.SetMarkerTexture(texture)
## \endcode
## @param fname texture file name
## @return sequence of tree values: texture's width, height in pixels and its byte stream
## @ingroup l1_geomBuilder_auxiliary
def ReadTexture(fname):
    """
    Read bitmap texture from the text file.
    In that file, any non-zero symbol represents '1' opaque pixel of the bitmap.
    A zero symbol ('0') represents transparent pixel of the texture bitmap.
    The function returns width and height of the pixmap in pixels and byte stream representing
    texture bitmap itself.
    This function can be used to read the texture to the byte stream in order to pass it to
    the AddTexture() function of geomBuilder class.

    Parameters:
        fname texture file name

    Returns:
        sequence of tree values: texture's width, height in pixels and its byte stream

    Example of usage:
        from salome.geom import geomBuilder
        geompy = geomBuilder.New(salome.myStudy)
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
#  @ingroup l1_geomBuilder_auxiliary
def EnumToLong(theItem):
    """
    Returns a long value from enumeration type
    Can be used for CORBA enumerator types like geomBuilder.ShapeType

    Parameters:
        theItem enumeration type
    """
    ret = theItem
    if hasattr(theItem, "_v"): ret = theItem._v
    return ret

## Pack an argument into a list
def ToList( arg ):
    if isinstance( arg, list ):
        return arg
    if hasattr( arg, "__getitem__" ):
        return list( arg )
    return [ arg ]

## Information about closed/unclosed state of shell or wire
#  @ingroup l1_geomBuilder_auxiliary
class info:
    """
    Information about closed/unclosed state of shell or wire
    """
    UNKNOWN  = 0
    CLOSED   = 1
    UNCLOSED = 2

## Private class used to bind calls of plugin operations to geomBuilder
class PluginOperation:
  def __init__(self, operation, function):
    self.operation = operation
    self.function = function
    pass

  @ManageTransactions("operation")
  def __call__(self, *args):
    res = self.function(self.operation, *args)
    RaiseIfFailed(self.function.__name__, self.operation)
    return res

# Warning: geom is a singleton
geom = None
engine = None
doLcc = False
created = False

class geomBuilder(object, GEOM._objref_GEOM_Gen):

        ## Enumeration ShapeType as a dictionary. \n
        ## Topological types of shapes (like Open Cascade types). See GEOM::shape_type for details.
        #  @ingroup l1_geomBuilder_auxiliary
        ShapeType = {"AUTO":-1, "COMPOUND":0, "COMPSOLID":1, "SOLID":2, "SHELL":3, "FACE":4, "WIRE":5, "EDGE":6, "VERTEX":7, "SHAPE":8, "FLAT":9}

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
        #
        #  - LCS:          [x y z  xx xy xz  yx yy yz  zx zy zz]
        #  @ingroup l1_geomBuilder_auxiliary
        kind = GEOM.GEOM_IKindOfShape

        def __new__(cls):
            global engine
            global geom
            global doLcc
            global created
            #print "==== __new__ ", engine, geom, doLcc, created
            if geom is None:
                # geom engine is either retrieved from engine, or created
                geom = engine
                # Following test avoids a recursive loop
                if doLcc:
                    if geom is not None:
                        # geom engine not created: existing engine found
                        doLcc = False
                    if doLcc and not created:
                        doLcc = False
                        # FindOrLoadComponent called:
                        # 1. CORBA resolution of server
                        # 2. the __new__ method is called again
                        #print "==== FindOrLoadComponent ", engine, geom, doLcc, created
                        geom = lcc.FindOrLoadComponent( "FactoryServer", "GEOM" )
                        #print "====1 ",geom
                else:
                    # FindOrLoadComponent not called
                    if geom is None:
                        # geomBuilder instance is created from lcc.FindOrLoadComponent
                        #print "==== super ", engine, geom, doLcc, created
                        geom = super(geomBuilder,cls).__new__(cls)
                        #print "====2 ",geom
                    else:
                        # geom engine not created: existing engine found
                        #print "==== existing ", engine, geom, doLcc, created
                        pass
                #print "return geom 1 ", geom
                return geom

            #print "return geom 2 ", geom
            return geom

        def __init__(self):
            global created
            #print "-------- geomBuilder __init__ --- ", created, self
            if not created:
              created = True
              GEOM._objref_GEOM_Gen.__init__(self)
              self.myMaxNbSubShapesAllowed = 0 # auto-publishing is disabled by default
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
              self.FieldOp  = None
            pass

        ## Process object publication in the study, as follows:
        #  - if @a theName is specified (not None), the object is published in the study
        #    with this name, not taking into account "auto-publishing" option;
        #  - if @a theName is NOT specified, the object is published in the study
        #    (using default name, which can be customized using @a theDefaultName parameter)
        #    only if auto-publishing is switched on.
        #
        #  @param theObj  object, a subject for publishing
        #  @param theName object name for study
        #  @param theDefaultName default name for the auto-publishing
        #
        #  @sa addToStudyAuto()
        def _autoPublish(self, theObj, theName, theDefaultName="noname"):
            # ---
            def _item_name(_names, _defname, _idx=-1):
                if not _names: _names = _defname
                if type(_names) in [types.ListType, types.TupleType]:
                    if _idx >= 0:
                        if _idx >= len(_names) or not _names[_idx]:
                            if type(_defname) not in [types.ListType, types.TupleType]:
                                _name = "%s_%d"%(_defname, _idx+1)
                            elif len(_defname) > 0 and _idx >= 0 and _idx < len(_defname):
                                _name = _defname[_idx]
                            else:
                                _name = "%noname_%d"%(dn, _idx+1)
                            pass
                        else:
                            _name = _names[_idx]
                        pass
                    else:
                        # must be wrong  usage
                        _name = _names[0]
                    pass
                else:
                    if _idx >= 0:
                        _name = "%s_%d"%(_names, _idx+1)
                    else:
                        _name = _names
                    pass
                return _name
            # ---
            def _publish( _name, _obj ):
                fatherObj = None
                if isinstance( _obj, GEOM._objref_GEOM_Field ):
                    fatherObj = _obj.GetShape()
                elif isinstance( _obj, GEOM._objref_GEOM_FieldStep ):
                    fatherObj = _obj.GetField()
                elif not _obj.IsMainShape():
                    fatherObj = _obj.GetMainShape()
                    pass
                if fatherObj and fatherObj.GetStudyEntry():
                    self.addToStudyInFather(fatherObj, _obj, _name)
                else:
                    self.addToStudy(_obj, _name)
                    pass
                return
            # ---
            if not theObj:
                return # null object
            if not theName and not self.myMaxNbSubShapesAllowed:
                return # nothing to do: auto-publishing is disabled
            if not theName and not theDefaultName:
                return # neither theName nor theDefaultName is given
            import types
            if type(theObj) in [types.ListType, types.TupleType]:
                # list of objects is being published
                idx = 0
                for obj in theObj:
                    if not obj: continue # bad object
                    name = _item_name(theName, theDefaultName, idx)
                    _publish( name, obj )
                    idx = idx+1
                    if not theName and idx == self.myMaxNbSubShapesAllowed: break
                    pass
                pass
            else:
                # single object is published
                name = _item_name(theName, theDefaultName)
                _publish( name, theObj )
            pass

        ## @addtogroup l1_geomBuilder_auxiliary
        ## @{
        def init_geom(self,theStudy):
            self.myStudy = theStudy
            self.myStudyId = self.myStudy._get_StudyId()
            self.myBuilder = self.myStudy.NewBuilder()
            self.father = self.myStudy.FindComponent("GEOM")
            notebook.myStudy = theStudy
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
            self.FieldOp  = self.GetIFieldOperations    (self.myStudyId)

            # set GEOM as root in the use case tree
            self.myUseCaseBuilder = self.myStudy.GetUseCaseBuilder()
            self.myUseCaseBuilder.SetRootCurrent()
            self.myUseCaseBuilder.Append(self.father)

            # load data from the study file, if necessary
            self.myBuilder.LoadWith(self.father, self)
            pass

        def GetPluginOperations(self, studyID, libraryName):
            op = GEOM._objref_GEOM_Gen.GetPluginOperations(self, studyID, libraryName)
            return op

        ## Enable / disable results auto-publishing
        #
        #  The automatic publishing is managed in the following way:
        #  - if @a maxNbSubShapes = 0, automatic publishing is disabled.
        #  - if @a maxNbSubShapes = -1 (default), automatic publishing is enabled and
        #  maximum number of sub-shapes allowed for publishing is unlimited; any negative
        #  value passed as parameter has the same effect.
        #  - if @a maxNbSubShapes is any positive value, automatic publishing is enabled and
        #  maximum number of sub-shapes allowed for publishing is set to specified value.
        #
        #  @param maxNbSubShapes maximum number of sub-shapes allowed for publishing.
        #  @ingroup l1_publish_data
        def addToStudyAuto(self, maxNbSubShapes=-1):
            """
            Enable / disable results auto-publishing

            The automatic publishing is managed in the following way:
            - if @a maxNbSubShapes = 0, automatic publishing is disabled;
            - if @a maxNbSubShapes = -1 (default), automatic publishing is enabled and
            maximum number of sub-shapes allowed for publishing is unlimited; any negative
            value passed as parameter has the same effect.
            - if @a maxNbSubShapes is any positive value, automatic publishing is enabled and
            maximum number of sub-shapes allowed for publishing is set to this value.

            Parameters:
                maxNbSubShapes maximum number of sub-shapes allowed for publishing.

            Example of usage:
                geompy.addToStudyAuto()   # enable auto-publishing
                geompy.MakeBoxDXDYDZ(100) # box is created and published with default name
                geompy.addToStudyAuto(0)  # disable auto-publishing
            """
            self.myMaxNbSubShapesAllowed = max(-1, maxNbSubShapes)
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
        @ManageTransactions("ShapesOp")
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
        #  @ingroup l1_publish_data
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
        #
        #  @ingroup l1_publish_data
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
                # hide references if any
                vso = self.myStudy.FindDependances(aSObject);
                for refObj in vso :
                    genericAttribute = self.myBuilder.FindOrCreateAttribute(refObj, "AttributeDrawable")
                    drwAttribute = genericAttribute._narrow(SALOMEDS.AttributeDrawable)
                    drwAttribute.SetDrawable(False)
                    pass
                pass

        # end of l1_geomBuilder_auxiliary
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        @ManageTransactions("BasicOp")
        def MakeVertex(self, theX, theY, theZ, theName=None):
            """
            Create point by three coordinates.

            Parameters:
                theX The X coordinate of the point.
                theY The Y coordinate of the point.
                theZ The Z coordinate of the point.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created point.
            """
            # Example: see GEOM_TestAll.py
            theX,theY,theZ,Parameters = ParseParameters(theX, theY, theZ)
            anObj = self.BasicOp.MakePointXYZ(theX, theY, theZ)
            RaiseIfFailed("MakePointXYZ", self.BasicOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Create a point, distant from the referenced point
        #  on the given distances along the coordinate axes.
        #  @param theReference The referenced point.
        #  @param theX Displacement from the referenced point along OX axis.
        #  @param theY Displacement from the referenced point along OY axis.
        #  @param theZ Displacement from the referenced point along OZ axis.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        @ManageTransactions("BasicOp")
        def MakeVertexWithRef(self, theReference, theX, theY, theZ, theName=None):
            """
            Create a point, distant from the referenced point
            on the given distances along the coordinate axes.

            Parameters:
                theReference The referenced point.
                theX Displacement from the referenced point along OX axis.
                theY Displacement from the referenced point along OY axis.
                theZ Displacement from the referenced point along OZ axis.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created point.
            """
            # Example: see GEOM_TestAll.py
            theX,theY,theZ,Parameters = ParseParameters(theX, theY, theZ)
            anObj = self.BasicOp.MakePointWithReference(theReference, theX, theY, theZ)
            RaiseIfFailed("MakePointWithReference", self.BasicOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Create a point, corresponding to the given parameter on the given curve.
        #  @param theRefCurve The referenced curve.
        #  @param theParameter Value of parameter on the referenced curve.
        #  @param takeOrientationIntoAccount flag that tells if it is necessary
        #         to take the curve's orientation into account for the
        #         operation. I.e. if this flag is set, the results for the same
        #         parameters (except the value 0.5) is different for forward
        #         and reversed curves. If it is not set the result is the same.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        @ManageTransactions("BasicOp")
        def MakeVertexOnCurve(self, theRefCurve, theParameter,
                              takeOrientationIntoAccount=False, theName=None):
            """
            Create a point, corresponding to the given parameter on the given curve.

            Parameters:
                theRefCurve The referenced curve.
                theParameter Value of parameter on the referenced curve.
                takeOrientationIntoAccount flag that tells if it is necessary
                        to take the curve's orientation into account for the
                        operation. I.e. if this flag is set, the results for
                        the same parameters (except the value 0.5) is different
                        for forward and reversed curves. If it is not set
                        the result is the same.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created point.

            Example of usage:
                p_on_arc = geompy.MakeVertexOnCurve(Arc, 0.25)
            """
            # Example: see GEOM_TestAll.py
            theParameter, takeOrientationIntoAccount, Parameters = ParseParameters(
                theParameter, takeOrientationIntoAccount)
            anObj = self.BasicOp.MakePointOnCurve(theRefCurve, theParameter,
                                                  takeOrientationIntoAccount)
            RaiseIfFailed("MakePointOnCurve", self.BasicOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Create a point by projection give coordinates on the given curve
        #  @param theRefCurve The referenced curve.
        #  @param theX X-coordinate in 3D space
        #  @param theY Y-coordinate in 3D space
        #  @param theZ Z-coordinate in 3D space
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        @ManageTransactions("BasicOp")
        def MakeVertexOnCurveByCoord(self, theRefCurve, theX, theY, theZ, theName=None):
            """
            Create a point by projection give coordinates on the given curve

            Parameters:
                theRefCurve The referenced curve.
                theX X-coordinate in 3D space
                theY Y-coordinate in 3D space
                theZ Z-coordinate in 3D space
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Create a point, corresponding to the given length on the given curve.
        #  @param theRefCurve The referenced curve.
        #  @param theLength Length on the referenced curve. It can be negative.
        #  @param theStartPoint Point allowing to choose the direction for the calculation
        #                       of the length. If None, start from the first point of theRefCurve.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_creation_point "Example"
        @ManageTransactions("BasicOp")
        def MakeVertexOnCurveByLength(self, theRefCurve, theLength, theStartPoint = None, theName=None):
            """
            Create a point, corresponding to the given length on the given curve.

            Parameters:
                theRefCurve The referenced curve.
                theLength Length on the referenced curve. It can be negative.
                theStartPoint Point allowing to choose the direction for the calculation
                              of the length. If None, start from the first point of theRefCurve.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created point.
            """
            # Example: see GEOM_TestAll.py
            theLength, Parameters = ParseParameters(theLength)
            anObj = self.BasicOp.MakePointOnCurveByLength(theRefCurve, theLength, theStartPoint)
            RaiseIfFailed("MakePointOnCurveByLength", self.BasicOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Create a point, corresponding to the given parameters on the
        #    given surface.
        #  @param theRefSurf The referenced surface.
        #  @param theUParameter Value of U-parameter on the referenced surface.
        #  @param theVParameter Value of V-parameter on the referenced surface.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref swig_MakeVertexOnSurface "Example"
        @ManageTransactions("BasicOp")
        def MakeVertexOnSurface(self, theRefSurf, theUParameter, theVParameter, theName=None):
            """
            Create a point, corresponding to the given parameters on the
            given surface.

            Parameters:
                theRefSurf The referenced surface.
                theUParameter Value of U-parameter on the referenced surface.
                theVParameter Value of V-parameter on the referenced surface.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Create a point by projection give coordinates on the given surface
        #  @param theRefSurf The referenced surface.
        #  @param theX X-coordinate in 3D space
        #  @param theY Y-coordinate in 3D space
        #  @param theZ Z-coordinate in 3D space
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref swig_MakeVertexOnSurfaceByCoord "Example"
        @ManageTransactions("BasicOp")
        def MakeVertexOnSurfaceByCoord(self, theRefSurf, theX, theY, theZ, theName=None):
            """
            Create a point by projection give coordinates on the given surface

            Parameters:
                theRefSurf The referenced surface.
                theX X-coordinate in 3D space
                theY Y-coordinate in 3D space
                theZ Z-coordinate in 3D space
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Create a point, which lays on the given face.
        #  The point will lay in arbitrary place of the face.
        #  The only condition on it is a non-zero distance to the face boundary.
        #  Such point can be used to uniquely identify the face inside any
        #  shape in case, when the shape does not contain overlapped faces.
        #  @param theFace The referenced face.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref swig_MakeVertexInsideFace "Example"
        @ManageTransactions("BasicOp")
        def MakeVertexInsideFace (self, theFace, theName=None):
            """
            Create a point, which lays on the given face.
            The point will lay in arbitrary place of the face.
            The only condition on it is a non-zero distance to the face boundary.
            Such point can be used to uniquely identify the face inside any
            shape in case, when the shape does not contain overlapped faces.

            Parameters:
                theFace The referenced face.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created point.

            Example of usage:
                p_on_face = geompy.MakeVertexInsideFace(Face)
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakePointOnFace(theFace)
            RaiseIfFailed("MakeVertexInsideFace", self.BasicOp)
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Create a point on intersection of two lines.
        #  @param theRefLine1, theRefLine2 The referenced lines.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref swig_MakeVertexOnLinesIntersection "Example"
        @ManageTransactions("BasicOp")
        def MakeVertexOnLinesIntersection(self, theRefLine1, theRefLine2, theName=None):
            """
            Create a point on intersection of two lines.

            Parameters:
                theRefLine1, theRefLine2 The referenced lines.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created point.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakePointOnLinesIntersection(theRefLine1, theRefLine2)
            RaiseIfFailed("MakePointOnLinesIntersection", self.BasicOp)
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Create a tangent, corresponding to the given parameter on the given curve.
        #  @param theRefCurve The referenced curve.
        #  @param theParameter Value of parameter on the referenced curve.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created tangent.
        #
        #  @ref swig_MakeTangentOnCurve "Example"
        @ManageTransactions("BasicOp")
        def MakeTangentOnCurve(self, theRefCurve, theParameter, theName=None):
            """
            Create a tangent, corresponding to the given parameter on the given curve.

            Parameters:
                theRefCurve The referenced curve.
                theParameter Value of parameter on the referenced curve.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created tangent.

            Example of usage:
                tan_on_arc = geompy.MakeTangentOnCurve(Arc, 0.7)
            """
            anObj = self.BasicOp.MakeTangentOnCurve(theRefCurve, theParameter)
            RaiseIfFailed("MakeTangentOnCurve", self.BasicOp)
            self._autoPublish(anObj, theName, "tangent")
            return anObj

        ## Create a tangent plane, corresponding to the given parameter on the given face.
        #  @param theFace The face for which tangent plane should be built.
        #  @param theParameterV vertical value of the center point (0.0 - 1.0).
        #  @param theParameterU horisontal value of the center point (0.0 - 1.0).
        #  @param theTrimSize the size of plane.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created tangent.
        #
        #  @ref swig_MakeTangentPlaneOnFace "Example"
        @ManageTransactions("BasicOp")
        def MakeTangentPlaneOnFace(self, theFace, theParameterU, theParameterV, theTrimSize, theName=None):
            """
            Create a tangent plane, corresponding to the given parameter on the given face.

            Parameters:
                theFace The face for which tangent plane should be built.
                theParameterV vertical value of the center point (0.0 - 1.0).
                theParameterU horisontal value of the center point (0.0 - 1.0).
                theTrimSize the size of plane.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

           Returns:
                New GEOM.GEOM_Object, containing the created tangent.

           Example of usage:
                an_on_face = geompy.MakeTangentPlaneOnFace(tan_extrusion, 0.7, 0.5, 150)
            """
            anObj = self.BasicOp.MakeTangentPlaneOnFace(theFace, theParameterU, theParameterV, theTrimSize)
            RaiseIfFailed("MakeTangentPlaneOnFace", self.BasicOp)
            self._autoPublish(anObj, theName, "tangent")
            return anObj

        ## Create a vector with the given components.
        #  @param theDX X component of the vector.
        #  @param theDY Y component of the vector.
        #  @param theDZ Z component of the vector.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created vector.
        #
        #  @ref tui_creation_vector "Example"
        @ManageTransactions("BasicOp")
        def MakeVectorDXDYDZ(self, theDX, theDY, theDZ, theName=None):
            """
            Create a vector with the given components.

            Parameters:
                theDX X component of the vector.
                theDY Y component of the vector.
                theDZ Z component of the vector.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created vector.
            """
            # Example: see GEOM_TestAll.py
            theDX,theDY,theDZ,Parameters = ParseParameters(theDX, theDY, theDZ)
            anObj = self.BasicOp.MakeVectorDXDYDZ(theDX, theDY, theDZ)
            RaiseIfFailed("MakeVectorDXDYDZ", self.BasicOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "vector")
            return anObj

        ## Create a vector between two points.
        #  @param thePnt1 Start point for the vector.
        #  @param thePnt2 End point for the vector.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created vector.
        #
        #  @ref tui_creation_vector "Example"
        @ManageTransactions("BasicOp")
        def MakeVector(self, thePnt1, thePnt2, theName=None):
            """
            Create a vector between two points.

            Parameters:
                thePnt1 Start point for the vector.
                thePnt2 End point for the vector.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created vector.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeVectorTwoPnt(thePnt1, thePnt2)
            RaiseIfFailed("MakeVectorTwoPnt", self.BasicOp)
            self._autoPublish(anObj, theName, "vector")
            return anObj

        ## Create a line, passing through the given point
        #  and parrallel to the given direction
        #  @param thePnt Point. The resulting line will pass through it.
        #  @param theDir Direction. The resulting line will be parallel to it.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created line.
        #
        #  @ref tui_creation_line "Example"
        @ManageTransactions("BasicOp")
        def MakeLine(self, thePnt, theDir, theName=None):
            """
            Create a line, passing through the given point
            and parrallel to the given direction

            Parameters:
                thePnt Point. The resulting line will pass through it.
                theDir Direction. The resulting line will be parallel to it.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created line.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeLine(thePnt, theDir)
            RaiseIfFailed("MakeLine", self.BasicOp)
            self._autoPublish(anObj, theName, "line")
            return anObj

        ## Create a line, passing through the given points
        #  @param thePnt1 First of two points, defining the line.
        #  @param thePnt2 Second of two points, defining the line.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created line.
        #
        #  @ref tui_creation_line "Example"
        @ManageTransactions("BasicOp")
        def MakeLineTwoPnt(self, thePnt1, thePnt2, theName=None):
            """
            Create a line, passing through the given points

            Parameters:
                thePnt1 First of two points, defining the line.
                thePnt2 Second of two points, defining the line.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created line.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeLineTwoPnt(thePnt1, thePnt2)
            RaiseIfFailed("MakeLineTwoPnt", self.BasicOp)
            self._autoPublish(anObj, theName, "line")
            return anObj

        ## Create a line on two faces intersection.
        #  @param theFace1 First of two faces, defining the line.
        #  @param theFace2 Second of two faces, defining the line.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created line.
        #
        #  @ref swig_MakeLineTwoFaces "Example"
        @ManageTransactions("BasicOp")
        def MakeLineTwoFaces(self, theFace1, theFace2, theName=None):
            """
            Create a line on two faces intersection.

            Parameters:
                theFace1 First of two faces, defining the line.
                theFace2 Second of two faces, defining the line.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created line.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BasicOp.MakeLineTwoFaces(theFace1, theFace2)
            RaiseIfFailed("MakeLineTwoFaces", self.BasicOp)
            self._autoPublish(anObj, theName, "line")
            return anObj

        ## Create a plane, passing through the given point
        #  and normal to the given vector.
        #  @param thePnt Point, the plane has to pass through.
        #  @param theVec Vector, defining the plane normal direction.
        #  @param theTrimSize Half size of a side of quadrangle face, representing the plane.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        @ManageTransactions("BasicOp")
        def MakePlane(self, thePnt, theVec, theTrimSize, theName=None):
            """
            Create a plane, passing through the given point
            and normal to the given vector.

            Parameters:
                thePnt Point, the plane has to pass through.
                theVec Vector, defining the plane normal direction.
                theTrimSize Half size of a side of quadrangle face, representing the plane.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created plane.
            """
            # Example: see GEOM_TestAll.py
            theTrimSize, Parameters = ParseParameters(theTrimSize);
            anObj = self.BasicOp.MakePlanePntVec(thePnt, theVec, theTrimSize)
            RaiseIfFailed("MakePlanePntVec", self.BasicOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "plane")
            return anObj

        ## Create a plane, passing through the three given points
        #  @param thePnt1 First of three points, defining the plane.
        #  @param thePnt2 Second of three points, defining the plane.
        #  @param thePnt3 Fird of three points, defining the plane.
        #  @param theTrimSize Half size of a side of quadrangle face, representing the plane.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        @ManageTransactions("BasicOp")
        def MakePlaneThreePnt(self, thePnt1, thePnt2, thePnt3, theTrimSize, theName=None):
            """
            Create a plane, passing through the three given points

            Parameters:
                thePnt1 First of three points, defining the plane.
                thePnt2 Second of three points, defining the plane.
                thePnt3 Fird of three points, defining the plane.
                theTrimSize Half size of a side of quadrangle face, representing the plane.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created plane.
            """
            # Example: see GEOM_TestAll.py
            theTrimSize, Parameters = ParseParameters(theTrimSize);
            anObj = self.BasicOp.MakePlaneThreePnt(thePnt1, thePnt2, thePnt3, theTrimSize)
            RaiseIfFailed("MakePlaneThreePnt", self.BasicOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "plane")
            return anObj

        ## Create a plane, similar to the existing one, but with another size of representing face.
        #  @param theFace Referenced plane or LCS(Marker).
        #  @param theTrimSize New half size of a side of quadrangle face, representing the plane.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        @ManageTransactions("BasicOp")
        def MakePlaneFace(self, theFace, theTrimSize, theName=None):
            """
            Create a plane, similar to the existing one, but with another size of representing face.

            Parameters:
                theFace Referenced plane or LCS(Marker).
                theTrimSize New half size of a side of quadrangle face, representing the plane.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created plane.
            """
            # Example: see GEOM_TestAll.py
            theTrimSize, Parameters = ParseParameters(theTrimSize);
            anObj = self.BasicOp.MakePlaneFace(theFace, theTrimSize)
            RaiseIfFailed("MakePlaneFace", self.BasicOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "plane")
            return anObj

        ## Create a plane, passing through the 2 vectors
        #  with center in a start point of the first vector.
        #  @param theVec1 Vector, defining center point and plane direction.
        #  @param theVec2 Vector, defining the plane normal direction.
        #  @param theTrimSize Half size of a side of quadrangle face, representing the plane.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        @ManageTransactions("BasicOp")
        def MakePlane2Vec(self, theVec1, theVec2, theTrimSize, theName=None):
            """
            Create a plane, passing through the 2 vectors
            with center in a start point of the first vector.

            Parameters:
                theVec1 Vector, defining center point and plane direction.
                theVec2 Vector, defining the plane normal direction.
                theTrimSize Half size of a side of quadrangle face, representing the plane.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created plane.
            """
            # Example: see GEOM_TestAll.py
            theTrimSize, Parameters = ParseParameters(theTrimSize);
            anObj = self.BasicOp.MakePlane2Vec(theVec1, theVec2, theTrimSize)
            RaiseIfFailed("MakePlane2Vec", self.BasicOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "plane")
            return anObj

        ## Create a plane, based on a Local coordinate system.
        #  @param theLCS  coordinate system, defining plane.
        #  @param theTrimSize Half size of a side of quadrangle face, representing the plane.
        #  @param theOrientation OXY, OYZ or OZX orientation - (1, 2 or 3)
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created plane.
        #
        #  @ref tui_creation_plane "Example"
        @ManageTransactions("BasicOp")
        def MakePlaneLCS(self, theLCS, theTrimSize, theOrientation, theName=None):
            """
            Create a plane, based on a Local coordinate system.

           Parameters:
                theLCS  coordinate system, defining plane.
                theTrimSize Half size of a side of quadrangle face, representing the plane.
                theOrientation OXY, OYZ or OZX orientation - (1, 2 or 3)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created plane.
            """
            # Example: see GEOM_TestAll.py
            theTrimSize, Parameters = ParseParameters(theTrimSize);
            anObj = self.BasicOp.MakePlaneLCS(theLCS, theTrimSize, theOrientation)
            RaiseIfFailed("MakePlaneLCS", self.BasicOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "plane")
            return anObj

        ## Create a local coordinate system.
        #  @param OX,OY,OZ Three coordinates of coordinate system origin.
        #  @param XDX,XDY,XDZ Three components of OX direction
        #  @param YDX,YDY,YDZ Three components of OY direction
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created coordinate system.
        #
        #  @ref swig_MakeMarker "Example"
        @ManageTransactions("BasicOp")
        def MakeMarker(self, OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ, theName=None):
            """
            Create a local coordinate system.

            Parameters:
                OX,OY,OZ Three coordinates of coordinate system origin.
                XDX,XDY,XDZ Three components of OX direction
                YDX,YDY,YDZ Three components of OY direction
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created coordinate system.
            """
            # Example: see GEOM_TestAll.py
            OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ, Parameters = ParseParameters(OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ);
            anObj = self.BasicOp.MakeMarker(OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ)
            RaiseIfFailed("MakeMarker", self.BasicOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "lcs")
            return anObj

        ## Create a local coordinate system from shape.
        #  @param theShape The initial shape to detect the coordinate system.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created coordinate system.
        #
        #  @ref tui_creation_lcs "Example"
        @ManageTransactions("BasicOp")
        def MakeMarkerFromShape(self, theShape, theName=None):
            """
            Create a local coordinate system from shape.

            Parameters:
                theShape The initial shape to detect the coordinate system.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created coordinate system.
            """
            anObj = self.BasicOp.MakeMarkerFromShape(theShape)
            RaiseIfFailed("MakeMarkerFromShape", self.BasicOp)
            self._autoPublish(anObj, theName, "lcs")
            return anObj

        ## Create a local coordinate system from point and two vectors.
        #  @param theOrigin Point of coordinate system origin.
        #  @param theXVec Vector of X direction
        #  @param theYVec Vector of Y direction
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created coordinate system.
        #
        #  @ref tui_creation_lcs "Example"
        @ManageTransactions("BasicOp")
        def MakeMarkerPntTwoVec(self, theOrigin, theXVec, theYVec, theName=None):
            """
            Create a local coordinate system from point and two vectors.

            Parameters:
                theOrigin Point of coordinate system origin.
                theXVec Vector of X direction
                theYVec Vector of Y direction
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created coordinate system.

            """
            anObj = self.BasicOp.MakeMarkerPntTwoVec(theOrigin, theXVec, theYVec)
            RaiseIfFailed("MakeMarkerPntTwoVec", self.BasicOp)
            self._autoPublish(anObj, theName, "lcs")
            return anObj

        # end of l3_basic_go
        ## @}

        ## @addtogroup l4_curves
        ## @{

        ##  Create an arc of circle, passing through three given points.
        #  @param thePnt1 Start point of the arc.
        #  @param thePnt2 Middle point of the arc.
        #  @param thePnt3 End point of the arc.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created arc.
        #
        #  @ref swig_MakeArc "Example"
        @ManageTransactions("CurvesOp")
        def MakeArc(self, thePnt1, thePnt2, thePnt3, theName=None):
            """
            Create an arc of circle, passing through three given points.

            Parameters:
                thePnt1 Start point of the arc.
                thePnt2 Middle point of the arc.
                thePnt3 End point of the arc.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created arc.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeArc(thePnt1, thePnt2, thePnt3)
            RaiseIfFailed("MakeArc", self.CurvesOp)
            self._autoPublish(anObj, theName, "arc")
            return anObj

        ##  Create an arc of circle from a center and 2 points.
        #  @param thePnt1 Center of the arc
        #  @param thePnt2 Start point of the arc. (Gives also the radius of the arc)
        #  @param thePnt3 End point of the arc (Gives also a direction)
        #  @param theSense Orientation of the arc
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created arc.
        #
        #  @ref swig_MakeArc "Example"
        @ManageTransactions("CurvesOp")
        def MakeArcCenter(self, thePnt1, thePnt2, thePnt3, theSense=False, theName=None):
            """
            Create an arc of circle from a center and 2 points.

            Parameters:
                thePnt1 Center of the arc
                thePnt2 Start point of the arc. (Gives also the radius of the arc)
                thePnt3 End point of the arc (Gives also a direction)
                theSense Orientation of the arc
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created arc.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeArcCenter(thePnt1, thePnt2, thePnt3, theSense)
            RaiseIfFailed("MakeArcCenter", self.CurvesOp)
            self._autoPublish(anObj, theName, "arc")
            return anObj

        ##  Create an arc of ellipse, of center and two points.
        #  @param theCenter Center of the arc.
        #  @param thePnt1 defines major radius of the arc by distance from Pnt1 to Pnt2.
        #  @param thePnt2 defines plane of ellipse and minor radius as distance from Pnt3 to line from Pnt1 to Pnt2.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created arc.
        #
        #  @ref swig_MakeArc "Example"
        @ManageTransactions("CurvesOp")
        def MakeArcOfEllipse(self, theCenter, thePnt1, thePnt2, theName=None):
            """
            Create an arc of ellipse, of center and two points.

            Parameters:
                theCenter Center of the arc.
                thePnt1 defines major radius of the arc by distance from Pnt1 to Pnt2.
                thePnt2 defines plane of ellipse and minor radius as distance from Pnt3 to line from Pnt1 to Pnt2.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created arc.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeArcOfEllipse(theCenter, thePnt1, thePnt2)
            RaiseIfFailed("MakeArcOfEllipse", self.CurvesOp)
            self._autoPublish(anObj, theName, "arc")
            return anObj

        ## Create a circle with given center, normal vector and radius.
        #  @param thePnt Circle center.
        #  @param theVec Vector, normal to the plane of the circle.
        #  @param theR Circle radius.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created circle.
        #
        #  @ref tui_creation_circle "Example"
        @ManageTransactions("CurvesOp")
        def MakeCircle(self, thePnt, theVec, theR, theName=None):
            """
            Create a circle with given center, normal vector and radius.

            Parameters:
                thePnt Circle center.
                theVec Vector, normal to the plane of the circle.
                theR Circle radius.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created circle.
            """
            # Example: see GEOM_TestAll.py
            theR, Parameters = ParseParameters(theR)
            anObj = self.CurvesOp.MakeCirclePntVecR(thePnt, theVec, theR)
            RaiseIfFailed("MakeCirclePntVecR", self.CurvesOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "circle")
            return anObj

        ## Create a circle with given radius.
        #  Center of the circle will be in the origin of global
        #  coordinate system and normal vector will be codirected with Z axis
        #  @param theR Circle radius.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created circle.
        @ManageTransactions("CurvesOp")
        def MakeCircleR(self, theR, theName=None):
            """
            Create a circle with given radius.
            Center of the circle will be in the origin of global
            coordinate system and normal vector will be codirected with Z axis

            Parameters:
                theR Circle radius.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created circle.
            """
            anObj = self.CurvesOp.MakeCirclePntVecR(None, None, theR)
            RaiseIfFailed("MakeCirclePntVecR", self.CurvesOp)
            self._autoPublish(anObj, theName, "circle")
            return anObj

        ## Create a circle, passing through three given points
        #  @param thePnt1,thePnt2,thePnt3 Points, defining the circle.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created circle.
        #
        #  @ref tui_creation_circle "Example"
        @ManageTransactions("CurvesOp")
        def MakeCircleThreePnt(self, thePnt1, thePnt2, thePnt3, theName=None):
            """
            Create a circle, passing through three given points

            Parameters:
                thePnt1,thePnt2,thePnt3 Points, defining the circle.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created circle.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeCircleThreePnt(thePnt1, thePnt2, thePnt3)
            RaiseIfFailed("MakeCircleThreePnt", self.CurvesOp)
            self._autoPublish(anObj, theName, "circle")
            return anObj

        ## Create a circle, with given point1 as center,
        #  passing through the point2 as radius and laying in the plane,
        #  defined by all three given points.
        #  @param thePnt1,thePnt2,thePnt3 Points, defining the circle.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created circle.
        #
        #  @ref swig_MakeCircle "Example"
        @ManageTransactions("CurvesOp")
        def MakeCircleCenter2Pnt(self, thePnt1, thePnt2, thePnt3, theName=None):
            """
            Create a circle, with given point1 as center,
            passing through the point2 as radius and laying in the plane,
            defined by all three given points.

            Parameters:
                thePnt1,thePnt2,thePnt3 Points, defining the circle.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created circle.
            """
            # Example: see GEOM_example6.py
            anObj = self.CurvesOp.MakeCircleCenter2Pnt(thePnt1, thePnt2, thePnt3)
            RaiseIfFailed("MakeCircleCenter2Pnt", self.CurvesOp)
            self._autoPublish(anObj, theName, "circle")
            return anObj

        ## Create an ellipse with given center, normal vector and radiuses.
        #  @param thePnt Ellipse center.
        #  @param theVec Vector, normal to the plane of the ellipse.
        #  @param theRMajor Major ellipse radius.
        #  @param theRMinor Minor ellipse radius.
        #  @param theVecMaj Vector, direction of the ellipse's main axis.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created ellipse.
        #
        #  @ref tui_creation_ellipse "Example"
        @ManageTransactions("CurvesOp")
        def MakeEllipse(self, thePnt, theVec, theRMajor, theRMinor, theVecMaj=None, theName=None):
            """
            Create an ellipse with given center, normal vector and radiuses.

            Parameters:
                thePnt Ellipse center.
                theVec Vector, normal to the plane of the ellipse.
                theRMajor Major ellipse radius.
                theRMinor Minor ellipse radius.
                theVecMaj Vector, direction of the ellipse's main axis.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "ellipse")
            return anObj

        ## Create an ellipse with given radiuses.
        #  Center of the ellipse will be in the origin of global
        #  coordinate system and normal vector will be codirected with Z axis
        #  @param theRMajor Major ellipse radius.
        #  @param theRMinor Minor ellipse radius.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created ellipse.
        @ManageTransactions("CurvesOp")
        def MakeEllipseRR(self, theRMajor, theRMinor, theName=None):
            """
            Create an ellipse with given radiuses.
            Center of the ellipse will be in the origin of global
            coordinate system and normal vector will be codirected with Z axis

            Parameters:
                theRMajor Major ellipse radius.
                theRMinor Minor ellipse radius.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
            New GEOM.GEOM_Object, containing the created ellipse.
            """
            anObj = self.CurvesOp.MakeEllipse(None, None, theRMajor, theRMinor)
            RaiseIfFailed("MakeEllipse", self.CurvesOp)
            self._autoPublish(anObj, theName, "ellipse")
            return anObj

        ## Create a polyline on the set of points.
        #  @param thePoints Sequence of points for the polyline.
        #  @param theIsClosed If True, build a closed wire.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created polyline.
        #
        #  @ref tui_creation_curve "Example"
        @ManageTransactions("CurvesOp")
        def MakePolyline(self, thePoints, theIsClosed=False, theName=None):
            """
            Create a polyline on the set of points.

            Parameters:
                thePoints Sequence of points for the polyline.
                theIsClosed If True, build a closed wire.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created polyline.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakePolyline(thePoints, theIsClosed)
            RaiseIfFailed("MakePolyline", self.CurvesOp)
            self._autoPublish(anObj, theName, "polyline")
            return anObj

        ## Create bezier curve on the set of points.
        #  @param thePoints Sequence of points for the bezier curve.
        #  @param theIsClosed If True, build a closed curve.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created bezier curve.
        #
        #  @ref tui_creation_curve "Example"
        @ManageTransactions("CurvesOp")
        def MakeBezier(self, thePoints, theIsClosed=False, theName=None):
            """
            Create bezier curve on the set of points.

            Parameters:
                thePoints Sequence of points for the bezier curve.
                theIsClosed If True, build a closed curve.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created bezier curve.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeSplineBezier(thePoints, theIsClosed)
            RaiseIfFailed("MakeSplineBezier", self.CurvesOp)
            self._autoPublish(anObj, theName, "bezier")
            return anObj

        ## Create B-Spline curve on the set of points.
        #  @param thePoints Sequence of points for the B-Spline curve.
        #  @param theIsClosed If True, build a closed curve.
        #  @param theDoReordering If TRUE, the algo does not follow the order of
        #                         \a thePoints but searches for the closest vertex.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created B-Spline curve.
        #
        #  @ref tui_creation_curve "Example"
        @ManageTransactions("CurvesOp")
        def MakeInterpol(self, thePoints, theIsClosed=False, theDoReordering=False, theName=None):
            """
            Create B-Spline curve on the set of points.

            Parameters:
                thePoints Sequence of points for the B-Spline curve.
                theIsClosed If True, build a closed curve.
                theDoReordering If True, the algo does not follow the order of
                                thePoints but searches for the closest vertex.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created B-Spline curve.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeSplineInterpolation(thePoints, theIsClosed, theDoReordering)
            RaiseIfFailed("MakeInterpol", self.CurvesOp)
            self._autoPublish(anObj, theName, "bspline")
            return anObj

        ## Create B-Spline curve on the set of points.
        #  @param thePoints Sequence of points for the B-Spline curve.
        #  @param theFirstVec Vector object, defining the curve direction at its first point.
        #  @param theLastVec Vector object, defining the curve direction at its last point.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created B-Spline curve.
        #
        #  @ref tui_creation_curve "Example"
        @ManageTransactions("CurvesOp")
        def MakeInterpolWithTangents(self, thePoints, theFirstVec, theLastVec, theName=None):
            """
            Create B-Spline curve on the set of points.

            Parameters:
                thePoints Sequence of points for the B-Spline curve.
                theFirstVec Vector object, defining the curve direction at its first point.
                theLastVec Vector object, defining the curve direction at its last point.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created B-Spline curve.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeSplineInterpolWithTangents(thePoints, theFirstVec, theLastVec)
            RaiseIfFailed("MakeInterpolWithTangents", self.CurvesOp)
            self._autoPublish(anObj, theName, "bspline")
            return anObj

        ## Creates a curve using the parametric definition of the basic points.
        #  @param thexExpr parametric equation of the coordinates X.
        #  @param theyExpr parametric equation of the coordinates Y.
        #  @param thezExpr parametric equation of the coordinates Z.
        #  @param theParamMin the minimal value of the parameter.
        #  @param theParamMax the maximum value of the parameter.
        #  @param theParamStep the number of steps if theNewMethod = True, else step value of the parameter.
        #  @param theCurveType the type of the curve,
        #         one of GEOM.Polyline, GEOM.Bezier, GEOM.Interpolation.
        #  @param theNewMethod flag for switching to the new method if the flag is set to false a deprecated method is used which can lead to a bug.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created curve.
        #
        #  @ref tui_creation_curve "Example"
        @ManageTransactions("CurvesOp")
        def MakeCurveParametric(self, thexExpr, theyExpr, thezExpr,
                                theParamMin, theParamMax, theParamStep, theCurveType, theNewMethod=False, theName=None ):
            """
            Creates a curve using the parametric definition of the basic points.

            Parameters:
                thexExpr parametric equation of the coordinates X.
                theyExpr parametric equation of the coordinates Y.
                thezExpr parametric equation of the coordinates Z.
                theParamMin the minimal value of the parameter.
                theParamMax the maximum value of the parameter.
                theParamStep the number of steps if theNewMethod = True, else step value of the parameter.
                theCurveType the type of the curve,
                             one of GEOM.Polyline, GEOM.Bezier, GEOM.Interpolation.
                theNewMethod flag for switching to the new method if the flag is set to false a deprecated
                             method is used which can lead to a bug.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created curve.
            """
            theParamMin,theParamMax,theParamStep,Parameters = ParseParameters(theParamMin,theParamMax,theParamStep)
            if theNewMethod:
              anObj = self.CurvesOp.MakeCurveParametricNew(thexExpr,theyExpr,thezExpr,theParamMin,theParamMax,theParamStep,theCurveType)
            else:
              anObj = self.CurvesOp.MakeCurveParametric(thexExpr,theyExpr,thezExpr,theParamMin,theParamMax,theParamStep,theCurveType)
            RaiseIfFailed("MakeCurveParametric", self.CurvesOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "curve")
            return anObj

        ## Create an isoline curve on a face.
        #  @param theFace the face for which an isoline is created.
        #  @param IsUIsoline True for U-isoline creation; False for V-isoline
        #         creation.
        #  @param theParameter the U parameter for U-isoline or V parameter
        #         for V-isoline.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created isoline edge or
        #          a compound of edges.
        #
        #  @ref tui_creation_curve "Example"
        @ManageTransactions("CurvesOp")
        def MakeIsoline(self, theFace, IsUIsoline, theParameter, theName=None):
            """
            Create an isoline curve on a face.

            Parameters:
                theFace the face for which an isoline is created.
                IsUIsoline True for U-isoline creation; False for V-isoline
                           creation.
                theParameter the U parameter for U-isoline or V parameter
                             for V-isoline.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created isoline edge or a
                compound of edges.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.CurvesOp.MakeIsoline(theFace, IsUIsoline, theParameter)
            RaiseIfFailed("MakeIsoline", self.CurvesOp)
            if IsUIsoline:
                self._autoPublish(anObj, theName, "U-Isoline")
            else:
                self._autoPublish(anObj, theName, "V-Isoline")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created wire.
        #
        #  @ref tui_sketcher_page "Example"
        @ManageTransactions("CurvesOp")
        def MakeSketcher(self, theCommand, theWorkingPlane = [0,0,0, 0,0,1, 1,0,0], theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created wire.
            """
            # Example: see GEOM_TestAll.py
            theCommand,Parameters = ParseSketcherCommand(theCommand)
            anObj = self.CurvesOp.MakeSketcher(theCommand, theWorkingPlane)
            RaiseIfFailed("MakeSketcher", self.CurvesOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "wire")
            return anObj

        ## Create a sketcher (wire or face), following the textual description,
        #  passed through <VAR>theCommand</VAR> argument. \n
        #  For format of the description string see MakeSketcher() method.\n
        #  @param theCommand String, defining the sketcher in local
        #                    coordinates of the working plane.
        #  @param theWorkingPlane Planar Face or LCS(Marker) of the working plane.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created wire.
        #
        #  @ref tui_sketcher_page "Example"
        @ManageTransactions("CurvesOp")
        def MakeSketcherOnPlane(self, theCommand, theWorkingPlane, theName=None):
            """
            Create a sketcher (wire or face), following the textual description,
            passed through theCommand argument.
            For format of the description string see geompy.MakeSketcher() method.

            Parameters:
                theCommand String, defining the sketcher in local
                           coordinates of the working plane.
                theWorkingPlane Planar Face or LCS(Marker) of the working plane.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created wire.
            """
            theCommand,Parameters = ParseSketcherCommand(theCommand)
            anObj = self.CurvesOp.MakeSketcherOnPlane(theCommand, theWorkingPlane)
            RaiseIfFailed("MakeSketcherOnPlane", self.CurvesOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "wire")
            return anObj

        ## Obtain a 2D sketcher interface
        #  @return An instance of @ref gsketcher.Sketcher2D "Sketcher2D" interface
        def Sketcher2D (self):
            """
            Obtain a 2D sketcher interface.

            Example of usage:
               sk = geompy.Sketcher2D()
               sk.addPoint(20, 20)
               sk.addSegmentRelative(15, 70)
               sk.addSegmentPerpY(50)
               sk.addArcRadiusRelative(25, 15, 14.5, 0)
               sk.addArcCenterAbsolute(1, 1, 50, 50, 0, 0)
               sk.addArcDirectionRadiusLength(20, 20, 101, 162.13)
               sk.close()
               Sketch_1 = sk.wire(geomObj_1)
            """
            sk = Sketcher2D (self)
            return sk

        ## Create a sketcher wire, following the numerical description,
        #  passed through <VAR>theCoordinates</VAR> argument. \n
        #  @param theCoordinates double values, defining points to create a wire,
        #                                                      passing from it.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created wire.
        #
        #  @ref tui_3dsketcher_page "Example"
        @ManageTransactions("CurvesOp")
        def Make3DSketcher(self, theCoordinates, theName=None):
            """
            Create a sketcher wire, following the numerical description,
            passed through theCoordinates argument.

            Parameters:
                theCoordinates double values, defining points to create a wire,
                               passing from it.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM_Object, containing the created wire.
            """
            theCoordinates,Parameters = ParseParameters(theCoordinates)
            anObj = self.CurvesOp.Make3DSketcher(theCoordinates)
            RaiseIfFailed("Make3DSketcher", self.CurvesOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "wire")
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

        ## Obtain a 2D polyline creation interface
        #  @return An instance of @ref gsketcher.Polyline2D "Polyline2D" interface
        #
        #  @ref tui_3dsketcher_page "Example"
        def Polyline2D (self):
            """
            Obtain a 2D polyline creation interface.

            Example of usage:
                pl = geompy.Polyline2D()
                pl.addSection("section 1", GEOM.Polyline, True)
                pl.addPoints(0, 0, 10, 0, 10, 10)
                pl.addSection("section 2", GEOM.Interpolation, False)
                pl.addPoints(20, 0, 30, 0, 30, 10)
                resultObj = pl.result(WorkingPlane)
            """
            pl = Polyline2D (self)
            return pl

        # end of l3_sketcher
        ## @}

        ## @addtogroup l3_3d_primitives
        ## @{

        ## Create a box by coordinates of two opposite vertices.
        #
        #  @param x1,y1,z1 double values, defining first point it.
        #  @param x2,y2,z2 double values, defining first point it.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created box.
        #
        #  @ref tui_creation_box "Example"
        def MakeBox(self, x1, y1, z1, x2, y2, z2, theName=None):
            """
            Create a box by coordinates of two opposite vertices.

            Parameters:
                x1,y1,z1 double values, defining first point.
                x2,y2,z2 double values, defining second point.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created box.
            """
            # Example: see GEOM_TestAll.py
            pnt1 = self.MakeVertex(x1,y1,z1)
            pnt2 = self.MakeVertex(x2,y2,z2)
            # note: auto-publishing is done in self.MakeBoxTwoPnt()
            return self.MakeBoxTwoPnt(pnt1, pnt2, theName)

        ## Create a box with specified dimensions along the coordinate axes
        #  and with edges, parallel to the coordinate axes.
        #  Center of the box will be at point (DX/2, DY/2, DZ/2).
        #  @param theDX Length of Box edges, parallel to OX axis.
        #  @param theDY Length of Box edges, parallel to OY axis.
        #  @param theDZ Length of Box edges, parallel to OZ axis.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created box.
        #
        #  @ref tui_creation_box "Example"
        @ManageTransactions("PrimOp")
        def MakeBoxDXDYDZ(self, theDX, theDY, theDZ, theName=None):
            """
            Create a box with specified dimensions along the coordinate axes
            and with edges, parallel to the coordinate axes.
            Center of the box will be at point (DX/2, DY/2, DZ/2).

            Parameters:
                theDX Length of Box edges, parallel to OX axis.
                theDY Length of Box edges, parallel to OY axis.
                theDZ Length of Box edges, parallel to OZ axis.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created box.
            """
            # Example: see GEOM_TestAll.py
            theDX,theDY,theDZ,Parameters = ParseParameters(theDX, theDY, theDZ)
            anObj = self.PrimOp.MakeBoxDXDYDZ(theDX, theDY, theDZ)
            RaiseIfFailed("MakeBoxDXDYDZ", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "box")
            return anObj

        ## Create a box with two specified opposite vertices,
        #  and with edges, parallel to the coordinate axes
        #  @param thePnt1 First of two opposite vertices.
        #  @param thePnt2 Second of two opposite vertices.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created box.
        #
        #  @ref tui_creation_box "Example"
        @ManageTransactions("PrimOp")
        def MakeBoxTwoPnt(self, thePnt1, thePnt2, theName=None):
            """
            Create a box with two specified opposite vertices,
            and with edges, parallel to the coordinate axes

            Parameters:
                thePnt1 First of two opposite vertices.
                thePnt2 Second of two opposite vertices.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created box.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakeBoxTwoPnt(thePnt1, thePnt2)
            RaiseIfFailed("MakeBoxTwoPnt", self.PrimOp)
            self._autoPublish(anObj, theName, "box")
            return anObj

        ## Create a face with specified dimensions with edges parallel to coordinate axes.
        #  @param theH height of Face.
        #  @param theW width of Face.
        #  @param theOrientation face orientation: 1-OXY, 2-OYZ, 3-OZX
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_face "Example"
        @ManageTransactions("PrimOp")
        def MakeFaceHW(self, theH, theW, theOrientation, theName=None):
            """
            Create a face with specified dimensions with edges parallel to coordinate axes.

            Parameters:
                theH height of Face.
                theW width of Face.
                theOrientation face orientation: 1-OXY, 2-OYZ, 3-OZX
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created face.
            """
            # Example: see GEOM_TestAll.py
            theH,theW,Parameters = ParseParameters(theH, theW)
            anObj = self.PrimOp.MakeFaceHW(theH, theW, theOrientation)
            RaiseIfFailed("MakeFaceHW", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "rectangle")
            return anObj

        ## Create a face from another plane and two sizes,
        #  vertical size and horisontal size.
        #  @param theObj   Normale vector to the creating face or
        #  the face object.
        #  @param theH     Height (vertical size).
        #  @param theW     Width (horisontal size).
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_face "Example"
        @ManageTransactions("PrimOp")
        def MakeFaceObjHW(self, theObj, theH, theW, theName=None):
            """
            Create a face from another plane and two sizes,
            vertical size and horisontal size.

            Parameters:
                theObj   Normale vector to the creating face or
                         the face object.
                theH     Height (vertical size).
                theW     Width (horisontal size).
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM_Object, containing the created face.
            """
            # Example: see GEOM_TestAll.py
            theH,theW,Parameters = ParseParameters(theH, theW)
            anObj = self.PrimOp.MakeFaceObjHW(theObj, theH, theW)
            RaiseIfFailed("MakeFaceObjHW", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "rectangle")
            return anObj

        ## Create a disk with given center, normal vector and radius.
        #  @param thePnt Disk center.
        #  @param theVec Vector, normal to the plane of the disk.
        #  @param theR Disk radius.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created disk.
        #
        #  @ref tui_creation_disk "Example"
        @ManageTransactions("PrimOp")
        def MakeDiskPntVecR(self, thePnt, theVec, theR, theName=None):
            """
            Create a disk with given center, normal vector and radius.

            Parameters:
                thePnt Disk center.
                theVec Vector, normal to the plane of the disk.
                theR Disk radius.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created disk.
            """
            # Example: see GEOM_TestAll.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.PrimOp.MakeDiskPntVecR(thePnt, theVec, theR)
            RaiseIfFailed("MakeDiskPntVecR", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "disk")
            return anObj

        ## Create a disk, passing through three given points
        #  @param thePnt1,thePnt2,thePnt3 Points, defining the disk.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created disk.
        #
        #  @ref tui_creation_disk "Example"
        @ManageTransactions("PrimOp")
        def MakeDiskThreePnt(self, thePnt1, thePnt2, thePnt3, theName=None):
            """
            Create a disk, passing through three given points

            Parameters:
                thePnt1,thePnt2,thePnt3 Points, defining the disk.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created disk.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakeDiskThreePnt(thePnt1, thePnt2, thePnt3)
            RaiseIfFailed("MakeDiskThreePnt", self.PrimOp)
            self._autoPublish(anObj, theName, "disk")
            return anObj

        ## Create a disk with specified dimensions along OX-OY coordinate axes.
        #  @param theR Radius of Face.
        #  @param theOrientation set the orientation belong axis OXY or OYZ or OZX
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created disk.
        #
        #  @ref tui_creation_face "Example"
        @ManageTransactions("PrimOp")
        def MakeDiskR(self, theR, theOrientation, theName=None):
            """
            Create a disk with specified dimensions along OX-OY coordinate axes.

            Parameters:
                theR Radius of Face.
                theOrientation set the orientation belong axis OXY or OYZ or OZX
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "disk")
            return anObj

        ## Create a cylinder with given base point, axis, radius and height.
        #  @param thePnt Central point of cylinder base.
        #  @param theAxis Cylinder axis.
        #  @param theR Cylinder radius.
        #  @param theH Cylinder height.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created cylinder.
        #
        #  @ref tui_creation_cylinder "Example"
        @ManageTransactions("PrimOp")
        def MakeCylinder(self, thePnt, theAxis, theR, theH, theName=None):
            """
            Create a cylinder with given base point, axis, radius and height.

            Parameters:
                thePnt Central point of cylinder base.
                theAxis Cylinder axis.
                theR Cylinder radius.
                theH Cylinder height.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created cylinder.
            """
            # Example: see GEOM_TestAll.py
            theR,theH,Parameters = ParseParameters(theR, theH)
            anObj = self.PrimOp.MakeCylinderPntVecRH(thePnt, theAxis, theR, theH)
            RaiseIfFailed("MakeCylinderPntVecRH", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "cylinder")
            return anObj
            
        ## Create a portion of cylinder with given base point, axis, radius, height and angle.
        #  @param thePnt Central point of cylinder base.
        #  @param theAxis Cylinder axis.
        #  @param theR Cylinder radius.
        #  @param theH Cylinder height.
        #  @param theA Cylinder angle in radians.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created cylinder.
        #
        #  @ref tui_creation_cylinder "Example"
        @ManageTransactions("PrimOp")
        def MakeCylinderA(self, thePnt, theAxis, theR, theH, theA, theName=None):
            """
            Create a portion of cylinder with given base point, axis, radius, height and angle.

            Parameters:
                thePnt Central point of cylinder base.
                theAxis Cylinder axis.
                theR Cylinder radius.
                theH Cylinder height.
                theA Cylinder angle in radians.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created cylinder.
            """
            # Example: see GEOM_TestAll.py
            flag = False
            if isinstance(theA,str):
                flag = True
            theR,theH,theA,Parameters = ParseParameters(theR, theH, theA)
	    if flag:
                theA = theA*math.pi/180.
	    if theA<=0. or theA>=2*math.pi:
	      raise ValueError("The angle parameter should be strictly between 0 and 2*pi.")
	    anObj = self.PrimOp.MakeCylinderPntVecRHA(thePnt, theAxis, theR, theH, theA)
	    RaiseIfFailed("MakeCylinderPntVecRHA", self.PrimOp)
	    anObj.SetParameters(Parameters)
	    self._autoPublish(anObj, theName, "cylinder")
            return anObj

        ## Create a cylinder with given radius and height at
        #  the origin of coordinate system. Axis of the cylinder
        #  will be collinear to the OZ axis of the coordinate system.
        #  @param theR Cylinder radius.
        #  @param theH Cylinder height.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created cylinder.
        #
        #  @ref tui_creation_cylinder "Example"
        @ManageTransactions("PrimOp")
        def MakeCylinderRH(self, theR, theH, theName=None):
            """
            Create a cylinder with given radius and height at
            the origin of coordinate system. Axis of the cylinder
            will be collinear to the OZ axis of the coordinate system.

            Parameters:
                theR Cylinder radius.
                theH Cylinder height.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created cylinder.
            """
            # Example: see GEOM_TestAll.py
            theR,theH,Parameters = ParseParameters(theR, theH)
            anObj = self.PrimOp.MakeCylinderRH(theR, theH)
            RaiseIfFailed("MakeCylinderRH", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "cylinder")
            return anObj
            
        ## Create a portion of cylinder with given radius, height and angle at
        #  the origin of coordinate system. Axis of the cylinder
        #  will be collinear to the OZ axis of the coordinate system.
        #  @param theR Cylinder radius.
        #  @param theH Cylinder height.
        #  @param theA Cylinder angle in radians.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created cylinder.
        #
        #  @ref tui_creation_cylinder "Example"
        @ManageTransactions("PrimOp")
        def MakeCylinderRHA(self, theR, theH, theA, theName=None):
            """
            Create a portion of cylinder with given radius, height and angle at
            the origin of coordinate system. Axis of the cylinder
            will be collinear to the OZ axis of the coordinate system.

            Parameters:
                theR Cylinder radius.
                theH Cylinder height.
                theA Cylinder angle in radians.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created cylinder.
            """
            # Example: see GEOM_TestAll.py
            flag = False
            if isinstance(theA,str):
                flag = True
            theR,theH,theA,Parameters = ParseParameters(theR, theH, theA)
            if flag:
                theA = theA*math.pi/180.
            if theA<=0. or theA>=2*math.pi:
	      raise ValueError("The angle parameter should be strictly between 0 and 2*pi.")
            anObj = self.PrimOp.MakeCylinderRHA(theR, theH, theA)
            RaiseIfFailed("MakeCylinderRHA", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "cylinder")
            return anObj

        ## Create a sphere with given center and radius.
        #  @param thePnt Sphere center.
        #  @param theR Sphere radius.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created sphere.
        #
        #  @ref tui_creation_sphere "Example"
        @ManageTransactions("PrimOp")
        def MakeSpherePntR(self, thePnt, theR, theName=None):
            """
            Create a sphere with given center and radius.

            Parameters:
                thePnt Sphere center.
                theR Sphere radius.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created sphere.
            """
            # Example: see GEOM_TestAll.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.PrimOp.MakeSpherePntR(thePnt, theR)
            RaiseIfFailed("MakeSpherePntR", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "sphere")
            return anObj

        ## Create a sphere with given center and radius.
        #  @param x,y,z Coordinates of sphere center.
        #  @param theR Sphere radius.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created sphere.
        #
        #  @ref tui_creation_sphere "Example"
        def MakeSphere(self, x, y, z, theR, theName=None):
            """
            Create a sphere with given center and radius.

            Parameters:
                x,y,z Coordinates of sphere center.
                theR Sphere radius.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created sphere.
            """
            # Example: see GEOM_TestAll.py
            point = self.MakeVertex(x, y, z)
            # note: auto-publishing is done in self.MakeSpherePntR()
            anObj = self.MakeSpherePntR(point, theR, theName)
            return anObj

        ## Create a sphere with given radius at the origin of coordinate system.
        #  @param theR Sphere radius.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created sphere.
        #
        #  @ref tui_creation_sphere "Example"
        @ManageTransactions("PrimOp")
        def MakeSphereR(self, theR, theName=None):
            """
            Create a sphere with given radius at the origin of coordinate system.

            Parameters:
                theR Sphere radius.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created sphere.
            """
            # Example: see GEOM_TestAll.py
            theR,Parameters = ParseParameters(theR)
            anObj = self.PrimOp.MakeSphereR(theR)
            RaiseIfFailed("MakeSphereR", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "sphere")
            return anObj

        ## Create a cone with given base point, axis, height and radiuses.
        #  @param thePnt Central point of the first cone base.
        #  @param theAxis Cone axis.
        #  @param theR1 Radius of the first cone base.
        #  @param theR2 Radius of the second cone base.
        #    \note If both radiuses are non-zero, the cone will be truncated.
        #    \note If the radiuses are equal, a cylinder will be created instead.
        #  @param theH Cone height.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created cone.
        #
        #  @ref tui_creation_cone "Example"
        @ManageTransactions("PrimOp")
        def MakeCone(self, thePnt, theAxis, theR1, theR2, theH, theName=None):
            """
            Create a cone with given base point, axis, height and radiuses.

            Parameters:
                thePnt Central point of the first cone base.
                theAxis Cone axis.
                theR1 Radius of the first cone base.
                theR2 Radius of the second cone base.
                theH Cone height.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "cone")
            return anObj

        ## Create a cone with given height and radiuses at
        #  the origin of coordinate system. Axis of the cone will
        #  be collinear to the OZ axis of the coordinate system.
        #  @param theR1 Radius of the first cone base.
        #  @param theR2 Radius of the second cone base.
        #    \note If both radiuses are non-zero, the cone will be truncated.
        #    \note If the radiuses are equal, a cylinder will be created instead.
        #  @param theH Cone height.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created cone.
        #
        #  @ref tui_creation_cone "Example"
        @ManageTransactions("PrimOp")
        def MakeConeR1R2H(self, theR1, theR2, theH, theName=None):
            """
            Create a cone with given height and radiuses at
            the origin of coordinate system. Axis of the cone will
            be collinear to the OZ axis of the coordinate system.

            Parameters:
                theR1 Radius of the first cone base.
                theR2 Radius of the second cone base.
                theH Cone height.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "cone")
            return anObj

        ## Create a torus with given center, normal vector and radiuses.
        #  @param thePnt Torus central point.
        #  @param theVec Torus axis of symmetry.
        #  @param theRMajor Torus major radius.
        #  @param theRMinor Torus minor radius.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created torus.
        #
        #  @ref tui_creation_torus "Example"
        @ManageTransactions("PrimOp")
        def MakeTorus(self, thePnt, theVec, theRMajor, theRMinor, theName=None):
            """
            Create a torus with given center, normal vector and radiuses.

            Parameters:
                thePnt Torus central point.
                theVec Torus axis of symmetry.
                theRMajor Torus major radius.
                theRMinor Torus minor radius.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

           Returns:
                New GEOM.GEOM_Object, containing the created torus.
            """
            # Example: see GEOM_TestAll.py
            theRMajor,theRMinor,Parameters = ParseParameters(theRMajor,theRMinor)
            anObj = self.PrimOp.MakeTorusPntVecRR(thePnt, theVec, theRMajor, theRMinor)
            RaiseIfFailed("MakeTorusPntVecRR", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "torus")
            return anObj

        ## Create a torus with given radiuses at the origin of coordinate system.
        #  @param theRMajor Torus major radius.
        #  @param theRMinor Torus minor radius.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created torus.
        #
        #  @ref tui_creation_torus "Example"
        @ManageTransactions("PrimOp")
        def MakeTorusRR(self, theRMajor, theRMinor, theName=None):
            """
           Create a torus with given radiuses at the origin of coordinate system.

           Parameters:
                theRMajor Torus major radius.
                theRMinor Torus minor radius.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

           Returns:
                New GEOM.GEOM_Object, containing the created torus.
            """
            # Example: see GEOM_TestAll.py
            theRMajor,theRMinor,Parameters = ParseParameters(theRMajor,theRMinor)
            anObj = self.PrimOp.MakeTorusRR(theRMajor, theRMinor)
            RaiseIfFailed("MakeTorusRR", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "torus")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        @ManageTransactions("PrimOp")
        def MakePrism(self, theBase, thePoint1, thePoint2, theScaleFactor = -1.0, theName=None):
            """
            Create a shape by extrusion of the base shape along a vector, defined by two points.

            Parameters:
                theBase Base shape to be extruded.
                thePoint1 First end of extrusion vector.
                thePoint2 Second end of extrusion vector.
                theScaleFactor Use it to make prism with scaled second base.
                               Nagative value means not scaled second base.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "prism")
            return anObj

        ## Create a shape by extrusion of the base shape along a
        #  vector, defined by two points, in 2 Ways (forward/backward).
        #  @param theBase Base shape to be extruded.
        #  @param thePoint1 First end of extrusion vector.
        #  @param thePoint2 Second end of extrusion vector.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        @ManageTransactions("PrimOp")
        def MakePrism2Ways(self, theBase, thePoint1, thePoint2, theName=None):
            """
            Create a shape by extrusion of the base shape along a
            vector, defined by two points, in 2 Ways (forward/backward).

            Parameters:
                theBase Base shape to be extruded.
                thePoint1 First end of extrusion vector.
                thePoint2 Second end of extrusion vector.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created prism.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakePrismTwoPnt2Ways(theBase, thePoint1, thePoint2)
            RaiseIfFailed("MakePrismTwoPnt", self.PrimOp)
            self._autoPublish(anObj, theName, "prism")
            return anObj

        ## Create a shape by extrusion of the base shape along the vector,
        #  i.e. all the space, transfixed by the base shape during its translation
        #  along the vector on the given distance.
        #  @param theBase Base shape to be extruded.
        #  @param theVec Direction of extrusion.
        #  @param theH Prism dimension along theVec.
        #  @param theScaleFactor Use it to make prism with scaled second base.
        #                        Negative value means not scaled second base.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        @ManageTransactions("PrimOp")
        def MakePrismVecH(self, theBase, theVec, theH, theScaleFactor = -1.0, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "prism")
            return anObj

        ## Create a shape by extrusion of the base shape along the vector,
        #  i.e. all the space, transfixed by the base shape during its translation
        #  along the vector on the given distance in 2 Ways (forward/backward).
        #  @param theBase Base shape to be extruded.
        #  @param theVec Direction of extrusion.
        #  @param theH Prism dimension along theVec in forward direction.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        @ManageTransactions("PrimOp")
        def MakePrismVecH2Ways(self, theBase, theVec, theH, theName=None):
            """
            Create a shape by extrusion of the base shape along the vector,
            i.e. all the space, transfixed by the base shape during its translation
            along the vector on the given distance in 2 Ways (forward/backward).

            Parameters:
                theBase Base shape to be extruded.
                theVec Direction of extrusion.
                theH Prism dimension along theVec in forward direction.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created prism.
            """
            # Example: see GEOM_TestAll.py
            theH,Parameters = ParseParameters(theH)
            anObj = self.PrimOp.MakePrismVecH2Ways(theBase, theVec, theH)
            RaiseIfFailed("MakePrismVecH2Ways", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "prism")
            return anObj

        ## Create a shape by extrusion of the base shape along the dx, dy, dz direction
        #  @param theBase Base shape to be extruded.
        #  @param theDX, theDY, theDZ Directions of extrusion.
        #  @param theScaleFactor Use it to make prism with scaled second base.
        #                        Nagative value means not scaled second base.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        @ManageTransactions("PrimOp")
        def MakePrismDXDYDZ(self, theBase, theDX, theDY, theDZ, theScaleFactor = -1.0, theName=None):
            """
            Create a shape by extrusion of the base shape along the dx, dy, dz direction

            Parameters:
                theBase Base shape to be extruded.
                theDX, theDY, theDZ Directions of extrusion.
                theScaleFactor Use it to make prism with scaled second base.
                               Nagative value means not scaled second base.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "prism")
            return anObj

        ## Create a shape by extrusion of the base shape along the dx, dy, dz direction
        #  i.e. all the space, transfixed by the base shape during its translation
        #  along the vector on the given distance in 2 Ways (forward/backward).
        #  @param theBase Base shape to be extruded.
        #  @param theDX, theDY, theDZ Directions of extrusion.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created prism.
        #
        #  @ref tui_creation_prism "Example"
        @ManageTransactions("PrimOp")
        def MakePrismDXDYDZ2Ways(self, theBase, theDX, theDY, theDZ, theName=None):
            """
            Create a shape by extrusion of the base shape along the dx, dy, dz direction
            i.e. all the space, transfixed by the base shape during its translation
            along the vector on the given distance in 2 Ways (forward/backward).

            Parameters:
                theBase Base shape to be extruded.
                theDX, theDY, theDZ Directions of extrusion.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created prism.
            """
            # Example: see GEOM_TestAll.py
            theDX,theDY,theDZ,Parameters = ParseParameters(theDX, theDY, theDZ)
            anObj = self.PrimOp.MakePrismDXDYDZ2Ways(theBase, theDX, theDY, theDZ)
            RaiseIfFailed("MakePrismDXDYDZ2Ways", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "prism")
            return anObj

        ## Create a shape by revolution of the base shape around the axis
        #  on the given angle, i.e. all the space, transfixed by the base
        #  shape during its rotation around the axis on the given angle.
        #  @param theBase Base shape to be rotated.
        #  @param theAxis Rotation axis.
        #  @param theAngle Rotation angle in radians.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created revolution.
        #
        #  @ref tui_creation_revolution "Example"
        @ManageTransactions("PrimOp")
        def MakeRevolution(self, theBase, theAxis, theAngle, theName=None):
            """
            Create a shape by revolution of the base shape around the axis
            on the given angle, i.e. all the space, transfixed by the base
            shape during its rotation around the axis on the given angle.

            Parameters:
                theBase Base shape to be rotated.
                theAxis Rotation axis.
                theAngle Rotation angle in radians.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created revolution.
            """
            # Example: see GEOM_TestAll.py
            theAngle,Parameters = ParseParameters(theAngle)
            anObj = self.PrimOp.MakeRevolutionAxisAngle(theBase, theAxis, theAngle)
            RaiseIfFailed("MakeRevolutionAxisAngle", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "revolution")
            return anObj

        ## Create a shape by revolution of the base shape around the axis
        #  on the given angle, i.e. all the space, transfixed by the base
        #  shape during its rotation around the axis on the given angle in
        #  both directions (forward/backward)
        #  @param theBase Base shape to be rotated.
        #  @param theAxis Rotation axis.
        #  @param theAngle Rotation angle in radians.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created revolution.
        #
        #  @ref tui_creation_revolution "Example"
        @ManageTransactions("PrimOp")
        def MakeRevolution2Ways(self, theBase, theAxis, theAngle, theName=None):
            """
            Create a shape by revolution of the base shape around the axis
            on the given angle, i.e. all the space, transfixed by the base
            shape during its rotation around the axis on the given angle in
            both directions (forward/backward).

            Parameters:
                theBase Base shape to be rotated.
                theAxis Rotation axis.
                theAngle Rotation angle in radians.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created revolution.
            """
            theAngle,Parameters = ParseParameters(theAngle)
            anObj = self.PrimOp.MakeRevolutionAxisAngle2Ways(theBase, theAxis, theAngle)
            RaiseIfFailed("MakeRevolutionAxisAngle2Ways", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "revolution")
            return anObj

        ## Create a face from a given set of contours.
        #  @param theContours either a list or a compound of edges/wires.
        #  @param theMinDeg a minimal degree of BSpline surface to create.
        #  @param theMaxDeg a maximal degree of BSpline surface to create.
        #  @param theTol2D a 2d tolerance to be reached.
        #  @param theTol3D a 3d tolerance to be reached.
        #  @param theNbIter a number of iteration of approximation algorithm.
        #  @param theMethod Kind of method to perform filling operation
        #         (see GEOM.filling_oper_method enum).
        #  @param isApprox if True, BSpline curves are generated in the process
        #                  of surface construction. By default it is False, that means
        #                  the surface is created using given curves. The usage of
        #                  Approximation makes the algorithm work slower, but allows
        #                  building the surface for rather complex cases.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object (face), containing the created filling surface.
        #
        #  @ref tui_creation_filling "Example"
        @ManageTransactions("PrimOp")
        def MakeFilling(self, theContours, theMinDeg=2, theMaxDeg=5, theTol2D=0.0001,
                        theTol3D=0.0001, theNbIter=0, theMethod=GEOM.FOM_Default, isApprox=0, theName=None):
            """
            Create a face from a given set of contours.

            Parameters:
                theContours either a list or a compound of edges/wires.
                theMinDeg a minimal degree of BSpline surface to create.
                theMaxDeg a maximal degree of BSpline surface to create.
                theTol2D a 2d tolerance to be reached.
                theTol3D a 3d tolerance to be reached.
                theNbIter a number of iteration of approximation algorithm.
                theMethod Kind of method to perform filling operation
                          (see GEOM.filling_oper_method enum).
                isApprox if True, BSpline curves are generated in the process
                         of surface construction. By default it is False, that means
                         the surface is created using given curves. The usage of
                         Approximation makes the algorithm work slower, but allows
                         building the surface for rather complex cases.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object (face), containing the created filling surface.

            Example of usage:
                filling = geompy.MakeFilling(compound, 2, 5, 0.0001, 0.0001, 5)
            """
            # Example: see GEOM_TestAll.py
            theMinDeg,theMaxDeg,theTol2D,theTol3D,theNbIter,Parameters = ParseParameters(theMinDeg, theMaxDeg, theTol2D, theTol3D, theNbIter)
            anObj = self.PrimOp.MakeFilling(ToList(theContours), theMinDeg, theMaxDeg,
                                            theTol2D, theTol3D, theNbIter,
                                            theMethod, isApprox)
            RaiseIfFailed("MakeFilling", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "filling")
            return anObj


        ## Create a face from a given set of contours.
        #  This method corresponds to MakeFilling() with isApprox=True.
        #  @param theContours either a list or a compound of edges/wires.
        #  @param theMinDeg a minimal degree of BSpline surface to create.
        #  @param theMaxDeg a maximal degree of BSpline surface to create.
        #  @param theTol3D a 3d tolerance to be reached.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object (face), containing the created filling surface.
        #
        #  @ref tui_creation_filling "Example"
        @ManageTransactions("PrimOp")
        def MakeFillingNew(self, theContours, theMinDeg=2, theMaxDeg=5, theTol3D=0.0001, theName=None):
            """
            Create a filling from the given compound of contours.
            This method corresponds to MakeFilling() with isApprox=True.

            Parameters:
                theContours either a list or a compound of edges/wires.
                theMinDeg a minimal degree of BSpline surface to create.
                theMaxDeg a maximal degree of BSpline surface to create.
                theTol3D a 3d tolerance to be reached.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object (face), containing the created filling surface.

            Example of usage:
                filling = geompy.MakeFillingNew(compound, 2, 5, 0.0001)
            """
            # Example: see GEOM_TestAll.py
            theMinDeg,theMaxDeg,theTol3D,Parameters = ParseParameters(theMinDeg, theMaxDeg, theTol3D)
            anObj = self.PrimOp.MakeFilling(ToList(theContours), theMinDeg, theMaxDeg,
                                            0, theTol3D, 0, GEOM.FOM_Default, True)
            RaiseIfFailed("MakeFillingNew", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "filling")
            return anObj

        ## Create a shell or solid passing through set of sections.Sections should be wires,edges or vertices.
        #  @param theSeqSections - set of specified sections.
        #  @param theModeSolid - mode defining building solid or shell
        #  @param thePreci - precision 3D used for smoothing
        #  @param theRuled - mode defining type of the result surfaces (ruled or smoothed).
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created shell or solid.
        #
        #  @ref swig_todo "Example"
        @ManageTransactions("PrimOp")
        def MakeThruSections(self, theSeqSections, theModeSolid, thePreci, theRuled, theName=None):
            """
            Create a shell or solid passing through set of sections.Sections should be wires,edges or vertices.

            Parameters:
                theSeqSections - set of specified sections.
                theModeSolid - mode defining building solid or shell
                thePreci - precision 3D used for smoothing
                theRuled - mode defining type of the result surfaces (ruled or smoothed).
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created shell or solid.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.PrimOp.MakeThruSections(theSeqSections,theModeSolid,thePreci,theRuled)
            RaiseIfFailed("MakeThruSections", self.PrimOp)
            self._autoPublish(anObj, theName, "filling")
            return anObj

        ## Create a shape by extrusion of the base shape along
        #  the path shape. The path shape can be a wire or an edge. It is
        #  possible to generate groups along with the result by means of
        #  setting the flag \a IsGenerateGroups.<BR>
        #  If \a thePath is a closed edge or wire and \a IsGenerateGroups is
        #  set, an error is occured. If \a thePath is not closed edge/wire,
        #  the following groups are returned:
        #  - If \a theBase is unclosed edge or wire: "Down", "Up", "Side1",
        #    "Side2";
        #  - If \a theBase is closed edge or wire, face or shell: "Down", "Up",
        #    "Other".
        #  .
        #  "Down" and "Up" groups contain:
        #  - Edges if \a theBase is edge or wire;
        #  - Faces if \a theBase is face or shell.<BR>
        #  .
        #  "Side1" and "Side2" groups contain edges generated from the first
        #  and last vertices of \a theBase. The first and last vertices are
        #  determined taking into account edge/wire orientation.<BR>
        #  "Other" group represents faces generated from the bounding edges of
        #  \a theBase.
        #
        #  @param theBase Base shape to be extruded.
        #  @param thePath Path shape to extrude the base shape along it.
        #  @param IsGenerateGroups flag that tells if it is necessary to
        #         create groups. It is equal to False by default.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created pipe if 
        #          \a IsGenerateGroups is not set. Otherwise it returns new
        #          GEOM.ListOfGO. Its first element is the created pipe, the
        #          remaining ones are created groups.
        #
        #  @ref tui_creation_pipe "Example"
        @ManageTransactions("PrimOp")
        def MakePipe(self, theBase, thePath,
                     IsGenerateGroups=False, theName=None):
            """
            Create a shape by extrusion of the base shape along
            the path shape. The path shape can be a wire or an edge. It is
            possible to generate groups along with the result by means of
            setting the flag IsGenerateGroups.
            If thePath is a closed edge or wire and IsGenerateGroups is
            set, an error is occured. If thePath is not closed edge/wire,
            the following groups are returned:
            - If theBase is unclosed edge or wire: "Down", "Up", "Side1",
              "Side2";
            - If theBase is closed edge or wire, face or shell: "Down", "Up",
              "Other".
            "Down" and "Up" groups contain:
            - Edges if theBase is edge or wire;
            - Faces if theBase is face or shell.
            "Side1" and "Side2" groups contain edges generated from the first
            and last vertices of theBase. The first and last vertices are
            determined taking into account edge/wire orientation.
            "Other" group represents faces generated from the bounding edges of
            theBase.

            Parameters:
                theBase Base shape to be extruded.
                thePath Path shape to extrude the base shape along it.
                IsGenerateGroups flag that tells if it is necessary to
                        create groups. It is equal to False by default.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created pipe if 
                IsGenerateGroups is not set. Otherwise it returns new
                GEOM.ListOfGO. Its first element is the created pipe, the
                remaining ones are created groups.
            """
            # Example: see GEOM_TestAll.py
            aList = self.PrimOp.MakePipe(theBase, thePath, IsGenerateGroups)
            RaiseIfFailed("MakePipe", self.PrimOp)

            if IsGenerateGroups:
              self._autoPublish(aList, theName, "pipe")
              return aList

            self._autoPublish(aList[0], theName, "pipe")
            return aList[0]

        ## Create a shape by extrusion of the profile shape along
        #  the path shape. The path shape can be a wire or an edge.
        #  the several profiles can be specified in the several locations of path.
        #  It is possible to generate groups along with the result by means of
        #  setting the flag \a IsGenerateGroups. For detailed information on
        #  groups that can be created please see the method MakePipe().
        #  @param theSeqBases - list of  Bases shape to be extruded.
        #  @param theLocations - list of locations on the path corresponding
        #                        specified list of the Bases shapes. Number of locations
        #                        should be equal to number of bases or list of locations can be empty.
        #  @param thePath - Path shape to extrude the base shape along it.
        #  @param theWithContact - the mode defining that the section is translated to be in
        #                          contact with the spine.
        #  @param theWithCorrection - defining that the section is rotated to be
        #                             orthogonal to the spine tangent in the correspondent point
        #  @param IsGenerateGroups - flag that tells if it is necessary to
        #                          create groups. It is equal to False by default.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created pipe if 
        #          \a IsGenerateGroups is not set. Otherwise it returns new
        #          GEOM.ListOfGO. Its first element is the created pipe, the
        #          remaining ones are created groups.
        #
        #  @ref tui_creation_pipe_with_diff_sec "Example"
        @ManageTransactions("PrimOp")
        def MakePipeWithDifferentSections(self, theSeqBases,
                                          theLocations, thePath,
                                          theWithContact, theWithCorrection,
                                          IsGenerateGroups=False, theName=None):
            """
            Create a shape by extrusion of the profile shape along
            the path shape. The path shape can be a wire or an edge.
            the several profiles can be specified in the several locations of path.
            It is possible to generate groups along with the result by means of
            setting the flag IsGenerateGroups. For detailed information on
            groups that can be created please see the method geompy.MakePipe().

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
                IsGenerateGroups - flag that tells if it is necessary to
                                 create groups. It is equal to False by default.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created pipe if 
                IsGenerateGroups is not set. Otherwise it returns new
                GEOM.ListOfGO. Its first element is the created pipe, the
                remaining ones are created groups.
            """
            aList = self.PrimOp.MakePipeWithDifferentSections(theSeqBases,
                                                              theLocations, thePath,
                                                              theWithContact, theWithCorrection,
                                                              False, IsGenerateGroups)
            RaiseIfFailed("MakePipeWithDifferentSections", self.PrimOp)

            if IsGenerateGroups:
              self._autoPublish(aList, theName, "pipe")
              return aList

            self._autoPublish(aList[0], theName, "pipe")
            return aList[0]

        ## Create a shape by extrusion of the profile shape along
        #  the path shape. This function is a version of
        #  MakePipeWithDifferentSections() with the same parameters, except
        #  eliminated theWithContact and theWithCorrection. So it is
        #  possible to find the description of all parameters is in this
        #  method. The difference is that this method performs the operation
        #  step by step, i.e. it creates pipes between each pair of neighbor
        #  sections and fuses them into a single shape.
        #
        #  @ref tui_creation_pipe_with_diff_sec "Example"
        @ManageTransactions("PrimOp")
        def MakePipeWithDifferentSectionsBySteps(self, theSeqBases,
                                                 theLocations, thePath,
                                                 IsGenerateGroups=False, theName=None):
            """
            Create a shape by extrusion of the profile shape along
            the path shape. This function is a version of
            MakePipeWithDifferentSections() with the same parameters, except
            eliminated theWithContact and theWithCorrection. So it is
            possible to find the description of all parameters is in this
            method. The difference is that this method performs the operation
            step by step, i.e. it creates pipes between each pair of neighbor
            sections and fuses them into a single shape.
            """
            aList = self.PrimOp.MakePipeWithDifferentSections(theSeqBases,
                                                              theLocations, thePath,
                                                              False, False,
                                                              True, IsGenerateGroups)
            RaiseIfFailed("MakePipeWithDifferentSectionsBySteps", self.PrimOp)

            if IsGenerateGroups:
              self._autoPublish(aList, theName, "pipe")
              return aList

            self._autoPublish(aList[0], theName, "pipe")
            return aList[0]

        ## Create a shape by extrusion of the profile shape along
        #  the path shape. The path shape can be a wire or an edge.
        #  the several profiles can be specified in the several locations of path.
        #  It is possible to generate groups along with the result by means of
        #  setting the flag \a IsGenerateGroups. For detailed information on
        #  groups that can be created please see the method MakePipe().
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
        #  @param IsGenerateGroups - flag that tells if it is necessary to
        #                          create groups. It is equal to False by default.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created solids if 
        #          \a IsGenerateGroups is not set. Otherwise it returns new
        #          GEOM.ListOfGO. Its first element is the created solids, the
        #          remaining ones are created groups.
        #
        #  @ref tui_creation_pipe_with_shell_sec "Example"
        @ManageTransactions("PrimOp")
        def MakePipeWithShellSections(self, theSeqBases, theSeqSubBases,
                                      theLocations, thePath,
                                      theWithContact, theWithCorrection,
                                      IsGenerateGroups=False, theName=None):
            """
            Create a shape by extrusion of the profile shape along
            the path shape. The path shape can be a wire or an edge.
            the several profiles can be specified in the several locations of path.
            It is possible to generate groups along with the result by means of
            setting the flag IsGenerateGroups. For detailed information on
            groups that can be created please see the method geompy.MakePipe().

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
                IsGenerateGroups - flag that tells if it is necessary to
                                 create groups. It is equal to False by default.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created solids if 
                IsGenerateGroups is not set. Otherwise it returns new
                GEOM.ListOfGO. Its first element is the created solids, the
                remaining ones are created groups.
            """
            aList = self.PrimOp.MakePipeWithShellSections(theSeqBases, theSeqSubBases,
                                                          theLocations, thePath,
                                                          theWithContact, theWithCorrection,
                                                          IsGenerateGroups)
            RaiseIfFailed("MakePipeWithShellSections", self.PrimOp)

            if IsGenerateGroups:
              self._autoPublish(aList, theName, "pipe")
              return aList

            self._autoPublish(aList[0], theName, "pipe")
            return aList[0]

        ## Create a shape by extrusion of the profile shape along
        #  the path shape. This function is used only for debug pipe
        #  functionality - it is a version of function MakePipeWithShellSections()
        #  which give a possibility to recieve information about
        #  creating pipe between each pair of sections step by step.
        @ManageTransactions("PrimOp")
        def MakePipeWithShellSectionsBySteps(self, theSeqBases, theSeqSubBases,
                                             theLocations, thePath,
                                             theWithContact, theWithCorrection,
                                             IsGenerateGroups=False, theName=None):
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
                aList = self.PrimOp.MakePipeWithShellSections(tmpSeqBases, tmpSeqSubBases,
                                                              tmpLocations, thePath,
                                                              theWithContact, theWithCorrection,
                                                              IsGenerateGroups)
                if self.PrimOp.IsDone() == 0:
                    print "Problems with pipe creation between ",i," and ",i+1," sections"
                    RaiseIfFailed("MakePipeWithShellSections", self.PrimOp)
                    break
                else:
                    print "Pipe between ",i," and ",i+1," sections is OK"
                    res.append(aList[0])
                    pass
                pass

            resc = self.MakeCompound(res)
            #resc = self.MakeSewing(res, 0.001)
            #print "resc: ",resc
            self._autoPublish(resc, theName, "pipe")
            return resc

        ## Create solids between given sections.
        #  It is possible to generate groups along with the result by means of
        #  setting the flag \a IsGenerateGroups. For detailed information on
        #  groups that can be created please see the method MakePipe().
        #  @param theSeqBases - list of sections (shell or face).
        #  @param theLocations - list of corresponding vertexes
        #  @param IsGenerateGroups - flag that tells if it is necessary to
        #         create groups. It is equal to False by default.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created solids if 
        #          \a IsGenerateGroups is not set. Otherwise it returns new
        #          GEOM.ListOfGO. Its first element is the created solids, the
        #          remaining ones are created groups.
        #
        #  @ref tui_creation_pipe_without_path "Example"
        @ManageTransactions("PrimOp")
        def MakePipeShellsWithoutPath(self, theSeqBases, theLocations,
                                      IsGenerateGroups=False, theName=None):
            """
            Create solids between given sections.
            It is possible to generate groups along with the result by means of
            setting the flag IsGenerateGroups. For detailed information on
            groups that can be created please see the method geompy.MakePipe().

            Parameters:
                theSeqBases - list of sections (shell or face).
                theLocations - list of corresponding vertexes
                IsGenerateGroups - flag that tells if it is necessary to
                                 create groups. It is equal to False by default.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created solids if 
                IsGenerateGroups is not set. Otherwise it returns new
                GEOM.ListOfGO. Its first element is the created solids, the
                remaining ones are created groups.
            """
            aList = self.PrimOp.MakePipeShellsWithoutPath(theSeqBases, theLocations,
                                                          IsGenerateGroups)
            RaiseIfFailed("MakePipeShellsWithoutPath", self.PrimOp)

            if IsGenerateGroups:
              self._autoPublish(aList, theName, "pipe")
              return aList

            self._autoPublish(aList[0], theName, "pipe")
            return aList[0]

        ## Create a shape by extrusion of the base shape along
        #  the path shape with constant bi-normal direction along the given vector.
        #  The path shape can be a wire or an edge.
        #  It is possible to generate groups along with the result by means of
        #  setting the flag \a IsGenerateGroups. For detailed information on
        #  groups that can be created please see the method MakePipe().
        #  @param theBase Base shape to be extruded.
        #  @param thePath Path shape to extrude the base shape along it.
        #  @param theVec Vector defines a constant binormal direction to keep the
        #                same angle beetween the direction and the sections
        #                along the sweep surface.
        #  @param IsGenerateGroups flag that tells if it is necessary to
        #         create groups. It is equal to False by default.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created pipe if 
        #          \a IsGenerateGroups is not set. Otherwise it returns new
        #          GEOM.ListOfGO. Its first element is the created pipe, the
        #          remaining ones are created groups.
        #
        #  @ref tui_creation_pipe "Example"
        @ManageTransactions("PrimOp")
        def MakePipeBiNormalAlongVector(self, theBase, thePath, theVec,
                                        IsGenerateGroups=False, theName=None):
            """
            Create a shape by extrusion of the base shape along
            the path shape with constant bi-normal direction along the given vector.
            The path shape can be a wire or an edge.
            It is possible to generate groups along with the result by means of
            setting the flag IsGenerateGroups. For detailed information on
            groups that can be created please see the method geompy.MakePipe().

            Parameters:
                theBase Base shape to be extruded.
                thePath Path shape to extrude the base shape along it.
                theVec Vector defines a constant binormal direction to keep the
                       same angle beetween the direction and the sections
                       along the sweep surface.
                IsGenerateGroups flag that tells if it is necessary to
                                 create groups. It is equal to False by default.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created pipe if 
                IsGenerateGroups is not set. Otherwise it returns new
                GEOM.ListOfGO. Its first element is the created pipe, the
                remaining ones are created groups.
            """
            # Example: see GEOM_TestAll.py
            aList = self.PrimOp.MakePipeBiNormalAlongVector(theBase, thePath,
                          theVec, IsGenerateGroups)
            RaiseIfFailed("MakePipeBiNormalAlongVector", self.PrimOp)

            if IsGenerateGroups:
              self._autoPublish(aList, theName, "pipe")
              return aList

            self._autoPublish(aList[0], theName, "pipe")
            return aList[0]

        ## Makes a thick solid from a shape. If the input is a surface shape
        #  (face or shell) the result is a thick solid. If an input shape is
        #  a solid the result is a hollowed solid with removed faces.
        #  @param theShape Face or Shell to get thick solid or solid to get
        #         hollowed solid.
        #  @param theThickness Thickness of the resulting solid
        #  @param theFacesIDs the list of face IDs to be removed from the
        #         result. It is ignored if \a theShape is a face or a shell.
        #         It is empty by default. 
        #  @param theInside If true the thickness is applied towards inside
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created solid
        #
        #  @ref tui_creation_thickness "Example"
        @ManageTransactions("PrimOp")
        def MakeThickSolid(self, theShape, theThickness,
                           theFacesIDs=[], theInside=False, theName=None):
            """
            Make a thick solid from a shape. If the input is a surface shape
            (face or shell) the result is a thick solid. If an input shape is
            a solid the result is a hollowed solid with removed faces.

            Parameters:
                 theShape Face or Shell to get thick solid or solid to get
                          hollowed solid.
                 theThickness Thickness of the resulting solid
                 theFacesIDs the list of face IDs to be removed from the
                          result. It is ignored if theShape is a face or a
                          shell. It is empty by default. 
                 theInside If true the thickness is applied towards inside
                 theName Object name; when specified, this parameter is used
                         for result publication in the study. Otherwise, if automatic
                         publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created solid
            """
            # Example: see GEOM_TestAll.py
            theThickness,Parameters = ParseParameters(theThickness)
            anObj = self.PrimOp.MakeThickening(theShape, theFacesIDs,
                                               theThickness, True, theInside)
            RaiseIfFailed("MakeThickSolid", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "thickSolid")
            return anObj


        ## Modifies a shape to make it a thick solid. If the input is a surface
        #  shape (face or shell) the result is a thick solid. If an input shape
        #  is a solid the result is a hollowed solid with removed faces.
        #  @param theShape Face or Shell to get thick solid or solid to get
        #         hollowed solid.
        #  @param theThickness Thickness of the resulting solid
        #  @param theFacesIDs the list of face IDs to be removed from the
        #         result. It is ignored if \a theShape is a face or a shell.
        #         It is empty by default. 
        #  @param theInside If true the thickness is applied towards inside
        #
        #  @return The modified shape
        #
        #  @ref tui_creation_thickness "Example"
        @ManageTransactions("PrimOp")
        def Thicken(self, theShape, theThickness, theFacesIDs=[], theInside=False):
            """
            Modifies a shape to make it a thick solid. If the input is a
            surface shape (face or shell) the result is a thick solid. If
            an input shape is a solid the result is a hollowed solid with
            removed faces.

            Parameters:
                theShape Face or Shell to get thick solid or solid to get
                         hollowed solid.
                theThickness Thickness of the resulting solid
                theFacesIDs the list of face IDs to be removed from the
                         result. It is ignored if \a theShape is a face or
                         a shell. It is empty by default. 
                theInside If true the thickness is applied towards inside

            Returns:
                The modified shape
            """
            # Example: see GEOM_TestAll.py
            theThickness,Parameters = ParseParameters(theThickness)
            anObj = self.PrimOp.MakeThickening(theShape, theFacesIDs,
                                               theThickness, False, theInside)
            RaiseIfFailed("Thicken", self.PrimOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Build a middle path of a pipe-like shape.
        #  The path shape can be a wire or an edge.
        #  @param theShape It can be closed or unclosed pipe-like shell
        #                  or a pipe-like solid.
        #  @param theBase1, theBase2 Two bases of the supposed pipe. This
        #                            should be wires or faces of theShape.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note It is not assumed that exact or approximate copy of theShape
        #        can be obtained by applying existing Pipe operation on the
        #        resulting "Path" wire taking theBase1 as the base - it is not
        #        always possible; though in some particular cases it might work
        #        it is not guaranteed. Thus, RestorePath function should not be
        #        considered as an exact reverse operation of the Pipe.
        #
        #  @return New GEOM.GEOM_Object, containing an edge or wire that represent
        #                                source pipe's "path".
        #
        #  @ref tui_creation_pipe_path "Example"
        @ManageTransactions("PrimOp")
        def RestorePath (self, theShape, theBase1, theBase2, theName=None):
            """
            Build a middle path of a pipe-like shape.
            The path shape can be a wire or an edge.

            Parameters:
                theShape It can be closed or unclosed pipe-like shell
                         or a pipe-like solid.
                theBase1, theBase2 Two bases of the supposed pipe. This
                                   should be wires or faces of theShape.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM_Object, containing an edge or wire that represent
                                 source pipe's path.
            """
            anObj = self.PrimOp.RestorePath(theShape, theBase1, theBase2)
            RaiseIfFailed("RestorePath", self.PrimOp)
            self._autoPublish(anObj, theName, "path")
            return anObj

        ## Build a middle path of a pipe-like shape.
        #  The path shape can be a wire or an edge.
        #  @param theShape It can be closed or unclosed pipe-like shell
        #                  or a pipe-like solid.
        #  @param listEdges1, listEdges2 Two bases of the supposed pipe. This
        #                                should be lists of edges of theShape.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note It is not assumed that exact or approximate copy of theShape
        #        can be obtained by applying existing Pipe operation on the
        #        resulting "Path" wire taking theBase1 as the base - it is not
        #        always possible; though in some particular cases it might work
        #        it is not guaranteed. Thus, RestorePath function should not be
        #        considered as an exact reverse operation of the Pipe.
        #
        #  @return New GEOM.GEOM_Object, containing an edge or wire that represent
        #                                source pipe's "path".
        #
        #  @ref tui_creation_pipe_path "Example"
        @ManageTransactions("PrimOp")
        def RestorePathEdges (self, theShape, listEdges1, listEdges2, theName=None):
            """
            Build a middle path of a pipe-like shape.
            The path shape can be a wire or an edge.

            Parameters:
                theShape It can be closed or unclosed pipe-like shell
                         or a pipe-like solid.
                listEdges1, listEdges2 Two bases of the supposed pipe. This
                                       should be lists of edges of theShape.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM_Object, containing an edge or wire that represent
                                 source pipe's path.
            """
            anObj = self.PrimOp.RestorePathEdges(theShape, listEdges1, listEdges2)
            RaiseIfFailed("RestorePath", self.PrimOp)
            self._autoPublish(anObj, theName, "path")
            return anObj

        # end of l3_complex
        ## @}

        ## @addtogroup l3_basic_go
        ## @{

        ## Create a linear edge with specified ends.
        #  @param thePnt1 Point for the first end of edge.
        #  @param thePnt2 Point for the second end of edge.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created edge.
        #
        #  @ref tui_creation_edge "Example"
        @ManageTransactions("ShapesOp")
        def MakeEdge(self, thePnt1, thePnt2, theName=None):
            """
            Create a linear edge with specified ends.

            Parameters:
                thePnt1 Point for the first end of edge.
                thePnt2 Point for the second end of edge.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created edge.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeEdge(thePnt1, thePnt2)
            RaiseIfFailed("MakeEdge", self.ShapesOp)
            self._autoPublish(anObj, theName, "edge")
            return anObj

        ## Create a new edge, corresponding to the given length on the given curve.
        #  @param theRefCurve The referenced curve (edge).
        #  @param theLength Length on the referenced curve. It can be negative.
        #  @param theStartPoint Any point can be selected for it, the new edge will begin
        #                       at the end of \a theRefCurve, close to the selected point.
        #                       If None, start from the first point of \a theRefCurve.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created edge.
        #
        #  @ref tui_creation_edge "Example"
        @ManageTransactions("ShapesOp")
        def MakeEdgeOnCurveByLength(self, theRefCurve, theLength, theStartPoint = None, theName=None):
            """
            Create a new edge, corresponding to the given length on the given curve.

            Parameters:
                theRefCurve The referenced curve (edge).
                theLength Length on the referenced curve. It can be negative.
                theStartPoint Any point can be selected for it, the new edge will begin
                              at the end of theRefCurve, close to the selected point.
                              If None, start from the first point of theRefCurve.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created edge.
            """
            # Example: see GEOM_TestAll.py
            theLength, Parameters = ParseParameters(theLength)
            anObj = self.ShapesOp.MakeEdgeOnCurveByLength(theRefCurve, theLength, theStartPoint)
            RaiseIfFailed("MakeEdgeOnCurveByLength", self.ShapesOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "edge")
            return anObj

        ## Create an edge from specified wire.
        #  @param theWire source Wire
        #  @param theLinearTolerance linear tolerance value (default = 1e-07)
        #  @param theAngularTolerance angular tolerance value (default = 1e-12)
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created edge.
        #
        #  @ref tui_creation_edge "Example"
        @ManageTransactions("ShapesOp")
        def MakeEdgeWire(self, theWire, theLinearTolerance = 1e-07, theAngularTolerance = 1e-12, theName=None):
            """
            Create an edge from specified wire.

            Parameters:
                theWire source Wire
                theLinearTolerance linear tolerance value (default = 1e-07)
                theAngularTolerance angular tolerance value (default = 1e-12)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created edge.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeEdgeWire(theWire, theLinearTolerance, theAngularTolerance)
            RaiseIfFailed("MakeEdgeWire", self.ShapesOp)
            self._autoPublish(anObj, theName, "edge")
            return anObj

        ## Create a wire from the set of edges and wires.
        #  @param theEdgesAndWires List of edges and/or wires.
        #  @param theTolerance Maximum distance between vertices, that will be merged.
        #                      Values less than 1e-07 are equivalent to 1e-07 (Precision::Confusion())
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created wire.
        #
        #  @ref tui_creation_wire "Example"
        @ManageTransactions("ShapesOp")
        def MakeWire(self, theEdgesAndWires, theTolerance = 1e-07, theName=None):
            """
            Create a wire from the set of edges and wires.

            Parameters:
                theEdgesAndWires List of edges and/or wires.
                theTolerance Maximum distance between vertices, that will be merged.
                             Values less than 1e-07 are equivalent to 1e-07 (Precision::Confusion()).
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created wire.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeWire(theEdgesAndWires, theTolerance)
            RaiseIfFailed("MakeWire", self.ShapesOp)
            self._autoPublish(anObj, theName, "wire")
            return anObj

        ## Create a face on the given wire.
        #  @param theWire closed Wire or Edge to build the face on.
        #  @param isPlanarWanted If TRUE, the algorithm tries to build a planar face.
        #                        If the tolerance of the obtained planar face is less
        #                        than 1e-06, this face will be returned, otherwise the
        #                        algorithm tries to build any suitable face on the given
        #                        wire and prints a warning message.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created face (compound of faces).
        #
        #  @ref tui_creation_face "Example"
        @ManageTransactions("ShapesOp")
        def MakeFace(self, theWire, isPlanarWanted, theName=None):
            """
            Create a face on the given wire.

            Parameters:
                theWire closed Wire or Edge to build the face on.
                isPlanarWanted If TRUE, the algorithm tries to build a planar face.
                               If the tolerance of the obtained planar face is less
                               than 1e-06, this face will be returned, otherwise the
                               algorithm tries to build any suitable face on the given
                               wire and prints a warning message.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created face (compound of faces).
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeFace(theWire, isPlanarWanted)
            if isPlanarWanted and anObj is not None and self.ShapesOp.GetErrorCode() == "MAKE_FACE_TOLERANCE_TOO_BIG":
                print "WARNING: Cannot build a planar face: required tolerance is too big. Non-planar face is built."
            else:
                RaiseIfFailed("MakeFace", self.ShapesOp)
            self._autoPublish(anObj, theName, "face")
            return anObj

        ## Create a face on the given wires set.
        #  @param theWires List of closed wires or edges to build the face on.
        #  @param isPlanarWanted If TRUE, the algorithm tries to build a planar face.
        #                        If the tolerance of the obtained planar face is less
        #                        than 1e-06, this face will be returned, otherwise the
        #                        algorithm tries to build any suitable face on the given
        #                        wire and prints a warning message.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created face (compound of faces).
        #
        #  @ref tui_creation_face "Example"
        @ManageTransactions("ShapesOp")
        def MakeFaceWires(self, theWires, isPlanarWanted, theName=None):
            """
            Create a face on the given wires set.

            Parameters:
                theWires List of closed wires or edges to build the face on.
                isPlanarWanted If TRUE, the algorithm tries to build a planar face.
                               If the tolerance of the obtained planar face is less
                               than 1e-06, this face will be returned, otherwise the
                               algorithm tries to build any suitable face on the given
                               wire and prints a warning message.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created face (compound of faces).
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeFaceWires(ToList(theWires), isPlanarWanted)
            if isPlanarWanted and anObj is not None and self.ShapesOp.GetErrorCode() == "MAKE_FACE_TOLERANCE_TOO_BIG":
                print "WARNING: Cannot build a planar face: required tolerance is too big. Non-planar face is built."
            else:
                RaiseIfFailed("MakeFaceWires", self.ShapesOp)
            self._autoPublish(anObj, theName, "face")
            return anObj

        ## See MakeFaceWires() method for details.
        #
        #  @ref tui_creation_face "Example 1"
        #  \n @ref swig_MakeFaces  "Example 2"
        def MakeFaces(self, theWires, isPlanarWanted, theName=None):
            """
            See geompy.MakeFaceWires() method for details.
            """
            # Example: see GEOM_TestOthers.py
            # note: auto-publishing is done in self.MakeFaceWires()
            anObj = self.MakeFaceWires(theWires, isPlanarWanted, theName)
            return anObj

        ## Create a face based on a surface from given face bounded
        #  by given wire.
        #  @param theFace the face whose surface is used to create a new face.
        #  @param theWire the wire that will bound a new face.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_face "Example"
        @ManageTransactions("ShapesOp")
        def MakeFaceFromSurface(self, theFace, theWire, theName=None):
            """
            Create a face based on a surface from given face bounded
            by given wire.

            Parameters:
                theFace the face whose surface is used to create a new face.
                theWire the wire that will bound a new face.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created face.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeFaceFromSurface(theFace, theWire)
            RaiseIfFailed("MakeFaceFromSurface", self.ShapesOp)
            self._autoPublish(anObj, theName, "face")
            return anObj
          
        ## Create a face from a set of edges with the given constraints.
        #  @param theConstraints List of edges and constraint faces (as a sequence of a Edge + Face couples):
        #         - edges should form a closed wire;
        #         - for each edge, constraint face is optional: if a constraint face is missing
        #           for some edge, this means that there no constraint associated with this edge.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        # 
        # @return New GEOM.GEOM_Object, containing the created face.
        # 
        # @ref tui_creation_face "Example"
        @ManageTransactions("ShapesOp")
        def MakeFaceWithConstraints(self, theConstraints, theName=None):
            """
            Create a face from a set of edges with the given constraints.

            Parameters:
                theConstraints List of edges and constraint faces (as a sequence of a Edge + Face couples):
                        - edges should form a closed wire;
                        - for each edge, constraint face is optional: if a constraint face is missing
                          for some edge, this means that there no constraint associated with this edge.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created face.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeFaceWithConstraints(theConstraints)
            if anObj is None:
                RaiseIfFailed("MakeFaceWithConstraints", self.ShapesOp)
            self._autoPublish(anObj, theName, "face")
            return anObj

        ## Create a shell from the set of faces, shells and/or compounds of faces.
        #  @param theFacesAndShells List of faces, shells and/or compounds of faces.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created shell (compound of shells).
        #
        #  @ref tui_creation_shell "Example"
        @ManageTransactions("ShapesOp")
        def MakeShell(self, theFacesAndShells, theName=None):
            """
            Create a shell from the set of faces and shells.

            Parameters:
                theFacesAndShells List of faces and/or shells.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created shell (compound of shells).
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeShell( ToList( theFacesAndShells ))
            RaiseIfFailed("MakeShell", self.ShapesOp)
            self._autoPublish(anObj, theName, "shell")
            return anObj

        ## Create a solid, bounded by the given shells.
        #  @param theShells Sequence of bounding shells.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created solid.
        #
        #  @ref tui_creation_solid "Example"
        @ManageTransactions("ShapesOp")
        def MakeSolid(self, theShells, theName=None):
            """
            Create a solid, bounded by the given shells.

            Parameters:
                theShells Sequence of bounding shells.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created solid.
            """
            # Example: see GEOM_TestAll.py
            theShells = ToList(theShells)
            if len(theShells) == 1:
                descr = self._IsGoodForSolid(theShells[0])
                #if len(descr) > 0:
                #    raise RuntimeError, "MakeSolidShells : " + descr
                if descr == "WRN_SHAPE_UNCLOSED":
                    raise RuntimeError, "MakeSolidShells : Unable to create solid from unclosed shape"
            anObj = self.ShapesOp.MakeSolidShells(theShells)
            RaiseIfFailed("MakeSolidShells", self.ShapesOp)
            self._autoPublish(anObj, theName, "solid")
            return anObj

        ## Create a compound of the given shapes.
        #  @param theShapes List of shapes to put in compound.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created compound.
        #
        #  @ref tui_creation_compound "Example"
        @ManageTransactions("ShapesOp")
        def MakeCompound(self, theShapes, theName=None):
            """
            Create a compound of the given shapes.

            Parameters:
                theShapes List of shapes to put in compound.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created compound.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeCompound(ToList(theShapes))
            RaiseIfFailed("MakeCompound", self.ShapesOp)
            self._autoPublish(anObj, theName, "compound")
            return anObj
        
        ## Create a solid (or solids) from the set of faces and/or shells.
        #  @param theFacesOrShells List of faces and/or shells.
        #  @param isIntersect If TRUE, forces performing intersections
        #         between arguments; otherwise (default) intersection is not performed.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created solid (or compound of solids).
        #
        #  @ref tui_creation_solid_from_faces "Example"
        @ManageTransactions("ShapesOp")
        def MakeSolidFromConnectedFaces(self, theFacesOrShells, isIntersect = False, theName=None):
            """
            Create a solid (or solids) from the set of connected faces and/or shells.

            Parameters:
                theFacesOrShells List of faces and/or shells.
                isIntersect If TRUE, forces performing intersections
                        between arguments; otherwise (default) intersection is not performed
                theName Object name; when specified, this parameter is used.
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created solid (or compound of solids).
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.MakeSolidFromConnectedFaces(theFacesOrShells, isIntersect)
            RaiseIfFailed("MakeSolidFromConnectedFaces", self.ShapesOp)
            self._autoPublish(anObj, theName, "solid")
            return anObj

        # end of l3_basic_go
        ## @}

        ## @addtogroup l2_measure
        ## @{

        ## Gives quantity of faces in the given shape.
        #  @param theShape Shape to count faces of.
        #  @return Quantity of faces.
        #
        #  @ref swig_NumberOf "Example"
        @ManageTransactions("ShapesOp")
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
        @ManageTransactions("ShapesOp")
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
        @ManageTransactions("ShapesOp")
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
        @ManageTransactions("ShapesOp")
        def NumberOfSolids(self, theShape):
            """
            Gives quantity of solids in the given shape.

            Parameters:
                theShape Shape to count solids in.

            Returns:
                Quantity of solids.
            """
            # Example: see GEOM_TestOthers.py
            nb_solids = self.ShapesOp.NumberOfSubShapes(theShape, self.ShapeType["SOLID"])
            RaiseIfFailed("NumberOfSolids", self.ShapesOp)
            return nb_solids

        # end of l2_measure
        ## @}

        ## @addtogroup l3_healing
        ## @{

        ## Reverses an orientation the given shape.
        #  @param theShape Shape to be reversed.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return The reversed copy of theShape.
        #
        #  @ref swig_ChangeOrientation "Example"
        @ManageTransactions("ShapesOp")
        def ChangeOrientation(self, theShape, theName=None):
            """
            Reverses an orientation the given shape.

            Parameters:
                theShape Shape to be reversed.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                The reversed copy of theShape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.ShapesOp.ChangeOrientation(theShape)
            RaiseIfFailed("ChangeOrientation", self.ShapesOp)
            self._autoPublish(anObj, theName, "reversed")
            return anObj

        ## See ChangeOrientation() method for details.
        #
        #  @ref swig_OrientationChange "Example"
        def OrientationChange(self, theShape, theName=None):
            """
            See geompy.ChangeOrientation method for details.
            """
            # Example: see GEOM_TestOthers.py
            # note: auto-publishing is done in self.ChangeOrientation()
            anObj = self.ChangeOrientation(theShape, theName)
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
        #  @ref tui_free_faces_page "Example"
        @ManageTransactions("ShapesOp")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of sub-shapes of theShape1, shared with theShape2.
        #
        #  @ref swig_GetSharedShapes "Example"
        @ManageTransactions("ShapesOp")
        def GetSharedShapes(self, theShape1, theShape2, theShapeType, theName=None):
            """
            Get all sub-shapes of theShape1 of the given type, shared with theShape2.

            Parameters:
                theShape1 Shape to find sub-shapes in.
                theShape2 Shape to find shared sub-shapes with.
                theShapeType Type of sub-shapes to be retrieved.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of sub-shapes of theShape1, shared with theShape2.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetSharedShapes(theShape1, theShape2, theShapeType)
            RaiseIfFailed("GetSharedShapes", self.ShapesOp)
            self._autoPublish(aList, theName, "shared")
            return aList

        ## Get sub-shapes, shared by input shapes.
        #  @param theShapes Either a list or compound of shapes to find common sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType()).
        #  @param theMultiShare Specifies what type of shares should be checked:
        #         - @c True (default): search sub-shapes from 1st input shape shared with all other input shapes;
        #         - @c False: causes to search sub-shapes shared between couples of input shapes.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note If @a theShapes contains single compound, the shares between all possible couples of 
        #        its top-level shapes are returned; otherwise, only shares between 1st input shape
        #        and all rest input shapes are returned.
        #
        #  @return List of all found sub-shapes.
        #
        #  Examples:
        #  - @ref tui_shared_shapes "Example 1"
        #  - @ref swig_GetSharedShapes "Example 2"
        @ManageTransactions("ShapesOp")
        def GetSharedShapesMulti(self, theShapes, theShapeType, theMultiShare=True, theName=None):
            """
            Get sub-shapes, shared by input shapes.

            Parameters:
                theShapes Either a list or compound of shapes to find common sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType).
                theMultiShare Specifies what type of shares should be checked:
                  - True (default): search sub-shapes from 1st input shape shared with all other input shapes;
                  - False: causes to search sub-shapes shared between couples of input shapes.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Note: if theShapes contains single compound, the shares between all possible couples of 
                  its top-level shapes are returned; otherwise, only shares between 1st input shape
                  and all rest input shapes are returned.

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetSharedShapesMulti(ToList(theShapes), theShapeType, theMultiShare)
            RaiseIfFailed("GetSharedShapesMulti", self.ShapesOp)
            self._autoPublish(aList, theName, "shared")
            return aList

        ## Find in <VAR>theShape</VAR> all sub-shapes of type <VAR>theShapeType</VAR>,
        #  situated relatively the specified plane by the certain way,
        #  defined through <VAR>theState</VAR> parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theAx1 Vector (or line, or linear edge), specifying normal
        #                direction and location of the plane to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnPlane "Example"
        @ManageTransactions("ShapesOp")
        def GetShapesOnPlane(self, theShape, theShapeType, theAx1, theState, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnPlane(theShape, theShapeType, theAx1, theState)
            RaiseIfFailed("GetShapesOnPlane", self.ShapesOp)
            self._autoPublish(aList, theName, "shapeOnPlane")
            return aList

        ## Find in <VAR>theShape</VAR> all sub-shapes of type <VAR>theShapeType</VAR>,
        #  situated relatively the specified plane by the certain way,
        #  defined through <VAR>theState</VAR> parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theAx1 Vector (or line, or linear edge), specifying normal
        #                direction and location of the plane to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnPlaneIDs "Example"
        @ManageTransactions("ShapesOp")
        def GetShapesOnPlaneIDs(self, theShape, theShapeType, theAx1, theState):
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnPlaneWithLocation "Example"
        @ManageTransactions("ShapesOp")
        def GetShapesOnPlaneWithLocation(self, theShape, theShapeType, theAx1, thePnt, theState, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnPlaneWithLocation(theShape, theShapeType,
                                                               theAx1, thePnt, theState)
            RaiseIfFailed("GetShapesOnPlaneWithLocation", self.ShapesOp)
            self._autoPublish(aList, theName, "shapeOnPlane")
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
        #
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnPlaneWithLocationIDs "Example"
        @ManageTransactions("ShapesOp")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnCylinder "Example"
        @ManageTransactions("ShapesOp")
        def GetShapesOnCylinder(self, theShape, theShapeType, theAxis, theRadius, theState, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnCylinder(theShape, theShapeType, theAxis, theRadius, theState)
            RaiseIfFailed("GetShapesOnCylinder", self.ShapesOp)
            self._autoPublish(aList, theName, "shapeOnCylinder")
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified cylinder by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theAxis Vector (or line, or linear edge), specifying
        #                 axis of the cylinder to find shapes on.
        #  @param theRadius Radius of the cylinder to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnCylinderIDs "Example"
        @ManageTransactions("ShapesOp")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnCylinderWithLocation "Example"
        @ManageTransactions("ShapesOp")
        def GetShapesOnCylinderWithLocation(self, theShape, theShapeType, theAxis, thePnt, theRadius, theState, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnCylinderWithLocation(theShape, theShapeType, theAxis, thePnt, theRadius, theState)
            RaiseIfFailed("GetShapesOnCylinderWithLocation", self.ShapesOp)
            self._autoPublish(aList, theName, "shapeOnCylinder")
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
        #
        #  @return List of all found sub-shapes indices
        #
        #  @ref swig_GetShapesOnCylinderWithLocationIDs "Example"
        @ManageTransactions("ShapesOp")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnSphere "Example"
        @ManageTransactions("ShapesOp")
        def GetShapesOnSphere(self, theShape, theShapeType, theCenter, theRadius, theState, theName=None):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified sphere by the certain way, defined through theState parameter.

            Parameters:
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theCenter Point, specifying center of the sphere to find shapes on.
                theRadius Radius of the sphere to find shapes on.
                theState The state of the sub-shapes to find (see GEOM::shape_state)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnSphere(theShape, theShapeType, theCenter, theRadius, theState)
            RaiseIfFailed("GetShapesOnSphere", self.ShapesOp)
            self._autoPublish(aList, theName, "shapeOnSphere")
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified sphere by the certain way, defined through \a theState parameter.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theCenter Point, specifying center of the sphere to find shapes on.
        #  @param theRadius Radius of the sphere to find shapes on.
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnSphereIDs "Example"
        @ManageTransactions("ShapesOp")
        def GetShapesOnSphereIDs(self, theShape, theShapeType, theCenter, theRadius, theState):
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnQuadrangle "Example"
        @ManageTransactions("ShapesOp")
        def GetShapesOnQuadrangle(self, theShape, theShapeType,
                                  theTopLeftPoint, theTopRigthPoint,
                                  theBottomLeftPoint, theBottomRigthPoint, theState, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnQuadrangle(theShape, theShapeType,
                                                        theTopLeftPoint, theTopRigthPoint,
                                                        theBottomLeftPoint, theBottomRigthPoint, theState)
            RaiseIfFailed("GetShapesOnQuadrangle", self.ShapesOp)
            self._autoPublish(aList, theName, "shapeOnQuadrangle")
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
        #
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnQuadrangleIDs "Example"
        @ManageTransactions("ShapesOp")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnBox "Example"
        @ManageTransactions("ShapesOp")
        def GetShapesOnBox(self, theBox, theShape, theShapeType, theState, theName=None):
            """
            Find in theShape all sub-shapes of type theShapeType, situated relatively
            the specified theBox by the certain way, defined through theState parameter.

            Parameters:
                theBox Shape for relative comparing.
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theState The state of the sub-shapes to find (see GEOM::shape_state)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnBox(theBox, theShape, theShapeType, theState)
            RaiseIfFailed("GetShapesOnBox", self.ShapesOp)
            self._autoPublish(aList, theName, "shapeOnBox")
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType, situated relatively
        #  the specified \a theBox by the certain way, defined through \a theState parameter.
        #  @param theBox Shape for relative comparing.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnBoxIDs "Example"
        @ManageTransactions("ShapesOp")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of all found sub-shapes.
        #
        #  @ref swig_GetShapesOnShape "Example"
        @ManageTransactions("ShapesOp")
        def GetShapesOnShape(self, theCheckShape, theShape, theShapeType, theState, theName=None):
            """
            Find in theShape all sub-shapes of type theShapeType,
            situated relatively the specified theCheckShape by the
            certain way, defined through theState parameter.

            Parameters:
                theCheckShape Shape for relative comparing. It must be a solid.
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theState The state of the sub-shapes to find (see GEOM::shape_state)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of all found sub-shapes.
            """
            # Example: see GEOM_TestOthers.py
            aList = self.ShapesOp.GetShapesOnShape(theCheckShape, theShape,
                                                   theShapeType, theState)
            RaiseIfFailed("GetShapesOnShape", self.ShapesOp)
            self._autoPublish(aList, theName, "shapeOnShape")
            return aList

        ## Find in \a theShape all sub-shapes of type \a theShapeType,
        #  situated relatively the specified \a theCheckShape by the
        #  certain way, defined through \a theState parameter.
        #  @param theCheckShape Shape for relative comparing. It must be a solid.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return All found sub-shapes as compound.
        #
        #  @ref swig_GetShapesOnShapeAsCompound "Example"
        @ManageTransactions("ShapesOp")
        def GetShapesOnShapeAsCompound(self, theCheckShape, theShape, theShapeType, theState, theName=None):
            """
            Find in theShape all sub-shapes of type theShapeType,
            situated relatively the specified theCheckShape by the
            certain way, defined through theState parameter.

            Parameters:
                theCheckShape Shape for relative comparing. It must be a solid.
                theShape Shape to find sub-shapes of.
                theShapeType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theState The state of the sub-shapes to find (see GEOM::shape_state)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                All found sub-shapes as compound.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.ShapesOp.GetShapesOnShapeAsCompound(theCheckShape, theShape,
                                                             theShapeType, theState)
            RaiseIfFailed("GetShapesOnShapeAsCompound", self.ShapesOp)
            self._autoPublish(anObj, theName, "shapeOnShape")
            return anObj

        ## Find in \a theShape all sub-shapes of type \a theShapeType,
        #  situated relatively the specified \a theCheckShape by the
        #  certain way, defined through \a theState parameter.
        #  @param theCheckShape Shape for relative comparing. It must be a solid.
        #  @param theShape Shape to find sub-shapes of.
        #  @param theShapeType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theState The state of the sub-shapes to find (see GEOM::shape_state)
        #
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetShapesOnShapeIDs "Example"
        @ManageTransactions("ShapesOp")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return Compound which includes all found sub-shapes if they have different types; 
        #          or group of all found shapes of the equal type; or a single found sub-shape.
        #
        #  @note This function has a restriction on argument shapes.
        #        If \a theShapeWhere has curved parts with significantly
        #        outstanding centres (i.e. the mass centre of a part is closer to
        #        \a theShapeWhat than to the part), such parts will not be found.
        #        @image html get_in_place_lost_part.png
        #
        #  @ref swig_GetInPlace "Example"
        @ManageTransactions("ShapesOp")
        def GetInPlace(self, theShapeWhere, theShapeWhat, isNewImplementation = False, theName=None):
            """
            Get sub-shape(s) of theShapeWhere, which are
            coincident with  theShapeWhat or could be a part of it.

            Parameters:
                theShapeWhere Shape to find sub-shapes of.
                theShapeWhat Shape, specifying what to find.
                isNewImplementation Implementation of GetInPlace functionality
                                    (default = False, old alghorithm based on shape properties)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                Compound which includes all found sub-shapes if they have different types; 
                or group of all found shapes of the equal type; or a single found sub-shape.


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
            self._autoPublish(anObj, theName, "inplace")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return Compound which includes all found sub-shapes if they have different types; 
        #          or group of all found shapes of the equal type; or a single found sub-shape.
        #
        #  @ref swig_GetInPlace "Example"
        @ManageTransactions("ShapesOp")
        def GetInPlaceByHistory(self, theShapeWhere, theShapeWhat, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                Compound which includes all found sub-shapes if they have different types; 
                or group of all found shapes of the equal type; or a single found sub-shape.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.ShapesOp.GetInPlaceByHistory(theShapeWhere, theShapeWhat)
            RaiseIfFailed("GetInPlaceByHistory", self.ShapesOp)
            self._autoPublish(anObj, theName, "inplace")
            return anObj

        ## Get sub-shape of theShapeWhere, which is
        #  equal to \a theShapeWhat.
        #  @param theShapeWhere Shape to find sub-shape of.
        #  @param theShapeWhat Shape, specifying what to find.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object for found sub-shape.
        #
        #  @ref swig_GetSame "Example"
        @ManageTransactions("ShapesOp")
        def GetSame(self, theShapeWhere, theShapeWhat, theName=None):
            """
            Get sub-shape of theShapeWhere, which is
            equal to theShapeWhat.

            Parameters:
                theShapeWhere Shape to find sub-shape of.
                theShapeWhat Shape, specifying what to find.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object for found sub-shape.
            """
            anObj = self.ShapesOp.GetSame(theShapeWhere, theShapeWhat)
            RaiseIfFailed("GetSame", self.ShapesOp)
            self._autoPublish(anObj, theName, "sameShape")
            return anObj


        ## Get sub-shape indices of theShapeWhere, which is
        #  equal to \a theShapeWhat.
        #  @param theShapeWhere Shape to find sub-shape of.
        #  @param theShapeWhat Shape, specifying what to find.
        #  @return List of all found sub-shapes indices.
        #
        #  @ref swig_GetSame "Example"
        @ManageTransactions("ShapesOp")
        def GetSameIDs(self, theShapeWhere, theShapeWhat):
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

        ## Resize the input edge with the new Min and Max parameters.
        #  The input edge parameters range is [0, 1]. If theMin parameter is
        #  negative, the input edge is extended, otherwise it is shrinked by
        #  theMin parameter. If theMax is greater than 1, the edge is extended,
        #  otherwise it is shrinked by theMax parameter.
        #  @param theEdge the input edge to be resized.
        #  @param theMin the minimal parameter value.
        #  @param theMax the maximal parameter value.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #  @return New GEOM.GEOM_Object, containing the created edge.
        #
        #  @ref tui_extend "Example"
        @ManageTransactions("ShapesOp")
        def ExtendEdge(self, theEdge, theMin, theMax, theName=None):
            """
            Resize the input edge with the new Min and Max parameters.
            The input edge parameters range is [0, 1]. If theMin parameter is
            negative, the input edge is extended, otherwise it is shrinked by
            theMin parameter. If theMax is greater than 1, the edge is extended,
            otherwise it is shrinked by theMax parameter.

            Parameters:
                theEdge the input edge to be resized.
                theMin the minimal parameter value.
                theMax the maximal parameter value.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created edge.
            """
            theMin, theMax, Parameters = ParseParameters(theMin, theMax)
            anObj = self.ShapesOp.ExtendEdge(theEdge, theMin, theMax)
            RaiseIfFailed("ExtendEdge", self.ShapesOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "edge")
            return anObj

        ## Resize the input face with the new UMin, UMax, VMin and VMax
        #  parameters. The input face U and V parameters range is [0, 1]. If
        #  theUMin parameter is negative, the input face is extended, otherwise
        #  it is shrinked along U direction by theUMin parameter. If theUMax is
        #  greater than 1, the face is extended, otherwise it is shrinked along
        #  U direction by theUMax parameter. So as for theVMin, theVMax and
        #  V direction of the input face.
        #  @param theFace the input face to be resized.
        #  @param theUMin the minimal U parameter value.
        #  @param theUMax the maximal U parameter value.
        #  @param theVMin the minimal V parameter value.
        #  @param theVMax the maximal V parameter value.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_extend "Example"
        @ManageTransactions("ShapesOp")
        def ExtendFace(self, theFace, theUMin, theUMax,
                       theVMin, theVMax, theName=None):
            """
            Resize the input face with the new UMin, UMax, VMin and VMax
            parameters. The input face U and V parameters range is [0, 1]. If
            theUMin parameter is negative, the input face is extended, otherwise
            it is shrinked along U direction by theUMin parameter. If theUMax is
            greater than 1, the face is extended, otherwise it is shrinked along
            U direction by theUMax parameter. So as for theVMin, theVMax and
            V direction of the input face.

            Parameters:
                theFace the input face to be resized.
                theUMin the minimal U parameter value.
                theUMax the maximal U parameter value.
                theVMin the minimal V parameter value.
                theVMax the maximal V parameter value.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created face.
            """
            theUMin, theUMax, theVMin, theVMax, Parameters = ParseParameters(theUMin, theUMax, theVMin, theVMax)
            anObj = self.ShapesOp.ExtendFace(theFace, theUMin, theUMax,
                                             theVMin, theVMax)
            RaiseIfFailed("ExtendFace", self.ShapesOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "face")
            return anObj

        ## This function takes some face as input parameter and creates new
        #  GEOM_Object, i.e. topological shape by extracting underlying surface
        #  of the source face and limiting it by the Umin, Umax, Vmin, Vmax
        #  parameters of the source face (in the parametrical space).
        #  @param theFace the input face.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_creation_surface "Example"
        @ManageTransactions("ShapesOp")
        def MakeSurfaceFromFace(self, theFace, theName=None):
            """
            This function takes some face as input parameter and creates new
            GEOM_Object, i.e. topological shape by extracting underlying surface
            of the source face and limiting it by the Umin, Umax, Vmin, Vmax
            parameters of the source face (in the parametrical space).

            Parameters:
                theFace the input face.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created face.
            """
            anObj = self.ShapesOp.MakeSurfaceFromFace(theFace)
            RaiseIfFailed("MakeSurfaceFromFace", self.ShapesOp)
            self._autoPublish(anObj, theName, "surface")
            return anObj

        # end of l4_obtain
        ## @}

        ## @addtogroup l4_access
        ## @{

        ## Obtain a composite sub-shape of <VAR>aShape</VAR>, composed from sub-shapes
        #  of aShape, selected by their unique IDs inside <VAR>aShape</VAR>
        #  @param aShape Shape to get sub-shape of.
        #  @param ListOfID List of sub-shapes indices.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return Found sub-shape.
        #
        #  @ref swig_all_decompose "Example"
        def GetSubShape(self, aShape, ListOfID, theName=None):
            """
            Obtain a composite sub-shape of aShape, composed from sub-shapes
            of aShape, selected by their unique IDs inside aShape

            Parameters:
                aShape Shape to get sub-shape of.
                ListOfID List of sub-shapes indices.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                Found sub-shape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.AddSubShape(aShape,ListOfID)
            self._autoPublish(anObj, theName, "subshape")
            return anObj

        ## Obtain unique ID of sub-shape <VAR>aSubShape</VAR> inside <VAR>aShape</VAR>
        #  of aShape, selected by their unique IDs inside <VAR>aShape</VAR>
        #  @param aShape Shape to get sub-shape of.
        #  @param aSubShape Sub-shapes of aShape.
        #  @return ID of found sub-shape.
        #
        #  @ref swig_all_decompose "Example"
        @ManageTransactions("LocalOp")
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

        ## Obtain unique IDs of sub-shapes <VAR>aSubShapes</VAR> inside <VAR>aShape</VAR>
        #  This function is provided for performance purpose. The complexity is O(n) with n
        #  the number of subobjects of aShape
        #  @param aShape Shape to get sub-shape of.
        #  @param aSubShapes Sub-shapes of aShape.
        #  @return list of IDs of found sub-shapes.
        #
        #  @ref swig_all_decompose "Example"
        @ManageTransactions("ShapesOp")
        def GetSubShapesIDs(self, aShape, aSubShapes):
            """
            Obtain a list of IDs of sub-shapes aSubShapes inside aShape
            This function is provided for performance purpose. The complexity is O(n) with n
            the number of subobjects of aShape

            Parameters:
               aShape Shape to get sub-shape of.
               aSubShapes Sub-shapes of aShape.

            Returns:
               List of IDs of found sub-shape.
            """
            # Example: see GEOM_TestAll.py
            anIDs = self.ShapesOp.GetSubShapesIndices(aShape, aSubShapes)
            RaiseIfFailed("GetSubShapesIndices", self.ShapesOp)
            return anIDs

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
        @ManageTransactions("ShapesOp")
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
        @ManageTransactions("ShapesOp")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of sub-shapes of type theShapeType, contained in theShape.
        #
        #  @ref swig_all_decompose "Example"
        @ManageTransactions("ShapesOp")
        def SubShapeAll(self, aShape, aType, theName=None):
            """
            Explode a shape on sub-shapes of a given type.
            If the shape itself matches the type, it is also returned.

            Parameters:
                aShape Shape to be exploded.
                aType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of sub-shapes of type theShapeType, contained in theShape.
            """
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.MakeAllSubShapes(aShape, EnumToLong( aType ), False)
            RaiseIfFailed("SubShapeAll", self.ShapesOp)
            self._autoPublish(ListObj, theName, "subshape")
            return ListObj

        ## Explode a shape on sub-shapes of a given type.
        #  @param aShape Shape to be exploded.
        #  @param aType Type of sub-shapes to be retrieved (see ShapeType())
        #  @return List of IDs of sub-shapes.
        #
        #  @ref swig_all_decompose "Example"
        @ManageTransactions("ShapesOp")
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
        #  selected by their indices in list of all sub-shapes of type <VAR>aType</VAR>.
        #  Each index is in range [1, Nb_Sub-Shapes_Of_Given_Type]
        #  @param aShape Shape to get sub-shape of.
        #  @param ListOfInd List of sub-shapes indices.
        #  @param aType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return A compound of sub-shapes of aShape.
        #
        #  @ref swig_all_decompose "Example"
        def SubShape(self, aShape, aType, ListOfInd, theName=None):
            """
            Obtain a compound of sub-shapes of aShape,
            selected by their indices in list of all sub-shapes of type aType.
            Each index is in range [1, Nb_Sub-Shapes_Of_Given_Type]

            Parameters:
                aShape Shape to get sub-shape of.
                ListOfID List of sub-shapes indices.
                aType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                A compound of sub-shapes of aShape.
            """
            # Example: see GEOM_TestAll.py
            ListOfIDs = []
            AllShapeIDsList = self.SubShapeAllIDs(aShape, EnumToLong( aType ))
            for ind in ListOfInd:
                ListOfIDs.append(AllShapeIDsList[ind - 1])
            # note: auto-publishing is done in self.GetSubShape()
            anObj = self.GetSubShape(aShape, ListOfIDs, theName)
            return anObj

        ## Explode a shape on sub-shapes of a given type.
        #  Sub-shapes will be sorted taking into account their gravity centers,
        #  to provide stable order of sub-shapes. Please see
        #  @ref sorting_shapes_page "Description of Sorting Shapes Algorithm".
        #  If the shape itself matches the type, it is also returned.
        #  @param aShape Shape to be exploded.
        #  @param aType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of sub-shapes of type theShapeType, contained in theShape.
        #
        #  @ref swig_SubShapeAllSorted "Example"
        @ManageTransactions("ShapesOp")
        def SubShapeAllSortedCentres(self, aShape, aType, theName=None):
            """
            Explode a shape on sub-shapes of a given type.
            Sub-shapes will be sorted taking into account their gravity centers,
            to provide stable order of sub-shapes.
            If the shape itself matches the type, it is also returned.

            Parameters:
                aShape Shape to be exploded.
                aType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of sub-shapes of type theShapeType, contained in theShape.
            """
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.MakeAllSubShapes(aShape, EnumToLong( aType ), True)
            RaiseIfFailed("SubShapeAllSortedCentres", self.ShapesOp)
            self._autoPublish(ListObj, theName, "subshape")
            return ListObj

        ## Explode a shape on sub-shapes of a given type.
        #  Sub-shapes will be sorted taking into account their gravity centers,
        #  to provide stable order of sub-shapes. Please see
        #  @ref sorting_shapes_page "Description of Sorting Shapes Algorithm".
        #  @param aShape Shape to be exploded.
        #  @param aType Type of sub-shapes to be retrieved (see ShapeType())
        #  @return List of IDs of sub-shapes.
        #
        #  @ref swig_all_decompose "Example"
        @ManageTransactions("ShapesOp")
        def SubShapeAllSortedCentresIDs(self, aShape, aType):
            """
            Explode a shape on sub-shapes of a given type.
            Sub-shapes will be sorted taking into account their gravity centers,
            to provide stable order of sub-shapes.

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
        #  Please see @ref sorting_shapes_page "Description of Sorting Shapes Algorithm".
        #  Each index is in range [1, Nb_Sub-Shapes_Of_Given_Type]
        #  @param aShape Shape to get sub-shape of.
        #  @param ListOfInd List of sub-shapes indices.
        #  @param aType Type of sub-shapes to be retrieved (see ShapeType())
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return A compound of sub-shapes of aShape.
        #
        #  @ref swig_all_decompose "Example"
        def SubShapeSortedCentres(self, aShape, aType, ListOfInd, theName=None):
            """
            Obtain a compound of sub-shapes of aShape,
            selected by they indices in sorted list of all sub-shapes of type aType.
            Each index is in range [1, Nb_Sub-Shapes_Of_Given_Type]

            Parameters:
                aShape Shape to get sub-shape of.
                ListOfID List of sub-shapes indices.
                aType Type of sub-shapes to be retrieved (see geompy.ShapeType)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                A compound of sub-shapes of aShape.
            """
            # Example: see GEOM_TestAll.py
            ListOfIDs = []
            AllShapeIDsList = self.SubShapeAllSortedCentresIDs(aShape, EnumToLong( aType ))
            for ind in ListOfInd:
                ListOfIDs.append(AllShapeIDsList[ind - 1])
            # note: auto-publishing is done in self.GetSubShape()
            anObj = self.GetSubShape(aShape, ListOfIDs, theName)
            return anObj

        ## Extract shapes (excluding the main shape) of given type.
        #  @param aShape The shape.
        #  @param aType  The shape type (see ShapeType())
        #  @param isSorted Boolean flag to switch sorting on/off. Please see
        #         @ref sorting_shapes_page "Description of Sorting Shapes Algorithm".
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of sub-shapes of type aType, contained in aShape.
        #
        #  @ref swig_FilletChamfer "Example"
        @ManageTransactions("ShapesOp")
        def ExtractShapes(self, aShape, aType, isSorted = False, theName=None):
            """
            Extract shapes (excluding the main shape) of given type.

            Parameters:
                aShape The shape.
                aType  The shape type (see geompy.ShapeType)
                isSorted Boolean flag to switch sorting on/off.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of sub-shapes of type aType, contained in aShape.
            """
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.ExtractSubShapes(aShape, EnumToLong( aType ), isSorted)
            RaiseIfFailed("ExtractSubShapes", self.ShapesOp)
            self._autoPublish(ListObj, theName, "subshape")
            return ListObj

        ## Get a set of sub-shapes defined by their unique IDs inside <VAR>aShape</VAR>
        #  @param aShape Main shape.
        #  @param anIDs List of unique IDs of sub-shapes inside <VAR>aShape</VAR>.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #  @return List of GEOM.GEOM_Object, corresponding to found sub-shapes.
        #
        #  @ref swig_all_decompose "Example"
        @ManageTransactions("ShapesOp")
        def SubShapes(self, aShape, anIDs, theName=None):
            """
            Get a set of sub-shapes defined by their unique IDs inside theMainShape

            Parameters:
                aShape Main shape.
                anIDs List of unique IDs of sub-shapes inside theMainShape.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of GEOM.GEOM_Object, corresponding to found sub-shapes.
            """
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.MakeSubShapes(aShape, anIDs)
            RaiseIfFailed("SubShapes", self.ShapesOp)
            self._autoPublish(ListObj, theName, "subshape")
            return ListObj

        ## Explode a shape into edges sorted in a row from a starting point.
        #  @param theShape the shape to be exploded on edges.
        #  @param theStartPoint the starting point.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #  @return List of GEOM.GEOM_Object that is actually an ordered list
        #          of edges sorted in a row from a starting point.
        #
        #  @ref swig_GetSubShapeEdgeSorted "Example"
        @ManageTransactions("ShapesOp")
        def GetSubShapeEdgeSorted(self, theShape, theStartPoint, theName=None):
            """
            Explode a shape into edges sorted in a row from a starting point.

            Parameters:
                theShape the shape to be exploded on edges.
                theStartPoint the starting point.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of GEOM.GEOM_Object that is actually an ordered list
                of edges sorted in a row from a starting point.
            """
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.GetSubShapeEdgeSorted(theShape, theStartPoint)
            RaiseIfFailed("GetSubShapeEdgeSorted", self.ShapesOp)
            self._autoPublish(ListObj, theName, "SortedEdges")
            return ListObj

        ##
        # Return the list of subshapes that satisfies a certain tolerance
        # criterion. The user defines the type of shapes to be returned, the
        # condition and the tolerance value. The operation is defined for
        # faces, edges and vertices only. E.g. for theShapeType FACE,
        # theCondition GEOM::CC_GT and theTolerance 1.e-7 this method returns
        # all faces of theShape that have tolerances greater then 1.e7.
        #
        #  @param theShape the shape to be exploded
        #  @param theShapeType the type of sub-shapes to be returned (see
        #         ShapeType()). Can have the values FACE, EDGE and VERTEX only.
        #  @param theCondition the condition type (see GEOM::comparison_condition).
        #  @param theTolerance the tolerance filter.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #  @return the list of shapes that satisfy the conditions.
        #
        #  @ref swig_GetSubShapesWithTolerance "Example"
        @ManageTransactions("ShapesOp")
        def GetSubShapesWithTolerance(self, theShape, theShapeType,
                                      theCondition, theTolerance, theName=None):
            """
            Return the list of subshapes that satisfies a certain tolerance
            criterion. The user defines the type of shapes to be returned, the
            condition and the tolerance value. The operation is defined for
            faces, edges and vertices only. E.g. for theShapeType FACE,
            theCondition GEOM::CC_GT and theTolerance 1.e-7 this method returns
            all faces of theShape that have tolerances greater then 1.e7.
            
            Parameters:
                theShape the shape to be exploded
                theShapeType the type of sub-shapes to be returned (see
                             ShapeType()). Can have the values FACE,
                             EDGE and VERTEX only.
                theCondition the condition type (see GEOM::comparison_condition).
                theTolerance the tolerance filter.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                The list of shapes that satisfy the conditions.
            """
            # Example: see GEOM_TestAll.py
            ListObj = self.ShapesOp.GetSubShapesWithTolerance(theShape, EnumToLong(theShapeType),
                                                              theCondition, theTolerance)
            RaiseIfFailed("GetSubShapesWithTolerance", self.ShapesOp)
            self._autoPublish(ListObj, theName, "SubShapeWithTolerance")
            return ListObj

        ## Check if the object is a sub-object of another GEOM object.
        #  @param aSubObject Checked sub-object (or its parent object, in case if
        #         \a theSubObjectIndex is non-zero).
        #  @param anObject An object that is checked for ownership (or its parent object,
        #         in case if \a theObjectIndex is non-zero).
        #  @param aSubObjectIndex When non-zero, specifies a sub-shape index that
        #         identifies a sub-object within its parent specified via \a theSubObject.
        #  @param anObjectIndex When non-zero, specifies a sub-shape index that
        #         identifies an object within its parent specified via \a theObject.
        #  @return TRUE, if the given object contains sub-object.
        @ManageTransactions("ShapesOp")
        def IsSubShapeBelongsTo(self, aSubObject, anObject, aSubObjectIndex = 0, anObjectIndex = 0):
            """
            Check if the object is a sub-object of another GEOM object.
            
            Parameters:
                aSubObject Checked sub-object (or its parent object, in case if
                    \a theSubObjectIndex is non-zero).
                anObject An object that is checked for ownership (or its parent object,
                    in case if \a theObjectIndex is non-zero).
                aSubObjectIndex When non-zero, specifies a sub-shape index that
                    identifies a sub-object within its parent specified via \a theSubObject.
                anObjectIndex When non-zero, specifies a sub-shape index that
                    identifies an object within its parent specified via \a theObject.

            Returns
                TRUE, if the given object contains sub-object.
            """
            IsOk = self.ShapesOp.IsSubShapeBelongsTo(aSubObject, aSubObjectIndex, anObject, anObjectIndex)
            RaiseIfFailed("IsSubShapeBelongsTo", self.ShapesOp)
            return IsOk

        ## Perform extraction of sub-shapes from the main shape.
        #
        #  @param theShape the main shape
        #  @param theListOfID the list of sub-shape IDs to be extracted from
        #         the main shape.
        #  @return New GEOM.GEOM_Object, containing the shape without
        #          extracted sub-shapes.
        #
        #  @ref swig_MakeExtraction "Example"
        @ManageTransactions("ShapesOp")
        def MakeExtraction(self, theShape, theListOfID, theName=None):
            """
            Perform extraction of sub-shapes from the main shape.

            Parameters:
                theShape the main shape
                theListOfID the list of sub-shape IDs to be extracted from
                            the main shape.

            Returns
                New GEOM.GEOM_Object, containing the shape without
                extracted sub-shapes.
            """
            # Example: see GEOM_TestAll.py
            (anObj, aStat) = self.ShapesOp.MakeExtraction(theShape, theListOfID)
            RaiseIfFailed("MakeExtraction", self.ShapesOp)
            self._autoPublish(anObj, theName, "Extraction")
            return anObj

        # end of l4_decompose
        ## @}

        ## @addtogroup l4_decompose_d
        ## @{

        ## Deprecated method
        #  It works like SubShapeAllSortedCentres(), but wrongly
        #  defines centres of faces, shells and solids.
        @ManageTransactions("ShapesOp")
        def SubShapeAllSorted(self, aShape, aType, theName=None):
            """
            Deprecated method
            It works like geompy.SubShapeAllSortedCentres, but wrongly
            defines centres of faces, shells and solids.
            """
            ListObj = self.ShapesOp.MakeExplode(aShape, EnumToLong( aType ), True)
            RaiseIfFailed("MakeExplode", self.ShapesOp)
            self._autoPublish(ListObj, theName, "subshape")
            return ListObj

        ## Deprecated method
        #  It works like SubShapeAllSortedCentresIDs(), but wrongly
        #  defines centres of faces, shells and solids.
        @ManageTransactions("ShapesOp")
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
        def SubShapeSorted(self, aShape, aType, ListOfInd, theName=None):
            """
            Deprecated method
            It works like geompy.SubShapeSortedCentres, but has a bug
            (wrongly defines centres of faces, shells and solids).
            """
            ListOfIDs = []
            AllShapeIDsList = self.SubShapeAllSortedIDs(aShape, EnumToLong( aType ))
            for ind in ListOfInd:
                ListOfIDs.append(AllShapeIDsList[ind - 1])
            # note: auto-publishing is done in self.GetSubShape()
            anObj = self.GetSubShape(aShape, ListOfIDs, theName)
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
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
        #  - \b DropSmallSolids - either removes small solids or merges them with neighboring ones. \n
        #  - \b DropSmallSolids.WidthFactorThreshold - defines maximum value of <em>2V/S</em> of a solid which is considered small, where \a V is volume and \a S is surface area of the solid. \n
        #  - \b DropSmallSolids.VolumeThreshold - defines maximum volume of a solid which is considered small. If the both tolerances are privided a solid is considered small if it meets the both criteria. \n
        #  - \b DropSmallSolids.MergeSolids - if "1", small solids are removed; if "0" small solids are merged to adjacent non-small solids or left untouched if cannot be merged. \n
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
        @ManageTransactions("HealOp")
        def ProcessShape(self, theShape, theOperators, theParameters, theValues, theName=None):
            """
            Apply a sequence of Shape Healing operators to the given object.

            Parameters:
                theShape Shape to be processed.
                theValues List of values of parameters, in the same order
                          as parameters are listed in theParameters list.
                theOperators List of names of operators ('FixShape', 'SplitClosedFaces', etc.).
                theParameters List of names of parameters
                              ('FixShape.Tolerance3d', 'SplitClosedFaces.NbSplitPoints', etc.).
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

                Operators and Parameters:

                 * FixShape - corrects invalid shapes.
                     * FixShape.Tolerance3d - work tolerance for detection of the problems and correction of them.
                     * FixShape.MaxTolerance3d - maximal possible tolerance of the shape after correction.
                 * FixFaceSize - removes small faces, such as spots and strips.
                     * FixFaceSize.Tolerance - defines minimum possible face size.
                 * DropSmallEdges - removes edges, which merge with neighbouring edges.
                     * DropSmallEdges.Tolerance3d - defines minimum possible distance between two parallel edges.
                 * DropSmallSolids - either removes small solids or merges them with neighboring ones.
                     * DropSmallSolids.WidthFactorThreshold - defines maximum value of 2V/S of a solid which is considered small, where V is volume and S is surface area of the solid.
                     * DropSmallSolids.VolumeThreshold - defines maximum volume of a solid which is considered small. If the both tolerances are privided a solid is considered small if it meets the both criteria.
                     * DropSmallSolids.MergeSolids - if '1', small solids are removed; if '0' small solids are merged to adjacent non-small solids or left untouched if cannot be merged.

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
            self._autoPublish(anObj, theName, "healed")
            return anObj

        ## Remove faces from the given object (shape).
        #  @param theObject Shape to be processed.
        #  @param theFaces Indices of faces to be removed, if EMPTY then the method
        #                  removes ALL faces of the given object.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_suppress_faces "Example"
        @ManageTransactions("HealOp")
        def SuppressFaces(self, theObject, theFaces, theName=None):
            """
            Remove faces from the given object (shape).

            Parameters:
                theObject Shape to be processed.
                theFaces Indices of faces to be removed, if EMPTY then the method
                         removes ALL faces of the given object.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.SuppressFaces(theObject, theFaces)
            RaiseIfFailed("SuppressFaces", self.HealOp)
            self._autoPublish(anObj, theName, "suppressFaces")
            return anObj

        ## Sewing of faces into a single shell.
        #  @param ListShape Shapes to be processed.
        #  @param theTolerance Required tolerance value.
        #  @param AllowNonManifold Flag that allows non-manifold sewing.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing a result shell.
        #
        #  @ref tui_sewing "Example"
        def MakeSewing(self, ListShape, theTolerance, AllowNonManifold=False, theName=None):
            """
            Sewing of faces into a single shell.

            Parameters:
                ListShape Shapes to be processed.
                theTolerance Required tolerance value.
                AllowNonManifold Flag that allows non-manifold sewing.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing containing a result shell.
            """
            # Example: see GEOM_TestHealing.py
            # note: auto-publishing is done in self.Sew()
            anObj = self.Sew(ListShape, theTolerance, AllowNonManifold, theName)
            return anObj

        ## Sewing of faces into a single shell.
        #  @param ListShape Shapes to be processed.
        #  @param theTolerance Required tolerance value.
        #  @param AllowNonManifold Flag that allows non-manifold sewing.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing a result shell.
        @ManageTransactions("HealOp")
        def Sew(self, ListShape, theTolerance, AllowNonManifold=False, theName=None):
            """
            Sewing of faces into a single shell.

            Parameters:
                ListShape Shapes to be processed.
                theTolerance Required tolerance value.
                AllowNonManifold Flag that allows non-manifold sewing.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing a result shell.
            """
            # Example: see MakeSewing() above
            theTolerance,Parameters = ParseParameters(theTolerance)
            if AllowNonManifold:
                anObj = self.HealOp.SewAllowNonManifold( ToList( ListShape ), theTolerance)
            else:
                anObj = self.HealOp.Sew( ToList( ListShape ), theTolerance)
            # To avoid script failure in case of good argument shape
            # (Fix of test cases geom/bugs11/L7,L8)
            if self.HealOp.GetErrorCode() == "ShHealOper_NotError_msg":
                return anObj
            RaiseIfFailed("Sew", self.HealOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "sewed")
            return anObj

        ## Rebuild the topology of theSolids by removing
        #  the faces that are shared by several solids.
        #  @param theSolids A compound or a list of solids to be processed.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_remove_webs "Example"
        @ManageTransactions("HealOp")
        def RemoveInternalFaces (self, theSolids, theName=None):
            """
            Rebuild the topology of theSolids by removing
            the faces that are shared by several solids.

            Parameters:
                theSolids A compound or a list of solids to be processed.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.RemoveInternalFaces(ToList(theSolids))
            RaiseIfFailed("RemoveInternalFaces", self.HealOp)
            self._autoPublish(anObj, theName, "removeWebs")
            return anObj

        ## Remove internal wires and edges from the given object (face).
        #  @param theObject Shape to be processed.
        #  @param theWires Indices of wires to be removed, if EMPTY then the method
        #                  removes ALL internal wires of the given object.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_suppress_internal_wires "Example"
        @ManageTransactions("HealOp")
        def SuppressInternalWires(self, theObject, theWires, theName=None):
            """
            Remove internal wires and edges from the given object (face).

            Parameters:
                theObject Shape to be processed.
                theWires Indices of wires to be removed, if EMPTY then the method
                         removes ALL internal wires of the given object.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.RemoveIntWires(theObject, theWires)
            RaiseIfFailed("RemoveIntWires", self.HealOp)
            self._autoPublish(anObj, theName, "suppressWires")
            return anObj

        ## Remove internal closed contours (holes) from the given object.
        #  @param theObject Shape to be processed.
        #  @param theWires Indices of wires to be removed, if EMPTY then the method
        #                  removes ALL internal holes of the given object
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_suppress_holes "Example"
        @ManageTransactions("HealOp")
        def SuppressHoles(self, theObject, theWires, theName=None):
            """
            Remove internal closed contours (holes) from the given object.

            Parameters:
                theObject Shape to be processed.
                theWires Indices of wires to be removed, if EMPTY then the method
                         removes ALL internal holes of the given object
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.FillHoles(theObject, theWires)
            RaiseIfFailed("FillHoles", self.HealOp)
            self._autoPublish(anObj, theName, "suppressHoles")
            return anObj

        ## Close an open wire.
        #  @param theObject Shape to be processed.
        #  @param theWires Indexes of edge(s) and wire(s) to be closed within <VAR>theObject</VAR>'s shape,
        #                  if [ ], then <VAR>theObject</VAR> itself is a wire.
        #  @param isCommonVertex If True  : closure by creation of a common vertex,
        #                        If False : closure by creation of an edge between ends.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_close_contour "Example"
        @ManageTransactions("HealOp")
        def CloseContour(self,theObject, theWires, isCommonVertex, theName=None):
            """
            Close an open wire.

            Parameters:
                theObject Shape to be processed.
                theWires Indexes of edge(s) and wire(s) to be closed within theObject's shape,
                         if [ ], then theObject itself is a wire.
                isCommonVertex If True  : closure by creation of a common vertex,
                               If False : closure by creation of an edge between ends.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.CloseContour(theObject, theWires, isCommonVertex)
            RaiseIfFailed("CloseContour", self.HealOp)
            self._autoPublish(anObj, theName, "closeContour")
            return anObj

        ## Addition of a point to a given edge object.
        #  @param theObject Shape to be processed.
        #  @param theEdgeIndex Index of edge to be divided within theObject's shape,
        #                      if -1, then theObject itself is the edge.
        #  @param theValue Value of parameter on edge or length parameter,
        #                  depending on \a isByParameter.
        #  @param isByParameter If TRUE : \a theValue is treated as a curve parameter [0..1], \n
        #                       if FALSE : \a theValue is treated as a length parameter [0..1]
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_add_point_on_edge "Example"
        @ManageTransactions("HealOp")
        def DivideEdge(self, theObject, theEdgeIndex, theValue, isByParameter, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            theEdgeIndex,theValue,isByParameter,Parameters = ParseParameters(theEdgeIndex,theValue,isByParameter)
            anObj = self.HealOp.DivideEdge(theObject, theEdgeIndex, theValue, isByParameter)
            RaiseIfFailed("DivideEdge", self.HealOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "divideEdge")
            return anObj

        ## Addition of points to a given edge of \a theObject by projecting
        #  other points to the given edge.
        #  @param theObject Shape to be processed.
        #  @param theEdgeIndex Index of edge to be divided within theObject's shape,
        #                      if -1, then theObject itself is the edge.
        #  @param thePoints List of points to project to theEdgeIndex-th edge.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_add_point_on_edge "Example"
        @ManageTransactions("HealOp")
        def DivideEdgeByPoint(self, theObject, theEdgeIndex, thePoints, theName=None):
            """
            Addition of points to a given edge of \a theObject by projecting
            other points to the given edge.

            Parameters:
                theObject Shape to be processed.
                theEdgeIndex The edge or its index to be divided within theObject's shape,
                             if -1, then theObject itself is the edge.
                thePoints List of points to project to theEdgeIndex-th edge.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
            # Example: see GEOM_TestHealing.py
            if isinstance( theEdgeIndex, GEOM._objref_GEOM_Object ):
                theEdgeIndex = self.GetSubShapeID( theObject, theEdgeIndex )
            anObj = self.HealOp.DivideEdgeByPoint(theObject, theEdgeIndex, ToList( thePoints ))
            RaiseIfFailed("DivideEdgeByPoint", self.HealOp)
            self._autoPublish(anObj, theName, "divideEdge")
            return anObj

        ## Suppress the vertices in the wire in case if adjacent edges are C1 continuous.
        #  @param theWire Wire to minimize the number of C1 continuous edges in.
        #  @param theVertices A list of vertices to suppress. If the list
        #                     is empty, all vertices in a wire will be assumed.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object with modified wire.
        #
        #  @ref tui_fuse_collinear_edges "Example"
        @ManageTransactions("HealOp")
        def FuseCollinearEdgesWithinWire(self, theWire, theVertices = [], theName=None):
            """
            Suppress the vertices in the wire in case if adjacent edges are C1 continuous.

            Parameters:
                theWire Wire to minimize the number of C1 continuous edges in.
                theVertices A list of vertices to suppress. If the list
                            is empty, all vertices in a wire will be assumed.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object with modified wire.
            """
            anObj = self.HealOp.FuseCollinearEdgesWithinWire(theWire, theVertices)
            RaiseIfFailed("FuseCollinearEdgesWithinWire", self.HealOp)
            self._autoPublish(anObj, theName, "fuseEdges")
            return anObj

        ## Change orientation of the given object. Updates given shape.
        #  @param theObject Shape to be processed.
        #  @return Updated <var>theObject</var>
        #
        #  @ref swig_todo "Example"
        @ManageTransactions("HealOp")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref swig_todo "Example"
        @ManageTransactions("HealOp")
        def ChangeOrientationShellCopy(self, theObject, theName=None):
            """
            Change orientation of the given object.

            Parameters:
                theObject Shape to be processed.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
            anObj = self.HealOp.ChangeOrientationCopy(theObject)
            RaiseIfFailed("ChangeOrientationCopy", self.HealOp)
            self._autoPublish(anObj, theName, "reversed")
            return anObj

        ## Try to limit tolerance of the given object by value \a theTolerance.
        #  @param theObject Shape to be processed.
        #  @param theTolerance Required tolerance value.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing processed shape.
        #
        #  @ref tui_limit_tolerance "Example"
        @ManageTransactions("HealOp")
        def LimitTolerance(self, theObject, theTolerance = 1e-07, theName=None):
            """
            Try to limit tolerance of the given object by value theTolerance.

            Parameters:
                theObject Shape to be processed.
                theTolerance Required tolerance value.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing processed shape.
            """
            anObj = self.HealOp.LimitTolerance(theObject, theTolerance)
            RaiseIfFailed("LimitTolerance", self.HealOp)
            self._autoPublish(anObj, theName, "limitTolerance")
            return anObj

        ## Get a list of wires (wrapped in GEOM.GEOM_Object-s),
        #  that constitute a free boundary of the given shape.
        #  @param theObject Shape to get free boundary of.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return [\a status, \a theClosedWires, \a theOpenWires]
        #  \n \a status: FALSE, if an error(s) occured during the method execution.
        #  \n \a theClosedWires: Closed wires on the free boundary of the given shape.
        #  \n \a theOpenWires: Open wires on the free boundary of the given shape.
        #
        #  @ref tui_free_boundaries_page "Example"
        @ManageTransactions("HealOp")
        def GetFreeBoundary(self, theObject, theName=None):
            """
            Get a list of wires (wrapped in GEOM.GEOM_Object-s),
            that constitute a free boundary of the given shape.

            Parameters:
                theObject Shape to get free boundary of.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                [status, theClosedWires, theOpenWires]
                 status: FALSE, if an error(s) occured during the method execution.
                 theClosedWires: Closed wires on the free boundary of the given shape.
                 theOpenWires: Open wires on the free boundary of the given shape.
            """
            # Example: see GEOM_TestHealing.py
            anObj = self.HealOp.GetFreeBoundary( ToList( theObject ))
            RaiseIfFailed("GetFreeBoundary", self.HealOp)
            self._autoPublish(anObj[1], theName, "closedWire")
            self._autoPublish(anObj[2], theName, "openWire")
            return anObj

        ## Replace coincident faces in \a theShapes by one face.
        #  @param theShapes Initial shapes, either a list or compound of shapes.
        #  @param theTolerance Maximum distance between faces, which can be considered as coincident.
        #  @param doKeepNonSolids If FALSE, only solids will present in the result,
        #                         otherwise all initial shapes.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing copies of theShapes without coincident faces.
        #
        #  @ref tui_glue_faces "Example"
        @ManageTransactions("ShapesOp")
        def MakeGlueFaces(self, theShapes, theTolerance, doKeepNonSolids=True, theName=None):
            """
            Replace coincident faces in theShapes by one face.

            Parameters:
                theShapes Initial shapes, either a list or compound of shapes.
                theTolerance Maximum distance between faces, which can be considered as coincident.
                doKeepNonSolids If FALSE, only solids will present in the result,
                                otherwise all initial shapes.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing copies of theShapes without coincident faces.
            """
            # Example: see GEOM_Spanner.py
            theTolerance,Parameters = ParseParameters(theTolerance)
            anObj = self.ShapesOp.MakeGlueFaces(ToList(theShapes), theTolerance, doKeepNonSolids)
            if anObj is None:
                raise RuntimeError, "MakeGlueFaces : " + self.ShapesOp.GetErrorCode()
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "glueFaces")
            return anObj

        ## Find coincident faces in \a theShapes for possible gluing.
        #  @param theShapes Initial shapes, either a list or compound of shapes.
        #  @param theTolerance Maximum distance between faces,
        #                      which can be considered as coincident.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return GEOM.ListOfGO
        #
        #  @ref tui_glue_faces "Example"
        @ManageTransactions("ShapesOp")
        def GetGlueFaces(self, theShapes, theTolerance, theName=None):
            """
            Find coincident faces in theShapes for possible gluing.

            Parameters:
                theShapes Initial shapes, either a list or compound of shapes.
                theTolerance Maximum distance between faces,
                             which can be considered as coincident.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                GEOM.ListOfGO
            """
            anObj = self.ShapesOp.GetGlueFaces(ToList(theShapes), theTolerance)
            RaiseIfFailed("GetGlueFaces", self.ShapesOp)
            self._autoPublish(anObj, theName, "facesToGlue")
            return anObj

        ## Replace coincident faces in \a theShapes by one face
        #  in compliance with given list of faces
        #  @param theShapes Initial shapes, either a list or compound of shapes.
        #  @param theTolerance Maximum distance between faces,
        #                      which can be considered as coincident.
        #  @param theFaces List of faces for gluing.
        #  @param doKeepNonSolids If FALSE, only solids will present in the result,
        #                         otherwise all initial shapes.
        #  @param doGlueAllEdges If TRUE, all coincident edges of <VAR>theShape</VAR>
        #                        will be glued, otherwise only the edges,
        #                        belonging to <VAR>theFaces</VAR>.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing copies of theShapes without coincident faces.
        #
        #  @ref tui_glue_faces "Example"
        @ManageTransactions("ShapesOp")
        def MakeGlueFacesByList(self, theShapes, theTolerance, theFaces,
                                doKeepNonSolids=True, doGlueAllEdges=True, theName=None):
            """
            Replace coincident faces in theShapes by one face
            in compliance with given list of faces

            Parameters:
                theShapes theShapes Initial shapes, either a list or compound of shapes.
                theTolerance Maximum distance between faces,
                             which can be considered as coincident.
                theFaces List of faces for gluing.
                doKeepNonSolids If FALSE, only solids will present in the result,
                                otherwise all initial shapes.
                doGlueAllEdges If TRUE, all coincident edges of theShape
                               will be glued, otherwise only the edges,
                               belonging to theFaces.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing copies of theShapes without coincident faces.
            """
            anObj = self.ShapesOp.MakeGlueFacesByList(ToList(theShapes), theTolerance, ToList(theFaces),
                                                      doKeepNonSolids, doGlueAllEdges)
            if anObj is None:
                raise RuntimeError, "MakeGlueFacesByList : " + self.ShapesOp.GetErrorCode()
            self._autoPublish(anObj, theName, "glueFaces")
            return anObj

        ## Replace coincident edges in \a theShapes by one edge.
        #  @param theShapes Initial shapes, either a list or compound of shapes.
        #  @param theTolerance Maximum distance between edges, which can be considered as coincident.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing copies of theShapes without coincident edges.
        #
        #  @ref tui_glue_edges "Example"
        @ManageTransactions("ShapesOp")
        def MakeGlueEdges(self, theShapes, theTolerance, theName=None):
            """
            Replace coincident edges in theShapes by one edge.

            Parameters:
                theShapes Initial shapes, either a list or compound of shapes.
                theTolerance Maximum distance between edges, which can be considered as coincident.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing copies of theShapes without coincident edges.
            """
            theTolerance,Parameters = ParseParameters(theTolerance)
            anObj = self.ShapesOp.MakeGlueEdges(ToList(theShapes), theTolerance)
            if anObj is None:
                raise RuntimeError, "MakeGlueEdges : " + self.ShapesOp.GetErrorCode()
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "glueEdges")
            return anObj

        ## Find coincident edges in \a theShapes for possible gluing.
        #  @param theShapes Initial shapes, either a list or compound of shapes.
        #  @param theTolerance Maximum distance between edges,
        #                      which can be considered as coincident.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return GEOM.ListOfGO
        #
        #  @ref tui_glue_edges "Example"
        @ManageTransactions("ShapesOp")
        def GetGlueEdges(self, theShapes, theTolerance, theName=None):
            """
            Find coincident edges in theShapes for possible gluing.

            Parameters:
                theShapes Initial shapes, either a list or compound of shapes.
                theTolerance Maximum distance between edges,
                             which can be considered as coincident.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                GEOM.ListOfGO
            """
            anObj = self.ShapesOp.GetGlueEdges(ToList(theShapes), theTolerance)
            RaiseIfFailed("GetGlueEdges", self.ShapesOp)
            self._autoPublish(anObj, theName, "edgesToGlue")
            return anObj

        ## Replace coincident edges in theShapes by one edge
        #  in compliance with given list of edges.
        #  @param theShapes Initial shapes, either a list or compound of shapes.
        #  @param theTolerance Maximum distance between edges,
        #                      which can be considered as coincident.
        #  @param theEdges List of edges for gluing.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing copies of theShapes without coincident edges.
        #
        #  @ref tui_glue_edges "Example"
        @ManageTransactions("ShapesOp")
        def MakeGlueEdgesByList(self, theShapes, theTolerance, theEdges, theName=None):
            """
            Replace coincident edges in theShapes by one edge
            in compliance with given list of edges.

            Parameters:
                theShapes Initial shapes, either a list or compound of shapes.
                theTolerance Maximum distance between edges,
                             which can be considered as coincident.
                theEdges List of edges for gluing.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing copies of theShapes without coincident edges.
            """
            anObj = self.ShapesOp.MakeGlueEdgesByList(ToList(theShapes), theTolerance, theEdges)
            if anObj is None:
                raise RuntimeError, "MakeGlueEdgesByList : " + self.ShapesOp.GetErrorCode()
            self._autoPublish(anObj, theName, "glueEdges")
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
        #  @param checkSelfInte The flag that tells if the arguments should
        #         be checked for self-intersection prior to the operation.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note This algorithm doesn't find all types of self-intersections.
        #        It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
        #        vertex/face and edge/face intersections. Face/face
        #        intersections detection is switched off as it is a
        #        time-consuming operation that gives an impact on performance.
        #        To find all self-intersections please use
        #        CheckSelfIntersections() method.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fuse "Example"
        @ManageTransactions("BoolOp")
        def MakeBoolean(self, theShape1, theShape2, theOperation, checkSelfInte=False, theName=None):
            """
            Perform one of boolean operations on two given shapes.

            Parameters:
                theShape1 First argument for boolean operation.
                theShape2 Second argument for boolean operation.
                theOperation Indicates the operation to be done:
                             1 - Common, 2 - Cut, 3 - Fuse, 4 - Section.
                checkSelfInte The flag that tells if the arguments should
                              be checked for self-intersection prior to
                              the operation.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Note:
                    This algorithm doesn't find all types of self-intersections.
                    It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
                    vertex/face and edge/face intersections. Face/face
                    intersections detection is switched off as it is a
                    time-consuming operation that gives an impact on performance.
                    To find all self-intersections please use
                    CheckSelfIntersections() method.

            Returns:
                New GEOM.GEOM_Object, containing the result shape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BoolOp.MakeBoolean(theShape1, theShape2, theOperation, checkSelfInte)
            RaiseIfFailed("MakeBoolean", self.BoolOp)
            def_names = { 1: "common", 2: "cut", 3: "fuse", 4: "section" }
            self._autoPublish(anObj, theName, def_names[theOperation])
            return anObj

        ## Perform Common boolean operation on two given shapes.
        #  @param theShape1 First argument for boolean operation.
        #  @param theShape2 Second argument for boolean operation.
        #  @param checkSelfInte The flag that tells if the arguments should
        #         be checked for self-intersection prior to the operation.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note This algorithm doesn't find all types of self-intersections.
        #        It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
        #        vertex/face and edge/face intersections. Face/face
        #        intersections detection is switched off as it is a
        #        time-consuming operation that gives an impact on performance.
        #        To find all self-intersections please use
        #        CheckSelfIntersections() method.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_common "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        def MakeCommon(self, theShape1, theShape2, checkSelfInte=False, theName=None):
            """
            Perform Common boolean operation on two given shapes.

            Parameters:
                theShape1 First argument for boolean operation.
                theShape2 Second argument for boolean operation.
                checkSelfInte The flag that tells if the arguments should
                              be checked for self-intersection prior to
                              the operation.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Note:
                    This algorithm doesn't find all types of self-intersections.
                    It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
                    vertex/face and edge/face intersections. Face/face
                    intersections detection is switched off as it is a
                    time-consuming operation that gives an impact on performance.
                    To find all self-intersections please use
                    CheckSelfIntersections() method.

            Returns:
                New GEOM.GEOM_Object, containing the result shape.
            """
            # Example: see GEOM_TestOthers.py
            # note: auto-publishing is done in self.MakeBoolean()
            return self.MakeBoolean(theShape1, theShape2, 1, checkSelfInte, theName)

        ## Perform Cut boolean operation on two given shapes.
        #  @param theShape1 First argument for boolean operation.
        #  @param theShape2 Second argument for boolean operation.
        #  @param checkSelfInte The flag that tells if the arguments should
        #         be checked for self-intersection prior to the operation.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note This algorithm doesn't find all types of self-intersections.
        #        It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
        #        vertex/face and edge/face intersections. Face/face
        #        intersections detection is switched off as it is a
        #        time-consuming operation that gives an impact on performance.
        #        To find all self-intersections please use
        #        CheckSelfIntersections() method.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_cut "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        def MakeCut(self, theShape1, theShape2, checkSelfInte=False, theName=None):
            """
            Perform Cut boolean operation on two given shapes.

            Parameters:
                theShape1 First argument for boolean operation.
                theShape2 Second argument for boolean operation.
                checkSelfInte The flag that tells if the arguments should
                              be checked for self-intersection prior to
                              the operation.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Note:
                    This algorithm doesn't find all types of self-intersections.
                    It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
                    vertex/face and edge/face intersections. Face/face
                    intersections detection is switched off as it is a
                    time-consuming operation that gives an impact on performance.
                    To find all self-intersections please use
                    CheckSelfIntersections() method.

            Returns:
                New GEOM.GEOM_Object, containing the result shape.

            """
            # Example: see GEOM_TestOthers.py
            # note: auto-publishing is done in self.MakeBoolean()
            return self.MakeBoolean(theShape1, theShape2, 2, checkSelfInte, theName)

        ## Perform Fuse boolean operation on two given shapes.
        #  @param theShape1 First argument for boolean operation.
        #  @param theShape2 Second argument for boolean operation.
        #  @param checkSelfInte The flag that tells if the arguments should
        #         be checked for self-intersection prior to the operation.
        #  @param rmExtraEdges The flag that tells if Remove Extra Edges
        #         operation should be performed during the operation.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note This algorithm doesn't find all types of self-intersections.
        #        It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
        #        vertex/face and edge/face intersections. Face/face
        #        intersections detection is switched off as it is a
        #        time-consuming operation that gives an impact on performance.
        #        To find all self-intersections please use
        #        CheckSelfIntersections() method.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fuse "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        @ManageTransactions("BoolOp")
        def MakeFuse(self, theShape1, theShape2, checkSelfInte=False,
                     rmExtraEdges=False, theName=None):
            """
            Perform Fuse boolean operation on two given shapes.

            Parameters:
                theShape1 First argument for boolean operation.
                theShape2 Second argument for boolean operation.
                checkSelfInte The flag that tells if the arguments should
                              be checked for self-intersection prior to
                              the operation.
                rmExtraEdges The flag that tells if Remove Extra Edges
                             operation should be performed during the operation.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Note:
                    This algorithm doesn't find all types of self-intersections.
                    It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
                    vertex/face and edge/face intersections. Face/face
                    intersections detection is switched off as it is a
                    time-consuming operation that gives an impact on performance.
                    To find all self-intersections please use
                    CheckSelfIntersections() method.

            Returns:
                New GEOM.GEOM_Object, containing the result shape.

            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BoolOp.MakeFuse(theShape1, theShape2,
                                         checkSelfInte, rmExtraEdges)
            RaiseIfFailed("MakeFuse", self.BoolOp)
            self._autoPublish(anObj, theName, "fuse")
            return anObj

        ## Perform Section boolean operation on two given shapes.
        #  @param theShape1 First argument for boolean operation.
        #  @param theShape2 Second argument for boolean operation.
        #  @param checkSelfInte The flag that tells if the arguments should
        #         be checked for self-intersection prior to the operation.
        #         If a self-intersection detected the operation fails.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_section "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        def MakeSection(self, theShape1, theShape2, checkSelfInte=False, theName=None):
            """
            Perform Section boolean operation on two given shapes.

            Parameters:
                theShape1 First argument for boolean operation.
                theShape2 Second argument for boolean operation.
                checkSelfInte The flag that tells if the arguments should
                              be checked for self-intersection prior to the operation.
                              If a self-intersection detected the operation fails.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.
            Returns:
                New GEOM.GEOM_Object, containing the result shape.

            """
            # Example: see GEOM_TestOthers.py
            # note: auto-publishing is done in self.MakeBoolean()
            return self.MakeBoolean(theShape1, theShape2, 4, checkSelfInte, theName)

        ## Perform Fuse boolean operation on the list of shapes.
        #  @param theShapesList Shapes to be fused.
        #  @param checkSelfInte The flag that tells if the arguments should
        #         be checked for self-intersection prior to the operation.
        #  @param rmExtraEdges The flag that tells if Remove Extra Edges
        #         operation should be performed during the operation.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note This algorithm doesn't find all types of self-intersections.
        #        It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
        #        vertex/face and edge/face intersections. Face/face
        #        intersections detection is switched off as it is a
        #        time-consuming operation that gives an impact on performance.
        #        To find all self-intersections please use
        #        CheckSelfIntersections() method.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fuse "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        @ManageTransactions("BoolOp")
        def MakeFuseList(self, theShapesList, checkSelfInte=False,
                         rmExtraEdges=False, theName=None):
            """
            Perform Fuse boolean operation on the list of shapes.

            Parameters:
                theShapesList Shapes to be fused.
                checkSelfInte The flag that tells if the arguments should
                              be checked for self-intersection prior to
                              the operation.
                rmExtraEdges The flag that tells if Remove Extra Edges
                             operation should be performed during the operation.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Note:
                    This algorithm doesn't find all types of self-intersections.
                    It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
                    vertex/face and edge/face intersections. Face/face
                    intersections detection is switched off as it is a
                    time-consuming operation that gives an impact on performance.
                    To find all self-intersections please use
                    CheckSelfIntersections() method.

            Returns:
                New GEOM.GEOM_Object, containing the result shape.

            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BoolOp.MakeFuseList(theShapesList, checkSelfInte,
                                             rmExtraEdges)
            RaiseIfFailed("MakeFuseList", self.BoolOp)
            self._autoPublish(anObj, theName, "fuse")
            return anObj

        ## Perform Common boolean operation on the list of shapes.
        #  @param theShapesList Shapes for Common operation.
        #  @param checkSelfInte The flag that tells if the arguments should
        #         be checked for self-intersection prior to the operation.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note This algorithm doesn't find all types of self-intersections.
        #        It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
        #        vertex/face and edge/face intersections. Face/face
        #        intersections detection is switched off as it is a
        #        time-consuming operation that gives an impact on performance.
        #        To find all self-intersections please use
        #        CheckSelfIntersections() method.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_common "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        @ManageTransactions("BoolOp")
        def MakeCommonList(self, theShapesList, checkSelfInte=False, theName=None):
            """
            Perform Common boolean operation on the list of shapes.

            Parameters:
                theShapesList Shapes for Common operation.
                checkSelfInte The flag that tells if the arguments should
                              be checked for self-intersection prior to
                              the operation.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Note:
                    This algorithm doesn't find all types of self-intersections.
                    It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
                    vertex/face and edge/face intersections. Face/face
                    intersections detection is switched off as it is a
                    time-consuming operation that gives an impact on performance.
                    To find all self-intersections please use
                    CheckSelfIntersections() method.

            Returns:
                New GEOM.GEOM_Object, containing the result shape.

            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BoolOp.MakeCommonList(theShapesList, checkSelfInte)
            RaiseIfFailed("MakeCommonList", self.BoolOp)
            self._autoPublish(anObj, theName, "common")
            return anObj

        ## Perform Cut boolean operation on one object and the list of tools.
        #  @param theMainShape The object of the operation.
        #  @param theShapesList The list of tools of the operation.
        #  @param checkSelfInte The flag that tells if the arguments should
        #         be checked for self-intersection prior to the operation.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note This algorithm doesn't find all types of self-intersections.
        #        It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
        #        vertex/face and edge/face intersections. Face/face
        #        intersections detection is switched off as it is a
        #        time-consuming operation that gives an impact on performance.
        #        To find all self-intersections please use
        #        CheckSelfIntersections() method.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_cut "Example 1"
        #  \n @ref swig_MakeCommon "Example 2"
        @ManageTransactions("BoolOp")
        def MakeCutList(self, theMainShape, theShapesList, checkSelfInte=False, theName=None):
            """
            Perform Cut boolean operation on one object and the list of tools.

            Parameters:
                theMainShape The object of the operation.
                theShapesList The list of tools of the operation.
                checkSelfInte The flag that tells if the arguments should
                              be checked for self-intersection prior to
                              the operation.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Note:
                    This algorithm doesn't find all types of self-intersections.
                    It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
                    vertex/face and edge/face intersections. Face/face
                    intersections detection is switched off as it is a
                    time-consuming operation that gives an impact on performance.
                    To find all self-intersections please use
                    CheckSelfIntersections() method.

            Returns:
                New GEOM.GEOM_Object, containing the result shape.

            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BoolOp.MakeCutList(theMainShape, theShapesList, checkSelfInte)
            RaiseIfFailed("MakeCutList", self.BoolOp)
            self._autoPublish(anObj, theName, "cut")
            return anObj

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
        #
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
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
        @ManageTransactions("BoolOp")
        def MakePartition(self, ListShapes, ListTools=[], ListKeepInside=[], ListRemoveInside=[],
                          Limit=ShapeType["AUTO"], RemoveWebs=0, ListMaterials=[],
                          KeepNonlimitShapes=0, theName=None):
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

                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.
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
            if Limit == self.ShapeType["AUTO"]:
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
            self._autoPublish(anObj, theName, "partition")
            return anObj

        ## Perform partition operation.
        #  This method may be useful if it is needed to make a partition for
        #  compound contains nonintersected shapes. Performance will be better
        #  since intersection between shapes from compound is not performed.
        #
        #  Description of all parameters as in previous method MakePartition().
        #  One additional parameter is provided:
        #  @param checkSelfInte The flag that tells if the arguments should
        #         be checked for self-intersection prior to the operation.
        #
        #  @note This algorithm doesn't find all types of self-intersections.
        #        It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
        #        vertex/face and edge/face intersections. Face/face
        #        intersections detection is switched off as it is a
        #        time-consuming operation that gives an impact on performance.
        #        To find all self-intersections please use
        #        CheckSelfIntersections() method.
        #
        #  @note Passed compounds (via ListShapes or via ListTools)
        #           have to consist of nonintersecting shapes.
        #
        #  @return New GEOM.GEOM_Object, containing the result shapes.
        #
        #  @ref swig_todo "Example"
        @ManageTransactions("BoolOp")
        def MakePartitionNonSelfIntersectedShape(self, ListShapes, ListTools=[],
                                                 ListKeepInside=[], ListRemoveInside=[],
                                                 Limit=ShapeType["AUTO"], RemoveWebs=0,
                                                 ListMaterials=[], KeepNonlimitShapes=0,
                                                 checkSelfInte=False, theName=None):
            """
            Perform partition operation.
            This method may be useful if it is needed to make a partition for
            compound contains nonintersected shapes. Performance will be better
            since intersection between shapes from compound is not performed.

            Parameters:
                Description of all parameters as in method geompy.MakePartition.
                One additional parameter is provided:
                checkSelfInte The flag that tells if the arguments should
                              be checked for self-intersection prior to
                              the operation.

            Note:
                    This algorithm doesn't find all types of self-intersections.
                    It is tuned to detect vertex/vertex, vertex/edge, edge/edge,
                    vertex/face and edge/face intersections. Face/face
                    intersections detection is switched off as it is a
                    time-consuming operation that gives an impact on performance.
                    To find all self-intersections please use
                    CheckSelfIntersections() method.

            NOTE:
                Passed compounds (via ListShapes or via ListTools)
                have to consist of nonintersecting shapes.

            Returns:
                New GEOM.GEOM_Object, containing the result shapes.
            """
            if Limit == self.ShapeType["AUTO"]:
                # automatic detection of the most appropriate shape limit type
                lim = GEOM.SHAPE
                for s in ListShapes: lim = min( lim, s.GetMaxShapeType() )
                Limit = EnumToLong(lim)
                pass
            anObj = self.BoolOp.MakePartitionNonSelfIntersectedShape(ListShapes, ListTools,
                                                                     ListKeepInside, ListRemoveInside,
                                                                     Limit, RemoveWebs, ListMaterials,
                                                                     KeepNonlimitShapes, checkSelfInte);
            RaiseIfFailed("MakePartitionNonSelfIntersectedShape", self.BoolOp)
            self._autoPublish(anObj, theName, "partition")
            return anObj

        ## See method MakePartition() for more information.
        #
        #  @ref tui_partition "Example 1"
        #  \n @ref swig_Partition "Example 2"
        def Partition(self, ListShapes, ListTools=[], ListKeepInside=[], ListRemoveInside=[],
                      Limit=ShapeType["AUTO"], RemoveWebs=0, ListMaterials=[],
                      KeepNonlimitShapes=0, theName=None):
            """
            See method geompy.MakePartition for more information.
            """
            # Example: see GEOM_TestOthers.py
            # note: auto-publishing is done in self.MakePartition()
            anObj = self.MakePartition(ListShapes, ListTools,
                                       ListKeepInside, ListRemoveInside,
                                       Limit, RemoveWebs, ListMaterials,
                                       KeepNonlimitShapes, theName);
            return anObj

        ## Perform partition of the Shape with the Plane
        #  @param theShape Shape to be intersected.
        #  @param thePlane Tool shape, to intersect theShape.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @note This operation is a shortcut to the more general @ref MakePartition
        #  operation, where @a theShape specifies single "object" (shape being partitioned)
        #  and @a thePlane specifies single "tool" (intersector shape). Other parameters of
        #  @ref MakePartition operation have default values:
        #  - @a Limit: GEOM::SHAPE (shape limit corresponds to the type of @a theShape)
        #  - @a KeepNonlimitShapes: 0
        #  - @a KeepInside, @a RemoveInside, @a RemoveWebs,
        #    @a Materials (obsolete parameters): empty
        #
        #  @note I.e. the following two operations are equivalent:
        #  @code
        #  Result = geompy.MakeHalfPartition(Object, Plane)
        #  Result = geompy.MakePartition([Object], [Plane])
        #  @endcode
        #
        #  @sa MakePartition, MakePartitionNonSelfIntersectedShape
        #
        #  @ref tui_partition "Example"
        @ManageTransactions("BoolOp")
        def MakeHalfPartition(self, theShape, thePlane, theName=None):
            """
            Perform partition of the Shape with the Plane

            Parameters:
                theShape Shape to be intersected.
                thePlane Tool shape, to intersect theShape.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the result shape.
         
            Note: This operation is a shortcut to the more general MakePartition
            operation, where theShape specifies single "object" (shape being partitioned)
            and thePlane specifies single "tool" (intersector shape). Other parameters of
            MakePartition operation have default values:
            - Limit: GEOM::SHAPE (shape limit corresponds to the type of theShape)
            - KeepNonlimitShapes: 0
            - KeepInside, RemoveInside, RemoveWebs, Materials (obsolete parameters): empty
         
            I.e. the following two operations are equivalent:
              Result = geompy.MakeHalfPartition(Object, Plane)
              Result = geompy.MakePartition([Object], [Plane])
            """
            # Example: see GEOM_TestAll.py
            anObj = self.BoolOp.MakeHalfPartition(theShape, thePlane)
            RaiseIfFailed("MakeHalfPartition", self.BoolOp)
            self._autoPublish(anObj, theName, "partition")
            return anObj

        # end of l3_basic_op
        ## @}

        ## @addtogroup l3_transform
        ## @{

        ## Translate the given object along the vector, specified
        #  by its end points.
        #  @param theObject The object to be translated.
        #  @param thePoint1 Start point of translation vector.
        #  @param thePoint2 End point of translation vector.
        #  @param theCopy Flag used to translate object itself or create a copy.
        #  @return Translated @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the translated object if @a theCopy flag is @c True.
        @ManageTransactions("TrsfOp")
        def TranslateTwoPoints(self, theObject, thePoint1, thePoint2, theCopy=False):
            """
            Translate the given object along the vector, specified by its end points.

            Parameters:
                theObject The object to be translated.
                thePoint1 Start point of translation vector.
                thePoint2 End point of translation vector.
                theCopy Flag used to translate object itself or create a copy.

            Returns:
                Translated theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the translated object if theCopy flag is True.
            """
            if theCopy:
                anObj = self.TrsfOp.TranslateTwoPointsCopy(theObject, thePoint1, thePoint2)
            else:
                anObj = self.TrsfOp.TranslateTwoPoints(theObject, thePoint1, thePoint2)
            RaiseIfFailed("TranslateTwoPoints", self.TrsfOp)
            return anObj

        ## Translate the given object along the vector, specified
        #  by its end points, creating its copy before the translation.
        #  @param theObject The object to be translated.
        #  @param thePoint1 Start point of translation vector.
        #  @param thePoint2 End point of translation vector.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example 1"
        #  \n @ref swig_MakeTranslationTwoPoints "Example 2"
        @ManageTransactions("TrsfOp")
        def MakeTranslationTwoPoints(self, theObject, thePoint1, thePoint2, theName=None):
            """
            Translate the given object along the vector, specified
            by its end points, creating its copy before the translation.

            Parameters:
                theObject The object to be translated.
                thePoint1 Start point of translation vector.
                thePoint2 End point of translation vector.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the translated object.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.TranslateTwoPointsCopy(theObject, thePoint1, thePoint2)
            RaiseIfFailed("TranslateTwoPointsCopy", self.TrsfOp)
            self._autoPublish(anObj, theName, "translated")
            return anObj

        ## Translate the given object along the vector, specified by its components.
        #  @param theObject The object to be translated.
        #  @param theDX,theDY,theDZ Components of translation vector.
        #  @param theCopy Flag used to translate object itself or create a copy.
        #  @return Translated @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the translated object if @a theCopy flag is @c True.
        #
        #  @ref tui_translation "Example"
        @ManageTransactions("TrsfOp")
        def TranslateDXDYDZ(self, theObject, theDX, theDY, theDZ, theCopy=False):
            """
            Translate the given object along the vector, specified by its components.

            Parameters:
                theObject The object to be translated.
                theDX,theDY,theDZ Components of translation vector.
                theCopy Flag used to translate object itself or create a copy.

            Returns:
                Translated theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the translated object if theCopy flag is True.
            """
            # Example: see GEOM_TestAll.py
            theDX, theDY, theDZ, Parameters = ParseParameters(theDX, theDY, theDZ)
            if theCopy:
                anObj = self.TrsfOp.TranslateDXDYDZCopy(theObject, theDX, theDY, theDZ)
            else:
                anObj = self.TrsfOp.TranslateDXDYDZ(theObject, theDX, theDY, theDZ)
            anObj.SetParameters(Parameters)
            RaiseIfFailed("TranslateDXDYDZ", self.TrsfOp)
            return anObj

        ## Translate the given object along the vector, specified
        #  by its components, creating its copy before the translation.
        #  @param theObject The object to be translated.
        #  @param theDX,theDY,theDZ Components of translation vector.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example"
        @ManageTransactions("TrsfOp")
        def MakeTranslation(self,theObject, theDX, theDY, theDZ, theName=None):
            """
            Translate the given object along the vector, specified
            by its components, creating its copy before the translation.

            Parameters:
                theObject The object to be translated.
                theDX,theDY,theDZ Components of translation vector.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the translated object.
            """
            # Example: see GEOM_TestAll.py
            theDX, theDY, theDZ, Parameters = ParseParameters(theDX, theDY, theDZ)
            anObj = self.TrsfOp.TranslateDXDYDZCopy(theObject, theDX, theDY, theDZ)
            anObj.SetParameters(Parameters)
            RaiseIfFailed("TranslateDXDYDZ", self.TrsfOp)
            self._autoPublish(anObj, theName, "translated")
            return anObj

        ## Translate the given object along the given vector.
        #  @param theObject The object to be translated.
        #  @param theVector The translation vector.
        #  @param theCopy Flag used to translate object itself or create a copy.
        #  @return Translated @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the translated object if @a theCopy flag is @c True.
        @ManageTransactions("TrsfOp")
        def TranslateVector(self, theObject, theVector, theCopy=False):
            """
            Translate the given object along the given vector.

            Parameters:
                theObject The object to be translated.
                theVector The translation vector.
                theCopy Flag used to translate object itself or create a copy.

            Returns:
                Translated theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the translated object if theCopy flag is True.
            """
            if theCopy:
                anObj = self.TrsfOp.TranslateVectorCopy(theObject, theVector)
            else:
                anObj = self.TrsfOp.TranslateVector(theObject, theVector)
            RaiseIfFailed("TranslateVector", self.TrsfOp)
            return anObj

        ## Translate the given object along the given vector,
        #  creating its copy before the translation.
        #  @param theObject The object to be translated.
        #  @param theVector The translation vector.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example"
        @ManageTransactions("TrsfOp")
        def MakeTranslationVector(self, theObject, theVector, theName=None):
            """
            Translate the given object along the given vector,
            creating its copy before the translation.

            Parameters:
                theObject The object to be translated.
                theVector The translation vector.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the translated object.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.TranslateVectorCopy(theObject, theVector)
            RaiseIfFailed("TranslateVectorCopy", self.TrsfOp)
            self._autoPublish(anObj, theName, "translated")
            return anObj

        ## Translate the given object along the given vector on given distance.
        #  @param theObject The object to be translated.
        #  @param theVector The translation vector.
        #  @param theDistance The translation distance.
        #  @param theCopy Flag used to translate object itself or create a copy.
        #  @return Translated @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the translated object if @a theCopy flag is @c True.
        #
        #  @ref tui_translation "Example"
        @ManageTransactions("TrsfOp")
        def TranslateVectorDistance(self, theObject, theVector, theDistance, theCopy=False):
            """
            Translate the given object along the given vector on given distance.

            Parameters:
                theObject The object to be translated.
                theVector The translation vector.
                theDistance The translation distance.
                theCopy Flag used to translate object itself or create a copy.

            Returns:
                Translated theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the translated object if theCopy flag is True.
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the translated object.
        #
        #  @ref tui_translation "Example"
        @ManageTransactions("TrsfOp")
        def MakeTranslationVectorDistance(self, theObject, theVector, theDistance, theName=None):
            """
            Translate the given object along the given vector on given distance,
            creating its copy before the translation.

            Parameters:
                theObject The object to be translated.
                theVector The translation vector.
                theDistance The translation distance.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the translated object.
            """
            # Example: see GEOM_TestAll.py
            theDistance,Parameters = ParseParameters(theDistance)
            anObj = self.TrsfOp.TranslateVectorDistance(theObject, theVector, theDistance, 1)
            RaiseIfFailed("TranslateVectorDistance", self.TrsfOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "translated")
            return anObj

        ## Rotate the given object around the given axis on the given angle.
        #  @param theObject The object to be rotated.
        #  @param theAxis Rotation axis.
        #  @param theAngle Rotation angle in radians.
        #  @param theCopy Flag used to rotate object itself or create a copy.
        #
        #  @return Rotated @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the rotated object if @a theCopy flag is @c True.
        #
        #  @ref tui_rotation "Example"
        @ManageTransactions("TrsfOp")
        def Rotate(self, theObject, theAxis, theAngle, theCopy=False):
            """
            Rotate the given object around the given axis on the given angle.

            Parameters:
                theObject The object to be rotated.
                theAxis Rotation axis.
                theAngle Rotation angle in radians.
                theCopy Flag used to rotate object itself or create a copy.

            Returns:
                Rotated theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the rotated object if theCopy flag is True.
            """
            # Example: see GEOM_TestAll.py
            flag = False
            if isinstance(theAngle,str):
                flag = True
            theAngle, Parameters = ParseParameters(theAngle)
            if flag:
                theAngle = theAngle*math.pi/180.0
            if theCopy:
                anObj = self.TrsfOp.RotateCopy(theObject, theAxis, theAngle)
            else:
                anObj = self.TrsfOp.Rotate(theObject, theAxis, theAngle)
            RaiseIfFailed("Rotate", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Rotate the given object around the given axis
        #  on the given angle, creating its copy before the rotation.
        #  @param theObject The object to be rotated.
        #  @param theAxis Rotation axis.
        #  @param theAngle Rotation angle in radians.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the rotated object.
        #
        #  @ref tui_rotation "Example"
        @ManageTransactions("TrsfOp")
        def MakeRotation(self, theObject, theAxis, theAngle, theName=None):
            """
            Rotate the given object around the given axis
            on the given angle, creating its copy before the rotatation.

            Parameters:
                theObject The object to be rotated.
                theAxis Rotation axis.
                theAngle Rotation angle in radians.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "rotated")
            return anObj

        ## Rotate given object around vector perpendicular to plane
        #  containing three points.
        #  @param theObject The object to be rotated.
        #  @param theCentPoint central point the axis is the vector perpendicular to the plane
        #  containing the three points.
        #  @param thePoint1,thePoint2 points in a perpendicular plane of the axis.
        #  @param theCopy Flag used to rotate object itself or create a copy.
        #  @return Rotated @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the rotated object if @a theCopy flag is @c True.
        @ManageTransactions("TrsfOp")
        def RotateThreePoints(self, theObject, theCentPoint, thePoint1, thePoint2, theCopy=False):
            """
            Rotate given object around vector perpendicular to plane
            containing three points.

            Parameters:
                theObject The object to be rotated.
                theCentPoint central point  the axis is the vector perpendicular to the plane
                             containing the three points.
                thePoint1,thePoint2 points in a perpendicular plane of the axis.
                theCopy Flag used to rotate object itself or create a copy.

            Returns:
                Rotated theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the rotated object if theCopy flag is True.
            """
            if theCopy:
                anObj = self.TrsfOp.RotateThreePointsCopy(theObject, theCentPoint, thePoint1, thePoint2)
            else:
                anObj = self.TrsfOp.RotateThreePoints(theObject, theCentPoint, thePoint1, thePoint2)
            RaiseIfFailed("RotateThreePoints", self.TrsfOp)
            return anObj

        ## Rotate given object around vector perpendicular to plane
        #  containing three points, creating its copy before the rotatation.
        #  @param theObject The object to be rotated.
        #  @param theCentPoint central point the axis is the vector perpendicular to the plane
        #  containing the three points.
        #  @param thePoint1,thePoint2 in a perpendicular plane of the axis.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the rotated object.
        #
        #  @ref tui_rotation "Example"
        @ManageTransactions("TrsfOp")
        def MakeRotationThreePoints(self, theObject, theCentPoint, thePoint1, thePoint2, theName=None):
            """
            Rotate given object around vector perpendicular to plane
            containing three points, creating its copy before the rotatation.

            Parameters:
                theObject The object to be rotated.
                theCentPoint central point  the axis is the vector perpendicular to the plane
                             containing the three points.
                thePoint1,thePoint2  in a perpendicular plane of the axis.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the rotated object.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.RotateThreePointsCopy(theObject, theCentPoint, thePoint1, thePoint2)
            RaiseIfFailed("RotateThreePointsCopy", self.TrsfOp)
            self._autoPublish(anObj, theName, "rotated")
            return anObj

        ## Scale the given object by the specified factor.
        #  @param theObject The object to be scaled.
        #  @param thePoint Center point for scaling.
        #                  Passing None for it means scaling relatively the origin of global CS.
        #  @param theFactor Scaling factor value.
        #  @param theCopy Flag used to scale object itself or create a copy.
        #  @return Scaled @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the scaled object if @a theCopy flag is @c True.
        @ManageTransactions("TrsfOp")
        def Scale(self, theObject, thePoint, theFactor, theCopy=False):
            """
            Scale the given object by the specified factor.

            Parameters:
                theObject The object to be scaled.
                thePoint Center point for scaling.
                         Passing None for it means scaling relatively the origin of global CS.
                theFactor Scaling factor value.
                theCopy Flag used to scale object itself or create a copy.

            Returns:
                Scaled theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the scaled object if theCopy flag is True.
            """
            # Example: see GEOM_TestAll.py
            theFactor, Parameters = ParseParameters(theFactor)
            if theCopy:
                anObj = self.TrsfOp.ScaleShapeCopy(theObject, thePoint, theFactor)
            else:
                anObj = self.TrsfOp.ScaleShape(theObject, thePoint, theFactor)
            RaiseIfFailed("Scale", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Scale the given object by the factor, creating its copy before the scaling.
        #  @param theObject The object to be scaled.
        #  @param thePoint Center point for scaling.
        #                  Passing None for it means scaling relatively the origin of global CS.
        #  @param theFactor Scaling factor value.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the scaled shape.
        #
        #  @ref tui_scale "Example"
        @ManageTransactions("TrsfOp")
        def MakeScaleTransform(self, theObject, thePoint, theFactor, theName=None):
            """
            Scale the given object by the factor, creating its copy before the scaling.

            Parameters:
                theObject The object to be scaled.
                thePoint Center point for scaling.
                         Passing None for it means scaling relatively the origin of global CS.
                theFactor Scaling factor value.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the scaled shape.
            """
            # Example: see GEOM_TestAll.py
            theFactor, Parameters = ParseParameters(theFactor)
            anObj = self.TrsfOp.ScaleShapeCopy(theObject, thePoint, theFactor)
            RaiseIfFailed("ScaleShapeCopy", self.TrsfOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "scaled")
            return anObj

        ## Scale the given object by different factors along coordinate axes.
        #  @param theObject The object to be scaled.
        #  @param thePoint Center point for scaling.
        #                  Passing None for it means scaling relatively the origin of global CS.
        #  @param theFactorX,theFactorY,theFactorZ Scaling factors along each axis.
        #  @param theCopy Flag used to scale object itself or create a copy.
        #  @return Scaled @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the scaled object if @a theCopy flag is @c True.
        @ManageTransactions("TrsfOp")
        def ScaleAlongAxes(self, theObject, thePoint, theFactorX, theFactorY, theFactorZ, theCopy=False):
            """
            Scale the given object by different factors along coordinate axes.

            Parameters:
                theObject The object to be scaled.
                thePoint Center point for scaling.
                            Passing None for it means scaling relatively the origin of global CS.
                theFactorX,theFactorY,theFactorZ Scaling factors along each axis.
                theCopy Flag used to scale object itself or create a copy.

            Returns:
                Scaled theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the scaled object if theCopy flag is True.
            """
            # Example: see GEOM_TestAll.py
            theFactorX, theFactorY, theFactorZ, Parameters = ParseParameters(theFactorX, theFactorY, theFactorZ)
            if theCopy:
                anObj = self.TrsfOp.ScaleShapeAlongAxesCopy(theObject, thePoint,
                                                            theFactorX, theFactorY, theFactorZ)
            else:
                anObj = self.TrsfOp.ScaleShapeAlongAxes(theObject, thePoint,
                                                        theFactorX, theFactorY, theFactorZ)
            RaiseIfFailed("ScaleAlongAxes", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Scale the given object by different factors along coordinate axes,
        #  creating its copy before the scaling.
        #  @param theObject The object to be scaled.
        #  @param thePoint Center point for scaling.
        #                  Passing None for it means scaling relatively the origin of global CS.
        #  @param theFactorX,theFactorY,theFactorZ Scaling factors along each axis.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the scaled shape.
        #
        #  @ref swig_scale "Example"
        @ManageTransactions("TrsfOp")
        def MakeScaleAlongAxes(self, theObject, thePoint, theFactorX, theFactorY, theFactorZ, theName=None):
            """
            Scale the given object by different factors along coordinate axes,
            creating its copy before the scaling.

            Parameters:
                theObject The object to be scaled.
                thePoint Center point for scaling.
                            Passing None for it means scaling relatively the origin of global CS.
                theFactorX,theFactorY,theFactorZ Scaling factors along each axis.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the scaled shape.
            """
            # Example: see GEOM_TestAll.py
            theFactorX, theFactorY, theFactorZ, Parameters = ParseParameters(theFactorX, theFactorY, theFactorZ)
            anObj = self.TrsfOp.ScaleShapeAlongAxesCopy(theObject, thePoint,
                                                        theFactorX, theFactorY, theFactorZ)
            RaiseIfFailed("MakeScaleAlongAxes", self.TrsfOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "scaled")
            return anObj

        ## Mirror an object relatively the given plane.
        #  @param theObject The object to be mirrored.
        #  @param thePlane Plane of symmetry.
        #  @param theCopy Flag used to mirror object itself or create a copy.
        #  @return Mirrored @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the mirrored object if @a theCopy flag is @c True.
        @ManageTransactions("TrsfOp")
        def MirrorByPlane(self, theObject, thePlane, theCopy=False):
            """
            Mirror an object relatively the given plane.

            Parameters:
                theObject The object to be mirrored.
                thePlane Plane of symmetry.
                theCopy Flag used to mirror object itself or create a copy.

            Returns:
                Mirrored theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the mirrored object if theCopy flag is True.
            """
            if theCopy:
                anObj = self.TrsfOp.MirrorPlaneCopy(theObject, thePlane)
            else:
                anObj = self.TrsfOp.MirrorPlane(theObject, thePlane)
            RaiseIfFailed("MirrorByPlane", self.TrsfOp)
            return anObj

        ## Create an object, symmetrical
        #  to the given one relatively the given plane.
        #  @param theObject The object to be mirrored.
        #  @param thePlane Plane of symmetry.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the mirrored shape.
        #
        #  @ref tui_mirror "Example"
        @ManageTransactions("TrsfOp")
        def MakeMirrorByPlane(self, theObject, thePlane, theName=None):
            """
            Create an object, symmetrical to the given one relatively the given plane.

            Parameters:
                theObject The object to be mirrored.
                thePlane Plane of symmetry.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the mirrored shape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.MirrorPlaneCopy(theObject, thePlane)
            RaiseIfFailed("MirrorPlaneCopy", self.TrsfOp)
            self._autoPublish(anObj, theName, "mirrored")
            return anObj

        ## Mirror an object relatively the given axis.
        #  @param theObject The object to be mirrored.
        #  @param theAxis Axis of symmetry.
        #  @param theCopy Flag used to mirror object itself or create a copy.
        #  @return Mirrored @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the mirrored object if @a theCopy flag is @c True.
        @ManageTransactions("TrsfOp")
        def MirrorByAxis(self, theObject, theAxis, theCopy=False):
            """
            Mirror an object relatively the given axis.

            Parameters:
                theObject The object to be mirrored.
                theAxis Axis of symmetry.
                theCopy Flag used to mirror object itself or create a copy.

            Returns:
                Mirrored theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the mirrored object if theCopy flag is True.
            """
            if theCopy:
                anObj = self.TrsfOp.MirrorAxisCopy(theObject, theAxis)
            else:
                anObj = self.TrsfOp.MirrorAxis(theObject, theAxis)
            RaiseIfFailed("MirrorByAxis", self.TrsfOp)
            return anObj

        ## Create an object, symmetrical
        #  to the given one relatively the given axis.
        #  @param theObject The object to be mirrored.
        #  @param theAxis Axis of symmetry.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the mirrored shape.
        #
        #  @ref tui_mirror "Example"
        @ManageTransactions("TrsfOp")
        def MakeMirrorByAxis(self, theObject, theAxis, theName=None):
            """
            Create an object, symmetrical to the given one relatively the given axis.

            Parameters:
                theObject The object to be mirrored.
                theAxis Axis of symmetry.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the mirrored shape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.MirrorAxisCopy(theObject, theAxis)
            RaiseIfFailed("MirrorAxisCopy", self.TrsfOp)
            self._autoPublish(anObj, theName, "mirrored")
            return anObj

        ## Mirror an object relatively the given point.
        #  @param theObject The object to be mirrored.
        #  @param thePoint Point of symmetry.
        #  @param theCopy Flag used to mirror object itself or create a copy.
        #  @return Mirrored @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the mirrored object if @a theCopy flag is @c True.
        @ManageTransactions("TrsfOp")
        def MirrorByPoint(self, theObject, thePoint, theCopy=False):
            """
            Mirror an object relatively the given point.

            Parameters:
                theObject The object to be mirrored.
                thePoint Point of symmetry.
                theCopy Flag used to mirror object itself or create a copy.

            Returns:
                Mirrored theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the mirrored object if theCopy flag is True.
            """
            # Example: see GEOM_TestAll.py
            if theCopy:
                anObj = self.TrsfOp.MirrorPointCopy(theObject, thePoint)
            else:
                anObj = self.TrsfOp.MirrorPoint(theObject, thePoint)
            RaiseIfFailed("MirrorByPoint", self.TrsfOp)
            return anObj

        ## Create an object, symmetrical
        #  to the given one relatively the given point.
        #  @param theObject The object to be mirrored.
        #  @param thePoint Point of symmetry.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the mirrored shape.
        #
        #  @ref tui_mirror "Example"
        @ManageTransactions("TrsfOp")
        def MakeMirrorByPoint(self, theObject, thePoint, theName=None):
            """
            Create an object, symmetrical
            to the given one relatively the given point.

            Parameters:
                theObject The object to be mirrored.
                thePoint Point of symmetry.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the mirrored shape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.MirrorPointCopy(theObject, thePoint)
            RaiseIfFailed("MirrorPointCopy", self.TrsfOp)
            self._autoPublish(anObj, theName, "mirrored")
            return anObj

        ## Modify the location of the given object.
        #  @param theObject The object to be displaced.
        #  @param theStartLCS Coordinate system to perform displacement from it.\n
        #                     If \a theStartLCS is NULL, displacement
        #                     will be performed from global CS.\n
        #                     If \a theObject itself is used as \a theStartLCS,
        #                     its location will be changed to \a theEndLCS.
        #  @param theEndLCS Coordinate system to perform displacement to it.
        #  @param theCopy Flag used to displace object itself or create a copy.
        #  @return Displaced @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the displaced object if @a theCopy flag is @c True.
        @ManageTransactions("TrsfOp")
        def Position(self, theObject, theStartLCS, theEndLCS, theCopy=False):
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
                theCopy Flag used to displace object itself or create a copy.

            Returns:
                Displaced theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the displaced object if theCopy flag is True.
            """
            # Example: see GEOM_TestAll.py
            if theCopy:
                anObj = self.TrsfOp.PositionShapeCopy(theObject, theStartLCS, theEndLCS)
            else:
                anObj = self.TrsfOp.PositionShape(theObject, theStartLCS, theEndLCS)
            RaiseIfFailed("Displace", self.TrsfOp)
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the displaced shape.
        #
        #  @ref tui_modify_location "Example"
        @ManageTransactions("TrsfOp")
        def MakePosition(self, theObject, theStartLCS, theEndLCS, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "displaced")
            return anObj

        ## Modify the Location of the given object by Path.
        #  @param  theObject The object to be displaced.
        #  @param  thePath Wire or Edge along that the object will be translated.
        #  @param  theDistance progress of Path (0 = start location, 1 = end of path location).
        #  @param  theCopy is to create a copy objects if true.
        #  @param  theReverse  0 - for usual direction, 1 - to reverse path direction.
        #  @return Displaced @a theObject (GEOM.GEOM_Object) if @a theCopy is @c False or
        #          new GEOM.GEOM_Object, containing the displaced shape if @a theCopy is @c True.
        #
        #  @ref tui_modify_location "Example"
        @ManageTransactions("TrsfOp")
        def PositionAlongPath(self,theObject, thePath, theDistance, theCopy, theReverse):
            """
            Modify the Location of the given object by Path.

            Parameters:
                 theObject The object to be displaced.
                 thePath Wire or Edge along that the object will be translated.
                 theDistance progress of Path (0 = start location, 1 = end of path location).
                 theCopy is to create a copy objects if true.
                 theReverse  0 - for usual direction, 1 - to reverse path direction.

            Returns:
                 Displaced theObject (GEOM.GEOM_Object) if theCopy is False or
                 new GEOM.GEOM_Object, containing the displaced shape if theCopy is True.

            Example of usage:
                position = geompy.PositionAlongPath(cylinder, circle, 0.75, 1, 1)
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.PositionAlongPath(theObject, thePath, theDistance, theCopy, theReverse)
            RaiseIfFailed("PositionAlongPath", self.TrsfOp)
            return anObj

        ## Modify the Location of the given object by Path, creating its copy before the operation.
        #  @param theObject The object to be displaced.
        #  @param thePath Wire or Edge along that the object will be translated.
        #  @param theDistance progress of Path (0 = start location, 1 = end of path location).
        #  @param theReverse  0 - for usual direction, 1 - to reverse path direction.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the displaced shape.
        @ManageTransactions("TrsfOp")
        def MakePositionAlongPath(self, theObject, thePath, theDistance, theReverse, theName=None):
            """
            Modify the Location of the given object by Path, creating its copy before the operation.

            Parameters:
                 theObject The object to be displaced.
                 thePath Wire or Edge along that the object will be translated.
                 theDistance progress of Path (0 = start location, 1 = end of path location).
                 theReverse  0 - for usual direction, 1 - to reverse path direction.
                 theName Object name; when specified, this parameter is used
                         for result publication in the study. Otherwise, if automatic
                         publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the displaced shape.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.PositionAlongPath(theObject, thePath, theDistance, 1, theReverse)
            RaiseIfFailed("PositionAlongPath", self.TrsfOp)
            self._autoPublish(anObj, theName, "displaced")
            return anObj

        ## Offset given shape.
        #  @param theObject The base object for the offset.
        #  @param theOffset Offset value.
        #  @param theCopy Flag used to offset object itself or create a copy.
        #  @return Modified @a theObject (GEOM.GEOM_Object) if @a theCopy flag is @c False (default) or
        #  new GEOM.GEOM_Object, containing the result of offset operation if @a theCopy flag is @c True.
        @ManageTransactions("TrsfOp")
        def Offset(self, theObject, theOffset, theCopy=False):
            """
            Offset given shape.

            Parameters:
                theObject The base object for the offset.
                theOffset Offset value.
                theCopy Flag used to offset object itself or create a copy.

            Returns:
                Modified theObject (GEOM.GEOM_Object) if theCopy flag is False (default) or
                new GEOM.GEOM_Object, containing the result of offset operation if theCopy flag is True.
            """
            theOffset, Parameters = ParseParameters(theOffset)
            if theCopy:
                anObj = self.TrsfOp.OffsetShapeCopy(theObject, theOffset)
            else:
                anObj = self.TrsfOp.OffsetShape(theObject, theOffset)
            RaiseIfFailed("Offset", self.TrsfOp)
            anObj.SetParameters(Parameters)
            return anObj

        ## Create new object as offset of the given one.
        #  @param theObject The base object for the offset.
        #  @param theOffset Offset value.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the offset object.
        #
        #  @ref tui_offset "Example"
        @ManageTransactions("TrsfOp")
        def MakeOffset(self, theObject, theOffset, theName=None):
            """
            Create new object as offset of the given one.

            Parameters:
                theObject The base object for the offset.
                theOffset Offset value.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "offset")
            return anObj

        ## Create new object as projection of the given one on another.
        #  @param theSource The source object for the projection. It can be a point, edge or wire.
        #         Edge and wire are acceptable if @a theTarget is a face.
        #  @param theTarget The target object. It can be planar or cylindrical face, edge or wire.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the projection.
        #
        #  @ref tui_projection "Example"
        @ManageTransactions("TrsfOp")
        def MakeProjection(self, theSource, theTarget, theName=None):
            """
            Create new object as projection of the given one on another.

            Parameters:
                theSource The source object for the projection. It can be a point, edge or wire.
                          Edge and wire are acceptable if theTarget is a face.
                theTarget The target object. It can be planar or cylindrical face, edge or wire.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the projection.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.ProjectShapeCopy(theSource, theTarget)
            RaiseIfFailed("ProjectShapeCopy", self.TrsfOp)
            self._autoPublish(anObj, theName, "projection")
            return anObj

        ## Create a projection of the given point on a wire or an edge.
        #  If there are no solutions or there are 2 or more solutions It throws an
        #  exception.
        #  @param thePoint the point to be projected.
        #  @param theWire the wire. The edge is accepted as well.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return [\a u, \a PointOnEdge, \a EdgeInWireIndex]
        #  \n \a u: The parameter of projection point on edge.
        #  \n \a PointOnEdge: The projection point.
        #  \n \a EdgeInWireIndex: The index of an edge in a wire.
        #
        #  @ref tui_projection "Example"
        @ManageTransactions("TrsfOp")
        def MakeProjectionOnWire(self, thePoint, theWire, theName=None):
            """
            Create a projection of the given point on a wire or an edge.
            If there are no solutions or there are 2 or more solutions It throws an
            exception.

            Parameters:
                thePoint the point to be projected.
                theWire the wire. The edge is accepted as well.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                [u, PointOnEdge, EdgeInWireIndex]
                 u: The parameter of projection point on edge.
                 PointOnEdge: The projection point.
                 EdgeInWireIndex: The index of an edge in a wire.
            """
            # Example: see GEOM_TestAll.py
            anObj = self.TrsfOp.ProjectPointOnWire(thePoint, theWire)
            RaiseIfFailed("ProjectPointOnWire", self.TrsfOp)
            self._autoPublish(anObj[1], theName, "projection")
            return anObj

        # -----------------------------------------------------------------------------
        # Patterns
        # -----------------------------------------------------------------------------

        ## Translate the given object along the given vector a given number times
        #  @param theObject The object to be translated.
        #  @param theVector Direction of the translation. DX if None.
        #  @param theStep Distance to translate on.
        #  @param theNbTimes Quantity of translations to be done.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing compound of all
        #          the shapes, obtained after each translation.
        #
        #  @ref tui_multi_translation "Example"
        @ManageTransactions("TrsfOp")
        def MakeMultiTranslation1D(self, theObject, theVector, theStep, theNbTimes, theName=None):
            """
            Translate the given object along the given vector a given number times

            Parameters:
                theObject The object to be translated.
                theVector Direction of the translation. DX if None.
                theStep Distance to translate on.
                theNbTimes Quantity of translations to be done.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "multitranslation")
            return anObj

        ## Conseqently apply two specified translations to theObject specified number of times.
        #  @param theObject The object to be translated.
        #  @param theVector1 Direction of the first translation. DX if None.
        #  @param theStep1 Step of the first translation.
        #  @param theNbTimes1 Quantity of translations to be done along theVector1.
        #  @param theVector2 Direction of the second translation. DY if None.
        #  @param theStep2 Step of the second translation.
        #  @param theNbTimes2 Quantity of translations to be done along theVector2.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing compound of all
        #          the shapes, obtained after each translation.
        #
        #  @ref tui_multi_translation "Example"
        @ManageTransactions("TrsfOp")
        def MakeMultiTranslation2D(self, theObject, theVector1, theStep1, theNbTimes1,
                                   theVector2, theStep2, theNbTimes2, theName=None):
            """
            Conseqently apply two specified translations to theObject specified number of times.

            Parameters:
                theObject The object to be translated.
                theVector1 Direction of the first translation. DX if None.
                theStep1 Step of the first translation.
                theNbTimes1 Quantity of translations to be done along theVector1.
                theVector2 Direction of the second translation. DY if None.
                theStep2 Step of the second translation.
                theNbTimes2 Quantity of translations to be done along theVector2.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "multitranslation")
            return anObj

        ## Rotate the given object around the given axis a given number times.
        #  Rotation angle will be 2*PI/theNbTimes.
        #  @param theObject The object to be rotated.
        #  @param theAxis The rotation axis. DZ if None.
        #  @param theNbTimes Quantity of rotations to be done.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing compound of all the
        #          shapes, obtained after each rotation.
        #
        #  @ref tui_multi_rotation "Example"
        @ManageTransactions("TrsfOp")
        def MultiRotate1DNbTimes (self, theObject, theAxis, theNbTimes, theName=None):
            """
            Rotate the given object around the given axis a given number times.
            Rotation angle will be 2*PI/theNbTimes.

            Parameters:
                theObject The object to be rotated.
                theAxis The rotation axis. DZ if None.
                theNbTimes Quantity of rotations to be done.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing compound of all the
                shapes, obtained after each rotation.

            Example of usage:
                rot1d = geompy.MultiRotate1DNbTimes(prism, vect, 4)
            """
            # Example: see GEOM_TestAll.py
            theNbTimes, Parameters = ParseParameters(theNbTimes)
            anObj = self.TrsfOp.MultiRotate1D(theObject, theAxis, theNbTimes)
            RaiseIfFailed("MultiRotate1DNbTimes", self.TrsfOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "multirotation")
            return anObj

        ## Rotate the given object around the given axis
        #  a given number times on the given angle.
        #  @param theObject The object to be rotated.
        #  @param theAxis The rotation axis. DZ if None.
        #  @param theAngleStep Rotation angle in radians.
        #  @param theNbTimes Quantity of rotations to be done.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing compound of all the
        #          shapes, obtained after each rotation.
        #
        #  @ref tui_multi_rotation "Example"
        @ManageTransactions("TrsfOp")
        def MultiRotate1DByStep(self, theObject, theAxis, theAngleStep, theNbTimes, theName=None):
            """
            Rotate the given object around the given axis
            a given number times on the given angle.

            Parameters:
                theObject The object to be rotated.
                theAxis The rotation axis. DZ if None.
                theAngleStep Rotation angle in radians.
                theNbTimes Quantity of rotations to be done.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing compound of all the
                shapes, obtained after each rotation.

            Example of usage:
                rot1d = geompy.MultiRotate1DByStep(prism, vect, math.pi/4, 4)
            """
            # Example: see GEOM_TestAll.py
            theAngleStep, theNbTimes, Parameters = ParseParameters(theAngleStep, theNbTimes)
            anObj = self.TrsfOp.MultiRotate1DByStep(theObject, theAxis, theAngleStep, theNbTimes)
            RaiseIfFailed("MultiRotate1DByStep", self.TrsfOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "multirotation")
            return anObj

        ## Rotate the given object around the given axis a given
        #  number times and multi-translate each rotation result.
        #  Rotation angle will be 2*PI/theNbTimes1.
        #  Translation direction passes through center of gravity
        #  of rotated shape and its projection on the rotation axis.
        #  @param theObject The object to be rotated.
        #  @param theAxis Rotation axis. DZ if None.
        #  @param theNbTimes1 Quantity of rotations to be done.
        #  @param theRadialStep Translation distance.
        #  @param theNbTimes2 Quantity of translations to be done.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing compound of all the
        #          shapes, obtained after each transformation.
        #
        #  @ref tui_multi_rotation "Example"
        @ManageTransactions("TrsfOp")
        def MultiRotate2DNbTimes(self, theObject, theAxis, theNbTimes1, theRadialStep, theNbTimes2, theName=None):
            """
            Rotate the given object around the
            given axis on the given angle a given number
            times and multi-translate each rotation result.
            Translation direction passes through center of gravity
            of rotated shape and its projection on the rotation axis.

            Parameters:
                theObject The object to be rotated.
                theAxis Rotation axis. DZ if None.
                theNbTimes1 Quantity of rotations to be done.
                theRadialStep Translation distance.
                theNbTimes2 Quantity of translations to be done.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing compound of all the
                shapes, obtained after each transformation.

            Example of usage:
                rot2d = geompy.MultiRotate2D(prism, vect, 60, 4, 50, 5)
            """
            # Example: see GEOM_TestAll.py
            theNbTimes1, theRadialStep, theNbTimes2, Parameters = ParseParameters(theNbTimes1, theRadialStep, theNbTimes2)
            anObj = self.TrsfOp.MultiRotate2DNbTimes(theObject, theAxis, theNbTimes1, theRadialStep, theNbTimes2)
            RaiseIfFailed("MultiRotate2DNbTimes", self.TrsfOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "multirotation")
            return anObj

        ## Rotate the given object around the
        #  given axis on the given angle a given number
        #  times and multi-translate each rotation result.
        #  Translation direction passes through center of gravity
        #  of rotated shape and its projection on the rotation axis.
        #  @param theObject The object to be rotated.
        #  @param theAxis Rotation axis. DZ if None.
        #  @param theAngleStep Rotation angle in radians.
        #  @param theNbTimes1 Quantity of rotations to be done.
        #  @param theRadialStep Translation distance.
        #  @param theNbTimes2 Quantity of translations to be done.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing compound of all the
        #          shapes, obtained after each transformation.
        #
        #  @ref tui_multi_rotation "Example"
        @ManageTransactions("TrsfOp")
        def MultiRotate2DByStep (self, theObject, theAxis, theAngleStep, theNbTimes1, theRadialStep, theNbTimes2, theName=None):
            """
            Rotate the given object around the
            given axis on the given angle a given number
            times and multi-translate each rotation result.
            Translation direction passes through center of gravity
            of rotated shape and its projection on the rotation axis.

            Parameters:
                theObject The object to be rotated.
                theAxis Rotation axis. DZ if None.
                theAngleStep Rotation angle in radians.
                theNbTimes1 Quantity of rotations to be done.
                theRadialStep Translation distance.
                theNbTimes2 Quantity of translations to be done.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing compound of all the
                shapes, obtained after each transformation.

            Example of usage:
                rot2d = geompy.MultiRotate2D(prism, vect, math.pi/3, 4, 50, 5)
            """
            # Example: see GEOM_TestAll.py
            theAngleStep, theNbTimes1, theRadialStep, theNbTimes2, Parameters = ParseParameters(theAngleStep, theNbTimes1, theRadialStep, theNbTimes2)
            anObj = self.TrsfOp.MultiRotate2DByStep(theObject, theAxis, theAngleStep, theNbTimes1, theRadialStep, theNbTimes2)
            RaiseIfFailed("MultiRotate2DByStep", self.TrsfOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "multirotation")
            return anObj

        ## The same, as MultiRotate1DNbTimes(), but axis is given by direction and point
        #
        #  @ref swig_MakeMultiRotation "Example"
        def MakeMultiRotation1DNbTimes(self, aShape, aDir, aPoint, aNbTimes, theName=None):
            """
            The same, as geompy.MultiRotate1DNbTimes, but axis is given by direction and point

            Example of usage:
                pz = geompy.MakeVertex(0, 0, 100)
                vy = geompy.MakeVectorDXDYDZ(0, 100, 0)
                MultiRot1D = geompy.MakeMultiRotation1DNbTimes(prism, vy, pz, 6)
            """
            # Example: see GEOM_TestOthers.py
            aVec = self.MakeLine(aPoint,aDir)
            # note: auto-publishing is done in self.MultiRotate1D()
            anObj = self.MultiRotate1DNbTimes(aShape, aVec, aNbTimes, theName)
            return anObj

        ## The same, as MultiRotate1DByStep(), but axis is given by direction and point
        #
        #  @ref swig_MakeMultiRotation "Example"
        def MakeMultiRotation1DByStep(self, aShape, aDir, aPoint, anAngle, aNbTimes, theName=None):
            """
            The same, as geompy.MultiRotate1D, but axis is given by direction and point

            Example of usage:
                pz = geompy.MakeVertex(0, 0, 100)
                vy = geompy.MakeVectorDXDYDZ(0, 100, 0)
                MultiRot1D = geompy.MakeMultiRotation1DByStep(prism, vy, pz, math.pi/3, 6)
            """
            # Example: see GEOM_TestOthers.py
            aVec = self.MakeLine(aPoint,aDir)
            # note: auto-publishing is done in self.MultiRotate1D()
            anObj = self.MultiRotate1DByStep(aShape, aVec, anAngle, aNbTimes, theName)
            return anObj

        ## The same, as MultiRotate2DNbTimes(), but axis is given by direction and point
        #
        #  @ref swig_MakeMultiRotation "Example"
        def MakeMultiRotation2DNbTimes(self, aShape, aDir, aPoint, nbtimes1, aStep, nbtimes2, theName=None):
            """
            The same, as MultiRotate2DNbTimes(), but axis is given by direction and point

            Example of usage:
                pz = geompy.MakeVertex(0, 0, 100)
                vy = geompy.MakeVectorDXDYDZ(0, 100, 0)
                MultiRot2D = geompy.MakeMultiRotation2DNbTimes(f12, vy, pz, 6, 30, 3)
            """
            # Example: see GEOM_TestOthers.py
            aVec = self.MakeLine(aPoint,aDir)
            # note: auto-publishing is done in self.MultiRotate2DNbTimes()
            anObj = self.MultiRotate2DNbTimes(aShape, aVec, nbtimes1, aStep, nbtimes2, theName)
            return anObj

        ## The same, as MultiRotate2DByStep(), but axis is given by direction and point
        #
        #  @ref swig_MakeMultiRotation "Example"
        def MakeMultiRotation2DByStep(self, aShape, aDir, aPoint, anAngle, nbtimes1, aStep, nbtimes2, theName=None):
            """
            The same, as MultiRotate2DByStep(), but axis is given by direction and point

            Example of usage:
                pz = geompy.MakeVertex(0, 0, 100)
                vy = geompy.MakeVectorDXDYDZ(0, 100, 0)
                MultiRot2D = geompy.MakeMultiRotation2DByStep(f12, vy, pz, math.pi/4, 6, 30, 3)
            """
            # Example: see GEOM_TestOthers.py
            aVec = self.MakeLine(aPoint,aDir)
            # note: auto-publishing is done in self.MultiRotate2D()
            anObj = self.MultiRotate2DByStep(aShape, aVec, anAngle, nbtimes1, aStep, nbtimes2, theName)
            return anObj

        ##
        #  Compute a wire or a face that represents a projection of the source
        #  shape onto cylinder. The cylinder's coordinate system is the same
        #  as the global coordinate system.
        #
        #  @param theObject The object to be projected. It can be either
        #         a planar wire or a face.
        #  @param theRadius The radius of the cylinder.
        #  @param theStartAngle The starting angle in radians from
        #         the cylinder's X axis around Z axis. The angle from which
        #         the projection is started.
        #  @param theAngleLength The projection length angle in radians.
        #         The angle in which to project the total length of the wire.
        #         If it is negative the projection is not scaled and natural
        #         wire length is kept for the projection.
        #  @param theAngleRotation The desired angle in radians between
        #         the tangent vector to the first curve at the first point of
        #         the theObject's projection in 2D space and U-direction of
        #         cylinder's 2D space.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape. The result
        #         represents a wire or a face that represents a projection of
        #         the source shape onto a cylinder.
        #
        #  @ref tui_projection "Example"
        def MakeProjectionOnCylinder (self, theObject, theRadius,
                                      theStartAngle=0.0, theAngleLength=-1.0,
                                      theAngleRotation=0.0,
                                      theName=None):
            """
            Compute a wire or a face that represents a projection of the source
            shape onto cylinder. The cylinder's coordinate system is the same
            as the global coordinate system.

            Parameters:
                theObject The object to be projected. It can be either
                        a planar wire or a face.
                theRadius The radius of the cylinder.
                theStartAngle The starting angle in radians from the cylinder's X axis
                        around Z axis. The angle from which the projection is started.
                theAngleLength The projection length angle in radians. The angle in which
                        to project the total length of the wire. If it is negative the
                        projection is not scaled and natural wire length is kept for
                        the projection.
                theAngleRotation The desired angle in radians between
                        the tangent vector to the first curve at the first
                        point of the theObject's projection in 2D space and
                        U-direction of cylinder's 2D space.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the result shape. The result
                represents a wire or a face that represents a projection of
                the source shape onto a cylinder.
            """
            # Example: see GEOM_TestAll.py
            flagStartAngle = False
            if isinstance(theStartAngle,str):
                flagStartAngle = True
            flagAngleLength = False
            if isinstance(theAngleLength,str):
                flagAngleLength = True
            flagAngleRotation = False
            if isinstance(theAngleRotation,str):
                flagAngleRotation = True
            theRadius, theStartAngle, theAngleLength, theAngleRotation, Parameters = ParseParameters(
              theRadius, theStartAngle, theAngleLength, theAngleRotation)
            if flagStartAngle:
                theStartAngle = theStartAngle*math.pi/180.
            if flagAngleLength:
                theAngleLength = theAngleLength*math.pi/180.
            if flagAngleRotation:
                theAngleRotation = theAngleRotation*math.pi/180.
            anObj = self.TrsfOp.MakeProjectionOnCylinder(theObject, theRadius,
                theStartAngle, theAngleLength, theAngleRotation)
            RaiseIfFailed("MakeProjectionOnCylinder", self.TrsfOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "projection")
            return anObj

        # end of l3_transform
        ## @}

        ## @addtogroup l3_transform_d
        ## @{

        ## Deprecated method. Use MultiRotate1DNbTimes instead.
        def MultiRotate1D(self, theObject, theAxis, theNbTimes, theName=None):
            """
            Deprecated method. Use MultiRotate1DNbTimes instead.
            """
            print "The method MultiRotate1D is DEPRECATED. Use MultiRotate1DNbTimes instead."
            return self.MultiRotate1DNbTimes(theObject, theAxis, theNbTimes, theName)

        ## The same, as MultiRotate2DByStep(), but theAngle is in degrees.
        #  This method is DEPRECATED. Use MultiRotate2DByStep() instead.
        @ManageTransactions("TrsfOp")
        def MultiRotate2D(self, theObject, theAxis, theAngle, theNbTimes1, theStep, theNbTimes2, theName=None):
            """
            The same, as MultiRotate2DByStep(), but theAngle is in degrees.
            This method is DEPRECATED. Use MultiRotate2DByStep() instead.

            Example of usage:
                rot2d = geompy.MultiRotate2D(prism, vect, 60, 4, 50, 5)
            """
            print "The method MultiRotate2D is DEPRECATED. Use MultiRotate2DByStep instead."
            theAngle, theNbTimes1, theStep, theNbTimes2, Parameters = ParseParameters(theAngle, theNbTimes1, theStep, theNbTimes2)
            anObj = self.TrsfOp.MultiRotate2D(theObject, theAxis, theAngle, theNbTimes1, theStep, theNbTimes2)
            RaiseIfFailed("MultiRotate2D", self.TrsfOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "multirotation")
            return anObj

        ## The same, as MultiRotate1D(), but axis is given by direction and point
        #  This method is DEPRECATED. Use MakeMultiRotation1DNbTimes instead.
        def MakeMultiRotation1D(self, aShape, aDir, aPoint, aNbTimes, theName=None):
            """
            The same, as geompy.MultiRotate1D, but axis is given by direction and point.
            This method is DEPRECATED. Use MakeMultiRotation1DNbTimes instead.

            Example of usage:
                pz = geompy.MakeVertex(0, 0, 100)
                vy = geompy.MakeVectorDXDYDZ(0, 100, 0)
                MultiRot1D = geompy.MakeMultiRotation1D(prism, vy, pz, 6)
            """
            print "The method MakeMultiRotation1D is DEPRECATED. Use MakeMultiRotation1DNbTimes instead."
            aVec = self.MakeLine(aPoint,aDir)
            # note: auto-publishing is done in self.MultiRotate1D()
            anObj = self.MultiRotate1D(aShape, aVec, aNbTimes, theName)
            return anObj

        ## The same, as MultiRotate2D(), but axis is given by direction and point
        #  This method is DEPRECATED. Use MakeMultiRotation2DByStep instead.
        def MakeMultiRotation2D(self, aShape, aDir, aPoint, anAngle, nbtimes1, aStep, nbtimes2, theName=None):
            """
            The same, as MultiRotate2D(), but axis is given by direction and point
            This method is DEPRECATED. Use MakeMultiRotation2DByStep instead.

            Example of usage:
                pz = geompy.MakeVertex(0, 0, 100)
                vy = geompy.MakeVectorDXDYDZ(0, 100, 0)
                MultiRot2D = geompy.MakeMultiRotation2D(f12, vy, pz, 45, 6, 30, 3)
            """
            print "The method MakeMultiRotation2D is DEPRECATED. Use MakeMultiRotation2DByStep instead."
            aVec = self.MakeLine(aPoint,aDir)
            # note: auto-publishing is done in self.MultiRotate2D()
            anObj = self.MultiRotate2D(aShape, aVec, anAngle, nbtimes1, aStep, nbtimes2, theName)
            return anObj

        # end of l3_transform_d
        ## @}

        ## @addtogroup l3_local
        ## @{

        ## Perform a fillet on all edges of the given shape.
        #  @param theShape Shape, to perform fillet on.
        #  @param theR Fillet radius.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet "Example 1"
        #  \n @ref swig_MakeFilletAll "Example 2"
        @ManageTransactions("LocalOp")
        def MakeFilletAll(self, theShape, theR, theName=None):
            """
            Perform a fillet on all edges of the given shape.

            Parameters:
                theShape Shape, to perform fillet on.
                theR Fillet radius.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "fillet")
            return anObj

        ## Perform a fillet on the specified edges/faces of the given shape
        #  @param theShape Shape, to perform fillet on.
        #  @param theR Fillet radius.
        #  @param theShapeType Type of shapes in <VAR>theListShapes</VAR> (see ShapeType())
        #  @param theListShapes Global indices of edges/faces to perform fillet on.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note Global index of sub-shape can be obtained, using method GetSubShapeID().
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet "Example"
        @ManageTransactions("LocalOp")
        def MakeFillet(self, theShape, theR, theShapeType, theListShapes, theName=None):
            """
            Perform a fillet on the specified edges/faces of the given shape

            Parameters:
                theShape Shape, to perform fillet on.
                theR Fillet radius.
                theShapeType Type of shapes in theListShapes (see geompy.ShapeTypes)
                theListShapes Global indices of edges/faces to perform fillet on.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            if theShapeType == self.ShapeType["EDGE"]:
                anObj = self.LocalOp.MakeFilletEdges(theShape, theR, theListShapes)
                RaiseIfFailed("MakeFilletEdges", self.LocalOp)
            else:
                anObj = self.LocalOp.MakeFilletFaces(theShape, theR, theListShapes)
                RaiseIfFailed("MakeFilletFaces", self.LocalOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "fillet")
            return anObj

        ## The same that MakeFillet() but with two Fillet Radius R1 and R2
        @ManageTransactions("LocalOp")
        def MakeFilletR1R2(self, theShape, theR1, theR2, theShapeType, theListShapes, theName=None):
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
            if theShapeType == self.ShapeType["EDGE"]:
                anObj = self.LocalOp.MakeFilletEdgesR1R2(theShape, theR1, theR2, theListShapes)
                RaiseIfFailed("MakeFilletEdgesR1R2", self.LocalOp)
            else:
                anObj = self.LocalOp.MakeFilletFacesR1R2(theShape, theR1, theR2, theListShapes)
                RaiseIfFailed("MakeFilletFacesR1R2", self.LocalOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "fillet")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet2d "Example"
        @ManageTransactions("LocalOp")
        def MakeFillet1D(self, theShape, theR, theListOfVertexes, doIgnoreSecantVertices = True, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.
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
            self._autoPublish(anObj, theName, "fillet")
            return anObj

        ## Perform a fillet at the specified vertices of the given face/shell.
        #  @param theShape Face or Shell shape to perform fillet on.
        #  @param theR Fillet radius.
        #  @param theListOfVertexes Global indices of vertexes to perform fillet on.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note Global index of sub-shape can be obtained, using method GetSubShapeID().
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_fillet2d "Example"
        @ManageTransactions("LocalOp")
        def MakeFillet2D(self, theShape, theR, theListOfVertexes, theName=None):
            """
            Perform a fillet at the specified vertices of the given face/shell.

            Parameters:
                theShape  Face or Shell shape to perform fillet on.
                theR  Fillet radius.
                theListOfVertexes Global indices of vertexes to perform fillet on.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.
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
            self._autoPublish(anObj, theName, "fillet")
            return anObj

        ## Perform a symmetric chamfer on all edges of the given shape.
        #  @param theShape Shape, to perform chamfer on.
        #  @param theD Chamfer size along each face.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_chamfer "Example 1"
        #  \n @ref swig_MakeChamferAll "Example 2"
        @ManageTransactions("LocalOp")
        def MakeChamferAll(self, theShape, theD, theName=None):
            """
            Perform a symmetric chamfer on all edges of the given shape.

            Parameters:
                theShape Shape, to perform chamfer on.
                theD Chamfer size along each face.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "chamfer")
            return anObj

        ## Perform a chamfer on edges, common to the specified faces,
        #  with distance D1 on the Face1
        #  @param theShape Shape, to perform chamfer on.
        #  @param theD1 Chamfer size along \a theFace1.
        #  @param theD2 Chamfer size along \a theFace2.
        #  @param theFace1,theFace2 Global indices of two faces of \a theShape.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note Global index of sub-shape can be obtained, using method GetSubShapeID().
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_chamfer "Example"
        @ManageTransactions("LocalOp")
        def MakeChamferEdge(self, theShape, theD1, theD2, theFace1, theFace2, theName=None):
            """
            Perform a chamfer on edges, common to the specified faces,
            with distance D1 on the Face1

            Parameters:
                theShape Shape, to perform chamfer on.
                theD1 Chamfer size along theFace1.
                theD2 Chamfer size along theFace2.
                theFace1,theFace2 Global indices of two faces of theShape.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "chamfer")
            return anObj

        ## Perform a chamfer on edges
        #  @param theShape Shape, to perform chamfer on.
        #  @param theD Chamfer length
        #  @param theAngle Angle of chamfer (angle in radians or a name of variable which defines angle in degrees)
        #  @param theFace1,theFace2 Global indices of two faces of \a theShape.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note Global index of sub-shape can be obtained, using method GetSubShapeID().
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        @ManageTransactions("LocalOp")
        def MakeChamferEdgeAD(self, theShape, theD, theAngle, theFace1, theFace2, theName=None):
            """
            Perform a chamfer on edges

            Parameters:
                theShape Shape, to perform chamfer on.
                theD1 Chamfer size along theFace1.
                theAngle Angle of chamfer (angle in radians or a name of variable which defines angle in degrees).
                theFace1,theFace2 Global indices of two faces of theShape.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "chamfer")
            return anObj

        ## Perform a chamfer on all edges of the specified faces,
        #  with distance D1 on the first specified face (if several for one edge)
        #  @param theShape Shape, to perform chamfer on.
        #  @param theD1 Chamfer size along face from \a theFaces. If both faces,
        #               connected to the edge, are in \a theFaces, \a theD1
        #               will be get along face, which is nearer to \a theFaces beginning.
        #  @param theD2 Chamfer size along another of two faces, connected to the edge.
        #  @param theFaces Sequence of global indices of faces of \a theShape.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note Global index of sub-shape can be obtained, using method GetSubShapeID().
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_chamfer "Example"
        @ManageTransactions("LocalOp")
        def MakeChamferFaces(self, theShape, theD1, theD2, theFaces, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Note: Global index of sub-shape can be obtained, using method geompy.GetSubShapeID().

            Returns:
                New GEOM.GEOM_Object, containing the result shape.
            """
            # Example: see GEOM_TestAll.py
            theD1,theD2,Parameters = ParseParameters(theD1,theD2)
            anObj = self.LocalOp.MakeChamferFaces(theShape, theD1, theD2, theFaces)
            RaiseIfFailed("MakeChamferFaces", self.LocalOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "chamfer")
            return anObj

        ## The Same that MakeChamferFaces() but with params theD is chamfer lenght and
        #  theAngle is Angle of chamfer (angle in radians or a name of variable which defines angle in degrees)
        #
        #  @ref swig_FilletChamfer "Example"
        @ManageTransactions("LocalOp")
        def MakeChamferFacesAD(self, theShape, theD, theAngle, theFaces, theName=None):
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
            self._autoPublish(anObj, theName, "chamfer")
            return anObj

        ## Perform a chamfer on edges,
        #  with distance D1 on the first specified face (if several for one edge)
        #  @param theShape Shape, to perform chamfer on.
        #  @param theD1,theD2 Chamfer size
        #  @param theEdges Sequence of edges of \a theShape.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref swig_FilletChamfer "Example"
        @ManageTransactions("LocalOp")
        def MakeChamferEdges(self, theShape, theD1, theD2, theEdges, theName=None):
            """
            Perform a chamfer on edges,
            with distance D1 on the first specified face (if several for one edge)

            Parameters:
                theShape Shape, to perform chamfer on.
                theD1,theD2 Chamfer size
                theEdges Sequence of edges of theShape.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the result shape.
            """
            theD1,theD2,Parameters = ParseParameters(theD1,theD2)
            anObj = self.LocalOp.MakeChamferEdges(theShape, theD1, theD2, theEdges)
            RaiseIfFailed("MakeChamferEdges", self.LocalOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "chamfer")
            return anObj

        ## The Same that MakeChamferEdges() but with params theD is chamfer lenght and
        #  theAngle is Angle of chamfer (angle in radians or a name of variable which defines angle in degrees)
        @ManageTransactions("LocalOp")
        def MakeChamferEdgesAD(self, theShape, theD, theAngle, theEdges, theName=None):
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
            self._autoPublish(anObj, theName, "chamfer")
            return anObj

        ## @sa MakeChamferEdge(), MakeChamferFaces()
        #
        #  @ref swig_MakeChamfer "Example"
        def MakeChamfer(self, aShape, d1, d2, aShapeType, ListShape, theName=None):
            """
            See geompy.MakeChamferEdge() and geompy.MakeChamferFaces() functions for more information.
            """
            # Example: see GEOM_TestOthers.py
            anObj = None
            # note: auto-publishing is done in self.MakeChamferEdge() or self.MakeChamferFaces()
            if aShapeType == self.ShapeType["EDGE"]:
                anObj = self.MakeChamferEdge(aShape,d1,d2,ListShape[0],ListShape[1],theName)
            else:
                anObj = self.MakeChamferFaces(aShape,d1,d2,ListShape,theName)
            return anObj

        ## Remove material from a solid by extrusion of the base shape on the given distance.
        #  @param theInit Shape to remove material from. It must be a solid or
        #  a compound made of a single solid.
        #  @param theBase Closed edge or wire defining the base shape to be extruded.
        #  @param theH Prism dimension along the normal to theBase
        #  @param theAngle Draft angle in degrees.
        #  @param theInvert If true material changes the direction
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the initial shape with removed material
        #
        #  @ref tui_creation_prism "Example"
        @ManageTransactions("PrimOp")
        def MakeExtrudedCut(self, theInit, theBase, theH, theAngle, theInvert=False, theName=None):
            """
            Add material to a solid by extrusion of the base shape on the given distance.

            Parameters:
                theInit Shape to remove material from. It must be a solid or a compound made of a single solid.
                theBase Closed edge or wire defining the base shape to be extruded.
                theH Prism dimension along the normal  to theBase
                theAngle Draft angle in degrees.
                theInvert If true material changes the direction.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object,  containing the initial shape with removed material.
            """
            # Example: see GEOM_TestAll.py
            theH,theAngle,Parameters = ParseParameters(theH,theAngle)
            anObj = self.PrimOp.MakeDraftPrism(theInit, theBase, theH, theAngle, False, theInvert)
            RaiseIfFailed("MakeExtrudedBoss", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "extrudedCut")
            return anObj

        ## Add material to a solid by extrusion of the base shape on the given distance.
        #  @param theInit Shape to add material to. It must be a solid or
        #  a compound made of a single solid.
        #  @param theBase Closed edge or wire defining the base shape to be extruded.
        #  @param theH Prism dimension along the normal to theBase
        #  @param theAngle Draft angle in degrees.
        #  @param theInvert If true material changes the direction
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the initial shape with added material
        #
        #  @ref tui_creation_prism "Example"
        @ManageTransactions("PrimOp")
        def MakeExtrudedBoss(self, theInit, theBase, theH, theAngle, theInvert=False, theName=None):
            """
            Add material to a solid by extrusion of the base shape on the given distance.

            Parameters:
                theInit Shape to add material to. It must be a solid or a compound made of a single solid.
                theBase Closed edge or wire defining the base shape to be extruded.
                theH Prism dimension along the normal  to theBase
                theAngle Draft angle in degrees.
                theInvert If true material changes the direction.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object,  containing the initial shape with added material.
            """
            # Example: see GEOM_TestAll.py
            theH,theAngle,Parameters = ParseParameters(theH,theAngle)
            anObj = self.PrimOp.MakeDraftPrism(theInit, theBase, theH, theAngle, True, theInvert)
            RaiseIfFailed("MakeExtrudedBoss", self.PrimOp)
            anObj.SetParameters(Parameters)
            self._autoPublish(anObj, theName, "extrudedBoss")
            return anObj

        # end of l3_local
        ## @}

        ## @addtogroup l3_basic_op
        ## @{

        ## Perform an Archimde operation on the given shape with given parameters.
        #  The object presenting the resulting face is returned.
        #  @param theShape Shape to be put in water.
        #  @param theWeight Weight of the shape.
        #  @param theWaterDensity Density of the water.
        #  @param theMeshDeflection Deflection of the mesh, using to compute the section.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing a section of \a theShape
        #          by a plane, corresponding to water level.
        #
        #  @ref tui_archimede "Example"
        @ManageTransactions("LocalOp")
        def Archimede(self, theShape, theWeight, theWaterDensity, theMeshDeflection, theName=None):
            """
            Perform an Archimde operation on the given shape with given parameters.
            The object presenting the resulting face is returned.

            Parameters:
                theShape Shape to be put in water.
                theWeight Weight of the shape.
                theWaterDensity Density of the water.
                theMeshDeflection Deflection of the mesh, using to compute the section.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "archimede")
            return anObj

        # end of l3_basic_op
        ## @}

        ## @addtogroup l2_measure
        ## @{

        ## Get point coordinates
        #  @return [x, y, z]
        #
        #  @ref tui_point_coordinates_page "Example"
        @ManageTransactions("MeasuOp")
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

        ## Get vector coordinates
        #  @return [x, y, z]
        #
        #  @ref tui_measurement_tools_page "Example"
        def VectorCoordinates(self,Vector):
            """
            Get vector coordinates

            Returns:
                [x, y, z]
            """

            p1=self.GetFirstVertex(Vector)
            p2=self.GetLastVertex(Vector)

            X1=self.PointCoordinates(p1)
            X2=self.PointCoordinates(p2)

            return (X2[0]-X1[0],X2[1]-X1[1],X2[2]-X1[2])


        ## Compute cross product
        #  @return vector w=u^v
        #
        #  @ref tui_measurement_tools_page "Example"
        def CrossProduct(self, Vector1, Vector2):
            """
            Compute cross product

            Returns: vector w=u^v
            """
            u=self.VectorCoordinates(Vector1)
            v=self.VectorCoordinates(Vector2)
            w=self.MakeVectorDXDYDZ(u[1]*v[2]-u[2]*v[1], u[2]*v[0]-u[0]*v[2], u[0]*v[1]-u[1]*v[0])

            return w

        ## Compute cross product
        #  @return dot product  p=u.v
        #
        #  @ref tui_measurement_tools_page "Example"
        def DotProduct(self, Vector1, Vector2):
            """
            Compute cross product

            Returns: dot product  p=u.v
            """
            u=self.VectorCoordinates(Vector1)
            v=self.VectorCoordinates(Vector2)
            p=u[0]*v[0]+u[1]*v[1]+u[2]*v[2]

            return p


        ## Get summarized length of all wires,
        #  area of surface and volume of the given shape.
        #  @param theShape Shape to define properties of.
        #  @param theTolerance maximal relative error of area
        #         and volume computation.
        #  @return [theLength, theSurfArea, theVolume]\n
        #  theLength:   Summarized length of all wires of the given shape.\n
        #  theSurfArea: Area of surface of the given shape.\n
        #  theVolume:   Volume of the given shape.
        #
        #  @ref tui_basic_properties_page "Example"
        @ManageTransactions("MeasuOp")
        def BasicProperties(self,theShape, theTolerance=1.e-6):
            """
            Get summarized length of all wires,
            area of surface and volume of the given shape.

            Parameters:
                theShape Shape to define properties of.
                theTolerance maximal relative error of area
                             and volume computation.

            Returns:
                [theLength, theSurfArea, theVolume]
                 theLength:   Summarized length of all wires of the given shape.
                 theSurfArea: Area of surface of the given shape.
                 theVolume:   Volume of the given shape.
            """
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetBasicProperties(theShape, theTolerance)
            RaiseIfFailed("GetBasicProperties", self.MeasuOp)
            return aTuple

        ## Get parameters of bounding box of the given shape
        #  @param theShape Shape to obtain bounding box of.
        #  @param precise TRUE for precise computation; FALSE for fast one.
        #  @return [Xmin,Xmax, Ymin,Ymax, Zmin,Zmax]
        #  Xmin,Xmax: Limits of shape along OX axis.
        #  Ymin,Ymax: Limits of shape along OY axis.
        #  Zmin,Zmax: Limits of shape along OZ axis.
        #
        #  @ref tui_bounding_box_page "Example"
        @ManageTransactions("MeasuOp")
        def BoundingBox (self, theShape, precise=False):
            """
            Get parameters of bounding box of the given shape

            Parameters:
                theShape Shape to obtain bounding box of.
                precise TRUE for precise computation; FALSE for fast one.

            Returns:
                [Xmin,Xmax, Ymin,Ymax, Zmin,Zmax]
                 Xmin,Xmax: Limits of shape along OX axis.
                 Ymin,Ymax: Limits of shape along OY axis.
                 Zmin,Zmax: Limits of shape along OZ axis.
            """
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetBoundingBox(theShape, precise)
            RaiseIfFailed("GetBoundingBox", self.MeasuOp)
            return aTuple

        ## Get bounding box of the given shape
        #  @param theShape Shape to obtain bounding box of.
        #  @param precise TRUE for precise computation; FALSE for fast one.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created box.
        #
        #  @ref tui_bounding_box_page "Example"
        @ManageTransactions("MeasuOp")
        def MakeBoundingBox (self, theShape, precise=False, theName=None):
            """
            Get bounding box of the given shape

            Parameters:
                theShape Shape to obtain bounding box of.
                precise TRUE for precise computation; FALSE for fast one.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created box.
            """
            # Example: see GEOM_TestMeasures.py
            anObj = self.MeasuOp.MakeBoundingBox(theShape, precise)
            RaiseIfFailed("MakeBoundingBox", self.MeasuOp)
            self._autoPublish(anObj, theName, "bndbox")
            return anObj

        ## Get inertia matrix and moments of inertia of theShape.
        #  @param theShape Shape to calculate inertia of.
        #  @return [I11,I12,I13, I21,I22,I23, I31,I32,I33, Ix,Iy,Iz]
        #  I(1-3)(1-3): Components of the inertia matrix of the given shape.
        #  Ix,Iy,Iz:    Moments of inertia of the given shape.
        #
        #  @ref tui_inertia_page "Example"
        @ManageTransactions("MeasuOp")
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
        @ManageTransactions("MeasuOp")
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
        #  @ref tui_min_distance_page "Example"
        @ManageTransactions("MeasuOp")
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
        #  @return Value of the minimal distance between the given shapes, in form of list
        #          [Distance, DX, DY, DZ].
        #
        #  @ref tui_min_distance_page "Example"
        @ManageTransactions("MeasuOp")
        def MinDistanceComponents(self, theShape1, theShape2):
            """
            Get minimal distance between the given shapes.

            Parameters:
                theShape1,theShape2 Shapes to find minimal distance between.

            Returns:
                Value of the minimal distance between the given shapes, in form of list
                [Distance, DX, DY, DZ]
            """
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.GetMinDistance(theShape1, theShape2)
            RaiseIfFailed("GetMinDistance", self.MeasuOp)
            aRes = [aTuple[0], aTuple[4] - aTuple[1], aTuple[5] - aTuple[2], aTuple[6] - aTuple[3]]
            return aRes

        ## Get closest points of the given shapes.
        #  @param theShape1,theShape2 Shapes to find closest points of.
        #  @return The number of found solutions (-1 in case of infinite number of
        #          solutions) and a list of (X, Y, Z) coordinates for all couples of points.
        #
        #  @ref tui_min_distance_page "Example"
        @ManageTransactions("MeasuOp")
        def ClosestPoints (self, theShape1, theShape2):
            """
            Get closest points of the given shapes.

            Parameters:
                theShape1,theShape2 Shapes to find closest points of.

            Returns:
                The number of found solutions (-1 in case of infinite number of
                solutions) and a list of (X, Y, Z) coordinates for all couples of points.
            """
            # Example: see GEOM_TestMeasures.py
            aTuple = self.MeasuOp.ClosestPoints(theShape1, theShape2)
            RaiseIfFailed("ClosestPoints", self.MeasuOp)
            return aTuple

        ## Get angle between the given shapes in degrees.
        #  @param theShape1,theShape2 Lines or linear edges to find angle between.
        #  @note If both arguments are vectors, the angle is computed in accordance
        #        with their orientations, otherwise the minimum angle is computed.
        #  @return Value of the angle between the given shapes in degrees.
        #
        #  @ref tui_angle_page "Example"
        @ManageTransactions("MeasuOp")
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
        #  @ref tui_angle_page "Example"
        @ManageTransactions("MeasuOp")
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
        #  @ref tui_angle_page "Example"
        @ManageTransactions("MeasuOp")
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
        @ManageTransactions("MeasuOp")
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
        @ManageTransactions("MeasuOp")
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
        @ManageTransactions("MeasuOp")
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
        @ManageTransactions("MeasuOp")
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
        @ManageTransactions("MeasuOp")
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
        @ManageTransactions("MeasuOp")
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
        #  @ref tui_tolerance_page "Example"
        @ManageTransactions("MeasuOp")
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
        #  @ref tui_whatis_page "Example"
        @ManageTransactions("MeasuOp")
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
            for typeSh in self.ShapeType:
                if typeSh in ( "AUTO", "SHAPE" ): continue
                listSh = self.SubShapeAllIDs(theShape, self.ShapeType[typeSh])
                Nb = len(listSh)
                aDict[typeSh] = Nb
                pass
            return aDict

        def GetCreationInformation(self, theShape):
            res = ''
            infos = theShape.GetCreationInformation()
            for info in infos:
                # operationName
                opName = info.operationName
                if not opName: opName = "no info available"
                if res: res += "\n"
                res += "Operation: " + opName
                # parameters
                for parVal in info.params:
                    res += "\n \t%s = %s" % ( parVal.name, parVal.value )
            return res

        ## Get a point, situated at the centre of mass of theShape.
        #  @param theShape Shape to define centre of mass of.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created point.
        #
        #  @ref tui_center_of_mass_page "Example"
        @ManageTransactions("MeasuOp")
        def MakeCDG(self, theShape, theName=None):
            """
            Get a point, situated at the centre of mass of theShape.

            Parameters:
                theShape Shape to define centre of mass of.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created point.
            """
            # Example: see GEOM_TestMeasures.py
            anObj = self.MeasuOp.GetCentreOfMass(theShape)
            RaiseIfFailed("GetCentreOfMass", self.MeasuOp)
            self._autoPublish(anObj, theName, "centerOfMass")
            return anObj

        ## Get a vertex sub-shape by index depended with orientation.
        #  @param theShape Shape to find sub-shape.
        #  @param theIndex Index to find vertex by this index (starting from zero)
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created vertex.
        #
        #  @ref tui_measurement_tools_page "Example"
        @ManageTransactions("MeasuOp")
        def GetVertexByIndex(self, theShape, theIndex, theName=None):
            """
            Get a vertex sub-shape by index depended with orientation.

            Parameters:
                theShape Shape to find sub-shape.
                theIndex Index to find vertex by this index (starting from zero)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created vertex.
            """
            # Example: see GEOM_TestMeasures.py
            anObj = self.MeasuOp.GetVertexByIndex(theShape, theIndex)
            RaiseIfFailed("GetVertexByIndex", self.MeasuOp)
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Get the first vertex of wire/edge depended orientation.
        #  @param theShape Shape to find first vertex.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created vertex.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetFirstVertex(self, theShape, theName=None):
            """
            Get the first vertex of wire/edge depended orientation.

            Parameters:
                theShape Shape to find first vertex.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created vertex.
            """
            # Example: see GEOM_TestMeasures.py
            # note: auto-publishing is done in self.GetVertexByIndex()
            return self.GetVertexByIndex(theShape, 0, theName)

        ## Get the last vertex of wire/edge depended orientation.
        #  @param theShape Shape to find last vertex.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created vertex.
        #
        #  @ref tui_measurement_tools_page "Example"
        def GetLastVertex(self, theShape, theName=None):
            """
            Get the last vertex of wire/edge depended orientation.

            Parameters:
                theShape Shape to find last vertex.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created vertex.
            """
            # Example: see GEOM_TestMeasures.py
            nb_vert =  self.NumberOfSubShapes(theShape, self.ShapeType["VERTEX"])
            # note: auto-publishing is done in self.GetVertexByIndex()
            return self.GetVertexByIndex(theShape, (nb_vert-1), theName)

        ## Get a normale to the given face. If the point is not given,
        #  the normale is calculated at the center of mass.
        #  @param theFace Face to define normale of.
        #  @param theOptionalPoint Point to compute the normale at.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created vector.
        #
        #  @ref swig_todo "Example"
        @ManageTransactions("MeasuOp")
        def GetNormal(self, theFace, theOptionalPoint = None, theName=None):
            """
            Get a normale to the given face. If the point is not given,
            the normale is calculated at the center of mass.

            Parameters:
                theFace Face to define normale of.
                theOptionalPoint Point to compute the normale at.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created vector.
            """
            # Example: see GEOM_TestMeasures.py
            anObj = self.MeasuOp.GetNormal(theFace, theOptionalPoint)
            RaiseIfFailed("GetNormal", self.MeasuOp)
            self._autoPublish(anObj, theName, "normal")
            return anObj

        ## Print shape errors obtained from CheckShape.
        #  @param theShape Shape that was checked.
        #  @param theShapeErrors the shape errors obtained by CheckShape.
        #  @param theReturnStatus If 0 the description of problem is printed.
        #                         If 1 the description of problem is returned.
        #  @return If theReturnStatus is equal to 1 the description is returned.
        #          Otherwise doesn't return anything.
        #
        #  @ref tui_check_shape_page "Example"
        @ManageTransactions("MeasuOp")
        def PrintShapeErrors(self, theShape, theShapeErrors, theReturnStatus = 0):
            """
            Print shape errors obtained from CheckShape.

            Parameters:
                theShape Shape that was checked.
                theShapeErrors the shape errors obtained by CheckShape.
                theReturnStatus If 0 the description of problem is printed.
                                If 1 the description of problem is returned.

            Returns:
                If theReturnStatus is equal to 1 the description is returned.
                  Otherwise doesn't return anything.
            """
            # Example: see GEOM_TestMeasures.py
            Descr = self.MeasuOp.PrintShapeErrors(theShape, theShapeErrors)
            if theReturnStatus == 1:
                return Descr
            print Descr
            pass

        ## Check a topology of the given shape.
        #  @param theShape Shape to check validity of.
        #  @param theIsCheckGeom If FALSE, only the shape's topology will be checked, \n
        #                        if TRUE, the shape's geometry will be checked also.
        #  @param theReturnStatus If 0 and if theShape is invalid, a description
        #                         of problem is printed.
        #                         If 1 isValid flag and the description of
        #                         problem is returned.
        #                         If 2 isValid flag and the list of error data
        #                         is returned.
        #  @return TRUE, if the shape "seems to be valid".
        #          If theShape is invalid, prints a description of problem.
        #          If theReturnStatus is equal to 1 the description is returned
        #          along with IsValid flag.
        #          If theReturnStatus is equal to 2 the list of error data is
        #          returned along with IsValid flag.
        #
        #  @ref tui_check_shape_page "Example"
        @ManageTransactions("MeasuOp")
        def CheckShape(self,theShape, theIsCheckGeom = 0, theReturnStatus = 0):
            """
            Check a topology of the given shape.

            Parameters:
                theShape Shape to check validity of.
                theIsCheckGeom If FALSE, only the shape's topology will be checked,
                               if TRUE, the shape's geometry will be checked also.
                theReturnStatus If 0 and if theShape is invalid, a description
                                of problem is printed.
                                If 1 IsValid flag and the description of
                                problem is returned.
                                If 2 IsValid flag and the list of error data
                                is returned.

            Returns:
                TRUE, if the shape "seems to be valid".
                If theShape is invalid, prints a description of problem.
                If theReturnStatus is equal to 1 the description is returned
                along with IsValid flag.
                If theReturnStatus is equal to 2 the list of error data is
                returned along with IsValid flag.
            """
            # Example: see GEOM_TestMeasures.py
            if theIsCheckGeom:
                (IsValid, ShapeErrors) = self.MeasuOp.CheckShapeWithGeometry(theShape)
                RaiseIfFailed("CheckShapeWithGeometry", self.MeasuOp)
            else:
                (IsValid, ShapeErrors) = self.MeasuOp.CheckShape(theShape)
                RaiseIfFailed("CheckShape", self.MeasuOp)
            if IsValid == 0:
                if theReturnStatus == 0:
                    Descr = self.MeasuOp.PrintShapeErrors(theShape, ShapeErrors)
                    print Descr
            if theReturnStatus == 1:
              Descr = self.MeasuOp.PrintShapeErrors(theShape, ShapeErrors)
              return (IsValid, Descr)
            elif theReturnStatus == 2:
              return (IsValid, ShapeErrors)
            return IsValid

        ## Detect self-intersections in the given shape.
        #  @param theShape Shape to check.
        #  @param theCheckLevel is the level of self-intersection check.
        #         Possible input values are:
        #         - GEOM.SI_V_V(0) - only V/V interferences
        #         - GEOM.SI_V_E(1) - V/V and V/E interferences
        #         - GEOM.SI_E_E(2) - V/V, V/E and E/E interferences
        #         - GEOM.SI_V_F(3) - V/V, V/E, E/E and V/F interferences
        #         - GEOM.SI_E_F(4) - V/V, V/E, E/E, V/F and E/F interferences
        #         - GEOM.SI_ALL(5) - all interferences.
        #  @return TRUE, if the shape contains no self-intersections.
        #
        #  @ref tui_check_self_intersections_page "Example"
        @ManageTransactions("MeasuOp")
        def CheckSelfIntersections(self, theShape, theCheckLevel = GEOM.SI_ALL):
            """
            Detect self-intersections in the given shape.

            Parameters:
                theShape Shape to check.
                theCheckLevel is the level of self-intersection check.
                  Possible input values are:
                   - GEOM.SI_V_V(0) - only V/V interferences
                   - GEOM.SI_V_E(1) - V/V and V/E interferences
                   - GEOM.SI_E_E(2) - V/V, V/E and E/E interferences
                   - GEOM.SI_V_F(3) - V/V, V/E, E/E and V/F interferences
                   - GEOM.SI_E_F(4) - V/V, V/E, E/E, V/F and E/F interferences
                   - GEOM.SI_ALL(5) - all interferences.
 
            Returns:
                TRUE, if the shape contains no self-intersections.
            """
            # Example: see GEOM_TestMeasures.py
            (IsValid, Pairs) = self.MeasuOp.CheckSelfIntersections(theShape, EnumToLong(theCheckLevel))
            RaiseIfFailed("CheckSelfIntersections", self.MeasuOp)
            return IsValid

        ## Detect self-intersections of the given shape with algorithm based on mesh intersections.
        #  @param theShape Shape to check.
        #  @param theDeflection Linear deflection coefficient that specifies quality of tesselation:
        #         - if \a theDeflection <= 0, default deflection 0.001 is used
        #  @param theTolerance Specifies a distance between sub-shapes used for detecting gaps:
        #         - if \a theTolerance <= 0, algorithm detects intersections (default behavior)
        #         - if \a theTolerance > 0, algorithm detects gaps
        #  @return TRUE, if the shape contains no self-intersections.
        #
        #  @ref tui_check_self_intersections_fast_page "Example"
        @ManageTransactions("MeasuOp")
        def CheckSelfIntersectionsFast(self, theShape, theDeflection = 0.001, theTolerance = 0.0):
            """
            Detect self-intersections of the given shape with algorithm based on mesh intersections.

            Parameters:
                theShape Shape to check.
                theDeflection Linear deflection coefficient that specifies quality of tesselation:
                    - if theDeflection <= 0, default deflection 0.001 is used
                theTolerance Specifies a distance between shapes used for detecting gaps:
                    - if theTolerance <= 0, algorithm detects intersections (default behavior)
                    - if theTolerance > 0, algorithm detects gaps
 
            Returns:
                TRUE, if the shape contains no self-intersections.
            """
            # Example: see GEOM_TestMeasures.py
            (IsValid, Pairs) = self.MeasuOp.CheckSelfIntersectionsFast(theShape, theDeflection, theTolerance)
            RaiseIfFailed("CheckSelfIntersectionsFast", self.MeasuOp)
            return IsValid

        ## Check boolean and partition operations agruments.
        #  @param theShape the agrument of an operation to be checked
        #  @return TRUE if the agrument is valid for a boolean or partition
        #          operation; FALSE otherwise.
        @ManageTransactions("MeasuOp")
        def CheckBOPArguments(self, theShape):
            """
            Check boolean and partition operations agruments.

            Parameters:
                theShape the agrument of an operation to be checked

            Returns:
                TRUE if the agrument is valid for a boolean or partition
                operation; FALSE otherwise.
            """
            return self.MeasuOp.CheckBOPArguments(theShape)

        ## Detect intersections of the given shapes with algorithm based on mesh intersections.
        #  @param theShape1 First source object
        #  @param theShape2 Second source object
        #  @param theTolerance Specifies a distance between shapes used for detecting gaps:
        #         - if \a theTolerance <= 0, algorithm detects intersections (default behavior)
        #         - if \a theTolerance > 0, algorithm detects gaps
        #  @param theDeflection Linear deflection coefficient that specifies quality of tesselation:
        #         - if \a theDeflection <= 0, default deflection 0.001 is used
        #  @return TRUE, if there are intersections (gaps) between source shapes
        #  @return List of sub-shapes IDs from 1st shape that localize intersection.
        #  @return List of sub-shapes IDs from 2nd shape that localize intersection.
        #
        #  @ref tui_fast_intersection_page "Example"
        @ManageTransactions("MeasuOp")
        def FastIntersect(self, theShape1, theShape2, theTolerance = 0.0, theDeflection = 0.001):
            """
            Detect intersections of the given shapes with algorithm based on mesh intersections.

            Parameters:
                theShape1 First source object
                theShape2 Second source object
                theTolerance Specifies a distance between shapes used for detecting gaps:
                    - if theTolerance <= 0, algorithm detects intersections (default behavior)
                    - if theTolerance > 0, algorithm detects gaps
                theDeflection Linear deflection coefficient that specifies quality of tesselation:
                    - if theDeflection <= 0, default deflection 0.001 is used
 
            Returns:
                TRUE, if there are intersections (gaps) between source shapes
                List of sub-shapes IDs from 1st shape that localize intersection.
                List of sub-shapes IDs from 2nd shape that localize intersection.
            """
            # Example: see GEOM_TestMeasures.py
            IsOk, Res1, Res2 = self.MeasuOp.FastIntersect(theShape1, theShape2, theTolerance, theDeflection)
            RaiseIfFailed("FastIntersect", self.MeasuOp)
            return IsOk, Res1, Res2

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
        @ManageTransactions("MeasuOp")
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
        @ManageTransactions("MeasuOp")
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

        ## Returns the string that describes if the shell is good for solid.
        #  This is a support method for MakeSolid.
        #
        #  @param theShell the shell to be checked.
        #  @return Returns a string that describes the shell validity for
        #          solid construction.
        @ManageTransactions("MeasuOp")
        def _IsGoodForSolid(self, theShell):
            """
            Returns the string that describes if the shell is good for solid.
            This is a support method for MakeSolid.

            Parameter:
                theShell the shell to be checked.

            Returns:
                Returns a string that describes the shell validity for
                solid construction.
            """
            aDescr = self.MeasuOp.IsGoodForSolid(theShell)
            return aDescr

        # end of l2_measure
        ## @}

        ## @addtogroup l2_import_export
        ## @{

        ## Import a shape from the BREP, IGES, STEP or other file
        #  (depends on given format) with given name.
        #
        #  Note: this function is deprecated, it is kept for backward compatibility only
        #  Use Import<FormatName> instead, where <FormatName> is a name of desirable format to import.
        #
        #  @param theFileName The file, containing the shape.
        #  @param theFormatName Specify format for the file reading.
        #         Available formats can be obtained with InsertOp.ImportTranslators() method.
        #         If format 'IGES_SCALE' is used instead of 'IGES' or
        #            format 'STEP_SCALE' is used instead of 'STEP',
        #            length unit will be set to 'meter' and result model will be scaled.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the imported shape.
        #          If material names are imported it returns the list of
        #          objects. The first one is the imported object followed by
        #          material groups.
        #  @note Auto publishing is allowed for the shape itself. Imported
        #        material groups are not automatically published.
        #
        #  @ref swig_Import_Export "Example"
        @ManageTransactions("InsertOp")
        def ImportFile(self, theFileName, theFormatName, theName=None):
            """
            Import a shape from the BREP, IGES, STEP or other file
            (depends on given format) with given name.

            Note: this function is deprecated, it is kept for backward compatibility only
            Use Import<FormatName> instead, where <FormatName> is a name of desirable format to import.

            Parameters: 
                theFileName The file, containing the shape.
                theFormatName Specify format for the file reading.
                    Available formats can be obtained with geompy.InsertOp.ImportTranslators() method.
                    If format 'IGES_SCALE' is used instead of 'IGES' or
                       format 'STEP_SCALE' is used instead of 'STEP',
                       length unit will be set to 'meter' and result model will be scaled.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the imported shape.
                If material names are imported it returns the list of
                objects. The first one is the imported object followed by
                material groups.
            Note:
                Auto publishing is allowed for the shape itself. Imported
                material groups are not automatically published.
            """
            # Example: see GEOM_TestOthers.py
            print """
            WARNING: Function ImportFile is deprecated, use Import<FormatName> instead,
            where <FormatName> is a name of desirable format for importing.
            """
            aListObj = self.InsertOp.ImportFile(theFileName, theFormatName)
            RaiseIfFailed("ImportFile", self.InsertOp)
            aNbObj = len(aListObj)
            if aNbObj > 0:
                self._autoPublish(aListObj[0], theName, "imported")
            if aNbObj == 1:
                return aListObj[0]
            return aListObj

        ## Deprecated analog of ImportFile()
        def Import(self, theFileName, theFormatName, theName=None):
            """
            Deprecated analog of geompy.ImportFile, kept for backward compatibility only.
            """
            # note: auto-publishing is done in self.ImportFile()
            return self.ImportFile(theFileName, theFormatName, theName)

        ## Read a shape from the binary stream, containing its bounding representation (BRep).
        #  @note This method will not be dumped to the python script by DumpStudy functionality.
        #  @note GEOM.GEOM_Object.GetShapeStream() method can be used to obtain the shape's BRep stream.
        #  @param theStream The BRep binary stream.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM_Object, containing the shape, read from theStream.
        #
        #  @ref swig_Import_Export "Example"
        @ManageTransactions("InsertOp")
        def RestoreShape (self, theStream, theName=None):
            """
            Read a shape from the binary stream, containing its bounding representation (BRep).

            Note:
                shape.GetShapeStream() method can be used to obtain the shape's BRep stream.

            Parameters:
                theStream The BRep binary stream.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM_Object, containing the shape, read from theStream.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.InsertOp.RestoreShape(theStream)
            RaiseIfFailed("RestoreShape", self.InsertOp)
            self._autoPublish(anObj, theName, "restored")
            return anObj

        ## Export the given shape into a file with given name.
        #
        #  Note: this function is deprecated, it is kept for backward compatibility only
        #  Use Export<FormatName> instead, where <FormatName> is a name of desirable format to export.
        #
        #  @param theObject Shape to be stored in the file.
        #  @param theFileName Name of the file to store the given shape in.
        #  @param theFormatName Specify format for the shape storage.
        #         Available formats can be obtained with
        #         geompy.InsertOp.ExportTranslators()[0] method.
        #
        #  @ref swig_Import_Export "Example"
        @ManageTransactions("InsertOp")
        def Export(self, theObject, theFileName, theFormatName):
            """
            Export the given shape into a file with given name.

            Note: this function is deprecated, it is kept for backward compatibility only
            Use Export<FormatName> instead, where <FormatName> is a name of desirable format to export.
            
            Parameters: 
                theObject Shape to be stored in the file.
                theFileName Name of the file to store the given shape in.
                theFormatName Specify format for the shape storage.
                              Available formats can be obtained with
                              geompy.InsertOp.ExportTranslators()[0] method.
            """
            # Example: see GEOM_TestOthers.py
            print """
            WARNING: Function Export is deprecated, use Export<FormatName> instead,
            where <FormatName> is a name of desirable format for exporting.
            """
            self.InsertOp.Export(theObject, theFileName, theFormatName)
            if self.InsertOp.IsDone() == 0:
                raise RuntimeError,  "Export : " + self.InsertOp.GetErrorCode()
                pass
            pass

        # end of l2_import_export
        ## @}

        ## @addtogroup l3_blocks
        ## @{

        ## Create a quadrangle face from four edges. Order of Edges is not
        #  important. It is  not necessary that edges share the same vertex.
        #  @param E1,E2,E3,E4 Edges for the face bound.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_building_by_blocks_page "Example"
        @ManageTransactions("BlocksOp")
        def MakeQuad(self, E1, E2, E3, E4, theName=None):
            """
            Create a quadrangle face from four edges. Order of Edges is not
            important. It is  not necessary that edges share the same vertex.

            Parameters:
                E1,E2,E3,E4 Edges for the face bound.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created face.

            Example of usage:
                qface1 = geompy.MakeQuad(edge1, edge2, edge3, edge4)
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeQuad(E1, E2, E3, E4)
            RaiseIfFailed("MakeQuad", self.BlocksOp)
            self._autoPublish(anObj, theName, "quad")
            return anObj

        ## Create a quadrangle face on two edges.
        #  The missing edges will be built by creating the shortest ones.
        #  @param E1,E2 Two opposite edges for the face.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_building_by_blocks_page "Example"
        @ManageTransactions("BlocksOp")
        def MakeQuad2Edges(self, E1, E2, theName=None):
            """
            Create a quadrangle face on two edges.
            The missing edges will be built by creating the shortest ones.

            Parameters:
                E1,E2 Two opposite edges for the face.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "quad")
            return anObj

        ## Create a quadrangle face with specified corners.
        #  The missing edges will be built by creating the shortest ones.
        #  @param V1,V2,V3,V4 Corner vertices for the face.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created face.
        #
        #  @ref tui_building_by_blocks_page "Example 1"
        #  \n @ref swig_MakeQuad4Vertices "Example 2"
        @ManageTransactions("BlocksOp")
        def MakeQuad4Vertices(self, V1, V2, V3, V4, theName=None):
            """
            Create a quadrangle face with specified corners.
            The missing edges will be built by creating the shortest ones.

            Parameters:
                V1,V2,V3,V4 Corner vertices for the face.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "quad")
            return anObj

        ## Create a hexahedral solid, bounded by the six given faces. Order of
        #  faces is not important. It is  not necessary that Faces share the same edge.
        #  @param F1,F2,F3,F4,F5,F6 Faces for the hexahedral solid.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created solid.
        #
        #  @ref tui_building_by_blocks_page "Example 1"
        #  \n @ref swig_MakeHexa "Example 2"
        @ManageTransactions("BlocksOp")
        def MakeHexa(self, F1, F2, F3, F4, F5, F6, theName=None):
            """
            Create a hexahedral solid, bounded by the six given faces. Order of
            faces is not important. It is  not necessary that Faces share the same edge.

            Parameters:
                F1,F2,F3,F4,F5,F6 Faces for the hexahedral solid.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created solid.

            Example of usage:
                solid = geompy.MakeHexa(qface1, qface2, qface3, qface4, qface5, qface6)
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeHexa(F1, F2, F3, F4, F5, F6)
            RaiseIfFailed("MakeHexa", self.BlocksOp)
            self._autoPublish(anObj, theName, "hexa")
            return anObj

        ## Create a hexahedral solid between two given faces.
        #  The missing faces will be built by creating the smallest ones.
        #  @param F1,F2 Two opposite faces for the hexahedral solid.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the created solid.
        #
        #  @ref tui_building_by_blocks_page "Example 1"
        #  \n @ref swig_MakeHexa2Faces "Example 2"
        @ManageTransactions("BlocksOp")
        def MakeHexa2Faces(self, F1, F2, theName=None):
            """
            Create a hexahedral solid between two given faces.
            The missing faces will be built by creating the smallest ones.

            Parameters:
                F1,F2 Two opposite faces for the hexahedral solid.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the created solid.

            Example of usage:
                solid1 = geompy.MakeHexa2Faces(qface1, qface2)
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.MakeHexa2Faces(F1, F2)
            RaiseIfFailed("MakeHexa2Faces", self.BlocksOp)
            self._autoPublish(anObj, theName, "hexa")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the found vertex.
        #
        #  @ref swig_GetPoint "Example"
        @ManageTransactions("BlocksOp")
        def GetPoint(self, theShape, theX, theY, theZ, theEpsilon, theName=None):
            """
            Get a vertex, found in the given shape by its coordinates.

            Parameters:
                theShape Block or a compound of blocks.
                theX,theY,theZ Coordinates of the sought vertex.
                theEpsilon Maximum allowed distance between the resulting
                           vertex and point with the given coordinates.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the found vertex.

            Example of usage:
                pnt = geompy.GetPoint(shape, -50,  50,  50, 0.01)
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetPoint(theShape, theX, theY, theZ, theEpsilon)
            RaiseIfFailed("GetPoint", self.BlocksOp)
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Find a vertex of the given shape, which has minimal distance to the given point.
        #  @param theShape Any shape.
        #  @param thePoint Point, close to the desired vertex.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the found vertex.
        #
        #  @ref swig_GetVertexNearPoint "Example"
        @ManageTransactions("BlocksOp")
        def GetVertexNearPoint(self, theShape, thePoint, theName=None):
            """
            Find a vertex of the given shape, which has minimal distance to the given point.

            Parameters:
                theShape Any shape.
                thePoint Point, close to the desired vertex.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the found vertex.

            Example of usage:
                pmidle = geompy.MakeVertex(50, 0, 50)
                edge1 = geompy.GetEdgeNearPoint(blocksComp, pmidle)
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetVertexNearPoint(theShape, thePoint)
            RaiseIfFailed("GetVertexNearPoint", self.BlocksOp)
            self._autoPublish(anObj, theName, "vertex")
            return anObj

        ## Get an edge, found in the given shape by two given vertices.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint1,thePoint2 Points, close to the ends of the desired edge.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the found edge.
        #
        #  @ref swig_GetEdge "Example"
        @ManageTransactions("BlocksOp")
        def GetEdge(self, theShape, thePoint1, thePoint2, theName=None):
            """
            Get an edge, found in the given shape by two given vertices.

            Parameters:
                theShape Block or a compound of blocks.
                thePoint1,thePoint2 Points, close to the ends of the desired edge.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the found edge.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetEdge(theShape, thePoint1, thePoint2)
            RaiseIfFailed("GetEdge", self.BlocksOp)
            self._autoPublish(anObj, theName, "edge")
            return anObj

        ## Find an edge of the given shape, which has minimal distance to the given point.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint Point, close to the desired edge.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the found edge.
        #
        #  @ref swig_GetEdgeNearPoint "Example"
        @ManageTransactions("BlocksOp")
        def GetEdgeNearPoint(self, theShape, thePoint, theName=None):
            """
            Find an edge of the given shape, which has minimal distance to the given point.

            Parameters:
                theShape Block or a compound of blocks.
                thePoint Point, close to the desired edge.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the found edge.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetEdgeNearPoint(theShape, thePoint)
            RaiseIfFailed("GetEdgeNearPoint", self.BlocksOp)
            self._autoPublish(anObj, theName, "edge")
            return anObj

        ## Returns a face, found in the given shape by four given corner vertices.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint1,thePoint2,thePoint3,thePoint4 Points, close to the corners of the desired face.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the found face.
        #
        #  @ref swig_todo "Example"
        @ManageTransactions("BlocksOp")
        def GetFaceByPoints(self, theShape, thePoint1, thePoint2, thePoint3, thePoint4, theName=None):
            """
            Returns a face, found in the given shape by four given corner vertices.

            Parameters:
                theShape Block or a compound of blocks.
                thePoint1,thePoint2,thePoint3,thePoint4 Points, close to the corners of the desired face.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the found face.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceByPoints(theShape, thePoint1, thePoint2, thePoint3, thePoint4)
            RaiseIfFailed("GetFaceByPoints", self.BlocksOp)
            self._autoPublish(anObj, theName, "face")
            return anObj

        ## Get a face of block, found in the given shape by two given edges.
        #  @param theShape Block or a compound of blocks.
        #  @param theEdge1,theEdge2 Edges, close to the edges of the desired face.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the found face.
        #
        #  @ref swig_todo "Example"
        @ManageTransactions("BlocksOp")
        def GetFaceByEdges(self, theShape, theEdge1, theEdge2, theName=None):
            """
            Get a face of block, found in the given shape by two given edges.

            Parameters:
                theShape Block or a compound of blocks.
                theEdge1,theEdge2 Edges, close to the edges of the desired face.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the found face.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceByEdges(theShape, theEdge1, theEdge2)
            RaiseIfFailed("GetFaceByEdges", self.BlocksOp)
            self._autoPublish(anObj, theName, "face")
            return anObj

        ## Find a face, opposite to the given one in the given block.
        #  @param theBlock Must be a hexahedral solid.
        #  @param theFace Face of \a theBlock, opposite to the desired face.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the found face.
        #
        #  @ref swig_GetOppositeFace "Example"
        @ManageTransactions("BlocksOp")
        def GetOppositeFace(self, theBlock, theFace, theName=None):
            """
            Find a face, opposite to the given one in the given block.

            Parameters:
                theBlock Must be a hexahedral solid.
                theFace Face of theBlock, opposite to the desired face.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the found face.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetOppositeFace(theBlock, theFace)
            RaiseIfFailed("GetOppositeFace", self.BlocksOp)
            self._autoPublish(anObj, theName, "face")
            return anObj

        ## Find a face of the given shape, which has minimal distance to the given point.
        #  @param theShape Block or a compound of blocks.
        #  @param thePoint Point, close to the desired face.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the found face.
        #
        #  @ref swig_GetFaceNearPoint "Example"
        @ManageTransactions("BlocksOp")
        def GetFaceNearPoint(self, theShape, thePoint, theName=None):
            """
            Find a face of the given shape, which has minimal distance to the given point.

            Parameters:
                theShape Block or a compound of blocks.
                thePoint Point, close to the desired face.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the found face.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceNearPoint(theShape, thePoint)
            RaiseIfFailed("GetFaceNearPoint", self.BlocksOp)
            self._autoPublish(anObj, theName, "face")
            return anObj

        ## Find a face of block, whose outside normale has minimal angle with the given vector.
        #  @param theBlock Block or a compound of blocks.
        #  @param theVector Vector, close to the normale of the desired face.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the found face.
        #
        #  @ref swig_todo "Example"
        @ManageTransactions("BlocksOp")
        def GetFaceByNormale(self, theBlock, theVector, theName=None):
            """
            Find a face of block, whose outside normale has minimal angle with the given vector.

            Parameters:
                theBlock Block or a compound of blocks.
                theVector Vector, close to the normale of the desired face.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the found face.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetFaceByNormale(theBlock, theVector)
            RaiseIfFailed("GetFaceByNormale", self.BlocksOp)
            self._autoPublish(anObj, theName, "face")
            return anObj

        ## Find all sub-shapes of type \a theShapeType of the given shape,
        #  which have minimal distance to the given point.
        #  @param theShape Any shape.
        #  @param thePoint Point, close to the desired shape.
        #  @param theShapeType Defines what kind of sub-shapes is searched GEOM::shape_type
        #  @param theTolerance The tolerance for distances comparison. All shapes
        #                      with distances to the given point in interval
        #                      [minimal_distance, minimal_distance + theTolerance] will be gathered.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM_Object, containing a group of all found shapes.
        #
        #  @ref swig_GetShapesNearPoint "Example"
        @ManageTransactions("BlocksOp")
        def GetShapesNearPoint(self, theShape, thePoint, theShapeType, theTolerance = 1e-07, theName=None):
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
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM_Object, containing a group of all found shapes.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetShapesNearPoint(theShape, thePoint, theShapeType, theTolerance)
            RaiseIfFailed("GetShapesNearPoint", self.BlocksOp)
            self._autoPublish(anObj, theName, "group")
            return anObj

        # end of l3_blocks_op
        ## @}

        ## @addtogroup l4_blocks_measure
        ## @{

        ## Check, if the compound of blocks is given.
        #  To be considered as a compound of blocks, the
        #  given shape must satisfy the following conditions:
        #  - Each element of the compound should be a Block (6 faces).
        #  - Each face should be a quadrangle, i.e. it should have only 1 wire
        #       with 4 edges. If <VAR>theIsUseC1</VAR> is set to True and
        #       there are more than 4 edges in the only wire of a face,
        #       this face is considered to be quadrangle if it has 4 bounds
        #       (1 or more edge) of C1 continuity.
        #  - A connection between two Blocks should be an entire quadrangle face or an entire edge.
        #  - The compound should be connexe.
        #  - The glue between two quadrangle faces should be applied.
        #  @param theCompound The compound to check.
        #  @param theIsUseC1 Flag to check if there are 4 bounds on a face
        #         taking into account C1 continuity.
        #  @param theAngTolerance the angular tolerance to check if two neighbor
        #         edges are codirectional in the common vertex with this
        #         tolerance. This parameter is used only if
        #         <VAR>theIsUseC1</VAR> is set to True.
        #  @return TRUE, if the given shape is a compound of blocks.
        #  If theCompound is not valid, prints all discovered errors.
        #
        #  @ref tui_check_compound_of_blocks_page "Example 1"
        #  \n @ref swig_CheckCompoundOfBlocks "Example 2"
        @ManageTransactions("BlocksOp")
        def CheckCompoundOfBlocks(self,theCompound, theIsUseC1 = False,
                                  theAngTolerance = 1.e-12):
            """
            Check, if the compound of blocks is given.
            To be considered as a compound of blocks, the
            given shape must satisfy the following conditions:
            - Each element of the compound should be a Block (6 faces).
            - Each face should be a quadrangle, i.e. it should have only 1 wire
                 with 4 edges. If theIsUseC1 is set to True and
                 there are more than 4 edges in the only wire of a face,
                 this face is considered to be quadrangle if it has 4 bounds
                 (1 or more edge) of C1 continuity.
            - A connection between two Blocks should be an entire quadrangle face or an entire edge.
            - The compound should be connexe.
            - The glue between two quadrangle faces should be applied.

            Parameters:
                theCompound The compound to check.
                theIsUseC1 Flag to check if there are 4 bounds on a face
                           taking into account C1 continuity.
                theAngTolerance the angular tolerance to check if two neighbor
                           edges are codirectional in the common vertex with this
                           tolerance. This parameter is used only if
                           theIsUseC1 is set to True.

            Returns:
                TRUE, if the given shape is a compound of blocks.
                If theCompound is not valid, prints all discovered errors.
            """
            # Example: see GEOM_Spanner.py
            aTolerance = -1.0
            if theIsUseC1:
                aTolerance = theAngTolerance
            (IsValid, BCErrors) = self.BlocksOp.CheckCompoundOfBlocks(theCompound, aTolerance)
            RaiseIfFailed("CheckCompoundOfBlocks", self.BlocksOp)
            if IsValid == 0:
                Descr = self.BlocksOp.PrintBCErrors(theCompound, BCErrors)
                print Descr
            return IsValid

        ## Retrieve all non blocks solids and faces from \a theShape.
        #  @param theShape The shape to explore.
        #  @param theIsUseC1 Flag to check if there are 4 bounds on a face
        #         taking into account C1 continuity.
        #  @param theAngTolerance the angular tolerance to check if two neighbor
        #         edges are codirectional in the common vertex with this
        #         tolerance. This parameter is used only if
        #         <VAR>theIsUseC1</VAR> is set to True.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return A tuple of two GEOM_Objects. The first object is a group of all
        #          non block solids (= not 6 faces, or with 6 faces, but with the
        #          presence of non-quadrangular faces). The second object is a
        #          group of all non quadrangular faces (= faces with more then
        #          1 wire or, if <VAR>theIsUseC1</VAR> is set to True, faces
        #          with 1 wire with not 4 edges that do not form 4 bounds of
        #          C1 continuity).
        #
        #  @ref tui_get_non_blocks_page "Example 1"
        #  \n @ref swig_GetNonBlocks "Example 2"
        @ManageTransactions("BlocksOp")
        def GetNonBlocks (self, theShape, theIsUseC1 = False,
                          theAngTolerance = 1.e-12, theName=None):
            """
            Retrieve all non blocks solids and faces from theShape.

            Parameters:
                theShape The shape to explore.
                theIsUseC1 Flag to check if there are 4 bounds on a face
                           taking into account C1 continuity.
                theAngTolerance the angular tolerance to check if two neighbor
                           edges are codirectional in the common vertex with this
                           tolerance. This parameter is used only if
                           theIsUseC1 is set to True.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                A tuple of two GEOM_Objects. The first object is a group of all
                non block solids (= not 6 faces, or with 6 faces, but with the
                presence of non-quadrangular faces). The second object is a
                group of all non quadrangular faces (= faces with more then
                1 wire or, if <VAR>theIsUseC1</VAR> is set to True, faces
                with 1 wire with not 4 edges that do not form 4 bounds of
                C1 continuity).

            Usage:
                (res_sols, res_faces) = geompy.GetNonBlocks(myShape1)
            """
            # Example: see GEOM_Spanner.py
            aTolerance = -1.0
            if theIsUseC1:
                aTolerance = theAngTolerance
            aTuple = self.BlocksOp.GetNonBlocks(theShape, aTolerance)
            RaiseIfFailed("GetNonBlocks", self.BlocksOp)
            self._autoPublish(aTuple, theName, ("groupNonHexas", "groupNonQuads"))
            return aTuple

        ## Remove all seam and degenerated edges from \a theShape.
        #  Unite faces and edges, sharing one surface. It means that
        #  this faces must have references to one C++ surface object (handle).
        #  @param theShape The compound or single solid to remove irregular edges from.
        #  @param doUnionFaces If True, then unite faces. If False (the default value),
        #         do not unite faces.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return Improved shape.
        #
        #  @ref swig_RemoveExtraEdges "Example"
        @ManageTransactions("BlocksOp")
        def RemoveExtraEdges(self, theShape, doUnionFaces=False, theName=None):
            """
            Remove all seam and degenerated edges from theShape.
            Unite faces and edges, sharing one surface. It means that
            this faces must have references to one C++ surface object (handle).

            Parameters:
                theShape The compound or single solid to remove irregular edges from.
                doUnionFaces If True, then unite faces. If False (the default value),
                             do not unite faces.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                Improved shape.
            """
            # Example: see GEOM_TestOthers.py
            nbFacesOptimum = -1 # -1 means do not unite faces
            if doUnionFaces is True: nbFacesOptimum = 0 # 0 means unite faces
            anObj = self.BlocksOp.RemoveExtraEdges(theShape, nbFacesOptimum)
            RaiseIfFailed("RemoveExtraEdges", self.BlocksOp)
            self._autoPublish(anObj, theName, "removeExtraEdges")
            return anObj

        ## Performs union faces of \a theShape
        #  Unite faces sharing one surface. It means that
        #  these faces must have references to one C++ surface object (handle).
        #  @param theShape The compound or single solid that contains faces
        #         to perform union.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return Improved shape.
        #
        #  @ref swig_UnionFaces "Example"
        @ManageTransactions("BlocksOp")
        def UnionFaces(self, theShape, theName=None):
            """
            Performs union faces of theShape.
            Unite faces sharing one surface. It means that
            these faces must have references to one C++ surface object (handle).

            Parameters:
                theShape The compound or single solid that contains faces
                         to perform union.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                Improved shape.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.UnionFaces(theShape)
            RaiseIfFailed("UnionFaces", self.BlocksOp)
            self._autoPublish(anObj, theName, "unionFaces")
            return anObj

        ## Check, if the given shape is a blocks compound.
        #  Fix all detected errors.
        #    \note Single block can be also fixed by this method.
        #  @param theShape The compound to check and improve.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return Improved compound.
        #
        #  @ref swig_CheckAndImprove "Example"
        @ManageTransactions("BlocksOp")
        def CheckAndImprove(self, theShape, theName=None):
            """
            Check, if the given shape is a blocks compound.
            Fix all detected errors.

            Note:
                Single block can be also fixed by this method.

            Parameters:
                theShape The compound to check and improve.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                Improved compound.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.CheckAndImprove(theShape)
            RaiseIfFailed("CheckAndImprove", self.BlocksOp)
            self._autoPublish(anObj, theName, "improved")
            return anObj

        # end of l4_blocks_measure
        ## @}

        ## @addtogroup l3_blocks_op
        ## @{

        ## Get all the blocks, contained in the given compound.
        #  @param theCompound The compound to explode.
        #  @param theMinNbFaces If solid has lower number of faces, it is not a block.
        #  @param theMaxNbFaces If solid has higher number of faces, it is not a block.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note If theMaxNbFaces = 0, the maximum number of faces is not restricted.
        #
        #  @return List of GEOM.GEOM_Object, containing the retrieved blocks.
        #
        #  @ref tui_explode_on_blocks "Example 1"
        #  \n @ref swig_MakeBlockExplode "Example 2"
        @ManageTransactions("BlocksOp")
        def MakeBlockExplode(self, theCompound, theMinNbFaces, theMaxNbFaces, theName=None):
            """
            Get all the blocks, contained in the given compound.

            Parameters:
                theCompound The compound to explode.
                theMinNbFaces If solid has lower number of faces, it is not a block.
                theMaxNbFaces If solid has higher number of faces, it is not a block.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(aList, theName, "block")
            return aList

        ## Find block, containing the given point inside its volume or on boundary.
        #  @param theCompound Compound, to find block in.
        #  @param thePoint Point, close to the desired block. If the point lays on
        #         boundary between some blocks, we return block with nearest center.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the found block.
        #
        #  @ref swig_todo "Example"
        @ManageTransactions("BlocksOp")
        def GetBlockNearPoint(self, theCompound, thePoint, theName=None):
            """
            Find block, containing the given point inside its volume or on boundary.

            Parameters:
                theCompound Compound, to find block in.
                thePoint Point, close to the desired block. If the point lays on
                         boundary between some blocks, we return block with nearest center.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM.GEOM_Object, containing the found block.
            """
            # Example: see GEOM_Spanner.py
            anObj = self.BlocksOp.GetBlockNearPoint(theCompound, thePoint)
            RaiseIfFailed("GetBlockNearPoint", self.BlocksOp)
            self._autoPublish(anObj, theName, "block")
            return anObj

        ## Find block, containing all the elements, passed as the parts, or maximum quantity of them.
        #  @param theCompound Compound, to find block in.
        #  @param theParts List of faces and/or edges and/or vertices to be parts of the found block.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the found block.
        #
        #  @ref swig_GetBlockByParts "Example"
        @ManageTransactions("BlocksOp")
        def GetBlockByParts(self, theCompound, theParts, theName=None):
            """
             Find block, containing all the elements, passed as the parts, or maximum quantity of them.

             Parameters:
                theCompound Compound, to find block in.
                theParts List of faces and/or edges and/or vertices to be parts of the found block.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM_Object, containing the found block.
            """
            # Example: see GEOM_TestOthers.py
            anObj = self.BlocksOp.GetBlockByParts(theCompound, theParts)
            RaiseIfFailed("GetBlockByParts", self.BlocksOp)
            self._autoPublish(anObj, theName, "block")
            return anObj

        ## Return all blocks, containing all the elements, passed as the parts.
        #  @param theCompound Compound, to find blocks in.
        #  @param theParts List of faces and/or edges and/or vertices to be parts of the found blocks.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of GEOM.GEOM_Object, containing the found blocks.
        #
        #  @ref swig_todo "Example"
        @ManageTransactions("BlocksOp")
        def GetBlocksByParts(self, theCompound, theParts, theName=None):
            """
            Return all blocks, containing all the elements, passed as the parts.

            Parameters:
                theCompound Compound, to find blocks in.
                theParts List of faces and/or edges and/or vertices to be parts of the found blocks.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of GEOM.GEOM_Object, containing the found blocks.
            """
            # Example: see GEOM_Spanner.py
            aList = self.BlocksOp.GetBlocksByParts(theCompound, theParts)
            RaiseIfFailed("GetBlocksByParts", self.BlocksOp)
            self._autoPublish(aList, theName, "block")
            return aList

        ## Multi-transformate block and glue the result.
        #  Transformation is defined so, as to superpose direction faces.
        #  @param Block Hexahedral solid to be multi-transformed.
        #  @param DirFace1 ID of First direction face.
        #  @param DirFace2 ID of Second direction face.
        #  @param NbTimes Quantity of transformations to be done.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @note Unique ID of sub-shape can be obtained, using method GetSubShapeID().
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_multi_transformation "Example"
        @ManageTransactions("BlocksOp")
        def MakeMultiTransformation1D(self, Block, DirFace1, DirFace2, NbTimes, theName=None):
            """
            Multi-transformate block and glue the result.
            Transformation is defined so, as to superpose direction faces.

            Parameters:
                Block Hexahedral solid to be multi-transformed.
                DirFace1 ID of First direction face.
                DirFace2 ID of Second direction face.
                NbTimes Quantity of transformations to be done.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "transformed")
            return anObj

        ## Multi-transformate block and glue the result.
        #  @param Block Hexahedral solid to be multi-transformed.
        #  @param DirFace1U,DirFace2U IDs of Direction faces for the first transformation.
        #  @param DirFace1V,DirFace2V IDs of Direction faces for the second transformation.
        #  @param NbTimesU,NbTimesV Quantity of transformations to be done.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM.GEOM_Object, containing the result shape.
        #
        #  @ref tui_multi_transformation "Example"
        @ManageTransactions("BlocksOp")
        def MakeMultiTransformation2D(self, Block, DirFace1U, DirFace2U, NbTimesU,
                                      DirFace1V, DirFace2V, NbTimesV, theName=None):
            """
            Multi-transformate block and glue the result.

            Parameters:
                Block Hexahedral solid to be multi-transformed.
                DirFace1U,DirFace2U IDs of Direction faces for the first transformation.
                DirFace1V,DirFace2V IDs of Direction faces for the second transformation.
                NbTimesU,NbTimesV Quantity of transformations to be done.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

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
            self._autoPublish(anObj, theName, "transformed")
            return anObj

        ## Build all possible propagation groups.
        #  Propagation group is a set of all edges, opposite to one (main)
        #  edge of this group directly or through other opposite edges.
        #  Notion of Opposite Edge make sence only on quadrangle face.
        #  @param theShape Shape to build propagation groups on.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return List of GEOM.GEOM_Object, each of them is a propagation group.
        #
        #  @ref swig_Propagate "Example"
        @ManageTransactions("BlocksOp")
        def Propagate(self, theShape, theName=None):
            """
            Build all possible propagation groups.
            Propagation group is a set of all edges, opposite to one (main)
            edge of this group directly or through other opposite edges.
            Notion of Opposite Edge make sence only on quadrangle face.

            Parameters:
                theShape Shape to build propagation groups on.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                List of GEOM.GEOM_Object, each of them is a propagation group.
            """
            # Example: see GEOM_TestOthers.py
            listChains = self.BlocksOp.Propagate(theShape)
            RaiseIfFailed("Propagate", self.BlocksOp)
            self._autoPublish(listChains, theName, "propagate")
            return listChains

        # end of l3_blocks_op
        ## @}

        ## @addtogroup l3_groups
        ## @{

        ## Creates a new group which will store sub-shapes of theMainShape
        #  @param theMainShape is a GEOM object on which the group is selected
        #  @param theShapeType defines a shape type of the group (see GEOM::shape_type)
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return a newly created GEOM group (GEOM.GEOM_Object)
        #
        #  @ref tui_working_with_groups_page "Example 1"
        #  \n @ref swig_CreateGroup "Example 2"
        @ManageTransactions("GroupOp")
        def CreateGroup(self, theMainShape, theShapeType, theName=None):
            """
            Creates a new group which will store sub-shapes of theMainShape

            Parameters:
               theMainShape is a GEOM object on which the group is selected
               theShapeType defines a shape type of the group:"COMPOUND", "COMPSOLID",
                            "SOLID", "SHELL", "FACE", "WIRE", "EDGE", "VERTEX", "SHAPE".
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
               a newly created GEOM group

            Example of usage:
                group = geompy.CreateGroup(Box, geompy.ShapeType["FACE"])

            """
            # Example: see GEOM_TestOthers.py
            anObj = self.GroupOp.CreateGroup(theMainShape, theShapeType)
            RaiseIfFailed("CreateGroup", self.GroupOp)
            self._autoPublish(anObj, theName, "group")
            return anObj

        ## Adds a sub-object with ID theSubShapeId to the group
        #  @param theGroup is a GEOM group to which the new sub-shape is added
        #  @param theSubShapeID is a sub-shape ID in the main object.
        #  \note Use method GetSubShapeID() to get an unique ID of the sub-shape
        #
        #  @ref tui_working_with_groups_page "Example"
        @ManageTransactions("GroupOp")
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
        @ManageTransactions("GroupOp")
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
        @ManageTransactions("GroupOp")
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
        @ManageTransactions("GroupOp")
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
        @ManageTransactions("GroupOp")
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
        @ManageTransactions("GroupOp")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return a newly created GEOM group.
        #
        #  @ref tui_union_groups_anchor "Example"
        @ManageTransactions("GroupOp")
        def UnionGroups (self, theGroup1, theGroup2, theName=None):
            """
            Union of two groups.
            New group is created. It will contain all entities
            which are present in groups theGroup1 and theGroup2.

            Parameters:
                theGroup1, theGroup2 are the initial GEOM groups
                                     to create the united group from.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.UnionGroups(theGroup1, theGroup2)
            RaiseIfFailed("UnionGroups", self.GroupOp)
            self._autoPublish(aGroup, theName, "group")
            return aGroup

        ## Intersection of two groups.
        #  New group is created. It will contain only those entities
        #  which are present in both groups theGroup1 and theGroup2.
        #  @param theGroup1, theGroup2 are the initial GEOM groups to get common part of.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return a newly created GEOM group.
        #
        #  @ref tui_intersect_groups_anchor "Example"
        @ManageTransactions("GroupOp")
        def IntersectGroups (self, theGroup1, theGroup2, theName=None):
            """
            Intersection of two groups.
            New group is created. It will contain only those entities
            which are present in both groups theGroup1 and theGroup2.

            Parameters:
                theGroup1, theGroup2 are the initial GEOM groups to get common part of.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.IntersectGroups(theGroup1, theGroup2)
            RaiseIfFailed("IntersectGroups", self.GroupOp)
            self._autoPublish(aGroup, theName, "group")
            return aGroup

        ## Cut of two groups.
        #  New group is created. It will contain entities which are
        #  present in group theGroup1 but are not present in group theGroup2.
        #  @param theGroup1 is a GEOM group to include elements of.
        #  @param theGroup2 is a GEOM group to exclude elements of.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return a newly created GEOM group.
        #
        #  @ref tui_cut_groups_anchor "Example"
        @ManageTransactions("GroupOp")
        def CutGroups (self, theGroup1, theGroup2, theName=None):
            """
            Cut of two groups.
            New group is created. It will contain entities which are
            present in group theGroup1 but are not present in group theGroup2.

            Parameters:
                theGroup1 is a GEOM group to include elements of.
                theGroup2 is a GEOM group to exclude elements of.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.CutGroups(theGroup1, theGroup2)
            RaiseIfFailed("CutGroups", self.GroupOp)
            self._autoPublish(aGroup, theName, "group")
            return aGroup

        ## Union of list of groups.
        #  New group is created. It will contain all entities that are
        #  present in groups listed in theGList.
        #  @param theGList is a list of GEOM groups to create the united group from.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return a newly created GEOM group.
        #
        #  @ref tui_union_groups_anchor "Example"
        @ManageTransactions("GroupOp")
        def UnionListOfGroups (self, theGList, theName=None):
            """
            Union of list of groups.
            New group is created. It will contain all entities that are
            present in groups listed in theGList.

            Parameters:
                theGList is a list of GEOM groups to create the united group from.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.UnionListOfGroups(theGList)
            RaiseIfFailed("UnionListOfGroups", self.GroupOp)
            self._autoPublish(aGroup, theName, "group")
            return aGroup

        ## Cut of lists of groups.
        #  New group is created. It will contain only entities
        #  which are present in groups listed in theGList.
        #  @param theGList is a list of GEOM groups to include elements of.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return a newly created GEOM group.
        #
        #  @ref tui_intersect_groups_anchor "Example"
        @ManageTransactions("GroupOp")
        def IntersectListOfGroups (self, theGList, theName=None):
            """
            Cut of lists of groups.
            New group is created. It will contain only entities
            which are present in groups listed in theGList.

            Parameters:
                theGList is a list of GEOM groups to include elements of.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.IntersectListOfGroups(theGList)
            RaiseIfFailed("IntersectListOfGroups", self.GroupOp)
            self._autoPublish(aGroup, theName, "group")
            return aGroup

        ## Cut of lists of groups.
        #  New group is created. It will contain only entities
        #  which are present in groups listed in theGList1 but
        #  are not present in groups from theGList2.
        #  @param theGList1 is a list of GEOM groups to include elements of.
        #  @param theGList2 is a list of GEOM groups to exclude elements of.
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return a newly created GEOM group.
        #
        #  @ref tui_cut_groups_anchor "Example"
        @ManageTransactions("GroupOp")
        def CutListOfGroups (self, theGList1, theGList2, theName=None):
            """
            Cut of lists of groups.
            New group is created. It will contain only entities
            which are present in groups listed in theGList1 but
            are not present in groups from theGList2.

            Parameters:
                theGList1 is a list of GEOM groups to include elements of.
                theGList2 is a list of GEOM groups to exclude elements of.
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                a newly created GEOM group.
            """
            # Example: see GEOM_TestOthers.py
            aGroup = self.GroupOp.CutListOfGroups(theGList1, theGList2)
            RaiseIfFailed("CutListOfGroups", self.GroupOp)
            self._autoPublish(aGroup, theName, "group")
            return aGroup

        ## Returns a list of sub-objects ID stored in the group
        #  @param theGroup is a GEOM group for which a list of IDs is requested
        #
        #  @ref swig_GetObjectIDs "Example"
        @ManageTransactions("GroupOp")
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
        @ManageTransactions("GroupOp")
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
        @ManageTransactions("GroupOp")
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
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return a newly created GEOM group of edges
        #
        #  @@ref swig_todo "Example"
        def GetEdgesByLength (self, theShape, min_length, max_length, include_min = 1, include_max = 1, theName=None):
            """
            Create group of edges of theShape, whose length is in range [min_length, max_length].
            If include_min/max == 0, edges with length == min/max_length will not be included in result.

            Parameters:
                theShape given shape
                min_length minimum length of edges of theShape
                max_length maximum length of edges of theShape
                include_max indicating if edges with length == max_length should be included in result, 1-yes, 0-no (default=1)
                include_min indicating if edges with length == min_length should be included in result, 1-yes, 0-no (default=1)
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

             Returns:
                a newly created GEOM group of edges.
            """
            edges = self.SubShapeAll(theShape, self.ShapeType["EDGE"])
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
                return None

            # note: auto-publishing is done in self.CreateGroup()
            group_edges = self.CreateGroup(theShape, self.ShapeType["EDGE"], theName)
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

            sg.updateObjBrowser(True)

            return group_edges

        # end of l3_groups
        ## @}

        #@@ insert new functions before this line @@ do not remove this line @@#

        ## Create a copy of the given object
        #
        #  @param theOriginal geometry object for copy
        #  @param theName Object name; when specified, this parameter is used
        #         for result publication in the study. Otherwise, if automatic
        #         publication is switched on, default value is used for result name.
        #
        #  @return New GEOM_Object, containing the copied shape.
        #
        #  @ingroup l1_geomBuilder_auxiliary
        #  @ref swig_MakeCopy "Example"
        @ManageTransactions("InsertOp")
        def MakeCopy(self, theOriginal, theName=None):
            """
            Create a copy of the given object

            Parameters:
                theOriginal geometry object for copy
                theName Object name; when specified, this parameter is used
                        for result publication in the study. Otherwise, if automatic
                        publication is switched on, default value is used for result name.

            Returns:
                New GEOM_Object, containing the copied shape.

            Example of usage: Copy = geompy.MakeCopy(Box)
            """
            # Example: see GEOM_TestAll.py
            anObj = self.InsertOp.MakeCopy(theOriginal)
            RaiseIfFailed("MakeCopy", self.InsertOp)
            self._autoPublish(anObj, theName, "copy")
            return anObj

        ## Add Path to load python scripts from
        #  @param Path a path to load python scripts from
        #  @ingroup l1_geomBuilder_auxiliary
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
        #  @ingroup l1_geomBuilder_auxiliary
        @ManageTransactions("InsertOp")
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
        #  @ingroup l1_geomBuilder_auxiliary
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
        #  @ingroup l1_geomBuilder_auxiliary
        @ManageTransactions("InsertOp")
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

        ## Transfer not topological data from one GEOM object to another.
        #
        #  @param theObjectFrom the source object of non-topological data
        #  @param theObjectTo the destination object of non-topological data
        #  @param theFindMethod method to search sub-shapes of theObjectFrom
        #         in shape theObjectTo. Possible values are: GEOM.FSM_GetInPlace,
        #         GEOM.FSM_GetInPlaceByHistory and GEOM.FSM_GetInPlace_Old.
        #         Other values of GEOM.find_shape_method are not supported.
        #
        #  @return True in case of success; False otherwise.
        #
        #  @ingroup l1_geomBuilder_auxiliary
        #
        #  @ref swig_TransferData "Example"
        @ManageTransactions("InsertOp")
        def TransferData(self, theObjectFrom, theObjectTo,
                         theFindMethod=GEOM.FSM_GetInPlace):
            """
            Transfer not topological data from one GEOM object to another.

            Parameters:
                theObjectFrom the source object of non-topological data
                theObjectTo the destination object of non-topological data
                theFindMethod method to search sub-shapes of theObjectFrom
                              in shape theObjectTo. Possible values are:
                              GEOM.FSM_GetInPlace, GEOM.FSM_GetInPlaceByHistory
                              and GEOM.FSM_GetInPlace_Old. Other values of
                              GEOM.find_shape_method are not supported.

            Returns:
                True in case of success; False otherwise.

            # Example: see GEOM_TestOthers.py
            """
            # Example: see GEOM_TestAll.py
            isOk = self.InsertOp.TransferData(theObjectFrom,
                                               theObjectTo, theFindMethod)
            RaiseIfFailed("TransferData", self.InsertOp)
            return isOk

        ## Creates a new folder object. It is a container for any GEOM objects.
        #  @param Name name of the container
        #  @param Father parent object. If None,
        #         folder under 'Geometry' root object will be created.
        #  @return a new created folder
        #  @ingroup l1_publish_data
        def NewFolder(self, Name, Father=None):
            """
            Create a new folder object. It is an auxiliary container for any GEOM objects.

            Parameters:
                Name name of the container
                Father parent object. If None,
                folder under 'Geometry' root object will be created.

            Returns:
                a new created folder
            """
            if not Father: Father = self.father
            return self.CreateFolder(Name, Father)

        ## Move object to the specified folder
        #  @param Object object to move
        #  @param Folder target folder
        #  @ingroup l1_publish_data
        def PutToFolder(self, Object, Folder):
            """
            Move object to the specified folder

            Parameters:
                Object object to move
                Folder target folder
            """
            self.MoveToFolder(Object, Folder)
            pass

        ## Move list of objects to the specified folder
        #  @param ListOfSO list of objects to move
        #  @param Folder target folder
        #  @ingroup l1_publish_data
        def PutListToFolder(self, ListOfSO, Folder):
            """
            Move list of objects to the specified folder

            Parameters:
                ListOfSO list of objects to move
                Folder target folder
            """
            self.MoveListToFolder(ListOfSO, Folder)
            pass

        ## @addtogroup l2_field
        ## @{

        ## Creates a field
        #  @param shape the shape the field lies on
        #  @param name the field name
        #  @param type type of field data: 0 - bool, 1 - int, 2 - double, 3 - string
        #  @param dimension dimension of the shape the field lies on
        #         0 - VERTEX, 1 - EDGE, 2 - FACE, 3 - SOLID, -1 - whole shape
        #  @param componentNames names of components
        #  @return a created field
        @ManageTransactions("FieldOp")
        def CreateField(self, shape, name, type, dimension, componentNames):
            """
            Creates a field

            Parameters:
                shape the shape the field lies on
                name  the field name
                type  type of field data
                dimension dimension of the shape the field lies on
                          0 - VERTEX, 1 - EDGE, 2 - FACE, 3 - SOLID, -1 - whole shape
                componentNames names of components

            Returns:
                a created field
            """
            if isinstance( type, int ):
                if type < 0 or type > 3:
                    raise RuntimeError, "CreateField : Error: data type must be within [0-3] range"
                type = [GEOM.FDT_Bool,GEOM.FDT_Int,GEOM.FDT_Double,GEOM.FDT_String][type]

            f = self.FieldOp.CreateField( shape, name, type, dimension, componentNames)
            RaiseIfFailed("CreateField", self.FieldOp)
            global geom
            geom._autoPublish( f, "", name)
            return f

        ## Removes a field from the GEOM component
        #  @param field the field to remove
        def RemoveField(self, field):
            "Removes a field from the GEOM component"
            global geom
            if isinstance( field, GEOM._objref_GEOM_Field ):
                geom.RemoveObject( field )
            elif isinstance( field, geomField ):
                geom.RemoveObject( field.field )
            else:
                raise RuntimeError, "RemoveField() : the object is not a field"
            return

        ## Returns number of fields on a shape
        @ManageTransactions("FieldOp")
        def CountFields(self, shape):
            "Returns number of fields on a shape"
            nb = self.FieldOp.CountFields( shape )
            RaiseIfFailed("CountFields", self.FieldOp)
            return nb

        ## Returns all fields on a shape
        @ManageTransactions("FieldOp")
        def GetFields(self, shape):
            "Returns all fields on a shape"
            ff = self.FieldOp.GetFields( shape )
            RaiseIfFailed("GetFields", self.FieldOp)
            return ff

        ## Returns a field on a shape by its name
        @ManageTransactions("FieldOp")
        def GetField(self, shape, name):
            "Returns a field on a shape by its name"
            f = self.FieldOp.GetField( shape, name )
            RaiseIfFailed("GetField", self.FieldOp)
            return f

        # end of l2_field
        ## @}


import omniORB
# Register the new proxy for GEOM_Gen
omniORB.registerObjref(GEOM._objref_GEOM_Gen._NP_RepositoryId, geomBuilder)


## Field on Geometry
#  @ingroup l2_field
class geomField( GEOM._objref_GEOM_Field ):

    def __init__(self):
        GEOM._objref_GEOM_Field.__init__(self)
        self.field = GEOM._objref_GEOM_Field
        return

    ## Returns the shape the field lies on
    def getShape(self):
        "Returns the shape the field lies on"
        return self.field.GetShape(self)

    ## Returns the field name
    def getName(self):
        "Returns the field name"
        return self.field.GetName(self)

    ## Returns type of field data as integer [0-3]
    def getType(self):
        "Returns type of field data"
        return self.field.GetDataType(self)._v

    ## Returns type of field data:
    #  one of GEOM.FDT_Bool, GEOM.FDT_Int, GEOM.FDT_Double, GEOM.FDT_String
    def getTypeEnum(self):
        "Returns type of field data"
        return self.field.GetDataType(self)

    ## Returns dimension of the shape the field lies on:
    #  0 - VERTEX, 1 - EDGE, 2 - FACE, 3 - SOLID, -1 - whole shape
    def getDimension(self):
        """Returns dimension of the shape the field lies on:
        0 - VERTEX, 1 - EDGE, 2 - FACE, 3 - SOLID, -1 - whole shape"""
        return self.field.GetDimension(self)

    ## Returns names of components
    def getComponents(self):
        "Returns names of components"
        return self.field.GetComponents(self)

    ## Adds a time step to the field
    #  @param step the time step number further used as the step identifier
    #  @param stamp the time step time
    #  @param values the values of the time step
    def addStep(self, step, stamp, values):
        "Adds a time step to the field"
        stp = self.field.AddStep( self, step, stamp )
        if not stp:
            raise RuntimeError, \
                  "Field.addStep() : Error: step %s already exists in this field"%step
        global geom
        geom._autoPublish( stp, "", "Step %s, %s"%(step,stamp))
        self.setValues( step, values )
        return stp

    ## Remove a time step from the field
    def removeStep(self,step):
        "Remove a time step from the field"
        stepSO = None
        try:
            stepObj = self.field.GetStep( self, step )
            if stepObj:
                stepSO = geom.myStudy.FindObjectID( stepObj.GetStudyEntry() )
        except:
            #import traceback
            #traceback.print_exc()
            pass
        self.field.RemoveStep( self, step )
        if stepSO:
            geom.myBuilder.RemoveObjectWithChildren( stepSO )
        return

    ## Returns number of time steps in the field
    def countSteps(self):
        "Returns number of time steps in the field"
        return self.field.CountSteps(self)

    ## Returns a list of time step IDs in the field
    def getSteps(self):
        "Returns a list of time step IDs in the field"
        return self.field.GetSteps(self)

    ## Returns a time step by its ID
    def getStep(self,step):
        "Returns a time step by its ID"
        stp = self.field.GetStep(self, step)
        if not stp:
            raise RuntimeError, "Step %s is missing from this field"%step
        return stp

    ## Returns the time of the field step
    def getStamp(self,step):
        "Returns the time of the field step"
        return self.getStep(step).GetStamp()

    ## Changes the time of the field step
    def setStamp(self, step, stamp):
        "Changes the time of the field step"
        return self.getStep(step).SetStamp(stamp)

    ## Returns values of the field step
    def getValues(self, step):
        "Returns values of the field step"
        return self.getStep(step).GetValues()

    ## Changes values of the field step
    def setValues(self, step, values):
        "Changes values of the field step"
        stp = self.getStep(step)
        errBeg = "Field.setValues(values) : Error: "
        try:
            ok = stp.SetValues( values )
        except Exception, e:
            excStr = str(e)
            if excStr.find("WrongPythonType") > 0:
                raise RuntimeError, errBeg +\
                      "wrong type of values, %s values are expected"%str(self.getTypeEnum())[4:]
            raise RuntimeError, errBeg + str(e)
        if not ok:
            nbOK = self.field.GetArraySize(self)
            nbKO = len(values)
            if nbOK != nbKO:
                raise RuntimeError, errBeg + "len(values) must be %s but not %s"%(nbOK,nbKO)
            else:
                raise RuntimeError, errBeg + "failed"
        return

    pass # end of class geomField

# Register the new proxy for GEOM_Field
omniORB.registerObjref(GEOM._objref_GEOM_Field._NP_RepositoryId, geomField)


## Create a new geomBuilder instance.The geomBuilder class provides the Python
#  interface to GEOM operations.
#
#  Typical use is:
#  \code
#    import salome
#    salome.salome_init()
#    from salome.geom import geomBuilder
#    geompy = geomBuilder.New(salome.myStudy)
#  \endcode
#  @param  study     SALOME study, generally obtained by salome.myStudy.
#  @param  instance  CORBA proxy of GEOM Engine. If None, the default Engine is used.
#  @return geomBuilder instance
def New( study, instance=None):
    """
    Create a new geomBuilder instance.The geomBuilder class provides the Python
    interface to GEOM operations.

    Typical use is:
        import salome
        salome.salome_init()
        from salome.geom import geomBuilder
        geompy = geomBuilder.New(salome.myStudy)

    Parameters:
        study     SALOME study, generally obtained by salome.myStudy.
        instance  CORBA proxy of GEOM Engine. If None, the default Engine is used.
    Returns:
        geomBuilder instance
    """
    #print "New geomBuilder ", study, instance
    global engine
    global geom
    global doLcc
    engine = instance
    if engine is None:
      doLcc = True
    geom = geomBuilder()
    assert isinstance(geom,geomBuilder), "Geom engine class is %s but should be geomBuilder.geomBuilder. Import geomBuilder before creating the instance."%geom.__class__
    geom.init_geom(study)
    return geom


# Register methods from the plug-ins in the geomBuilder class 
plugins_var = os.environ.get( "GEOM_PluginsList" )

plugins = None
if plugins_var is not None:
    plugins = plugins_var.split( ":" )
    plugins=filter(lambda x: len(x)>0, plugins)
if plugins is not None:
    for pluginName in plugins:
        pluginBuilderName = pluginName + "Builder"
        try:
            exec( "from salome.%s.%s import *" % (pluginName, pluginBuilderName))
        except Exception, e:
            from salome_utils import verbose
            print "Exception while loading %s: %s" % ( pluginBuilderName, e )
            continue
        exec( "from salome.%s import %s" % (pluginName, pluginBuilderName))
        plugin = eval( pluginBuilderName )
        
        # add methods from plugin module to the geomBuilder class
        for k in dir( plugin ):
            if k[0] == '_': continue
            method = getattr( plugin, k )
            if type( method ).__name__ == 'function':
                if not hasattr( geomBuilder, k ):
                    setattr( geomBuilder, k, method )
                pass
            pass
        del pluginName
        pass
    pass   
